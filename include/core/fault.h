// **************************************************************************
// File       [ fault.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/04 created ]
// **************************************************************************

#ifndef _CORE_FAULT_H_
#define _CORE_FAULT_H_

#include <vector> //added by pan
#include <list>

#include "circuit.h"

namespace CoreNs
{

	class Fault;
	// typedef std::vector<Fault *> FaultVec; ,removed by pan
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
		enum FAULT_TYPE // Type => FAULT_TYPE, modified by pan
		{
			SA0 = 0,
			SA1,
			STR,
			STF,
			BR
		};
		enum FAULT_STATE // State => FAULT_STATE, modified by pan
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
		~Fault();

		// int aggr_;				     // ID of the aggressor gate ,removed by pan
		int gateID_;						 // ID of the faulty gate. gate_ => gateID_, modified by pan
		FAULT_TYPE faultType_;	 // fault type. type_ => faultType_, modified by pan
		int faultyLine_;				 // faulty line location: 0 means gate output fault. 1+ means gate input fault
														 // on the corresponding gate input line. line_ => faultyLine_, modified by pan
		int detection_;					 // number of detection. det_ => detection_, modified by pan
		FAULT_STATE faultState_; // fault state. state_ => faultState_, modified by pan
		int equivalent_;				 // the number of equivalent faults, used to calculate uncollapsed fault coverage. eq => equivalent, modified by pan
	};

	class FaultListExtract
	{
	public:
		enum FAULTLIST_TYPE // Type => FAULTLIST_TYPE, modified by pan
		{
			SAF = 0,
			TDF,
			BRF
		};

		FaultListExtract();
		~FaultListExtract();

		void extractFaultFromCircuit(Circuit *circuit); // cir => circuit, modified by pan

		std::vector<int> gateIndexToFaultIndex_; // map gate index to fault list index. int* => std::vector<int>, gateToFault => gateIndexToFaultIndex, modified by pan
		std::vector<Fault *> uncollapsedFaults_; // record faults without fault collapsing, used for adding part of faults in atpg_cmd.cpp
		std::vector<Fault *> extractedFaults_;	 // faults extracted from the circuit. faults => extractedFaults, modified by pan
		FaultList faultsInCircuit_;							 // faults used in the ATPG. current_ => faultsInCircuit_, modified by pan
		FAULTLIST_TYPE faultListType_;					 // fault list type. type_ => faultListType, modified by pan
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

	inline Fault::~Fault() {}

	inline FaultListExtract::FaultListExtract()
	{
		faultListType_ = SAF;
	}

	inline FaultListExtract::~FaultListExtract() {}

};

#endif
