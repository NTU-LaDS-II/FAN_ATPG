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
		enum FAULT_STATE
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
		Fault(int gate, Type type, int line, int eq = 1, FAULT_STATE state = UD, int aggr = -1);
		~Fault();

		int aggr_;					// ID of the aggressor gate
		int gateID_;					// ID of the faulty gate
		Type faultType_;					// fault type
		int faultyLine_;					// faulty line location: 0 means gate output fault. 1+ means gate input fault
												//              on the corresponding gate input line
		int detection_;						// number of detection
		FAULT_STATE faultState_; // fault state
		int equivalent_;						// the number of equivalent faults, used to calculate uncollapsed fault coverage
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

		int *gateIndexToFaultIndex;
		FaultVec extractedFaults_;
		FaultList faultsInCircuit_;
		Type faultListType_;
	};

	inline Fault::Fault()
	{
		aggr_ = -1;
		gateID_ = -1;
		faultType_ = SA0;
		faultyLine_ = -1;
		detection_ = 0;
		faultState_ = UD;
		equivalent_ = 1;
	}

	inline Fault::Fault(int gate, Type type, int line, int eq, FAULT_STATE state, int aggr)
	{
		aggr_ = aggr;
		gateID_ = gate;
		faultType_ = type;
		faultyLine_ = line;
		detection_ = 0;
		faultState_ = state;
		equivalent_ = eq;
	}

	inline Fault::~Fault() {}

	inline FaultListExtract::FaultListExtract()
	{
		gateIndexToFaultIndex = NULL;
		faultListType_ = SAF;
	}

	inline FaultListExtract::~FaultListExtract() {}

};

#endif
