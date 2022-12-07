// **************************************************************************
// File       [ sdf_test.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/19 created ]
// **************************************************************************

#include <cstdio>
#include <cstdlib>

#include "sdf_file.h"

using namespace std;
using namespace IntfNs;


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "**ERROR main(): please provide input verilog\n");
        exit(0);
    }

    SdfFile *sdf = new SdfFile;
    if (!sdf->read(argv[1], true)) {
        fprintf(stderr, "**ERROR main(): SDF parser failed\n");
        exit(0);
    }

    delete sdf;

    return 0;
}

