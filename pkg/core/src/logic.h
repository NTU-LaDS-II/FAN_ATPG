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
	typedef unsigned char Value;				 // typedef uint8_t   Value;
	typedef unsigned long ParallelValue; // typedef uintptr_t ParallelValue;

	// constant single logic
	constexpr Value L = 0;	 // Low
	constexpr Value H = 1;	 // High
	constexpr Value X = 2;	 // Unknown
	constexpr Value D = 3;	 // D (good 1 / faulty 0)
	constexpr Value B = 4;	 // D-bar (good 0 / faulty 1)
	constexpr Value Z = 5;	 // High-impedance
	constexpr Value I = 255; // Invalid

	// constant multi-bit logic
	constexpr ParallelValue PARA_L = 0;				// all bits are zero
	constexpr ParallelValue PARA_H = ~PARA_L; // all bits are one

	// determine word size
	constexpr int BYTE_SIZE = 8;
	constexpr int WORD_SIZE = sizeof(ParallelValue) * BYTE_SIZE;
	inline void setBitValue(ParallelValue &parallelValue, const size_t &bit, const Value &value)
	{
		parallelValue = value == L ? parallelValue & ~((ParallelValue)0x01 << bit) : parallelValue | ((ParallelValue)0x01 << bit);
	}

	inline Value getBitValue(const ParallelValue &parallelValue, const size_t &bit)
	{
		return (parallelValue & ((ParallelValue)0x01 << bit)) == PARA_L ? L : H;
	}

	void printValue(const Value &value, std::ostream &out = std::cout);
	void printParallelValue(const ParallelValue &parallelValue, std::ostream &out = std::cout);
	void printSimulationValue(const ParallelValue &low, const ParallelValue &high, std::ostream &out = std::cout);

};

#endif