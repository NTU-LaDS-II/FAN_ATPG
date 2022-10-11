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
		for (int i = 0; i < cir->ngate_; ++i)
		{
			gateToFault_[i] = faults_.size();
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
				faults_.push_back(new Fault(i, Fault::SA0, j + 1));
				faults_.push_back(new Fault(i, Fault::SA1, j + 1));
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
