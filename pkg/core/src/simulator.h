// **************************************************************************
// File       [ simulator.h ]
// Author     [ littleshamoo ]
// Synopsis   [ it supports two modes: pp means parallel pattern;
//                                     pf means parallel fault
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
		Simulator(Circuit *cir);
		// ~Simulator();

		// used by both parallel pattern and parallel fault
		void setNdet(const int &ndet); // this for n-detect
		void goodSim();
		void goodSimCopyToFault();
		void goodEval(const int &i);
		void faultEval(const int &i);
		void assignPatternToPi(const Pattern &p);

		void eventFaultSim();

		// parallel fault
		void pfFaultSim(PatternProcessor *pcoll, FaultListExtract *fListExtract);
		void pfFaultSim(const Pattern &p, FaultPtrList &remain);
		void pfFaultSim(FaultPtrList &remain);

		// parallel pattern
		void ppGoodSim(PatternProcessor *pcoll);
		void ppFaultSim(PatternProcessor *pcoll, FaultListExtract *fListExtract);
		void ppFaultSim(FaultPtrList &remain);

	private:
		// used by both parallel pattern and parallel fault
		Circuit *cir_;
		int ndet_;		 // for n-detect
		int nrecover_; // number of recovers needed
		FaultPtrListIter injected_[WORD_SIZE];
		int ninjected_;
		ParaValue activated_;
		std::vector<std::stack<int>> events_;
		std::vector<bool> processed_; // array of processed flags.  TRUE means this gate is processed
		std::vector<int> recover_;		// array of gates to be recovered from the last fault injection
																	//  this is to inject fault into the circuit
																	//  faultInjectL_ =1 faultInjectH_ =0 inject a stuck-at zero fault
		std::vector<std::array<ParaValue, 5>> faultInjectL_;
		std::vector<std::array<ParaValue, 5>> faultInjectH_;

		// parallel fault
		void pfReset();
		bool pfCheckActivation(const Fault *const f);
		void pfInject(const Fault *const f, const size_t &i);
		void pfCheckDetection(FaultPtrList &remain);

		// parallel pattern
		void ppReset();
		bool ppCheckActivation(const Fault *const f);
		void ppInject(const Fault *const f);
		void ppCheckDetection(Fault *const f);
		void ppSetPattern(PatternProcessor *pcoll, const int &i);
	};

	inline Simulator::Simulator(Circuit *cir)
			: cir_(cir),
				ndet_(1),
				nrecover_(0),
				ninjected_(0),
				activated_(PARA_L),
				events_(cir->totalLvl_),
				processed_(cir->totalGate_, false),
				recover_(cir->totalGate_),
				faultInjectL_(cir->totalGate_, std::array<ParaValue, 5>({0, 0, 0, 0, 0})),
				faultInjectH_(cir->totalGate_, std::array<ParaValue, 5>({0, 0, 0, 0, 0}))
	{
		// used by both parallel pattern and parallel fault
		// cir_ = cir;
		// ndet_ = 1;
		// events_ = new std::stack<int>[cir_->totalLvl_];
		// processed_ = new bool[cir_->totalGate_];
		// recover_ = new int[cir_->totalGate_];
		// nrecover_ = 0;
		// faultInjectL_ = new ParaValue[cir_->totalGate_][5];
		// faultInjectH_ = new ParaValue[cir_->totalGate_][5];

		// memset(processed_, 0, cir_->totalGate_ * sizeof(bool));
		// memset(faultInjectL_, 0, cir_->totalGate_ * 5 * sizeof(ParaValue));
		// memset(faultInjectH_, 0, cir_->totalGate_ * 5 * sizeof(ParaValue));

		// parallel fault
		// ninjected_ = 0;

		// parallel pattern
		// activated_ = PARA_L;
	}

	// inline Simulator::~Simulator()
	// {
	// 	// delete[] events_;
	// 	// delete[] processed_;
	// 	// delete[] recover_;
	// 	// delete[] faultInjectL_;
	// 	// delete[] faultInjectH_;
	// }

	// Make error if goodSim(), assignPatternToPi(const Pattern &p), (goodEval(const int &i))
	// are defined in simulator.cpp. Looks like undefined reference to the function
	// Another make error if eventFaultSim() is defined in simulator.h.
	// Looks like multiple definition of function

	// **************************************************************************
	// Function   [ Simulator::setNdet ]
	// Commentor  [ CJY CBH ]
	// Synopsis   [ usage: set number of detection (default = 1)
	//              in:    #detection
	//              out:   void
	//            ]
	// Date       [ CJY Ver. 1.0 started 2013/08/18 ]
	// **************************************************************************
	inline void Simulator::setNdet(const int &ndet)
	{
		ndet_ = ndet;
	}

	// **************************************************************************
	// Function   [ Simulator::goodSim ]
	// Commentor  [ CJY CBH ]
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
		for (int i = 0; i < cir_->totalGate_; ++i)
		{
			goodEval(i);
		}
	}

	// **************************************************************************
	// Function   [ Simulator::goodSimCopyToFault ]
	// Commentor  [ CJY CBH]
	// Synopsis   [ usage: call goodEval function for each gate
	//                     and copy goodsim result to faultsim variable
	//              in:    void
	//              out:   void
	//            ]
	// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
	// **************************************************************************
	inline void Simulator::goodSimCopyToFault()
	{
		for (int i = 0; i < cir_->totalGate_; ++i)
		{
			goodEval(i);
			cir_->circuitGates_[i].faultSimLow_ = cir_->circuitGates_[i].goodSimLow_;
			cir_->circuitGates_[i].faultSimHigh_ = cir_->circuitGates_[i].goodSimHigh_;
		}
	}

	// **************************************************************************
	// Function   [ Simulator::goodEval ]
	// Commentor  [ CJY CBH ]
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
	inline void Simulator::goodEval(const int &i)
	{
		// find number of fanin
		const int fi1 = cir_->circuitGates_[i].numFI_ > 0 ? cir_->circuitGates_[i].faninVector_[0] : 0;
		const int fi2 = cir_->circuitGates_[i].numFI_ > 1 ? cir_->circuitGates_[i].faninVector_[1] : 0;
		const int fi3 = cir_->circuitGates_[i].numFI_ > 2 ? cir_->circuitGates_[i].faninVector_[2] : 0;
		const int fi4 = cir_->circuitGates_[i].numFI_ > 3 ? cir_->circuitGates_[i].faninVector_[3] : 0;
		// read value of fanin
		const ParaValue &l1 = cir_->circuitGates_[fi1].goodSimLow_;
		const ParaValue &h1 = cir_->circuitGates_[fi1].goodSimHigh_;
		const ParaValue &l2 = cir_->circuitGates_[fi2].goodSimLow_;
		const ParaValue &h2 = cir_->circuitGates_[fi2].goodSimHigh_;
		const ParaValue &l3 = cir_->circuitGates_[fi3].goodSimLow_;
		const ParaValue &h3 = cir_->circuitGates_[fi3].goodSimHigh_;
		const ParaValue &l4 = cir_->circuitGates_[fi4].goodSimLow_;
		const ParaValue &h4 = cir_->circuitGates_[fi4].goodSimHigh_;
		// evaluate good value of gate's output
		switch (cir_->circuitGates_[i].gateType_)
		{
			case Gate::INV:
				cir_->circuitGates_[i].goodSimLow_ = h1;
				cir_->circuitGates_[i].goodSimHigh_ = l1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				cir_->circuitGates_[i].goodSimLow_ = l1;
				cir_->circuitGates_[i].goodSimHigh_ = h1;
				break;
			case Gate::AND2:
				cir_->circuitGates_[i].goodSimLow_ = l1 | l2;
				cir_->circuitGates_[i].goodSimHigh_ = h1 & h2;
				break;
			case Gate::AND3:
				cir_->circuitGates_[i].goodSimLow_ = l1 | l2 | l3;
				cir_->circuitGates_[i].goodSimHigh_ = h1 & h2 & h3;
				break;
			case Gate::AND4:
				cir_->circuitGates_[i].goodSimLow_ = l1 | l2 | l3 | l4;
				cir_->circuitGates_[i].goodSimHigh_ = h1 & h2 & h3 & h4;
				break;
			case Gate::NAND2:
				cir_->circuitGates_[i].goodSimLow_ = h1 & h2;
				cir_->circuitGates_[i].goodSimHigh_ = l1 | l2;
				break;
			case Gate::NAND3:
				cir_->circuitGates_[i].goodSimLow_ = h1 & h2 & h3;
				cir_->circuitGates_[i].goodSimHigh_ = l1 | l2 | l3;
				break;
			case Gate::NAND4:
				cir_->circuitGates_[i].goodSimLow_ = h1 & h2 & h3 & h4;
				cir_->circuitGates_[i].goodSimHigh_ = l1 | l2 | l3 | l4;
				break;
			case Gate::OR2:
				cir_->circuitGates_[i].goodSimLow_ = l1 & l2;
				cir_->circuitGates_[i].goodSimHigh_ = h1 | h2;
				break;
			case Gate::OR3:
				cir_->circuitGates_[i].goodSimLow_ = l1 & l2 & l3;
				cir_->circuitGates_[i].goodSimHigh_ = h1 | h2 | h3;
				break;
			case Gate::OR4:
				cir_->circuitGates_[i].goodSimLow_ = l1 & l2 & l3 & l4;
				cir_->circuitGates_[i].goodSimHigh_ = h1 | h2 | h3 | h4;
				break;
			case Gate::NOR2:
				cir_->circuitGates_[i].goodSimLow_ = h1 | h2;
				cir_->circuitGates_[i].goodSimHigh_ = l1 & l2;
				break;
			case Gate::NOR3:
				cir_->circuitGates_[i].goodSimLow_ = h1 | h2 | h3;
				cir_->circuitGates_[i].goodSimHigh_ = l1 & l2 & l3;
				break;
			case Gate::NOR4:
				cir_->circuitGates_[i].goodSimLow_ = h1 | h2 | h3 | h4;
				cir_->circuitGates_[i].goodSimHigh_ = l1 & l2 & l3 & l4;
				break;
			case Gate::XOR2:
				// TO-DO homework 01
				cir_->circuitGates_[i].goodSimLow_ = (l1 & l2) | (h1 & h2);
				cir_->circuitGates_[i].goodSimHigh_ = (l1 & h2) | (l2 & h1);
				// end of TO-DO
				break;
			case Gate::XOR3:
				// TO-DO homework 01
				cir_->circuitGates_[i].goodSimLow_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				cir_->circuitGates_[i].goodSimHigh_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				// end of TO-DO
				break;
			case Gate::XNOR2:
				// TO-DO homework 01
				cir_->circuitGates_[i].goodSimLow_ = (l1 & h2) | (l2 & h1);
				cir_->circuitGates_[i].goodSimHigh_ = (l1 & l2) | (h1 & h2);
				// end of TO-DO
				break;
			case Gate::XNOR3:
				// TO-DO homework 01
				cir_->circuitGates_[i].goodSimLow_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				cir_->circuitGates_[i].goodSimHigh_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				// end of TO-DO
				break;
			case Gate::TIE1:
				cir_->circuitGates_[i].goodSimLow_ = PARA_L;
				cir_->circuitGates_[i].goodSimHigh_ = PARA_H;
				break;
			case Gate::TIE0:
				cir_->circuitGates_[i].goodSimLow_ = PARA_H;
				cir_->circuitGates_[i].goodSimHigh_ = PARA_L;
				break;
			case Gate::PPI:
				if (cir_->timeFrameConnectType_ == Circuit::CAPTURE && cir_->circuitGates_[i].frame_ > 0)
				{
					cir_->circuitGates_[i].goodSimLow_ = l1;
					cir_->circuitGates_[i].goodSimHigh_ = h1;
				}
				break;
			default:
				break;
		}
	}

	// **************************************************************************
	// Function   [ Simulator::faultEval ]
	// Commentor  [ CJY CBH ]
	// Synopsis   [ usage: assign faulty value from fanin value
	//                     to output of gate(fl and gh)
	//              in:    gate's ID
	//              out:   void
	//            ]
	// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
	// **************************************************************************
	inline void Simulator::faultEval(const int &i)
	{
		// find number of fanin
		const int fi1 = cir_->circuitGates_[i].numFI_ > 0 ? cir_->circuitGates_[i].faninVector_[0] : 0;
		const int fi2 = cir_->circuitGates_[i].numFI_ > 1 ? cir_->circuitGates_[i].faninVector_[1] : 0;
		const int fi3 = cir_->circuitGates_[i].numFI_ > 2 ? cir_->circuitGates_[i].faninVector_[2] : 0;
		const int fi4 = cir_->circuitGates_[i].numFI_ > 3 ? cir_->circuitGates_[i].faninVector_[3] : 0;
		// read value of fanin with fault masking
		const ParaValue l1 = (cir_->circuitGates_[fi1].faultSimLow_ & ~faultInjectH_[i][1]) | faultInjectL_[i][1];
		const ParaValue h1 = (cir_->circuitGates_[fi1].faultSimHigh_ & ~faultInjectL_[i][1]) | faultInjectH_[i][1];
		const ParaValue l2 = (cir_->circuitGates_[fi2].faultSimLow_ & ~faultInjectH_[i][2]) | faultInjectL_[i][2];
		const ParaValue h2 = (cir_->circuitGates_[fi2].faultSimHigh_ & ~faultInjectL_[i][2]) | faultInjectH_[i][2];
		const ParaValue l3 = (cir_->circuitGates_[fi3].faultSimLow_ & ~faultInjectH_[i][3]) | faultInjectL_[i][3];
		const ParaValue h3 = (cir_->circuitGates_[fi3].faultSimHigh_ & ~faultInjectL_[i][3]) | faultInjectH_[i][3];
		const ParaValue l4 = (cir_->circuitGates_[fi4].faultSimLow_ & ~faultInjectH_[i][4]) | faultInjectL_[i][4];
		const ParaValue h4 = (cir_->circuitGates_[fi4].faultSimHigh_ & ~faultInjectL_[i][4]) | faultInjectH_[i][4];
		// evaluate faulty value of gate's output
		switch (cir_->circuitGates_[i].gateType_)
		{
			case Gate::INV:
				cir_->circuitGates_[i].faultSimLow_ = h1;
				cir_->circuitGates_[i].faultSimHigh_ = l1;
				break;
			case Gate::PO:
			case Gate::PPO:
			case Gate::BUF:
				cir_->circuitGates_[i].faultSimLow_ = l1;
				cir_->circuitGates_[i].faultSimHigh_ = h1;
				break;
			case Gate::AND2:
				cir_->circuitGates_[i].faultSimLow_ = l1 | l2;
				cir_->circuitGates_[i].faultSimHigh_ = h1 & h2;
				break;
			case Gate::AND3:
				cir_->circuitGates_[i].faultSimLow_ = l1 | l2 | l3;
				cir_->circuitGates_[i].faultSimHigh_ = h1 & h2 & h3;
				break;
			case Gate::AND4:
				cir_->circuitGates_[i].faultSimLow_ = l1 | l2 | l3 | l4;
				cir_->circuitGates_[i].faultSimHigh_ = h1 & h2 & h3 & h4;
				break;
			case Gate::NAND2:
				cir_->circuitGates_[i].faultSimLow_ = h1 & h2;
				cir_->circuitGates_[i].faultSimHigh_ = l1 | l2;
				break;
			case Gate::NAND3:
				cir_->circuitGates_[i].faultSimLow_ = h1 & h2 & h3;
				cir_->circuitGates_[i].faultSimHigh_ = l1 | l2 | l3;
				break;
			case Gate::NAND4:
				cir_->circuitGates_[i].faultSimLow_ = h1 & h2 & h3 & h4;
				cir_->circuitGates_[i].faultSimHigh_ = l1 | l2 | l3 | l4;
				break;
			case Gate::OR2:
				cir_->circuitGates_[i].faultSimLow_ = l1 & l2;
				cir_->circuitGates_[i].faultSimHigh_ = h1 | h2;
				break;
			case Gate::OR3:
				cir_->circuitGates_[i].faultSimLow_ = l1 & l2 & l3;
				cir_->circuitGates_[i].faultSimHigh_ = h1 | h2 | h3;
				break;
			case Gate::OR4:
				cir_->circuitGates_[i].faultSimLow_ = l1 & l2 & l3 & l4;
				cir_->circuitGates_[i].faultSimHigh_ = h1 | h2 | h3 | h4;
				break;
			case Gate::NOR2:
				cir_->circuitGates_[i].faultSimLow_ = h1 | h2;
				cir_->circuitGates_[i].faultSimHigh_ = l1 & l2;
				break;
			case Gate::NOR3:
				cir_->circuitGates_[i].faultSimLow_ = h1 | h2 | h3;
				cir_->circuitGates_[i].faultSimHigh_ = l1 & l2 & l3;
				break;
			case Gate::NOR4:
				cir_->circuitGates_[i].faultSimLow_ = h1 | h2 | h3 | h4;
				cir_->circuitGates_[i].faultSimHigh_ = l1 & l2 & l3 & l4;
				break;
			case Gate::XOR2:
				// TO-DO homework 02
				cir_->circuitGates_[i].faultSimLow_ = (l1 & l2) | (h1 & h2);
				cir_->circuitGates_[i].faultSimHigh_ = (l1 & h2) | (l2 & h1);
				// end of TO-DO
				break;
			case Gate::XOR3:
				// TO-DO homework 02
				cir_->circuitGates_[i].faultSimLow_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				cir_->circuitGates_[i].faultSimHigh_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				// end of TO-DO
				break;
			case Gate::XNOR2:
				// TO-DO homework 02
				cir_->circuitGates_[i].faultSimLow_ = (l1 & h2) | (l2 & h1);
				cir_->circuitGates_[i].faultSimHigh_ = (l1 & l2) | (h1 & h2);
				// end of TO-DO
				break;
			case Gate::XNOR3:
				// TO-DO homework 02
				cir_->circuitGates_[i].faultSimLow_ = (h1 & l2 & l3) | (l1 & h2 & l3) | (l1 & l2 & h3) | (h1 & h2 & h3);
				cir_->circuitGates_[i].faultSimHigh_ = (l1 & l2 & l3) | (l1 & h2 & h3) | (h1 & l2 & h3) | (h1 & h2 & l3);
				// end of TO-DO
				break;
			case Gate::TIE1:
				cir_->circuitGates_[i].faultSimLow_ = PARA_L;
				cir_->circuitGates_[i].faultSimHigh_ = PARA_H;
				break;
			case Gate::TIE0:
				cir_->circuitGates_[i].faultSimLow_ = PARA_H;
				cir_->circuitGates_[i].faultSimHigh_ = PARA_L;
				break;
			case Gate::PPI:
				if (cir_->timeFrameConnectType_ == Circuit::CAPTURE && cir_->circuitGates_[i].frame_ > 0)
				{
					cir_->circuitGates_[i].faultSimLow_ = l1;
					cir_->circuitGates_[i].faultSimHigh_ = h1;
				}
				break;
			default:
				break;
		}
		cir_->circuitGates_[i].faultSimLow_ = (cir_->circuitGates_[i].faultSimLow_ & ~faultInjectH_[i][0]) | faultInjectL_[i][0];
		cir_->circuitGates_[i].faultSimHigh_ = (cir_->circuitGates_[i].faultSimHigh_ & ~faultInjectL_[i][0]) | faultInjectH_[i][0];
	}

	// **************************************************************************
	// Function   [ Simulator::assignPatternToPi ]
	// Commentor  [ HKY CYW ]
	// Synopsis   [ usage: assign test pattern to circuit PI & PPI
	//					   for further fault simulation
	//              in:    Pattern*
	//              out:   void
	//            ]
	// Date       [ HKY Ver. 1.1 started 2014/09/01 ]
	// **************************************************************************
	inline void Simulator::assignPatternToPi(const Pattern &pattern)
	{
		// set pattern; apply pattern to PI
		for (int j = 0; j < cir_->numPI_; ++j)
		{
			cir_->circuitGates_[j].goodSimLow_ = PARA_L;
			cir_->circuitGates_[j].goodSimHigh_ = PARA_L;
			if (!pattern.primaryInputs1st_.empty())
			{
				if (pattern.primaryInputs1st_[j] == L)
				{
					cir_->circuitGates_[j].goodSimLow_ = PARA_H;
				}
				else if (pattern.primaryInputs1st_[j] == H)
				{
					cir_->circuitGates_[j].goodSimHigh_ = PARA_H;
				}
			}
			if (cir_->numFrame_ > 1)
			{
				cir_->circuitGates_[j + cir_->numGate_].goodSimLow_ = PARA_L;
				cir_->circuitGates_[j + cir_->numGate_].goodSimHigh_ = PARA_L;
				if (!pattern.primaryInputs2nd_.empty())
				{
					if (pattern.primaryInputs2nd_[j] == L)
					{
						cir_->circuitGates_[j + cir_->numGate_].goodSimLow_ = PARA_H;
					}
					else if (pattern.primaryInputs2nd_[j] == H)
					{
						cir_->circuitGates_[j + cir_->numGate_].goodSimHigh_ = PARA_H;
					}
				}
			}
		}

		// set pattern; apply pattern to PPI
		for (int j = cir_->numPI_; j < cir_->numPI_ + cir_->numPPI_; ++j)
		{
			cir_->circuitGates_[j].goodSimLow_ = PARA_L;
			cir_->circuitGates_[j].goodSimHigh_ = PARA_L;
			if (!pattern.pseudoPrimaryInputs_.empty())
			{
				if (pattern.pseudoPrimaryInputs_[j - cir_->numPI_] == L)
				{
					cir_->circuitGates_[j].goodSimLow_ = PARA_H;
				}
				else if (pattern.pseudoPrimaryInputs_[j - cir_->numPI_] == H)
				{
					cir_->circuitGates_[j].goodSimHigh_ = PARA_H;
				}
			}
			if (cir_->timeFrameConnectType_ == Circuit::SHIFT && cir_->numFrame_ > 1)
			{
				for (int k = 1; k < cir_->numFrame_; ++k)
				{
					cir_->circuitGates_[j + cir_->numGate_ * k].goodSimLow_ = PARA_L;
					cir_->circuitGates_[j + cir_->numGate_ * k].goodSimHigh_ = PARA_L;
					if (j == cir_->numPI_)
					{
						if (!pattern.shiftIn_.empty())
						{
							if (pattern.shiftIn_[k - 1] == L)
							{
								cir_->circuitGates_[j + cir_->numGate_ * k].goodSimLow_ = PARA_H;
							}
							else if (pattern.shiftIn_[k - 1] == H)
							{
								cir_->circuitGates_[j + cir_->numGate_ * k].goodSimHigh_ = PARA_H;
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
