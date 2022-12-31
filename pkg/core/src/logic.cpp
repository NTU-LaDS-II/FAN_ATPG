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
	switch(value){
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
	// if (value == L)
	// 	out << "0";
	// else if (value == H)
	// 	out << "1";
	// else if (value == X)
	// 	out << "X";
	// else if (value == D)
	// 	out << "D";
	// else if (value == B)
	// 	out << "B";
	// else if (value == Z)
	// 	out << "Z";
	// else
	// 	out << "I";
}

void CoreNs::printValue(const ParaValue &paraValue, std::ostream &out)
{
	for (int i = 0; i < WORD_SIZE; ++i)
	{
		int bit = WORD_SIZE - i - 1;
		ParaValue mask = 0x01;
		mask <<= bit;
		if ((paraValue & mask) != PARA_L)
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

void CoreNs::printValue(const ParaValue &low, const ParaValue &high,
												std::ostream &out)
{
	for (int i = 0; i < WORD_SIZE; ++i)
	{
		int bit = WORD_SIZE - i - 1;
		ParaValue mask = 0x01;
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
