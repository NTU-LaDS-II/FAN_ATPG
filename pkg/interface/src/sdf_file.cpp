// **************************************************************************
// File       [ sdf_file.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/19 created ]
// **************************************************************************

#include <cstdio>

#include "sdf_file.h"

extern int sdf_fileparse(void *);
extern FILE *sdf_filein;
using namespace IntfNs;

bool SdfFile::read(const char *const fname, const bool &verbose)
{

	verbose_ = verbose;
	sdf_filein = fopen(fname, "r");
	if (!sdf_filein)
	{
		fprintf(stderr, "**ERROR SdfFile::read(): cannot open netlist file");
		fprintf(stderr, "`%s'\n", fname);
		return false;
	}

	success_ = true;
	int res = sdf_fileparse(this);
	if (res != 0)
	{
		fprintf(stderr, "**ERROR SdfFile::read(): wrong input format\n");
		return false;
	}
	fclose(sdf_filein);

	return success_;
}

bool SdfFile::addVersion(const char *const version)
{
	if (verbose_)
	{
		printf("add version: %s\n", version);
	}
	return true;
}

bool SdfFile::addDesign(const char *const design)
{
	if (verbose_)
	{
		printf("add design: %s\n", design);
	}
	return true;
}

bool SdfFile::addDate(const char *const date)
{
	if (verbose_)
	{
		printf("add date: %s\n", date);
	}
	return true;
}

bool SdfFile::addVendor(const char *const vendor)
{
	if (verbose_)
	{
		printf("add vendor: %s\n", vendor);
	}
	return true;
}

bool SdfFile::addProgName(const char *const name)
{
	if (verbose_)
	{
		printf("add program name: %s\n", name);
	}
	return true;
}

bool SdfFile::addProgVersion(const char *const version)
{
	if (verbose_)
	{
		printf("add program version: %s\n", version);
	}
	return true;
}

bool SdfFile::addHierChar(const char &hier)
{
	if (verbose_)
	{
		printf("add hier char: %c\n", hier);
	}
	return true;
}

bool SdfFile::addVoltage(const SdfValue &volt)
{
	if (verbose_)
	{
		printf("add voltages:");
		for (int i = 0; i < volt.n; ++i)
		{
			printf(" %f", volt.v[i]);
		}
		printf("\n");
	}
	return true;
}

bool SdfFile::addProcess(const char *const proc)
{
	if (verbose_)
	{
		printf("add process: %s\n", proc);
	}
	return true;
}

bool SdfFile::addTemperature(const SdfValue &temp)
{
	if (verbose_)
	{
		printf("add temperature:");
		for (int i = 0; i < temp.n; ++i)
		{
			printf(" %f", temp.v[i]);
		}
		printf("\n");
	}
	return true;
}

bool SdfFile::addTimeScale(const float &num, const char *const unit)
{
	if (verbose_)
	{
		printf("add time scale: %f %s\n", num, unit);
	}
	return true;
}

bool SdfFile::addCell(const char *const type, const char *const name)
{
	if (verbose_)
	{
		printf("add cell: %s %s\n", type, name);
	}
	return true;
}

bool SdfFile::addIoDelay(const SdfDelayType &type, const SdfPortSpec &spec,
												 const char *const port, const SdfDelayValueList &v)
{
	if (verbose_)
	{
		printf("add io delay: ");
		if (type == DELAY_ABSOLUTE)
		{
			printf("ABSOLUTE ");
		}
		else
		{
			printf("INCREMENT ");
		}

		switch (spec.type)
		{
			case SdfPortSpec::EDGE_NA:
				break;
			case SdfPortSpec::EDGE_01:
				printf("edge 01 ");
				break;
			case SdfPortSpec::EDGE_10:
				printf("edge 10 ");
				break;
			case SdfPortSpec::EDGE_0Z:
				printf("edge 0Z ");
				break;
			case SdfPortSpec::EDGE_Z1:
				printf("edge Z1 ");
				break;
			case SdfPortSpec::EDGE_1Z:
				printf("edge 1Z ");
				break;
			case SdfPortSpec::EDGE_Z0:
				printf("edge Z0 ");
				break;
		}
		printf("%s ", spec.port);
		printf("%s ", port);
		for (int i = 0; i < v.n; ++i)
		{
			printf("{");
			for (int j = 0; j < v.v[i].n; ++j)
			{
				printf("(");
				for (int k = 0; k < v.v[i].v[j].n; ++k)
				{
					printf("%f ", v.v[i].v[j].v[k]);
				}
				printf(") ");
			}
			printf("} ");
		}
		printf("\n");
	}
	return true;
}

bool SdfFile::addIoRetain(const SdfDelayType &type, const SdfPortSpec &spec,
													const char *const port, const SdfDelayValueList &v)
{
	if (verbose_)
	{
		printf("add io retain: ");
		if (type == DELAY_ABSOLUTE)
		{
			printf("ABSOLUTE ");
		}
		else
		{
			printf("INCREMENT ");
		}

		switch (spec.type)
		{
			case SdfPortSpec::EDGE_NA:
				break;
			case SdfPortSpec::EDGE_01:
				printf("edge 01 ");
				break;
			case SdfPortSpec::EDGE_10:
				printf("edge 10 ");
				break;
			case SdfPortSpec::EDGE_0Z:
				printf("edge 0Z ");
				break;
			case SdfPortSpec::EDGE_Z1:
				printf("edge Z1 ");
				break;
			case SdfPortSpec::EDGE_1Z:
				printf("edge 1Z ");
				break;
			case SdfPortSpec::EDGE_Z0:
				printf("edge Z0 ");
				break;
		}
		printf("%s ", spec.port);
		printf("%s ", port);
		for (int i = 0; i < v.n; ++i)
		{
			printf("{");
			for (int j = 0; j < v.v[i].n; ++j)
			{
				printf("(");
				for (int k = 0; k < v.v[i].v[j].n; ++k)
				{
					printf("%f ", v.v[i].v[j].v[k]);
				}
				printf(") ");
			}
			printf("} ");
		}
		printf("\n");
	}
	return true;
}

bool SdfFile::addPortDelay(const SdfDelayType &type, const char *const port,
													 const SdfDelayValueList &v)
{
	if (verbose_)
	{
		printf("add port delay: ");
		if (type == DELAY_ABSOLUTE)
		{
			printf("ABSOLUTE ");
		}
		else
		{
			printf("INCREMENT ");
		}
		printf("%s ", port);
		for (int i = 0; i < v.n; ++i)
		{
			printf("{");
			for (int j = 0; j < v.v[i].n; ++j)
			{
				printf("(");
				for (int k = 0; k < v.v[i].v[j].n; ++k)
				{
					printf("%f ", v.v[i].v[j].v[k]);
				}
				printf(") ");
			}
			printf("} ");
		}
		printf("\n");
	}
	return true;
}

bool SdfFile::addInterconnectDelay(const SdfDelayType &type,
																	 const char *const from,
																	 const char *const to,
																	 const SdfDelayValueList &v)
{
	if (verbose_)
	{
		printf("add interconnect delay: ");
		if (type == DELAY_ABSOLUTE)
		{
			printf("ABSOLUTE ");
		}
		else
		{
			printf("INCREMENT ");
		}
		printf("%s to %s ", from, to);
		for (int i = 0; i < v.n; ++i)
		{
			printf("{");
			for (int j = 0; j < v.v[i].n; ++j)
			{
				printf("(");
				for (int k = 0; k < v.v[i].v[j].n; ++k)
				{
					printf("%f ", v.v[i].v[j].v[k]);
				}
				printf(") ");
			}
			printf("} ");
		}
		printf("\n");
	}
	return true;
}

bool SdfFile::addDeviceDelay(const SdfDelayType &type, const char *const dev,
														 const SdfDelayValueList &v)
{
	if (verbose_)
	{
		printf("add device delay: ");
		if (type == DELAY_ABSOLUTE)
		{
			printf("ABSOLUTE ");
		}
		else
		{
			printf("INCREMENT ");
		}
		printf("%s ", dev);
		for (int i = 0; i < v.n; ++i)
		{
			printf("{");
			for (int j = 0; j < v.v[i].n; ++j)
			{
				printf("(");
				for (int k = 0; k < v.v[i].v[j].n; ++k)
				{
					printf("%f ", v.v[i].v[j].v[k]);
				}
				printf(") ");
			}
			printf("} ");
		}
		printf("\n");
	}
	return true;
}