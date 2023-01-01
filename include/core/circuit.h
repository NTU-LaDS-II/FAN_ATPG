// **************************************************************************
// File       [ circuit.h ]
// Author     [ littleshamoo ]
// Synopsis   [ this file define the circuits ]
// Date       [ 2010/12/29 created ]
// **************************************************************************

#ifndef _CORE_CIRCUIT_H_
#define _CORE_CIRCUIT_H_

#include "interface/netlist.h"
#include "gate.h"

namespace CoreNs
{
	class Circuit
	{
	public:
		// specify how to connect multiple time frames of circuits
		// CAPTURE means Launch-on-capture;  SHIFT means launch-on-shift
		enum TIME_FRAME_CONNECT_TYPE
		{
			CAPTURE = 0,
			SHIFT
		};

		inline Circuit();

		// buildCircuit circuit from netlist
		bool buildCircuit(IntfNs::Netlist *const pNetlist, const int &numFrame = 1,
		                  const TIME_FRAME_CONNECT_TYPE &timeFrameConnectType = CAPTURE);

		// info for one time frame
		IntfNs::Netlist *pNetlist_; // corresponding netlist
		int numPI_;                 // number of PIs
		int numPPI_;                // number of PPIs (PPOs)
		int numPO_;                 // number of POs
		int numComb_;               // number of combinational gates
		int numGate_;               // number of gates
		int numNet_;                // number of nets
		int circuitLvl_;            // circuit level, starting from inputs

		// info for multiple time frames
		int numFrame_;                                 // number of time frames
		TIME_FRAME_CONNECT_TYPE timeFrameConnectType_; // time frame connection type
		int totalGate_;                                // number of total gates. Equal to numGate_ * numFrame_
		int totalLvl_;                                 // total level. Equal to circuitLvl_ * numFrame_

		// structure
		// **********************************************************************
		// frame 1                frame 2                      frame n
		// |--- ---- ---- --- ----|--- ---- ---- --- ----|     |--- ---- ----
		// |PI1|PPI1|cell|PO1|PPO1|PI2|PPI2|cell|PO2|PPO2| ... |PIn|PPIn|cell|...
		// |--- ---- ---- --- ----|--- ---- ---- --- ----|     |--- ---- ----
		// **********************************************************************

		std::vector<Gate> circuitGates_;
		std::vector<int> cellIndexToGateIndex_; // map cells in netlist to gates
		std::vector<int> portIndexToGateIndex_; // map ports in netlist to gates

	protected:
		// for circuit building
		void mapNetlistToCircuit();
		void calculateNumGate();
		void calculateNumNet();
		void createCircuitGates();
		void createCircuitPI();
		void createCircuitPPI();
		void createCircuitComb();
		void createCircuitPmt(const int &gateID, const IntfNs::Cell *const cell,
		                      const IntfNs::Pmt *const pmt);
		void determineGateType(const int &gateID, const IntfNs::Cell *const cell,
		                       const IntfNs::Pmt *const pmt);
		void createCircuitPO();
		void createCircuitPPO();
		void connectMultipleTimeFrame();
		void assignFiMinLvl();
	};

	inline Circuit::Circuit()
	{
		pNetlist_ = NULL;
		numPI_ = 0;
		numPPI_ = 0;
		numPO_ = 0;
		numComb_ = 0;
		numGate_ = 0;
		numNet_ = 0;
		circuitLvl_ = -1;
		numFrame_ = 1;
		timeFrameConnectType_ = CAPTURE;
	}
};

#endif