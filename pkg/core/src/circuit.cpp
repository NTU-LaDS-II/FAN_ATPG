// **************************************************************************
// File       [ circuit.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ function definition for circuit.h ]
// Date       [ 2011/07/05 created ]
// **************************************************************************

#include "circuit.h"

using namespace IntfNs;
using namespace CoreNs;

// **************************************************************************
// Function   [ Circuit::buildCircuit ]
// Commenter  [ Pan ]
// Synopsis   [ usage: map the circuit to our data structure
//              in:    Netlist *const pNetlist, const int &numFrame
//                     , const TIME_FRAME_CONNECT_TYPE &timeFrameConnectType
//              out:   bool
//            ]
// Date       [ Ver. 1.0 started 20130811 last modified 20221230]
// **************************************************************************
bool Circuit::buildCircuit(Netlist *const pNetlist, const int &numFrame,
													 const TIME_FRAME_CONNECT_TYPE &timeFrameConnectType)
{
	Cell *top = pNetlist->getTop();
	if (!top)
	{
		std::cerr << "**ERROR Circuit::buildCircuit(): no top module set\n";
		return false;
	}
	Techlib *techlib = pNetlist->getTechlib();
	if (!techlib)
	{
		std::cerr << "**ERROR Circuit::buildCircuit(): no technology library\n";
		return false;
	}
	pNetlist_ = pNetlist;
	numFrame_ = numFrame;
	timeFrameConnectType_ = timeFrameConnectType;

	// levelize
	pNetlist->levelize();
	techlib->levelize();

	// map netlist to circuit
	mapNetlistToCircuit();

	// allocate gate memory
	circuitGates_.resize(numGate_ * numFrame);

	// create gates
	createCircuitGates();
	connectMultipleTimeFrame(); // for multiple time frames
	assignFiMinLvl();

	return true;
}

// **************************************************************************
// Function   [ void Circuit::mapNetlistToCircuit() ]
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
void Circuit::mapNetlistToCircuit()
{
	calculateNumGate();
	calculateNumNet();
}

// **************************************************************************
// Function   [ calculateNumGate ]
// Commenter  [ Pan ]
// Synopsis   [ usage: calculate the number of gates in circuit
//                     and the number of PI, PPI(PPO), PO and combinational gates
//                     ,also determine the mapping from cell,port to gate
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::calculateNumGate()
{
	Cell *top = pNetlist_->getTop();
	Techlib *techlib = pNetlist_->getTechlib();

	numGate_ = 0; // number of gates

	// map PI to pseudo gates
	portIndexToGateIndex_.resize(top->getNPort());
	numPI_ = 0;
	for (int i = 0; i < top->getNPort(); ++i)
	{
		if (top->getPort(i)->type_ == Port::INPUT) // Input port
		{
			if (strcmp(top->getPort(i)->name_, "CK") && strcmp(top->getPort(i)->name_, "test_si") && strcmp(top->getPort(i)->name_, "test_se")) // Not clock or test input
			{
				portIndexToGateIndex_[i] = numPI_;
				++numGate_;
				++numPI_;
			}
		}
	}

	// map cell to gates
	// a single cell can have more than one gate in it
	numPPI_ = 0;
	cellIndexToGateIndex_.resize(top->getNCell());
	for (int i = 0; i < top->getNCell(); ++i)
	{
		cellIndexToGateIndex_[i] = numGate_;

		if (techlib->hasPmt(top->getCell(i)->libc_->id_, Pmt::DFF)) // D flip flop, indicates a PPI (PPO)
		{
			++numPPI_;
			++numGate_;
		}
		else
		{
			numGate_ += top->getCell(i)->libc_->getNCell();
		}
	}

	// calculate the number of combinational gates
	numComb_ = numGate_ - numPI_ - numPPI_;

	// map PO to pseudo gates
	numPO_ = 0;
	for (int i = 0; i < top->getNPort(); ++i)
	{
		if (top->getPort(i)->type_ == Port::OUTPUT) // Output port
		{
			if (strcmp(top->getPort(i)->name_, "test_so")) // Not test output
			{
				portIndexToGateIndex_[i] = numGate_;
				++numGate_;
				++numPO_;
			}
		}
	}

	// add the number of PPOs (PPIs) to the number of gates
	numGate_ += numPPI_;
}

// **************************************************************************
// Function   [ calculateNumNet ]
// Commenter  [ Pan ]
// Synopsis   [ usage: calculate the number of nets
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::calculateNumNet()
{
	Cell *top = pNetlist_->getTop();
	Techlib *techlib = pNetlist_->getTechlib();

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
		Cell *cell = top->getCell(i);
		if (!techlib->hasPmt(cell->typeName_, Pmt::DFF))
		{
			numNet_ += cell->libc_->getNNet() - cell->libc_->getNPort();
		}
	}
}

// **************************************************************************
// Function   [ createCircuitGates ]
// Commenter  [ Bill ]
// Synopsis   [ usage: create gate's PI,PPI,PP,PPO,Comb,initial all kind of gate's data
//              in:    None
//              out:   void
//            ]
// Date       [ Bill Ver. 1.0 started 20130811 ]
// **************************************************************************
void Circuit::createCircuitGates()
{
	createCircuitPI();
	createCircuitPPI();
	createCircuitComb();
	createCircuitPO();
	createCircuitPPO();
}

// **************************************************************************
// Function   [ createCircuitPI ]
// Commenter  [ Pan ]
// Synopsis   [ usage: create PI gates of the circuit
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::createCircuitPI()
{
	Cell *top = pNetlist_->getTop();

	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *port = top->getPort(i);
		if (!strcmp(port->name_, "CK") || !strcmp(port->name_, "test_si") || !strcmp(port->name_, "test_se"))
		{
			continue;
		}
		int id = portIndexToGateIndex_[i];
		if (port->type_ != Port::INPUT || id < 0)
		{
			continue;
		}
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = 0;
		circuitGates_[id].gateType_ = Gate::PI;
		circuitGates_[id].fanoutVector_.reserve(top->getNetPorts(port->inNet_->id_).size() - 1);
	}
}

// **************************************************************************
// Function   [ createCircuitPPI ]
// Commenter  [ Pan ]
// Synopsis   [ usage: create PPI gates of the circuit
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::createCircuitPPI()
{
	Cell *top = pNetlist_->getTop();
	for (int i = 0; i < numPPI_; ++i)
	{
		Cell *cell = top->getCell(i);
		int id = cellIndexToGateIndex_[i];
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = 0;
		circuitGates_[id].gateType_ = Gate::PPI;

		int qid = 0;
		int size = 0; // calculate size of fanoutVector_ of circuitGates_[id]
		while (strcmp(cell->getPort(qid)->name_, "Q"))
		{
			++qid;
		}
		size += top->getNetPorts(cell->getPort(qid)->exNet_->id_).size() - 2;
		if (numFrame_ > 1 && timeFrameConnectType_ == SHIFT && i < numPPI_ - 1)
		{
			++size;
		}
		circuitGates_[id].fanoutVector_.reserve(size);
	}
}

// **************************************************************************
// Function   [ createCircuitComb ]
// Commenter  [ Pan ]
// Synopsis   [ usage: create combinational logic gates of the circuit
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::createCircuitComb()
{
	Cell *top = pNetlist_->getTop();

	for (int i = numPPI_; i < (int)top->getNCell(); ++i)
	{
		Cell *cell = top->getCell(i);
		for (int j = 0; j < (int)cell->libc_->getNCell(); ++j)
		{
			int id = cellIndexToGateIndex_[i] + j;
			circuitGates_[id].gateId_ = id;
			circuitGates_[id].cellId_ = cell->id_;
			circuitGates_[id].primitiveId_ = j;

			Pmt *pmt = (Pmt *)cell->libc_->getCell(j);
			createCircuitPmt(id, cell, pmt);
		}
	}
	if (top->getNCell() > 0)
	{
		circuitLvl_ = circuitGates_[cellIndexToGateIndex_[top->getNCell() - 1]].numLevel_ + 2;
	}
}

// **************************************************************************
// Function   [ createCircuitPmt ]
// Commenter  [ Pan ]
// Synopsis   [ usage: create primitives of the circuit
//                     primitive is from Mentor .mdt
//                     cell => primitive => gate
//                     but primitive is not actually in our data structure
//              in:    gateID, cell, pmt
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::createCircuitPmt(const int &gateID, const Cell *const cell,
                               const Pmt *const pmt)
{
	// determine gate type
	determineGateType(gateID, cell, pmt);

	// determine fanin and level
	int maxLvl = -1;
	for (int i = 0; i < pmt->getNPort(); ++i)
	{
		if (pmt->getPort(i)->type_ != Port::INPUT)
		{
			continue;
		}
		Net *nin = pmt->getPort(i)->exNet_;
		circuitGates_[gateID].faninVector_.reserve(nin->getNPort());
		for (int j = 0; j < nin->getNPort(); ++j)
		{
			Port *port = nin->getPort(j);
			if (port == pmt->getPort(i))
			{
				continue;
			}

			int inId = 0;
			// internal connection
			if (port->type_ == Port::OUTPUT && port->top_ != cell->libc_)
			{
				inId = cellIndexToGateIndex_[cell->id_] + port->top_->id_;
			}
			else if (port->type_ == Port::INPUT && port->top_ == cell->libc_) // external connection
			{
				Net *nex = cell->getPort(port->id_)->exNet_;
				PortSet ps = cell->top_->getNetPorts(nex->id_);
				PortSet::iterator it = ps.begin();
				for (; it != ps.end(); ++it)
				{
					Cell *cin = (*it)->top_;
					if ((*it)->type_ == Port::OUTPUT && cin != cell->top_)
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
					else if ((*it)->type_ == Port::INPUT && cin == cell->top_)
					{
						inId = portIndexToGateIndex_[(*it)->id_];
						break;
					}
				}
			}
			circuitGates_[gateID].faninVector_.push_back(inId);
			++circuitGates_[gateID].numFI_;
			circuitGates_[inId].fanoutVector_.push_back(gateID);
			++circuitGates_[inId].numFO_;

			if (circuitGates_[inId].numLevel_ > maxLvl)
			{
				maxLvl = circuitGates_[inId].numLevel_;
			}
		}
	}
	circuitGates_[gateID].numLevel_ = maxLvl + 1;

	// determine fanout

	Port *outp = NULL;
	int foSize = 0;
	for (int i = 0; i < pmt->getNPort() && !outp; ++i)
	{
		if (pmt->getPort(i)->type_ == Port::OUTPUT)
		{
			outp = pmt->getPort(i);
		}
	}
	PortSet ps = cell->libc_->getNetPorts(outp->exNet_->id_);
	PortSet::iterator it = ps.begin();
	for (; it != ps.end(); ++it)
	{
		if ((*it)->top_ != cell->libc_ && (*it)->top_ != pmt)
		{
			++foSize;
		}
		else if ((*it)->top_ == cell->libc_)
		{
			int nid = cell->getPort((*it)->id_)->exNet_->id_;
			foSize += cell->top_->getNetPorts(nid).size() - 1;
		}
	}
	circuitGates_[gateID].fanoutVector_.reserve(foSize);
}

// **************************************************************************
// Function   [ determineGateType ]
// Commenter  [ Pan ]
// Synopsis   [ usage: determine the gateType_ of the gates
//              in:    gateID, cell, pmt
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::determineGateType(const int &gateID, const Cell *const cell,
                                const Pmt *const pmt)
{
	switch (pmt->type_)
	{
		case Pmt::BUF:
			circuitGates_[gateID].gateType_ = Gate::BUF;
			break;
		case Pmt::INV:
		case Pmt::INVF:
			circuitGates_[gateID].gateType_ = Gate::INV;
			break;
		case Pmt::MUX:
			circuitGates_[gateID].gateType_ = Gate::MUX;
			break;
		case Pmt::AND:
			if (cell->getNPort() == 3)
			{
				circuitGates_[gateID].gateType_ = Gate::AND2;
			}
			else if (cell->getNPort() == 4)
			{
				circuitGates_[gateID].gateType_ = Gate::AND3;
			}
			else if (cell->getNPort() == 5)
			{
				circuitGates_[gateID].gateType_ = Gate::AND4;
			}
			else
			{
				circuitGates_[gateID].gateType_ = Gate::NA;
			}
			break;
		case Pmt::NAND:
			if (cell->getNPort() == 3)
			{
				circuitGates_[gateID].gateType_ = Gate::NAND2;
			}
			else if (cell->getNPort() == 4)
			{
				circuitGates_[gateID].gateType_ = Gate::NAND3;
			}
			else if (cell->getNPort() == 5)
			{
				circuitGates_[gateID].gateType_ = Gate::NAND4;
			}
			else
			{
				circuitGates_[gateID].gateType_ = Gate::NA;
			}
			break;
		case Pmt::OR:
			if (cell->getNPort() == 3)
			{
				circuitGates_[gateID].gateType_ = Gate::OR2;
			}
			else if (cell->getNPort() == 4)
			{
				circuitGates_[gateID].gateType_ = Gate::OR3;
			}
			else if (cell->getNPort() == 5)
			{
				circuitGates_[gateID].gateType_ = Gate::OR4;
			}
			else
			{
				circuitGates_[gateID].gateType_ = Gate::NA;
			}
			break;
		case Pmt::NOR:
			if (cell->getNPort() == 3)
			{
				circuitGates_[gateID].gateType_ = Gate::NOR2;
			}
			else if (cell->getNPort() == 4)
			{
				circuitGates_[gateID].gateType_ = Gate::NOR3;
			}
			else if (cell->getNPort() == 5)
			{
				circuitGates_[gateID].gateType_ = Gate::NOR4;
			}
			else
			{
				circuitGates_[gateID].gateType_ = Gate::NA;
			}
			break;
		case Pmt::XOR:
			if (cell->getNPort() == 3)
			{
				circuitGates_[gateID].gateType_ = Gate::XOR2;
			}
			else if (cell->getNPort() == 4)
			{
				circuitGates_[gateID].gateType_ = Gate::XOR3;
			}
			else
			{
				circuitGates_[gateID].gateType_ = Gate::NA;
			}
			break;
		case Pmt::XNOR:
			if (cell->getNPort() == 3)
			{
				circuitGates_[gateID].gateType_ = Gate::XNOR2;
			}
			else if (cell->getNPort() == 4)
			{
				circuitGates_[gateID].gateType_ = Gate::XNOR3;
			}
			else
			{
				circuitGates_[gateID].gateType_ = Gate::NA;
			}
			break;
		case Pmt::TIE1:
			circuitGates_[gateID].gateType_ = Gate::TIE1;
			break;
		case Pmt::TIE0:
			circuitGates_[gateID].gateType_ = Gate::TIE0;
			break;
		case Pmt::TIEX:
			circuitGates_[gateID].gateType_ = Gate::TIE0;
			break;
		case Pmt::TIEZ:
			circuitGates_[gateID].gateType_ = Gate::TIEZ;
			break;
		default:
			circuitGates_[gateID].gateType_ = Gate::NA;
	}
}

// **************************************************************************
// Function   [ createCircuitPO ]
// Commenter  [ Pan ]
// Synopsis   [ usage: create PO gates of the circuit
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::createCircuitPO()
{
	Cell *top = pNetlist_->getTop();

	for (int i = 0; i < top->getNPort(); ++i)
	{
		Port *port = top->getPort(i);
		if (!strcmp(port->name_, "test_so"))
		{
			continue;
		}
		int id = portIndexToGateIndex_[i];
		if (port->type_ != Port::OUTPUT || id < 0)
		{
			continue;
		}
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = circuitLvl_ - 1;
		circuitGates_[id].gateType_ = Gate::PO;

		PortSet ps = top->getNetPorts(port->inNet_->id_);
		PortSet::iterator it = ps.begin();
		for (; it != ps.end(); ++it)
		{
			if ((*it) == port)
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

			circuitGates_[id].faninVector_.push_back(inId);
			++circuitGates_[id].numFI_;
			circuitGates_[inId].fanoutVector_.push_back(id);
			++circuitGates_[inId].numFO_;
		}
	}
}

// **************************************************************************
// Function   [ createCircuitPPO ]
// Commenter  [ Pan ]
// Synopsis   [ usage: create PPO gates of the circuit
//              in:    None
//              out:   void
//            ]
// Date       [ Pan Ver. 1.0 started 20130811 lase modified 20221230]
// **************************************************************************
void Circuit::createCircuitPPO()
{
	Cell *top = pNetlist_->getTop();
	for (int i = 0; i < numPPI_; ++i)
	{
		Cell *cell = top->getCell(i);
		int id = numGate_ - numPPI_ + i;
		circuitGates_[id].gateId_ = id;
		circuitGates_[id].cellId_ = i;
		circuitGates_[id].primitiveId_ = 0;
		circuitGates_[id].numLevel_ = circuitLvl_ - 1;
		circuitGates_[id].gateType_ = Gate::PPO;

		int did = 0;
		while (strcmp(cell->getPort(did)->name_, "D"))
		{
			++did;
		}
		Port *inp = cell->getPort(did);
		PortSet ps = top->getNetPorts(cell->getPort(inp->id_)->exNet_->id_);
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

			circuitGates_[id].faninVector_.push_back(inId);
			++circuitGates_[id].numFI_;
			circuitGates_[inId].fanoutVector_.push_back(id);
			++circuitGates_[inId].numFO_;
		}
	}
}

// **************************************************************************
// Function   [ connectMultipleTimeFrame ]
// Commenter  [ LingYunHsu ]
// Synopsis   [ usage: connect gates in different time frames,
//                     this is for multiple time frame ATPG
//              in:    void
//              out:   void
//            ]
// Date       [ LingYunHsu Ver. 1.0 started 20130818 ]
// **************************************************************************
void Circuit::connectMultipleTimeFrame()
{
	for (int i = 1; i < numFrame_; ++i)
	{
		int offset = numGate_ * i;
		if (timeFrameConnectType_ == CAPTURE)
		{
			for (int j = 0; j < numPPI_; ++j)
			{ /// give nfo and fos before numPPI_
				int gateID = offset - numPPI_ + j;
				circuitGates_[gateID].numFO_ = 1;
				circuitGates_[gateID].fanoutVector_.resize(circuitGates_[gateID].numFO_);
			}
		}
		for (int j = 0; j < numGate_; ++j)
		{ /// read gate information
			int gateID = offset + j;
			circuitGates_[gateID].gateId_ = gateID;
			circuitGates_[gateID].cellId_ = circuitGates_[j].cellId_;
			circuitGates_[gateID].numLevel_ = circuitLvl_ * i + circuitGates_[j].numLevel_;
			circuitGates_[gateID].gateType_ = circuitGates_[j].gateType_;
			circuitGates_[gateID].frame_ = i; /// record id, cid, level, type and frame
			if (circuitGates_[gateID].gateType_ != Gate::PPI && circuitGates_[gateID].gateType_ != Gate::PPO)
			{ /// if not ppi or ppo
				// add corresponding fanout
				circuitGates_[gateID].numFO_ = circuitGates_[j].numFO_;
				circuitGates_[gateID].fanoutVector_.resize(circuitGates_[j].numFO_);
				for (int k = 0; k < circuitGates_[j].numFO_; ++k)
				{
					circuitGates_[gateID].fanoutVector_[k] = circuitGates_[j].fanoutVector_[k] + offset;
				}
				// add corresponding fanin
				circuitGates_[gateID].numFI_ = circuitGates_[j].numFI_;
				circuitGates_[gateID].faninVector_.resize(circuitGates_[j].numFI_);
				for (int k = 0; k < circuitGates_[j].numFI_; ++k)
				{
					circuitGates_[gateID].faninVector_[k] = circuitGates_[j].faninVector_[k] + offset;
				}
			}
			else if (circuitGates_[gateID].gateType_ == Gate::PPI)
			{ /// if ppi
				// add corresponding fanout
				circuitGates_[gateID].numFO_ = circuitGates_[j].numFO_;
				circuitGates_[gateID].fanoutVector_.resize(circuitGates_[j].numFO_);
				for (int k = 0; k < circuitGates_[j].numFO_; ++k)
				{
					circuitGates_[gateID].fanoutVector_[k] = circuitGates_[j].fanoutVector_[k] + offset;
				}
				// add fanin for PPI
				circuitGates_[gateID].numFI_ = 1;
				circuitGates_[gateID].faninVector_.resize(circuitGates_[gateID].numFI_);
				if (timeFrameConnectType_ == CAPTURE)
				{ /// do CAPTURE
					circuitGates_[gateID].faninVector_[0] = gateID - numPI_ - numPPI_;
					circuitGates_[gateID].gateType_ = Gate::BUF;
					circuitGates_[gateID - numPI_ - numPPI_].fanoutVector_[0] = gateID;
				}
				else if (gateID != (offset + numPI_))
				{ /// do SHIFT
					circuitGates_[gateID].faninVector_[0] = gateID - numGate_ - 1;
					circuitGates_[gateID].gateType_ = Gate::BUF;
					circuitGates_[gateID - numGate_ - 1].fanoutVector_[circuitGates_[gateID - numGate_ - 1].numFO_] = gateID;
					++circuitGates_[gateID - numGate_ - 1].numFO_;
				}
				else
				{
					circuitGates_[gateID].numFI_ = 0;
				}
			}
			else
			{ /// if ppo
			  // add corresponding fanout
				circuitGates_[gateID].numFO_ = 0;
				circuitGates_[gateID].numFI_ = circuitGates_[j].numFI_;
				for (int k = 0; k < circuitGates_[j].numFI_; ++k)
				{
					circuitGates_[gateID].faninVector_[k] = circuitGates_[j].faninVector_[k] + offset;
				}
			}
		}
		// remove previous PPO after adding new time frame
		if (timeFrameConnectType_ == CAPTURE)
		{
			for (int j = 0; j < numPPI_; ++j)
			{
				int gateID = offset - numPPI_ + j;
				circuitGates_[gateID].gateType_ = Gate::BUF;
			}
		}
	}
	totalLvl_ = circuitLvl_ * numFrame_;
	totalGate_ = numGate_ * numFrame_;
}

// **************************************************************************
// Function   [ Circuit::assignFiMinLvl() ]
// Commenter  [ Jun-Han,Pan ]
// Synopsis   [ usage: Find the minimum from every gate's fan-ins' level and assign
//                     it to the gate->fiMinLvl.
//                     used for headLine justification (atpg.h)
//              in:    gate array.
//              out:   gate->minLevelOfFanins_ is updated
//            ]
// Date       [ Jun-Han,Pan Ver. 1.0 at 2013/08/18 ]
// **************************************************************************
void Circuit::assignFiMinLvl() // Need to modify this function name
{
	for (int i = 0; i < totalGate_; ++i)
	{
		Gate *gate = &circuitGates_[i];
		int minLvl = gate->numLevel_;
		for (int j = 0; j < gate->numFI_; ++j)
		{
			if (circuitGates_[gate->faninVector_[j]].numLevel_ < minLvl)
			{
				minLvl = circuitGates_[gate->faninVector_[j]].numLevel_;
				gate->minLevelOfFanins_ = gate->faninVector_[j];
				//          find the minimum among gate->fis_[j] and replace gate->fiMinLvl with it.
				//                                            **********
				//                             gate->numFI_   *        *
				//          *****************                 *  gate  *
				//          * gate->fis_[j] * --------------- *        *
				//          *****************                 **********
			}
		}
	}
}
