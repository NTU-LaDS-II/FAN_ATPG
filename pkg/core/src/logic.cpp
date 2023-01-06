// **************************************************************************
// File       [ logic.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/05 created ]
// **************************************************************************

#include "logic.h"

using namespace CoreNs;

void CoreNs::printValue(const Value &value, std::ostream &out)
{
	switch (value)
	{
		case L:
			out << "0";
			break;
		case H:
			out << "1";
			break;
		case X:
			out << "X";
			break;
		case D:
			out << "D";
			break;
		case B:
			out << "B";
			break;
		case Z:
			out << "Z";
			break;
		default:
			out << "I";
			break;
	}
	out << std::flush;
}

void CoreNs::printParallelValue(const ParallelValue &parallelValue, std::ostream &out)
{
	for (int bit = WORD_SIZE - 1; bit >= 0; --bit)
	{
		ParallelValue mask = 0x01;
		mask <<= bit;
		if ((parallelValue & mask) != PARA_L)
		{
			out << "1";
		}
		else
		{
			out << "0";
		}
		out << std::flush;
	}
}

void CoreNs::printSimulationValue(const ParallelValue &low, const ParallelValue &high,
												std::ostream &out)
{
	for (int bit = WORD_SIZE - 1; bit >= 0; --bit)
	{
		ParallelValue mask = 0x01;
		mask <<= bit;
		if ((low & mask) != PARA_L)
		{
			out << "0";
		}
		else if ((high & mask) != PARA_L)
		{
			out << "1";
		}
		else
		{
			out << "X";
		}
		out << std::flush;
	}
}
