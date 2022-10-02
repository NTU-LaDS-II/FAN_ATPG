// **************************************************************************
// File       [ logic.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/05 created ]
// **************************************************************************

#include "logic.h"

using namespace std;
using namespace CoreNs;

void CoreNs::printValue(const Value &v, ostream &out) {
    if (v == L)
        out << "0";
    else if (v == H)
        out << "1";
    else if (v == X)
        out << "X";
    else if (v == D)
        out << "D";
    else if (v == B)
        out << "B";
    else if (v == Z)
        out << "Z";
    else
        out << "I";
    out << flush;
}

void CoreNs::printValue(const ParaValue &v, ostream &out) {
    for (size_t i = 0; i < WORD_SIZE; ++i) {
        size_t j = WORD_SIZE - i - 1;
        ParaValue mask = 0x01;
        mask <<= j;
        if ((v & mask) != PARA_L)
            out << "1";
        else
            out << "0";
        out << flush;
    }
}

void CoreNs::printValue(const ParaValue &l, const ParaValue &h,
                        ostream &out) {
    for (size_t i = 0; i < WORD_SIZE; ++i) {
        size_t j = WORD_SIZE - i - 1;
        ParaValue mask = 0x01;
        mask <<= j;
        if ((l & mask) != PARA_L)
            out << "0";
        else if ((h & mask) != PARA_L)
            out << "1";
        else
            out << "X";
        out << flush;
    }
}

