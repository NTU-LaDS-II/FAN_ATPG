// **************************************************************************
// File       [ fault.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ this program extract faults from the circuit
//              currently there is no fault collapsing yet. 2014/8/26
//              implement fault collapsing and refactor. 2022/12/1 ]
// Date       [ 2011/10/05 created ]
// **************************************************************************

#include "fault.h"

// using namespace IntfNs; removed by pan
using namespace CoreNs;

// **************************************************************************
// Function   [ FaultListExtract::extractFaultFromCircuit ]
// Commenter  [ Pan ]
// Synopsis   [ usage: extract faults from the circuit
//              in:  Circuit *pCircuit
//              out: void
//              P.S. : In the fault model, even if the fanout has only one
//              branch, we still consider the output faults and input faults
//              to be different faults. In theory, these two faults should
//              be collapsed to the same faults. But currently there are some
//              bug with MTO, thus these two faults are separated now.
//            ]
// Date       [ Ver. 2.0 last modified 2022/12/29 ]
// **************************************************************************
void FaultListExtract::extractFaultFromCircuit(Circuit *pCircuit)
{
	bool useFC = true; // should be able to set on or off like test compression

	// since the function only called once, we don't need to clear faults initially
	// reserve enough space for faults push_back, 10 * circuit->numGate_ is maximum possible faults in a circuit
	int reservedSize = 10 * pCircuit->numGate_;
	uncollapsedFaults_.reserve(reservedSize);
	extractedFaults_.reserve(reservedSize);

	// resize gateIndexToFaultIndex to proper size
	gateIndexToFaultIndex_.resize(pCircuit->numGate_);

	// add stuck-at faults
	if (faultListType_ == SAF)
	{
		// extract uncollapsed faults
		// doesn't extract faults between two time frames
		for (int i = 0; i < pCircuit->numGate_; ++i)
		{
			gateIndexToFaultIndex_[i] = uncollapsedFaults_.size();
			// extract faults of gate outputs
			if (pCircuit->circuitGates_[i].numFO_ > 0 && i < pCircuit->numGate_ - pCircuit->numPPI_)
			{
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, 0));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, 0));
			}
			// extract faults of gate inputs
			for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
			{
				// // if (circuit->circuitGates_[circuit->circuitGates_[i].faninVector_[j]].numFO_ > 1) // fanout stem
				// // {
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, j + 1));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, j + 1));
				// // }
			}
			// add additional faults for PPI
			if (pCircuit->circuitGates_[i].gateType_ == Gate::PPI)
			{
				// CK
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, -1, 1, Fault::DT));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, -1, 1, Fault::DT));
				// SE
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, -2, 1, Fault::DT));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, -2, 1, Fault::DT));
				// SI
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, -3, 1, Fault::DT));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, -3, 1, Fault::DT));
				// QN
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, -4, 1, Fault::UD));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, -4, 1, Fault::UD));
			}
		}

		// extract faults
		// Without Fault Collapsing
		if (!useFC)
		{
			extractedFaults_.resize(uncollapsedFaults_.size());
			extractedFaults_.assign(uncollapsedFaults_.begin(), uncollapsedFaults_.end());
		}
		else // Simple Equivalent Fault Collapsing
		{
			std::vector<int> SA0Equivalent(pCircuit->numGate_, 1), SA1Equivalent(pCircuit->numGate_, 1); // used to count the number of equivalent faults
			int SA0EquivalentOfInput, SA1EquivalentOfInput;																							 // SA0Equivalent, SA1Equivalent of the input gates
			for (int i = 0; i < pCircuit->numGate_; ++i)
			{
				// initialize SA0Equivalent, SA1Equivalent
				// SA0Equivalent[i] = 1;
				// SA1Equivalent[i] = 1;
				// adding input faults
				switch (pCircuit->circuitGates_[i].gateType_)
				{
					// AND gates
					case Gate::AND2:
					case Gate::AND3:
					case Gate::AND4:
						for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA1, j + 1, SA1EquivalentOfInput));
							SA0Equivalent[i] += SA0EquivalentOfInput;
						}
						break;
					// NAND gates
					case Gate::NAND2:
					case Gate::NAND3:
					case Gate::NAND4:
						for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA1, j + 1, SA1EquivalentOfInput));
							SA1Equivalent[i] += SA0EquivalentOfInput;
						}
						break;
					// OR gates
					case Gate::OR2:
					case Gate::OR3:
					case Gate::OR4:
						for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA0, j + 1, SA0EquivalentOfInput));
							SA1Equivalent[i] += SA1EquivalentOfInput;
						}
						break;
					// NOR gates
					case Gate::NOR2:
					case Gate::NOR3:
					case Gate::NOR4:
						for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA0, j + 1, SA0EquivalentOfInput));
							SA0Equivalent[i] += SA1EquivalentOfInput;
						}
						break;
					// INV and BUF gates
					// We don't need to add fault at these two types
					// But we need calculate the number of equivalent faults
					case Gate::INV:
						SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[0]];
						SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[0]];
						SA0Equivalent[i] = SA1EquivalentOfInput + 1;
						SA1Equivalent[i] = SA0EquivalentOfInput + 1;
						break;
					case Gate::BUF:
						SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[0]];
						SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[0]];
						SA0Equivalent[i] = SA0EquivalentOfInput + 1;
						SA1Equivalent[i] = SA1EquivalentOfInput + 1;
						break;
					// Other gates, including PO and PPO gates
					default:
						for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							SA1EquivalentOfInput = SA1Equivalent[pCircuit->circuitGates_[i].faninVector_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA0, j + 1, SA0EquivalentOfInput));
							extractedFaults_.push_back(Fault(i, Fault::SA1, j + 1, SA1EquivalentOfInput));
						}
						break;
				}
				// add output faults
				// add faults with calculated SA0Equivalent, SA1Equivalent and reset them to 1
				if (pCircuit->circuitGates_[i].numFO_ > 0 && i < pCircuit->numGate_ - pCircuit->numPPI_)
				{
					extractedFaults_.push_back(Fault(i, Fault::SA0, 0, SA0Equivalent[i]));
					extractedFaults_.push_back(Fault(i, Fault::SA1, 0, SA1Equivalent[i]));
				}
				SA0Equivalent[i] = 1;
				SA1Equivalent[i] = 1;
				// Only for fanout stem, including PI,PPI with fanout stem
				// if (pCircuit->circuitGates_[i].numFO_ > 1 && i < (pCircuit->numGate_ - pCircuit->numPPI_))
				// {
				// 	// add faults with calculated SA0_eq, SA1_eq and reset them to 1
				// 	extractedFaults_.push_back(Fault(i, Fault::SA0, 0, SA0Equivalent[i]));
				// 	extractedFaults_.push_back(Fault(i, Fault::SA1, 0, SA1Equivalent[i]));
				// 	SA0Equivalent[i] = 1;
				// 	SA1Equivalent[i] = 1;
				// }
				// else if (pCircuit->circuitGates_[i].numFO_ == 1 && i < (pCircuit->numGate_ - pCircuit->numPPI_))
				// {
				// 	++SA0Equivalent[i];
				// 	++SA1Equivalent[i];
				// }

				// add additional faults for PPI
				if (pCircuit->circuitGates_[i].gateType_ == Gate::PPI)
				{
					// CK
					extractedFaults_.push_back(Fault(i, Fault::SA0, -1, 1, Fault::DT));
					extractedFaults_.push_back(Fault(i, Fault::SA1, -1, 1, Fault::DT));
					// SE
					extractedFaults_.push_back(Fault(i, Fault::SA0, -2, 1, Fault::DT));
					extractedFaults_.push_back(Fault(i, Fault::SA1, -2, 1, Fault::DT));
					// SI
					extractedFaults_.push_back(Fault(i, Fault::SA0, -3, 1, Fault::DT));
					extractedFaults_.push_back(Fault(i, Fault::SA1, -3, 1, Fault::DT));
					// QN
					extractedFaults_.push_back(Fault(i, Fault::SA0, -4, 1, Fault::UD));
					extractedFaults_.push_back(Fault(i, Fault::SA1, -4, 1, Fault::UD));
				}
			}
		}

		// HYH try to fix the fault number @20141121
		for (int i = 0; i < pCircuit->pNetlist_->getTop()->getNPort(); ++i)
		{
			IntfNs::Port *p = pCircuit->pNetlist_->getTop()->getPort(i);
			if (!strcmp(p->name_, "CK")) // sequential circuit
			{
				// CK
				extractedFaults_.push_back(Fault(-1, Fault::SA0, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-1, Fault::SA1, 0, 1, Fault::DT));
				// test_si
				extractedFaults_.push_back(Fault(-2, Fault::SA0, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-2, Fault::SA1, 0, 1, Fault::DT));
				// test_so
				extractedFaults_.push_back(Fault(-3, Fault::SA0, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-3, Fault::SA1, 0, 1, Fault::DT));
				// test_se
				extractedFaults_.push_back(Fault(-4, Fault::SA0, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-4, Fault::SA1, 0, 1, Fault::DT));
			}
		}
	}
	// end of adding stuck-at faults

	// add transition faults
	else
	{
		// extract uncollapsed faults
		for (int i = 0; i < pCircuit->numGate_; ++i)
		{
			gateIndexToFaultIndex_[i] = uncollapsedFaults_.size();
			// extract faults of gate outputs
			// but do not extract faults between two time frames
			if (pCircuit->circuitGates_[i].numFO_ > 0 && i < pCircuit->numGate_ - pCircuit->numPPI_)
			{
				if (pCircuit->circuitGates_[i].gateType_ != Gate::PPI)
				{
					uncollapsedFaults_.push_back(Fault(i, Fault::STR, 0));
					uncollapsedFaults_.push_back(Fault(i, Fault::STF, 0));
				}
				else
				{
					uncollapsedFaults_.push_back(Fault(i, Fault::STR, 0, 1, Fault::DT));
					uncollapsedFaults_.push_back(Fault(i, Fault::STF, 0, 1, Fault::DT));
				}
			}
			// extract faults of gate inputs
			for (int j = 0; j < pCircuit->circuitGates_[i].numFI_; ++j)
			{
				if (pCircuit->circuitGates_[pCircuit->circuitGates_[i].faninVector_[j]].numFO_ > 1) // fanout stem
				{
					uncollapsedFaults_.push_back(Fault(i, Fault::STR, j + 1));
					uncollapsedFaults_.push_back(Fault(i, Fault::STF, j + 1));
				}
			}
			// add faults for PPI
			if (pCircuit->circuitGates_[i].gateType_ == Gate::PPI)
			{
				// CK
				uncollapsedFaults_.push_back(Fault(i, Fault::STR, -1, 1, Fault::DT));
				uncollapsedFaults_.push_back(Fault(i, Fault::STF, -1, 1, Fault::DT));
				// SE
				uncollapsedFaults_.push_back(Fault(i, Fault::STR, -2, 1, Fault::DT));
				uncollapsedFaults_.push_back(Fault(i, Fault::STF, -2, 1, Fault::DT));
				// SI
				uncollapsedFaults_.push_back(Fault(i, Fault::STR, -3, 1, Fault::DT));
				uncollapsedFaults_.push_back(Fault(i, Fault::STF, -3, 1, Fault::DT));
				// QN
				uncollapsedFaults_.push_back(Fault(i, Fault::STR, -4, 1, Fault::UD));
				uncollapsedFaults_.push_back(Fault(i, Fault::STF, -4, 1, Fault::UD));
			}
		}

		// extract faults
		extractedFaults_.resize(uncollapsedFaults_.size());
		extractedFaults_.assign(uncollapsedFaults_.begin(), uncollapsedFaults_.end());

		// HYH try to fix the fault number @20141121
		for (int i = 0; i < pCircuit->pNetlist_->getTop()->getNPort(); ++i)
		{
			IntfNs::Port *p = pCircuit->pNetlist_->getTop()->getPort(i);
			if (!strcmp(p->name_, "CK")) // sequential circuit
			{
				// CK
				extractedFaults_.push_back(Fault(-1, Fault::STR, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-1, Fault::STF, 0, 1, Fault::DT));
				// test_si
				extractedFaults_.push_back(Fault(-2, Fault::STR, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-2, Fault::STF, 0, 1, Fault::DT));
				// test_so
				extractedFaults_.push_back(Fault(-3, Fault::STR, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-3, Fault::STF, 0, 1, Fault::DT));
				// test_se
				extractedFaults_.push_back(Fault(-4, Fault::STR, 0, 1, Fault::DT));
				extractedFaults_.push_back(Fault(-4, Fault::STF, 0, 1, Fault::DT));
			}
		}
	}
	// end of adding transition faults
}