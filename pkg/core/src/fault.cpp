// **************************************************************************
// File       [ fault.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ this program extract faults from the circuit
//              currently there is no fault collapsing yet. 2014/8/26 ]
// Date       [ 2011/10/05 created ]
// **************************************************************************

#include "fault.h"

using namespace std;
using namespace IntfNs;
using namespace CoreNs;

void FaultListExtract::extract(Circuit *cir)
{

	Fault *temp;
	// clear faults
	for (size_t i = 0; i < faults_.size(); ++i)
		delete faults_[i];
	faults_.clear();
	delete[] gateToFault_;

	// For stuck-at fault
	if (type_ == SAF)
	{
		gateToFault_ = new int[cir->ngate_];
		// used to count the number of equivalent faults
		vector<int> SA0_eq, SA1_eq;
		for (int i = 0; i < cir->ngate_; ++i)
		{
			gateToFault_[i] = faults_.size();
			if (!useFC)
			{
				// Without Fault Collapsing
				// extract fault of gate outputs
				// but do not extract faults between two time frames
				if (cir->gates_[i].nfo_ > 0 && i < cir->ngate_ - cir->nppi_)
				{
					if (cir->gates_[i].type_ != Gate::PPI)
					{
						faults_.push_back(new Fault(i, Fault::SA0, 0));
						faults_.push_back(new Fault(i, Fault::SA1, 0));
					}
					else
					{
						temp = new Fault(i, Fault::SA0, 0); // Q
						// temp->state_ = Fault::DT;
						faults_.push_back(temp);
						temp = new Fault(i, Fault::SA1, 0); // Q
						// temp->state_ = Fault::DT;
						faults_.push_back(temp);
					}
				}
				// extract faults of gate inputs
				for (int j = 0; j < cir->gates_[i].nfi_; ++j)
				{
					if (cir->gates_[cir->gates_[i].fis_[j]].nfo_ > 1)
					{
						faults_.push_back(new Fault(i, Fault::SA0, j + 1));
						faults_.push_back(new Fault(i, Fault::SA1, j + 1));
					}
				}
			}
			else
			{
				// Simple Equivalent Fault Collapsing
				SA0_eq.push_back(1);
				SA1_eq.push_back(1);
				// Input faults
				// AND, NAND gates
				if (cir->gates_[i].type_ == Gate::AND2 || cir->gates_[i].type_ == Gate::AND3 || cir->gates_[i].type_ == Gate::AND4)
				{
					for (int j = 0; j < cir->gates_[i].nfi_; ++j)
					{
						int temp0 = SA0_eq[cir->gates_[i].fis_[j]];
						int temp1 = SA1_eq[cir->gates_[i].fis_[j]];
						faults_.push_back(new Fault(i, Fault::SA1, j + 1, temp1));
						SA0_eq[i] += temp0;
					}
				}
				else if (cir->gates_[i].type_ == Gate::NAND2 || cir->gates_[i].type_ == Gate::NAND3 || cir->gates_[i].type_ == Gate::NAND4)
				{
					for (int j = 0; j < cir->gates_[i].nfi_; ++j)
					{
						int temp0 = SA0_eq[cir->gates_[i].fis_[j]];
						int temp1 = SA1_eq[cir->gates_[i].fis_[j]];
						faults_.push_back(new Fault(i, Fault::SA1, j + 1, temp1));
						SA1_eq[i] += temp0;
					}
				}
				// OR, NOR gates
				else if (cir->gates_[i].type_ == Gate::OR2 || cir->gates_[i].type_ == Gate::OR3 || cir->gates_[i].type_ == Gate::OR4)
				{
					for (int j = 0; j < cir->gates_[i].nfi_; ++j)
					{
						int temp0 = SA0_eq[cir->gates_[i].fis_[j]];
						int temp1 = SA1_eq[cir->gates_[i].fis_[j]];
						faults_.push_back(new Fault(i, Fault::SA0, j + 1, temp0));
						SA1_eq[i] += temp1;
					}
				}
				else if (cir->gates_[i].type_ == Gate::NOR2 || cir->gates_[i].type_ == Gate::NOR3 || cir->gates_[i].type_ == Gate::NOR4)
				{
					for (int j = 0; j < cir->gates_[i].nfi_; ++j)
					{
						int temp0 = SA0_eq[cir->gates_[i].fis_[j]];
						int temp1 = SA1_eq[cir->gates_[i].fis_[j]];
						faults_.push_back(new Fault(i, Fault::SA0, j + 1, temp0));
						SA0_eq[i] += temp1;
					}
				}
				else if (cir->gates_[i].type_ == Gate::INV || cir->gates_[i].type_ == Gate::BUF)
				{
					// We don't need to add fault at these two types
					int temp0 = SA0_eq[cir->gates_[i].fis_[0]];
					int temp1 = SA1_eq[cir->gates_[i].fis_[0]];
					if (cir->gates_[i].type_ == Gate::INV)
					{
						SA0_eq[i] = temp1 + 1;
						SA1_eq[i] = temp0 + 1;
					}
					else
					{
						SA0_eq[i] = temp0 + 1;
						SA1_eq[i] = temp1 + 1;
					}
				}
				// other gates, including PO,PPO
				else
				{
					for (int j = 0; j < cir->gates_[i].nfi_; ++j)
					{
						int temp0 = SA0_eq[cir->gates_[i].fis_[j]];
						int temp1 = SA1_eq[cir->gates_[i].fis_[j]];
						faults_.push_back(new Fault(i, Fault::SA0, j + 1, temp0));
						faults_.push_back(new Fault(i, Fault::SA1, j + 1, temp1));
					}
				}
				// Output faults
				// fanout stem, including PI,PPI with fanout stem
				if (cir->gates_[i].nfo_ > 1)
				{
					faults_.push_back(new Fault(i, Fault::SA0, 0, SA0_eq[i]));
					faults_.push_back(new Fault(i, Fault::SA1, 0, SA1_eq[i]));
					SA0_eq[i] = 1;
					SA1_eq[i] = 1;
				}
			}
			if (cir->gates_[i].type_ == Gate::PPI)
			{
				temp = new Fault(i, Fault::SA0, -1); // CK
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA0, -2); // SE
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA0, -3); // SI
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA0, -4); // QN
				temp->state_ = Fault::UD;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA1, -1); // CK
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA1, -2); // SE
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA1, -3); // SI
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::SA1, -4); // QN
				temp->state_ = Fault::UD;
				faults_.push_back(temp);
			}
		}

		// HYH try to fix the fault number @20141121

		for (size_t i = 0; i < cir->nl_->getTop()->getNPort(); ++i)
		{
			Port *p = cir->nl_->getTop()->getPort(i);
			if (!strcmp(p->name_, "CK"))
			{																			 // sequential circuit
				temp = new Fault(-1, Fault::SA0, 0); // CK
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-1, Fault::SA1, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);

				temp = new Fault(-2, Fault::SA0, 0); // test_si
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-2, Fault::SA1, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);

				temp = new Fault(-3, Fault::SA0, 0); // test_so
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-3, Fault::SA1, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);

				temp = new Fault(-4, Fault::SA0, 0); // test_se
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-4, Fault::SA1, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
			}
		}
		//
	} // end of stuck-at fault
		// for transition fault
		//
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
					temp = new Fault(i, Fault::STR, 0); // Q
					temp->state_ = Fault::DT;
					faults_.push_back(temp);
					temp = new Fault(i, Fault::STF, 0); // Q
					temp->state_ = Fault::DT;
					faults_.push_back(temp);
				}
			}
			for (int j = 0; j < cir->gates_[i].nfi_; ++j)
			{
				faults_.push_back(new Fault(i, Fault::STR, j + 1));
				faults_.push_back(new Fault(i, Fault::STF, j + 1));
			}
			if (cir->gates_[i].type_ == Gate::PPI)
			{
				temp = new Fault(i, Fault::STR, -1); // CK
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STR, -2); // SE
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STR, -3); // SI
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STF, -4); // QN
				temp->state_ = Fault::UD;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STF, -1); // CK
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STF, -2); // SE
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STF, -3); // SI
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(i, Fault::STF, -4); // QN
				temp->state_ = Fault::UD;
				faults_.push_back(temp);
			}
		}

		// HYH try to fix the fault number @20141121

		for (size_t i = 0; i < cir->nl_->getTop()->getNPort(); ++i)
		{
			Port *p = cir->nl_->getTop()->getPort(i);
			if (!strcmp(p->name_, "CK"))
			{																			 // sequential circuit
				temp = new Fault(-1, Fault::STR, 0); // CK
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-1, Fault::STF, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);

				temp = new Fault(-2, Fault::STR, 0); // test_si
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-2, Fault::STF, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);

				temp = new Fault(-3, Fault::STR, 0); // test_so
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-3, Fault::STF, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);

				temp = new Fault(-4, Fault::STR, 0); // test_se
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
				temp = new Fault(-4, Fault::STF, 0);
				temp->state_ = Fault::DT;
				faults_.push_back(temp);
			}
		}
		//

	} // end of transition fault
}
