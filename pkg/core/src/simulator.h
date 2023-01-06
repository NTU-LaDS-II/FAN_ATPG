// **************************************************************************
// File       [ simulator.h ]
// Author     [ littleshamoo ]
// Synopsis   [ It supports two modes: parallel pattern ; parallel fault. ]
// Date       [ 2011/09/14 created ]
// **************************************************************************

#ifndef _CORE_SIMULATOR_H_
#define _CORE_SIMULATOR_H_

#include <array>
#include <stack>
#include <vector>

#include "pattern.h"
#include "fault.h"
#include "logic.h"

namespace CoreNs
{
	class Simulator
	{
	public:
		inline Simulator(Circuit *pCircuit);

		// Used by both parallel pattern and parallel fault.
		inline void setNumDetection(const int &numDetection); // This for n-detect.
		inline void goodSim();
		inline void goodSimCopyGoodToFault();
		inline void goodValueEvaluation(const int &gateID);
		inline void faultyValueEvaluation(const int &gateID);
		inline void assignPatternToCircuitInputs(const Pattern &pattern);
		void eventFaultSim();

		// Parallel fault simulator.
		void parallelFaultFaultSimWithAllPattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract);
		void parallelFaultFaultSimWithOnePattern(const Pattern &pattern, FaultPtrList &remainingFaults);
		void parallelFaultFaultSim(FaultPtrList &remainingFaults);

		// Parallel pattern simulator.
		void parallelPatternGoodSimWithAllPattern(PatternProcessor *pPatternCollector);
		void parallelPatternFaultSimWithAllPattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract);
		void parallelPatternFaultSim(FaultPtrList &remainingFaults);

	private:
		// Used by both parallel fault and parallel pattern simulation.
		Circuit *pCircuit_;                   // The circuit use in simulator.
		int numDetection_;                    // For n-detect.
		int numRecover_;                      // Number of recovers needed.
		std::vector<std::stack<int>> events_; // The event stacks for every circuit levels.
		std::vector<int> processed_;          // Array of processed flags. 1 means this gate is processed.
		std::vector<int> recoverGates_;       // Array of gates to be recovered from the last fault injection.
		// This is to inject fault into the circuit.
		// faultInjectLow_ = 1 faultInjectHigh_ = 0 means we inject a stuck-at zero fault.
		// faultInjectLow_ = 0 faultInjectHigh_ = 1 means we inject a stuck-at one fault.
		// We use 5 ParallelValues since a gate have 1 fanout and at most 4 fanins.
		std::vector<std::array<ParallelValue, 5>> faultInjectLow_;
		std::vector<std::array<ParallelValue, 5>> faultInjectHigh_;

		// Used by parallel fault simulation.
		FaultPtrListIter injectedFaults_[WORD_SIZE]; // The injected faults, used for erase detected faults.
		int numInjectedFaults_;                      // The number of injected faults.
		// Used by parallel pattern simulation.
		ParallelValue activated_; // Record which pattern is activated.

		// Functions for parallel fault simulator.
		void parallelFaultReset();
		bool parallelFaultCheckActivation(const Fault *const pfault);
		void parallelFaultFaultInjection(const Fault *const pfault, const size_t &injectFaultIndex);
		void parallelFaultCheckDetectionDropFaults(FaultPtrList &remainingFaults);

		// Functions for parallel pattern simulator.
		void parallelPatternReset();
		bool parallelPatternCheckActivation(const Fault *const pfault);
		void parallelPatternFaultInjection(const Fault *const pfault);
		void parallelPatternCheckDetection(Fault *const pfault);
		void parallelPatternSetPattern(PatternProcessor *pPatternProcessor, const int &patternStartIndex);
	};

	inline Simulator::Simulator(Circuit *pCircuit)
			: pCircuit_(pCircuit),
				numDetection_(1),
				numRecover_(0),
				events_(pCircuit->totalLvl_),
				processed_(pCircuit->totalGate_, 0),
				recoverGates_(pCircuit->totalGate_),
				faultInjectLow_(pCircuit->totalGate_, std::array<ParallelValue, 5>({0, 0, 0, 0, 0})),
				faultInjectHigh_(pCircuit->totalGate_, std::array<ParallelValue, 5>({0, 0, 0, 0, 0})),
				numInjectedFaults_(0),
				activated_(PARA_L)
	{
	}

	// **************************************************************************
	// Function   [ Simulator::setNumDetection ]
	// Commenter  [ CJY, CBH, PYH ]
	// Synopsis   [ usage: Set number of detection (default = 1).
	//              description:
	//              	Set numDetection_ (default = 1) for n-detect.
	//              arguments:
	//              	[in] numDetection : The number of detection.
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/05 ]
	// **************************************************************************
	inline void Simulator::setNumDetection(const int &numDetection)
	{
		numDetection_ = numDetection;
	}

	// **************************************************************************
	// Function   [ Simulator::goodSim ]
	// Commenter  [ CJY, CBH, PYH ]
	// Synopsis   [ usage: Simulate the good value of every gate.
	//              description:
	//              	Call the goodValueEvaluation function for each gate. Here we use
	//              	goodSimLow_ and goodSimHigh_ instead of atpgVal_ in each gate.
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/05 ]
	// **************************************************************************
	inline void Simulator::goodSim()
	{
		for (int gateID = 0; gateID < pCircuit_->totalGate_; ++gateID)
		{
			goodValueEvaluation(gateID);
		}
	}

	// **************************************************************************
	// Function   [ Simulator::goodSimCopyGoodToFault ]
	// Commenter  [ CJY, CBH,PYH ]
	// Synopsis   [ usage: Simulate the good value of every gate and copy to the fault value.
	//              description:
	//              	Call the goodValueEvaluation function for each gate and copy the goodsim
	//              	result to the faultsim variable.
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/05 ]
	// **************************************************************************
	inline void Simulator::goodSimCopyGoodToFault()
	{
		for (int gateID = 0; gateID < pCircuit_->totalGate_; ++gateID)
		{
			goodValueEvaluation(gateID);
			pCircuit_->circuitGates_[gateID].faultSimLow_ = pCircuit_->circuitGates_[gateID].goodSimLow_;
			pCircuit_->circuitGates_[gateID].faultSimHigh_ = pCircuit_->circuitGates_[gateID].goodSimHigh_;
		}
	}

	// **************************************************************************
	// Function   [ Simulator::goodValueEvaluation ]
	// Commenter  [ CJY, CBH,PYH ]
	// Synopsis   [ usage: Assign good value from fanin value to output of gate.
	//              description:
	//              	Evaluate good output value (goodSimLow_ and goodSimHigh_)
	//              	from the fanin values. We have the relationships :
	//              	goodSimLow_ = 1, goodSimHigh_ = 0 => Real value = 0.
	//              	goodSimLow_ = 0, goodSimHigh_ = 1 => Real value = 1.
	//              	goodSimLow_ = 0, goodSimHigh_ = 1 => Real value = X.
	//              arguments:
	//              	[in] gateID : The gate we want to evaluate.
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/18 last modified 2023/01/05 ]
	// **************************************************************************
	inline void Simulator::goodValueEvaluation(const int &gateID)
	{
		// Find the number of fanins.
		const int fanin1 = pCircuit_->circuitGates_[gateID].numFI_ > 0 ? pCircuit_->circuitGates_[gateID].faninVector_[0] : 0;
		const int fanin2 = pCircuit_->circuitGates_[gateID].numFI_ > 1 ? pCircuit_->circuitGates_[gateID].faninVector_[1] : 0;
		const int fanin3 = pCircuit_->circuitGates_[gateID].numFI_ > 2 ? pCircuit_->circuitGates_[gateID].faninVector_[2] : 0;
		const int fanin4 = pCircuit_->circuitGates_[gateID].numFI_ > 3 ? pCircuit_->circuitGates_[gateID].faninVector_[3] : 0;
		// Read the value of fanins.
		const ParallelValue &l1 = pCircuit_->circuitGates_[fanin1].goodSimLow_;
		const ParallelValue &h1 = pCircuit_->circuitGates_[fanin1].goodSimHigh_;
		const ParallelValue &l2 = pCircuit_->circuitGates_[fanin2].goodSimLow_;
		const ParallelValue &h2 = pCircuit_->circuitGates_[fanin2].goodSimHigh_;
		const ParallelValue &l3 = pCircuit_->circuitGates_[fanin3].goodSimLow_;
		const ParallelValue &h3 = pCircuit_->circuitGates_[fanin3].goodSimHigh_;
		const ParallelValue &l4 = pCircuit_->circuitGates_[fanin4].goodSimLow_;
		const ParallelValue &h4 = pCircuit_->circuitGates_[fanin4].goodSimHigh_;
		// Evaluate the good value of gate's output.
		switch (pCircuit_->circuitGates_[gateID].gateType_)
		{
			case Gate::INV:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1;
				break;
			case Gate::AND2:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1 | l2;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1 & h2;
				break;
			case Gate::AND3:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1 | l2 | l3;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1 & h2 & h3;
				break;
			case Gate::AND4:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1 | l2 | l3 | l4;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1 & h2 & h3 & h4;
				break;
			case Gate::NAND2:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1 & h2;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1 | l2;
				break;
			case Gate::NAND3:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1 & h2 & h3;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1 | l2 | l3;
				break;
			case Gate::NAND4:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1 & h2 & h3 & h4;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1 | l2 | l3 | l4;
				break;
			case Gate::OR2:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1 & l2;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1 | h2;
				break;
			case Gate::OR3:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1 & l2 & l3;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1 | h2 | h3;
				break;
			case Gate::OR4:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = l1 & l2 & l3 & l4;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1 | h2 | h3 | h4;
				break;
			case Gate::NOR2:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1 | h2;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1 & l2;
				break;
			case Gate::NOR3:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1 | h2 | h3;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1 & l2 & l3;
				break;
			case Gate::NOR4:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = h1 | h2 | h3 | h4;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = l1 & l2 & l3 & l4;
				break;
			case Gate::XOR2:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (l1 & l2) | (h1 & h2);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (l1 & h2) | (l2 & h1);
				// End of TO-DO
				break;
			case Gate::XOR3:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				// End of TO-DO
				break;
			case Gate::XNOR2:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (l1 & h2) | (l2 & h1);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (l1 & l2) | (h1 & h2);
				// End of TO-DO
				break;
			case Gate::XNOR3:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				// End of TO-DO
				break;
			case Gate::TIE1:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = PARA_L;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = PARA_H;
				break;
			case Gate::TIE0:
				pCircuit_->circuitGates_[gateID].goodSimLow_ = PARA_H;
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = PARA_L;
				break;
			case Gate::PPI:
				if (pCircuit_->timeFrameConnectType_ == Circuit::CAPTURE && pCircuit_->circuitGates_[gateID].frame_ > 0)
				{
					pCircuit_->circuitGates_[gateID].goodSimLow_ = l1;
					pCircuit_->circuitGates_[gateID].goodSimHigh_ = h1;
				}
				break;
			default:
				break;
		}
	}

	// **************************************************************************
	// Function   [ Simulator::faultyValueEvaluation ]
	// Commenter  [ CJY, CBH, PYH ]
	// Synopsis   [ usage: Assign faulty value from fanin value to output of gate.
	//              description:
	//              	Evaluate faulty output value (faultSimLow_ and faultSimHigh_)
	//              	from the fanin values. We have the relationships :
	//              	faultSimLow_ = 1, faultSimHigh_ = 0 => Real value = 0.
	//              	faultSimLow_ = 0, faultSimHigh_ = 1 => Real value = 1.
	//              	faultSimLow_ = 0, faultSimHigh_ = 1 => Real value = X.
	//              	The calculation is similar to GoodValueEvaluation . The difference
	//              	is that there are fault masking at input and output of the gate.
	//              arguments:
	//              	[in] gateID : The gate we want to evaluate.
	//            ]
	// Date       [ Ver. 1.0 started 2013/08/14 last modified 2023/01/06 ]
	// **************************************************************************
	inline void Simulator::faultyValueEvaluation(const int &gateID)
	{
		// Find the number of fanins.
		const int fanin1 = pCircuit_->circuitGates_[gateID].numFI_ > 0 ? pCircuit_->circuitGates_[gateID].faninVector_[0] : 0;
		const int fanin2 = pCircuit_->circuitGates_[gateID].numFI_ > 1 ? pCircuit_->circuitGates_[gateID].faninVector_[1] : 0;
		const int fanin3 = pCircuit_->circuitGates_[gateID].numFI_ > 2 ? pCircuit_->circuitGates_[gateID].faninVector_[2] : 0;
		const int fanin4 = pCircuit_->circuitGates_[gateID].numFI_ > 3 ? pCircuit_->circuitGates_[gateID].faninVector_[3] : 0;
		// Read the value of fanins with fault masking.
		const ParallelValue l1 = (pCircuit_->circuitGates_[fanin1].faultSimLow_ & ~faultInjectHigh_[gateID][1]) | faultInjectLow_[gateID][1];
		const ParallelValue h1 = (pCircuit_->circuitGates_[fanin1].faultSimHigh_ & ~faultInjectLow_[gateID][1]) | faultInjectHigh_[gateID][1];
		const ParallelValue l2 = (pCircuit_->circuitGates_[fanin2].faultSimLow_ & ~faultInjectHigh_[gateID][2]) | faultInjectLow_[gateID][2];
		const ParallelValue h2 = (pCircuit_->circuitGates_[fanin2].faultSimHigh_ & ~faultInjectLow_[gateID][2]) | faultInjectHigh_[gateID][2];
		const ParallelValue l3 = (pCircuit_->circuitGates_[fanin3].faultSimLow_ & ~faultInjectHigh_[gateID][3]) | faultInjectLow_[gateID][3];
		const ParallelValue h3 = (pCircuit_->circuitGates_[fanin3].faultSimHigh_ & ~faultInjectLow_[gateID][3]) | faultInjectHigh_[gateID][3];
		const ParallelValue l4 = (pCircuit_->circuitGates_[fanin4].faultSimLow_ & ~faultInjectHigh_[gateID][4]) | faultInjectLow_[gateID][4];
		const ParallelValue h4 = (pCircuit_->circuitGates_[fanin4].faultSimHigh_ & ~faultInjectLow_[gateID][4]) | faultInjectHigh_[gateID][4];
		// Evaluate the faulty value of gate's output.
		switch (pCircuit_->circuitGates_[gateID].gateType_)
		{
			case Gate::INV:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1;
				break;
			case Gate::AND2:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1 | l2;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1 & h2;
				break;
			case Gate::AND3:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1 | l2 | l3;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1 & h2 & h3;
				break;
			case Gate::AND4:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1 | l2 | l3 | l4;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1 & h2 & h3 & h4;
				break;
			case Gate::NAND2:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1 & h2;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1 | l2;
				break;
			case Gate::NAND3:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1 & h2 & h3;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1 | l2 | l3;
				break;
			case Gate::NAND4:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1 & h2 & h3 & h4;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1 | l2 | l3 | l4;
				break;
			case Gate::OR2:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1 & l2;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1 | h2;
				break;
			case Gate::OR3:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1 & l2 & l3;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1 | h2 | h3;
				break;
			case Gate::OR4:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = l1 & l2 & l3 & l4;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1 | h2 | h3 | h4;
				break;
			case Gate::NOR2:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1 | h2;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1 & l2;
				break;
			case Gate::NOR3:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1 | h2 | h3;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1 & l2 & l3;
				break;
			case Gate::NOR4:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = h1 | h2 | h3 | h4;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = l1 & l2 & l3 & l4;
				break;
			case Gate::XOR2:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (l1 & l2) | (h1 & h2);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (l1 & h2) | (l2 & h1);
				// End of TO-DO
				break;
			case Gate::XOR3:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				// End of TO-DO
				break;
			case Gate::XNOR2:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (l1 & h2) | (l2 & h1);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (l1 & l2) | (h1 & h2);
				// End of TO-DO
				break;
			case Gate::XNOR3:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				// End of TO-DO
				break;
			case Gate::TIE1:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = PARA_L;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = PARA_H;
				break;
			case Gate::TIE0:
				pCircuit_->circuitGates_[gateID].faultSimLow_ = PARA_H;
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = PARA_L;
				break;
			case Gate::PPI:
				if (pCircuit_->timeFrameConnectType_ == Circuit::CAPTURE && pCircuit_->circuitGates_[gateID].frame_ > 0)
				{
					pCircuit_->circuitGates_[gateID].faultSimLow_ = l1;
					pCircuit_->circuitGates_[gateID].faultSimHigh_ = h1;
				}
				break;
			default:
				break;
		}
		// Output the value of fanout with fault masking.
		pCircuit_->circuitGates_[gateID].faultSimLow_ = (pCircuit_->circuitGates_[gateID].faultSimLow_ & ~faultInjectHigh_[gateID][0]) | faultInjectLow_[gateID][0];
		pCircuit_->circuitGates_[gateID].faultSimHigh_ = (pCircuit_->circuitGates_[gateID].faultSimHigh_ & ~faultInjectLow_[gateID][0]) | faultInjectHigh_[gateID][0];
	}

	// **************************************************************************
	// Function   [ Simulator::assignPatternToCircuitInputs ]
	// Commenter  [ HKY, CYW, PYH ]
	// Synopsis   [ usage: Assign test pattern to circuit PI & PPI.
	//              description:
	//              	Assign test pattern to circuit PI & PPI for further fault
	//              	simulation.
	//              arguments:
	//              	[in] pattern : The pattern we want to assign.
	//            ]
	// Date       [ Ver. 1.1 started 2014/09/01 last modified 2023/01/06 ]
	// **************************************************************************
	inline void Simulator::assignPatternToCircuitInputs(const Pattern &pattern)
	{
		// Set pattern : Apply the pattern to PIs.
		for (int j = 0; j < pCircuit_->numPI_; ++j)
		{
			pCircuit_->circuitGates_[j].goodSimLow_ = PARA_L;
			pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_L;
			if (!pattern.PI1_.empty())
			{
				if (pattern.PI1_[j] == L)
				{
					pCircuit_->circuitGates_[j].goodSimLow_ = PARA_H;
				}
				else if (pattern.PI1_[j] == H)
				{
					pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_H;
				}
			}
			if (pCircuit_->numFrame_ > 1)
			{
				pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimLow_ = PARA_L;
				pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimHigh_ = PARA_L;
				if (!pattern.PI2_.empty())
				{
					if (pattern.PI2_[j] == L)
					{
						pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimLow_ = PARA_H;
					}
					else if (pattern.PI2_[j] == H)
					{
						pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimHigh_ = PARA_H;
					}
				}
			}
		}

		// Set pattern : Apply the pattern to PPIs.
		for (int j = pCircuit_->numPI_; j < pCircuit_->numPI_ + pCircuit_->numPPI_; ++j)
		{
			pCircuit_->circuitGates_[j].goodSimLow_ = PARA_L;
			pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_L;
			if (!pattern.PPI_.empty())
			{
				if (pattern.PPI_[j - pCircuit_->numPI_] == L)
				{
					pCircuit_->circuitGates_[j].goodSimLow_ = PARA_H;
				}
				else if (pattern.PPI_[j - pCircuit_->numPI_] == H)
				{
					pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_H;
				}
			}
			if (pCircuit_->timeFrameConnectType_ == Circuit::SHIFT && pCircuit_->numFrame_ > 1)
			{
				for (int k = 1; k < pCircuit_->numFrame_; ++k)
				{
					pCircuit_->circuitGates_[j + pCircuit_->numGate_ * k].goodSimLow_ = PARA_L;
					pCircuit_->circuitGates_[j + pCircuit_->numGate_ * k].goodSimHigh_ = PARA_L;
					if (j == pCircuit_->numPI_)
					{
						if (!pattern.SI_.empty())
						{
							if (pattern.SI_[k - 1] == L)
							{
								pCircuit_->circuitGates_[j + pCircuit_->numGate_ * k].goodSimLow_ = PARA_H;
							}
							else if (pattern.SI_[k - 1] == H)
							{
								pCircuit_->circuitGates_[j + pCircuit_->numGate_ * k].goodSimHigh_ = PARA_H;
							}
						}
					}
					else
					{
					}
				}
			}
		}
	}
};

#endif
