// **************************************************************************
// File       [ cell.h ]
// Author     [ littleshamoo ]
// Synopsis   [ define cell structure ]
// Date       [ 2011/03/23 created ]
// **************************************************************************

#ifndef _INTF_CELL_H_
#define _INTF_CELL_H_

#include <set>
#include <vector>
#include <map>

#include "global.h"

namespace IntfNs
{

	class Cell;
	class Port;
	class Net;

	typedef std::vector<Cell *> CellVec;
	typedef std::set<Cell *> CellSet;
	typedef std::map<NameKey, Cell *> CellMap;

	typedef std::vector<Port *> PortVec;
	typedef std::set<Port *> PortSet;
	typedef std::map<NameKey, Port *> PortMap;

	typedef std::vector<Net *> NetVec;
	typedef std::set<Net *> NetSet;
	typedef std::map<NameKey, Net *> NetMap;

	class Port
	{
	public:
		Port(const char *const name);
		~Port();

		enum Type
		{
			NA = -1,
			INPUT,
			OUTPUT,
			INOUT
		};

		char name_[NAME_LEN];
		Type type_;
		int id_;
		Cell *top_;
		Net *inNet_;
		Net *exNet_;
	};

	class Net
	{
	public:
		Net(const char *const name);
		~Net();

		size_t getNPort() const;
		Port *getPort(const size_t &i) const;
		bool addPort(Port *const p);
		bool removePort(const size_t &i);
		bool removePort(Port *const p);
		void clearPorts();

		size_t getNEqv() const;
		Net *getEqv(const size_t &i) const;
		bool addEqv(Net *const n);
		bool isEqv(Net *const n) const;

		char name_[NAME_LEN];
		int id_;
		Cell *top_;

	protected:
		NetVec eqvs_;
		PortVec ports_;
	};

	class Cell
	{
	public:
		Cell(const char *const name);
		virtual ~Cell();

		// internal cells
		CellVec *getCells();
		size_t getNCell() const;
		Cell *getCell(const size_t &i) const;
		Cell *getCell(const char *const name) const;
		bool addCell(Cell *const c);
		CellSet getFanout(const size_t &i) const;
		CellSet getFanout(const char *const name) const;
		CellSet getFanin(const size_t &i) const;
		CellSet getFanin(const char *const name) const;

		// internal nets
		size_t getNNet() const;
		Net *getNet(const size_t &i) const;
		Net *getNet(const char *const name) const;
		bool addNet(Net *const n);
		bool removeNet(const size_t &i);
		bool removeNet(const char *const name);
		NetSet getEqvNets(const size_t &i) const;
		NetSet getEqvNets(const char *const name) const;
		PortSet getNetPorts(const size_t &i) const;
		PortSet getNetPorts(const char *const name) const;
		CellSet getNetCells(const size_t &i) const;
		CellSet getNetCells(const char *const name) const;
		CellSet getNetCells(const size_t &i,
												const bool &input) const;
		CellSet getNetCells(const char *const name,
												const bool &input) const;

		// internal ports
		size_t getNPort() const;
		Port *getPort(const size_t &i) const;
		Port *getPort(const char *const name) const;
		bool addPort(Port *const p);
		void clearPorts();
		CellSet getPortCells(const size_t &i) const;
		CellSet getPortCells(const char *const name) const;

		char name_[NAME_LEN];
		char typeName_[NAME_LEN];
		int id_;
		int lvl_;
		Cell *top_;
		Cell *libc_;
		bool isPmt_;
		bool inLib_;

	protected:
		CellVec cells_;
		CellMap nameToCell_;
		NetVec nets_;
		NetMap nameToNet_;
		PortVec ports_;
		PortMap nameToPort_;
	};

	// Port inline methods
	inline Port::Port(const char *const name)
	{
		strcpy(name_, name);
		id_ = -1;
		type_ = NA;
		top_ = NULL;
		inNet_ = NULL;
		exNet_ = NULL;
	}

	inline Port::~Port() {}

	// Net inline methods
	inline Net::Net(const char *const name)
	{
		strcpy(name_, name);
		id_ = -1;
		top_ = NULL;
	}

	inline Net::~Net() {}

	inline size_t Net::getNPort() const
	{
		return ports_.size();
	}

	inline Port *Net::getPort(const size_t &i) const
	{
		return i < ports_.size() ? ports_[i] : NULL;
	}

	inline bool Net::addPort(Port *const p)
	{
		for (size_t i = 0; i < ports_.size(); ++i)
			if (p == ports_[i])
				return false;
		ports_.push_back(p);
		return true;
	}

	inline bool Net::removePort(const size_t &i)
	{
		if (i >= ports_.size())
			return false;
		ports_.erase(ports_.begin() + i);
		return true;
	}

	inline bool Net::removePort(Port *const p)
	{
		for (size_t i = 0; i < ports_.size(); ++i)
		{
			if (p == ports_[i])
			{
				ports_.erase(ports_.begin() + i);
				return true;
			}
		}
		return false;
	}

	inline void Net::clearPorts()
	{
		ports_.clear();
	}

	inline size_t Net::getNEqv() const
	{
		return eqvs_.size();
	}

	inline Net *Net::getEqv(const size_t &i) const
	{
		return i < eqvs_.size() ? eqvs_[i] : NULL;
	}

	inline bool Net::isEqv(Net *const n) const
	{
		for (size_t i = 0; i < eqvs_.size(); ++i)
			if (eqvs_[i] == this)
				return true;
		return false;
	}

	inline bool Net::addEqv(Net *const n)
	{
		for (size_t i = 0; i < eqvs_.size(); ++i)
			if (n == eqvs_[i])
				return false;
		eqvs_.push_back(n);
		return true;
	}
	// Cell inline methods
	inline Cell::Cell(const char *const name)
	{
		strcpy(name_, name);
		typeName_[0] = '\0';
		id_ = -1;
		lvl_ = -1;
		top_ = NULL;
		libc_ = NULL;
		isPmt_ = false;
		inLib_ = true;
	}

	inline Cell::~Cell() {}

	inline CellVec *Cell::getCells()
	{
		return &cells_;
	}

	inline size_t Cell::getNCell() const
	{
		return cells_.size();
	}

	inline Cell *Cell::getCell(const char *const name) const
	{
		CellMap::const_iterator it = nameToCell_.find(name);
		return it == nameToCell_.end() ? NULL : it->second;
	}

	inline Cell *Cell::getCell(const size_t &i) const
	{
		return i < cells_.size() ? cells_[i] : NULL;
	}

	inline bool Cell::addCell(Cell *const c)
	{
		CellMap::iterator it = nameToCell_.find(c->name_);
		if (it != nameToCell_.end())
			return false;
		cells_.push_back(c);
		nameToCell_[c->name_] = c;
		c->id_ = cells_.size() - 1;
		c->top_ = this;
		return true;
	}

	inline size_t Cell::getNNet() const
	{
		return nets_.size();
	}

	inline Net *Cell::getNet(const size_t &i) const
	{
		return i < nets_.size() ? nets_[i] : NULL;
	}

	inline Net *Cell::getNet(const char *const name) const
	{
		NetMap::const_iterator it = nameToNet_.find(name);
		return it == nameToNet_.end() ? NULL : it->second;
	}

	inline bool Cell::addNet(Net *const n)
	{
		NetMap::iterator it = nameToNet_.find(n->name_);
		if (it != nameToNet_.end())
			return false;
		nets_.push_back(n);
		nameToNet_[n->name_] = n;
		n->id_ = nets_.size() - 1;
		n->top_ = this;
		return true;
	}

	inline bool Cell::removeNet(const size_t &i)
	{
		if (i >= nets_.size())
		{
			return false;
		}
		nameToNet_.erase(nets_[i]->name_);
		nets_.erase(nets_.begin() + i);
		for (size_t idx = i; idx < nets_.size(); ++idx)
		{
			--nets_[idx]->id_;
		}
		return true;
	}

	inline bool Cell::removeNet(const char *const name)
	{
		std::map<NameKey, Net *>::iterator it = nameToNet_.find(name);
		if (it == nameToNet_.end())
		{
			return false;
		}
		return removeNet(it->second->id_);
	}

	inline size_t Cell::getNPort() const
	{
		return ports_.size();
	}

	inline Port *Cell::getPort(const size_t &i) const
	{
		return i < ports_.size() ? ports_[i] : NULL;
	}

	inline Port *Cell::getPort(const char *const name) const
	{
		PortMap::const_iterator it = nameToPort_.find(name);
		return it == nameToPort_.end() ? NULL : it->second;
	}

	inline bool Cell::addPort(Port *const p)
	{
		PortMap::iterator it = nameToPort_.find(p->name_);
		if (it != nameToPort_.end())
		{
			return false;
		}
		ports_.push_back(p);
		nameToPort_[p->name_] = p;
		p->id_ = ports_.size() - 1;
		p->top_ = this;
		return true;
	}

	inline void Cell::clearPorts()
	{
		ports_.clear();
		nameToPort_.clear();
	}

	// function, not
	inline bool cmpCellLvl(const Cell *const i, const Cell *const j)
	{
		return i->lvl_ < j->lvl_;
	}
};

#endif