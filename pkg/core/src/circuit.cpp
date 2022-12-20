// **************************************************************************
// File       [ circuit.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/05 created ]
// **************************************************************************

#include "circuit.h"

using namespace IntfNs;
using namespace CoreNs;

// map the circuit to our data structure
bool Circuit::build(Netlist *const nl, const int &nframe,
										const tfConnectType &type)
{
	nframe_ = nframe;
	Cell *top = nl->getTop();
	if (!top)
	{
		std::cerr << "**ERROR Circuit::build(): no top module set\n";
		return false;
	}
	Techlib *lib = nl->getTechlib();
	if (!lib)
	{
		std::cerr << "**ERROR Circuit::build(): no technology library\n";
		return false;
	}
	nl_ = nl;
	nframe_ = nframe;
	connType_ = type;

	// levelize
	nl->levelize();
	lib->levelize();

	// map netlist to circuit
	createMap();

	// allocate gate memory
	// gates_ = new Gate[ngate_ * nframe];
	// gates_.clear();
	// gates_ = std::vector<Gate>(ngate_ * nframe);
	gates_.resize(ngate_ * nframe);
	// fis_ = new int[nnet_ * nframe + nppi_ * (nframe - 1)];
	// fos_ = new int[nnet_ * nframe + nppi_ * (nframe - 1)];
	// fis_.resize(nnet_ * nframe + nppi_ * (nframe - 1));
	// fos_.resize(nnet_ * nframe + nppi_ * (nframe - 1));
	// create gates
	createGate();
	connectFrame(); // for multiple time frames
	assignFiMinLvl();

	return true;
}

// **************************************************************************
// Function   [ void Circuit::createMap() ]
// Author     [ littleshamoo ]
// Synopsis   [ map cell's and ports (in verilog netlist) into gates (in the circuit)
//
//              Gate layout
//              frame 1                           frame 2
//              |----- ------ ------ ----- ------|----- ------ ------
//              | PI1 | PPI1 | gate | PO1 | PPO1 | PI2 | PPI2 | gate | ...
//              |----- ------ ------ ----- ------|----- ------ ------
//            ]
// Date       [ ]
// **************************************************************************
void Circuit::createMap()
{
	calNgate();
	calNnet();
}

// calculate total number of gates
void Circuit::calNgate()
{
	Cell *top = nl_->getTop();
	Techlib *lib = nl_->getTechlib();

	ngate_ = 0; // number of gates
	// map PI to pseudo gates
	portToGate_.resize(top->getNPort());
	npi_ = 0;
	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *p = top->getPort(i);
		if (p->type_ != Port::INPUT)
		{
			continue;
		}

		if (!strcmp(p->name_, "CK") || !strcmp(p->name_, "test_si") || !strcmp(p->name_, "test_se"))
		{
			continue;
		}
		portToGate_[i] = npi_;
		++ngate_;
		++npi_;
	}

	// map cell to gates
	// a single cell can have more than one gate in it
	nppi_ = 0;
	cellToGate_.resize(top->getNCell());
	for (int i = 0; i < top->getNCell(); ++i)
	{
		cellToGate_[i] = ngate_;
		if (lib->hasPmt(top->getCell(i)->libc_->id_, Pmt::DFF))
		{
			++nppi_;
			++ngate_;
		}
		else
		{
			ngate_ += top->getCell(i)->libc_->getNCell();
		}
	}

	// calculate combinational gates
	ncomb_ = ngate_ - npi_ - nppi_;

	// map PO to pseudo gates
	npo_ = 0;
	for (int i = 0; i < top->getNPort(); ++i)
	{
		if (top->getPort(i)->type_ == Port::OUTPUT)
		{
			if (!strcmp(top->getPort(i)->name_, "test_so"))
			{
				continue;
			}
			portToGate_[i] = ngate_;
			++ngate_;
			++npo_;
		}
	}

	// calculate PPO
	ngate_ += nppi_;
}

// calculate number of nets
void Circuit::calNnet()
{
	Cell *top = nl_->getTop();
	Techlib *lib = nl_->getTechlib();

	nnet_ = 0;

	for (int i = 0; i < top->getNNet(); ++i)
	{
		NetSet eqvs = top->getEqvNets(i);
		int nports = 0;
		bool smallest = true;
		for (NetSet::iterator it = eqvs.begin(); it != eqvs.end(); ++it)
		{
			if ((*it)->id_ < i)
			{
				smallest = false;
				break;
			}
			nports += (*it)->getNPort();
		}
		if (smallest)
		{
			nnet_ += nports - 1;
		}
	}

	// add internal nets
	for (int i = 0; i < top->getNCell(); ++i)
	{
		Cell *c = top->getCell(i);
		if (!lib->hasPmt(c->typeName_, Pmt::DFF))
		{
			nnet_ += c->libc_->getNNet() - c->libc_->getNPort();
		}
	}
}

// **************************************************************************
// Function   [ createGate ]
// Commentor  [ Bill ]
// Synopsis   [ usage: create gate's PI,PPI,PP,PPO,Comb,initial all kind of gate's data
//              in:    None
//              out:   void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Circuit::createGate()
{
	int nfi = 0;
	int nfo = 0;
	createPi(nfo);
	createPpi(nfo);
	createComb(nfi, nfo);
	createPo(nfi);
	createPpo(nfi);
}

void Circuit::createPi(int &nfo)
{
	Cell *top = nl_->getTop();

	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *p = top->getPort(i);
		if (!strcmp(p->name_, "CK") || !strcmp(p->name_, "test_si") || !strcmp(p->name_, "test_se"))
		{
			continue;
		}
		int id = portToGate_[i];
		if (p->type_ != Port::INPUT || id < 0)
		{
			continue;
		}
		gates_[id].gateId_ = id;
		gates_[id].cellId_ = i;
		gates_[id].primitiveId_ = 0;
		gates_[id].numLevel_ = 0;
		gates_[id].gateType_ = Gate::PI;
		// gates_.emplace_back(id, i, 0, 0, Gate::PI);
		gates_[id].fanoutVector_.reserve(top->getNetPorts(p->inNet_->id_).size() - 1);
		// gates_[id].fanoutVector_ = &fos_[nfo];
		nfo += top->getNetPorts(p->inNet_->id_).size() - 1;
	}
}

void Circuit::createPpi(int &nfo)
{
	Cell *top = nl_->getTop();
	for (int i = 0; i < nppi_; ++i)
	{
		Cell *c = top->getCell(i);
		int id = cellToGate_[i];
		gates_[id].gateId_ = id;
		gates_[id].cellId_ = i;
		gates_[id].primitiveId_ = 0;
		gates_[id].numLevel_ = 0;
		gates_[id].gateType_ = Gate::PPI;
		// gates_[id].fanoutVector_ = &fos_[nfo];
		// int qid = 0;
		// while (strcmp(c->getPort(qid)->name_, "Q"))
		// 	++qid;
		// nfo += top->getNetPorts(c->getPort(qid)->exNet_->id_).size() - 2;
		// if (nframe_ > 1 && connType_ == SHIFT && i < nppi_ - 1)
		// 	++nfo;
		int qid = 0;
		int size = 0; // calculate size of fanoutVector_ of gates_[id]!
		while (strcmp(c->getPort(qid)->name_, "Q"))
		{
			++qid;
		}
		size += top->getNetPorts(c->getPort(qid)->exNet_->id_).size() - 2;
		nfo += top->getNetPorts(c->getPort(qid)->exNet_->id_).size() - 2;
		if (nframe_ > 1 && connType_ == SHIFT && i < nppi_ - 1)
		{
			++size;
			++nfo;
		}
		gates_[id].fanoutVector_.reserve(size);
	}
}

// create combinational logic gates
void Circuit::createComb(int &nfi, int &nfo)
{
	Cell *top = nl_->getTop();

	for (int i = nppi_; i < (int)top->getNCell(); ++i)
	{
		Cell *c = top->getCell(i);
		for (int j = 0; j < (int)c->libc_->getNCell(); ++j)
		{
			int id = cellToGate_[i] + j;
			gates_[id].gateId_ = id;
			gates_[id].cellId_ = c->id_;
			gates_[id].primitiveId_ = j;

			Pmt *pmt = (Pmt *)c->libc_->getCell(j);
			createPmt(id, c, pmt, nfi, nfo);
		}
	}
	if (top->getNCell() > 0)
	{
		lvl_ = gates_[cellToGate_[top->getNCell() - 1]].numLevel_ + 2;
	}
}

// primitive is from Mentor .mdt
// cell => primitive => gate
// but primitive is not actually in our data structure
void Circuit::createPmt(const int &id, const Cell *const c,
												const Pmt *const pmt, int &nfi, int &nfo)
{
	// determine gate type
	detGateType(id, c, pmt);

	// determine fanin and level
	// gates_[id].faninVector_ = &fis_[nfi]; // resize later!
	int maxLvl = -1;
	for (int i = 0; i < pmt->getNPort(); ++i)
	{
		if (pmt->getPort(i)->type_ != Port::INPUT)
		{
			continue;
		}
		Net *nin = pmt->getPort(i)->exNet_;
		gates_[id].faninVector_.reserve(nin->getNPort());
		// gates_[id].fanoutVector_.reserve(nin->getNPort());
		for (int j = 0; j < nin->getNPort(); ++j)
		{
			Port *p = nin->getPort(j);
			if (p == pmt->getPort(i))
			{
				continue;
			}

			int inId = 0;
			// internal connection
			if (p->type_ == Port::OUTPUT && p->top_ != c->libc_)
			{
				inId = cellToGate_[c->id_] + p->top_->id_;
			}
			else if (p->type_ == Port::INPUT && p->top_ == c->libc_) // external connection
			{
				Net *nex = c->getPort(p->id_)->exNet_;
				PortSet ps = c->top_->getNetPorts(nex->id_);
				PortSet::iterator it = ps.begin();
				for (; it != ps.end(); ++it)
				{
					Cell *cin = (*it)->top_;
					if ((*it)->type_ == Port::OUTPUT && cin != c->top_)
					{
						CellSet cs = cin->libc_->getPortCells((*it)->id_);
						inId = cellToGate_[cin->id_];
						if (nl_->getTechlib()->hasPmt(cin->libc_->id_, Pmt::DFF)) // NE
						{
							break;
						}
						inId += (*cs.begin())->id_;
						break;
					}
					else if ((*it)->type_ == Port::INPUT && cin == c->top_)
					{
						inId = portToGate_[(*it)->id_];
						break;
					}
				}
			}
			// gates_[id].faninVector_[gates_[id].numFI_] = inId;
			gates_[id].faninVector_.push_back(inId);
			++gates_[id].numFI_;
			++nfi;
			// gates_[inId].fanoutVector_[gates_[inId].numFO_] = id;
			gates_[inId].fanoutVector_.push_back(id);
			++gates_[inId].numFO_;

			if (gates_[inId].numLevel_ > maxLvl)
			{
				maxLvl = gates_[inId].numLevel_;
			}
		}
	}
	gates_[id].numLevel_ = maxLvl + 1;

	// determine fanout

	// gates_[id].fanoutVector_ = &fos_[nfo]; // resize later
	Port *outp = NULL;
	int foSize = 0;
	for (int i = 0; i < pmt->getNPort() && !outp; ++i)
	{
		if (pmt->getPort(i)->type_ == Port::OUTPUT)
		{
			outp = pmt->getPort(i);
		}
	}
	PortSet ps = c->libc_->getNetPorts(outp->exNet_->id_);
	PortSet::iterator it = ps.begin();
	for (; it != ps.end(); ++it)
	{
		if ((*it)->top_ != c->libc_ && (*it)->top_ != pmt)
		{
			nfo += 1;
			foSize += 1;
		}
		else if ((*it)->top_ == c->libc_)
		{
			int nid = c->getPort((*it)->id_)->exNet_->id_;
			nfo += c->top_->getNetPorts(nid).size() - 1;
			foSize += c->top_->getNetPorts(nid).size() - 1;
		}
	}
	gates_[id].fanoutVector_.reserve(foSize);
}

void Circuit::detGateType(const int &id, const Cell *const c,
													const Pmt *const pmt)
{
	switch (pmt->type_)
	{
		case Pmt::BUF:
			gates_[id].gateType_ = Gate::BUF;
			break;
		case Pmt::INV:
		case Pmt::INVF:
			gates_[id].gateType_ = Gate::INV;
			break;
		case Pmt::MUX:
			gates_[id].gateType_ = Gate::MUX;
			break;
		case Pmt::AND:
			if (c->getNPort() == 3)
			{
				gates_[id].gateType_ = Gate::AND2;
			}
			else if (c->getNPort() == 4)
			{
				gates_[id].gateType_ = Gate::AND3;
			}
			else if (c->getNPort() == 5)
			{
				gates_[id].gateType_ = Gate::AND4;
			}
			else
			{
				gates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::NAND:
			if (c->getNPort() == 3)
			{
				gates_[id].gateType_ = Gate::NAND2;
			}
			else if (c->getNPort() == 4)
			{
				gates_[id].gateType_ = Gate::NAND3;
			}
			else if (c->getNPort() == 5)
			{
				gates_[id].gateType_ = Gate::NAND4;
			}
			else
			{
				gates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::OR:
			if (c->getNPort() == 3)
			{
				gates_[id].gateType_ = Gate::OR2;
			}
			else if (c->getNPort() == 4)
			{
				gates_[id].gateType_ = Gate::OR3;
			}
			else if (c->getNPort() == 5)
			{
				gates_[id].gateType_ = Gate::OR4;
			}
			else
			{
				gates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::NOR:
			if (c->getNPort() == 3)
			{
				gates_[id].gateType_ = Gate::NOR2;
			}
			else if (c->getNPort() == 4)
			{
				gates_[id].gateType_ = Gate::NOR3;
			}
			else if (c->getNPort() == 5)
			{
				gates_[id].gateType_ = Gate::NOR4;
			}
			else
			{
				gates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::XOR:
			if (c->getNPort() == 3)
			{
				gates_[id].gateType_ = Gate::XOR2;
			}
			else if (c->getNPort() == 4)
			{
				gates_[id].gateType_ = Gate::XOR3;
			}
			else
			{
				gates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::XNOR:
			if (c->getNPort() == 3)
			{
				gates_[id].gateType_ = Gate::XNOR2;
			}
			else if (c->getNPort() == 4)
			{
				gates_[id].gateType_ = Gate::XNOR3;
			}
			else
			{
				gates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::TIE1:
			gates_[id].gateType_ = Gate::TIE1;
			break;
		case Pmt::TIE0:
			gates_[id].gateType_ = Gate::TIE0;
			break;
		case Pmt::TIEX:
			gates_[id].gateType_ = Gate::TIE0;
			break;
		case Pmt::TIEZ:
			gates_[id].gateType_ = Gate::TIEZ;
			break;
		default:
			gates_[id].gateType_ = Gate::NA;
	}
}

void Circuit::createPo(int &nfi)
{
	Cell *top = nl_->getTop();

	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *p = top->getPort(i);
		if (!strcmp(p->name_, "test_so"))
		{
			continue;
		}
		int id = portToGate_[i];
		if (p->type_ != Port::OUTPUT || id < 0)
		{
			continue;
		}
		gates_[id].gateId_ = id;
		gates_[id].cellId_ = i;
		gates_[id].primitiveId_ = 0;
		gates_[id].numLevel_ = lvl_ - 1;
		gates_[id].gateType_ = Gate::PO;
		// gates_[id].faninVector_ = &fis_[nfi];
		PortSet ps = top->getNetPorts(p->inNet_->id_);
		// gates_[id].faninVector_.reserve(ps.size());
		PortSet::iterator it = ps.begin();
		for (; it != ps.end(); ++it)
		{
			if ((*it) == p)
			{
				continue;
			}
			int inId = 0;
			if ((*it)->top_ == top && (*it)->type_ == Port::INPUT)
			{
				inId = portToGate_[(*it)->id_];
			}
			else if ((*it)->top_ != top && (*it)->type_ == Port::OUTPUT)
			{
				inId = cellToGate_[(*it)->top_->id_];
				Cell *libc = (*it)->top_->libc_;
				if (!nl_->getTechlib()->hasPmt(libc->id_, Pmt::DFF)) // NE
				{
					inId += (*libc->getPortCells((*it)->id_).begin())->id_;
				}
			}
			else
			{
				continue;
			}
			// gates_[inId].fanoutVector_.reserve(ps.size());
			// gates_[id].faninVector_[gates_[id].numFI_] = inId;
			gates_[id].faninVector_.push_back(inId);
			++gates_[id].numFI_;

			// gates_[inId].fanoutVector_[gates_[inId].numFO_] = id;
			gates_[inId].fanoutVector_.push_back(id);
			++gates_[inId].numFO_;

			++nfi;
		}
	}
}

void Circuit::createPpo(int &nfi)
{
	Cell *top = nl_->getTop();
	for (int i = 0; i < nppi_; ++i)
	{
		Cell *c = top->getCell(i);
		int id = ngate_ - nppi_ + i;
		gates_[id].gateId_ = id;
		gates_[id].cellId_ = i;
		gates_[id].primitiveId_ = 0;
		gates_[id].numLevel_ = lvl_ - 1;
		gates_[id].gateType_ = Gate::PPO;
		// gates_[id].faninVector_ = &fis_[nfi];
		int did = 0;
		while (strcmp(c->getPort(did)->name_, "D"))
		{
			++did;
		}
		Port *inp = c->getPort(did);

		PortSet ps = top->getNetPorts(c->getPort(inp->id_)->exNet_->id_);
		// gates_[id].faninVector_.reserve(ps.size());
		PortSet::iterator it = ps.begin();
		for (; it != ps.end(); ++it)
		{
			if ((*it) == inp)
			{
				continue;
			}
			int inId = 0;
			if ((*it)->top_ == top && (*it)->type_ == Port::INPUT)
			{
				inId = portToGate_[(*it)->id_];
			}
			else if ((*it)->top_ != top && (*it)->type_ == Port::OUTPUT)
			{
				inId = cellToGate_[(*it)->top_->id_];
				Cell *libc = (*it)->top_->libc_;
				if (!nl_->getTechlib()->hasPmt(libc->id_, Pmt::DFF)) // NE
				{
					inId += (*libc->getPortCells((*it)->id_).begin())->id_;
				}
			}
			else
			{
				continue;
			}
			// gates_[inId].fanoutVector_.reserve(ps.size());
			// gates_[id].faninVector_[gates_[id].numFI_] = inId;
			gates_[id].faninVector_.push_back(inId);
			++gates_[id].numFI_;

			// gates_[inId].fanoutVector_[gates_[inId].numFO_] = id;
			gates_[inId].fanoutVector_.push_back(id);
			++gates_[inId].numFO_;

			++nfi;
		}
	}
}

// **************************************************************************
// Function   [ connectFrame]
// Commentor  [ LingYunHsu]
// Synopsis   [ usage: connect gates in different time rames,
//              this is for multiple time frame ATPG
//              in:    void
//              out:   void
//            ]
// Date       [ LingYunHsu Ver. 1.0 started 20130818 ]
// **************************************************************************
void Circuit::connectFrame()
{
	int nfo = nnet_;
	int nfi = nnet_;
	for (int i = 1; i < nframe_; ++i)
	{
		int offset = ngate_ * i;
		if (connType_ == CAPTURE)
		{
			for (int j = 0; j < nppi_; ++j)
			{ /// give nfo and fos before nppi_
				int id = offset - nppi_ + j;
				gates_[id].numFO_ = 1;
				// gates_[id].fanoutVector_ = &fos_[nfo];
				nfo += gates_[id].numFO_;
				gates_[id].fanoutVector_.reserve(gates_[id].numFO_);
			}
		}
		for (int j = 0; j < ngate_; ++j)
		{ /// read pattern
			int id = offset + j;
			gates_[id].gateId_ = id;
			gates_[id].cellId_ = gates_[j].cellId_;
			gates_[id].numLevel_ = lvl_ * i + gates_[j].numLevel_;
			gates_[id].gateType_ = gates_[j].gateType_;
			gates_[id].frame_ = i; /// record id, cid, level, type and frame
			if (gates_[id].gateType_ != Gate::PPI && gates_[id].gateType_ != Gate::PPO)
			{
				gates_[id].numFO_ = gates_[j].numFO_;
				// gates_[id].fanoutVector_ = &fos_[nfo];
				gates_[id].fanoutVector_.resize(gates_[j].numFO_);
				for (int k = 0; k < gates_[j].numFO_; ++k)
				{
					gates_[id].fanoutVector_[k] = gates_[j].fanoutVector_[k] + offset;
				}
				nfo += gates_[id].numFO_;
				gates_[id].numFI_ = gates_[j].numFI_;
				// gates_[id].faninVector_ = &fis_[nfi];
				gates_[id].faninVector_.resize(gates_[j].numFI_);
				for (int k = 0; k < gates_[j].numFI_; ++k)
				{
					gates_[id].faninVector_[k] = gates_[j].faninVector_[k] + offset;
				}
				nfi += gates_[id].numFI_;
			} /// if not ppi or ppo
			else if (gates_[id].gateType_ == Gate::PPI)
			{
				gates_[id].numFO_ = gates_[j].numFO_;
				// if(connType_ == SHIFT && i == nframe_-1 && (j!= npi_+nppi_-1))--gates_[id].numFO_;
				// gates_[id].fanoutVector_ = &fos_[nfo];
				gates_[id].fanoutVector_.resize(gates_[j].numFO_);
				for (int k = 0; k < gates_[j].numFO_; ++k)
				{
					gates_[id].fanoutVector_[k] = gates_[j].fanoutVector_[k] + offset;
				}
				nfo += gates_[id].numFO_;
				gates_[id].numFI_ = 1;
				// gates_[id].faninVector_ = &fis_[nfi];
				gates_[id].faninVector_.resize(gates_[id].numFI_);
				if (connType_ == CAPTURE)
				{
					// gates_[id].faninVector_[0] = id - npi_ - nppi_;
					gates_[id].faninVector_.push_back(id - npi_ - nppi_);
					gates_[id].gateType_ = Gate::BUF;
					gates_[id - npi_ - nppi_].fanoutVector_[0] = id;
					// gates_[id - npi_ - nppi_].gateType_ = Gate::BUF ;
				} /// do CAPTURE
				else if (id != (offset + npi_))
				{
					gates_[id].faninVector_[0] = id - ngate_ - 1;
					// gates_[id].faninVector_ = {id - ngate_ - 1};
					gates_[id].gateType_ = Gate::BUF;
					gates_[id - ngate_ - 1].fanoutVector_[gates_[id - ngate_ - 1].numFO_] = id;
					++gates_[id - ngate_ - 1].numFO_;
				} /// do SHIFT
				else
				{
					gates_[id].numFI_ = 0;
				}
				// gates_[id - npi_ - nppi_].fanoutVector_[0] = id ;
				nfi += gates_[id].numFI_;
			} /// if ppi
			else
			{
				gates_[id].numFO_ = 0;
				// gates_[id].fanoutVector_ = &fos_[nfo];
				gates_[id].numFI_ = gates_[j].numFI_;
				// gates_[id].faninVector_ = &fis_[nfi];
				// gates_[id].faninVector_.resize(gates_[j].numFI_);
				for (int k = 0; k < gates_[j].numFI_; ++k)
				{
					gates_[id].faninVector_[k] = gates_[j].faninVector_[k] + offset;
				}
				nfi += gates_[id].numFI_;
			} /// if ppo
		}
		if (connType_ == CAPTURE)
		{
			for (int j = 0; j < nppi_; ++j)
			{
				int id = offset - nppi_ + j;
				gates_[id].gateType_ = Gate::BUF;
			} // reset after for loop
		}
	}
	tlvl_ = lvl_ * nframe_;
	tgate_ = ngate_ * nframe_;
}

// **************************************************************************
// Function   [ Circuit::assignFiMinLvl() ]
// Commentor  [ Jun-Han,Pan ]
// Synopsis   [ usage: Find the minimum from every gate's fan-ins' level and assign
//                     it to the gate->fiMinLvl.
//                     used for headLine justification (atpg.h)
//              in:    gate array.
//              out:   g->fiMinLvl_ is updated
//            ]
// Date       [ Jun-Han,Pan Ver. 1.0 at 2013/08/18 ]
// **************************************************************************
void Circuit::assignFiMinLvl()
{
	for (int i = 0; i < tgate_; ++i)
	{
		Gate *g = &gates_[i];
		int minLvl = g->numLevel_;
		for (int j = 0; j < g->numFI_; ++j)
		{
			if (gates_[g->faninVector_[j]].numLevel_ < minLvl)
			{
				minLvl = gates_[g->faninVector_[j]].numLevel_;
				g->fiMinLvl_ = g->faninVector_[j];
				//          find the minimum among g->fis_[j] and replace g->fiMinLvl with it.
				//                                *********
				//                       g->numFI_  *       *
				//          ************          *   g   *
				//          *g->fis_[j]* -------- *       *
				//          ************          *********
			}
		}
	}
}
