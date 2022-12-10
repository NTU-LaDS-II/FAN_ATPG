#include "atpg.h"

using namespace CoreNs;

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
	if (gate.v_ == D)
	{
		gate.v_ = H;
	}
	else if (gate.v_ == B)
	{
		gate.v_ = L;
	}
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

	// Remove fault effects in input gates
	int numOfInputGate = pCircuit_->npi_ + pCircuit_->nppi_;
	for (int i = 0; i < numOfInputGate; ++i)
	{
		Gate &gate = pCircuit_->gates_[i];
		clearOneGateFaultEffect(gate);
	}

	// Simulate the whole circuit ( gates were sorted by lvl_ in "pCircuit_->gates_" )
	for (int i = 0; i < pCircuit_->tgate_; ++i)
	{
		Gate &gate = pCircuit_->gates_[i];
		gate.v_ = evaluateGoodVal(gate);
	}
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
void Atpg::testClearFaultEffect(FaultList &faultListToTest)
{
	for (auto it = faultListToTest.begin(); it != faultListToTest.end(); ++it)
	{
		SINGLE_PATTERN_GENERATION_STATUS result = generateSinglePatternOnTargetFault((**it), false);
		clearAllFaultEffectBySimulation();
		for (int i = 0; i < pCircuit_->tgate_; ++i)
		{
			Gate &gate = pCircuit_->gates_[i];
			if ((gate.v_ == D) || (gate.v_ == B))
			{
				std::cerr << "testClearFaultEffect found bug" << std::endl;
				std::cin.get();
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::storeCurrentGateValue ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Store gate.v_ to gate.preV_
//              in:   void
//              out:  Count of values which change from H/L to the value which is not
//                    the same as preV_.
//            ]
// Date       [ started 2020/07/04    last modified 2020/07/04 ]
// **************************************************************************
int Atpg::storeCurrentGateValue()
{
	int numAssignedValueChanged = 0;
	for (int i = 0; i < pCircuit_->tgate_; ++i)
	{
		Gate &gate = pCircuit_->gates_[i];
		if ((gate.preV_ != X) && (gate.preV_ != gate.v_))
		{
			numAssignedValueChanged++;
		}
		gate.preV_ = gate.v_;
	}
	if (numAssignedValueChanged != 0)
	{
		std::cout << "Bug: storeCurrentGateValue detects the numAssignedValueChanged is not 0." << std::endl;
		// std::cin.get();
	}
	return numAssignedValueChanged;
}

// **************************************************************************
// Function   [ Atpg::storeCurrentGateValue ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Calculate the depthFromPo_ of each gate.
//              notes:
//                If there is no path from a gate to PO/PPO, then I will
//                set its depthFromPo_ as pCircuit_->tlvl_ + 100
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/06    last modified 2020/07/06 ]
// **************************************************************************
void Atpg::calDepthFromPo()
{
	int tlvlAddPlus100 = pCircuit_->tlvl_ + 100;
	for (int i = 0; i < pCircuit_->tgate_; ++i)
	{
		Gate &gate = pCircuit_->gates_[i];
		/*
		 * Because the default is -1, I want to check if it was changed or not.
		 * */
		if (gate.depthFromPo_ != -1)
		{
			std::cout << "depthFromPo_ is not -1 " << std::endl;
			std::cin.get();
		}
	}

	// Update depthFromPo_ form PO/PPO to PI/PPI
	for (int i = pCircuit_->tgate_ - 1; i >= 0; --i)
	{
		Gate &gate = pCircuit_->gates_[i];
		if ((gate.type_ == Gate::PO) || (gate.type_ == Gate::PPO))
		{
			gate.depthFromPo_ = 0;
		}
		else if (gate.nfo_ > 0)
		{
			// This output gate does not exist a path to PO/PPO
			if (pCircuit_->gates_[gate.fos_[0]].depthFromPo_ == tlvlAddPlus100)
			{
				gate.depthFromPo_ = tlvlAddPlus100;
			}
			else
			{
				gate.depthFromPo_ = pCircuit_->gates_[gate.fos_[0]].depthFromPo_ + 1;
			}

			for (int j = 1; j < gate.nfo_; j++)
			{
				Gate &go = pCircuit_->gates_[gate.fos_[j]];
				if (go.depthFromPo_ < gate.depthFromPo_)
				{
					gate.depthFromPo_ = go.depthFromPo_ + 1;
				}
			}
		}
		else
		{
			/* Assign a value greater than maximal lvl_ as our default */
			gate.depthFromPo_ = tlvlAddPlus100;
		}
	}

	// for ( int i = 0 ; i < pCircuit_->tgate_ ; ++i ) {
	//   Gate &gate = pCircuit_->gates_[i];
	//   std::cout << "gate.depthFromPo_ is " << gate.depthFromPo_ << std::endl;
	// }
	// std::cin.get();
}

// **************************************************************************
// Function   [ Atpg::resetPreValue ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Reset the preV_ of each gate to X.
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::resetPreValue()
{
	for (int i = 0; i < pCircuit_->tgate_; ++i)
	{
		Gate &gate = pCircuit_->gates_[i];
		gate.preV_ = X;
	}
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
	/* Clear the gateID_to_xPathStatus_ from target gate to PO/PPO */
	/* TO-DO
	 * This part can be implemented by event-driven method
	 * */
	for (int i = pGate->id_; i < pCircuit_->tgate_; ++i)
	{
		gateID_to_xPathStatus_[i] = UNKNOWN;
	}

	return xPathTracing(pGate);
}

// **************************************************************************
// Function   [ Atpg::clearEventList_ ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Clear circuitLevel_to_EventStack_ and carefully reset gateID_to_valModified_ and isInEventList_
//              in:    check isInEventList_ correctness
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::clearEventList_(bool isDebug)
{

	// pop and unmark
	for (int i = 0; i < pCircuit_->tlvl_; ++i)
	{
		while (!circuitLevel_to_EventStack_[i].empty())
		{
			int gateID = circuitLevel_to_EventStack_[i].top();
			circuitLevel_to_EventStack_[i].pop();
			gateID_to_valModified_[gateID] = 0;
			isInEventList_[gateID] = false;
		}
	}

	/*
	 * Because I expect that all gates in circuit must be unmark after the above
	 * for-loop.
	 * */
	if (isDebug == true)
	{
		for (int i = 0; i < pCircuit_->tgate_; ++i)
		{
			if (isInEventList_[i] == true)
			{
				std::cout << "Warning clearEventList_ found unexpected behavior" << std::endl;
				isInEventList_[i] = false;
				std::cin.get();
			}
		}
	}
}

// **************************************************************************
// Function   [ Atpg::resetIsInEventList ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Set all element in isInEventList_ to false
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/07    last modified 2020/07/07 ]
// **************************************************************************
void Atpg::resetIsInEventList()
{
	std::fill(isInEventList_.begin(), isInEventList_.end(), false);
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
void Atpg::setValueAndRunImp(Gate &gate, Value val)
{
	clearEventList_(true);
	gate.v_ = val;
	for (int i = 0; i < gate.nfo_; ++i)
	{
		Gate &og = pCircuit_->gates_[gate.fos_[i]];
		if (isInEventList_[og.id_] == false)
		{
			circuitLevel_to_EventStack_[og.lvl_].push(og.id_);
			isInEventList_[og.id_] = true;
		}
	}

	// event-driven simulation
	for (int i = gate.lvl_; i < pCircuit_->tlvl_; ++i)
	{
		while (!circuitLevel_to_EventStack_[i].empty())
		{
			int gateID = circuitLevel_to_EventStack_[i].top();
			circuitLevel_to_EventStack_[i].pop();
			isInEventList_[gateID] = false;
			// current gate
			Gate &cg = pCircuit_->gates_[gateID];
			Value newValue = evaluateGoodVal(cg);
			if (cg.v_ != newValue)
			{
				cg.v_ = newValue;
				for (int j = 0; j < cg.nfo_; ++j)
				{
					Gate &og = pCircuit_->gates_[cg.fos_[j]];
					if (isInEventList_[og.id_] == false)
					{
						circuitLevel_to_EventStack_[og.lvl_].push(og.id_);
						isInEventList_[og.id_] = true;
					}
				}
			}
		}
	}
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
		if (gate.lvl_ >= pCircuit_->tlvl_)
		{
			std::cout << "checkLevelInfo found that at least one gate.lvl_ is greater than pCircuit_->tlvl_" << std::endl;
			std::cin.get();
		}
	}
}

// **************************************************************************
// Function   [ Atpg::StuckAtFaultATPGWithDTC ]
// Commentor  [ CAL ]
// Synopsis   [ usage: do stuck at fault model ATPG with dynamic test compression
//              in:    Pattern list, Fault list, int untest
//              out:   void //add pattern to PatternProcessor*
//            ]
// Date       [ started 2020/07/07    last modified 2021/09/14 ]
// **************************************************************************
void Atpg::StuckAtFaultATPGWithDTC(FaultList &faultListToGen, PatternProcessor *pcoll, int &untest)
{
	static int oriPatNum = 0;
	SINGLE_PATTERN_GENERATION_STATUS result = generateSinglePatternOnTargetFault(*faultListToGen.front(), false);

	if (result == PATTERN_FOUND)
	{
		oriPatNum++;
		// std::cout << oriPatNum << "-th pattern" << std::endl;
		Pattern *p = new Pattern;
		p->pi1_ = new Value[pCircuit_->npi_];
		p->ppi_ = new Value[pCircuit_->nppi_];
		p->po1_ = new Value[pCircuit_->npo_];
		p->ppo_ = new Value[pCircuit_->nppi_];
		pcoll->pats_.push_back(p);
		resetPreValue();
		clearAllFaultEffectBySimulation();
		storeCurrentGateValue();
		assignPatternPiFromGateVal(pcoll->pats_.back());

		if (pcoll->dynamicCompression_ == PatternProcessor::ON)
		{

			FaultList faultListTemp = faultListToGen;
			pSimulator_->pfFaultSim(pcoll->pats_.back(), faultListToGen);
			pSimulator_->goodSim();
			assignPatternPoFromGoodSimVal(pcoll->pats_.back());

			for (FaultListIter it = faultListTemp.begin(); it != faultListTemp.end(); ++it)
			{
				// skip detected faults
				if ((*it)->state_ == Fault::DT)
					continue;

				Gate *pGateForAtivation = getWireForActivation((**it));
				if (((pGateForAtivation->v_ == L) && ((*it)->type_ == Fault::SA0)) ||
						((pGateForAtivation->v_ == H) && ((*it)->type_ == Fault::SA1)))
				{
					continue;
				}

				// Activation check
				if (pGateForAtivation->v_ != X)
				{
					if (((*it)->type_ == Fault::SA0) || ((*it)->type_ == Fault::SA1))
					{
						setValueAndRunImp((*pGateForAtivation), X);
					}
					else
					{
						continue;
					}
				}

				if (isExistXPath(pGateForAtivation) == true)
				{
					// TO-DO homework 05 implement DTC here end of TO-DO
					SINGLE_PATTERN_GENERATION_STATUS resultDTC = generateSinglePatternOnTargetFault((**it), true);
					if (resultDTC == PATTERN_FOUND)
					{
						clearAllFaultEffectBySimulation();
						storeCurrentGateValue();
						assignPatternPiFromGateVal(pcoll->pats_.back());
					}
					else
					{
						for (int i = 0; i < pCircuit_->tgate_; ++i)
						{
							Gate &gate = pCircuit_->gates_[i];
							gate.v_ = gate.preV_;
						}
					}
				}
				else
				{
					setValueAndRunImp((*pGateForAtivation), pGateForAtivation->preV_);
				}
			}
		}

		clearAllFaultEffectBySimulation();
		storeCurrentGateValue();
		assignPatternPiFromGateVal(pcoll->pats_.back());

		if (pcoll->XFill_ == PatternProcessor::ON)
		{
			/*
			 * Randomly fill the pats_.back().
			 * Please note that the v_, gh_, gl_, fh_ and fl_ do not be changed.
			 * */
			randomFill(pcoll->pats_.back());
		}

		/*
		 * This function will assign pi/ppi stored in pats_.back() to
		 * the gh_ and gl_ in each gate, and then it will run fault
		 * simulation to drop fault.
		 * */
		pSimulator_->pfFaultSim(pcoll->pats_.back(), faultListToGen);
		/*
		 * After pSimulator_->pfFaultSim(pcoll->pats_.back(),faultListToGen) , the pi/ppi
		 * values have been passed to gh_ and gl_ of each gate.  Therefore, we can
		 * directly use "assignPatternPoFromGoodSimVal" to perform goodSim to get the PoValue.
		 * */
		pSimulator_->goodSim();
		assignPatternPoFromGoodSimVal(pcoll->pats_.back());
	}
	else if (result == FAULT_UNTESTABLE)
	{
		faultListToGen.front()->state_ = Fault::AU;
		faultListToGen.pop_front();
		untest++;
	}
	else
	{
		faultListToGen.front()->state_ = Fault::AB;
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
Gate *Atpg::getWireForActivation(Fault &fault)
{
	bool isOutputFault = (fault.line_ == 0);
	Gate *pGateForAtivation;
	Gate *pFaultyGate = &pCircuit_->gates_[fault.gate_];
	if (!isOutputFault)
	{
		pGateForAtivation = &pCircuit_->gates_[pFaultyGate->fis_[fault.line_ - 1]];
	}
	else
	{
		pGateForAtivation = pFaultyGate;
	}

	return pGateForAtivation;
}

// **************************************************************************
// Function   [ Atpg::reverseFaultSimulation ]
// Commentor  [ CAL ]
// Synopsis   [ usage:
//                Perform reverse fault simulation
//              in:    void
//              out:   void
//            ]
// Date       [ started 2020/07/08    last modified 2020/07/08 ]
// **************************************************************************
void Atpg::reverseFaultSimulation(PatternProcessor *pcoll, FaultList &originalFaultList)
{
	// set TD to UD
	for (auto it = originalFaultList.begin(); it != originalFaultList.end(); ++it)
	{
		(*it)->det_ = 0;
		if ((*it)->state_ == Fault::DT)
		{
			(*it)->state_ = Fault::UD;
		}
	}

	PatternVec tmp = pcoll->pats_;
	pcoll->pats_.clear();

	// Perform reverse fault simulation
	size_t leftFaultCount = originalFaultList.size();
	for (auto rit = tmp.rbegin(); rit != tmp.rend(); ++rit)
	{
		pSimulator_->pfFaultSim((*rit), originalFaultList);
		if (leftFaultCount > originalFaultList.size())
		{
			leftFaultCount = originalFaultList.size();
			pcoll->pats_.push_back((*rit));
		}
		else if (leftFaultCount < originalFaultList.size())
		{
			std::cout << "reverseFaultSimulation: unexpected behavior" << std::endl;
			std::cin.get();
		}
		else
		{
			delete (*rit);
		}
	}
}
