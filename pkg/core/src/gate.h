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
		Gate();
		~Gate();

		enum Type
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

		// basic info
		int id_;		// position in circuit gate array
		int cid_;		// original cell id in the netlist
		int pmtid_; // original primitive id in the library cell
		int lvl_;		// level number after levelization
		int frame_; // time frame of the gate, for 2-pattern test
		Type type_; // type of the gate

		// connection
		int nfi_;	 // number of fanin
		int *fis_; // fanin array
		int nfo_;	 // number of fanout
		int *fos_; // fanout array

		// values
		Value v_;			 // single value for ATPG
		ParaValue gl_; // good low
		ParaValue gh_; // good high
		ParaValue fl_; // faulty low
		ParaValue fh_; // faulty high

		// constraint
		// user can tie the gate to certain value
		bool hasCons_;
		ParaValue cons_;

		// SCOAP, testability
		int cc0_;
		int cc1_;
		int co_;

		int depthFromPo_; // depth from po, this is for fault effect propagation
		int fiMinLvl_;		// the minimum level of the fanin gates, this is to justify the headline cone, (in atpg.cpp)

		Value isUnary() const;
		Value isInverse() const;
		Value getInputNonCtrlValue() const;
		Value getInputCtrlValue() const;
		Value getOutputCtrlValue() const;

		/* Added by Shi-Tang Liu */
		Value preV_;
	};

	inline Gate::Gate()
	{
		id_ = -1;
		cid_ = -1;
		pmtid_ = 0;
		lvl_ = -1;
		frame_ = 0;
		type_ = NA;
		nfi_ = 0;
		fis_ = NULL;
		nfo_ = 0;
		fos_ = NULL;
		v_ = X;
		gl_ = PARA_L;
		gh_ = PARA_L;
		fl_ = PARA_L;
		fh_ = PARA_L;
		hasCons_ = false;
		cons_ = PARA_L;

		cc0_ = 0;
		cc1_ = 0;
		co_ = 0;

		depthFromPo_ = -1;
		fiMinLvl_ = -1;

		/* Added by Shi-Tang Liu */
		preV_ = X;
	}

	inline Gate::~Gate() {}

	inline Value Gate::isUnary() const
	{
		return nfi_ == 1 ? H : L;
	}

	inline Value Gate::isInverse() const
	{
		switch (type_)
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

	inline Value Gate::getInputNonCtrlValue() const
	{
		return isInverse() == getOutputCtrlValue() ? L : H;
	}

	inline Value Gate::getInputCtrlValue() const
	{
		return getInputNonCtrlValue() == H ? L : H;
	}

	inline Value Gate::getOutputCtrlValue() const
	{
		switch (type_)
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
