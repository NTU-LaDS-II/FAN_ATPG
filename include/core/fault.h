// **************************************************************************
// File       [ fault.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/04 created ]
// **************************************************************************

#ifndef _CORE_FAULT_H_
#define _CORE_FAULT_H_

#include <list>

#include "circuit.h"

namespace CoreNs
{

	class Fault;
	typedef std::vector<Fault *> FaultVec;
	typedef std::list<Fault *> FaultList;
	typedef std::list<Fault *>::iterator FaultListIter;

	class Fault
	{
	public:
		// ************************************
		// * fault types
		// *     SA0    stuck-at zero
		// *     SA1    stuck-at one
		// *     STR    slow to rise
		// *     STF    slow to fall
		// *     BR     bridge
		// *
		// * fault states  , defined according to TMAX
		// *     UD     undetected
		// *     DT     detected
		// *     PT     possibly testable
		// *     AU     ATPG untestable
		// *     TI     tied to logic zero or one
		// *     RE     redundant
		// *     AB     aborted
		// ************************************
		enum Type
		{
			SA0 = 0,
			SA1,
			STR,
			STF,
			BR
		};
		enum State
		{
			UD = 0,
			DT,
			PT,
			AU,
			TI,
			RE,
			AB
		};

		Fault();
		Fault(int gate, Type type, int line, int eq = 1, State state = UD, int aggr = -1);
		~Fault();

		int aggr_;		// ID of the aggressor gate
		int gate_;		// ID of the faulty gate
		Type type_;		// fault type
		int line_;		// faulty line location: 0 means gate output fault. 1+ means gate input fault
									//              on the corresponding gate input line
		int det_;			// number of detection
		State state_; // fault state
		int eq_;			// the number of equivalent faults, used to calculate uncollapsed fault coverage
	};

	class FaultListExtract
	{
	public:
		enum Type
		{
			SAF = 0,
			TDF,
			BRF
		};

		FaultListExtract();
		~FaultListExtract();

		void extract(Circuit *cir);

		int *gateToFault_;
		FaultVec faults_;
		FaultList current_;
		Type type_;
	};

	inline Fault::Fault()
	{
		aggr_ = -1;
		gate_ = -1;
		type_ = SA0;
		line_ = -1;
		det_ = 0;
		state_ = UD;
		eq_ = 1;
	}
	
	inline Fault::Fault(int gate, Type type, int line, int eq, State state, int aggr)
	{
		aggr_ = aggr;
		gate_ = gate;
		type_ = type;
		line_ = line;
		det_ = 0;
		state_ = state;
		eq_ = eq;
	}

	inline Fault::~Fault() {}

	inline FaultListExtract::FaultListExtract()
	{
		gateToFault_ = NULL;
		type_ = SAF;
	}

	inline FaultListExtract::~FaultListExtract() {}

};

#endif
