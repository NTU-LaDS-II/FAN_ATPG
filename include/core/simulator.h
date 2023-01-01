// **************************************************************************
// File       [ simulator.h ]
// Author     [ littleshamoo ]
// Synopsis   [ it supports two modes: parallel pattern;
//                                     parallel fault
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
// TODO comment for each attribute

namespace CoreNs
{
	class Simulator
	{
	public:
		inline Simulator(Circuit *pCircuit);

		// used by both parallel pattern and parallel fault
		inline void setNumDetection(const int &numDetection); // this for n-detect
		inline void goodSim();
		inline void goodSimCopyGoodToFault();
		inline void goodValueEvaluation(const int &gateID);
		inline void faultValueEvaluation(const int &gateID);
		inline void assignPatternToCircuitInputs(const Pattern &pattern);
		void eventFaultSim();

		// parallel fault simulator
		void parallelFaultFaultSimWithMultiplePattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract);
		void parallelFaultFaultSimWithOnePattern(const Pattern &pattern, FaultPtrList &remainingFaults);
		void parallelFaultFaultSim(FaultPtrList &remainingFaults);

		// parallel pattern simulator
		void parallelPatternGoodSimWithAllPattern(PatternProcessor *pPatternCollector);
		void parallelPatternFaultSimWithPattern(PatternProcessor *pPatternCollector, FaultListExtract *pFaultListExtract);
		void parallelPatternFaultSim(FaultPtrList &remainingFaults);

	private:
		// used by both parallel pattern and parallel fault
		Circuit *pCircuit_;
		int numDetection_; // for n-detect
		int numRecover_;	 // number of recovers needed
		FaultPtrListIter injectedFaults[WORD_SIZE];
		int numInjectedFaults_;
		ParallelValue activated_;
		std::vector<std::stack<int>> events_;
		std::vector<int> processed_;   // array of processed flags.  TRUE means this gate is processed
		std::vector<int> recoverGates_;// array of gates to be recovered from the last fault injection
		//  this is to inject fault into the circuit
		//  faultInjectLow_ = 1 faultInjectHigh_ = 0 inject a stuck-at zero fault
		//  We use 5 ParallelValue since a gate have 1 fanout and at most 4 fanin
		std::vector<std::array<ParallelValue, 5>> faultInjectLow_;
		std::vector<std::array<ParallelValue, 5>> faultInjectHigh_;

		// functions for parallel fault simulator
		void parallelFaultReset();
		bool parallelFaultCheckActivation(const Fault *const pfault);
		void parallelFaultFaultInjection(const Fault *const pfault, const size_t &injectFaultIndex);
		void parallelFaultCheckDetectionDropFaults(FaultPtrList &remainingFaults);

		// functions for parallel pattern simulator
		void parallelPatternReset();
		bool parallelPatternCheckActivation(const Fault *const pfault);
		void parallelPatternFaultInjection(const Fault *const pfault);
		void parallelPatternCheckDetection(Fault *const f);
		void parallelPatternSetPattern(PatternProcessor *pcoll, const int &i);
	};

	inline Simulator::Simulator(Circuit *pCircuit)
			: pCircuit_(pCircuit),
				numDetection_(1),
				numRecover_(0),
				numInjectedFaults_(0),
				activated_(PARA_L),
				events_(pCircuit->totalLvl_),
				processed_(pCircuit->totalGate_, 0),
				recoverGates_(pCircuit->totalGate_),
				faultInjectLow_(pCircuit->totalGate_, std::array<ParallelValue, 5>({0, 0, 0, 0, 0})),
				faultInjectHigh_(pCircuit->totalGate_, std::array<ParallelValue, 5>({0, 0, 0, 0, 0}))
	{
	}

	// **************************************************************************
	// Function   [ Simulator::setNumDetection ]
	// Commenter  [ CJY CBH ]
	// Synopsis   [ usage: set number of detection (default = 1)
	//              in:    #detection
	//              out:   void
	//            ]
	// Date       [ CJY Ver. 1.0 started 2013/08/18 ]
	// **************************************************************************
	inline void Simulator::setNumDetection(const int &numDetection)
	{
		numDetection_ = numDetection;
	}

	// **************************************************************************
	// Function   [ Simulator::goodSim ]
	// Commenter  [ CJY CBH ]
	// Synopsis   [ usage:
	//                sim good value of every gate; call goodEval function for each gate
	//                Please note that we use gl_ and gh_ instead of v_ in each gate.
	//              in:    void
	//              out:   void
	//            ]
	// Date       [ CJY Ver. 1.0 started 2013/08/18 ]
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
	// Commenter  [ CJY CBH]
	// Synopsis   [ usage: call goodValueEvaluation function for each gate
	//                     and copy goodsim result to faultsim variable
	//              in:    void
	//              out:   void
	//            ]
	// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
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
	// Commenter  [ CJY CBH ]
	// Synopsis   [ usage: assign good value from fanin value
	//                     to output of gate(gl and gh)
	//                       PS:  case1:  gl = 1, gh = 0 => 0
	//                            case2:  gl = 0, gh = 1 => 1
	//                            case3:  gl = 0, gh = 0 => X
	//              in:    gate's ID
	//              out:   void
	//            ]
	// Date       [ CJY Ver. 1.0 started 2013/08/18 ]
	// **************************************************************************
	inline void Simulator::goodValueEvaluation(const int &gateID)
	{
		// find number of fanin
		const int fanin1 = pCircuit_->circuitGates_[gateID].numFI_ > 0 ? pCircuit_->circuitGates_[gateID].faninVector_[0] : 0;
		const int fanin2 = pCircuit_->circuitGates_[gateID].numFI_ > 1 ? pCircuit_->circuitGates_[gateID].faninVector_[1] : 0;
		const int fanin3 = pCircuit_->circuitGates_[gateID].numFI_ > 2 ? pCircuit_->circuitGates_[gateID].faninVector_[2] : 0;
		const int fanin4 = pCircuit_->circuitGates_[gateID].numFI_ > 3 ? pCircuit_->circuitGates_[gateID].faninVector_[3] : 0;
		// read value of fanin
		const ParallelValue &l1 = pCircuit_->circuitGates_[fanin1].goodSimLow_;
		const ParallelValue &h1 = pCircuit_->circuitGates_[fanin1].goodSimHigh_;
		const ParallelValue &l2 = pCircuit_->circuitGates_[fanin2].goodSimLow_;
		const ParallelValue &h2 = pCircuit_->circuitGates_[fanin2].goodSimHigh_;
		const ParallelValue &l3 = pCircuit_->circuitGates_[fanin3].goodSimLow_;
		const ParallelValue &h3 = pCircuit_->circuitGates_[fanin3].goodSimHigh_;
		const ParallelValue &l4 = pCircuit_->circuitGates_[fanin4].goodSimLow_;
		const ParallelValue &h4 = pCircuit_->circuitGates_[fanin4].goodSimHigh_;
		// evaluate good value of gate's output
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
				// end of TO-DO
				break;
			case Gate::XOR3:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				// end of TO-DO
				break;
			case Gate::XNOR2:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (l1 & h2) | (l2 & h1);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (l1 & l2) | (h1 & h2);
				// end of TO-DO
				break;
			case Gate::XNOR3:
				// TO-DO homework 01
				pCircuit_->circuitGates_[gateID].goodSimLow_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				pCircuit_->circuitGates_[gateID].goodSimHigh_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				// end of TO-DO
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
	// Function   [ Simulator::faultValueEvaluation ]
	// Commenter  [ CJY CBH ]
	// Synopsis   [ usage: assign faulty value from fanin value
	//                     to output of gate(fl and gh)
	//              in:    gate's ID
	//              out:   void
	//            ]
	// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
	// **************************************************************************
	inline void Simulator::faultValueEvaluation(const int &gateID)
	{
		// find number of fanin
		const int fanin1 = pCircuit_->circuitGates_[gateID].numFI_ > 0 ? pCircuit_->circuitGates_[gateID].faninVector_[0] : 0;
		const int fanin2 = pCircuit_->circuitGates_[gateID].numFI_ > 1 ? pCircuit_->circuitGates_[gateID].faninVector_[1] : 0;
		const int fanin3 = pCircuit_->circuitGates_[gateID].numFI_ > 2 ? pCircuit_->circuitGates_[gateID].faninVector_[2] : 0;
		const int fanin4 = pCircuit_->circuitGates_[gateID].numFI_ > 3 ? pCircuit_->circuitGates_[gateID].faninVector_[3] : 0;
		// read value of fanin with fault masking
		const ParallelValue l1 = (pCircuit_->circuitGates_[fanin1].faultSimLow_ & ~faultInjectHigh_[gateID][1]) | faultInjectLow_[gateID][1];
		const ParallelValue h1 = (pCircuit_->circuitGates_[fanin1].faultSimHigh_ & ~faultInjectLow_[gateID][1]) | faultInjectHigh_[gateID][1];
		const ParallelValue l2 = (pCircuit_->circuitGates_[fanin2].faultSimLow_ & ~faultInjectHigh_[gateID][2]) | faultInjectLow_[gateID][2];
		const ParallelValue h2 = (pCircuit_->circuitGates_[fanin2].faultSimHigh_ & ~faultInjectLow_[gateID][2]) | faultInjectHigh_[gateID][2];
		const ParallelValue l3 = (pCircuit_->circuitGates_[fanin3].faultSimLow_ & ~faultInjectHigh_[gateID][3]) | faultInjectLow_[gateID][3];
		const ParallelValue h3 = (pCircuit_->circuitGates_[fanin3].faultSimHigh_ & ~faultInjectLow_[gateID][3]) | faultInjectHigh_[gateID][3];
		const ParallelValue l4 = (pCircuit_->circuitGates_[fanin4].faultSimLow_ & ~faultInjectHigh_[gateID][4]) | faultInjectLow_[gateID][4];
		const ParallelValue h4 = (pCircuit_->circuitGates_[fanin4].faultSimHigh_ & ~faultInjectLow_[gateID][4]) | faultInjectHigh_[gateID][4];
		// evaluate faulty value of gate's output
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
				// end of TO-DO
				break;
			case Gate::XOR3:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				// end of TO-DO
				break;
			case Gate::XNOR2:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (l1 & h2) | (l2 & h1);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (l1 & l2) | (h1 & h2);
				// end of TO-DO
				break;
			case Gate::XNOR3:
				// TO-DO homework 02
				pCircuit_->circuitGates_[gateID].faultSimLow_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				pCircuit_->circuitGates_[gateID].faultSimHigh_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				// end of TO-DO
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
		// output value of fanout with fault masking
		pCircuit_->circuitGates_[gateID].faultSimLow_ = (pCircuit_->circuitGates_[gateID].faultSimLow_ & ~faultInjectHigh_[gateID][0]) | faultInjectLow_[gateID][0];
		pCircuit_->circuitGates_[gateID].faultSimHigh_ = (pCircuit_->circuitGates_[gateID].faultSimHigh_ & ~faultInjectLow_[gateID][0]) | faultInjectHigh_[gateID][0];
	}

	// **************************************************************************
	// Function   [ Simulator::assignPatternToCircuitInputs ]
	// Commenter  [ HKY CYW ]
	// Synopsis   [ usage: assign test pattern to circuit PI & PPI
	//					   for further fault simulation
	//              in:    Pattern*
	//              out:   void
	//            ]
	// Date       [ HKY Ver. 1.1 started 2014/09/01 ]
	// **************************************************************************
	inline void Simulator::assignPatternToCircuitInputs(const Pattern &pattern)
	{
		// set pattern; apply pattern to PI
		for (int j = 0; j < pCircuit_->numPI_; ++j)
		{
			pCircuit_->circuitGates_[j].goodSimLow_ = PARA_L;
			pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_L;
			if (!pattern.primaryInputs1st_.empty())
			{
				if (pattern.primaryInputs1st_[j] == L)
				{
					pCircuit_->circuitGates_[j].goodSimLow_ = PARA_H;
				}
				else if (pattern.primaryInputs1st_[j] == H)
				{
					pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_H;
				}
			}
			if (pCircuit_->numFrame_ > 1)
			{
				pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimLow_ = PARA_L;
				pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimHigh_ = PARA_L;
				if (!pattern.primaryInputs2nd_.empty())
				{
					if (pattern.primaryInputs2nd_[j] == L)
					{
						pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimLow_ = PARA_H;
					}
					else if (pattern.primaryInputs2nd_[j] == H)
					{
						pCircuit_->circuitGates_[j + pCircuit_->numGate_].goodSimHigh_ = PARA_H;
					}
				}
			}
		}

		// set pattern; apply pattern to PPI
		for (int j = pCircuit_->numPI_; j < pCircuit_->numPI_ + pCircuit_->numPPI_; ++j)
		{
			pCircuit_->circuitGates_[j].goodSimLow_ = PARA_L;
			pCircuit_->circuitGates_[j].goodSimHigh_ = PARA_L;
			if (!pattern.pseudoPrimaryInputs_.empty())
			{
				if (pattern.pseudoPrimaryInputs_[j - pCircuit_->numPI_] == L)
				{
					pCircuit_->circuitGates_[j].goodSimLow_ = PARA_H;
				}
				else if (pattern.pseudoPrimaryInputs_[j - pCircuit_->numPI_] == H)
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
						if (!pattern.shiftIn_.empty())
						{
							if (pattern.shiftIn_[k - 1] == L)
							{
								pCircuit_->circuitGates_[j + pCircuit_->numGate_ * k].goodSimLow_ = PARA_H;
							}
							else if (pattern.shiftIn_[k - 1] == H)
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
