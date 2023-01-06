// **************************************************************************
// File       [ simulator.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ Function definitions for simulator.h ]
// Date       [ 2011/09/14 created ]
// **************************************************************************

#include "simulator.h"

using namespace IntfNs;
using namespace CoreNs;

// **************************************************************************
// Function   [ Simulator::eventFaultSim ]
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Do event-driven fault simulation.
//              description:
//              	Call the faultyValueEvaluation function for gates in the
//              	event stacks and check if the faulty value is equal to the good
//              	value or not. If the values are the same, no more process
//              	is needed. If the values are not the same, keep processing.
//            ]
// Date       [ Ver. 1.0 started 2014/08/14 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::eventFaultSim()
{
	for (int i = 0; i < pCircuit_->totalLvl_; ++i)
	{
		while (!events_[i].empty())
		{
			int gateID = events_[i].top(); // Get gate's ID.
			events_[i].pop();
			processed_[gateID] = 0;
			faultyValueEvaluation(gateID);
			recoverGates_[numRecover_] = gateID; // Record gate's ID for later recovering.
			++numRecover_;

			// Check whether faulty value and good value are equal.
			bool faultyEqualGood = pCircuit_->circuitGates_[gateID].faultSimLow_ == pCircuit_->circuitGates_[gateID].goodSimLow_ && pCircuit_->circuitGates_[gateID].faultSimHigh_ == pCircuit_->circuitGates_[gateID].goodSimHigh_;
			if (faultyEqualGood)
			{
				continue;
			}

			// If not equal and flag = false(0), set flag true(1).
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
// Function   [ Simulator::parallelFaultFaultSimWithAllPattern ]
// Commenter  [ littleshamoo, PYH ]
// Synopsis   [ usage: Perform parallel fault fault simulation on all patterns.
//              description:
//              	First we extract undetected faults from the fault list. Then for
//              	each pattern, we assign the pattern and call the parallelFaultFaultSim
//              	function to do the fault simulation. May stop earlier if all
//              	faults are detected.
//              arguments:
//              	[in] pPatternCollector : The patterns generated in ATPG.
//              	[in] pFaultListExtract : The whole fault list.
//            ]
// Date       [ Ver. 1.0 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelFaultFaultSimWithAllPattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract)
{
	// Undetected faults are remaining faults.
	FaultPtrList remainingFaults;
	for (Fault *const &pFault : pFaultListExtract->faultsInCircuit_)
	{
		bool faultNotDetect = pFault->faultState_ != Fault::DT && pFault->faultState_ != Fault::RE && pFault->faultyLine_ >= 0;
		if (faultNotDetect)
		{
			remainingFaults.push_back(pFault);
		}
	}

	// Simulate all patterns for all faults.
	for (const Pattern &pattern : pPatternCollector->patternVector_)
	{
		if (remainingFaults.size() == 0)
		{
			break;
		}

		// Assign pattern to circuit PI & PPI for further fault simulation.
		assignPatternToCircuitInputs(pattern);
		parallelFaultFaultSim(remainingFaults);
	}
}

// **************************************************************************
// Function   [ Simulator::parallelFaultFaultSimWithOnePattern ]
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Perform parallel fault fault simulation on one pattern.
//              description:
//              	Set the pattern and call the parallelFaultFaultSim function to
//              	do the fault simulation for this pattern.
//              arguments:
//              	[in] pattern : The test pattern for the fault simulation.
//              	[in] remainingFaults : The list of undetected faults.
//            ]
// Date       [ Ver. 1.0 started 2013/08/14 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelFaultFaultSimWithOnePattern(const Pattern &pattern, FaultPtrList &remainingFaults)
{
	// Assign pattern to circuit PI & PPI for further fault simulation.
	assignPatternToCircuitInputs(pattern);
	parallelFaultFaultSim(remainingFaults);
}

// **************************************************************************
// Function   [ Simulator::parallelfFaultFaultSim ]
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Perform parallel fault fault simulation with assigned pattern.
//              description:
//              	First we simulate good value. Then for the pattern, if the
//              	fault can be activated, inject the fault. When we inject enough
//              	faults or reach the end of the fault list, we run fault simulation
//              	for the injected faults and try to drop the detected faults.
//              	Here we can inject at most WORD_SIZE faults in one simulation.
//              arguments:
//              	[in] remainingFaults : The list of undetected faults.
//            ]
// Date       [ Ver. 1.0 started 2013/08/14 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelFaultFaultSim(FaultPtrList &remainingFaults)
{
	if (remainingFaults.size() == 0)
	{
		return;
	}

	goodSimCopyGoodToFault(); // Run good simulation first.

	// Inject number of WORD_SIZE activated faults.
	FaultPtrListIter it = remainingFaults.begin();
	while (it != remainingFaults.end())
	{
		// If fault is activated, inject fault.
		if (parallelFaultCheckActivation(*it))
		{
			parallelFaultFaultInjection(*it, numInjectedFaults_);
			injectedFaults_[numInjectedFaults_++] = it;
		}
		++it;
		// Run fault simulation if enough fault or end of fault list.
		if (numInjectedFaults_ == (int)WORD_SIZE || (it == remainingFaults.end() && numInjectedFaults_ > 0))
		{
			eventFaultSim();
			parallelFaultCheckDetectionDropFaults(remainingFaults); // Drop fault here.
			parallelFaultReset();
		}
	}
}
// **************************************************************************
// Function   [ Simulator::parallelPatternGoodSimWithAllPattern ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Perform parallel pattern good simulation with all patterns.
//              description:
//              	Set many patterns in parallel (at most WORD_SIZE) and run the
//              	good simulation.
//              arguments:
//              	[in] pPatternCollector : The patterns generated in ATPG.
//            ]
// Date       [ Ver. 1.0 started 2013/08/11 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelPatternGoodSimWithAllPattern(PatternProcessor *pPatternCollector)
{
	for (int patternStartIndex = 0; patternStartIndex < (int)pPatternCollector->patternVector_.size(); patternStartIndex += WORD_SIZE)
	{
		parallelPatternSetPattern(pPatternCollector, patternStartIndex);
		goodSim();
	}
}

// **************************************************************************
// Function   [ Simulator::parallelPatternFaultSimWithAllPattern ]
// Commenter  [ littleshamoo, PYH ]
// Synopsis   [ usage: Perform parallel pattern fault simulation with all patterns
//                     on all faults.
//              description:
//              	First we extract undetected faults from the fault list. Then we
//              	collect many patterns (at most WORD_SIZE) and call the
//              	parallelPatternFaultSim function to do the fault simulation
//              	on undetected faults for these patterns.
//              arguments:
//              	[in] pPatternCollector : The patterns generated in ATPG.
//              	[in] pFaultListExtract : The whole fault list.
//            ]
// Date       [ Ver. 1.0 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelPatternFaultSimWithAllPattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract)
{
	// Undetected faults are remaining faults.
	FaultPtrList remainingFaults;
	for (Fault *const &pFault : pFaultListExtract->faultsInCircuit_)
	{
		bool faultNotDetect = pFault->faultState_ != Fault::DT && pFault->faultState_ != Fault::RE && pFault->faultyLine_ >= 0;
		if (faultNotDetect)
		{
			remainingFaults.push_back(pFault);
		}
	}

	// Simulate all patterns for all faults.
	for (int patternStartIndex = 0; patternStartIndex < (int)pPatternCollector->patternVector_.size(); patternStartIndex += WORD_SIZE)
	{
		parallelPatternSetPattern(pPatternCollector, patternStartIndex);
		parallelPatternFaultSim(remainingFaults);
	}
}

// **************************************************************************
// Function   [ Simulator::parallelPatternFaultSim ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Perform parallel pattern fault simulation on all faults
//                     after assigning patterns.
//              description:
//              	First we simulate the good value for the assigned pattern. Then for
//              	all undetected faults, if the fault can be activated, inject
//              	the fault. If the fault can be detected, we can drop this fault.
//              arguments:
//              	[in] remainingFaults : The list of undetected faults.
//            ]
// Date       [ Ver. 1.0 started 2013/08/11 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelPatternFaultSim(FaultPtrList &remainingFaults)
{
	if (remainingFaults.size() == 0)
	{
		return;
	}

	// Run good simulation first.
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
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Reset simulation after doing parallel fault fault simulation.
//              description:
//              	Reset faulty value of the fault gate to good value. Also, reset
//              	processed flags and fault masks to 0.
//            ]
// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/06 ]
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
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Check whether the fault can be activated to the fanout of the gate.
//              description:
//              	Compare the goodSimLow_ & goodSimHigh_ of the faulty gate
//              	with the fault type to check whether the fault can be activated.
//              arguments:
//              	[in] pfault : The fault we want to check.
//              	[out] bool : Indicate whether the fault can be activated or not.
//              	             If activated then we can inject this fault.
//            ]
// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/06 ]
// **************************************************************************
bool Simulator::parallelFaultCheckActivation(const Fault *const pfault)
{
	// If output fault, faultyGate = gateID of the faulty gate.
	// Else if input fault, faultyGate = gateID of the faulty gate's fanin array.
	const int &faultyGate = pfault->faultyLine_ == 0 ? pfault->gateID_ : pCircuit_->circuitGates_[pfault->gateID_].faninVector_[pfault->faultyLine_ - 1];
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
// Function   [ Simulator::parallelFaultFaultInjection ]
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Inject fault and push faulty gate into event list.
//              description:
//              	In parallel fault fault simulation, we add fault on "one" bit
//              	in ParallelValue. Then we can have at most WORD_SIZE faults
//              	in one fault simulation.
//              arguments:
//              	[in] pfault : The fault we want to inject.
//              	[in] faultInjectIndex : The index we want to inject to.
//            ]
// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/06 ]
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

	// Put gate into event list.
	if (!processed_[faultyGate])
	{
		events_[pCircuit_->circuitGates_[faultyGate].numLevel_].push(faultyGate);
		processed_[faultyGate] = 1;
	}
}

// **************************************************************************
// Function   [ Simulator::parallelFaultCheckDetectionDropFaults ]
// Commenter  [ CJY, CBH, PYH ]
// Synopsis   [ usage: Check whether the injected fault can be detected by the pattern.
//              description:
//              	Compare the result of the good simulator and fault simulator and
//              	check whether the injected fault can be detected by the pattern.
//              	Finally, drop the detected faults.
//              arguments:
//              	[in] remainingFaults : The list of undetected faults.
//            ]
// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelFaultCheckDetectionDropFaults(FaultPtrList &remainingFaults)
{
	ParallelValue detected = PARA_L;
	int start = pCircuit_->totalGate_ - pCircuit_->numPO_ - pCircuit_->numPPI_;
	for (int i = start; i < pCircuit_->totalGate_; ++i)
	{
		detected |= ((pCircuit_->circuitGates_[i].goodSimLow_ & pCircuit_->circuitGates_[i].faultSimHigh_) | (pCircuit_->circuitGates_[i].goodSimHigh_ & pCircuit_->circuitGates_[i].faultSimLow_));
	}

	// Fault drop.
	for (int i = 0; i < numInjectedFaults_; ++i)
	{
		if (getBitValue(detected, (size_t)i) == L)
		{
			continue;
		}
		++((*injectedFaults_[i])->detection_);
		if ((*injectedFaults_[i])->detection_ >= numDetection_)
		{
			(*injectedFaults_[i])->faultState_ = Fault::DT;
			remainingFaults.erase(injectedFaults_[i]);
		}
	}
}

// **************************************************************************
// Function   [ Simulator::parallelPatternReset ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Reset simulation after doing parallel pattern pattern simulation.
//              description:
//              	Reset faulty value of the fault gate to good value. Also, reset
//              	processed flags, activated flags, and fault masks to 0.
//            ]
// Date       [ Ver. 1.0 started 2013/08/11 last modified 2023/01/06 ]
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
// Function   [ Simulator::parallelPatternCheckActivation ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Check whether the fault can be activated to the fanout of the gate.
//              description:
//              	Compare the goodSimLow_ & goodSimHigh_ of the faulty gate
//              	with the fault type to check whether the fault can be activated.
//              	We then save the activated flags for patterns in the activated_ variable.
//              arguments:
//              	[in] pfault : The fault we want to check.
//              	[out] bool : Indicate whether the fault can be activated or not.
//              	             If activated then we can inject this fault.
//            ]
// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/06 ]
// **************************************************************************
bool Simulator::parallelPatternCheckActivation(const Fault *const pfault)
{
	// If output fault, faultyGate = gateID of the faulty gate.
	// Else if input fault, faultyGate = gateID of the faulty gate's fanin array.
	const int &faultyGate = pfault->faultyLine_ == 0 ? pfault->gateID_ : pCircuit_->circuitGates_[pfault->gateID_].faninVector_[pfault->faultyLine_ - 1];
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
// Function   [ Simulator::parallelPatternFaultInjection ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Inject fault and push faulty gate into event list.
//              description:
//              	In parallel pattern fault simulation, we add fault on "all" bits
//              	in ParallelValue since we simulate the fault for all patterns.
//              arguments:
//              	[in] pfault : The fault we want to inject.
//            ]
// Date       [ Ver. 1.0 started 2013/08/11 last modified 2023/01/06 ]
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

	// Put gate into event list.
	if (!processed_[faultyGate])
	{
		events_[pCircuit_->circuitGates_[faultyGate].numLevel_].push(faultyGate);
		processed_[faultyGate] = 1;
	}
}

// **************************************************************************
// Function   [ Simulator::parallelPatternCheckDetection ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Check whether the injected fault can be detected by the pattern.
//              description:
//              	Compare the result of the good simulator and fault simulator and
//              	check whether the injected fault can be detected by the patterns.
//              	If yes, then set its fault state to detected(DT) for fault drop.
//              arguments:
//              	[in] pfault : The fault we want to check.
//            ]
// Date       [ Ver. 1.0 started 2013/08/11 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelPatternCheckDetection(Fault *const pfault)
{
	ParallelValue detected = PARA_L;
	int start = pCircuit_->totalGate_ - pCircuit_->numPO_ - pCircuit_->numPPI_;
	for (int i = start; i < pCircuit_->totalGate_; ++i)
	{
		// TO-DO homework 02
		detected |= ((pCircuit_->circuitGates_[i].goodSimLow_ & pCircuit_->circuitGates_[i].faultSimHigh_) | (pCircuit_->circuitGates_[i].goodSimHigh_ & pCircuit_->circuitGates_[i].faultSimLow_));
		// End of TO-DO
	}
	detected &= activated_; // Check if detected and activated.

	// Set fault state to DT for fault drop.
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
// Function   [ Simulator::parallelPatternSetPattern  ]
// Commenter  [ Bill, PYH ]
// Synopsis   [ usage: Apply patterns to PIs and PPIs.
//              description:
//              	Starting from PatternStartIndex, we apply patterns up to WORD_SIZE
//              	to PIs and PPIs for further fault simulation.
//              arguments:
//              	[in] pPatternProcessor : The patterns generated in ATPG.
//              	[in] PatternStartIndex : Indicate where we start applying patterns
//              	                         in the pattern vector.
//            ]
// Date       [ Ver. 1.0 started 2013/08/11 last modified 2023/01/06 ]
// **************************************************************************
void Simulator::parallelPatternSetPattern(PatternProcessor *pPatternProcessor, const int &patternStartIndex)
{ // TODO LOS not yet supported
	// Reset PI and PPI values to unknowns.
	for (int j = 0; j < pCircuit_->numPI_ + pCircuit_->numPPI_; ++j)
	{
		for (int k = 0; k < pCircuit_->numFrame_; ++k)
		{
			pCircuit_->circuitGates_[j + k * pCircuit_->numGate_].goodSimLow_ = PARA_L;
			pCircuit_->circuitGates_[j + k * pCircuit_->numGate_].goodSimHigh_ = PARA_L;
		}
	}
	// Assign up to WORD_SIZE number of pattern values.
	int endpat = (int)pPatternProcessor->patternVector_.size();
	if (patternStartIndex + (int)WORD_SIZE <= (int)pPatternProcessor->patternVector_.size())
	{
		endpat = patternStartIndex + WORD_SIZE;
	}
	for (int j = patternStartIndex; j < endpat; ++j)
	{
		// Assign PIs.
		if (!pPatternProcessor->patternVector_[j].PI1_.empty())
		{
			for (int k = 0; k < pPatternProcessor->numPI_; ++k)
			{
				if (pPatternProcessor->patternVector_[j].PI1_[k] == L)
				{
					setBitValue(pCircuit_->circuitGates_[k].goodSimLow_, j - patternStartIndex, H);
				}
				else if (pPatternProcessor->patternVector_[j].PI1_[k] == H)
				{
					setBitValue(pCircuit_->circuitGates_[k].goodSimHigh_, j - patternStartIndex, H);
				}
			}
		}

		if (!pPatternProcessor->patternVector_[j].PI2_.empty() && pCircuit_->numFrame_ > 1)
		{
			for (int k = 0; k < pPatternProcessor->numPI_; ++k)
			{
				int index = k + pCircuit_->numGate_;
				if (pPatternProcessor->patternVector_[j].PI2_[k] == L)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimLow_, j - patternStartIndex, H);
				}
				else if (pPatternProcessor->patternVector_[j].PI2_[k] == H)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimHigh_, j - patternStartIndex, H);
				}
			}
		}
		// Assign PPIS
		if (!pPatternProcessor->patternVector_[j].PPI_.empty())
		{
			for (int k = 0; k < pPatternProcessor->numPPI_; ++k)
			{
				int index = k + pCircuit_->numPI_;
				if (pPatternProcessor->patternVector_[j].PPI_[k] == L)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimLow_, j - patternStartIndex, H);
				}
				else if (pPatternProcessor->patternVector_[j].PPI_[k] == H)
				{
					setBitValue(pCircuit_->circuitGates_[index].goodSimHigh_, j - patternStartIndex, H);
				}
			}
		}

		if (!pPatternProcessor->patternVector_[j].SI_.empty() && pCircuit_->numFrame_ > 1 && pCircuit_->timeFrameConnectType_ == Circuit::SHIFT)
		{
			int index = pCircuit_->numGate_ + pCircuit_->numPI_;
			if (pPatternProcessor->patternVector_[j].SI_[0] == L)
			{
				setBitValue(pCircuit_->circuitGates_[index].goodSimLow_, j - patternStartIndex, H);
			}
			else if (pPatternProcessor->patternVector_[j].SI_[0] == H)
			{
				setBitValue(pCircuit_->circuitGates_[index].goodSimHigh_, j - patternStartIndex, H);
			}
		}
	}
}
