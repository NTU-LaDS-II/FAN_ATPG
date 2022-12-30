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
// Commentor  [ CJY CBH ]
// Synopsis   [ usage: call faultEval function whose gate's ID in
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
	for (int i = 0; i < cir_->totalLvl_; ++i)
	{
		while (!events_[i].empty())
		{
			int gid = events_[i].top(); // get gate's ID
			events_[i].pop();
			processed_[gid] = false;
			faultEval(gid);
			recover_[nrecover_] = gid; // record gate's ID
			++nrecover_;

			// check whether faulty value and good value are equal
			if (cir_->circuitGates_[gid].faultSimLow_ == cir_->circuitGates_[gid].goodSimLow_ && cir_->circuitGates_[gid].faultSimHigh_ == cir_->circuitGates_[gid].goodSimHigh_)
			{
				continue;
			}

			// if not equal and flag = false. set flag true
			for (int j = 0; j < cir_->circuitGates_[gid].numFO_; ++j)
			{
				int foid = cir_->circuitGates_[gid].fanoutVector_[j];
				if (processed_[foid])
				{
					continue;
				}
				events_[cir_->circuitGates_[foid].numLevel_].push(foid);
				processed_[foid] = true;
			}
		}
	}
}

// **************************************************************************
// Function   [ void Simulator::pfFaultSim(PatternProcessor *, FaultListExtract *) ]
// Author     [ littleshamoo ]
// Synopsis   [ usage: perform parallel fault simulation on all patterns.
//              call pfFaultSim(FaultPtrList &remain) for each pattern
//              in:	   a set of pattern
//              out:   void //TODO not void
//            ]
// Date       [ ]
// **************************************************************************
void Simulator::pfFaultSim(PatternProcessor *pcoll, FaultListExtract *fListExtract)
{
	// undetected faults are remaining faults
	FaultPtrList remain;
	for (Fault *const &pFault : fListExtract->faultsInCircuit_)
		if (pFault->faultState_ != Fault::DT && pFault->faultState_ != Fault::RE && pFault->faultyLine_ >= 0)
			remain.push_back(pFault);

	// simulate all patterns for all faults
	for (const Pattern &pattern : pcoll->patternVector_)
	{
		if (remain.size() == 0)
		{
			break;
		}

		// Assign pattern to circuit PI & PPI for further fault sim
		assignPatternToPi(pattern);
		pfFaultSim(remain);
	}
}

// **************************************************************************
// Function   [ Simulator::pfFaultSim ]
// Commentor  [ CJY CBH ]
// Synopsis   [ usage: set pattern. call pfFaultSim(FaultPtrList &remain)
//                     to do faultsim for this pattern
//              in:    one pattern , FaultPtrList contain undetected faults
//              out:   void //TODO not void
//            ]
// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
// **************************************************************************
void Simulator::pfFaultSim(const Pattern &p, FaultPtrList &remain)
{

	// Assign pattern to circuit PI & PPI for further fault sim
	assignPatternToPi(p);
	pfFaultSim(remain);
}

// **************************************************************************
// Function   [ Simulator::pfFaultSim ]
// Commentor  [ CJY CBH ]
// Synopsis   [ usage: do faultsim for the fault in fault list
//              in:    FaultPtrList ( = list<Fault *> ) contains undetected faults
//              out:   void //TODO not void
//            ]
// Date       [ CJY Ver. 1.0 started 2013/08/14 ]
// **************************************************************************
void Simulator::pfFaultSim(FaultPtrList &remain)
{
	if (remain.size() == 0)
	{
		return;
	}

	goodSimCopyToFault(); // run good simulation first

	// inject number of WORD_SIZE activated faults
	// pfReset();
	FaultPtrListIter it = remain.begin();
	while (it != remain.end()) // do while => while by wang
	{
		// if fault is activated, inject fault
		if (pfCheckActivation(*it))
		{
			pfInject(*it, ninjected_);
			injected_[ninjected_++] = it;
		}
		++it;
		// run fault sim if enough fault or end of fault list
		if (ninjected_ == (int)WORD_SIZE || (it == remain.end() && ninjected_ > 0))
		{
			eventFaultSim();
			pfCheckDetection(remain); // drop fault here
			pfReset();
		}
	}
}

// **************************************************************************
// Function   [ ppGoodSim ]
// Commentor  [ Bill ]
// Synopsis   [ usage: for each pattern generated,
//					   apply them separately and run good circuit simulation
//              in:    all the patterns generated by atpg
//              out:   void //TODO not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::ppGoodSim(PatternProcessor *pcoll)
{
	for (int i = 0; i < (int)pcoll->patternVector_.size(); i += WORD_SIZE)
	{
		ppSetPattern(pcoll, i);
		goodSim();
	}
}

// **************************************************************************
// Function   [ void Simulator::ppFaultSim(PatternSet *, FaultListExtract *) ]
// Author     [ littleshamoo ]
// Synopsis   [ usage: perform parallel pattern simulation on all faults.
//				in:    a set of pattern, FaultListExtract
//				out:   void
//			  ]
// Date       [ ]
// **************************************************************************
void Simulator::ppFaultSim(PatternProcessor *pcoll, FaultListExtract *fListExtract)
{
	// undetected faults are remaining faults
	FaultPtrList remain;
	FaultPtrListIter it = fListExtract->faultsInCircuit_.begin();
	for (; it != fListExtract->faultsInCircuit_.end(); ++it)
	{
		if ((*it)->faultState_ != Fault::DT && (*it)->faultState_ != Fault::RE && (*it)->faultyLine_ >= 0)
		{
			remain.push_back(*it);
		}
	}

	// simulate all patterns for all faults
	for (int i = 0; i < (int)pcoll->patternVector_.size(); i += WORD_SIZE)
	{
		ppSetPattern(pcoll, i);
		ppFaultSim(remain);
	}
}

// **************************************************************************
// Function   [ Simulator::ppFaultSim ]
// Commentor  [ Bill ]
// Synopsis   [ usage: simulate all undetected faults
//              in:    a set of undetected fault
//              out:   void //TODO not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::ppFaultSim(FaultPtrList &remain)
{
	if (remain.size() == 0)
	{
		return;
	}

	// run good simulation first
	goodSimCopyToFault();

	FaultPtrListIter it = remain.begin();
	do
	{
		if (ppCheckActivation((*it)))
		{
			ppInject((*it));
			eventFaultSim();
			ppCheckDetection((*it));
			ppReset();
		}
		if ((*it)->faultState_ == Fault::DT)
		{
			it = remain.erase(it);
		}
		else
		{
			++it;
		}
	} while (it != remain.end());
}

// **************************************************************************
// Function   [ Simulator::pfReset ]
// Commentor  [ CJY CBH]
// Synopsis   [ usage: reset faulty value of the fault gate to good value
//                     reset mask to zero
//              in:    void
//              out:   void
//            ]
// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
// **************************************************************************
inline void Simulator::pfReset()
{
	for (int i = 0; i < nrecover_; ++i)
	{
		cir_->circuitGates_[recover_[i]].faultSimLow_ = cir_->circuitGates_[recover_[i]].goodSimLow_;
		cir_->circuitGates_[recover_[i]].faultSimHigh_ = cir_->circuitGates_[recover_[i]].goodSimHigh_;
	}
	nrecover_ = 0;
	// memset(processed_, 0, cir_->totalGate_ * sizeof(bool));
	// memset(faultInjectL_, 0, cir_->totalGate_ * 5 * sizeof(ParaValue));
	// memset(faultInjectH_, 0, cir_->totalGate_ * 5 * sizeof(ParaValue));
	std::fill(processed_.begin(), processed_.end(), false);
	std::fill(faultInjectL_.begin(), faultInjectL_.end(), std::array<ParaValue, 5>({0, 0, 0, 0, 0}));
	std::fill(faultInjectH_.begin(), faultInjectH_.end(), std::array<ParaValue, 5>({0, 0, 0, 0, 0}));

	ninjected_ = 0;
}

// **************************************************************************
// Function   [ Simulator::pfCheckActivation ]
// Commentor  [ CJY CBH ]
// Synopsis   [ usage: check whether the fault can be activate to the fanout of the gate
//                     ie, compare good low & high with the fault type
//              in:    address of fault to be tested
//              out:   bool //TODO what does this boolean mean
//            ]
// Date       [ CBH Ver. 1.0 started 2013/08/18 ]
// **************************************************************************
bool Simulator::pfCheckActivation(const Fault *const f)
{
	const int &fg = f->faultyLine_ == 0 ? f->gateID_ : cir_->circuitGates_[f->gateID_].faninVector_[f->faultyLine_ - 1];
	const ParaValue &gl = cir_->circuitGates_[fg].goodSimLow_;
	const ParaValue &gh = cir_->circuitGates_[fg].goodSimHigh_;

	switch (f->faultType_)
	{
		case Fault::SA0:
			return gh != PARA_L;
		case Fault::SA1:
			return gl != PARA_L;
		case Fault::STR:
			if (cir_->numFrame_ < 2)
			{
				return false;
			}
			return (gl & cir_->circuitGates_[fg + cir_->numGate_].goodSimHigh_) != PARA_L;
		case Fault::STF:
			if (cir_->numFrame_ < 2)
			{
				return false;
			}
			return (gh & cir_->circuitGates_[fg + cir_->numGate_].goodSimLow_) != PARA_L;
		default:
			break;
	}
	return false;
}

// **************************************************************************
// Function   [ simulator::pfInject ]
// Commentor  [ CJY CBH]
// Synopsis   [ usage: push faulty gate to event list
//              in:    fault, index of injected fault
//              out:   void //TODO not void
//            ]
// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
// **************************************************************************
void Simulator::pfInject(const Fault *const f, const size_t &i)
{
	int fg = f->gateID_;
	switch (f->faultType_)
	{
		case Fault::SA0:
			setBitValue(faultInjectL_[fg][f->faultyLine_], i, H);
			break;
		case Fault::SA1:
			setBitValue(faultInjectH_[fg][f->faultyLine_], i, H);
			break;
		case Fault::STR:
			fg += cir_->numGate_;
			setBitValue(faultInjectL_[fg][f->faultyLine_], i, H);
			break;
		case Fault::STF:
			fg += cir_->numGate_;
			setBitValue(faultInjectH_[fg][f->faultyLine_], i, H);
			break;
		default:
			break;
	}

	// put gate into event list
	if (!processed_[fg])
	{
		events_[cir_->circuitGates_[fg].numLevel_].push(fg);
		processed_[fg] = true;
	}
}

// **************************************************************************
// Function   [ simulator::pfCheckDetection ]
// Commentor  [ CJY CBH]
// Synopsis   [ usage: compare the good sim and fault sim result
//					   and check whether the injected fault can be detected by the  pattern
//              in:    the list of undetected fault
//              out:   void //TODO not void
//            ]
// Date       [ CBH Ver. 1.0 started 2031/08/18 ]
// **************************************************************************
void Simulator::pfCheckDetection(FaultPtrList &remain)
{
	ParaValue detected = PARA_L;
	int start = cir_->totalGate_ - cir_->numPO_ - cir_->numPPI_;
	for (int i = start; i < cir_->totalGate_; ++i)
	{
		detected |= ((cir_->circuitGates_[i].goodSimLow_ & cir_->circuitGates_[i].faultSimHigh_) | (cir_->circuitGates_[i].goodSimHigh_ & cir_->circuitGates_[i].faultSimLow_));
	}

	// fault drop
	for (int i = 0; i < ninjected_; ++i)
	{
		if (getBitValue(detected, (size_t)i) == L)
		{
			continue;
		}
		++((*injected_[i])->detection_);
		if ((*injected_[i])->detection_ >= ndet_)
		{
			(*injected_[i])->faultState_ = Fault::DT;
			remain.erase(injected_[i]);
		}
	}
}

// **************************************************************************
// Function   [ Simulator::ppReset ]
// Commentor  [ Bill ]
// Synopsis   [ usage: reset all circuit gates ,faulty low & faulty high to good low & goood high
//              in:    none
//              out:   void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
inline void Simulator::ppReset()
{
	for (int i = 0; i < nrecover_; ++i)
	{
		cir_->circuitGates_[recover_[i]].faultSimLow_ = cir_->circuitGates_[recover_[i]].goodSimLow_;
		cir_->circuitGates_[recover_[i]].faultSimHigh_ = cir_->circuitGates_[recover_[i]].goodSimHigh_;
	}
	nrecover_ = 0;
	// memset(processed_, 0, cir_->totalGate_ * sizeof(bool));
	// memset(faultInjectL_, 0, cir_->totalGate_ * 5 * sizeof(ParaValue));
	// memset(faultInjectH_, 0, cir_->totalGate_ * 5 * sizeof(ParaValue));
	std::fill(processed_.begin(), processed_.end(), false);
	std::fill(faultInjectL_.begin(), faultInjectL_.end(), std::array<ParaValue, 5>({0, 0, 0, 0, 0}));
	std::fill(faultInjectH_.begin(), faultInjectH_.end(), std::array<ParaValue, 5>({0, 0, 0, 0, 0}));
	activated_ = PARA_L;
}

// **************************************************************************
// Function   [ ppCheckActivation ]
// Commentor  [ Bill ]
// Synopsis   [ usage: check whether the fault can be activate to the fanout of the gate
//              in:    fault to be tested
//              out:   bool // what does this boolean mean
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
bool Simulator::ppCheckActivation(const Fault *const f)
{
	const int &fg = f->faultyLine_ == 0 ? f->gateID_ : /// if output fault,fg=ID of the faulty gate,else if input fault,fg=ID of the faulty gate's fanin array
											cir_->circuitGates_[f->gateID_].faninVector_[f->faultyLine_ - 1];
	const ParaValue &gl = cir_->circuitGates_[fg].goodSimLow_;
	const ParaValue &gh = cir_->circuitGates_[fg].goodSimHigh_;

	switch (f->faultType_)
	{
		case Fault::SA0:
			activated_ = gh;
			return activated_ != PARA_L;
		case Fault::SA1:
			activated_ = gl;
			return activated_ != PARA_L;
		case Fault::STR:
			if (cir_->numFrame_ < 2)
			{
				return false;
			}
			activated_ = (gl & cir_->circuitGates_[fg + cir_->numGate_].goodSimHigh_);
			return activated_ != PARA_L;
		case Fault::STF:
			if (cir_->numFrame_ < 2)
			{
				return false;
			}
			activated_ = (gh & cir_->circuitGates_[fg + cir_->numGate_].goodSimLow_);
			return activated_ != PARA_L;
		default:
			break;
	}
	return false;
}

// **************************************************************************
// Function   [ ppInject ]
// Commentor  [ Bill ]
// Synopsis   [ usage: inject fault
//              in:    a fault to be injected
//              out:   void //TODO not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
//{{{ void Simulator::ppInject(const Fault * const)
void Simulator::ppInject(const Fault *const f)
{
	int fg = f->gateID_;
	switch (f->faultType_)
	{
		case Fault::SA0:
			faultInjectL_[fg][f->faultyLine_] = PARA_H;
			break;
		case Fault::SA1:
			faultInjectH_[fg][f->faultyLine_] = PARA_H;
			break;
		case Fault::STR:
			fg += cir_->numGate_;
			faultInjectL_[fg][f->faultyLine_] = PARA_H;
			break;
		case Fault::STF:
			fg += cir_->numGate_;
			faultInjectH_[fg][f->faultyLine_] = PARA_H;
			break;
		default:
			break;
	}

	// put gate into event list
	if (!processed_[fg])
	{
		events_[cir_->circuitGates_[fg].numLevel_].push(fg);
		processed_[fg] = true;
	}
}

// **************************************************************************
// Function   [ ppCheckDetection ]
// Commentor  [ Bill ]
// Synopsis   [ usage: compare the good sim and fault sim result
//					   and check whether the injected fault can be detected by the  pattern
//              in:    a fault which is injected into the circuit
//              out:   void //not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::ppCheckDetection(Fault *const f)
{
	ParaValue detected = PARA_L;
	int start = cir_->totalGate_ - cir_->numPO_ - cir_->numPPI_;
	for (int i = start; i < cir_->totalGate_; ++i)
	{
		// TO-DO homework 02
		detected |= ((cir_->circuitGates_[i].goodSimLow_ & cir_->circuitGates_[i].faultSimHigh_) | (cir_->circuitGates_[i].goodSimHigh_ & cir_->circuitGates_[i].faultSimLow_));
		// end of TO-DO
	}
	detected &= activated_;

	// fault drop
	for (int i = 0; i < WORD_SIZE; ++i)
	{
		if (getBitValue(detected, i) == L)
		{
			continue;
		}
		++(f->detection_);
		if (f->detection_ >= ndet_)
		{
			f->faultState_ = Fault::DT;
			break;
		}
	}
}

// **************************************************************************
// Function   [ ppSetPattern ]
// Commentor  [ Bill ]
// Synopsis   [ usage: apply pattern
//              in:    the total pattern set,
//					   and an integer to represent the starting pattern of current fsim
//              out:   void // not void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Simulator::ppSetPattern(PatternProcessor *pPatternProcessor, const int &i)
{ // TODO LOS not yet supported
	// reset PI and PPI values to unknowns
	for (int j = 0; j < cir_->numPI_ + cir_->numPPI_; ++j)
	{
		for (int k = 0; k < cir_->numFrame_; ++k)
		{
			cir_->circuitGates_[j + k * cir_->numGate_].goodSimLow_ = PARA_L;
			cir_->circuitGates_[j + k * cir_->numGate_].goodSimHigh_ = PARA_L;
		}
	}
	// assign up to WORD_SIZE number of pattern values
	int endpat = (int)pPatternProcessor->patternVector_.size();
	if (i + (int)WORD_SIZE <= (int)pPatternProcessor->patternVector_.size())
	{
		endpat = i + WORD_SIZE;
	}
	for (int j = i; j < endpat; ++j)
	{
		// assign PI
		if (!pPatternProcessor->patternVector_[j].primaryInputs1st_.empty())
		{
			for (int k = 0; k < pPatternProcessor->numPI_; ++k)
			{
				if (pPatternProcessor->patternVector_[j].primaryInputs1st_[k] == L)
				{
					setBitValue(cir_->circuitGates_[k].goodSimLow_, j - i, H);
				}
				else if (pPatternProcessor->patternVector_[j].primaryInputs1st_[k] == H)
				{
					setBitValue(cir_->circuitGates_[k].goodSimHigh_, j - i, H);
				}
			}
		}

		if (!pPatternProcessor->patternVector_[j].primaryInputs2nd_.empty() && cir_->numFrame_ > 1)
		{
			for (int k = 0; k < pPatternProcessor->numPI_; ++k)
			{
				int index = k + cir_->numGate_;
				if (pPatternProcessor->patternVector_[j].primaryInputs2nd_[k] == L)
				{
					setBitValue(cir_->circuitGates_[index].goodSimLow_, j - i, H);
				}
				else if (pPatternProcessor->patternVector_[j].primaryInputs2nd_[k] == H)
				{
					setBitValue(cir_->circuitGates_[index].goodSimHigh_, j - i, H);
				}
			}
		}
		// assign PPI
		if (!pPatternProcessor->patternVector_[j].pseudoPrimaryInputs_.empty())
		{
			for (int k = 0; k < pPatternProcessor->numPPI_; ++k)
			{
				int index = k + cir_->numPI_;
				if (pPatternProcessor->patternVector_[j].pseudoPrimaryInputs_[k] == L)
				{
					setBitValue(cir_->circuitGates_[index].goodSimLow_, j - i, H);
				}
				else if (pPatternProcessor->patternVector_[j].pseudoPrimaryInputs_[k] == H)
				{
					setBitValue(cir_->circuitGates_[index].goodSimHigh_, j - i, H);
				}
			}
		}

		// if (pcoll->patternVector_[j].shiftIn_ && cir_->numFrame_ > 1 && cir_->timeFrameConnectType_ == Circuit::SHIFT)
		if (!pPatternProcessor->patternVector_[j].shiftIn_.empty() && cir_->numFrame_ > 1 && cir_->timeFrameConnectType_ == Circuit::SHIFT)
		{
			int index = cir_->numGate_ + cir_->numPI_;
			if (pPatternProcessor->patternVector_[j].shiftIn_[0] == L)
			{
				setBitValue(cir_->circuitGates_[index].goodSimLow_, j - i, H);
			}
			else if (pPatternProcessor->patternVector_[j].shiftIn_[0] == H)
			{
				setBitValue(cir_->circuitGates_[index].goodSimHigh_, j - i, H);
			}
		}
	}
}
