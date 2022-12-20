// **************************************************************************
// File       [ netlist.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ define netlist structure ]
// Date       [ 2010/12/21 created ]
// **************************************************************************

#include <algorithm>

#include "netlist.h"

using namespace IntfNs;

// info
bool Netlist::check(const bool &verbose) const
{
	if (!top_)
	{
		fprintf(stderr, "**ERROR Netlist::check(): top module not set\n");
		return false;
	}

	bool res = true;
	// check ports
	for (size_t i = 0; i < top_->getNPort(); ++i)
	{
		Port *p = top_->getPort(i);
		if (p->type_ == Port::NA)
		{
			if (verbose)
			{
				fprintf(stderr, "**WARN Netlist::check(): ");
				fprintf(stderr, "port `%s/%s' ", top_->name_, p->name_);
				fprintf(stderr, "has no type\n");
			}
		}
		if (p->top_ != top_)
		{
			res = false;
			if (verbose)
			{
				if (!p->top_)
				{
					fprintf(stderr, "**ERROR Netlist::check(): ");
					fprintf(stderr, "port `%s/%s' ", top_->name_, p->name_);
					fprintf(stderr, "has no top cell\n");
				}
				else
				{
					fprintf(stderr, "**ERROR Netlist::check(): ");
					fprintf(stderr, "port `%s/%s' ", top_->name_, p->name_);
					fprintf(stderr, "has invalic top cell ");
					fprintf(stderr, "`%s'\n", p->top_->name_);
				}
			}
		}
		Net *n = p->inNet_;
		if (!n)
		{
			if (verbose)
			{
				fprintf(stderr, "**WARN Netlist::check(): ");
				fprintf(stderr, "port `%s/%s' ", top_->name_, p->name_);
				fprintf(stderr, "unconnected\n");
			}
		}
		else if (!top_->getNet(n->name_))
		{
			res = false;
			if (verbose)
			{
				fprintf(stderr, "**ERROR Netlist::check(): ");
				fprintf(stderr, "port `%s/%s' ", top_->name_, p->name_);
				fprintf(stderr, "connected to invalid net `%s'\n", n->name_);
			}
		}
	}

	for (size_t i = 0; i < top_->getNNet(); ++i)
	{
		Net *n = top_->getNet(i);
		if (n->getNPort() == 0)
		{
			res = false;
			if (verbose)
			{
				fprintf(stderr, "**ERROR Netlist::check(): net ");
				fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
				fprintf(stderr, "is unconnected\n");
			}
		}
		if (n->getNPort() == 1 && n->getNEqv() == 0)
		{
			if (verbose)
			{
				fprintf(stderr, "**WARN Netlist::check(): net ");
				fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
				fprintf(stderr, "is floating\n");
			}
		}
		if (n->top_ != top_)
		{
			res = false;
			if (verbose)
			{
				if (!n->top_)
				{
					fprintf(stderr, "**ERROR Netlist::check(): net ");
					fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
					fprintf(stderr, "has no top cell\n");
				}
				else
				{
					fprintf(stderr, "**ERROR Netlist::check(): net ");
					fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
					fprintf(stderr, "has invalid top cell ");
					fprintf(stderr, "`%s'\n", n->top_->name_);
				}
			}
		}
		bool hasIn = false;
		bool hasOut = false;
		NetSet eqvs = n->top_->getEqvNets(n->id_);
		NetSet::iterator it = eqvs.begin();
		for (; it != eqvs.end(); ++it)
		{
			Net *eqv = (*it);
			for (size_t k = 0; k < eqv->getNPort(); ++k)
			{
				Port *p = eqv->getPort(k);
				if ((p->top_ != top_ && p->type_ == Port::OUTPUT) || (p->top_ == top_ && p->type_ == Port::INPUT))
				{
					hasIn = true;
				}
				if ((p->top_ != top_ && p->type_ == Port::INPUT) || (p->top_ == top_ && p->type_ == Port::OUTPUT))
				{
					hasOut = true;
				}
				if ((p->top_ != eqv->top_ && p->exNet_ != eqv) || (p->top_ == eqv->top_ && p->inNet_ != eqv))
				{
					res = false;
					if (verbose)
					{
						fprintf(stderr, "**ERROR Netlist::check(): net ");
						fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
						fprintf(stderr, "not connected to port ");
						fprintf(stderr, "`%s/%s'\n", top_->name_, p->name_);
					}
				}
			}
		}
		if (!hasIn)
		{
			res = false;
			if (verbose)
			{
				fprintf(stderr, "**ERROR Netlist::check(): net ");
				fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
				fprintf(stderr, "has no driver\n");
			}
		}
		if (!hasOut)
		{
			res = false;
			if (verbose)
			{
				fprintf(stderr, "**ERROR Netlist::check(): net ");
				fprintf(stderr, "`%s/%s' ", top_->name_, n->name_);
				fprintf(stderr, "drives no cell\n");
			}
		}
	}
	// check cells
	for (size_t i = 0; i < top_->getNCell(); ++i)
	{
		Cell *c = top_->getCell(i);
		if (!c->libc_ && verbose)
		{
			fprintf(stderr, "**WARN Netlist::check(): cell ");
			fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
			fprintf(stderr, "is treated as blackbox\n");
		}
		if (c->libc_)
		{
			if (lib_->hasPmt(c->libc_->id_, Pmt::TSL) || lib_->hasPmt(c->libc_->id_, Pmt::TSLI) || lib_->hasPmt(c->libc_->id_, Pmt::TSH) || lib_->hasPmt(c->libc_->id_, Pmt::TSHI))
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains tristate primitives\n");
				}
			}
			if (lib_->hasPmt(c->libc_->id_, Pmt::DELAY))
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains delay primitives\n");
				}
			}
			if (lib_->hasPmt(c->libc_->id_, Pmt::WIRE))
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains wire primitives\n");
				}
			}
			if (lib_->hasPmt(c->libc_->id_, Pmt::PULL))
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains pull primitives\n");
				}
			}
			if (lib_->hasPmt(c->libc_->id_, Pmt::UNDEF))
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains undefined primitives\n");
				}
			}
			if (lib_->hasPmt(c->libc_->id_, Pmt::NMOS) || lib_->hasPmt(c->libc_->id_, Pmt::PMOS) || lib_->hasPmt(c->libc_->id_, Pmt::RNMOS) || lib_->hasPmt(c->libc_->id_, Pmt::RPMOS) || lib_->hasPmt(c->libc_->id_, Pmt::NMOSF) || lib_->hasPmt(c->libc_->id_, Pmt::PMOSF) || lib_->hasPmt(c->libc_->id_, Pmt::CMOS1) || lib_->hasPmt(c->libc_->id_, Pmt::CMOS2) || lib_->hasPmt(c->libc_->id_, Pmt::RCMOS1) || lib_->hasPmt(c->libc_->id_, Pmt::RCMOS2) || lib_->hasPmt(c->libc_->id_, Pmt::CMOS1F) || lib_->hasPmt(c->libc_->id_, Pmt::CMOS2F))
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains MOS primitives\n");
				}
			}
			if (lib_->hasPmt(c->libc_->id_, Pmt::TIE1) || lib_->hasPmt(c->libc_->id_, Pmt::TIE0) || lib_->hasPmt(c->libc_->id_, Pmt::TIEX) || lib_->hasPmt(c->libc_->id_, Pmt::TIEZ))
			{
				if (verbose)
				{
					fprintf(stderr, "**WARN Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "contains tie cell primitives\n");
				}
			}
		}
		if (c->top_ != top_)
		{
			res = false;
			if (verbose)
			{
				if (!c->top_)
				{
					fprintf(stderr, "**ERROR Netlist::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "has no top cell\n");
				}
				else
				{
					fprintf(stderr, "**ERROR Techlib::check(): cell ");
					fprintf(stderr, "`%s/%s' ", top_->name_, c->name_);
					fprintf(stderr, "has invalid top cell ");
					fprintf(stderr, "`%s'\n", c->top_->name_);
				}
			}
		}
		for (size_t j = 0; j < c->getNPort(); ++j)
		{
			Port *p = c->getPort(j);
			if (!p->exNet_)
			{
				if (verbose)
				{
					fprintf(stderr, "**WARN Techlib::check(): port ");
					fprintf(stderr, "`%s/", top_->name_);
					fprintf(stderr, "%s/%s' ", c->name_, p->name_);
					fprintf(stderr, "unconnected\n");
				}
			}
			else if (p->exNet_->top_ != top_)
			{
				res = false;
				if (verbose)
				{
					fprintf(stderr, "**ERROR Techlib::check(): port ");
					fprintf(stderr, "`%s/", top_->name_);
					fprintf(stderr, "%s/%s' ", c->name_, p->name_);
					fprintf(stderr, "connected to invalid net ");
					fprintf(stderr, "`%s'\n", p->exNet_->name_);
				}
			}
		}
	}
	return res;
}

// operation
bool Netlist::removeFloatingNets()
{
	if (!top_)
	{
		fprintf(stderr, "**ERROR Netlist::removeFloatingNets(): ");
		fprintf(stderr, "top module not set\n");
		return false;
	}

	for (size_t i = 0; i < top_->getNNet(); ++i)
	{
		Net *n = top_->getNet(i);
		if (n->getNPort() < 2 && n->getNEqv() == 0)
		{
			if (n->getNPort() == 1)
			{
				Port *p = n->getPort(0);
				if (p->top_ == top_)
				{
					p->inNet_ = NULL;
				}
				else
				{
					p->exNet_ = NULL;
				}
			}
			top_->removeNet(i);
			delete n;
			n = NULL;
			--i;
		}
	}
	return true;
}

bool Netlist::levelize()
{
	if (!top_)
	{
		fprintf(stderr, "**ERROR Netlist::levelize(): top module not set\n");
		return false;
	}
	if (!lib_)
	{
		fprintf(stderr, "**ERROR Netlist::levelize(): technology library ");
		fprintf(stderr, "not set\n");
		return false;
	}

	bool processed[top_->getNCell()];
	bool levelized[top_->getNCell()];
	for (size_t i = 0; i < top_->getNCell(); ++i)
	{
		processed[i] = false;
		levelized[i] = false;
	}

	std::queue<Cell *> cque;

	// process PPIs
	for (size_t i = 0; i < top_->getNCell(); ++i)
	{
		Cell *c = top_->getCell(i);
		if (!lib_->hasPmt(c->libc_->id_, Pmt::DFF))
		{
			continue;
		}
		c->lvl_ = 0;
		processed[i] = true;
		levelized[i] = true;
		CellSet cells = top_->getFanout(i);
		for (CellSet::iterator it = cells.begin(); it != cells.end(); ++it)
		{
			if (processed[(*it)->id_] || lib_->hasPmt((*it)->libc_->id_, Pmt::DFF))
			{
				continue;
			}
			processed[(*it)->id_] = true;
			cque.push((*it));
		}
	}

	// process cells with no inputs
	for (size_t i = 0; i < top_->getNCell(); ++i)
	{
		Cell *c = top_->getCell(i);
		bool hasInput = false;
		for (size_t j = 0; j < c->getNPort(); ++j)
		{
			if (c->getPort(j)->type_ == Port::INPUT)
			{
				hasInput = true;
				break;
			}
		}
		if (hasInput)
		{
			continue;
		}
		c->lvl_ = 0;
		processed[i] = true;
		levelized[i] = true;
		CellSet cells = top_->getFanout(i);
		for (CellSet::iterator it = cells.begin(); it != cells.end(); ++it)
		{
			if (processed[(*it)->id_])
			{
				continue;
			}
			processed[(*it)->id_] = true;
			cque.push((*it));
		}
	}

	// process PIs Modified by NE
	for (size_t i = 0; i < top_->getNPort(); ++i)
	{
		Port *p = top_->getPort(i);
		if (p->type_ != Port::INPUT)
		{
			continue;
		}
		CellSet cells = top_->getPortCells(i);
		for (CellSet::iterator it = cells.begin(); it != cells.end(); ++it)
		{
			if (processed[(*it)->id_])
			{
				continue;
			}
			processed[(*it)->id_] = true;
			cque.push((*it));
		}
	}

	while (!cque.empty())
	{
		Cell *c = cque.front();
		cque.pop();

		// check if cell is ready to process
		int maxLvl = 0;
		bool ready = true;
		CellSet fis = top_->getFanin(c->id_);
		for (CellSet::iterator it = fis.begin(); it != fis.end(); ++it)
		{
			if (!levelized[(*it)->id_])
			{
				ready = false;
				break;
			}
			if ((*it)->lvl_ > maxLvl)
			{
				maxLvl = (*it)->lvl_;
			}
		}
		if (!ready)
		{
			cque.push(c);
			continue;
		}

		// set cell level
		c->lvl_ = maxLvl + 1;
		levelized[c->id_] = true;

		// put fanout into the queue
		CellSet fos = top_->getFanout(c->id_);
		for (CellSet::iterator it = fos.begin(); it != fos.end(); ++it)
		{
			if (processed[(*it)->id_])
			{
				continue;
			}
			cque.push((*it));
			processed[(*it)->id_] = true;
		}
	}

	// sort the vector
	stable_sort(top_->getCells()->begin(), top_->getCells()->end(),
							cmpCellLvl);

	// reassign ID
	for (size_t i = 0; i < top_->getNCell(); ++i)
	{
		top_->getCell(i)->id_ = i;
	}

	return true;
}
