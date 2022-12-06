// **************************************************************************
// File       [ fault.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ this program extract faults from the circuit
//              currently there is no fault collapsing yet. 2014/8/26
//              implement fault collapsing and refactor. 2022/12/1 ]
// Date       [ 2011/10/05 created ]
// **************************************************************************

#include "fault.h"

using namespace std;
using namespace IntfNs;
using namespace CoreNs;

void FaultListExtract::extract(Circuit *cir)
{
	// clear faults
	for (size_t i = 0; i < faults_.size(); ++i)
		delete faults_[i];
	faults_.clear();
	delete[] gateToFault_;

	// add stuck-at faults
	if (type_ == SAF)
	{
		gateToFault_ = new int[cir->ngate_];
		bool useFC = true;
		// Without Fault Collapsing
		if (!useFC)
		{
			// doesn't extract faults between two time frames
			for (int i = 0; i < cir->ngate_; ++i)
			{
				gateToFault_[i] = faults_.size();
				// extract faults of gate outputs
				if (cir->gates_[i].nfo_ > 0 && i < cir->ngate_ - cir->nppi_)
				{
					faults_.push_back(new Fault(i, Fault::SA0, 0));
					faults_.push_back(new Fault(i, Fault::SA1, 0));
				}
				// extract faults of gate inputs
				for (int j = 0; j < cir->gates_[i].nfi_; ++j)
				{
					if (cir->gates_[cir->gates_[i].fis_[j]].nfo_ > 1) // fanout stem
					{
						faults_.push_back(new Fault(i, Fault::SA0, j + 1));
						faults_.push_back(new Fault(i, Fault::SA1, j + 1));
					}
				}
			}
		}
		else
		// Simple Equivalent Fault Collapsing
		{
			vector<int> SA0_eq, SA1_eq; // used to count the number of equivalent faults
			int SA0_eq_in, SA1_eq_in;		// SA0_eq, SA1_eq of the input gates
			for (int i = 0; i < cir->ngate_; ++i)
			{
				gateToFault_[i] = faults_.size();
				// initialize SA0_eq, SA1_eq
				SA0_eq.push_back(1);
				SA1_eq.push_back(1);
				// adding input faults
				switch (cir->gates_[i].type_)
				{
					// AND gates
					case Gate::AND2:
					case Gate::AND3:
					case Gate::AND4:
						for (int j = 0; j < cir->gates_[i].nfi_; ++j)
						{
							SA0_eq_in = SA0_eq[cir->gates_[i].fis_[j]];
							SA1_eq_in = SA1_eq[cir->gates_[i].fis_[j]];
							faults_.push_back(new Fault(i, Fault::SA1, j + 1, SA1_eq_in));
							SA0_eq[i] += SA0_eq_in;
						}
						break;
					// NAND gates
					case Gate::NAND2:
					case Gate::NAND3:
					case Gate::NAND4:
						for (int j = 0; j < cir->gates_[i].nfi_; ++j)
						{
							SA0_eq_in = SA0_eq[cir->gates_[i].fis_[j]];
							SA1_eq_in = SA1_eq[cir->gates_[i].fis_[j]];
							faults_.push_back(new Fault(i, Fault::SA1, j + 1, SA1_eq_in));
							SA1_eq[i] += SA0_eq_in;
						}
						break;
					// OR gates
					case Gate::OR2:
					case Gate::OR3:
					case Gate::OR4:
						for (int j = 0; j < cir->gates_[i].nfi_; ++j)
						{
							SA0_eq_in = SA0_eq[cir->gates_[i].fis_[j]];
							SA1_eq_in = SA1_eq[cir->gates_[i].fis_[j]];
							faults_.push_back(new Fault(i, Fault::SA0, j + 1, SA0_eq_in));
							SA1_eq[i] += SA1_eq_in;
						}
						break;
					// NOR gates
					case Gate::NOR2:
					case Gate::NOR3:
					case Gate::NOR4:
						for (int j = 0; j < cir->gates_[i].nfi_; ++j)
						{
							SA0_eq_in = SA0_eq[cir->gates_[i].fis_[j]];
							SA1_eq_in = SA1_eq[cir->gates_[i].fis_[j]];
							faults_.push_back(new Fault(i, Fault::SA0, j + 1, SA0_eq_in));
							SA0_eq[i] += SA1_eq_in;
						}
						break;
					// INV and BUF gates
					// We don't need to add fault at these two types
					// But we need calculate the number of equivalent faults
					case Gate::INV:
						SA0_eq_in = SA0_eq[cir->gates_[i].fis_[0]];
						SA1_eq_in = SA1_eq[cir->gates_[i].fis_[0]];
						SA0_eq[i] = SA1_eq_in + 1;
						SA1_eq[i] = SA0_eq_in + 1;
						break;
					case Gate::BUF:
						SA0_eq_in = SA0_eq[cir->gates_[i].fis_[0]];
						SA1_eq_in = SA1_eq[cir->gates_[i].fis_[0]];
						SA0_eq[i] = SA0_eq_in + 1;
						SA1_eq[i] = SA1_eq_in + 1;
						break;
					// Other gates, including PO and PPO gates
					default:
						for (int j = 0; j < cir->gates_[i].nfi_; ++j)
						{
							SA0_eq_in = SA0_eq[cir->gates_[i].fis_[j]];
							SA1_eq_in = SA1_eq[cir->gates_[i].fis_[j]];
							faults_.push_back(new Fault(i, Fault::SA0, j + 1, SA0_eq_in));
							faults_.push_back(new Fault(i, Fault::SA1, j + 1, SA1_eq_in));
						}
						break;
				}
				// add output faults
				// Only for fanout stem, including PI,PPI with fanout stem
				if (cir->gates_[i].nfo_ > 1)
				{
					// add faults with calculated SA0_eq, SA1_eq and reset them to 1
					faults_.push_back(new Fault(i, Fault::SA0, 0, SA0_eq[i]));
					faults_.push_back(new Fault(i, Fault::SA1, 0, SA1_eq[i]));
					SA0_eq[i] = 1;
					SA1_eq[i] = 1;
				}
			}
		}

		// add faults for PPI
		for (int i = cir->npi_; i < cir->npi_ + cir->nppi_; i++)
		{
			// CK
			faults_.push_back(new Fault(i, Fault::SA0, -1, 1, Fault::DT));
			faults_.push_back(new Fault(i, Fault::SA1, -1, 1, Fault::DT));
			// SE
			faults_.push_back(new Fault(i, Fault::SA0, -2, 1, Fault::DT));
			faults_.push_back(new Fault(i, Fault::SA1, -2, 1, Fault::DT));
			// SI
			faults_.push_back(new Fault(i, Fault::SA0, -3, 1, Fault::DT));
			faults_.push_back(new Fault(i, Fault::SA1, -3, 1, Fault::DT));
			// QN
			faults_.push_back(new Fault(i, Fault::SA0, -4, 1, Fault::UD));
			faults_.push_back(new Fault(i, Fault::SA1, -4, 1, Fault::UD));
		}

		// HYH try to fix the fault number @20141121
		for (size_t i = 0; i < cir->nl_->getTop()->getNPort(); ++i)
		{
			Port *p = cir->nl_->getTop()->getPort(i);
			if (!strcmp(p->name_, "CK")) // sequential circuit
			{
				// CK
				faults_.push_back(new Fault(-1, Fault::SA0, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-1, Fault::SA1, 0, 1, Fault::DT));
				// test_si
				faults_.push_back(new Fault(-2, Fault::SA0, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-2, Fault::SA1, 0, 1, Fault::DT));
				// test_so
				faults_.push_back(new Fault(-3, Fault::SA0, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-3, Fault::SA1, 0, 1, Fault::DT));
				// test_se
				faults_.push_back(new Fault(-4, Fault::SA0, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-4, Fault::SA1, 0, 1, Fault::DT));
			}
		}
	}
	// end of adding stuck-at faults

	// add transition faults
	else
	{
		gateToFault_ = new int[cir->ngate_];
		for (int i = 0; i < cir->ngate_; ++i)
		{
			gateToFault_[i] = faults_.size();
			// extract faults of gate outputs
			// but do not extract faults between two time frames
			if (cir->gates_[i].nfo_ > 0 && i < cir->ngate_ - cir->nppi_)
			{
				if (cir->gates_[i].type_ != Gate::PPI)
				{
					faults_.push_back(new Fault(i, Fault::STR, 0));
					faults_.push_back(new Fault(i, Fault::STF, 0));
				}
				else
				{
					faults_.push_back(new Fault(i, Fault::STR, 0, 1, Fault::DT));
					faults_.push_back(new Fault(i, Fault::STF, 0, 1, Fault::DT));
				}
			}
			// extract faults of gate inputs
			for (int j = 0; j < cir->gates_[i].nfi_; ++j)
			{
				if (cir->gates_[cir->gates_[i].fis_[j]].nfo_ > 1) // fanout stem
				{
					faults_.push_back(new Fault(i, Fault::STR, j + 1));
					faults_.push_back(new Fault(i, Fault::STF, j + 1));
				}
			}
			// add faults for PPI
			if (cir->gates_[i].type_ == Gate::PPI)
			{
				// CK
				faults_.push_back(new Fault(i, Fault::STR, -1, 1, Fault::DT));
				faults_.push_back(new Fault(i, Fault::STF, -1, 1, Fault::DT));
				// SE
				faults_.push_back(new Fault(i, Fault::STR, -2, 1, Fault::DT));
				faults_.push_back(new Fault(i, Fault::STF, -2, 1, Fault::DT));
				// SI
				faults_.push_back(new Fault(i, Fault::STR, -3, 1, Fault::DT));
				faults_.push_back(new Fault(i, Fault::STF, -3, 1, Fault::DT));
				// QN
				faults_.push_back(new Fault(i, Fault::STR, -4, 1, Fault::UD));
				faults_.push_back(new Fault(i, Fault::STF, -4, 1, Fault::UD));
			}
		}

		// HYH try to fix the fault number @20141121
		for (size_t i = 0; i < cir->nl_->getTop()->getNPort(); ++i)
		{
			Port *p = cir->nl_->getTop()->getPort(i);
			if (!strcmp(p->name_, "CK")) // sequential circuit
			{
				// CK
				faults_.push_back(new Fault(-1, Fault::STR, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-1, Fault::STF, 0, 1, Fault::DT));
				// test_si
				faults_.push_back(new Fault(-2, Fault::STR, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-2, Fault::STF, 0, 1, Fault::DT));
				// test_so
				faults_.push_back(new Fault(-3, Fault::STR, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-3, Fault::STF, 0, 1, Fault::DT));
				// test_se
				faults_.push_back(new Fault(-4, Fault::STR, 0, 1, Fault::DT));
				faults_.push_back(new Fault(-4, Fault::STF, 0, 1, Fault::DT));
			}
		}
	}
	// end of adding transition faults
}
