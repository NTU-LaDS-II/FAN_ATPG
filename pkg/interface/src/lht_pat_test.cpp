// **************************************************************************
// File       [ lht_pat_test.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/14 created ]
// **************************************************************************


#include <cstdio>
#include <cstdlib>

#include "lht_pat_file.h"

using namespace std;
using namespace IntfNs;


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "**ERROR main(): please provide input pattern\n");
        exit(0);
    }

    LhtPatFile *pat = new LhtPatFile;
    if (!pat->read(argv[1], true)) {
        fprintf(stderr, "**ERROR main(): pattern parser failed\n");
        exit(0);
    }

    delete pat;

    return 0;
}

