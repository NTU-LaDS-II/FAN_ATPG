// **************************************************************************
// File       [ logic.h ]
// Author     [ littleshamoo ]
// Synopsis   [ Logic representation and operation ]
// Date       [ 2010/12/14 created ]
// **************************************************************************

#ifndef _CORE_LOGIC_H_
#define _CORE_LOGIC_H_

#include <iostream>

namespace CoreNs {

// type defines
// typedef uint8_t   Value;
// typedef uintptr_t ParaValue;
typedef unsigned char Value;
typedef unsigned long ParaValue;

// constant single logic
const   Value     L         = 0;       // Low
const   Value     H         = 1;       // High
const   Value     X         = 2;       // Unknown
const   Value     D         = 3;       // D (good 1 / faulty 0)
const   Value     B         = 4;       // D-bar (good 0 / faulty 1)
const   Value     Z         = 5;       // High-impedance
const   Value     I         = 255;     // Invalid

// constant multi-bit logic
const   ParaValue PARA_L    = 0;       // all bits are zero
const   ParaValue PARA_H    = ~PARA_L; // all bits are one

// determine word size
const   size_t    BYTE_SIZE = 8;
const   size_t    WORD_SIZE = sizeof(ParaValue) * BYTE_SIZE;

inline void setBitValue(ParaValue &pv, const size_t &i, const Value &v) {
    pv = v == L ? pv & ~((ParaValue)0x01 << i) : pv | ((ParaValue)0x01 << i);
}

inline Value getBitValue(const ParaValue &pv, const size_t &i) {
    return (pv & ((ParaValue)0x01 << i)) == PARA_L ? L : H;
}

void printValue(const Value &v, std::ostream &out = std::cout);
void printValue(const ParaValue &v, std::ostream &out = std::cout);
void printValue(const ParaValue &l, const ParaValue &h,
                std::ostream &out = std::cout);

};

#endif

