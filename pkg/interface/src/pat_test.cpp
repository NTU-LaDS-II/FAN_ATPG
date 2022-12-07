// **************************************************************************
// File       [ pat_test.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/13 created ]
// **************************************************************************

#include <cstdio>
#include <cstdlib>

#include "pat_file.h"

using namespace std;
using namespace IntfNs;


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "**ERROR main(): please provide input pattern\n");
        exit(0);
    }

    PatFile *pat = new PatFile;
    if (!pat->read(argv[1], true)) {
        fprintf(stderr, "**ERROR main(): pattern parser failed\n");
        exit(0);
    }

    delete pat;

    return 0;
}

