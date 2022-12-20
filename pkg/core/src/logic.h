// **************************************************************************
// File       [ logic.h ]
// Author     [ littleshamoo ]
// Synopsis   [ Logic representation and operation ]
// Date       [ 2010/12/14 created ]
// **************************************************************************

#ifndef _CORE_LOGIC_H_
#define _CORE_LOGIC_H_

#include <iostream>

namespace CoreNs
{

	// type defines
	typedef unsigned char Value;		 // typedef uint8_t   Value;
	typedef unsigned long ParaValue; // typedef uintptr_t ParaValue;

	// constant single logic
	constexpr Value L = 0;	 // Low
	constexpr Value H = 1;	 // High
	constexpr Value X = 2;	 // Unknown
	constexpr Value D = 3;	 // D (good 1 / faulty 0)
	constexpr Value B = 4;	 // D-bar (good 0 / faulty 1)
	constexpr Value Z = 5;	 // High-impedance
	constexpr Value I = 255; // Invalid

	// constant multi-bit logic
	constexpr ParaValue PARA_L = 0;				// all bits are zero
	constexpr ParaValue PARA_H = ~PARA_L; // all bits are one

	// determine word size
	constexpr int BYTE_SIZE = 8;
	constexpr int WORD_SIZE = sizeof(ParaValue) * BYTE_SIZE;

	inline void setBitValue(ParaValue &pv, const size_t &i, const Value &v)
	{
		pv = v == L ? pv & ~((ParaValue)0x01 << i) : pv | ((ParaValue)0x01 << i);
	}

	inline Value getBitValue(const ParaValue &pv, const size_t &i)
	{
		return (pv & ((ParaValue)0x01 << i)) == PARA_L ? L : H;
	}

	void printValue(const Value &v, std::ostream &out = std::cout);
	void printValue(const ParaValue &v, std::ostream &out = std::cout);
	void printValue(const ParaValue &l, const ParaValue &h, std::ostream &out = std::cout);

};

#endif