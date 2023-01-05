// **************************************************************************
// File       [ fault.h ]
// Author     [ littleshamoo ]
// Synopsis   [ This file defines the faults. ]
// Date       [ 2011/10/04 created ]
// **************************************************************************

#ifndef _CORE_FAULT_H_
#define _CORE_FAULT_H_

#include <vector>
#include <list>
#include "circuit.h"

namespace CoreNs
{

	class Fault;
	typedef std::list<Fault *> FaultPtrList;
	typedef std::list<Fault *>::iterator FaultPtrListIter;

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
		enum FAULT_TYPE
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
		Fault(int gateID, FAULT_TYPE faultType, int faultyLine, int equivalent = 1, FAULT_STATE faultState = UD);

		// int aggr_;            // ID of the aggressor gate.
		int gateID_;             // ID of the faulty gate.
		FAULT_TYPE faultType_;   // Fault type.
		int faultyLine_;         // Faulty line location: 0 means gate output fault,
		                         // 1+ means gate input fault on the corresponding gate input line.
		int detection_;          // Number of detection.
		FAULT_STATE faultState_; // Fault state.
		int equivalent_;         // The number of equivalent faults, used to calculate uncollapsed fault coverage.
	};

	class FaultListExtract
	{
	public:
		enum FAULTLIST_TYPE
		{
			SAF = 0,
			TDF,
			BRF
		};

		FaultListExtract();

		// Extract faults from the circuit.
		void extractFaultFromCircuit(Circuit *pCircuit);

		std::vector<int> gateIndexToFaultIndex_; // Map gate index to fault list index.
		std::vector<Fault> uncollapsedFaults_;   // Record faults without fault collapsing, used for adding part of faults for ATPG.
		std::vector<Fault> extractedFaults_;     // Faults extracted from the circuit.
		FaultPtrList faultsInCircuit_;           // Faults used in the ATPG.
		FAULTLIST_TYPE faultListType_;           // Fault list type.
	};

	inline Fault::Fault()
	{
		gateID_ = -1;
		faultType_ = SA0;
		faultyLine_ = -1;
		detection_ = 0;
		faultState_ = UD;
		equivalent_ = 1;
	}

	inline Fault::Fault(int gateID, FAULT_TYPE faultType, int faultyLine, int equivalent, FAULT_STATE faultState)
	{
		gateID_ = gateID;
		faultType_ = faultType;
		faultyLine_ = faultyLine;
		detection_ = 0;
		faultState_ = faultState;
		equivalent_ = equivalent;
	}

	inline FaultListExtract::FaultListExtract()
	{
		faultListType_ = SAF;
	}

};

#endif