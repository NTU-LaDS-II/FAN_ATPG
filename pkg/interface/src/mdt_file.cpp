// **************************************************************************
// File       [ mdt_file.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/06/23 created ]
// **************************************************************************

#include <cstdio>

#include "mdt_file.h"

extern int mdt_fileparse(void *);
extern int mdt_filewarning;
extern FILE *mdt_filein;
using namespace IntfNs;

bool MdtFile::read(const char *const fname, const bool &verbose)
{

	verbose_ = verbose;

	if (verbose_)
	{
		mdt_filewarning = 1;
	}
	else
	{
		mdt_filewarning = 0;
	}

	mdt_filein = fopen(fname, "r");
	if (!mdt_filein)
	{
		fprintf(stderr, "**ERROR MdtFile::read(): cannot open netlist file");
		fprintf(stderr, "`%s'\n", fname);
		return false;
	}

	success_ = true;
	int res = mdt_fileparse(this);
	if (res != 0)
	{
		fprintf(stderr, "**ERROR MdtFile::read(): wrong input format\n");
		return false;
	}
	fclose(mdt_filein);

	return success_;
}

void MdtFile::addModel(const char *const name)
{
	if (verbose_)
	{
		printf("add model: %s\n", name);
	}
}

void MdtFile::addPorts(MdtNames *const ports)
{
	if (verbose_)
	{
		printf("add ports: ");
		MdtNames *port = ports->head;
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

void MdtFile::setInputNets(MdtNames *const nets)
{
	if (verbose_)
	{
		printf("set input nets: ");
		MdtNames *net = nets->head;
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

void MdtFile::setOutputNets(MdtNames *const nets)
{
	if (verbose_)
	{
		printf("set output nets: ");
		MdtNames *net = nets->head;
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

void MdtFile::setInoutNets(MdtNames *const nets)
{
	if (verbose_)
	{
		printf("set inout nets: ");
		MdtNames *net = nets->head;
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

void MdtFile::setInternNets(MdtNames *const nets)
{
	if (verbose_)
	{
		printf("set intern nets: ");
		MdtNames *net = nets->head;
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

void MdtFile::addPrimitive(const char *const type, const char *const name,
													 MdtNames *const ports)
{
	if (verbose_)
	{
		printf("add primitive: %s %s (", type, name);
		MdtNames *port = ports->head;
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

void MdtFile::addInstance(const char *const type, const char *const name,
													MdtNames *const ports)
{
	if (verbose_)
	{
		printf("add instance: %s %s (", type, name);
		MdtNames *port = ports->head;
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

void MdtFile::addInstance(const char *const type, const char *const name,
													MdtPortToNet *const portToNet)
{
	if (verbose_)
	{
		printf("add instance: %s %s (", type, name);
		MdtPortToNet *p2n = portToNet->head;
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