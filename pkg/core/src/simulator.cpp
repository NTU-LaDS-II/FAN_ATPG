// **************************************************************************
// File       [ simulator.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/14 created ]
// **************************************************************************

#include "simulator.h"

using namespace IntfNs;
using namespace CoreNs;

// **************************************************************************
// Function   [ Simulator::eventFaultSim ]
// Commenter  [ CJY CBH ]
// Synopsis   [ usage: call faultValueEvaluation function whose gate's ID in
//                     stack event and check faulty value is equal to good
//                     value or not
//                     If the values are the same, no more process is needed
//                     If the values are not the same, keep processing
//              in:    void //TODO not void
//              out:   void //TODO not void
//            ]
// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
// **************************************************************************
void Simulator::eventFaultSim()
{
	for (int i = 0; i < pCircuit_->totalLvl_; ++i)
	{
		while (!events_[i].empty())
		{
			int gateID = events_[i].top(); // get gate's ID
			events_[i].pop();
			processed_[gateID] = 0;
			faultValueEvaluation(gateID);
			recoverGates_[numRecover_] = gateID; // record gate's ID
			++numRecover_;

			// check whether faulty value and good value are equal
			if (pCircuit_->circuitGates_[gateID].faultSimLow_ == pCircuit_->circuitGates_[gateID].goodSimLow_ && pCircuit_->circuitGates_[gateID].faultSimHigh_ == pCircuit_->circuitGates_[gateID].goodSimHigh_)
			{
				continue;
			}

			// if not equal and flag = false. set flag true
			for (int j = 0; j < pCircuit_->circuitGates_[gateID].numFO_; ++j)
			{
				int fanoutGateID = pCircuit_->circuitGates_[gateID].fanoutVector_[j];
				if (processed_[fanoutGateID])
				{
					continue;
				}
				events_[pCircuit_->circuitGates_[fanoutGateID].numLevel_].push(fanoutGateID);
				processed_[fanoutGateID] = 1;
			}
		}
	}
}

// **************************************************************************
// Function   [ void Simulator::parallelFaultFaultSimWithMultiplePattern(PatternProcessor *, FaultListExtract *) ]
// Author     [ littleshamoo ]
// Synopsis   [ usage: perform parallel fault simulation on all patterns.
//              call parallelFaultFaultSim(FaultPtrList &remainingFaults) for each pattern
//              in:	   a set of pattern
//              out:   void //TODO not void
//            ]
// Date       [ ]
// **************************************************************************
void Simulator::parallelFaultFaultSimWithMultiplePattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract)
{
	// undetected faults are remaining faults
	FaultPtrList remainingFaults;
	for (Fault *const &pFault : pFaultListExtract->faultsInCircuit_)
	{
		if (pFault->faultState_ != Fault::DT && pFault->faultState_ != Fault::RE && pFault->faultyLine_ >= 0)
		{
			remainingFaults.push_back(pFault);
		}
	}

	// simulate all patterns for all faults
	for (const Pattern &pattern : pPatternCollector->patternVector_)
	{
		if (remainingFaults.size() == 0)
		{
			break;
		}

		// Assign pattern to circuit PI & PPI for further fault sim
		assignPatternToCircuitInputs(pattern);
		parallelFaultFaultSim(remainingFaults);
	}
}

// **************************************************************************
// Function   [ Simulator::parallelFaultFaultSimWithOnePattern ]
// Commenter  [ CJY CBH ]
// Synopsis   [ usage: set pattern. call parallelFaultFaultSim(FaultPtrList &remainingFaults)
//                     to do faultsim for this pattern
//              in:    one pattern , FaultPtrList contain undetected faults
//              out:   void //TODO not void
//            ]
// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
// **************************************************************************
void Simulator::parallelFaultFaultSimWithOnePattern(const Pattern &pattern, FaultPtrList &remainingFaults)
{
	// Assign pattern to circuit PI & PPI for further fault sim
	assignPatternToCircuitInputs(pattern);
	parallelFaultFaultSim(remainingFaults);
}

// **************************************************************************
// Function   [ Simulator::parallelfFaultFaultSim ]
// Commenter  [ CJY CBH ]
// Synopsis   [ usage: do faultsim for the fault in fault list after assigning pattern
//              in:    FaultPtrList ( = list<Fault *> ) contains undetected faults
//              out:   void //TODO not void
//            ]
// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
// **************************************************************************
void Simulator::parallelFaultFaultSim(FaultPtrList &remainingFaults)
{
	if (remainingFaults.size() == 0)
	{
		return;
	}

	goodSimCopyGoodToFault(); // run good simulation first

	// inject number of WORD_SIZE activated faults
	FaultPtrListIter it = remainingFaults.begin();
	while (it != remainingFaults.end()) // do while => while by wang
	{
		// if fault is activated, inject fault
		if (parallelFaultCheckActivation(*it))
		{
			parallelFaultFaultInjection(*it, numInjectedFaults_);
			injectedFaults[numInjectedFaults_++] = it;
		}
		++it;
		// run fault sim if enough fault or end of fault list
		if (numInjectedFaults_ == (int)WORD_SIZE || (it == remainingFaults.end() && numInjectedFaults_ > 0))
		{
			eventFaultSim();
			parallelFaultCheckDetectionDropFaults(remainingFaults); // drop fault here
			parallelFaultReset();
		}
	}
}

// **************************************************************************
// Function   [ parallelPatternGoodSimWithAllPattern ]
// Commenter  [ Bill ]
// Synopsis   [ usage: for each pattern generated,
//              apply them separately and run good circuit simulation
//              in:    all the patterns generated by atpg
//              out:   void //TODO not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::parallelPatternGoodSimWithAllPattern(PatternProcessor *pPatternCollector)
{
	for (int i = 0; i < (int)pPatternCollector->patternVector_.size(); i += WORD_SIZE)
	{
		parallelPatternSetPattern(pPatternCollector, i);
		goodSim();
	}
}

// **************************************************************************
// Function   [ void Simulator::parallelPatternFaultSimWithPattern(PatternProcessor *, FaultListExtract *) ]
// Author     [ littleshamoo ]
// Synopsis   [ usage: perform parallel pattern simulation on all faults.
//              in:    a set of pattern, FaultListExtract
//              out:   void
//            ]
// Date       [ ]
// **************************************************************************
void Simulator::parallelPatternFaultSimWithPattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract)
{
	// undetected faults are remaining faults
	FaultPtrList remainingFaults;
	for (Fault *const &pFault : pFaultListExtract->faultsInCircuit_)
	{
		if (pFault->faultState_ != Fault::DT && pFault->faultState_ != Fault::RE && pFault->faultyLine_ >= 0)
		{
			remainingFaults.push_back(pFault);
		}
	}

	// simulate all patterns for all faults
	for (int patternStartIndex = 0; patternStartIndex < (int)pPatternCollector->patternVector_.size(); patternStartIndex += WORD_SIZE)
	{
		parallelPatternSetPattern(pPatternCollector, patternStartIndex);
		parallelPatternFaultSim(remainingFaults);
	}
}

// **************************************************************************
// Function   [ Simulator::parallelPatternFaultSim ]
// Commenter  [ Bill ]
// Synopsis   [ usage: simulate all undetected faults
//              in:    a set of undetected fault
//              out:   void //TODO not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::parallelPatternFaultSim(FaultPtrList &remainingFaults)
{
	if (remainingFaults.size() == 0)
	{
		return;
	}

	// run good simulation first
	goodSimCopyGoodToFault();

	FaultPtrListIter it = remainingFaults.begin();
	while (it != remainingFaults.end())
	{
		if (parallelPatternCheckActivation((*it)))
		{
			parallelPatternFaultInjection((*it));
			eventFaultSim();
			parallelPatternCheckDetection((*it));
			parallelPatternReset();
		}
		if ((*it)->faultState_ == Fault::DT)
		{
			it = remainingFaults.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// **************************************************************************
// Function   [ Simulator::parallelFaultReset ]
// Commenter  [ CJY CBH ]
// Synopsis   [ usage: reset faulty value of the fault gate to good value
//                     reset mask to zero
//              in:    void
//              out:   void
//            ]
// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
// **************************************************************************
void Simulator::parallelFaultReset()
{
	for (int i = 0; i < numRecover_; ++i)
	{
		pCircuit_->circuitGates_[recoverGates_[i]].faultSimLow_ = pCircuit_->circuitGates_[recoverGates_[i]].goodSimLow_;
		pCircuit_->circuitGates_[recoverGates_[i]].faultSimHigh_ = pCircuit_->circuitGates_[recoverGates_[i]].goodSimHigh_;
	}
	numRecover_ = 0;
	std::fill(processed_.begin(), processed_.end(), 0);
	std::fill(faultInjectLow_.begin(), faultInjectLow_.end(), std::array<ParallelValue, 5>({0, 0, 0, 0, 0}));
	std::fill(faultInjectHigh_.begin(), faultInjectHigh_.end(), std::array<ParallelValue, 5>({0, 0, 0, 0, 0}));

	numInjectedFaults_ = 0;
}

// **************************************************************************
// Function   [ Simulator::parallelFaultCheckActivation ]
// Commenter  [ CJY CBH ]
// Synopsis   [ usage: check whether the fault can be activate to the fanout of the gate
//                     ie, compare good low & high with the fault type
//              in:    a fault to be tested
//              out:   bool, indicate that we can inject this fault
//            ]
// Date       [ CBH Ver. 1.0 started 2013/08/18 ]
// **************************************************************************
bool Simulator::parallelFaultCheckActivation(const Fault *const pfault)
{
	const int &faultyGate = pfault->faultyLine_ == 0 ? pfault->gateID_ : pCircuit_->circuitGates_[pfault->gateID_].faninVector_[pfault->faultyLine_ - 1];
	                        /// if output fault,faultyGate = gateID of the faulty gate,else if input fault,faultyGate = gateID of the faulty gate's fanin array
	const ParallelValue &faultyGateGoodSimLow = pCircuit_->circuitGates_[faultyGate].goodSimLow_;
	const ParallelValue &faultyGateGoodSimHigh = pCircuit_->circuitGates_[faultyGate].goodSimHigh_;

	switch (pfault->faultType_)
	{
		case Fault::SA0:
			return faultyGateGoodSimHigh != PARA_L;
		case Fault::SA1:
			return faultyGateGoodSimLow != PARA_L;
		case Fault::STR:
			if (pCircuit_->numFrame_ < 2)
			{
				return false;
			}
			return (faultyGateGoodSimLow & pCircuit_->circuitGates_[faultyGate + pCircuit_->numGate_].goodSimHigh_) != PARA_L;
		case Fault::STF:
			if (pCircuit_->numFrame_ < 2)
			{
				return false;
			}
			return (faultyGateGoodSimHigh & pCircuit_->circuitGates_[faultyGate + pCircuit_->numGate_].goodSimLow_) != PARA_L;
		default:
			break;
	}
	return false;
}

// **************************************************************************
// Function   [ simulator::parallelFaultFaultInjection ]
// Commenter  [ CJY CBH]
// Synopsis   [ usage: inject fault and push faulty gate to event list.
//                     in parallel fault, we add fault on "one" bit in ParallelValue
//                     then we can have at most WORD_SIZE faults in one simulation
//              in:    fault, index of injected fault
//              out:   void //TODO not void
//            ]
// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
// **************************************************************************
void Simulator::parallelFaultFaultInjection(const Fault *const pfault, const size_t &injectFaultIndex)
{
	int faultyGate = pfault->gateID_;
	switch (pfault->faultType_)
	{
		case Fault::SA0:
			setBitValue(faultInjectLow_[faultyGate][pfault->faultyLine_], injectFaultIndex, H);
			break;
		case Fault::SA1:
			setBitValue(faultInjectHigh_[faultyGate][pfault->faultyLine_], injectFaultIndex, H);
			break;
		case Fault::STR:
			faultyGate += pCircuit_->numGate_;
			setBitValue(faultInjectLow_[faultyGate][pfault->faultyLine_], injectFaultIndex, H);
			break;
		case Fault::STF:
			faultyGate += pCircuit_->numGate_;
			setBitValue(faultInjectHigh_[faultyGate][pfault->faultyLine_], injectFaultIndex, H);
			break;
		default:
			break;
	}

	// put gate into event list
	if (!processed_[faultyGate])
	{
		events_[pCircuit_->circuitGates_[faultyGate].numLevel_].push(faultyGate);
		processed_[faultyGate] = 1;
	}
}

// **************************************************************************
// Function   [ simulator::parallelFaultCheckDetectionDropFaults ]
// Commenter  [ CJY CBH]
// Synopsis   [ usage: compare the good sim and fault sim result
//              and check whether the injected fault can be detected by the pattern
//              Finally, drop the detected faults
//              in:    the list of undetected fault
//              out:   void //TODO not void
//            ]
// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
// **************************************************************************
void Simulator::parallelFaultCheckDetectionDropFaults(FaultPtrList &remainingFaults)
{
	ParallelValue detected = PARA_L;
	int start = pCircuit_->totalGate_ - pCircuit_->numPO_ - pCircuit_->numPPI_;
	for (int i = start; i < pCircuit_->totalGate_; ++i)
	{
		detected |= ((pCircuit_->circuitGates_[i].goodSimLow_ & pCircuit_->circuitGates_[i].faultSimHigh_) | (pCircuit_->circuitGates_[i].goodSimHigh_ & pCircuit_->circuitGates_[i].faultSimLow_));
	}

	// fault drop
	for (int i = 0; i < numInjectedFaults_; ++i)
	{
		if (getBitValue(detected, (size_t)i) == L)
		{
			continue;
		}
		++((*injectedFaults[i])->detection_);
		if ((*injectedFaults[i])->detection_ >= numDetection_)
		{
			(*injectedFaults[i])->faultState_ = Fault::DT;
			remainingFaults.erase(injectedFaults[i]);
		}
	}
}

// **************************************************************************
// Function   [ Simulator::parallelPatternReset ]
// Commenter  [ Bill ]
// Synopsis   [ usage: reset all circuit gates ,faulty low & faulty high to good low & good high
//                     reset the processed flag and injected faults
//              in:    none
//              out:   void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::parallelPatternReset()
{
	for (int i = 0; i < numRecover_; ++i)
	{
		pCircuit_->circuitGates_[recoverGates_[i]].faultSimLow_ = pCircuit_->circuitGates_[recoverGates_[i]].goodSimLow_;
		pCircuit_->circuitGates_[recoverGates_[i]].faultSimHigh_ = pCircuit_->circuitGates_[recoverGates_[i]].goodSimHigh_;
	}
	numRecover_ = 0;
	std::fill(processed_.begin(), processed_.end(), 0);
	std::fill(faultInjectLow_.begin(), faultInjectLow_.end(), std::array<ParallelValue, 5>({0, 0, 0, 0, 0}));
	std::fill(faultInjectHigh_.begin(), faultInjectHigh_.end(), std::array<ParallelValue, 5>({0, 0, 0, 0, 0}));
	activated_ = PARA_L;
}

// **************************************************************************
// Function   [ parallelPatternCheckActivation ]
// Commenter  [ Bill ]
// Synopsis   [ usage: check whether the fault can be activate to the fanout of the gate
//              in:    fault to be tested
//              out:   bool, indicate that we can inject this fault
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
bool Simulator::parallelPatternCheckActivation(const Fault *const pfault)
{
	const int &faultyGate = pfault->faultyLine_ == 0 ? pfault->gateID_ : pCircuit_->circuitGates_[pfault->gateID_].faninVector_[pfault->faultyLine_ - 1];
	/// if output fault,faultyGate = gateID of the faulty gate,else if input fault,faultyGate = gateID of the faulty gate's fanin array
	const ParallelValue &faultyGateGoodSimLow = pCircuit_->circuitGates_[faultyGate].goodSimLow_;
	const ParallelValue &faultyGateGoodSimHigh = pCircuit_->circuitGates_[faultyGate].goodSimHigh_;

	switch (pfault->faultType_)
	{
		case Fault::SA0:
			activated_ = faultyGateGoodSimHigh;
			return activated_ != PARA_L;
		case Fault::SA1:
			activated_ = faultyGateGoodSimLow;
			return activated_ != PARA_L;
		case Fault::STR:
			if (pCircuit_->numFrame_ < 2)
			{
				return false;
			}
			activated_ = (faultyGateGoodSimLow & pCircuit_->circuitGates_[faultyGate + pCircuit_->numGate_].goodSimHigh_);
			return activated_ != PARA_L;
		case Fault::STF:
			if (pCircuit_->numFrame_ < 2)
			{
				return false;
			}
			activated_ = (faultyGateGoodSimHigh & pCircuit_->circuitGates_[faultyGate + pCircuit_->numGate_].goodSimLow_);
			return activated_ != PARA_L;
		default:
			break;
	}
	return false;
}

// **************************************************************************
// Function   [ parallelPatternFaultInjection ]
// Commenter  [ Bill ]
// Synopsis   [ usage: inject fault and push faulty gate to event list.
//                     in parallel pattern, we add fault on "all" bit in ParallelValue
//                     since we simulate the fault for all patterns
//              in:    a fault to be injected
//              out:   void //TODO not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::parallelPatternFaultInjection(const Fault *const pfault)
{
	int faultyGate = pfault->gateID_;
	switch (pfault->faultType_)
	{
		case Fault::SA0:
			faultInjectLow_[faultyGate][pfault->faultyLine_] = PARA_H;
			break;
		case Fault::SA1:
			faultInjectHigh_[faultyGate][pfault->faultyLine_] = PARA_H;
			break;
		case Fault::STR:
			faultyGate += pCircuit_->numGate_;
			faultInjectLow_[faultyGate][pfault->faultyLine_] = PARA_H;
			break;
		case Fault::STF:
			faultyGate += pCircuit_->numGate_;
			faultInjectHigh_[faultyGate][pfault->faultyLine_] = PARA_H;
			break;
		default:
			break;
	}

	// put gate into event list
	if (!processed_[faultyGate])
	{
		events_[pCircuit_->circuitGates_[faultyGate].numLevel_].push(faultyGate);
		processed_[faultyGate] = 1;
	}
}

// **************************************************************************
// Function   [ parallelPatternCheckDetection ]
// Commenter  [ Bill ]
// Synopsis   [ usage: compare the good sim and fault sim result
//              and check whether the injected fault can be detected by the pattern
//              in:    a fault which is injected into the circuit
//              out:   void //not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::parallelPatternCheckDetection(Fault *const pfault)
{
	ParallelValue detected = PARA_L;
	int start = pCircuit_->totalGate_ - pCircuit_->numPO_ - pCircuit_->numPPI_;
	for (int i = start; i < pCircuit_->totalGate_; ++i)
	{
		// TO-DO homework 02
		detected |= ((pCircuit_->circuitGates_[i].goodSimLow_ & pCircuit_->circuitGates_[i].faultSimHigh_) | (pCircuit_->circuitGates_[i].goodSimHigh_ & pCircuit_->circuitGates_[i].faultSimLow_));
		// end of TO-DO
	}
	detected &= activated_;

	// set fault state to DT for fault drop
	for (int i = 0; i < WORD_SIZE; ++i)
	{
		if (getBitValue(detected, i) == L)
		{
			continue;
		}
		++(pfault->detection_);
		if (pfault->detection_ >= numDetection_)
		{
			pfault->faultState_ = Fault::DT;
			break;
		}
	}
}

// **************************************************************************
// Function   [ parallelPatternSetPattern ]
// Commenter  [ Bill ]
// Synopsis   [ usage: apply patterns to PI and PPI
//              in:    the total pattern set,
//              and an integer to represent the starting pattern of current fsim
//              out:   void // not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::parallelPatternSetPattern(PatternProcessor *pPatternProcessor, const int &patternStartIndex)
{ // TODO LOS not yet supported
	// reset PI and PPI values to unknowns
	for (int j = 0; j < pCircuit_->numPI_ + pCircuit_->numPPI_; ++j)
	{
		for (int k = 0; k < pCircuit_->numFrame_; ++k)
		{
			pCircuit_->circuitGates_[j + k * pCircuit_->numGate_].goodSimLow_ = PARA_L;
			pCircuit_->circuitGates_[j + k * pCircuit_->numGate_].goodSimHigh_ = PARA_L;
		}
	}
	// assign up to WORD_SIZE number of pattern values
	int endpat = (int)pPatternProcessor->patternVector_.size();
	if (patternStartIndex + (int)WORD_SIZE <= (int)pPatternProcessor->patternVector_.size())
	{
		endpat = patternStartIndex + WORD_SIZE;
	}
	for (int j = patternStartIndex; j < endpat; ++j)
	{
		// assign PI
		if (!pPatternProcessor->patternVector_[j].primaryInputs1st_.empty())
		{
			for (int k = 0; k < pPatternProcessor->numPI_; ++k)
			{
				if (pPatternProcessor->patternVector_[j].primaryInputs1st_[k] == L)
				{
					setBitValue(pCircuit_->circuitGates_[k].goodSimLow_, j - patternStartIndex, H);
				}
				else if (pPatternProcessor->patternVector_[j].primaryInputs1st_[k] == H)
				{
					setBitValue(pCircuit_->circuitGates_[k].goodSimHigh_, j - patternStartIndex, H);
				}
			}
		}

		if (!pPatternProcessor->patternVector_[j].primaryInputs2nd_.empty() && pCircuit_->numFrame_ > 1)
		{
			for (int k = 0; k < pPatternProcessor->numPI_; ++k)
			{
				int index = k + pCircuit_->numGate_;
				if (pPatternProcessor->patternVector_[j].primaryInputs2nd_[k] == L)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimLow_, j - patternStartIndex, H);
				}
				else if (pPatternProcessor->patternVector_[j].primaryInputs2nd_[k] == H)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimHigh_, j - patternStartIndex, H);
				}
			}
		}
		// assign PPI
		if (!pPatternProcessor->patternVector_[j].pseudoPrimaryInputs_.empty())
		{
			for (int k = 0; k < pPatternProcessor->numPPI_; ++k)
			{
				int index = k + pCircuit_->numPI_;
				if (pPatternProcessor->patternVector_[j].pseudoPrimaryInputs_[k] == L)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimLow_, j - patternStartIndex, H);
				}
				else if (pPatternProcessor->patternVector_[j].pseudoPrimaryInputs_[k] == H)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimHigh_, j - patternStartIndex, H);
				}
			}
		}

		if (!pPatternProcessor->patternVector_[j].shiftIn_.empty() && pCircuit_->numFrame_ > 1 && pCircuit_->timeFrameConnectType_ == Circuit::SHIFT)
		{
			int index = pCircuit_->numGate_ + pCircuit_->numPI_;
			if (pPatternProcessor->patternVector_[j].shiftIn_[0] == L)
			{
				setBitValue(pCircuit_->circuitGates_[index].goodSimLow_, j - patternStartIndex, H);
			}
			else if (pPatternProcessor->patternVector_[j].shiftIn_[0] == H)
			{
				setBitValue(pCircuit_->circuitGates_[index].goodSimHigh_, j - patternStartIndex, H);
			}
		}
	}
}
