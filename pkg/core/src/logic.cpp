// **************************************************************************
// File       [ logic.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/05 created ]
// **************************************************************************

#include "logic.h"

using namespace CoreNs;

void CoreNs::printValue(const Value &v, std::ostream &out)
{
	switch(v){
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
	// if (v == L)
	// 	out << "0";
	// else if (v == H)
	// 	out << "1";
	// else if (v == X)
	// 	out << "X";
	// else if (v == D)
	// 	out << "D";
	// else if (v == B)
	// 	out << "B";
	// else if (v == Z)
	// 	out << "Z";
	// else
	// 	out << "I";
}

void CoreNs::printValue(const ParaValue &v, std::ostream &out)
{
	for (int i = 0; i < WORD_SIZE; ++i)
	{
		int j = WORD_SIZE - i - 1;
		ParaValue mask = 0x01;
		mask <<= j;
		if ((v & mask) != PARA_L)
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

void CoreNs::printValue(const ParaValue &l, const ParaValue &h,
												std::ostream &out)
{
	for (int i = 0; i < WORD_SIZE; ++i)
	{
		int j = WORD_SIZE - i - 1;
		ParaValue mask = 0x01;
		mask <<= j;
		if ((l & mask) != PARA_L)
		{
			out << "0";
		}
		else if ((h & mask) != PARA_L)
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
