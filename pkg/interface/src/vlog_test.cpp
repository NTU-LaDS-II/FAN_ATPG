// **************************************************************************
// File       [ vlog_test.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ test verilog parser ]
// Date       [ 2010/12/29 created ]
// **************************************************************************

#include <cstdio>
#include <cstdlib>

#include "vlog_file.h"

using namespace IntfNs;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "**ERROR main(): please provide input verilog\n");
		exit(0);
	}

	VlogFile *vlog = new VlogFile;
	if (!vlog->read(argv[1], true))
	{
		fprintf(stderr, "**ERROR main(): verilog parser failed\n");
		exit(0);
	}

	delete vlog;
	vlog = NULL;
	return 0;
}