// **************************************************************************
// File       [ cell.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ define cell structure ]
// Date       [ 2011/03/23 created ]
// **************************************************************************

#include <queue>

#include "cell.h"

using namespace IntfNs;

CellSet Cell::getFanout(const size_t &i) const
{
	CellSet fo;
	if (i >= cells_.size())
	{
		return fo;
	}
	Cell *c = cells_[i];
	NetSet eqvs;
	for (size_t i = 0; i < c->getNPort(); ++i)
	{
		if (c->getPort(i)->type_ != Port::OUTPUT || !c->getPort(i)->exNet_)
		{
			continue;
		}
		NetSet eqv = getEqvNets(c->getPort(i)->exNet_->id_);
		eqvs.insert(eqv.begin(), eqv.end());
	}
	NetSet::iterator it = eqvs.begin();
	for (; it != eqvs.end(); ++it)
	{
		Net *n = *it;
		for (size_t j = 0; j < n->getNPort(); ++j)
		{
			Port *p = n->getPort(j);
			if (p->top_ != this && p->type_ == Port::INPUT)
			{
				fo.insert(p->top_);
			}
		}
	}
	return fo;
}

CellSet Cell::getFanout(const char *const name) const
{
	Cell *c = getCell(name);
	if (!c)
	{
		CellSet fo;
		return fo;
	}
	return (getFanout(c->id_));
}

CellSet Cell::getFanin(const size_t &i) const
{
	CellSet fi;
	if (i >= cells_.size())
	{
		return fi;
	}
	Cell *c = cells_[i];
	NetSet eqvs;
	for (size_t i = 0; i < c->getNPort(); ++i)
	{
		if (c->getPort(i)->type_ != Port::INPUT || !c->getPort(i)->exNet_)
		{
			continue;
		}
		NetSet eqv = getEqvNets(c->getPort(i)->exNet_->id_);
		eqvs.insert(eqv.begin(), eqv.end());
	}
	NetSet::iterator it = eqvs.begin();
	for (; it != eqvs.end(); ++it)
	{
		Net *n = *it;
		for (size_t j = 0; j < n->getNPort(); ++j)
		{
			Port *p = n->getPort(j);
			if (p->top_ != this && p->type_ == Port::OUTPUT)
			{
				fi.insert(p->top_);
			}
		}
	}
	return fi;
}

CellSet Cell::getFanin(const char *const name) const
{
	Cell *c = getCell(name);
	if (!c)
	{
		CellSet fi;
		return fi;
	}
	return (getFanin(c->id_));
}

NetSet Cell::getEqvNets(const size_t &i) const
{
	NetSet eqvs;
	if (i >= nets_.size())
	{
		return eqvs;
	}
	eqvs.insert(nets_[i]);
	std::queue<Net *> nQue;
	nQue.push(nets_[i]);
	while (!nQue.empty())
	{
		Net *n = nQue.front();
		nQue.pop();
		for (size_t idx = 0; idx < n->getNEqv(); ++idx)
		{
			Net *eqv = n->getEqv(idx);
			if (eqvs.count(eqv) == 0)
			{
				eqvs.insert(eqv);
				nQue.push(eqv);
			}
		}
	}
	return eqvs;
}

NetSet Cell::getEqvNets(const char *const name) const
{
	Net *n = getNet(name);
	if (!n)
	{
		NetSet eqvs;
		return eqvs;
	}
	return (getEqvNets(n->id_));
}

PortSet Cell::getNetPorts(const size_t &i) const
{
	PortSet ports;
	if (i >= nets_.size())
	{
		return ports;
	}
	NetSet eqv = getEqvNets(i);
	for (NetSet::iterator it = eqv.begin(); it != eqv.end(); ++it)
	{
		for (size_t i = 0; i < (*it)->getNPort(); ++i)
		{
			ports.insert((*it)->getPort(i));
		}
	}
	return ports;
}

PortSet Cell::getNetPorts(const char *const name) const
{
	Net *n = getNet(name);
	if (!n)
	{
		PortSet ports;
		return ports;
	}
	return (getNetPorts(n->id_));
}

CellSet Cell::getNetCells(const size_t &i) const
{
	CellSet cells;
	if (i >= nets_.size())
	{
		return cells;
	}
	NetSet eqv = getEqvNets(i);
	for (NetSet::iterator it = eqv.begin(); it != eqv.end(); ++it)
	{
		for (size_t i = 0; i < (*it)->getNPort(); ++i)
			if ((*it)->getPort(i)->top_ != this)
			{
				cells.insert((*it)->getPort(i)->top_);
			}
	}
	return cells;
}

CellSet Cell::getNetCells(const char *const name) const
{
	Net *n = getNet(name);
	if (!n)
	{
		CellSet cells;
		return cells;
	}
	return (getNetCells(n->id_));
}

CellSet Cell::getNetCells(const size_t &i, const bool &input) const
{
	CellSet cells;
	if (i >= nets_.size())
	{
		return cells;
	}
	NetSet eqv = getEqvNets(i);
	for (NetSet::iterator it = eqv.begin(); it != eqv.end(); ++it)
	{
		for (size_t i = 0; i < (*it)->getNPort(); ++i)
		{
			Port *p = (*it)->getPort(i);
			if (p->top_ != this && input && p->type_ == Port::INPUT)
			{
				cells.insert(p->top_);
			}
			else if (p->top_ != this && !input && p->type_ == Port::OUTPUT)
			{
				cells.insert(p->top_);
			}
		}
	}
	return cells;
}

CellSet Cell::getNetCells(const char *const name, const bool &input) const
{
	Net *n = getNet(name);
	if (!n)
	{
		CellSet cells;
		return cells;
	}
	return (getNetCells(n->id_, input));
}

CellSet Cell::getPortCells(const size_t &i) const
{
	CellSet cells;
	if (i >= ports_.size())
	{
		return cells;
	}
	Net *n = ports_[i]->inNet_;
	if (!n)
	{
		return cells;
	}
	NetSet eqv = getEqvNets(n->id_);
	for (NetSet::iterator it = eqv.begin(); it != eqv.end(); ++it)
	{
		Net *n = *it;
		for (size_t i = 0; i < n->getNPort(); ++i)
		{
			Port *p = n->getPort(i);
			if (p->top_ != this)
			{
				cells.insert(p->top_);
			}
		}
	}
	return cells;
}

CellSet Cell::getPortCells(const char *const name) const
{
	Port *p = getPort(name);
	if (!p)
	{
		CellSet cells;
		return cells;
	}
	return (getPortCells(p->id_));
}