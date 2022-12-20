// **************************************************************************
// File       [ netlist_builder.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/03/01 created ]
// **************************************************************************

#include <cstdio>

#include "netlist_builder.h"

using namespace IntfNs;

void VlogNlBuilder::addModule(const char *const name)
{
	if (!success_)
	{
		return;
	}
	if (!nl_->getTechlib())
	{
		if (verbose_)
		{
			fprintf(stderr, "**ERROR VlogNlBuilder::addModule(): ");
			fprintf(stderr, "technology library not set\n");
		}
		success_ = false;
		return;
	}
	if (nl_->getModule(name))
	{
		if (verbose_)
		{
			fprintf(stderr, "**ERROR VlogNlBuilder::addModule(): ");
			fprintf(stderr, "module `%s' already exists\n", name);
		}
		success_ = false;
		return;
	}
	curModule_ = new Cell(name);
	nl_->addModule(curModule_);
	nl_->setTop(name);
}

void VlogNlBuilder::addPorts(VlogNames *const ports)
{
	if (!success_)
	{
		return;
	}
	VlogNames *port = ports->head;
	while (port)
	{
		if (curModule_->getPort(port->name))
		{
			if (verbose_)
			{
				fprintf(stderr, "**ERROR VlogNlBuilder::addPorts(): port ");
				fprintf(stderr, "`%s/%s' ", curModule_->name_, port->name);
				fprintf(stderr, "already exists\n");
			}
			success_ = false;
			return;
		}
		Port *p = new Port(port->name);
		p->top_ = curModule_;
		curModule_->addPort(p);
		port = port->next;
	}
}

void VlogNlBuilder::setInputNets(VlogNames *const nets)
{
	if (!success_)
	{
		return;
	}
	if (!addPortNets(nets->head))
	{
		success_ = false;
		return;
	}
	VlogNames *net = nets->head;
	while (net)
	{
		Port *p = curModule_->getPort(net->name);
		p->type_ = Port::INPUT;
		net = net->next;
	}
}

void VlogNlBuilder::setOutputNets(VlogNames *const nets)
{
	if (!success_)
	{
		return;
	}
	if (!addPortNets(nets->head))
	{
		success_ = false;
		return;
	}
	VlogNames *net = nets->head;
	while (net)
	{
		Port *p = curModule_->getPort(net->name);
		p->type_ = Port::OUTPUT;
		net = net->next;
	}
}

void VlogNlBuilder::setInoutNets(VlogNames *const nets)
{
	if (!success_)
	{
		return;
	}
	if (!addPortNets(nets->head))
	{
		success_ = false;
		return;
	}
	VlogNames *net = nets->head;
	while (net)
	{
		Port *p = curModule_->getPort(net->name);
		p->type_ = Port::INOUT;
		net = net->next;
	}
}

void VlogNlBuilder::setWireNets(VlogNames *const nets)
{
	if (!success_)
	{
		return;
	}
	VlogNames *net = nets->head;
	while (net)
	{
		if (!curModule_->getNet(net->name))
		{
			Net *n = new Net(net->name);
			curModule_->addNet(n);
		}
		net = net->next;
	}
}

void VlogNlBuilder::addCell(const char *const type, const char *const name,
														VlogPortToNet *const portToNet)
{
	if (!success_)
		return;
	char cname[NAME_LEN];
	if (!strcmp(name, ""))
		sprintf(cname, "LOO%d", (int)curModule_->getNCell());
	else
		strcpy(cname, name);
	if (curModule_->getCell(cname))
	{
		if (verbose_)
		{
			fprintf(stderr, "**ERROR VlogNlBuilder::addCell(): cell ");
			fprintf(stderr, "`%s/%s'\n", curModule_->name_, cname);
		}
		success_ = false;
		return;
	}
	Cell *c = new Cell(cname);
	c->top_ = curModule_;
	strcpy(c->typeName_, type);
	bool hasLib = nl_->getTechlib() && nl_->getTechlib()->getCell(type);
	if (hasLib)
	{
		Cell *libCell = nl_->getTechlib()->getCell(type);
		c->libc_ = libCell;
		for (size_t i = 0; i < libCell->getNPort(); ++i)
		{
			Port *p = new Port(libCell->getPort(i)->name_);
			p->top_ = c;
			p->type_ = libCell->getPort(i)->type_;
			c->addPort(p);
		}
	}
	else
	{
		if (verbose_)
		{
			fprintf(stderr, "**WARN VlogNlBuilder::addCell(): cell ");
			fprintf(stderr, "`%s/%s' ", curModule_->name_, cname);
			fprintf(stderr, "set as black box\n");
		}
	}
	VlogPortToNet *p2n = portToNet->head;
	while (p2n)
	{
		Net *n = curModule_->getNet(p2n->net);
		if (!n)
		{
			if (verbose_)
			{
				fprintf(stderr, "**WARN VlogNlBuilder::addCell(): net ");
				fprintf(stderr, "`%s/%s' ", curModule_->name_, p2n->net);
				fprintf(stderr, "set as wire\n");
			}
			n = new Net(p2n->net);
			n->top_ = curModule_;
			curModule_->addNet(n);
		}
		Port *p;
		if (hasLib)
		{
			p = c->getPort(p2n->port);
			if (!p)
			{
				if (verbose_)
				{
					fprintf(stderr, "**ERROR VlogNlBuilder::addCell(): port");
					fprintf(stderr, " `%s/ ", curModule_->name_);
					fprintf(stderr, "%s' ", p2n->port);
					fprintf(stderr, "does not match library cell\n");
				}
				success_ = false;
				return;
			}
		}
		else
		{
			p = new Port(p2n->port);
			p->top_ = c;
			c->addPort(p);
		}
		p->exNet_ = n;
		n->addPort(p);

		p2n = p2n->next;
	}

	curModule_->addCell(c);
}

void VlogNlBuilder::addAssign(const char *const n1, const char *const n2)
{
	if (!success_)
	{
		return;
	}
	Net *net1 = curModule_->getNet(n1);
	if (!net1)
	{
		if (verbose_)
		{
			fprintf(stderr, "**WARN VlogNlBuilder::addAssign(): net ");
			fprintf(stderr, "`%s/%s' ", curModule_->name_, n1);
			fprintf(stderr, "set as wire\n");
		}
		net1 = new Net(n1);
		net1->top_ = curModule_;
		curModule_->addNet(net1);
	}
	Net *net2 = curModule_->getNet(n2);
	if (!net2)
	{
		if (verbose_)
		{
			fprintf(stderr, "**WARN VlogNlBuilder::addAssign(): net ");
			fprintf(stderr, "`%s/%s' ", curModule_->name_, n2);
			fprintf(stderr, "set as wire\n");
		}
		net2 = new Net(n2);
		net2->top_ = curModule_;
		curModule_->addNet(net2);
	}
	net1->addEqv(net2);
	net2->addEqv(net1);
}

bool VlogNlBuilder::addPortNets(VlogNames *const nets)
{
	VlogNames *net = nets->head;
	while (net)
	{
		Port *p = curModule_->getPort(net->name);
		if (!p)
		{
			if (verbose_)
			{
				fprintf(stderr, "**ERROR MdtLibBuilder::addPortNets(): port");
				fprintf(stderr, " `%s/%s' ", curModule_->name_, net->name);
				fprintf(stderr, "does not exist\n");
			}
			return false;
		}
		Net *n = curModule_->getNet(net->name);
		if (!n)
		{
			n = new Net(net->name);
			n->top_ = curModule_;
			curModule_->addNet(n);
		}
		n->addPort(p);
		p->inNet_ = n;
		net = net->next;
	}
	return true;
}

void VlogNlBuilder::setRegNets(VlogNames *const nets) {}
void VlogNlBuilder::setSupplyLNets(VlogNames *const nets) {}
void VlogNlBuilder::setSupplyHNets(VlogNames *const nets) {}
void VlogNlBuilder::addCell(const char *const type, const char *const name,
														VlogNames *const ports) {}
void VlogNlBuilder::addCell(const char *const type,
														VlogNames *const strengths,
														const char *const name,
														VlogNames *const ports) {}
