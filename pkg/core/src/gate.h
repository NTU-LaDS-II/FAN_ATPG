// **************************************************************************
// File       [ gate.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/07/05 created ]
// **************************************************************************

#ifndef _CORE_GATE_H_
#define _CORE_GATE_H_

#include <cstring>
#include "interface/cell.h"
#include "logic.h"

namespace CoreNs
{
	class Gate
	{
	public:
		enum GateType
		{
			NA = 0,
			PI,
			PO,
			PPI,
			PPO,
			PPI_IN,
			PPO_IN,
			INV,
			BUF,
			AND2,
			AND3,
			AND4,
			NAND2,
			NAND3,
			NAND4,
			OR2,
			OR3,
			OR4,
			NOR2,
			NOR3,
			NOR4,
			XOR2,
			XOR3,
			XNOR2,
			XNOR3,
			MUX,
			TIE0,
			TIE1,
			TIEX,
			TIEZ
		};
		inline Gate();
		inline Gate(int gateId, int cellId, int primitiveId, int numLevel, GateType gateType, int numFO);

		// basic info
		int gateId_;				// position in circuit gate array
		int cellId_;				// original cell id in the netlist
		int primitiveId_;		// original primitive id in the library cell
		int numLevel_;			// level number after levelization
		int frame_;					// time frame of the gate, for 2-pattern test
		GateType gateType_; // type of the gate

		// connection
		int numFI_;											// number of fanin
		int numFO_;											// number of fanout
		std::vector<int> faninVector_;	// fanin array
		std::vector<int> fanoutVector_; // fanout array

		// values
		Value atpgVal_;							 // single value for ATPG
		ParallelValue goodSimLow_;	 // good low
		ParallelValue goodSimHigh_;	 // good high
		ParallelValue faultSimLow_;	 // faulty low
		ParallelValue faultSimHigh_; // faulty high

		// constraint, user can tie the gate to certain value
		bool hasConstraint_;
		ParallelValue constraint_;

		// SCOAP testability
		int cc0_;
		int cc1_;
		int co_;

		int depthFromPo_;			 // depth from po, this is for fault effect propagation
		int minLevelOfFanins_; // the minimum level of the fanin gates, this is to justify the headline cone, (in atpg.cpp)

		Value isUnary() const;
		Value isInverse() const;
		Value getInputNonCtrlValue() const;
		Value getInputCtrlValue() const;
		Value getOutputCtrlValue() const;
		Value prevAtpgValStored_; // Added by Shi-Tang Liu
	};

	inline Gate::Gate()
	{
		gateId_ = -1;
		cellId_ = -1;
		primitiveId_ = 0;
		numLevel_ = -1;
		frame_ = 0;
		gateType_ = NA;
		numFI_ = 0;
		numFO_ = 0;
		atpgVal_ = X;
		goodSimLow_ = PARA_L;
		goodSimHigh_ = PARA_L;
		faultSimLow_ = PARA_L;
		faultSimHigh_ = PARA_L;
		hasConstraint_ = false;
		constraint_ = PARA_L;
		cc0_ = 0;
		cc1_ = 0;
		co_ = 0;
		depthFromPo_ = -1;
		minLevelOfFanins_ = -1;
		prevAtpgValStored_ = X; // Added by Shi-Tang Liu
	}

	inline Gate::Gate(int gateId, int cellId, int primitiveId, int numLevel, GateType gateType, int numFO)
			: gateId_(gateId),
				cellId_(cellId),
				primitiveId_(primitiveId),
				numLevel_(numLevel),
				gateType_(gateType),
				numFO_(numFO)
	{
		frame_ = 0;
		numFI_ = 0;
		fanoutVector_ = std::vector<int>(numFO);
		atpgVal_ = X;
		goodSimLow_ = PARA_L;
		goodSimHigh_ = PARA_L;
		faultSimLow_ = PARA_L;
		faultSimHigh_ = PARA_L;
		hasConstraint_ = false;
		constraint_ = PARA_L;
		cc0_ = 0;
		cc1_ = 0;
		co_ = 0;
		depthFromPo_ = -1;
		minLevelOfFanins_ = -1;
		prevAtpgValStored_ = X; // Added by Shi-Tang Liu
	}

	// **************************************************************************
	// Function   [ Gate::isUnary ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Check if the gate has only one fanin.
	//							description:
	//								If the gate has exactly one fanin, return H (Value).
	//								Otherwise, return L.
	//							arguments:
	// 								[out] Value : Return H if numFI_ == 1, return L otherwise.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline Value Gate::isUnary() const
	{
		return numFI_ == 1 ? H : L;
	}

	// **************************************************************************
	// Function   [ Gate::isInverse ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Check if the gate is an inverse gate.
	//							description:
	//								If the gate type is INV, NAND, NOR or XNOR,
	//								then it is an inverse gate. Otherwise, it is not.
	//							arguments:
	// 								[out] Value : Return H if it is inverse gate, return L otherwise.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline Value Gate::isInverse() const
	{
		switch (gateType_)
		{
			case INV:
			case NAND2:
			case NAND3:
			case NAND4:
			case NOR2:
			case NOR3:
			case NOR4:
			case XNOR2:
			case XNOR3:
				return H;
			default:
				return L;
		}
	}

	// **************************************************************************
	// Function   [ Gate::getInputNonCtrlValue ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Get input non-control value of the gate.
	//							description:
	//								Determined by comparing the output of isInverse() and the output
	//								control value of the gate. If identical then the input non-control
	//								value is L. Otherwise it is H.
	//								If the gate type is INV, NOR or OR, input non-control value is L.
	//							arguments:
	// 								[out] Value : Return H if it is inverse gate, return L otherwise.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline Value Gate::getInputNonCtrlValue() const
	{
		return (isInverse() == getOutputCtrlValue()) ? L : H;
	}

	// **************************************************************************
	// Function   [ Gate::getInputCtrlValue ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Get input control value of the gate.
	//							description:
	//								Call getInputNonCtrlValue() to get input non-control value
	//								and return the inverse value.
	//							arguments:
	// 								[out] Value : Return the input control value of the gate.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline Value Gate::getInputCtrlValue() const
	{
		return (getInputNonCtrlValue() == H) ? L : H;
	}

	// **************************************************************************
	// Function   [ Gate::getOutputCtrlValue ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Get output control value of the gate.
	//							description:
	//								If the gate type is OR or NAND, output control value is L.
	//								If the gate type is XOR or XNOR, output control value is X.
	//								Otherwise, output control value is H.
	//							arguments:
	// 								[out] Value : Return the output control value of the gate.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline Value Gate::getOutputCtrlValue() const
	{
		switch (gateType_)
		{
			case OR2:
			case OR3:
			case OR4:
			case NAND2:
			case NAND3:
			case NAND4:
				return L;
			case XOR2:
			case XOR3:
			case XNOR2:
			case XNOR3:
				return X;
			default:
				return H;
		}
	}

};

#endif