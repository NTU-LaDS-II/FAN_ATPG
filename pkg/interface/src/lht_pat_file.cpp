// **************************************************************************
// File       [ lht_pat_file.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/14 created ]
// **************************************************************************

#include <cstdio>

#include "lht_pat_file.h"

extern FILE *lht_pat_filein;
extern int lht_pat_fileparse(void *);
using namespace std;
using namespace IntfNs;

bool LhtPatFile::read(const char * const fname, const bool &verbose) {

    verbose_ = verbose;
    lht_pat_filein = fopen(fname, "r");
    if (!lht_pat_filein) {
        fprintf(stderr, "**ERROR LhtPatFile::read(): cannot open pattern ");
        fprintf(stderr, "file `%s'\n", fname);
        return false;
    }

    success_ = true;
    int res = lht_pat_fileparse(this);
    if (res != 0) {
        fprintf(stderr, "**ERROR LhtPatFile::read(): wrong input format\n");
        return false;
    }
    fclose(lht_pat_filein);

    return success_;
}

void LhtPatFile::setPatternType(const PatType &type) {
    if (verbose_) {
        printf("set pattern type: ");
        switch (type) {
            case BASIC_SCAN:
                printf("BASIC_SCAN\n");
                break;
            case LAUNCH_CAPTURE:
                printf("LAUNCH_CAPTURE\n");
                break;
            case LAUNCH_SHIFT:
                printf("LAUNCH_SHIFT\n");
                break;
        }
    }
}

void LhtPatFile::addPattern(const char * const pi1, const char * const pi2,
                            const char * const ppi, const char * const si,
                            const char * const po1, const char * const po2,
                            const char * const ppo) {
    if (verbose_) {
        const char *cpi1 = pi1 ? pi1 : "\0";
        const char *cpi2 = pi2 ? pi2 : "\0";
        const char *cppi = ppi ? ppi : "\0";
        const char *csi  = si  ? si  : "\0";
        const char *cpo1 = po1 ? po1 : "\0";
        const char *cpo2 = po2 ? po2 : "\0";
        const char *cppo = ppo ? ppo : "\0";
        printf("add pattern: %s | %s | %s | %s | ", cpi1, cpi2, cppi, csi);
        printf("%s | %s | %s\n", cpo1, cpo2, cppo);
    }
}

