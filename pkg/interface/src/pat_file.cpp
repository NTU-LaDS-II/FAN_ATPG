// **************************************************************************
// File       [ pat_file.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/13 created ]
// **************************************************************************

#include <cstdio>

#include "pat_file.h"

extern int pat_fileparse(void *);
extern FILE *pat_filein;
using namespace std;
using namespace IntfNs;

bool PatFile::read(const char * const fname, const bool &verbose) {

    verbose_ = verbose;
    pat_filein = fopen(fname, "r");
    if (!pat_filein) {
        fprintf(stderr, "**ERROR PatFile::read(): cannot open pattern file ");
        fprintf(stderr, "`%s'\n", fname);
        return false;
    }

    success_ = true;
    int res = pat_fileparse(this);
    if (res != 0) {
        fprintf(stderr, "**ERROR PatFile::read(): wrong input format\n");
        return false;
    }
    fclose(pat_filein);

    return success_;
}

void PatFile::setPiOrder(const PatNames * const pis) {
    if (verbose_) {
        printf("set PI order: ");
        PatNames *pi = pis->head;
        while (pi) {
            printf("%s", pi->name);
            pi = pi->next;
            if (pi)
                printf(", ");
        }
        printf("\n");
    }
}

void PatFile::setPpiOrder(const PatNames * const ppis) {
    if (verbose_) {
        printf("set PPI order: ");
        PatNames *ppi = ppis->head;
        while (ppi) {
            printf("%s", ppi->name);
            ppi = ppi->next;
            if (ppi)
                printf(", ");
        }
        printf("\n");
    }
}

void PatFile::setPoOrder(const PatNames * const pos) {
    if (verbose_) {
        printf("set PO order: ");
        PatNames *po = pos->head;
        while (po) {
            printf("%s", po->name);
            po = po->next;
            if (po)
                printf(", ");
        }
        printf("\n");
    }
}

void PatFile::setPatternType(const PatType &type) {
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

void PatFile::setPatternNum(const int &num) {
    if (verbose_)
        printf("set pattern num: %d\n", num);
}

void PatFile::addPattern(const char * const pi1, const char * const pi2,
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

