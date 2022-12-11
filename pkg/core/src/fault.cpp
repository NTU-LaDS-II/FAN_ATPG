// **************************************************************************
// File       [ fault.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ this program extract faults from the circuit
//              currently there is no fault collapsing yet. 2014/8/26
//              implement fault collapsing and refactor. 2022/12/1 ]
// Date       [ 2011/10/05 created ]
// **************************************************************************

#include "fault.h"

#include <cstring>

// using namespace IntfNs; removed by pan
using namespace CoreNs;

void FaultListExtract::extractFaultFromCircuit(Circuit *circuit)
{
	// since the function only called once, we don't need to clear faults initially
	// reserve enough space for faults push_back, 10 * circuit->ngate_ is maximum possible faults in a circuit
	uncollapsedFaults_.reserve(10 * circuit->ngate_);
	extractedFaults_.reserve(10 * circuit->ngate_);

	// resize gateIndexToFaultIndex to proper size
	gateIndexToFaultIndex_.resize(circuit->ngate_);

	// add stuck-at faults
	if (faultListType_ == SAF)
	{
		// extract uncollapsed faults
		// doesn't extract faults between two time frames
		for (int i = 0; i < circuit->ngate_; ++i)
		{
			gateIndexToFaultIndex_[i] = uncollapsedFaults_.size();
			// extract faults of gate outputs
			if (circuit->gates_[i].nfo_ > 0 && i < circuit->ngate_ - circuit->nppi_)
			{
				uncollapsedFaults_.push_back(Fault(i, Fault::SA0, 0));
				uncollapsedFaults_.push_back(Fault(i, Fault::SA1, 0));
			}
			// extract faults of gate inputs
			for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
			{
				if (circuit->gates_[circuit->gates_[i].fis_[j]].nfo_ > 1) // fanout stem
				{
					uncollapsedFaults_.push_back(Fault(i, Fault::SA0, j + 1));
					uncollapsedFaults_.push_back(Fault(i, Fault::SA1, j + 1));
				}
			}
			// add additional faults for PPI
			if (circuit->gates_[i].type_ == Gate::PPI)
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
		bool useFC = true;
		// Without Fault Collapsing
		if (!useFC)
		{
			// Copy uncollapsedFaults_ to extractedFaults_
			extractedFaults_.resize(uncollapsedFaults_.size());
			extractedFaults_.assign(uncollapsedFaults_.begin(), uncollapsedFaults_.end());
		}
		else
		// Simple Equivalent Fault Collapsing
		{
			std::vector<int> SA0Equivalent(circuit->ngate_), SA1Equivalent(circuit->ngate_); // used to count the number of equivalent faults
			int SA0EquivalentOfInput, SA1EquivalentOfInput;				// SA0Equivalent, SA1Equivalent of the input gates
			for (int i = 0; i < circuit->ngate_; ++i)
			{
				// initialize SA0Equivalent, SA1Equivalent
				SA0Equivalent[i] = 1;
				SA1Equivalent[i] = 1;
				// adding input faults
				switch (circuit->gates_[i].type_)
				{
					// AND gates
					case Gate::AND2:
					case Gate::AND3:
					case Gate::AND4:
						for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[j]];
							SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA1, j + 1, SA1EquivalentOfInput));
							SA0Equivalent[i] += SA0EquivalentOfInput;
						}
						break;
					// NAND gates
					case Gate::NAND2:
					case Gate::NAND3:
					case Gate::NAND4:
						for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[j]];
							SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA1, j + 1, SA1EquivalentOfInput));
							SA1Equivalent[i] += SA0EquivalentOfInput;
						}
						break;
					// OR gates
					case Gate::OR2:
					case Gate::OR3:
					case Gate::OR4:
						for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[j]];
							SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA0, j + 1, SA0EquivalentOfInput));
							SA1Equivalent[i] += SA1EquivalentOfInput;
						}
						break;
					// NOR gates
					case Gate::NOR2:
					case Gate::NOR3:
					case Gate::NOR4:
						for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[j]];
							SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA0, j + 1, SA0EquivalentOfInput));
							SA0Equivalent[i] += SA1EquivalentOfInput;
						}
						break;
					// INV and BUF gates
					// We don't need to add fault at these two types
					// But we need calculate the number of equivalent faults
					case Gate::INV:
						SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[0]];
						SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[0]];
						SA0Equivalent[i] = SA1EquivalentOfInput + 1;
						SA1Equivalent[i] = SA0EquivalentOfInput + 1;
						break;
					case Gate::BUF:
						SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[0]];
						SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[0]];
						SA0Equivalent[i] = SA0EquivalentOfInput + 1;
						SA1Equivalent[i] = SA1EquivalentOfInput + 1;
						break;
					// Other gates, including PO and PPO gates
					default:
						for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
						{
							SA0EquivalentOfInput = SA0Equivalent[circuit->gates_[i].fis_[j]];
							SA1EquivalentOfInput = SA1Equivalent[circuit->gates_[i].fis_[j]];
							extractedFaults_.push_back(Fault(i, Fault::SA0, j + 1, SA0EquivalentOfInput));
							extractedFaults_.push_back(Fault(i, Fault::SA1, j + 1, SA1EquivalentOfInput));
						}
						break;
				}
				// add output faults
				// Only for fanout stem, including PI,PPI with fanout stem
				if (circuit->gates_[i].nfo_ > 1)
				{
					// add faults with calculated SA0Equivalent, SA1Equivalent and reset them to 1
					extractedFaults_.push_back(Fault(i, Fault::SA0, 0, SA0Equivalent[i]));
					extractedFaults_.push_back(Fault(i, Fault::SA1, 0, SA1Equivalent[i]));
					SA0Equivalent[i] = 1;
					SA1Equivalent[i] = 1;
				}
				// add additional faults for PPI
				if (circuit->gates_[i].type_ == Gate::PPI)
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
		for (size_t i = 0; i < circuit->nl_->getTop()->getNPort(); ++i)
		{
			IntfNs::Port *p = circuit->nl_->getTop()->getPort(i);
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
		for (int i = 0; i < circuit->ngate_; ++i)
		{
			gateIndexToFaultIndex_[i] = uncollapsedFaults_.size();
			// extract faults of gate outputs
			// but do not extract faults between two time frames
			if (circuit->gates_[i].nfo_ > 0 && i < circuit->ngate_ - circuit->nppi_)
			{
				if (circuit->gates_[i].type_ != Gate::PPI)
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
			for (int j = 0; j < circuit->gates_[i].nfi_; ++j)
			{
				if (circuit->gates_[circuit->gates_[i].fis_[j]].nfo_ > 1) // fanout stem
				{
					uncollapsedFaults_.push_back(Fault(i, Fault::STR, j + 1));
					uncollapsedFaults_.push_back(Fault(i, Fault::STF, j + 1));
				}
			}
			// add faults for PPI
			if (circuit->gates_[i].type_ == Gate::PPI)
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
		for (size_t i = 0; i < circuit->nl_->getTop()->getNPort(); ++i)
		{
			IntfNs::Port *p = circuit->nl_->getTop()->getPort(i);
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
