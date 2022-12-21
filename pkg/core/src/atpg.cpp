// **************************************************************************
// File       [ atpg.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ This files include most of the method of class Atpg ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#include "atpg.h"

using namespace CoreNs;

// **************************************************************************
// Function   [ Atpg::generatePatternSet ]
// Commentor  [ CAL ]
// Synopsis   [ usage: the main function of atpg generate the complete test pattern set for faults
//              in:    PatternProcessor, FaultListExtract
//              out:   void //TODO not void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::generatePatternSet(PatternProcessor *pPatternProcessor, FaultListExtract *pFaultListExtracter)
{
	Fault *pCurrentFault = NULL;
	FaultPtrList originalFaultPtrListForPatternGeneration, reorderedFaultPtrListForPatternGeneration;
	setupCircuitParameter();
	pPatternProcessor->init(pCircuit_);
	// checkLevelInfo(); // for debug, not neccessary, removed by wang

	// setting faults for running ATPG
	for (Fault *pFault : pFaultListExtracter->faultsInCircuit_)
	{
		const bool faultIsQualified = (pFault->faultState_ != Fault::DT && pFault->faultState_ != Fault::RE && pFault->faultyLine_ >= 0);
		if (faultIsQualified)
		{
			originalFaultPtrListForPatternGeneration.push_back(pFault);
			reorderedFaultPtrListForPatternGeneration.push_back(pFault);
			// faultPtrListForStaticTestCompression.push_back(pFault); // save a copy for static test compression
		}
	}
	// faultPtrListForStaticTestCompression = originalFaultPtrListForPatternGeneration; removed to avoid peforming looping twice (copying is O(n))

	// To test clearAllFaultEffectBySimulation
	// testClearFaultEffect(originalFaultPtrListForPatternGeneration); // removed for now seems like debug usage, by wang

	// start ATPG, take one undetected fault from the reorderedFaultPtrListForPatternGeneration
	// if the fault is undetected, run ATPG on it
	const double faultPtrListSize = (double)(originalFaultPtrListForPatternGeneration.size());
	const int halfListSize = faultPtrListSize / 2.0;
	const int iterations = log2(faultPtrListSize) + 1;
	int minNumOfFaultsLeft = INFINITE;
	int numOfAtpgUntestableFaults = 0;
	std::vector<Pattern> bestTestPatternSet;
	for (int i = 0; i < iterations; ++i)
	{
		std::cerr << i << "th reorder\n";
		numOfAtpgUntestableFaults = 0;
		if (i != 0)
		{
			reorderedFaultPtrListForPatternGeneration.assign(originalFaultPtrListForPatternGeneration.begin(), originalFaultPtrListForPatternGeneration.end());
			FaultPtrListIter it = reorderedFaultPtrListForPatternGeneration.begin();
			for (int j = 0; j < halfListSize; ++j)
			{
				reorderedFaultPtrListForPatternGeneration.insert(it, reorderedFaultPtrListForPatternGeneration.back());
				reorderedFaultPtrListForPatternGeneration.pop_back();
				++it;
			}
			for (Fault *pFault : reorderedFaultPtrListForPatternGeneration)
			{
				pFault->detection_ = 0;
				pFault->faultState_ = Fault::UD;
			}
			originalFaultPtrListForPatternGeneration.assign(reorderedFaultPtrListForPatternGeneration.begin(), reorderedFaultPtrListForPatternGeneration.end());
		}
		pPatternProcessor->patternVector_.clear();
		// pPatternProcessor->patternVector_.reserve(MAX_LIST_SIZE);
		while (!reorderedFaultPtrListForPatternGeneration.empty())
		{
			// if the reorderedFaultPtrListForPatternGeneration is already left with aborted fault
			if (reorderedFaultPtrListForPatternGeneration.front()->faultState_ == Fault::AB)
			{
				break;
			}

			// the fault is not popped in previous call of StuckAtFaultATPGWithDTC()
			// => the fault is neither aborted or untestable => a pattern was found => detected fault
			if (pCurrentFault == reorderedFaultPtrListForPatternGeneration.front())
			{
				reorderedFaultPtrListForPatternGeneration.front()->faultState_ = Fault::DT;
				reorderedFaultPtrListForPatternGeneration.pop_front();
				continue;
			}
			else
			{
				pCurrentFault = reorderedFaultPtrListForPatternGeneration.front();
			}

			const bool isTransitionDelayFault = (pCurrentFault->faultType_ == Fault::STR || pCurrentFault->faultType_ == Fault::STF);
			if (isTransitionDelayFault)
			{
				TransitionDelayFaultATPG(reorderedFaultPtrListForPatternGeneration, pPatternProcessor, numOfAtpgUntestableFaults);
			}
			else
			{
				StuckAtFaultATPGWithDTC(reorderedFaultPtrListForPatternGeneration, pPatternProcessor, numOfAtpgUntestableFaults);
			}
		}
		int currNumOfFaultsLeft = numOfAtpgUntestableFaults;
		for (Fault *pFault : reorderedFaultPtrListForPatternGeneration)
		{
			currNumOfFaultsLeft += pFault->equivalent_;
		}
		if (i == 0)
		{
			bestTestPatternSet = pPatternProcessor->patternVector_;
			minNumOfFaultsLeft = currNumOfFaultsLeft;
			std::cerr << "Undetected Fault Count: " << currNumOfFaultsLeft << "\n";
			std::cerr << "Test Length: " << bestTestPatternSet.size() << "\n";
		}
		else if (currNumOfFaultsLeft < minNumOfFaultsLeft)
		{
			bestTestPatternSet = pPatternProcessor->patternVector_;
			minNumOfFaultsLeft = currNumOfFaultsLeft;
			std::cerr << "Undetected Fault Count: " << currNumOfFaultsLeft << "\n";
		}
		else if (currNumOfFaultsLeft == minNumOfFaultsLeft)
		{
			if (pPatternProcessor->patternVector_.size() < bestTestPatternSet.size())
			{
				bestTestPatternSet = pPatternProcessor->patternVector_;
				std::cerr << "Test Length: " << bestTestPatternSet.size() << "\n";
			}
		}
	}
	pPatternProcessor->patternVector_ = bestTestPatternSet;
	if (pPatternProcessor->staticCompression_ == PatternProcessor::ON)
	{
		staticTestCompressionByReverseFaultSimulation(pPatternProcessor, originalFaultPtrListForPatternGeneration);
	}

	// if (pPatternProcessor->XFill_ == PatternProcessor::ON) XFill(pPatternProcessor);, unecessary, alread Xfilled in previsous functions, removed by wang
}

// **************************************************************************
// Function   [ Atpg::setupCircuitParameter ]
// Commentor  [ KOREAL ]
// Synopsis   [ usage: initialize gate's data including
//					    numOfZero = {0} numOfOne  = {0} // unneccesary by wang
//							gateID_to_valModified_ // initialized in calGateDepthFromPO()
//						circuitLevel_to_EventStack_
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/10 ]
// **************************************************************************
void Atpg::setupCircuitParameter()
{
	// vector is 0 at the start and can be initialized in constructor hence, removed by wang
	// for (int i = 0; i < pCircuit_->tgate_; ++i)
	// {
	// 	gateID_to_n0_[i] = 0;          // unneccesary
	// 	gateID_to_n1_[i] = 0;          // unneccesary
	// 	gateID_to_valModified_[i] = 0; // moved to calGateDepthFromPO() by wang
	// }

	// set depthFromPo_
	calGateDepthFromPO();

	// Determine the lineType of a gate is FREE_LINE, BOUND_LINE or HEAD_LINE.
	identifyLineParameter();

	// see identifyDominator()
	identifyDominator();

	// see identifyUniquePath()
	identifyUniquePath();
}

// **************************************************************************
// Function   [ Atpg::calGateDepthFromPO ]
// Commentor  [ CAL ]
// Synopsis   [ usage: Calculate the depthFromPo_ of each gate.
//              notes:
//                If there is no path from a gate to PO/PPO,
//                set its depthFromPo_ as pCircuit_->tlvl_ + 100
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/06    last modified 2020/07/06 ]
// **************************************************************************
void Atpg::calGateDepthFromPO()
{
	// debug code removed by wang
	// for (int gateD = 0; gateD < pCircuit_->tgate_; ++gateD)
	// {
	// 	Gate &gate = pCircuit_->gates_[gateD];
	// 	// default -1, check if it was changed or not.
	// 	if (gate.depthFromPo_ != -1)
	// 	{
	// 		std::cerr << "depthFromPo_ is not -1\n";
	// 		std::cin.get();
	// 	}
	// }

	for (int gateID = pCircuit_->tgate_ - 1; gateID >= 0; --gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		gateID_to_valModified_[gateID] = 0; // sneak the initialization assignment in here

		gate.depthFromPo_ = INFINITE;
		if ((gate.gateType_ == Gate::PO) || (gate.gateType_ == Gate::PPO))
		{
			gate.depthFromPo_ = 0;
		}
		else if (gate.numFO_ > 0)
		{
			for (const int &fanOutGateID : gate.fanoutVector_)
			{
				const Gate &fanOutGate = pCircuit_->gates_[fanOutGateID];
				if (fanOutGate.depthFromPo_ < gate.depthFromPo_)
				{
					gate.depthFromPo_ = fanOutGate.depthFromPo_ + 1;
				}
			}
		}
		// else exist no path to output, so default assignment large number
	}
}

// **************************************************************************
// Function   [ Atpg::identifyLineParameter ]
// Commentor  [ CKY ]
// Synopsis   [ usage: fill in LineParameter(FREE or HEAD or BOUND)
//              in:    void //TODO
//              out:   void //TODO
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
void Atpg::identifyLineParameter()
{
	numberOfHeadLine_ = 0;

	for (const Gate &gate : pCircuit_->gates_)
	{
		const int &gateID = gate.gateId_;

		gateID_to_lineType_[gateID] = FREE_LINE; // initialize to freeline

		if (gate.gateType_ != Gate::PI && gate.gateType_ != Gate::PPI)
		{
			for (const int &fanInGateID : gate.faninVector_)
			{
				if (gateID_to_lineType_[fanInGateID] != FREE_LINE)
				{
					gateID_to_lineType_[gateID] = BOUND_LINE;
					break;
				}
			}
		}

		// check it is HEAD_LINE or not(rule 1)
		if ((gateID_to_lineType_[gateID] == FREE_LINE) && (gate.numFO_ != 1))
		{
			gateID_to_lineType_[gateID] = HEAD_LINE;
			++numberOfHeadLine_;
		}

		// check it is HEAD_LINE or not(rule 2)
		if (gateID_to_lineType_[gate.gateId_] == BOUND_LINE)
		{
			for (const int &fanInGateID : gate.faninVector_) // gate.numFI_  number of fanin
			{
				if (gateID_to_lineType_[fanInGateID] == FREE_LINE)
				{
					gateID_to_lineType_[fanInGateID] = HEAD_LINE;
					++numberOfHeadLine_;
				}
			}
		}
	}

	// store all head lines to array headLineGateIDs_
	headLineGateIDs_.reserve(numberOfHeadLine_);

	int count = 0;
	for (const Gate &gate : pCircuit_->gates_)
	{
		if (gateID_to_lineType_[gate.gateId_] == HEAD_LINE)
		{
			headLineGateIDs_.push_back(gate.gateId_);
			++count;
		}
		if (count == numberOfHeadLine_)
		{
			break;
		}
	}
}

// **************************************************************************
// Function   [ Atpg::identifyDominator ]
// Commentor  [ CAL ]
// Synopsis   [ usage: identify Dominator of the output gate
//                     for unique sensitization. After this function, each
//                     gate have one or zero Dominator in its gateID_to_uniquePath_ vector
//              in:    gate list
//              out:   void //TODO
//            ]
// Date       [ Ver. 1.0 started 2013/08/13   last modified 2020/07/09 ]
// **************************************************************************
void Atpg::identifyDominator()
{
	for (int i = pCircuit_->tgate_ - 1; i >= 0; --i)
	{
		Gate &gate = pCircuit_->gates_[i];
		if (gate.numFO_ <= 1) // if gate has only 1 or 0 output skip this gate
		{
			continue;
		}

		int gateCount = pushGateFanoutsToEventStack(i);
		for (int j = gate.numLevel_ + 1; j < pCircuit_->tlvl_; ++j)
		{
			// if next level's output isn't empty
			while (!circuitLevel_to_EventStack_[j].empty())
			{
				Gate &gDom = pCircuit_->gates_[circuitLevel_to_EventStack_[j].top()];
				circuitLevel_to_EventStack_[j].pop();
				gateID_to_valModified_[gDom.gateId_] = 0; // set the gDom to not handle
				// Because the gateCount is zero while the circuitLevel_to_EventStack_ is not zero, we
				// only need to pop without other operations. That is, continue operation.
				if (gateCount <= 0)
				{
					continue;
				}

				--gateCount;

				if (gateCount == 0)
				{
					if ((int)gateID_to_uniquePath_.capacity() < pCircuit_->tlvl_)
					{
						gateID_to_uniquePath_.reserve(pCircuit_->tlvl_);
					}
					// when all the fanout gate has been calculated
					gateID_to_uniquePath_[gate.gateId_].push_back(gDom.gateId_); // push the last calculate fanout gate into gateID_to_uniquePath_
					break;
				}
				// If there is a gate without gates on its output, then we suppose that this
				// gate is PO/PPO.  Because there are other gates still inside the
				// event queue (gateCount larger than 1), it means that gate gate does
				// not have any dominator.  Hence, we will set gateCount to zero as a
				// signal to clear the gates left in circuitLevel_to_EventStack_.  While the circuitLevel_to_EventStack_ is
				// not empty but gateCount is zero, we will continue.
				if (gDom.numFO_ == 0)
				{
					gateCount = 0;
				}
				else if (gDom.numFO_ > 1)
				{
					if (gateID_to_uniquePath_[gDom.gateId_].size() == 0)
					{
						gateCount = 0;
					}
					else
					{
						// Because the first gate in gateID_to_uniquePath_ is the closest Dominator, we just
						// push it to the circuitLevel_to_EventStack_. Then, we can skip the operation we have done
						// for gates with higher lvl_ than gate gate.
						Gate &gTmp = pCircuit_->gates_[gateID_to_uniquePath_[gDom.gateId_][0]];
						if (!gateID_to_valModified_[gTmp.gateId_])
						{
							circuitLevel_to_EventStack_[gTmp.numLevel_].push(gTmp.gateId_);
							gateID_to_valModified_[gTmp.gateId_] = 1;
							++gateCount;
						}
					}
				}
				else if (!gateID_to_valModified_[gDom.fanoutVector_[0]])
				{
					Gate &gTmp = pCircuit_->gates_[gDom.fanoutVector_[0]];
					circuitLevel_to_EventStack_[gTmp.numLevel_].push(gTmp.gateId_);
					gateID_to_valModified_[gTmp.gateId_] = 1;
					++gateCount;
				}
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::identifyUniquePath ]
// Commentor  [ CAL ]
// Synopsis   [ usage: compute the gateID_to_uniquePath_
//                In unique path sensitizatoin phase, we will need to know
//                if the inputs of a gate is fault reachable. Then, we can
//                prevent to assign non-controlling value to them.
//
//                We find the dominator, then we push_back the input gate
//                which is fault reachable from gate gate.
//
//                After identifyUniquePath, If a gate with dominator, then
//                the gateID_to_uniquePath_ vector of this gate will contains the
//                following gate id:
//
//                  [dominator_id   fault-reachable_input_gate_1_id   fault-reachable_input_gate_2_id  ......]
//
//              in:    gate list
//              out:   void //TODO
//            ]
// Date       [ Ver. 1.0 started 2013/08/13   last modified 2020/07/09 ]
// **************************************************************************
void Atpg::identifyUniquePath()
{
	static std::vector<int> reachableByDominator(pCircuit_->tgate_); // replace the original faultReach_ to avoid ambiguity, added by wang
	for (int i = pCircuit_->tgate_ - 1; i >= 0; --i)
	{
		Gate &gate = pCircuit_->gates_[i];
		// Because we will call identifyDominator before entering this function,
		// a gate with gateID_to_uniquePath_ will contain one Dominator.  Hence, we can
		// skip the gates while the sizes of their gateID_to_uniquePath_ is zero.
		if (gateID_to_uniquePath_[gate.gateId_].size() == 0)
		{
			continue;
		}

		reachableByDominator[gate.gateId_] = i;
		int count = pushGateFanoutsToEventStack(i);
		for (int j = gate.numLevel_ + 1; j < pCircuit_->tlvl_; ++j)
		{
			while (!circuitLevel_to_EventStack_[j].empty())
			{ // if fanout gate was not empty
				Gate &gTmp = pCircuit_->gates_[circuitLevel_to_EventStack_[j].top()];
				circuitLevel_to_EventStack_[j].pop();
				gateID_to_valModified_[gTmp.gateId_] = 0;
				reachableByDominator[gTmp.gateId_] = i;
				--count;
				if (count == 0)
				{
					for (int gReachID : gTmp.faninVector_)
					{
						if (reachableByDominator[gReachID] == i) // if it is UniquePath
						{
							// save gate to gateID_to_uniquePath_ list
							gateID_to_uniquePath_[gate.gateId_].push_back(gReachID);
						}
					}
					break;
				}
				count += pushGateFanoutsToEventStack(gTmp.gateId_);
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::TransitionDelayFaultATPG ]
// Commentor  [ HKY CYW ]
// Synopsis   [ usage: do transition delay fault model ATPG
//              in:    Pattern list, Fault list, int numOfAtpgUntestableFaults
//              out:   void//TODO
//            ]
// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
// **************************************************************************
void Atpg::TransitionDelayFaultATPG(FaultPtrList &faultListToGen, PatternProcessor *pPatternProcessor, int &numOfAtpgUntestableFaults)
{
	const Fault &fTDF = *faultListToGen.front();

	SINGLE_PATTERN_GENERATION_STATUS result = generateSinglePatternOnTargetFault(Fault(fTDF.gateID_ + pCircuit_->ngate_, fTDF.faultType_, fTDF.faultyLine_, fTDF.equivalent_, fTDF.faultState_), false);
	if (result == PATTERN_FOUND)
	{
		Pattern pattern(pCircuit_);
		pattern.initForTransitionDelayFault(pCircuit_);
		pPatternProcessor->patternVector_.push_back(pattern);
		assignPatternPI_fromGateVal(pPatternProcessor->patternVector_.back());

		if ((pPatternProcessor->staticCompression_ == PatternProcessor::OFF) && (pPatternProcessor->XFill_ == PatternProcessor::ON))
		{
			randomFill(pPatternProcessor->patternVector_.back());
		}

		pSimulator_->pfFaultSim(pPatternProcessor->patternVector_.back(), faultListToGen);
		pSimulator_->goodSim();
		assignPatternPO_fromGoodSimVal(pPatternProcessor->patternVector_.back());
	}
	else if (result == FAULT_UNTESTABLE)
	{
		faultListToGen.front()->faultState_ = Fault::AU;
		numOfAtpgUntestableFaults += faultListToGen.front()->equivalent_;
		faultListToGen.pop_front();
	}
	else
	{
		faultListToGen.front()->faultState_ = Fault::AB;
		faultListToGen.push_back(faultListToGen.front());
		faultListToGen.pop_front();
	}
}

// **************************************************************************
// Function   [ Atpg::StuckAtFaultATPG ]
// Commentor  [ HKY CYW ]
// Synopsis   [ usage: do stuck at fault model ATPG
//              in:    Pattern list, Fault list, int numOfAtpgUntestableFaults
//              out:   void //TODO
//            ]
// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
// **************************************************************************
// void Atpg::StuckAtFaultATPG(FaultPtrList &faultListToGen, PatternProcessor *pPatternProcessor, int &numOfAtpgUntestableFaults)
// {
// 	SINGLE_PATTERN_GENERATION_STATUS result = generateSinglePatternOnTargetFault(*faultListToGen.front(), false);
// 	if (result == PATTERN_FOUND)
// 	{
// 		Pattern *p = new Pattern;
// 		p->primaryInputs1st_ = new Value[pCircuit_->npi_];
// 		p->pseudoPrimaryInputs_ = new Value[pCircuit_->nppi_];
// 		p->primaryOutputs1st_ = new Value[pCircuit_->npo_];
// 		p->pseudoPrimaryOutputs_ = new Value[pCircuit_->nppi_];
// 		pPatternProcessor->patternVector_.push_back(p);
// 		assignPatternPI_fromGateVal(pPatternProcessor->patternVector_.back());
// 		// if static compression is OFF and random fill in on
// 		// do random x-fill
// 		if ((pPatternProcessor->staticCompression_ == PatternProcessor::OFF) && (pPatternProcessor->XFill_ == PatternProcessor::ON))
// 		{
// 			randomFill(pPatternProcessor->patternVector_.back());
// 		}
// 		pSimulator_->pfFaultSim(pPatternProcessor->patternVector_.back(), faultListToGen);
// 		pSimulator_->goodSim();
// 		assignPatternPO_fromGoodSimVal(pPatternProcessor->patternVector_.back());
// 	}
// 	else if (result == FAULT_UNTESTABLE)
// 	{
// 		faultListToGen.front()->faultState_ = Fault::AU;
// 		numOfAtpgUntestableFaults += faultListToGen.front()->equivalent_;
// 		faultListToGen.pop_front();
// 	}
// 	else
// 	{
// 		faultListToGen.front()->faultState_ = Fault::AB;
// 		faultListToGen.push_back(faultListToGen.front());
// 		faultListToGen.pop_front();
// 	}
// }

// **************************************************************************
// Function   [ Atpg::StuckAtFaultATPGWithDTC ]
// Commentor  [ CAL ]
// Synopsis   [ usage: do stuck at fault model ATPG with dynamic test compression
//              in:    Pattern list, Fault list, int numOfAtpgUntestableFaults
//              out:   void //add pattern to PatternProcessor*
//            ]
// Date       [ started 2020/07/07    last modified 2021/09/14 ]
// **************************************************************************
void Atpg::StuckAtFaultATPGWithDTC(FaultPtrList &faultListToGen, PatternProcessor *pPatternProcessor, int &numOfAtpgUntestableFaults)
{
	SINGLE_PATTERN_GENERATION_STATUS result = generateSinglePatternOnTargetFault(*faultListToGen.front(), false);
	if (result == PATTERN_FOUND)
	{
		Pattern pattern(pCircuit_);
		pPatternProcessor->patternVector_.push_back(pattern);

		resetPreValue();
		clearAllFaultEffectBySimulation();
		storeCurrentGateValue();
		assignPatternPI_fromGateVal(pPatternProcessor->patternVector_.back());

		if (pPatternProcessor->dynamicCompression_ == PatternProcessor::ON)
		{
			FaultPtrList faultListTemp = faultListToGen;
			pSimulator_->pfFaultSim(pPatternProcessor->patternVector_.back(), faultListToGen);
			pSimulator_->goodSim();
			assignPatternPO_fromGoodSimVal(pPatternProcessor->patternVector_.back());

			for (Fault *pFault : faultListTemp)
			{
				// skip detected faults
				if (pFault->faultState_ == Fault::DT)
				{
					continue;
				}

				Gate *pGateForAtivation = getWireForActivation(*pFault);
				if (((pGateForAtivation->atpgVal_ == L) && (pFault->faultType_ == Fault::SA0)) ||
						((pGateForAtivation->atpgVal_ == H) && (pFault->faultType_ == Fault::SA1)))
				{
					continue;
				}

				// Activation check
				if (pGateForAtivation->atpgVal_ != X)
				{
					if ((pFault->faultType_ == Fault::SA0) || (pFault->faultType_ == Fault::SA1))
					{
						setValueAndRunImp((*pGateForAtivation), X);
					}
					else
					{
						continue;
					}
				}

				if (isExistXPath(pGateForAtivation))
				{
					// TO-DO homework 05 implement DTC here end of TO-DO
					if (generateSinglePatternOnTargetFault(*pFault, true) == PATTERN_FOUND)
					{
						resetPreValue();
						clearAllFaultEffectBySimulation();
						storeCurrentGateValue();
						assignPatternPI_fromGateVal(pPatternProcessor->patternVector_.back());
					}
					else
					{
						for (Gate &gate : pCircuit_->gates_)
						{
							gate.atpgVal_ = gate.preValue_;
						}
					}
				}
				else
				{
					setValueAndRunImp((*pGateForAtivation), pGateForAtivation->preValue_);
				}
			}
		}

		clearAllFaultEffectBySimulation();
		storeCurrentGateValue();
		assignPatternPI_fromGateVal(pPatternProcessor->patternVector_.back());

		if (pPatternProcessor->XFill_ == PatternProcessor::ON)
		{
			// Randomly fill the pats_.back().
			// Note that the v_, gh_, gl_, fh_ and fl_ do not be changed.
			randomFill(pPatternProcessor->patternVector_.back());
		}

		//  This function will assign pi/ppi stored in pats_.back() to
		//  the gh_ and gl_ in each gate, and then it will run fault
		//  simulation to drop fault.

		pSimulator_->pfFaultSim(pPatternProcessor->patternVector_.back(), faultListToGen);

		// After pSimulator_->pfFaultSim(pPatternProcessor->patternVector_.back(),faultListToGen) , the pi/ppi
		// values have been passed to gh_ and gl_ of each gate.  Therefore, we can
		// directly use "assignPatternPO_fromGoodSimVal" to perform goodSim to get the PoValue.
		pSimulator_->goodSim();
		assignPatternPO_fromGoodSimVal(pPatternProcessor->patternVector_.back());
	}
	else if (result == FAULT_UNTESTABLE)
	{
		faultListToGen.front()->faultState_ = Fault::AU;
		numOfAtpgUntestableFaults += faultListToGen.front()->equivalent_;
		faultListToGen.pop_front();
	}
	else
	{
		faultListToGen.front()->faultState_ = Fault::AB;
		faultListToGen.push_back(faultListToGen.front());
		faultListToGen.pop_front();
	}
}

// **************************************************************************
// Function   [ Atpg::getWireForActivation ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                return the gate need for activation of a fault
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
Gate *Atpg::getWireForActivation(const Fault &fault)
{
	bool isOutputFault = (fault.faultyLine_ == 0);
	Gate *pGateForAtivation = NULL;
	Gate *pFaultyGate = &pCircuit_->gates_[fault.gateID_];
	if (!isOutputFault)
	{
		pGateForAtivation = &pCircuit_->gates_[pFaultyGate->faninVector_[fault.faultyLine_ - 1]];
	}
	else
	{
		pGateForAtivation = pFaultyGate;
	}
	return pGateForAtivation;
}

// **************************************************************************
// Function   [ Atpg::setValueAndRunImp ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Directly set the output of a gate to specific value and
//                run implication by event driven.
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::setValueAndRunImp(Gate &gate, const Value &val)
{
	clearEventStack(true);
	gate.atpgVal_ = val;
	for (const int &fanoutID : gate.fanoutVector_)
	{
		Gate &og = pCircuit_->gates_[fanoutID];
		if (!isInEventStack_[og.gateId_])
		{
			circuitLevel_to_EventStack_[og.numLevel_].push(og.gateId_);
			isInEventStack_[og.gateId_] = true;
		}
	}

	// event-driven simulation
	for (int i = gate.numLevel_; i < pCircuit_->tlvl_; ++i)
	{
		while (!circuitLevel_to_EventStack_[i].empty())
		{
			int gateID = circuitLevel_to_EventStack_[i].top();
			circuitLevel_to_EventStack_[i].pop();
			isInEventStack_[gateID] = false;
			Gate &currGate = pCircuit_->gates_[gateID];
			Value newValue = evaluateGoodVal(currGate);
			if (currGate.atpgVal_ != newValue)
			{
				currGate.atpgVal_ = newValue;
				for (int j = 0; j < currGate.numFO_; ++j)
				{
					Gate &og = pCircuit_->gates_[currGate.fanoutVector_[j]];
					if (!isInEventStack_[og.gateId_])
					{
						circuitLevel_to_EventStack_[og.numLevel_].push(og.gateId_);
						isInEventStack_[og.gateId_] = true;
					}
				}
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::resetPreValue ]
// Commentor  [ CAL ]
// Synopsis   [ usage: Reset the preV_ of each gate to X. ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::resetPreValue()
{
	for (Gate &gate : pCircuit_->gates_)
	{
		gate.preValue_ = X;
	}
}

// **************************************************************************
// Function   [ Atpg::storeCurrentGateValue ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Store gate.atpgVal_ to gate.preValue_
//              in:   void
//              out:  Count of values which change from H/L to the value which is not
//                    the same as preV_.
//            ]
// Date       [ started 2020/07/04    last modified 2020/07/04 ]
// **************************************************************************
int Atpg::storeCurrentGateValue()
{
	int numAssignedValueChanged = 0;
	for (Gate &gate : pCircuit_->gates_)
	{
		if ((gate.preValue_ != X) && (gate.preValue_ != gate.atpgVal_))
		{
			++numAssignedValueChanged;
		}
		gate.preValue_ = gate.atpgVal_;
	}

	if (numAssignedValueChanged != 0)
	{
		std::cerr << "Bug: storeCurrentGateValue detects the numAssignedValueChanged is not 0.\n";
		std::cin.get();
	}
	return numAssignedValueChanged;
}

// **************************************************************************
// Function   [ Atpg::clearAllFaultEffectBySimulation ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Clear all the fault effects before test generation for next
//                target fault.
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/04    last modified 2020/07/04 ]
// **************************************************************************
void Atpg::clearAllFaultEffectBySimulation()
{
	int numOfInputGate = pCircuit_->npi_ + pCircuit_->nppi_;
	for (Gate &gate : pCircuit_->gates_)
	{
		if (gate.gateId_ < numOfInputGate)
		{
			// Remove fault effects in input gates
			clearOneGateFaultEffect(gate);
		}
		else
		{
			// Simulate the whole circuit ( gates were sorted by lvl_ in "pCircuit_->gates_" )
			gate.atpgVal_ = evaluateGoodVal(gate);
		}
	}

	// Remove fault effects in input gates
	// for (int i = 0; i < numOfInputGate; ++i)
	// {
	// 	Gate &gate = pCircuit_->gates_[i];
	// 	clearOneGateFaultEffect(gate);
	// }
	// // Simulate the whole circuit ( gates were sorted by lvl_ in "pCircuit_->gates_" )
	// for (int i = 0; i < pCircuit_->tgate_; ++i)
	// {
	// 	Gate &gate = pCircuit_->gates_[i];
	// 	gate.atpgVal_ = evaluateGoodVal(gate);
	// }
}

// **************************************************************************
// Function   [ Atpg::clearOneGateFaultEffect ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Replace value of a gate from D/B to H/L.
//              in:    Gate
//              out:   void
//            ]
// Date       [ started 2020/07/04    last modified 2020/07/04 ]
// **************************************************************************
void Atpg::clearOneGateFaultEffect(Gate &gate)
{
	if (gate.atpgVal_ == D)
	{
		gate.atpgVal_ = H;
	}
	else if (gate.atpgVal_ == B)
	{
		gate.atpgVal_ = L;
	}
}

// **************************************************************************
// Function   [ Atpg::generateSinglePatternOnTargetFault ]
// Commentor  [ KOREAL ]
// Synopsis   [ usage: Given a fault, generate the pattern
//              in:    fault isDTC
//              out:   PATTERN_GENERATION_STATUS(PATTERN_FOUND = 0, FAULT_UNTESTABLE, ABORT) //TODO
//				There are four main atpgStatus while generating the pattern:
//
//				IMPLY_AND_CHECK: 	Determine as many signal values as possible then check if the backtrace is meaningful or not.
//				DECISION:			Using the multiple backtrace procedure to determine a final objective.
//				BACKTRACK:			If the values are incompatible during propagation or implications, backtracking is necessary.
//				JUSTIFY_FREE:		at the end of the process. Finding values on the primary inputs which justify all the values on the head lines
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/10 ]
// **************************************************************************
Atpg::SINGLE_PATTERN_GENERATION_STATUS Atpg::generateSinglePatternOnTargetFault(Fault fault, bool isDTC)
{

	int BackImpLevel = 0;																				// backward imply level
	int numOfBacktrack = 0;																			// backtrack times
	bool Finish = false;																				// Finish is true when whole pattern generation process is done
	bool faultPropToPO = false;																	// faultPropToPO is true when the fault is propagate to the PO
	Gate *pFaultyLine = NULL;																		// the gate pointer, whose fanOut is the target fault
	Gate *pLastDFrontier = NULL;																// the D-frontier gate which has the highest level of the D-frontier
	IMPLICATION_STATUS implyStatus;															// decide implication to go forward or backward
	BACKTRACE_STATUS backtraceFlag;															// backtrace flag including    { INITIAL, CHECK_AND_SELECT, CURRENT_OBJ_DETERMINE, FAN_OBJ_DETERMINE }
	SINGLE_PATTERN_GENERATION_STATUS genStatus = PATTERN_FOUND; // the atpgStatus that will be return, including		{ PATTERN_FOUND, FAULT_UNTESTABLE, ABORT }

	// Get the gate whose output is fault line and set the BackImpLevel
	// SET A FAULT SIGNAL
	pFaultyLine = initialize(fault, BackImpLevel, implyStatus, isDTC);
	// If there's no such gate, return FAULT_UNTESTABLE
	if (!pFaultyLine)
	{
		return FAULT_UNTESTABLE;
	}
	// SET BACKTRACE FLAG
	backtraceFlag = INITIAL;

	while (!Finish)
	{
		// Implication
		if (!Implication(implyStatus, BackImpLevel))
		{
			// implication INCONSISTENCY
			// record the number of backtrack
			if (decisionTree_.lastNodeMarked())
			{
				++numOfBacktrack;
			}
			// Abort if numOfBacktrack reaching the BACKTRACK_LIMIT
			if (numOfBacktrack > BACKTRACK_LIMIT)
			{
				genStatus = ABORT;
				Finish = true;
			}

			clearAllEvent();

			// IS THERE AN UNTRIED COMBINATION OF VALUES ON ASSIGNED HEAD LINES OR FANOOUT POINTS?
			// If yes, SET UNTRIED COMBINATION OF VALUES in the function backtrack
			if (backtrack(BackImpLevel))
			{
				// backtrack success and initialize the data
				// SET BACKTRACE FLAG
				backtraceFlag = INITIAL;
				implyStatus = (BackImpLevel > 0) ? BACKWARD : FORWARD;
				pLastDFrontier = NULL;
			}
			else
			{
				// backtrack fail
				// EXIT: FAULT_UNTESTABLE FAULT
				genStatus = FAULT_UNTESTABLE;
				Finish = true;
			}
			continue;
		}

		// IS CONTINUATION OF BACKTRACE MEANINGFUL?
		if (!continuationMeaningful(pLastDFrontier))
		{
			backtraceFlag = INITIAL;
		}

		// FAULT SIGNAL PROPAGATED TO A PRIMARY OUTPUT?
		if (checkFaultPropToPO(faultPropToPO))
		{
			// IS THERE ANY UNJUSTIFIED BOUND LINE?
			if (checkUnjustifiedBoundLines())
			{
				// DETERMINE A FINAL OBJECTIVE TO ASSIGN A VALUE
				findFinalObjective(backtraceFlag, faultPropToPO, pLastDFrontier);
				// ASSIGN A VALUE TO THE FINAL OBJECTIVE LINE
				assignValueToFinalObject();
				implyStatus = FORWARD;
				continue;
			}
			else
			{
				// Finding values on the primary inputs which justify all the values on the head lines
				// LINE JUSTIFICATION OF FREE LINES
				justifyFreeLines(fault);
				// EXIT: TEST GENERATED
				genStatus = PATTERN_FOUND;
				Finish = true;
			}
		}
		else
		{
			// not propagate to PO
			// THE NUMBER OF GATES IN D-FRONTIER?
			int numGatesInDFrontier = countNumGatesInDFrontier(pFaultyLine);

			// ZERO
			if (numGatesInDFrontier == 0)
			{
				// no frontier can propagate to the PO
				// record the number of backtrack
				if (decisionTree_.lastNodeMarked())
				{
					++numOfBacktrack;
				}
				// Abort if numOfBacktrack reaching the BACKTRACK_LIMIT
				if (numOfBacktrack > BACKTRACK_LIMIT)
				{
					genStatus = ABORT;
					Finish = true;
				}

				clearAllEvent();

				// IS THERE AN UNTRIED COMBINATION OF VALUES ON ASSIGNED HEAD LINES OR FANOOUT POINTS?
				// If yes, SET UNTRIED COMBINATION OF VALUES in the function backtrack
				if (backtrack(BackImpLevel))
				{
					// backtrack success and initialize the data
					// SET BACKTRACE FLAG
					backtraceFlag = INITIAL;
					implyStatus = BackImpLevel > 0 ? BACKWARD : FORWARD;
					pLastDFrontier = NULL;
				}
				else
				{
					// backtrack fail
					// EXIT: FAULT_UNTESTABLE FAULT
					genStatus = FAULT_UNTESTABLE;
					Finish = true;
				}
			}
			else if (numGatesInDFrontier == 1)
			{
				// There exist just one path to the PO
				// UNIQUE SENSITIZATION
				BackImpLevel = uniquePathSensitize(pCircuit_->gates_[dFrontier_[0]]);
				// Unique Sensitization fail
				if (BackImpLevel == UNIQUE_PATH_SENSITIZE_FAIL)
				{
					// If UNIQUE_PATH_SENSITIZE_FAIL, the number of gates in d-frontier in the next while loop
					// and will backtrack
					continue;
				}
				// Unique Sensitization success
				if (BackImpLevel > 0)
				{
					implyStatus = BACKWARD;
					continue;
				}
				else if (BackImpLevel == 0)
				{
					continue;
				}
				else
				{
					// BackImpLevel < 0, find an objective and set backtraceFlag and pLastDFrontier
					findFinalObjective(backtraceFlag, faultPropToPO, pLastDFrontier);
					assignValueToFinalObject();
					implyStatus = FORWARD;
					continue;
				}
			}
			else
			{ // more than one
				// DETERMINE A FINAL OBJECTIVE TO ASSIGN A VALUE
				findFinalObjective(backtraceFlag, faultPropToPO, pLastDFrontier);
				// ASSIGN A VALUE TO THE FINAL OBJECTIVE LINE
				assignValueToFinalObject();
				implyStatus = FORWARD;
				continue;
			}
		}
	}
	return genStatus;
}

Gate *Atpg::initialize(Fault &fault, int &BackImpLevel, IMPLICATION_STATUS &implyStatus, bool isDTC)
{
	Gate *gFaultyLine = &pCircuit_->gates_[fault.gateID_];
	currentTargetFault_ = fault;

	// if fault at gate's input, change the gFaultyLine to the input gate
	if (fault.faultyLine_)
	{
		gFaultyLine = &pCircuit_->gates_[gFaultyLine->faninVector_[fault.faultyLine_ - 1]];
	}
	initialList(true);
	initialNetlist(*gFaultyLine, isDTC);
	int fGate_id = fault.gateID_;
	// currentTargetHeadLineFault_.gateID_ = -1; //bug report
	if (gateID_to_lineType_[gFaultyLine->gateId_] == FREE_LINE)
	{
		if ((fault.faultType_ == Fault::SA0 || fault.faultType_ == Fault::STR) && gFaultyLine->atpgVal_ != L)
		{
			gFaultyLine->atpgVal_ = D;
		}
		if ((fault.faultType_ == Fault::SA1 || fault.faultType_ == Fault::STF) && gFaultyLine->atpgVal_ != H)
		{
			gFaultyLine->atpgVal_ = B;
		}
		backtrackList_.push_back(gFaultyLine->gateId_);

		currentTargetHeadLineFault_ = setFreeFaultyGate(*gFaultyLine);
		currentTargetFault_ = currentTargetHeadLineFault_;
		BackImpLevel = 0;
		implyStatus = FORWARD;
		fGate_id = currentTargetHeadLineFault_.gateID_;
	}
	else
	{
		BackImpLevel = setFaultyGate(fault);
	}

	if (BackImpLevel < 0)
	{
		return NULL;
	}

	dFrontier_.push_back(fGate_id);

	int Level = uniquePathSensitize(pCircuit_->gates_[fGate_id]);
	if (Level == UNIQUE_PATH_SENSITIZE_FAIL)
	{
		return NULL;
	}

	if (Level > BackImpLevel)
	{
		BackImpLevel = Level;
		implyStatus = BACKWARD;
	}

	if (fault.faultType_ == Fault::STR || fault.faultType_ == Fault::STF)
	{
		Level = firstTimeFrameSetUp(fault);
		if (Level < 0)
		{
			return NULL;
		}

		if (Level > BackImpLevel)
		{
			BackImpLevel = Level;
			implyStatus = BACKWARD;
		}
	}
	return &pCircuit_->gates_[fGate_id];
}

void Atpg::initialList(bool initFlag)
{
	initObject_.clear();
	currentObject_.clear();
	fanoutObjective_.clear();
	headObject_.clear();
	finalObject_.clear();

	if (initFlag)
	{
		unjustified_.clear();
		dFrontier_.clear();
		backtrackList_.clear();
		decisionTree_.clear();
		currentTargetHeadLineFault_ = Fault(); // NE
		firstTimeFrameHeadLine_ = NULL;
	}
}

void Atpg::initialNetlist(Gate &gFaultyLine, bool isDTC)
{
	for (Gate &gate : pCircuit_->gates_)
	{
		if (gateID_to_lineType_[gate.gateId_] == FREE_LINE)
		{
			gateID_to_valModified_[gate.gateId_] = 1;
		}
		else
		{
			gateID_to_valModified_[gate.gateId_] = 0;
		}
		gateID_to_reachableByTargetFault_[gate.gateId_] = 0;

		// assign value outside the generateSinglePatternOnTargetFault for DTC, so
		// only need to initialize it for primary fault.
		if (!isDTC)
		{
			gate.atpgVal_ = X;
		}
		gateID_to_xPathStatus_[gate.gateId_] = UNKNOWN;
	}

	pushGateToEventStack(gFaultyLine.gateId_);

	for (int i = gFaultyLine.numLevel_; i < pCircuit_->tlvl_; ++i)
	{
		while (!circuitLevel_to_EventStack_[i].empty())
		{
			int gateID = popEventStack(i);
			const Gate &rCurrentGate = pCircuit_->gates_[gateID];
			gateID_to_valModified_[gateID] = 0;
			gateID_to_reachableByTargetFault_[gateID] = 1;

			for (int fanoutID : rCurrentGate.fanoutVector_)
			{
				pushGateToEventStack(fanoutID);
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::clearEventStack ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Clear circuitLevel_to_EventStack_ and carefully reset gateID_to_valModified_ and isInEventStack_
//              in:    check isInEventStack_ correctness
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::clearEventStack(bool isDebug)
{

	// pop and unmark
	for (std::stack<int> &eventStack : circuitLevel_to_EventStack_)
	{
		while (!eventStack.empty())
		{
			int gateID = eventStack.top();
			eventStack.pop();
			gateID_to_valModified_[gateID] = 0;
			isInEventStack_[gateID] = false;
		}
	}

	// expect all gates in circuit must be unmarked
	// after the above for-loop.
	if (isDebug)
	{
		for (int i = 0; i < pCircuit_->tgate_; ++i)
		{
			if (isInEventStack_[i])
			{
				std::cerr << "Warning clearEventStack found unexpected behavior\n";
				isInEventStack_[i] = false;
				std::cin.get();
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::Implication ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Do BACKWARD and FORWARD implication to gates in circuitLevel_to_EventStack_,
//                     also put gates which can't be implied into unjustified_ list.
//              in:    atpgStatus(BACKWARD or FORWARD), StartLevel
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
bool Atpg::Implication(IMPLICATION_STATUS atpgStatus, int StartLevel)
{
	IMPLICATION_STATUS impRet;

	if (atpgStatus != BACKWARD)
	{
		StartLevel = 0;
	}

	do
	{
		if (atpgStatus == BACKWARD)
		{
			// BACKWARD loop: Do evaluation() to gates in circuitLevel_to_EventStack_ in BACKWARD order from StartLevel.
			// If one of them returns CONFLICT, Implication() returns false.
			for (int i = StartLevel; i >= 0; --i)
			{
				while (!circuitLevel_to_EventStack_[i].empty())
				{
					Gate *pGate = &pCircuit_->gates_[popEventStack(i)];
					impRet = evaluation(pGate);
					if (impRet == CONFLICT)
					{
						return false;
					}
				}
			}
		}

		atpgStatus = FORWARD;
		for (int i = 0; i < pCircuit_->tlvl_; ++i)
		{
			// FORWARD loop: Do evaluation() to gates in circuitLevel_to_EventStack_ in FORWARD order till it gets to MaxLevel.
			// If one of them returns CONFLICT, Implication() returns false.
			// If one of them returns BACKWARD, set StartLevel to current level - 1, goto BACKWARD loop.
			while (!circuitLevel_to_EventStack_[i].empty())
			{
				Gate *pGate = &pCircuit_->gates_[popEventStack(i)];
				impRet = evaluation(pGate);
				if (impRet == CONFLICT)
				{
					return false;
				}
				else if (impRet == BACKWARD)
				{
					StartLevel = i - 1;
					atpgStatus = BACKWARD;
					break;
				}
			}

			if (atpgStatus == BACKWARD)
			{
				break;
			}
		}
	} while (atpgStatus == BACKWARD);

	return true;
}

Atpg::IMPLICATION_STATUS Atpg::backwardImplication(Gate *pGate)
{
	IMPLICATION_STATUS implyStatus = FORWARD;
	if (pGate->gateType_ == Gate::PI || pGate->gateType_ == Gate::PPI)
	{
		return FORWARD;
	}

	if (pGate->gateType_ == Gate::BUF || pGate->gateType_ == Gate::INV || pGate->gateType_ == Gate::PO || pGate->gateType_ == Gate::PPO)
	{
		Gate *pImpGate = &pCircuit_->gates_[pGate->faninVector_[0]];
		gateID_to_valModified_[pGate->gateId_] = 1;

		Value isINV = pGate->gateType_ == Gate::INV ? H : L;
		pImpGate->atpgVal_ = cXOR2(pGate->atpgVal_, isINV);

		backtrackList_.push_back(pImpGate->gateId_);
		// pushInputEvents(pGate->gateId_, 0); replaced by wang
		pushGateToEventStack(pGate->faninVector_[0]);
		pushGateFanoutsToEventStack(pGate->faninVector_[0]);
		implyStatus = BACKWARD;
	}
	else if (pGate->gateType_ == Gate::XOR2 || pGate->gateType_ == Gate::XNOR2)
	{
		Gate *pInputGate0 = &pCircuit_->gates_[pGate->faninVector_[0]];
		Gate *pInputGate1 = &pCircuit_->gates_[pGate->faninVector_[1]];

		implyStatus = BACKWARD;

		if (pInputGate0->atpgVal_ == X && pInputGate1->atpgVal_ != X)
		{
			if (pGate->gateType_ == Gate::XOR2)
			{
				pInputGate0->atpgVal_ = cXOR2(pGate->atpgVal_, pInputGate1->atpgVal_);
			}
			else
			{
				pInputGate0->atpgVal_ = cXNOR2(pGate->atpgVal_, pInputGate1->atpgVal_);
			}
			gateID_to_valModified_[pGate->gateId_] = 1;
			backtrackList_.push_back(pInputGate0->gateId_);
			// pushInputEvents(pGate->gateId_, 0); replaced by wang
			pushGateToEventStack(pGate->faninVector_[0]);
			pushGateFanoutsToEventStack(pGate->faninVector_[0]);
		}
		else if (pInputGate1->atpgVal_ == X && pInputGate0->atpgVal_ != X)
		{
			if (pGate->gateType_ == Gate::XOR2)
			{
				pInputGate1->atpgVal_ = cXOR2(pGate->atpgVal_, pInputGate0->atpgVal_);
			}
			else
			{
				pInputGate1->atpgVal_ = cXNOR2(pGate->atpgVal_, pInputGate0->atpgVal_);
			}
			gateID_to_valModified_[pGate->gateId_] = 1;
			backtrackList_.push_back(pInputGate1->gateId_);
			// pushInputEvents(pGate->gateId_, 1); replaced with following two function call by wang
			pushGateToEventStack(pGate->faninVector_[1]);
			pushGateFanoutsToEventStack(pGate->faninVector_[1]);
		}
		else
		{
			implyStatus = FORWARD;
			unjustified_.push_back(pGate->gateId_);
		}
	}
	else if (pGate->gateType_ == Gate::XOR3 || pGate->gateType_ == Gate::XNOR3)
	{
		Gate *pInputGate0 = &pCircuit_->gates_[pGate->faninVector_[0]];
		Gate *pInputGate1 = &pCircuit_->gates_[pGate->faninVector_[1]];
		Gate *pInputGate2 = &pCircuit_->gates_[pGate->faninVector_[2]];
		unsigned NumOfX = 0;
		unsigned ImpPtr = 0;
		if (pInputGate0->atpgVal_ == X)
		{
			++NumOfX;
			ImpPtr = 0;
		}
		if (pInputGate1->atpgVal_ == X)
		{
			++NumOfX;
			ImpPtr = 1;
		}
		if (pInputGate2->atpgVal_ == X)
		{
			++NumOfX;
			ImpPtr = 2;
		}
		if (NumOfX == 1)
		{
			Gate *pImpGate = &pCircuit_->gates_[pGate->faninVector_[ImpPtr]];
			Value temp;
			if (ImpPtr == 0)
			{
				temp = cXOR3(pGate->atpgVal_, pInputGate1->atpgVal_, pInputGate2->atpgVal_);
			}
			else if (ImpPtr == 1)
			{
				temp = cXOR3(pGate->atpgVal_, pInputGate0->atpgVal_, pInputGate2->atpgVal_);
			}
			else
			{
				temp = cXOR3(pGate->atpgVal_, pInputGate1->atpgVal_, pInputGate0->atpgVal_);
			}

			if (pGate->gateType_ == Gate::XNOR3)
			{
				temp = cINV(temp);
			}
			pImpGate->atpgVal_ = temp;
			gateID_to_valModified_[pGate->gateId_] = 1;
			backtrackList_.push_back(pImpGate->gateId_);
			// pushInputEvents(pGate->gateId_, ImpPtr); replaced by wang
			pushGateToEventStack(pGate->faninVector_[ImpPtr]);
			pushGateFanoutsToEventStack(pGate->faninVector_[ImpPtr]);
			implyStatus = BACKWARD;
		}
		else
		{
			unjustified_.push_back(pGate->gateId_);
			implyStatus = FORWARD;
		}
	}
	else
	{
		Value OutputControlVal = pGate->getOutputCtrlValue();
		Value InputControlVal = pGate->getInputCtrlValue();

		if (pGate->atpgVal_ == OutputControlVal)
		{
			gateID_to_valModified_[pGate->gateId_] = 1;
			Value InputNonControlVal = pGate->getInputNonCtrlValue();

			for (int i = 0; i < pGate->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
				if (pFaninGate->atpgVal_ == X)
				{
					pFaninGate->atpgVal_ = InputNonControlVal;
					backtrackList_.push_back(pFaninGate->gateId_);
					// pushInputEvents(pGate->gateId_, i); replaced by wang
					pushGateToEventStack(pGate->faninVector_[i]);
					pushGateFanoutsToEventStack(pGate->faninVector_[i]);
				}
			}
			implyStatus = BACKWARD;
		}
		else
		{
			int NumOfX = 0;
			int ImpPtr = 0;
			for (int i = 0; i < pGate->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
				if (pFaninGate->atpgVal_ == X)
				{
					++NumOfX;
					ImpPtr = i;
				}
			}

			if (NumOfX == 1)
			{
				Gate *pImpGate = &pCircuit_->gates_[pGate->faninVector_[ImpPtr]];
				pImpGate->atpgVal_ = InputControlVal;
				gateID_to_valModified_[pGate->gateId_] = 1;
				backtrackList_.push_back(pImpGate->gateId_);
				// pushInputEvents(pGate->gateId_, ImpPtr); replaced by wang
				pushGateToEventStack(pGate->faninVector_[ImpPtr]);
				pushGateFanoutsToEventStack(pGate->faninVector_[ImpPtr]);
				implyStatus = BACKWARD;
			}
			else
			{
				unjustified_.push_back(pGate->gateId_);
				implyStatus = FORWARD;
			}
		}
	}
	return implyStatus;
}

// **************************************************************************
// Function   [ Atpg::backtrack ]
// Commentor  [ CLT ]
// Synopsis   [ usage: If decisionTree_ is not empty, update BackImpLevel.
//                     If decisionTree_ is empty, return false.
//              in:    BackImpLevel
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
//
//
// those decision gates are put in the decisionTree
// the associated implication are put in the backtrackList
//
// when we backtrack a single gate in decision tree,
// we need to recover all the associated implications
// starting from the startPoint of backtrakList
//
//                       +------+------+
//  decision tree nodes: | G0=1 | G1=0 |...
//                       +------+------+
//       startPoint of G0=1 |        \ startPoint of G1=0
//                          V         \						.
//                       +----+----++----+----+----+
//  backtracklist:       |G4=0|G7=1||G6=1|G9=0|G8=0|...
//                       +----+----++----+----+----+
//                       \         /\______ _______/
//                        \___ ___/        V
//                            V            associated implications of G1=0
//         associated implications of G0=1
// **************************************************************************
bool Atpg::backtrack(int &BackImpLevel)
{
	Gate *pDecisionGate = NULL;
	int mDecisionGateID;
	unsigned BacktrackPoint = 0;
	Value Val;
	// backtrackList_ is for backtrack

	while (!decisionTree_.empty())
	{
		if (decisionTree_.get(mDecisionGateID, BacktrackPoint))
		{
			continue;
		}
		// pDecisionGate is the bottom node of decisionTree_

		updateUnjustifiedLines();
		pDecisionGate = &pCircuit_->gates_[mDecisionGateID];
		Val = cINV(pDecisionGate->atpgVal_);

		for (int i = BacktrackPoint; i < (int)backtrackList_.size(); ++i)
		{
			// Reset gates and their ouput in backtrackList_, starts from its backtrack point.
			Gate *pGate = &pCircuit_->gates_[backtrackList_[i]];

			pGate->atpgVal_ = X;
			gateID_to_valModified_[pGate->gateId_] = 0;

			for (int j = 0; j < pGate->numFO_; ++j)
			{
				Gate *pFanoutGate = &pCircuit_->gates_[pGate->fanoutVector_[j]];
				gateID_to_valModified_[pFanoutGate->gateId_] = 0;
			}
		}

		BackImpLevel = 0;

		for (int i = BacktrackPoint + 1; i < (int)backtrackList_.size(); ++i)
		{
			// Find MAX level output in backtrackList_ and save it to BackImpLevel.
			Gate *pGate = &pCircuit_->gates_[backtrackList_[i]];

			for (int j = 0; j < pGate->numFO_; ++j)
			{
				Gate *pFanoutGate = &pCircuit_->gates_[pGate->fanoutVector_[j]];

				if (pFanoutGate->atpgVal_ != X)
				{
					if (!gateID_to_valModified_[pFanoutGate->gateId_])
					{
						unjustified_.push_back(pFanoutGate->gateId_);
					}
					pushGateToEventStack(pFanoutGate->gateId_);
				}

				if (pFanoutGate->numLevel_ > BackImpLevel)
				{
					BackImpLevel = pFanoutGate->numLevel_;
				}
			}
		}

		backtrackList_.resize(BacktrackPoint + 1);
		pDecisionGate->atpgVal_ = Val; // Reverse its value, do backtrack.

		if (gateID_to_lineType_[pDecisionGate->gateId_] == HEAD_LINE)
		{
			gateID_to_valModified_[pDecisionGate->gateId_] = 0;
		}
		else
		{
			pushGateToEventStack(pDecisionGate->gateId_);
		}

		pushGateFanoutsToEventStack(pDecisionGate->gateId_);

		Gate *pFaultyGate = &pCircuit_->gates_[currentTargetFault_.gateID_];

		dFrontier_.clear();
		dFrontier_.push_back(pFaultyGate->gateId_);
		updateDFrontier();

		// Update unjustified_ list.
		for (int k = unjustified_.size() - 1; k >= 0; --k)
		{
			if (pCircuit_->gates_[unjustified_[k]].atpgVal_ == X)
			{
				vecDelete(unjustified_, k);
			}
		}
		// Reset xPathStatus.
		for (int i = pFaultyGate->gateId_; i < pCircuit_->tgate_; ++i)
		{
			gateID_to_xPathStatus_[i] = UNKNOWN;
		}
		return true;
	}
	return false;
}

bool Atpg::continuationMeaningful(Gate *pLastDFrontier)
{
	bool fDFrontierChanged; // fDFrontierChanged is true when D-frontier must change
	// update unjustified lines
	updateUnjustifiedLines();

	// if the initial object is modified, delete it from initObject_ list
	for (int k = initObject_.size() - 1; k >= 0; --k)
	{
		if (gateID_to_valModified_[initObject_[k]])
		{
			vecDelete(initObject_, k);
		}
	}

	// determine the pLastDFrontier should be changed or not
	if (pLastDFrontier != NULL)
	{
		if (pLastDFrontier->atpgVal_ == X)
		{
			fDFrontierChanged = false;
		}
		else
		{
			fDFrontierChanged = true;
		}
	}
	else
	{
		fDFrontierChanged = true;
	}
	// If all init. objectives have been implied or the last D-frontier has changed, reset backtrace atpgStatus
	return !(initObject_.empty() || fDFrontierChanged);
}

// **************************************************************************
// Function   [ Atpg::updateUnjustifiedLines ]
// Commentor  [ CAL ]
// Synopsis   [ usage: update unjustified_ list
//              in:    void
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::updateUnjustifiedLines()
{
	// scan all gates in unjustified_ List, if some gates were put into unjustified list but were implied afterwards by other gates, remove those gates from the unjustified list.
	// if gateID_to_valModified_[mGate.gateId_]  == true, delete it from unjustified_ List
	// else push mGate into finalObject_ List
	for (int i = unjustified_.size() - 1; i >= 0; --i)
	{
		Gate &mGate = pCircuit_->gates_[unjustified_[i]];
		if (gateID_to_valModified_[mGate.gateId_])
		{
			vecDelete(unjustified_, i);
		}
		else
		{
			gateID_to_valModified_[mGate.gateId_] = 0;
			finalObject_.push_back(mGate.gateId_);
		}
	}

	// pop all element from finalObject_ and set it's gateID_to_valModified_ to false till finalObject_ is empty
	int gateID;
	while (!finalObject_.empty())
	{
		gateID = vecPop(finalObject_);
		gateID_to_valModified_[gateID] = 0;
	}
}

// **************************************************************************
// Function   [ Atpg::updateDFrontier ]
// Commentor  [ CAL ]
// Synopsis   [ usage: update DFrontier
//              in:    void
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::updateDFrontier()
{
	for (int i = 0; i < dFrontier_.size();)
	{
		Gate &mGate = pCircuit_->gates_[dFrontier_[i]];
		if (mGate.atpgVal_ == D || mGate.atpgVal_ == B)
		{
			for (int j = 0; j < mGate.numFO_; ++j)
			{
				dFrontier_.push_back(mGate.fanoutVector_[j]);
			}
			vecDelete(dFrontier_, i);
		}
		else if (mGate.atpgVal_ == X)
		{
			++i;
		}
		else
		{
			vecDelete(dFrontier_, i);
		}
	}
}

bool Atpg::checkFaultPropToPO(bool &faultPropToPO)
{
	// find out is there a D or B at on any PO?(i.e. The fault is propagate to the PO)
	for (int i = 0; i < pCircuit_->npo_ + pCircuit_->nppi_; ++i)
	{
		Value v = pCircuit_->gates_[pCircuit_->tgate_ - i - 1].atpgVal_;
		if (v == D || v == B)
		{
			faultPropToPO = true;
			return true;
		}
	}
	faultPropToPO = false;
	return false;
}

bool Atpg::checkUnjustifiedBoundLines()
{
	// Find if there exists any unjustified bound line
	for (int i = 0; i < unjustified_.size(); ++i)
	{
		Gate *pGate = &pCircuit_->gates_[unjustified_[i]];
		if (pGate->atpgVal_ != X && !gateID_to_valModified_[pGate->gateId_] && gateID_to_lineType_[pGate->gateId_] == BOUND_LINE)
		{ // unjustified bound line
			return true;
		}
	}
	return false;
}

// **************************************************************************
// Function   [ Atpg::findFinalObjective ]
// Commentor  [ WYH ]
// Synopsis   [ usage: Determination of final objective.
//                     Choose a value and a line such that the chosen value assigned
//                     to the chosen line can meet the initial objectives.
//              in:    BACKTRACE_STATUS& flag: The flag indicates the backtrace atpgStatus.
//                     bool FaultPropPO: The FaultPropPo indicates whether the
//                     fault signal can propagate to PO or not.
//                     Gate* pLastDFrontier: The pLastDFrontier indicates the cloest
//                     DFrontier to PO
//              out:   flag(BACKTRACE_STATUS reference)
//                     pLastDFrontier(Gate pointer reference)
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/15 ]
// **************************************************************************
void Atpg::findFinalObjective(BACKTRACE_STATUS &flag, bool FaultPropPO, Gate *&pLastDFrontier)
{
	int index;
	Gate *pGate = NULL;
	BACKTRACE_RESULT result;
	int finalObjectiveId = -1;

	while (true)
	{
		// IS BACKTRACE FLAG ON?
		if (flag == INITIAL)
		{ // YES
			// RESET BACKTRACE FLAG
			flag = FAN_OBJ_DETERMINE;
			// set the times of objective 0 and objective 1 of the gate to be zero
			// AND LET ALL THE SETS OF OBJECTIVES BE EMPTY
			for (const int &gateID : backtraceResetList_)
			{
				setGaten0n1(gateID, 0, 0);
			}
			backtraceResetList_.clear();
			initialList(false);

			// IS THERE ANY UNJUSTIFIED LINE?
			if (!unjustified_.empty())
			{ // YES
				// LET ALL THE UNJUSTIFIED LINES BE THE SET OF INITIAL OBJECTIVES
				initObject_ = unjustified_;

				// FAULT SIGNAL PROPAGATED TO A PRIMARY OUTPUT?
				if (FaultPropPO) // YES
				{								 // do not add any gates
					pLastDFrontier = NULL;
				}
				else
				{ // NO
					// ADD A GATE IN D-FRONTIER TO THE SET OF INITIAL OBJECTIVES
					pLastDFrontier = findCloseToOutput(dFrontier_, index);
					initObject_.push_back(pLastDFrontier->gateId_);
				}
			}
			else
			{ // NO
				// ADD A GATE IN D-FRONTIER TO THE SET OF INITIAL OBJECTIVES
				pLastDFrontier = findCloseToOutput(dFrontier_, index);
				initObject_.push_back(pLastDFrontier->gateId_);
			}

			// A
			// MULTIPLE BACKTRACE FROM THE SET OF INITIAL OBJECTIVES
			result = multipleBacktrace(INITIAL, finalObjectiveId);
			// CONTRADICTORY REQUIREMENT AT A FANOUT-POINT OCCURRED?
			if (result == CONTRADICTORY)
			{ // YES
				// LET THE FANOUT-POINT OBJECTIVE BE FINAL OBJECTIVE TO ASSIGNA VALUE
				finalObject_.push_back(finalObjectiveId);
				// EXIT
				return;
			}
		}
		else
		{ // NO
			// IS THE SET OF FANOUT-POINT OBJECTIVE EMPTY?
			if (!fanoutObjective_.empty())
			{ // NO
				// B
				// MULTIPLE BACKTRACE FROM A FANOUT-POINT OBJECTIVE
				result = multipleBacktrace(FAN_OBJ_DETERMINE, finalObjectiveId);
				// CONTRADICTORY REQUIREMENT AT A FANOUT-POINT OCCURRED?
				if (result == CONTRADICTORY)
				{ // YES
					// LET THE FANOUT-POINT OBJECTIVE BE FINAL OBJECTIVE TO ASSIGNA VALUE
					finalObject_.push_back(finalObjectiveId);
					// EXIT
					return;
				}
			}
		}

		while (true)
		{
			// IS THE SET OF HEAD OBJECTIVES EMPTY?
			if (headObject_.empty())
			{ // YES
				flag = INITIAL;
				break;
			}
			else
			{ // NO
				// TAKE OUT A HEAD OBJECTIVE
				pGate = &pCircuit_->gates_[vecPop(headObject_)];
				// IS THE HEAD LINE UNSPECIFIED?
				if (pGate->atpgVal_ == X)
				{ // YES
					// LET THE HEAD OBJECTIVE BE FINAL OBJECTIVE
					finalObject_.push_back(pGate->gateId_);
					// EXIT
					return;
				}
			}
		}
	}
}

void Atpg::assignValueToFinalObject()
{
	while (!finalObject_.empty())
	{ // while exist any finalObject
		Gate *pGate = &pCircuit_->gates_[vecPop(finalObject_)];

		// judge the value by numOfZero and numOfOne
		if (gateID_to_n0_[pGate->gateId_] > gateID_to_n1_[pGate->gateId_])
		{
			pGate->atpgVal_ = L;
		}
		else
		{
			pGate->atpgVal_ = H;
		}

		// put decision of the finalObjective into decisionTree
		decisionTree_.put(pGate->gateId_, backtrackList_.size());
		// record this gate and backtrace later
		backtrackList_.push_back(pGate->gateId_);

		if (gateID_to_lineType_[pGate->gateId_] == HEAD_LINE)
		{
			gateID_to_valModified_[pGate->gateId_] = 1;
		}
		else
		{
			pushGateToEventStack(pGate->gateId_);
		}
		pushGateFanoutsToEventStack(pGate->gateId_);
	}
}

// **************************************************************************
// Function   [ Atpg::justifyFreeLines ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Justify free lines before terminating this test pattern generation.
//              in:    original fault
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::justifyFreeLines(Fault &fOriginalFault)
{
	Gate *pHeadLineFaultGate = NULL;

	int gateID = currentTargetHeadLineFault_.gateID_;
	if (gateID != -1)
	{
		pHeadLineFaultGate = &pCircuit_->gates_[gateID];
	}

	// scan each HEADLINE
	for (int i = 0; i < numberOfHeadLine_; ++i)
	{
		Gate *pGate = &pCircuit_->gates_[headLineGateIDs_[i]];
		if (pGate->preValue_ == pGate->atpgVal_)
		{
			continue;
		}
		if (pHeadLineFaultGate == pGate)
		{ // if the HEADLINE scaned now(pGate) is the new faulty gate
			// If the Original Fault is on a FREE LINE, map it back
			restoreFault(fOriginalFault);
			continue;
		}
		// for other HEADLINE, set D or D' to H or L respectively,
		if (pGate->atpgVal_ == D)
		{
			pGate->atpgVal_ = H;
		}
		else if (pGate->atpgVal_ == B)
		{
			pGate->atpgVal_ = L;
		}

		if (!(pGate->gateType_ == Gate::PI || pGate->gateType_ == Gate::PPI || pGate->atpgVal_ == X))
		{
			fanoutFreeBacktrace(pGate); // do fanoutFreeBacktrace()}
		}
	}
}
// **************************************************************************
// Function   [ Atpg::restoreFault ]
// Commentor  [ CLT ]
// Synopsis   [ usage: restore fault
//              in:    Fault list
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::restoreFault(Fault &fOriginalFault)
{
	Gate *pFaultPropGate = NULL;

	fanoutObjective_.clear();
	pFaultPropGate = &pCircuit_->gates_[fOriginalFault.gateID_];

	if (fOriginalFault.faultyLine_ == 0)
	{
		fanoutObjective_.push_back(pFaultPropGate->gateId_);
	}

	// To fix bugs of free line fault which is at gate input
	for (int i = 0; i < pFaultPropGate->numFI_; ++i)
	{
		Gate *pFaninGate = &pCircuit_->gates_[pFaultPropGate->faninVector_[i]];
		if (pFaninGate->atpgVal_ == D)
		{
			pFaninGate->atpgVal_ = H;
		}
		else if (pFaninGate->atpgVal_ == B)
		{
			pFaninGate->atpgVal_ = L;
		}

		if (pFaninGate->atpgVal_ == L || pFaninGate->atpgVal_ == H)
		{
			fanoutObjective_.push_back(pFaninGate->gateId_);
		}
	}

	// push original fault gate into fanoutObjective_ list

	// for each loop, scan all fanin gates of pFaultPropGate (initial to be original fault gate)
	// if fanin gates' value == 0 or 1, add it into fanoutObjective_ list iteratively
	// let pFaultPropGate's output gate (FREELINE only have one output gate) be new pFaultPropGate
	// and perform the procedure of each loop till pFaultPropGate is HEADLINE
	if (gateID_to_lineType_[pFaultPropGate->gateId_] == FREE_LINE)
	{
		while (pFaultPropGate->numFO_ > 0)
		{
			pFaultPropGate = &pCircuit_->gates_[pFaultPropGate->fanoutVector_[0]];
			for (int i = 0; i < pFaultPropGate->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pFaultPropGate->faninVector_[i]];
				if (pFaninGate->atpgVal_ == L || pFaninGate->atpgVal_ == H)
				{
					fanoutObjective_.push_back(pFaninGate->gateId_);
				}
			}
			if (gateID_to_lineType_[pFaultPropGate->gateId_] == HEAD_LINE)
			{
				break;
			}
		}
	}
	// each loop pop out an element in the fanoutObjective_ list
	while (!fanoutObjective_.empty())
	{
		Gate *pGate = &pCircuit_->gates_[vecPop(fanoutObjective_)];
		// if the gate's value is D set to H, D' set to L
		if (pGate->atpgVal_ == D)
		{
			pGate->atpgVal_ = H;
		}
		else if (pGate->atpgVal_ == B)
		{
			pGate->atpgVal_ = L;
		}

		if (!(pGate->gateType_ == Gate::PI || pGate->gateType_ == Gate::PPI || pGate->atpgVal_ == X)) // if the gate's value not unknown and the gates type not PI or PPI
		{
			fanoutFreeBacktrace(pGate);
		}
	}
}

int Atpg::countNumGatesInDFrontier(Gate *pFaultyLine)
{
	// update the frontier
	updateDFrontier();

	// Change the xPathStatus from XPATH_EXIST to UNKNOWN of a gate
	// which has equal or higher level than the faulty gate
	//(the gate array has been levelized already)
	// This is to clear the xPathStatus of previous xPathTracing
	for (int i = pFaultyLine->gateId_; i < pCircuit_->tgate_; ++i)
	{
		Gate *pGate = &pCircuit_->gates_[i];
		if (gateID_to_xPathStatus_[pGate->gateId_] == XPATH_EXIST)
		{
			gateID_to_xPathStatus_[pGate->gateId_] = UNKNOWN;
		}
	}

	// if D-frontier can't propagate to the PO, erase it
	for (int k = dFrontier_.size() - 1; k >= 0; --k)
	{
		if (!xPathTracing(&pCircuit_->gates_[dFrontier_[k]]))
		{
			vecDelete(dFrontier_, k);
		}
	}
	return dFrontier_.size();
}

// **************************************************************************
// Function   [ Atpg::uniquePathSensitize ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Finds the last gate(pnextgate) in the uniquepath starts from pgate,
//                     return BackImpLevel which is the max of the pnextgate's input level.
//                     BackImpLevel is -1 if no uniquepath.
//              in:    gate
//              out:   int(BackImpLevel)
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
int Atpg::uniquePathSensitize(Gate &gate)
{
	int BackImpLevel = NO_UNIQUE_PATH;
	Gate *pFaultyGate = &pCircuit_->gates_[currentTargetFault_.gateID_];

	// if gate is not the FaultyGate
	if (pFaultyGate->gateId_ != gate.gateId_)
	{
		Value NonControlVal = gate.getInputNonCtrlValue();
		// if gate has an NonControlVal (1 or 0)
		if (NonControlVal != X)
		{
			for (int i = 0; i < gate.numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[gate.faninVector_[i]];
				if (pFaninGate->atpgVal_ == X)
				{
					pFaninGate->atpgVal_ = NonControlVal;
					if (BackImpLevel < pFaninGate->numLevel_) // BackImpLevel becomes MAX of fan in level
					{
						BackImpLevel = pFaninGate->numLevel_;
					}

					backtrackList_.push_back(pFaninGate->gateId_);
					// pushInputEvents(gate.gateId_, i); replaced by wang
					pushGateToEventStack(gate.faninVector_[i]);
					pushGateFanoutsToEventStack(gate.faninVector_[i]);
				}
				else if (pFaninGate->atpgVal_ == gate.getInputCtrlValue())
				{
					return UNIQUE_PATH_SENSITIZE_FAIL;
				}
			}
		}
	}

	Gate *pCurrGate = &gate;
	Gate *pNextGate = NULL;

	while (true)
	{
		std::vector<int> &UniquePathList = gateID_to_uniquePath_[pCurrGate->gateId_];

		// If pCurrGate is PO or PPO, break.
		if (pCurrGate->gateType_ == Gate::PO || pCurrGate->gateType_ == Gate::PPO)
		{
			break;
		}
		else if (pCurrGate->numFO_ == 1) // If pCurrGate is fanout free, set pNextGate to its output gate.
		{
			pNextGate = &pCircuit_->gates_[pCurrGate->fanoutVector_[0]];
		}
		else if (UniquePathList.size() == 0) // If pCurrGate no UniquePath, break.
		{
			break;
		}
		else
		{
			pNextGate = &pCircuit_->gates_[UniquePathList[0]]; // set pNextGate to UniquePathList[0].
		}

		Value NonControlVal = pNextGate->getInputNonCtrlValue();

		if (NonControlVal != X && !pNextGate->isUnary())
		{ // If pNextGate is Unary and its NonControlVal is not unknown.
			if (pCurrGate->numFO_ == 1)
			{
				// If gCurrGate(pGate) is fanout free, pNextGate is gCurrGate's output gate.
				// Go through all pNextGate's input.
				for (int i = 0; i < pNextGate->numFI_; ++i)
				{
					Gate *pFaninGate = &pCircuit_->gates_[pNextGate->faninVector_[i]];

					if (pFaninGate != pCurrGate && pNextGate->getInputCtrlValue() != X && pFaninGate->atpgVal_ == pNextGate->getInputCtrlValue())
					{
						return UNIQUE_PATH_SENSITIZE_FAIL;
					}

					if (pFaninGate != pCurrGate && pFaninGate->atpgVal_ == X)
					{
						pFaninGate->atpgVal_ = NonControlVal; // Set input gate of pNextGate to pNextGate's NonControlVal
						if (BackImpLevel < pFaninGate->numLevel_)
						{
							BackImpLevel = pFaninGate->numLevel_;
						}
						// BackImpLevel becomes MAX of pNextGate's fanin level
						backtrackList_.push_back(pFaninGate->gateId_);
						// pushInputEvents(pNextGate->gateId_, i); replaced by wang
						pushGateToEventStack(pNextGate->faninVector_[i]);
						pushGateFanoutsToEventStack(pNextGate->faninVector_[i]);
					}
				}
			}
			else
			{
				// gCurrGate(pGate) is not fanout free, pNextGate is UniquePathList[0].
				bool DependOnCurrent;
				for (int i = 0; i < pNextGate->numFI_; ++i)
				{
					// Go through all pNextGate's input.
					Gate *pFaninGate = &pCircuit_->gates_[pNextGate->faninVector_[i]];
					DependOnCurrent = false;

					for (int j = 1; j < UniquePathList.size(); ++j)
					{
						if (UniquePathList[j] == pFaninGate->gateId_)
						{
							DependOnCurrent = true;
							break;
						}
					}

					if (!DependOnCurrent)
					{
						if (pFaninGate->atpgVal_ != X && pFaninGate->atpgVal_ == pNextGate->getInputCtrlValue() && pNextGate->getInputCtrlValue() != X)
						{
							return UNIQUE_PATH_SENSITIZE_FAIL;
						}

						if (pFaninGate->atpgVal_ != X)
						{
							continue;
						}

						pFaninGate->atpgVal_ = NonControlVal; // set to NonControlVal

						if (BackImpLevel < pFaninGate->numLevel_)
						{
							BackImpLevel = pFaninGate->numLevel_; // BackImpLevel becomes MAX of pNextGate's fanin level
						}
						backtrackList_.push_back(pFaninGate->gateId_);
						// pushInputEvents(pNextGate->gateId_, i); replaced by wang
						pushGateToEventStack(pNextGate->faninVector_[i]);
						pushGateFanoutsToEventStack(pNextGate->faninVector_[i]);
					}
				}
			}
		}
		pCurrGate = pNextGate; // move to last gate in gateID_to_uniquePath_
	}
	return BackImpLevel;
}

// **************************************************************************
// Function   [ Atpg::isExistXPath ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Used before generateSinglePatternOnTargetFault
//                Return true if there is X-path.
//                Otherwise, return false.
//              in:    Gate pointer
//              out:   possible or not
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
bool Atpg::isExistXPath(Gate *pGate)
{
	// Clear the gateID_to_xPathStatus_ from target gate to PO/PPO
	// TODO: This part can be implemented by event-driven method
	for (int i = pGate->gateId_; i < pCircuit_->tgate_; ++i)
	{
		gateID_to_xPathStatus_[i] = UNKNOWN;
	}
	return xPathTracing(pGate);
}

// **************************************************************************
// Function   [ Atpg::xPathTracing ]
// Commentor  [ CLT ]
// Synopsis   [ usage: recursive call the fanout of pGate to PO and check if pGate has a X path
//              in:    Gate* pGate
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
bool Atpg::xPathTracing(Gate *pGate)
{
	if (pGate->atpgVal_ != X || gateID_to_xPathStatus_[pGate->gateId_] == NO_XPATH_EXIST)
	{
		gateID_to_xPathStatus_[pGate->gateId_] = NO_XPATH_EXIST;
		return false;
	}

	if (gateID_to_xPathStatus_[pGate->gateId_] == XPATH_EXIST)
	{
		return true;
	}

	if (pGate->gateType_ == Gate::PO || pGate->gateType_ == Gate::PPO)
	{
		gateID_to_xPathStatus_[pGate->gateId_] = XPATH_EXIST;
		return true;
	}

	for (int i = 0; i < pGate->numFO_; ++i)
	{
		// TO-DO homework 03
		if (xPathTracing(&(pCircuit_->gates_[pGate->fanoutVector_[i]])))
		{
			gateID_to_xPathStatus_[pGate->gateId_] = XPATH_EXIST;
			return true;
		}
		// end of TO-DO
	}
	gateID_to_xPathStatus_[pGate->gateId_] = NO_XPATH_EXIST;
	return false;
}

// **************************************************************************
// Function   [ Atpg::setFaultyGate ]
// Commentor  [ WYH ]
// Synopsis   [ usage: Initial assignment of fault signal.
//                     There are two situations :
//                     1. Fault is on the input line of pFaultyGate, and
//                        pFaultyLine is the fanin gate of pFaultyGate
//                       (1) Activate the fault, and set value of pFaultyLine
//                           according to fault type.
//                       (2) According to the type of pFaultyGate, set other fanin
//                           gate of pFaultyGate to NoneControlling value of
//                           pFaultyGate, and set value of pFaultyGate.
//                       (3) Schedule all fanout gates of fanin gates of pFaultyGate,
//                           and schedule fanout gates of pFaultyGate.
//                       (4) Update BackImpLevel to be maximum level of
//                           fanin gates of pFaultyGate.
//                     2. Fault is on the ouput line of pFaultyGate, and
//                        pFaultyLine is pFaultyGate.
//                       (1) Activate the fault, and set value of pFaultyLine
//                           according to fault type.
//                       (2) Schedule fanout gates of pFaultyGate.
//                       (3) If pFaultyGate is a HEADLINE, all it's fanin gates are
//                           FREELINE, no need to set value.
//                           Else, set the value of it's fanin gates, and schedule
//                           all fanout gates of fanin gates of pFaultyGate.
//                       (4) Update BackImpLevel to be maximum level of
//                           fanin gates of pFaultyGate.
//              in:    Fault& fault
//              out:   int BackImpLevel: The BackImpLevel indicates the backward
//                     imply level return -1 when fault FAULT_UNTESTABLE
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/16 ]
// **************************************************************************
int Atpg::setFaultyGate(Fault &fault)
{
	int BackImpLevel = 0;
	Gate *pFaultyGate = NULL; // pFaultyLine is the gate before the fault (fanin gate of pFaultyGate)
	Gate *pFaultyLine = NULL;
	bool isOutputFault = (fault.faultyLine_ == 0); // if the fault is SA0 or STR, then set faulty value to D (1/0)
	Value vtemp;
	Value FaultyValue = (fault.faultType_ == Fault::SA0 || fault.faultType_ == Fault::STR) ? D : B;

	pFaultyGate = &pCircuit_->gates_[fault.gateID_];
	// if the fault is input fault, the pFaultyLine is decided by fault.faultyLine_
	// else pFaultyLine is pFaultyGate
	if (!isOutputFault)
	{
		pFaultyLine = &pCircuit_->gates_[pFaultyGate->faninVector_[fault.faultyLine_ - 1]];
	}
	else
	{
		pFaultyLine = pFaultyGate;
	}

	if (!isOutputFault)
	{
		if (FaultyValue == D && pFaultyLine->atpgVal_ != L)
		{
			pFaultyLine->atpgVal_ = H;
		}
		else if (FaultyValue == B && pFaultyLine->atpgVal_ != H)
		{
			pFaultyLine->atpgVal_ = L;
		}
		else
		{ // The fault can not propagate to PO
			return -1;
		}

		backtrackList_.push_back(pFaultyLine->gateId_);

		if (pFaultyGate->gateType_ == Gate::AND2 || pFaultyGate->gateType_ == Gate::AND3 || pFaultyGate->gateType_ == Gate::AND4 || pFaultyGate->gateType_ == Gate::NAND2 || pFaultyGate->gateType_ == Gate::NAND3 || pFaultyGate->gateType_ == Gate::NAND4 || pFaultyGate->gateType_ == Gate::NOR2 || pFaultyGate->gateType_ == Gate::NOR3 || pFaultyGate->gateType_ == Gate::NOR4 || pFaultyGate->gateType_ == Gate::OR2 || pFaultyGate->gateType_ == Gate::OR3 || pFaultyGate->gateType_ == Gate::OR4)
		{
			// scan all fanin gate of pFaultyGate
			bool isFaultyGateScanned = false;
			for (int i = 0; i < pFaultyGate->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pFaultyGate->faninVector_[i]];
				if (pFaninGate != pFaultyLine)
				{
					if (pFaninGate->atpgVal_ == X)
					{
						pFaninGate->atpgVal_ = pFaultyGate->getInputNonCtrlValue();
						backtrackList_.push_back(pFaninGate->gateId_);
					}
					else if (pFaninGate->atpgVal_ != pFaultyGate->getInputNonCtrlValue())
					{
						// If the value has already been set, it should be
						// noncontrol value, otherwise the fault can't propagate
						return -1;
					}
				}
				else
				{
					if (isFaultyGateScanned == false)
					{
						isFaultyGateScanned = true;
					}
					else
					{
						return -1; // FAULT_UNTESTABLE FAULT
					}
				}
			}
			vtemp = pFaultyGate->isInverse();
			// find the pFaultyGate output value
			pFaultyGate->atpgVal_ = cXOR2(vtemp, FaultyValue);
			backtrackList_.push_back(pFaultyGate->gateId_);
		}
		else if (pFaultyGate->gateType_ == Gate::INV || pFaultyGate->gateType_ == Gate::BUF || pFaultyGate->gateType_ == Gate::PO || pFaultyGate->gateType_ == Gate::PPO)
		{
			vtemp = pFaultyGate->isInverse();
			pFaultyGate->atpgVal_ = cXOR2(vtemp, FaultyValue);
			backtrackList_.push_back(pFaultyGate->gateId_);
		}

		if (pFaultyGate->atpgVal_ != X)
		{
			// schedule all the fanout gate of pFaultyGate
			pushGateFanoutsToEventStack(pFaultyGate->gateId_);
			gateID_to_valModified_[pFaultyGate->gateId_] = 1;
		}

		for (int i = 0; i < pFaultyGate->numFI_; ++i)
		{
			Gate *pFaninGate = &pCircuit_->gates_[pFaultyGate->faninVector_[i]];
			if (pFaninGate->atpgVal_ != X)
			{
				// set the BackImpLevel to be maximum of fanin gate's level
				if (BackImpLevel < pFaninGate->numLevel_)
				{
					BackImpLevel = pFaninGate->numLevel_;
				}
				// shedule all fanout gates of the fanin gate
				// pushInputEvents(pFaultyGate->gateId_, i); replaced by folloing, by wang
				pushGateToEventStack(pFaultyGate->faninVector_[i]);
				pushGateFanoutsToEventStack(pFaultyGate->faninVector_[i]);
			}
		}
	}
	else
	{ // output fault
		if ((FaultyValue == D && pFaultyGate->atpgVal_ == L) || (FaultyValue == B && pFaultyGate->atpgVal_ == H))
		{
			return -1;
		}
		pFaultyGate->atpgVal_ = FaultyValue;
		backtrackList_.push_back(pFaultyGate->gateId_);
		// schedule all of fanout gate of the pFaultyGate
		pushGateFanoutsToEventStack(pFaultyGate->gateId_);
		// backtrace stops at HEAD LINE
		if (gateID_to_lineType_[pFaultyGate->gateId_] == HEAD_LINE)
		{
			gateID_to_valModified_[pFaultyGate->gateId_] = 1;
		}
		else if (pFaultyGate->gateType_ == Gate::INV || pFaultyGate->gateType_ == Gate::BUF || pFaultyGate->gateType_ == Gate::PO || pFaultyGate->gateType_ == Gate::PPO)
		{
			Gate *pFaninGate = &pCircuit_->gates_[pFaultyGate->faninVector_[0]];
			gateID_to_valModified_[pFaultyGate->gateId_] = 1;

			Value Val = FaultyValue == D ? H : L;
			vtemp = pFaultyGate->isInverse();
			pFaninGate->atpgVal_ = cXOR2(vtemp, Val);
			backtrackList_.push_back(pFaninGate->gateId_);
			// pushInputEvents(pFaultyGate->gateId_, 0); replaced by following 2 func, by wang
			pushGateToEventStack(pFaultyGate->faninVector_[0]);
			pushGateFanoutsToEventStack(pFaultyGate->faninVector_[0]);

			if (BackImpLevel < pFaninGate->numLevel_)
			{
				BackImpLevel = pFaninGate->numLevel_;
			}
		}
		else if ((FaultyValue == D && pFaultyGate->getOutputCtrlValue() == H) || (FaultyValue == B && pFaultyGate->getOutputCtrlValue() == L))
		{
			gateID_to_valModified_[pFaultyGate->gateId_] = 1;
			// scan all fanin gate of pFaultyGate
			for (int i = 0; i < pFaultyGate->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pFaultyGate->faninVector_[i]];
				if (pFaninGate->atpgVal_ == X)
				{
					pFaninGate->atpgVal_ = pFaultyGate->getInputNonCtrlValue();
					backtrackList_.push_back(pFaninGate->gateId_);
					// shedule all fanout gate of the pFaninGate
					// pushInputEvents(pFaultyGate->gateId_, i); removed by wang
					pushGateToEventStack(pFaultyGate->faninVector_[i]);
					pushGateFanoutsToEventStack(pFaultyGate->faninVector_[i]);
					if (BackImpLevel < pFaninGate->numLevel_)
					{
						BackImpLevel = pFaninGate->numLevel_;
					}
				}
			}
		}
		else
		{
			pushGateToEventStack(pFaultyGate->gateId_);
			if (BackImpLevel < pFaultyGate->numLevel_)
			{
				BackImpLevel = pFaultyGate->numLevel_;
			}
		}
	}
	return BackImpLevel;
}

// **************************************************************************
// Function   [ Atpg::setFreeFaultyGate ]
// Commentor  [ WYH ]
// Synopsis   [ usage: This function is called when pFaultyLine is FREELINE.
//                     That means it has only one output gate.
//                     The fault must be on the output line of pFaultyGate.
//                     This function first sets the value of pFaultyGate, and
//                     pushes output gate pFaultyGate to BacktraceList.
//                     In the while loop, sets unknown fanin gate of pFaultyGate
//                     to noncontrol value of pFaultyGate and sets the value of
//                     pFaultyGate.
//                     The loop breaks when pFaultyGate becomes a HEADLINE.
//                     When pFaultyGate is a HEADLINE, this function schedules
//                     all fanout gate of pFaultyGate, and decides the new
//                     fault type according to the value of pFaultyGate
//                     and returns the new fault.
//              in:    Gate& gate
//              out:   Fault
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/17 ]
// **************************************************************************
Fault Atpg::setFreeFaultyGate(Gate &gate)
{
	int gateID = 0;
	Gate *pCurrentGate = NULL;
	std::vector<int> sStack;
	sStack.push_back(gate.fanoutVector_[0]);

	while (!sStack.empty())
	{
		gateID = vecPop(sStack);
		pCurrentGate = &pCircuit_->gates_[gateID];
		Value Val = pCurrentGate->getInputNonCtrlValue();
		if (Val == X)
		{
			Val = L;
		}
		for (int i = 0; i < pCurrentGate->numFI_; ++i)
		{
			Gate &gFanin = pCircuit_->gates_[pCurrentGate->faninVector_[i]];
			// set pCurrentGate unknown fanin gate to noncontrol value
			if (gFanin.atpgVal_ == X)
			{
				gFanin.atpgVal_ = Val;
				backtrackList_.push_back(gFanin.gateId_);
			}
		}

		if (pCurrentGate->atpgVal_ == X)
		{
			// set the value of pCurrentGate by evaluateGoodVal
			pCurrentGate->atpgVal_ = evaluateGoodVal(*pCurrentGate);
			backtrackList_.push_back(pCurrentGate->gateId_);
		}
		// if the pCurrentGate is FREE LINE, pCurrentGate output gate becomes a new pCurrentGate
		// until pCurrentGate becomes HEAD LINE
		if (gateID_to_lineType_[gateID] == FREE_LINE)
		{
			sStack.push_back(pCurrentGate->fanoutVector_[0]);
		}
	}
	gateID_to_valModified_[gateID] = 1;
	pushGateFanoutsToEventStack(gateID);
	// decide the new fault type according to pCurrentGate value
	return Fault(gateID, pCurrentGate->atpgVal_ == D ? Fault::SA0 : Fault::SA1, 0);
}

// **************************************************************************
// Function   [ Atpg::fanoutFreeBacktrace ]
// Commentor  [ CLT ]
// Synopsis   [ usage: Backtrace in Fanout Free situation
//              in:    the gate to start performing
//              out:   void
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
void Atpg::fanoutFreeBacktrace(Gate *pObjectGate)
{
	currentObject_.clear();
	currentObject_.reserve(MAX_LIST_SIZE);
	currentObject_.push_back(pObjectGate->gateId_);
	// clear the currentObject_ list, push pObjectGate in currentObject_

	while (!currentObject_.empty())
	{
		Gate *pGate = &pCircuit_->gates_[vecPop(currentObject_)];

		if (pGate->gateType_ == Gate::PI || pGate->gateType_ == Gate::PPI || pGate == firstTimeFrameHeadLine_)
		{
			continue;
		}

		Value vinv = pGate->isInverse();

		if (pGate->gateType_ == Gate::XOR2 || pGate->gateType_ == Gate::XNOR2)
		{
			if (&pCircuit_->gates_[pGate->faninVector_[1]] != firstTimeFrameHeadLine_)
			{
				if (pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ == X)
				{
					pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ = L;
				}
				pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_ = cXOR3(pGate->atpgVal_, pGate->isInverse(), pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_);
			}
			else
			{
				pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ = cXOR3(pGate->atpgVal_, pGate->isInverse(), pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_);
			}
			currentObject_.push_back(pGate->faninVector_[0]);
			currentObject_.push_back(pGate->faninVector_[1]); // push both input gates into currentObject_ list
		}
		else if (pGate->gateType_ == Gate::XOR3 || pGate->gateType_ == Gate::XNOR3) // debugged by wang
		{
			if (&pCircuit_->gates_[pGate->faninVector_[1]] != firstTimeFrameHeadLine_)
			{
				if (pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ == X)
				{
					pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ = L;
				}
				if (pCircuit_->gates_[pGate->faninVector_[2]].atpgVal_ == X)
				{
					pCircuit_->gates_[pGate->faninVector_[2]].atpgVal_ = L;
				}
				pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_ = cXOR3(cXOR2(pGate->atpgVal_, pGate->isInverse()), pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_, pCircuit_->gates_[pGate->faninVector_[2]].atpgVal_);
			}
			else
			{
				pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ = L;
				pCircuit_->gates_[pGate->faninVector_[2]].atpgVal_ = cXOR3(cXOR2(pGate->atpgVal_, pGate->isInverse()), pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_, pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_);
			}
			currentObject_.push_back(pGate->faninVector_[0]);
			currentObject_.push_back(pGate->faninVector_[1]);
			currentObject_.push_back(pGate->faninVector_[2]);
		}
		else if (pGate->isUnary())
		{ // if pGate only have one input gate
			if (&pCircuit_->gates_[pGate->faninVector_[0]] != firstTimeFrameHeadLine_)
			{
				pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_ = cXOR2(pGate->atpgVal_, vinv);
			}
			currentObject_.push_back(pGate->faninVector_[0]); // add input gate into currentObject_ list
		}
		else
		{
			Value Val = cXOR2(pGate->atpgVal_, vinv);
			if (Val == pGate->getInputCtrlValue())
			{
				Gate *pMinLevelGate = &pCircuit_->gates_[pGate->fiMinLvl_];
				if (pMinLevelGate != firstTimeFrameHeadLine_)
				{
					pMinLevelGate->atpgVal_ = Val;
					currentObject_.push_back(pMinLevelGate->gateId_);
				}
				else
				{
					Gate *pFaninGate = NULL;
					for (int i = 0; i < pGate->numFI_; ++i)
					{
						pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
						if (pFaninGate != firstTimeFrameHeadLine_)
						{
							break;
						}
					}
					pFaninGate->atpgVal_ = Val;
					currentObject_.push_back(pFaninGate->gateId_);
				}
			}
			else
			{
				Gate *pFaninGate = NULL;
				for (int i = 0; i < pGate->numFI_; ++i)
				{
					pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
					if (pFaninGate->atpgVal_ == X)
					{
						pFaninGate->atpgVal_ = Val;
					}
					currentObject_.push_back(pFaninGate->gateId_);
				}
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::multipleBacktrace ]
// Commentor  [ CKY ]
// Synopsis   [ usage: return NO_CONTRADICTORY  or  CONTRADICTORY after backtrace
//                     see paper P.4 P.5 and Fig.8 for detail information
//              in:    BACKTRACE_STATUS
//                       it have 2 possibilities,
//                       atpgStatus == INITIAL means Multiple Backtrace from the
//                        set of initial objectives
//                       atpgStatus == FAN_OBJ_DETERMINE means Multiple Backtrace
//                        from the set of Fanout-Point Objectives
//              out:   BACKTRACE_RESULT
//                       return CONTRADICTORY when find a
//                        Fanout-Point Objective is not reachable from the fault
//                        line and n0, n1 of it are both nonezero;
//                        otherwise, return NO_CONTRADICTORY
//                       n0 is the number of times objective 0 is required,
//                       n1 is the number of times objective 1 is required
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
Atpg::BACKTRACE_RESULT Atpg::multipleBacktrace(BACKTRACE_STATUS atpgStatus, int &finalObjectiveId)
{
	int index;
	int n0, n1, nn1, nn0;
	Gate *pCurrentObj = NULL;

	// atpgStatus may be INITIAL, CHECK_AND_SELECT, CURRENT_OBJ_DETERMINE, FAN_OBJ_DETERMINE
	if (atpgStatus == INITIAL)
	{
		// LET THE SET OF INITIAL OBJECTIVES BE THE SET OF CURRENT OBJECTIVES
		initialObjectives();
		atpgStatus = CHECK_AND_SELECT; // ready to get into while loop
	}

	while (true)
	{
		Gate *pEasiestInput = NULL;
		switch (atpgStatus)
		{
			case CHECK_AND_SELECT:
				// IS THE SET OF CURRENT OBJECTIVES EMPTY?
				if (currentObject_.empty())
				{ // YES
					// IS THE SET OF FANOUT-POINT OBJECTIVES EMPTY?
					if (fanoutObjective_.empty()) // YES
					{															// C, NO_CONTRADICTORY
						return NO_CONTRADICTORY;
					}
					else // NO
					{
						atpgStatus = FAN_OBJ_DETERMINE;
					}
				}
				else
				{ // NO
					// TAKE OUT A CURRENT OBJECTIVE
					pCurrentObj = &pCircuit_->gates_[vecPop(currentObject_)];
					atpgStatus = CURRENT_OBJ_DETERMINE;
				}
				break; // switch break
			case CURRENT_OBJ_DETERMINE:
				// IS OBJECTIVE LINE A HEAD LINE?
				if (gateID_to_lineType_[pCurrentObj->gateId_] == HEAD_LINE)
				{ // YES
					// ADD THE CURRENT OBJECTIVE TO THE SET OF HEAD OBJECTIVES
					headObject_.push_back(pCurrentObj->gateId_);
				}
				else
				{ // NO
					// assignBacktraceValue() declared in <Gate.h>, defined in <atpg.cpp>
					// store m_NumOfZero, m_NumOfOne in n0 or n1, depend on
					// different Gate Type
					// different Gate return different value, the value used in FindEasiestInput()
					Value Val = assignBacktraceValue(n0, n1, *pCurrentObj); // get Val,no and n1 of pCurrentObj

					// FindEasiestInput() in <atpg.cpp>,
					// return Gate* of the pCurrentObj's fanin gate that is
					// the easiest gate to control value to Val
					pEasiestInput = findEasiestInput(pCurrentObj, Val);

					// add next objectives to current objectives and
					// calculate n0, n1 by rule1~rule6
					// rule1~rule6:[Reference paper p4, p5 Fig6
					for (int i = 0; i < pCurrentObj->numFI_; ++i)
					{
						// go through all fan in gate of pCurrentObj
						Gate *pFaninGate = &pCircuit_->gates_[pCurrentObj->faninVector_[i]];

						// ignore the fanin gate that already set value
						// (not unknown)
						if (pFaninGate->atpgVal_ != X)
						{
							continue;
						}

						// DOES OBJECTIVE LINE FOLLOW A FANOUT-POINT?
						if (pFaninGate->numFO_ > 1)
						{ // YES
							// rule 1 ~ rule 5
							if (pFaninGate == pEasiestInput)
							{
								nn0 = n0;
								nn1 = n1;
							}
							else if (Val == L)
							{
								nn0 = 0;
								nn1 = n1;
							}
							else if (Val == H)
							{
								nn0 = n0;
								nn1 = 0;
							}
							else
							{
								if (n0 > n1)
								{
									nn0 = n0;
									nn1 = n1;
								}
								else
								{
									nn0 = n1;
									nn1 = n0;
								}
							}

							if (nn0 > 0 || nn1 > 0)
							{
								// first find this fanout point,  add to
								// Fanout-Point Objectives set
								if (gateID_to_n0_[pFaninGate->gateId_] == 0 && gateID_to_n1_[pFaninGate->gateId_] == 0)
								{
									fanoutObjective_.push_back(pFaninGate->gateId_);
								}

								// new value = old value(gateID_to_n0_(),gateID_to_n1_()) + this branch's value (nn0, nn1)
								// rule6: fanout point's n0, n1 = sum of it's branch's no, n1
								// int NewZero = gateID_to_n1_[pFaninGate->gateId_] + nn0; removed by wang
								// int NewOne = gateID_to_n1_[pFaninGate->gateId_] + nn1; removed by wang
								// ADD n0 AND n1 TO THE CORRESPONDING
								// modified to safe
								setGaten0n1(pFaninGate->gateId_, gateID_to_n0_[pFaninGate->gateId_] + nn0, gateID_to_n1_[pFaninGate->gateId_] + nn1);
								backtraceResetList_.push_back(pFaninGate->gateId_);
							}
						}
						else
						{ // NO
							// rule 1 ~ rule 5
							if (pFaninGate == pEasiestInput)
							{
								nn0 = n0;
								nn1 = n1;
							}
							else if (Val == L)
							{
								nn0 = 0;
								nn1 = n1;
							}
							else if (Val == H)
							{
								nn0 = n0;
								nn1 = 0;
							}
							else
							{
								if (n0 > n1)
								{
									nn0 = n0;
									nn1 = n1;
								}
								else
								{
									nn0 = n1;
									nn1 = n0;
								}
							}

							if (nn0 > 0 || nn1 > 0)
							{
								// add gate into Current Objective set
								// BY THE RULES(1)-(5) DETERMINE NEXT OBJECTIVES
								// AND ADD THEM TO THE SET OF CURRENT OBJECTIVES
								setGaten0n1(pFaninGate->gateId_, nn0, nn1);
								backtraceResetList_.push_back(pFaninGate->gateId_);
								currentObject_.push_back(pFaninGate->gateId_);
							}
						}
					}
				}
				atpgStatus = CHECK_AND_SELECT;
				break; // switch break

			case FAN_OBJ_DETERMINE:
				// TAKE OUT A FANOUT-POINT OBJECTIVE p CLOSEST TO A PRIMARY OUTPUT
				pCurrentObj = findCloseToOutput(fanoutObjective_, index);

				// specified by the index from FanObject
				vecDelete(fanoutObjective_, index);

				// if value of pCurrent is not X
				// ignore the Fanout-Point Objective that already set value
				// (not unknown), back to CHECK_AND_SELECT state
				if (pCurrentObj->atpgVal_ != X)
				{
					atpgStatus = CHECK_AND_SELECT;
					break; // switch break
				}

				// if fault reach of pCurrent is equal to 1 => reachable
				// IS p REACHABLE FROM THE FAULT LINE?
				if (gateID_to_reachableByTargetFault_[pCurrentObj->gateId_] == 1)
				{ // YES
					atpgStatus = CURRENT_OBJ_DETERMINE;
					break; // switch break
				}

				// if one of numOfZero or numOfOne is equal to 0
				if (!(gateID_to_n0_[pCurrentObj->gateId_] != 0 && gateID_to_n1_[pCurrentObj->gateId_] != 0))
				{
					atpgStatus = CURRENT_OBJ_DETERMINE;
					break; // switch break
				}

				// if three conditions are not set up, then push back pCurrentObj to finalObject
				// then terminate Multiple Backtrace procedure
				// when "a Fanout-Point objective is not reachable from the
				// fault line" and "both no, n1 of it are nonzero "
				// return CONTRADICTORY in this situation
				// Let the Fanout-Point objective to be a Final Objective
				finalObjectiveId = pCurrentObj->gateId_;
				return CONTRADICTORY;

			default:
				break;
		}
	}
	// after breaking while loop
	return NO_CONTRADICTORY;
}

// **************************************************************************
// Function   [ Atpg::assignBacktraceValue ]
// Commentor  [ CKY ]
// Synopsis   [ usage: help to get n0 n1 and Value depend on Gate's controlling value
//              in:    n0 (int reference), n1 (int reference), gate (Gate reference)
//              out:   Value, n0, n1
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
Value Atpg::assignBacktraceValue(int &n0, int &n1, Gate &gate)
{
	int v1;
	Value val;
	switch (gate.gateType_)
	{
		// when gate is AND type,n0 = numOfZero,n1 = numOfOne
		case Gate::AND2:
		case Gate::AND3:
		case Gate::AND4:
			n0 = gateID_to_n0_[gate.gateId_];
			n1 = gateID_to_n1_[gate.gateId_];
			return L;

			// when gate is OR type,n0 = numOfZero,n1 = numOfOne
		case Gate::OR2:
		case Gate::OR3:
		case Gate::OR4:
			// TO-DO homework 04
			n0 = gateID_to_n0_[gate.gateId_];
			n1 = gateID_to_n1_[gate.gateId_];
			return H;
			// end of TO-DO

			// when gate is NAND type,n0 = numOfOne,n1 = numOfZero
		case Gate::NAND2:
		case Gate::NAND3:
		case Gate::NAND4:
			n0 = gateID_to_n1_[gate.gateId_];
			n1 = gateID_to_n0_[gate.gateId_];
			return L;

			// when gate is NOR type,n0 = numOfOne,n1 = numOfZero
		case Gate::NOR2:
		case Gate::NOR3:
		case Gate::NOR4:
			// TO-DO homework 04
			n0 = gateID_to_n1_[gate.gateId_];
			n1 = gateID_to_n0_[gate.gateId_];
			return H;
			// end of TO-DO

			// when gate is inverter,n0 = numOfOne,n1 = numOfZero
		case Gate::INV:
			n0 = gateID_to_n1_[gate.gateId_];
			n1 = gateID_to_n0_[gate.gateId_];
			return X;

			// when gate is XOR2 or XNOR2
		case Gate::XOR2:
		case Gate::XNOR2:
			val = pCircuit_->gates_[gate.faninVector_[0]].atpgVal_;
			if (val == X)
			{
				val = pCircuit_->gates_[gate.faninVector_[1]].atpgVal_;
			}

			if (val == H)
			{
				n0 = gateID_to_n1_[gate.gateId_];
				n1 = gateID_to_n0_[gate.gateId_];
			}
			else
			{
				n0 = gateID_to_n0_[gate.gateId_];
				n1 = gateID_to_n1_[gate.gateId_];
			}

			if (gate.gateType_ == Gate::XNOR2)
			{
				int temp = n0;
				n0 = n1;
				n1 = temp;
			}
			return X;

			// when gate is XOR3 or XNOR3
		case Gate::XOR3:
		case Gate::XNOR3:
			v1 = 0;
			for (int i = 0; i < gate.numFI_; ++i)
			{
				if (pCircuit_->gates_[gate.faninVector_[0]].atpgVal_ == H)
				{
					++v1;
				}
			}
			if (v1 == 2)
			{
				n0 = gateID_to_n1_[gate.gateId_];
				n1 = gateID_to_n0_[gate.gateId_];
			}
			else
			{
				n0 = gateID_to_n0_[gate.gateId_];
				n1 = gateID_to_n1_[gate.gateId_];
			}

			if (gate.gateType_ == Gate::XNOR3)
			{
				int temp = n0;
				n0 = n1;
				n1 = temp;
			}
			return X;
		default:
			n0 = gateID_to_n0_[gate.gateId_];
			n1 = gateID_to_n1_[gate.gateId_];
			return X;
	}
}

// **************************************************************************
// Function   [ Atpg::initialObjectives ]
// Commentor  [ CKY ]
// Synopsis   [ usage: initial all objects of LineNum(gateID_to_n0_  gateID_to_n1_)
//              in:    void
//              out:   void
//            ]
// Date       [ CKY Ver. 1.0 commented and finished 2013/08/17 ]
// **************************************************************************
void Atpg::initialObjectives()
{
	currentObject_ = initObject_; // vector assignment

	// go through all current Object size
	for (const int &currentObjectGateID : currentObject_)
	{
		// get currentObject gate in pCircuit_ to pGate
		Gate *pGate = &pCircuit_->gates_[currentObjectGateID];

		// if single value of the gate is Low or D', numOfZero=1, numOfOne=0
		if (pGate->atpgVal_ == L || pGate->atpgVal_ == B)
		{
			setGaten0n1(pGate->gateId_, 1, 0);
		}
		else if (pGate->atpgVal_ == H || pGate->atpgVal_ == D)
		{ // if single value of the gate is High or D, numOfZero=0, numOfOne=1
			setGaten0n1(pGate->gateId_, 0, 1);
		}
		else
		{ // if single value of the gate is X or Z or I, set line number depend on gate type
			switch (pGate->gateType_)
			{
				case Gate::AND2:
				case Gate::AND3:
				case Gate::AND4:
				case Gate::NOR2:
				case Gate::NOR3:
				case Gate::NOR4:
				case Gate::XNOR2:
				case Gate::XNOR3:
					setGaten0n1(pGate->gateId_, 0, 1);
					break;
				case Gate::OR2:
				case Gate::OR3:
				case Gate::OR4:
				case Gate::NAND2:
				case Gate::NAND3:
				case Gate::NAND4:
				case Gate::XOR2:
				case Gate::XOR3:
					setGaten0n1(pGate->gateId_, 1, 0);
					break;
				default:
					break;
			}
		}
		// record reset list
		backtraceResetList_.push_back(pGate->gateId_);
	}
}

// **************************************************************************
// Function   [ Atpg::findEasiestInput ]
// Commentor  [ KOREAL ]
// Synopsis   [ usage: find the EasiestInput by gate::cc0_ or gate::cc1_
//              in:    1. Target gate (Gate* pGate)
//					   2. FanOut value of target gate (Value Val)
//              out:   FanIn of a target gate which is the easiestInput
//            ]
// Date       [ CPJ Ver. 1.0 started 2013/08/10 ]
// **************************************************************************
Gate *Atpg::findEasiestInput(Gate *pGate, Value Val)
{
	// declaration of the return gate pointer
	Gate *pRetGate = NULL;
	// easiest input gate's scope(non-select yet)
	int easyControlVal = INFINITE;

	// if the fanIn amount is 1, just return the only fanIn
	if (pGate->gateType_ == Gate::PO || pGate->gateType_ == Gate::PPO ||
			pGate->gateType_ == Gate::BUF || pGate->gateType_ == Gate::INV)
	{
		return &pCircuit_->gates_[pGate->faninVector_[0]];
	}

	if (Val == L)
	{
		// choose the value-undetermined faninGate which has smallest cc0_
		for (int i = 0; i < pGate->numFI_; ++i)
		{
			Gate *pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
			if (pFaninGate->atpgVal_ != X)
			{
				continue;
			}

			if (pFaninGate->cc0_ < easyControlVal)
			{
				easyControlVal = pFaninGate->cc0_;
				pRetGate = pFaninGate;
			}
		}
	}
	else
	{
		// choose the value-undetermined faninGate which has smallest cc1_
		for (int i = 0; i < pGate->numFI_; ++i)
		{
			Gate *pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
			if (pFaninGate->atpgVal_ != X)
			{
				continue;
			}

			if (pFaninGate->cc1_ < easyControlVal)
			{
				easyControlVal = pFaninGate->cc1_;
				pRetGate = pFaninGate;
			}
		}
	}
	return pRetGate;
}

// **************************************************************************
// Function   [ Atpg::findCloseToOutput ]
// Commentor  [ CLT ]
// Synopsis   [ usage: find the gate which close to output
//              in:    the list to search, the index of the gate
//              out:   return the gate which is closest to output
//            ]
// Date       [ Ver. 1.0 started 2013/08/13 ]
// **************************************************************************
Gate *Atpg::findCloseToOutput(std::vector<int> &list, int &index)
{
	Gate *pCloseGate = NULL;

	if (list.empty())
	{
		return NULL;
	}

	pCloseGate = &pCircuit_->gates_[list.back()];
	index = list.size() - 1;
	for (int i = list.size() - 2; i >= 0; --i)
	{
		if (pCircuit_->gates_[list[i]].depthFromPo_ < pCloseGate->depthFromPo_)
		{
			index = i;
			pCloseGate = &pCircuit_->gates_[list[i]];
		}
	}
	return pCloseGate;
}

// **************************************************************************
// Function   [ Atpg::Evaluation ]
// Commentor  [ KOREAL ]
// Synopsis   [ IN:  Gate *pGate
//              OUT: IMP_STATUS (FORWARD, BACKWARD, CONFLICT)
//
//              If pGate is the faulty gate, return FaultEvaluation(pGate);
//              else check the relationships between pGate's evaluated value
//              and current value.
//
//              If they are the same, set pGate to be modefied, return
//              FORWARD,
//              else if current value is unknown, set it to the evaluated
//              value and return FORWARD,
//              else if the evaluated value is different from current value,
//              return CONFLICT,
//              else (only know current value)return
//              BackwardImplication(pGate).
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/15 ]
// **************************************************************************
Atpg::IMPLICATION_STATUS Atpg::evaluation(Gate *pGate)
{
	gateID_to_valModified_[pGate->gateId_] = 0;

	if (gateID_to_lineType_[pGate->gateId_] == HEAD_LINE)
	{
		// pGate is head line, set modify and return FORWARD
		gateID_to_valModified_[pGate->gateId_] = 1;
		return FORWARD;
	}

	// pGate is the faulty gate, see FaultEvaluation();
	if (pGate->gateId_ == currentTargetFault_.gateID_)
	{
		return faultyGateEvaluation(pGate);
	}

	// pGate is not the faulty gate, see evaluateGoodVal(*pGate)
	Value Val = evaluateGoodVal(*pGate);

	if (Val == pGate->atpgVal_)
	{
		if (Val != X)
		{ // Good value is equal to the gate output, return FORWARD
			gateID_to_valModified_[pGate->gateId_] = 1;
		}
		return FORWARD;
	}
	else if (pGate->atpgVal_ == X)
	{
		// set it to the evaluated value.
		pGate->atpgVal_ = Val;
		backtrackList_.push_back(pGate->gateId_);
		gateID_to_valModified_[pGate->gateId_] = 1;
		pushGateFanoutsToEventStack(pGate->gateId_);
		return FORWARD;
	}
	else if (Val != X)
	{ // Good value is different to the gate output, return CONFLICT
		return CONFLICT;
	}
	return backwardImplication(pGate);
}

// **************************************************************************
// Function   [ Atpg::FaultEvaluation ]
// Commentor  [ KOREAL ]
// Synopsis   [ IN:  Gate *pGate
//              OUT: IMP_STATUS (FORWARD, BACKWARD, CONFLICT)
//
//              Check the relationships between pGate's current value and the
//              evaluated value of pGate.
//
//              If current value of pGate is unknown, set it to the
//              evaluated value, return FORWARD.
//
//              If they are the same, set pGate to be modified, return FORWARD.
//
//              If pGate has current value, if only one input has an unknown
//              value, set the input to proper value and return BACKWARD; if
//              pGate has more than one input with unknown value, push pGate
//              into unjustified_ list.
//
//              If the evaluated value is different from current value, return
//              CONFLICT.
//
//            ]
// Date       [ KOREAL Ver. 1.0 started 2013/08/15 ]
// **************************************************************************
Atpg::IMPLICATION_STATUS Atpg::faultyGateEvaluation(Gate *pGate)
{
	// get the evaluated value of pGate.
	Value Val = evaluateFaultyVal(*pGate);
	int ImpPtr = 0;
	if (Val == X)
	{ // The evaluated value is X, means the init faulty objective has not achieved yet.
		if (pGate->atpgVal_ != X)
		{
			int NumOfX = 0;

			// Count NumOfX, the amount of fanIn whose value is X.
			// set ImpPtr to be one of the fanIn whose value is X.
			for (int i = 0; i < pGate->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pGate->faninVector_[i]];
				if (pFaninGate->atpgVal_ == X)
				{
					++NumOfX;
					ImpPtr = i;
				}
			}
			if (NumOfX == 1)
			{ // The fanin has X value can be set to impl. value

				// ImpVal is the pGate pImpGate value
				Value ImpVal;

				// pImpGate is the pGate's fanIn whose value is X.
				Gate *pImpGate = &pCircuit_->gates_[pGate->faninVector_[ImpPtr]];

				// set ImpVal if pGate is not XOR or XNOR
				if (pGate->atpgVal_ == D)
				{
					ImpVal = H;
				}
				else if (pGate->atpgVal_ == B)
				{
					ImpVal = L;
				}
				else
				{
					ImpVal = pGate->atpgVal_;
				}

				// set ImpVal if pGate is XOR2 or XNOR2
				if (pGate->gateType_ == Gate::XOR2 || pGate->gateType_ == Gate::XNOR2)
				{
					Value temp = (ImpPtr == 0) ? pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_ : pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_;
					ImpVal = cXOR2(ImpVal, temp);
				}
				// set ImpVal if pGate is XOR3 or XNOR3
				if (pGate->gateType_ == Gate::XOR3 || pGate->gateType_ == Gate::XNOR3)
				{
					Value temp;
					if (ImpPtr == 0)
					{
						temp = cXOR2(pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_, pCircuit_->gates_[pGate->faninVector_[2]].atpgVal_);
					}
					else if (ImpPtr == 1)
					{
						temp = cXOR2(pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_, pCircuit_->gates_[pGate->faninVector_[2]].atpgVal_);
					}
					else
					{
						temp = cXOR2(pCircuit_->gates_[pGate->faninVector_[1]].atpgVal_, pCircuit_->gates_[pGate->faninVector_[0]].atpgVal_);
					}
					ImpVal = cXOR2(ImpVal, temp);
				}

				// if pGate is an inverse function, ImpVal = NOT(ImpVal)
				Value isInv = pGate->isInverse();
				ImpVal = cXOR2(ImpVal, isInv);

				// set modify and the final value of pImpGate
				gateID_to_valModified_[pGate->gateId_] = 1;
				pImpGate->atpgVal_ = ImpVal;

				// backward setting
				// pushInputEvents(pGate->gateId_, ImpPtr);
				pushGateToEventStack(pGate->faninVector_[ImpPtr]);
				pushGateFanoutsToEventStack(pGate->faninVector_[ImpPtr]);
				backtrackList_.push_back(pImpGate->gateId_);

				return BACKWARD;
			}
			else
			{
				unjustified_.push_back(pGate->gateId_);
			}
		}
	}
	else if (pGate->atpgVal_ == Val)
	{ // The initial faulty objective has already been achieved
		gateID_to_valModified_[pGate->gateId_] = 1;
	}
	else if (pGate->atpgVal_ == X)
	{
		// if pGate's value is unknown, set pGate's value
		gateID_to_valModified_[pGate->gateId_] = 1;
		pGate->atpgVal_ = Val;
		// forward setting
		pushGateFanoutsToEventStack(pGate->gateId_);
		backtrackList_.push_back(pGate->gateId_);
	}
	else
	{
		return CONFLICT; // If the evaluated value is different from current value, return CONFLICT.
	}
	return FORWARD;
}

// **************************************************************************
// Function   [ Atpg::staticTestCompressionByReverseFaultSimulation ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Perform reverse fault simulation
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/08    last modified 2020/07/08 ]
// **************************************************************************
void Atpg::staticTestCompressionByReverseFaultSimulation(PatternProcessor *pPatternProcessor, FaultPtrList &originalFaultList)
{
	for (Fault *pFault : originalFaultList)
	{
		pFault->detection_ = 0;
		if (pFault->faultState_ == Fault::DT)
		{
			pFault->faultState_ = Fault::UD;
		}
	}

	std::vector<Pattern> tmp = pPatternProcessor->patternVector_;
	pPatternProcessor->patternVector_.clear();

	// Perform reverse fault simulation
	int leftFaultCount = originalFaultList.size();
	for (std::vector<Pattern>::reverse_iterator rit = tmp.rbegin(); rit != tmp.rend(); ++rit)
	{
		pSimulator_->pfFaultSim((*rit), originalFaultList);
		if (leftFaultCount > originalFaultList.size())
		{
			leftFaultCount = originalFaultList.size();
			pPatternProcessor->patternVector_.push_back((*rit));
		}
		else if (leftFaultCount < originalFaultList.size())
		{
			std::cerr << "staticTestCompressionByReverseFaultSimulation: unexpected behavior\n";
			std::cin.get();
		}
		// else
		// {
		// 	delete (*rit);
		// }
	}
}

// **************************************************************************
// Function   [ Atpg::firstTimeFrameSetUp ]
// Commentor  [ WYH ]
// Synopsis   [ usage: Initial assignment of fault signal, and set the first time
//                     meet HEAD LINE gate.
//                     There are two situations :
//                     1.Faulty gate is FREE LINE
//                      (1)Activate the fault, and set value of pFaultyLine
//                         according to fault type.
//                      (2)Do while loop until pFaultyLine becomes HEAD LINE
//                      (3)Schedule fanout gates of pFaultyLine.
//                     2.Fault is HEAD LINE or BOUND LINE
//                      (1)Activate the fault, and set value of pFaultyLine
//                         according to fault type.
//                      (2)Schedule fanout gates of pFaultyLine.
//                      (3) Update BackImpLevel to be maximum level of
//                          fanin gates of pFaultyLine.
//              in:    Fault& fault
//              out:   int BackImpLevel: The BackImpLevel indicates the backward
//                     imply level return -1 when fault FAULT_UNTESTABLE
//            ]
// Date       [ WYH Ver. 1.0 started 2013/08/17 ]
// **************************************************************************
int Atpg::firstTimeFrameSetUp(Fault &fault)
{ // NE
	int BackImpLevel = 0;
	Gate *pFaultyGate = NULL;
	Gate *pFaultyLine = NULL;
	bool isOutputFault = (fault.faultyLine_ == 0);
	Value FaultyValue = (fault.faultType_ == Fault::STR) ? L : H;
	Value vtemp;

	pFaultyGate = &pCircuit_->gates_[fault.gateID_ - pCircuit_->ngate_];

	if (!isOutputFault)
	{
		pFaultyLine = &pCircuit_->gates_[pFaultyGate->faninVector_[fault.faultyLine_ - 1]];
	}
	else
	{
		pFaultyLine = pFaultyGate;
	}

	if (gateID_to_lineType_[pFaultyLine->gateId_] == FREE_LINE)
	{
		if ((FaultyValue == H && pFaultyLine->atpgVal_ == L) || (FaultyValue == L && pFaultyLine->atpgVal_ == H))
		{
			return -1;
		}

		pFaultyLine->atpgVal_ = FaultyValue;
		backtrackList_.push_back(pFaultyLine->gateId_);
		fanoutFreeBacktrace(pFaultyLine);
		Gate *gTemp = pFaultyLine;
		Gate *gNext = NULL;
		// propagate until meet the other LINE
		do
		{
			gNext = &pCircuit_->gates_[gTemp->fanoutVector_[0]];
			if (!gNext->isUnary() && (gNext->getOutputCtrlValue() == X || gTemp->atpgVal_ != gNext->getInputCtrlValue()))
			{
				break;
			}
			gNext->atpgVal_ = cXOR2(gNext->isInverse(), gTemp->atpgVal_);
			gTemp = gNext;
		} while (gateID_to_lineType_[gTemp->gateId_] == FREE_LINE);

		if (gateID_to_lineType_[gTemp->gateId_] == HEAD_LINE)
		{
			gateID_to_valModified_[gTemp->gateId_] = 1;
			backtrackList_.push_back(gTemp->gateId_);
			pushGateFanoutsToEventStack(gTemp->gateId_);
		}
		firstTimeFrameHeadLine_ = gTemp;
	}
	else
	{
		if ((FaultyValue == H && pFaultyLine->atpgVal_ == L) || (FaultyValue == L && pFaultyLine->atpgVal_ == H))
		{
			return -1;
		}

		pFaultyLine->atpgVal_ = FaultyValue;
		backtrackList_.push_back(pFaultyLine->gateId_);
		pushGateFanoutsToEventStack(pFaultyLine->gateId_);

		if (gateID_to_lineType_[pFaultyLine->gateId_] == HEAD_LINE)
		{
			gateID_to_valModified_[pFaultyLine->gateId_] = 1;
		}
		else if (pFaultyLine->gateType_ == Gate::INV || pFaultyLine->gateType_ == Gate::BUF || pFaultyLine->gateType_ == Gate::PO || pFaultyLine->gateType_ == Gate::PPO)
		{
			Gate *pFaninGate = &pCircuit_->gates_[pFaultyLine->faninVector_[0]];
			gateID_to_valModified_[pFaultyLine->gateId_] = 1;

			Value Val = FaultyValue == H ? H : L;
			vtemp = pFaultyLine->isInverse();
			pFaninGate->atpgVal_ = cXOR2(vtemp, Val);
			backtrackList_.push_back(pFaninGate->gateId_);
			// pushInputEvents(pFaultyLine->gateId_, 0); replaced by folloing by wang
			pushGateToEventStack(pFaultyLine->faninVector_[0]);
			pushGateFanoutsToEventStack(pFaultyLine->faninVector_[0]);
			if (BackImpLevel < pFaninGate->numLevel_)
			{
				BackImpLevel = pFaninGate->numLevel_;
			}
		}
		else if ((FaultyValue == H && pFaultyLine->getOutputCtrlValue() == H) || (FaultyValue == L && pFaultyLine->getOutputCtrlValue() == L))
		{
			gateID_to_valModified_[pFaultyLine->gateId_] = 1;
			for (int i = 0; i < pFaultyLine->numFI_; ++i)
			{
				Gate *pFaninGate = &pCircuit_->gates_[pFaultyLine->faninVector_[i]];
				// if the value has not been set, then set it to noncontrol value
				if (pFaninGate->atpgVal_ == X)
				{
					pFaninGate->atpgVal_ = pFaultyLine->getInputNonCtrlValue();
					backtrackList_.push_back(pFaninGate->gateId_);
					// pushInputEvents(pFaultyLine->gateId_, i); replaced by folloing by wang
					pushGateToEventStack(pFaultyLine->faninVector_[i]);
					pushGateFanoutsToEventStack(pFaultyLine->faninVector_[i]);
					// set the BackImpLevel to be maximum of fanin gate's level
					if (BackImpLevel < pFaninGate->numLevel_)
					{
						BackImpLevel = pFaninGate->numLevel_;
					}
				}
			}
		}
		else
		{
			pushGateToEventStack(pFaultyLine->gateId_);
			if (BackImpLevel < pFaultyLine->numLevel_)
			{
				BackImpLevel = pFaultyLine->numLevel_;
			}
		}
	}
	return BackImpLevel;
}

// **************************************************************************
// Function   [ Atpg::checkLevelInfo ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                To check if the lvl_ of all the gates does not exceed pCircuit_->tlvl_
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::checkLevelInfo()
{
	for (int i = 0; i < pCircuit_->tgate_; ++i)
	{
		Gate &gate = pCircuit_->gates_[i];
		if (gate.numLevel_ >= pCircuit_->tlvl_)
		{
			std::cerr << "checkLevelInfo found that at least one gate.numLevel_ is greater than pCircuit_->tlvl_\n";
			std::cin.get();
		}
	}
}

// **************************************************************************
// Function   [ Atpg::getValStr ]
// Commentor  [ CAL ]
// Synopsis   [ usage: return string type of Value
//              in:    Value
//              out:   string of Value
//            ]
// Date       [ started 2020/07/04    last modified 2020/07/04 ]
// **************************************************************************
std::string Atpg::getValStr(Value val)
{
	std::string valStr;
	switch (val)
	{
		case H:
			valStr = "H";
			break;
		case L:
			valStr = "L";
			break;
		case D:
			valStr = "D";
			break;
		case B:
			valStr = "B";
			break;
		case X:
			valStr = "X";
			break;
		default:
			valStr = "Error";
			break;
	}
	return valStr;
}

// TODO comment by wang
void Atpg::calSCOAP()
{
	// cc0, cc1 and co default is 0, check if is changed before
	for (int gateID = 0; gateID < pCircuit_->tgate_; ++gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		if (gate.cc0_ != 0)
		{
			std::cerr << "cc0_ is not -1\n";
			std::cin.get();
		}
		if (gate.cc1_ != 0)
		{
			std::cerr << "cc1_ is not -1\n";
			std::cin.get();
		}
		if (gate.co_ != 0)
		{
			std::cerr << "co_ is not -1\n";
			std::cin.get();
		}
	}

	// array for xor2, xor3, xnor2, xnor3
	// xor2 xnor2 : {00,01,10,11}
	// xor3 xnor3 : {000, 001, 010, 011, 100, 101, 110, 111}
	int xorcc[8] = {0};
	Gate gateInputs[3];

	// calculate cc0 and cc1 starting from PI and PPI
	for (int gateID = 0; gateID < pCircuit_->tgate_; ++gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		switch (gate.gateType_)
		{
			case Gate::PPI:
			case Gate::PI:
				gate.cc0_ = 1;
				gate.cc1_ = 1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				gate.cc0_ = pCircuit_->gates_[gate.faninVector_[0]].cc0_;
				gate.cc1_ = pCircuit_->gates_[gate.faninVector_[0]].cc1_;
				break;
			case Gate::INV:
				gate.cc0_ = pCircuit_->gates_[gate.faninVector_[0]].cc1_ + 1;
				gate.cc1_ = pCircuit_->gates_[gate.faninVector_[0]].cc0_ + 1;
				break;
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
				for (int j = 0; j < gate.numFI_; ++j)
				{
					Gate &gateInput = pCircuit_->gates_[gate.faninVector_[j]];
					if (j == 0 || (gateInput.cc0_ < gate.cc0_))
					{
						gate.cc0_ = gateInput.cc0_;
					}
					gate.cc1_ += gateInput.cc1_;
				}
				++gate.cc1_;
				++gate.cc0_;
				break;

			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				for (int j = 0; j < gate.numFI_; ++j)
				{
					Gate &gateInput = pCircuit_->gates_[gate.faninVector_[j]];
					if (j == 0 || (gateInput.cc0_ < gate.cc1_))
					{
						gate.cc1_ = gateInput.cc0_;
					}
					gate.cc0_ += gateInput.cc1_;
				}
				++gate.cc0_;
				++gate.cc1_;
				break;

			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
				for (int j = 0; j < gate.numFI_; ++j)
				{
					Gate &gateInput = pCircuit_->gates_[gate.faninVector_[j]];
					if (j == 0 || (gateInput.cc1_ < gate.cc1_))
					{
						gate.cc1_ = gateInput.cc1_;
					}
					gate.cc0_ += gateInput.cc0_;
				}
				++gate.cc0_;
				++gate.cc1_;
				break;
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				for (int j = 0; j < gate.numFI_; ++j)
				{
					Gate &gateInput = pCircuit_->gates_[gate.faninVector_[j]];
					if (j == 0 || (gateInput.cc1_ < gate.cc0_))
					{
						gate.cc0_ = gateInput.cc1_;
					}
					gate.cc1_ += gateInput.cc0_;
				}
				++gate.cc0_;
				++gate.cc1_;
				break;
			case Gate::XOR2:
				gateInputs[0] = pCircuit_->gates_[gate.faninVector_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.faninVector_[1]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc1_;
				xorcc[2] = gateInputs[0].cc1_ + gateInputs[1].cc0_;
				xorcc[3] = gateInputs[0].cc1_ + gateInputs[1].cc1_;
				gate.cc0_ = std::min(xorcc[0], xorcc[3]);
				gate.cc1_ = std::min(xorcc[1], xorcc[2]);
				++gate.cc0_;
				++gate.cc1_;
				break;
			case Gate::XOR3:
				gateInputs[0] = pCircuit_->gates_[gate.faninVector_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.faninVector_[1]];
				gateInputs[2] = pCircuit_->gates_[gate.faninVector_[2]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[2] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[3] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				xorcc[4] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[5] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[6] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[7] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				gate.cc0_ = std::min(xorcc[0], xorcc[7]);
				for (int j = 1; j < 7; ++j)
				{
					if (j == 1 || xorcc[j] < gate.cc1_)
					{
						gate.cc1_ = xorcc[j];
					}
				}
				++gate.cc0_;
				++gate.cc1_;
				break;
			case Gate::XNOR2:
				gateInputs[0] = pCircuit_->gates_[gate.faninVector_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.faninVector_[1]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc1_;
				xorcc[2] = gateInputs[0].cc1_ + gateInputs[1].cc0_;
				xorcc[3] = gateInputs[0].cc1_ + gateInputs[1].cc1_;
				gate.cc0_ = std::min(xorcc[1], xorcc[2]);
				gate.cc1_ = std::min(xorcc[0], xorcc[3]);
				++gate.cc0_;
				++gate.cc1_;
				break;
			case Gate::XNOR3:
				gateInputs[0] = pCircuit_->gates_[gate.faninVector_[0]];
				gateInputs[1] = pCircuit_->gates_[gate.faninVector_[1]];
				gateInputs[2] = pCircuit_->gates_[gate.faninVector_[2]];
				xorcc[0] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[1] = gateInputs[0].cc0_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[2] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[3] = gateInputs[0].cc0_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				xorcc[4] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc0_;
				xorcc[5] = gateInputs[0].cc1_ + gateInputs[1].cc0_ + gateInputs[2].cc1_;
				xorcc[6] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc0_;
				xorcc[7] = gateInputs[0].cc1_ + gateInputs[1].cc1_ + gateInputs[2].cc1_;
				gate.cc1_ = std::min(xorcc[0], xorcc[7]);
				for (int j = 1; j < 7; ++j)
				{
					if (j == 1 || xorcc[j] < gate.cc1_)
					{
						gate.cc0_ = xorcc[j];
					}
				}
				++gate.cc0_;
				++gate.cc1_;
				break;
			default:
				std::cerr << "Bug: reach switch case default while calculating cc0_, cc1_";
				std::cin.get();
				break;
		}
	}

	// calculate co_ starting from PO and PP
	for (int gateID = 0; gateID < pCircuit_->tgate_; ++gateID)
	{
		Gate &gate = pCircuit_->gates_[gateID];
		switch (gate.gateType_)
		{
			case Gate::PO:
			case Gate::PPO:
				gate.co_ = 0;
				break;
			case Gate::PPI:
			case Gate::PI:
			case Gate::BUF:
				for (int j = 0; j < gate.numFO_; ++j)
				{
					if (j == 0 || pCircuit_->gates_[gate.fanoutVector_[j]].co_ < gate.co_)
					{
						gate.co_ = pCircuit_->gates_[gate.fanoutVector_[j]].co_;
					}
				}
				break;
			case Gate::INV:
				gate.co_ = pCircuit_->gates_[gate.fanoutVector_[0]].co_ + 1;
				break;
			case Gate::AND2:
			case Gate::AND3:
			case Gate::AND4:
			case Gate::NAND2:
			case Gate::NAND3:
			case Gate::NAND4:
				gate.co_ = pCircuit_->gates_[gate.fanoutVector_[0]].co_ + 1;
				for (int j = 0; j < pCircuit_->gates_[gate.fanoutVector_[0]].numFI_; ++j)
				{
					if (pCircuit_->gates_[gate.fanoutVector_[0]].faninVector_[j] != gateID)
					{
						Gate &gateSibling = pCircuit_->gates_[pCircuit_->gates_[gate.fanoutVector_[0]].faninVector_[j]];
						gate.co_ += gateSibling.cc1_;
					}
				}
				break;
			case Gate::OR2:
			case Gate::OR3:
			case Gate::OR4:
			case Gate::NOR2:
			case Gate::NOR3:
			case Gate::NOR4:
				gate.co_ = pCircuit_->gates_[gate.fanoutVector_[0]].co_ + 1;
				for (int j = 0; j < pCircuit_->gates_[gate.fanoutVector_[0]].numFI_; ++j)
				{
					if (pCircuit_->gates_[gate.fanoutVector_[0]].faninVector_[j] != gateID)
					{
						Gate &gateSibling = pCircuit_->gates_[pCircuit_->gates_[gate.fanoutVector_[0]].faninVector_[j]];
						gate.co_ += gateSibling.cc0_;
					}
				}
				break;
			case Gate::XOR2:
			case Gate::XNOR2:
			case Gate::XOR3:
			case Gate::XNOR3:
				gate.co_ = pCircuit_->gates_[gate.fanoutVector_[0]].co_ + 1;
				for (int j = 0; j < pCircuit_->gates_[gate.fanoutVector_[0]].numFI_; ++j)
				{
					Gate &gateSibling = pCircuit_->gates_[pCircuit_->gates_[gate.fanoutVector_[0]].faninVector_[j]];
					if (pCircuit_->gates_[gate.fanoutVector_[0]].faninVector_[j] != gateID)
					{
						gate.co_ += std::min(gateSibling.cc0_, gateSibling.cc1_);
					}
				}
				break;
			default:
				std::cerr << "Bug: reach switch case default while calculating co_";
				std::cin.get();
				break;
		}
	}

	return;
}

// **************************************************************************
// Function   [ Atpg::testClearFaultEffect ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Test clearAllFaultEffectBySimulation for all faults
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/04    last modified 2020/07/04 ]
// **************************************************************************
void Atpg::testClearFaultEffect(FaultPtrList &faultListToTest)
{
	for (Fault *pFault : faultListToTest)
	{
		generateSinglePatternOnTargetFault(*pFault, false);
		clearAllFaultEffectBySimulation();

		for (int i = 0; i < pCircuit_->tgate_; ++i)
		{
			Gate &gate = pCircuit_->gates_[i];
			if ((gate.atpgVal_ == D) || (gate.atpgVal_ == B))
			{
				std::cerr << "testClearFaultEffect found bug" << std::endl;
				std::cin.get();
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::resetIsInEventStack ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Set all element in isInEventStack_ to false
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::resetIsInEventStack()
{
	std::fill(isInEventStack_.begin(), isInEventStack_.end(), false);
}

// **************************************************************************
// Function   [ Atpg::XFill ]
// Commentor  [ HKY CYW ]
// Synopsis   [ usage: do X-Fill on generated pattern
//              in:    Pattern list
//              out:   void //TODO
//            ]
// Date       [ HKY Ver. 1.0 started 2014/09/01 ]
// **************************************************************************
void Atpg::XFill(PatternProcessor *pPatternProcessor)
{
	for (int i = 0; i < (int)pPatternProcessor->patternVector_.size(); ++i)
	{
		randomFill(pPatternProcessor->patternVector_[i]);
		pSimulator_->assignPatternToPi(pPatternProcessor->patternVector_.at(i));
		pSimulator_->goodSim();
		assignPatternPO_fromGoodSimVal(pPatternProcessor->patternVector_.at(i));
	}
}