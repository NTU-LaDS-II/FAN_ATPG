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
bool Circuit::buildCircuit(Netlist *const pNetlist, const int &numFrame,
													 const TIME_FRAME_CONNECT_TYPE &timeFrameConnectType)
{
	numFrame_ = numFrame;
	Cell *top = pNetlist->getTop();
	if (!top)
	{
		std::cerr << "**ERROR Circuit::buildCircuit(): no top module set\n";
		return false;
	}
	Techlib *lib = pNetlist->getTechlib();
	if (!lib)
	{
		std::cerr << "**ERROR Circuit::buildCircuit(): no technology library\n";
		return false;
	}
	pNetlist_ = pNetlist;
	numFrame_ = numFrame;
	timeFrameConnectType_ = timeFrameConnectType;

	// levelize
	pNetlist->levelize();
	lib->levelize();

	// map netlist to circuit
	createMap();

	// allocate gate memory
	// circuitGates_ = new Gate[numGate_ * numFrame];
	// circuitGates_.clear();
	// circuitGates_ = std::vector<Gate>(numGate_ * numFrame);
	circuitGates_.resize(numGate_ * numFrame);
	// fis_ = new int[numNet_ * numFrame + numPPI_ * (numFrame - 1)];
	// fos_ = new int[numNet_ * numFrame + numPPI_ * (numFrame - 1)];
	// fis_.resize(numNet_ * numFrame + numPPI_ * (numFrame - 1));
	// fos_.resize(numNet_ * numFrame + numPPI_ * (numFrame - 1));
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
	Cell *top = pNetlist_->getTop();
	Techlib *lib = pNetlist_->getTechlib();

	numGate_ = 0; // number of gates
	// map PI to pseudo gates
	portIndexToGateIndex_.resize(top->getNPort());
	numPI_ = 0;
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
		portIndexToGateIndex_[i] = numPI_;
		++numGate_;
		++numPI_;
	}

	// map cell to gates
	// a single cell can have more than one gate in it
	numPPI_ = 0;
	cellIndexToGateIndex_.resize(top->getNCell());
	for (int i = 0; i < top->getNCell(); ++i)
	{
		cellIndexToGateIndex_[i] = numGate_;
		if (lib->hasPmt(top->getCell(i)->libc_->id_, Pmt::DFF))
		{
			++numPPI_;
			++numGate_;
		}
		else
		{
			numGate_ += top->getCell(i)->libc_->getNCell();
		}
	}

	// calculate combinational gates
	numComb_ = numGate_ - numPI_ - numPPI_;

	// map PO to pseudo gates
	numPO_ = 0;
	for (int i = 0; i < top->getNPort(); ++i)
	{
		if (top->getPort(i)->type_ == Port::OUTPUT)
		{
			if (!strcmp(top->getPort(i)->name_, "test_so"))
			{
				continue;
			}
			portIndexToGateIndex_[i] = numGate_;
			++numGate_;
			++numPO_;
		}
	}

	// calculate PPO
	numGate_ += numPPI_;
}

// calculate number of nets
void Circuit::calNnet()
{
	Cell *top = pNetlist_->getTop();
	Techlib *lib = pNetlist_->getTechlib();

	numNet_ = 0;

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
			numNet_ += nports - 1;
		}
	}

	// add internal nets
	for (int i = 0; i < top->getNCell(); ++i)
	{
		Cell *c = top->getCell(i);
		if (!lib->hasPmt(c->typeName_, Pmt::DFF))
		{
			numNet_ += c->libc_->getNNet() - c->libc_->getNPort();
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
	Cell *top = pNetlist_->getTop();

	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *p = top->getPort(i);
		if (!strcmp(p->name_, "CK") || !strcmp(p->name_, "test_si") || !strcmp(p->name_, "test_se"))
		{
			continue;
		}
		int id = portIndexToGateIndex_[i];
		if (p->type_ != Port::INPUT || id < 0)
		{
			continue;
		}
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = 0;
		circuitGates_[id].gateType_ = Gate::PI;
		// circuitGates_.emplace_back(id, i, 0, 0, Gate::PI);
		circuitGates_[id].fanoutVector_.reserve(top->getNetPorts(p->inNet_->id_).size() - 1);
		// circuitGates_[id].fanoutVector_ = &fos_[nfo];
		nfo += top->getNetPorts(p->inNet_->id_).size() - 1;
	}
}

void Circuit::createPpi(int &nfo)
{
	Cell *top = pNetlist_->getTop();
	for (int i = 0; i < numPPI_; ++i)
	{
		Cell *c = top->getCell(i);
		int id = cellIndexToGateIndex_[i];
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = 0;
		circuitGates_[id].gateType_ = Gate::PPI;
		// circuitGates_[id].fanoutVector_ = &fos_[nfo];
		// int qid = 0;
		// while (strcmp(c->getPort(qid)->name_, "Q"))
		// 	++qid;
		// nfo += top->getNetPorts(c->getPort(qid)->exNet_->id_).size() - 2;
		// if (numFrame_ > 1 && timeFrameConnectType_ == SHIFT && i < numPPI_ - 1)
		// 	++nfo;
		int qid = 0;
		int size = 0; // calculate size of fanoutVector_ of circuitGates_[id]!
		while (strcmp(c->getPort(qid)->name_, "Q"))
		{
			++qid;
		}
		size += top->getNetPorts(c->getPort(qid)->exNet_->id_).size() - 2;
		nfo += top->getNetPorts(c->getPort(qid)->exNet_->id_).size() - 2;
		if (numFrame_ > 1 && timeFrameConnectType_ == SHIFT && i < numPPI_ - 1)
		{
			++size;
			++nfo;
		}
		circuitGates_[id].fanoutVector_.reserve(size);
	}
}

// create combinational logic gates
void Circuit::createComb(int &nfi, int &nfo)
{
	Cell *top = pNetlist_->getTop();

	for (int i = numPPI_; i < (int)top->getNCell(); ++i)
	{
		Cell *c = top->getCell(i);
		for (int j = 0; j < (int)c->libc_->getNCell(); ++j)
		{
			int id = cellIndexToGateIndex_[i] + j;
			circuitGates_[id].gateId_ = id;
			circuitGates_[id].cellId_ = c->id_;
			circuitGates_[id].primitiveId_ = j;

			Pmt *pmt = (Pmt *)c->libc_->getCell(j);
			createPmt(id, c, pmt, nfi, nfo);
		}
	}
	if (top->getNCell() > 0)
	{
		circuitLvl_ = circuitGates_[cellIndexToGateIndex_[top->getNCell() - 1]].numLevel_ + 2;
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
	// circuitGates_[id].faninVector_ = &fis_[nfi]; // resize later!
	int maxLvl = -1;
	for (int i = 0; i < pmt->getNPort(); ++i)
	{
		if (pmt->getPort(i)->type_ != Port::INPUT)
		{
			continue;
		}
		Net *nin = pmt->getPort(i)->exNet_;
		circuitGates_[id].faninVector_.reserve(nin->getNPort());
		// circuitGates_[id].fanoutVector_.reserve(nin->getNPort());
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
				inId = cellIndexToGateIndex_[c->id_] + p->top_->id_;
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
						inId = cellIndexToGateIndex_[cin->id_];
						if (pNetlist_->getTechlib()->hasPmt(cin->libc_->id_, Pmt::DFF)) // NE
						{
							break;
						}
						inId += (*cs.begin())->id_;
						break;
					}
					else if ((*it)->type_ == Port::INPUT && cin == c->top_)
					{
						inId = portIndexToGateIndex_[(*it)->id_];
						break;
					}
				}
			}
			// circuitGates_[id].faninVector_[circuitGates_[id].numFI_] = inId;
			circuitGates_[id].faninVector_.push_back(inId);
			++circuitGates_[id].numFI_;
			++nfi;
			// circuitGates_[inId].fanoutVector_[circuitGates_[inId].numFO_] = id;
			circuitGates_[inId].fanoutVector_.push_back(id);
			++circuitGates_[inId].numFO_;

			if (circuitGates_[inId].numLevel_ > maxLvl)
			{
				maxLvl = circuitGates_[inId].numLevel_;
			}
		}
	}
	circuitGates_[id].numLevel_ = maxLvl + 1;

	// determine fanout

	// circuitGates_[id].fanoutVector_ = &fos_[nfo]; // resize later
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
	circuitGates_[id].fanoutVector_.reserve(foSize);
}

void Circuit::detGateType(const int &id, const Cell *const c,
													const Pmt *const pmt)
{
	switch (pmt->type_)
	{
		case Pmt::BUF:
			circuitGates_[id].gateType_ = Gate::BUF;
			break;
		case Pmt::INV:
		case Pmt::INVF:
			circuitGates_[id].gateType_ = Gate::INV;
			break;
		case Pmt::MUX:
			circuitGates_[id].gateType_ = Gate::MUX;
			break;
		case Pmt::AND:
			if (c->getNPort() == 3)
			{
				circuitGates_[id].gateType_ = Gate::AND2;
			}
			else if (c->getNPort() == 4)
			{
				circuitGates_[id].gateType_ = Gate::AND3;
			}
			else if (c->getNPort() == 5)
			{
				circuitGates_[id].gateType_ = Gate::AND4;
			}
			else
			{
				circuitGates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::NAND:
			if (c->getNPort() == 3)
			{
				circuitGates_[id].gateType_ = Gate::NAND2;
			}
			else if (c->getNPort() == 4)
			{
				circuitGates_[id].gateType_ = Gate::NAND3;
			}
			else if (c->getNPort() == 5)
			{
				circuitGates_[id].gateType_ = Gate::NAND4;
			}
			else
			{
				circuitGates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::OR:
			if (c->getNPort() == 3)
			{
				circuitGates_[id].gateType_ = Gate::OR2;
			}
			else if (c->getNPort() == 4)
			{
				circuitGates_[id].gateType_ = Gate::OR3;
			}
			else if (c->getNPort() == 5)
			{
				circuitGates_[id].gateType_ = Gate::OR4;
			}
			else
			{
				circuitGates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::NOR:
			if (c->getNPort() == 3)
			{
				circuitGates_[id].gateType_ = Gate::NOR2;
			}
			else if (c->getNPort() == 4)
			{
				circuitGates_[id].gateType_ = Gate::NOR3;
			}
			else if (c->getNPort() == 5)
			{
				circuitGates_[id].gateType_ = Gate::NOR4;
			}
			else
			{
				circuitGates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::XOR:
			if (c->getNPort() == 3)
			{
				circuitGates_[id].gateType_ = Gate::XOR2;
			}
			else if (c->getNPort() == 4)
			{
				circuitGates_[id].gateType_ = Gate::XOR3;
			}
			else
			{
				circuitGates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::XNOR:
			if (c->getNPort() == 3)
			{
				circuitGates_[id].gateType_ = Gate::XNOR2;
			}
			else if (c->getNPort() == 4)
			{
				circuitGates_[id].gateType_ = Gate::XNOR3;
			}
			else
			{
				circuitGates_[id].gateType_ = Gate::NA;
			}
			break;
		case Pmt::TIE1:
			circuitGates_[id].gateType_ = Gate::TIE1;
			break;
		case Pmt::TIE0:
			circuitGates_[id].gateType_ = Gate::TIE0;
			break;
		case Pmt::TIEX:
			circuitGates_[id].gateType_ = Gate::TIE0;
			break;
		case Pmt::TIEZ:
			circuitGates_[id].gateType_ = Gate::TIEZ;
			break;
		default:
			circuitGates_[id].gateType_ = Gate::NA;
	}
}

void Circuit::createPo(int &nfi)
{
	Cell *top = pNetlist_->getTop();

	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *p = top->getPort(i);
		if (!strcmp(p->name_, "test_so"))
		{
			continue;
		}
		int id = portIndexToGateIndex_[i];
		if (p->type_ != Port::OUTPUT || id < 0)
		{
			continue;
		}
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = circuitLvl_ - 1;
		circuitGates_[id].gateType_ = Gate::PO;
		// circuitGates_[id].faninVector_ = &fis_[nfi];
		PortSet ps = top->getNetPorts(p->inNet_->id_);
		// circuitGates_[id].faninVector_.reserve(ps.size());
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
				inId = portIndexToGateIndex_[(*it)->id_];
			}
			else if ((*it)->top_ != top && (*it)->type_ == Port::OUTPUT)
			{
				inId = cellIndexToGateIndex_[(*it)->top_->id_];
				Cell *libc = (*it)->top_->libc_;
				if (!pNetlist_->getTechlib()->hasPmt(libc->id_, Pmt::DFF)) // NE
				{
					inId += (*libc->getPortCells((*it)->id_).begin())->id_;
				}
			}
			else
			{
				continue;
			}
			// circuitGates_[inId].fanoutVector_.reserve(ps.size());
			// circuitGates_[id].faninVector_[circuitGates_[id].numFI_] = inId;
			circuitGates_[id].faninVector_.push_back(inId);
			++circuitGates_[id].numFI_;

			// circuitGates_[inId].fanoutVector_[circuitGates_[inId].numFO_] = id;
			circuitGates_[inId].fanoutVector_.push_back(id);
			++circuitGates_[inId].numFO_;

			++nfi;
		}
	}
}

void Circuit::createPpo(int &nfi)
{
	Cell *top = pNetlist_->getTop();
	for (int i = 0; i < numPPI_; ++i)
	{
		Cell *c = top->getCell(i);
		int id = numGate_ - numPPI_ + i;
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = circuitLvl_ - 1;
		circuitGates_[id].gateType_ = Gate::PPO;
		// circuitGates_[id].faninVector_ = &fis_[nfi];
		int did = 0;
		while (strcmp(c->getPort(did)->name_, "D"))
		{
			++did;
		}
		Port *inp = c->getPort(did);

		PortSet ps = top->getNetPorts(c->getPort(inp->id_)->exNet_->id_);
		// circuitGates_[id].faninVector_.reserve(ps.size());
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
				inId = portIndexToGateIndex_[(*it)->id_];
			}
			else if ((*it)->top_ != top && (*it)->type_ == Port::OUTPUT)
			{
				inId = cellIndexToGateIndex_[(*it)->top_->id_];
				Cell *libc = (*it)->top_->libc_;
				if (!pNetlist_->getTechlib()->hasPmt(libc->id_, Pmt::DFF)) // NE
				{
					inId += (*libc->getPortCells((*it)->id_).begin())->id_;
				}
			}
			else
			{
				continue;
			}
			// circuitGates_[inId].fanoutVector_.reserve(ps.size());
			// circuitGates_[id].faninVector_[circuitGates_[id].numFI_] = inId;
			circuitGates_[id].faninVector_.push_back(inId);
			++circuitGates_[id].numFI_;

			// circuitGates_[inId].fanoutVector_[circuitGates_[inId].numFO_] = id;
			circuitGates_[inId].fanoutVector_.push_back(id);
			++circuitGates_[inId].numFO_;

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
	int nfo = numNet_;
	int nfi = numNet_;
	for (int i = 1; i < numFrame_; ++i)
	{
		int offset = numGate_ * i;
		if (timeFrameConnectType_ == CAPTURE)
		{
			for (int j = 0; j < numPPI_; ++j)
			{ /// give nfo and fos before numPPI_
				int id = offset - numPPI_ + j;
				circuitGates_[id].numFO_ = 1;
				// circuitGates_[id].fanoutVector_ = &fos_[nfo];
				nfo += circuitGates_[id].numFO_;
				circuitGates_[id].fanoutVector_.reserve(circuitGates_[id].numFO_);
			}
		}
		for (int j = 0; j < numGate_; ++j)
		{ /// read pattern
			int id = offset + j;
			circuitGates_[id].gateId_ = id;
			circuitGates_[id].cellId_ = circuitGates_[j].cellId_;
			circuitGates_[id].numLevel_ = circuitLvl_ * i + circuitGates_[j].numLevel_;
			circuitGates_[id].gateType_ = circuitGates_[j].gateType_;
			circuitGates_[id].frame_ = i; /// record id, cid, level, type and frame
			if (circuitGates_[id].gateType_ != Gate::PPI && circuitGates_[id].gateType_ != Gate::PPO)
			{
				circuitGates_[id].numFO_ = circuitGates_[j].numFO_;
				// gates_[id].fanoutVector_ = &fos_[nfo];
				circuitGates_[id].fanoutVector_.resize(circuitGates_[j].numFO_);
				for (int k = 0; k < circuitGates_[j].numFO_; ++k)
				{
					circuitGates_[id].fanoutVector_[k] = circuitGates_[j].fanoutVector_[k] + offset;
				}
				nfo += circuitGates_[id].numFO_;
				circuitGates_[id].numFI_ = circuitGates_[j].numFI_;
				// circuitGates_[id].faninVector_ = &fis_[nfi];
				circuitGates_[id].faninVector_.resize(circuitGates_[j].numFI_);
				for (int k = 0; k < circuitGates_[j].numFI_; ++k)
				{
					circuitGates_[id].faninVector_[k] = circuitGates_[j].faninVector_[k] + offset;
				}
				nfi += circuitGates_[id].numFI_;
			} /// if not ppi or ppo
			else if (circuitGates_[id].gateType_ == Gate::PPI)
			{
				circuitGates_[id].numFO_ = circuitGates_[j].numFO_;
				// if(timeFrameConnectType_ == SHIFT && i == numFrame_-1 && (j!= numPI_+numPPI_-1))--circuitGates_[id].numFO_;
				// circuitGates_[id].fanoutVector_ = &fos_[nfo];
				circuitGates_[id].fanoutVector_.resize(circuitGates_[j].numFO_);
				for (int k = 0; k < circuitGates_[j].numFO_; ++k)
				{
					circuitGates_[id].fanoutVector_[k] = circuitGates_[j].fanoutVector_[k] + offset;
				}
				nfo += circuitGates_[id].numFO_;
				circuitGates_[id].numFI_ = 1;
				// circuitGates_[id].faninVector_ = &fis_[nfi];
				circuitGates_[id].faninVector_.resize(circuitGates_[id].numFI_);
				if (timeFrameConnectType_ == CAPTURE)
				{
					// circuitGates_[id].faninVector_[0] = id - numPI_ - numPPI_;
					circuitGates_[id].faninVector_.push_back(id - numPI_ - numPPI_);
					circuitGates_[id].gateType_ = Gate::BUF;
					circuitGates_[id - numPI_ - numPPI_].fanoutVector_[0] = id;
					// circuitGates_[id - numPI_ - numPPI_].gateType_ = Gate::BUF ;
				} /// do CAPTURE
				else if (id != (offset + numPI_))
				{
					circuitGates_[id].faninVector_[0] = id - numGate_ - 1;
					// circuitGates_[id].faninVector_ = {id - numGate_ - 1};
					circuitGates_[id].gateType_ = Gate::BUF;
					circuitGates_[id - numGate_ - 1].fanoutVector_[circuitGates_[id - numGate_ - 1].numFO_] = id;
					++circuitGates_[id - numGate_ - 1].numFO_;
				} /// do SHIFT
				else
				{
					circuitGates_[id].numFI_ = 0;
				}
				// circuitGates_[id - numPI_ - numPPI_].fanoutVector_[0] = id ;
				nfi += circuitGates_[id].numFI_;
			} /// if ppi
			else
			{
				circuitGates_[id].numFO_ = 0;
				// circuitGates_[id].fanoutVector_ = &fos_[nfo];
				circuitGates_[id].numFI_ = circuitGates_[j].numFI_;
				// circuitGates_[id].faninVector_ = &fis_[nfi];
				// circuitGates_[id].faninVector_.resize(circuitGates_[j].numFI_);
				for (int k = 0; k < circuitGates_[j].numFI_; ++k)
				{
					circuitGates_[id].faninVector_[k] = circuitGates_[j].faninVector_[k] + offset;
				}
				nfi += circuitGates_[id].numFI_;
			} /// if ppo
		}
		if (timeFrameConnectType_ == CAPTURE)
		{
			for (int j = 0; j < numPPI_; ++j)
			{
				int id = offset - numPPI_ + j;
				circuitGates_[id].gateType_ = Gate::BUF;
			} // reset after for loop
		}
	}
	totalLvl_ = circuitLvl_ * numFrame_;
	totalGate_ = numGate_ * numFrame_;
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
	for (int i = 0; i < totalGate_; ++i)
	{
		Gate *g = &circuitGates_[i];
		int minLvl = g->numLevel_;
		for (int j = 0; j < g->numFI_; ++j)
		{
			if (circuitGates_[g->faninVector_[j]].numLevel_ < minLvl)
			{
				minLvl = circuitGates_[g->faninVector_[j]].numLevel_;
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
