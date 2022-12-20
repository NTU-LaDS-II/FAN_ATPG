// **************************************************************************
// File       [ vlog_file.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/06/24 created ]
// **************************************************************************

#include <cstdio>

#include "vlog_file.h"

extern int vlog_fileparse(void *);
extern FILE *vlog_filein;
using namespace IntfNs;

bool VlogFile::read(const char *const fname, const bool &verbose)
{

	verbose_ = verbose;
	vlog_filein = fopen(fname, "r");
	if (!vlog_filein)
	{
		fprintf(stderr, "**ERROR VlogFile::read(): cannot open netlist file");
		fprintf(stderr, "`%s'\n", fname);
		return false;
	}

	success_ = true;
	int res = vlog_fileparse(this);
	if (res != 0)
	{
		fprintf(stderr, "**ERROR VlogFile::read(): wrong input format\n");
		return false;
	}
	fclose(vlog_filein);

	return success_;
}

void VlogFile::addModule(const char *const name)
{
	if (verbose_)
	{
		printf("add module: %s\n", name);
	}
}

void VlogFile::addPorts(VlogNames *const ports)
{
	if (verbose_)
	{
		printf("add ports: ");
		VlogNames *port = ports->head;
		while (port)
		{
			printf("%s", port->name);
			port = port->next;
			if (port)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setInputNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set input nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setOutputNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set output nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setInoutNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set inout nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setWireNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set wire nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setRegNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set reg nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setSupplyLNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set supply L nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::setSupplyHNets(VlogNames *const nets)
{
	if (verbose_)
	{
		printf("set supply H nets: ");
		VlogNames *net = nets->head;
		while (net)
		{
			printf("%s", net->name);
			net = net->next;
			if (net)
			{
				printf(", ");
			}
		}
		printf("\n");
	}
}

void VlogFile::addCell(const char *const type, const char *const name,
											 VlogNames *const ports)
{
	if (verbose_)
	{
		printf("add cell: %s %s (", type, name);
		VlogNames *port = ports->head;
		while (port)
		{
			printf("%s", port->name);
			port = port->next;
			if (port)
			{
				printf(", ");
			}
		}
		printf(")\n");
	}
}

void VlogFile::addCell(const char *const type, const char *const name,
											 VlogPortToNet *const portToNet)
{
	if (verbose_)
	{
		printf("add cell: %s %s (", type, name);
		VlogPortToNet *p2n = portToNet->head;
		while (p2n)
		{
			printf(".%s(%s)", p2n->port, p2n->net);
			p2n = p2n->next;
			if (p2n)
			{
				printf(", ");
			}
		}
		printf(")\n");
	}
}

void VlogFile::addCell(const char *const type, VlogNames *const strengths,
											 const char *const name, VlogNames *const ports)
{
	if (verbose_)
	{
		printf("add cell: %s (", type);
		VlogNames *strength = strengths->head;
		while (strength)
		{
			printf("%s", strength->name);
			strength = strength->next;
			if (strength)
			{
				printf(", ");
			}
		}
		printf(") %s(", name);
		VlogNames *port = ports->head;
		while (port)
		{
			printf("%s", port->name);
			port = port->next;
			if (port)
			{
				printf(", ");
			}
		}
		printf(")\n");
	}
}

void VlogFile::addAssign(const char *const n1, const char *const n2)
{
	if (verbose_)
	{
		printf("add assign: %s = %s\n", n1, n2);
	}
}