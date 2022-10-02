// **************************************************************************
// File       [ mdt_test.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/02/24 created ]
// **************************************************************************

#include <cstdlib>
#include <iostream>

#include "mdt_file.h"

using namespace std;
using namespace IntfNs;


int main(int argc, char **argv) {
    extern int mdt_filewarning;
    mdt_filewarning = 1;
    if (argc < 2) {
        cerr << "**ERROR main(): please provide mentor mdt lib" << endl;
        exit(0);
    }

    MdtFile *mdt = new MdtFile;
    if (!mdt->read(argv[1], true)) {
        cerr << "**ERROR main(): mdt lib parse failed" << endl;
        exit(0);
    }

    delete mdt;

    return 0;
}

