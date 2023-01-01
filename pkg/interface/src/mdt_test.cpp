// **************************************************************************
// File       [ mdt_test.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/02/24 created ]
// **************************************************************************

#include <cstdlib>
#include <iostream>

#include "mdt_file.h"

using namespace IntfNs;

extern int mdt_filewarning;
int main(int argc, char **argv)
{
	mdt_filewarning = 1;
	if (argc < 2)
	{
		std::cerr << "**ERROR main(): please provide mentor mdt lib"
							<< "\n";
		exit(0);
	}

	MdtFile *mdt = new MdtFile;
	if (!mdt->read(argv[1], true))
	{
		std::cerr << "**ERROR main(): mdt lib parse failed"
							<< "\n";
		exit(0);
	}

	delete mdt;
	mdt = NULL;
	return 0;
}
