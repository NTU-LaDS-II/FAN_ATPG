// **************************************************************************
// File       [ circuit.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2010/12/29 created ]
// **************************************************************************

#ifndef _CORE_CIRCUIT_H_
#define _CORE_CIRCUIT_H_

#include <set>

#include "interface/netlist.h"

#include "gate.h"

namespace CoreNs {

class Circuit {
public:
    Circuit();
    ~Circuit();

	// specify how to connect multiple time frames of circuits
	// CAPTURE means Launch-on-capture;  SHIFT means launch-on-shift
    enum            tfConnectType { CAPTURE = 0, SHIFT };

    // build circuit from netlist
    bool            build(IntfNs::Netlist * const nl, const int &nframe = 1,
                          const tfConnectType &type = CAPTURE);

    // info for one time frame
    IntfNs::Netlist *nl_;         // corresponding netlist
    int             npi_;         // number of PIs
    int             nppi_;        // number of PPIs
    int             npo_;         // number of POs
    int             ncomb_;       // number of combinational gates
    int             ngate_;       // number of gates
    int             nnet_;        // number of nets
    int             lvl_;         // circuit level, starting from inputs

    // info for multiple time frames
    int             nframe_;      // number of time frame
    tfConnectType        connType_;    // time frame connection type
    int             tgate_;       // number of total gates
    int             tlvl_;        // total level

    // structure
    // **********************************************************************
    // frame 1                frame 2                      frame n
    // |--- ---- ---- --- ----|--- ---- ---- --- ----|     |--- ---- ----
    // |PI1|PPI1|cell|PO1|PPO1|PI2|PPI2|cell|PO2|PPO2| ... |PIn|PPIn|cell|...
    // |--- ---- ---- --- ----|--- ---- ---- --- ----|     |--- ---- ----
    // **********************************************************************
    Gate            *gates_;      // gate array
    int             *fis_;        // fanin net array
    int             *fos_;        // fanout net array
    int             *cellToGate_; // map cells in netlist to gates
    int             *portToGate_; // map ports in netlist to gates

protected:
    // for circuit building
    void createMap();
    void calNgate();
    void calNnet();
    void createGate();
    void createPi(int &nfo);
    void createPpi(int &nfo);
    void createComb(int &nfi, int &nfo);
    void createPmt(const int &id, const IntfNs::Cell * const c,
                   const IntfNs::Pmt * const pmt, int &nfi, int &nfo);
    void detGateType(const int &id, const IntfNs::Cell * const c,
                     const IntfNs::Pmt * const pmt);
    void createPo(int &nfi);
    void createPpo(int &nfi);
    void connectFrame();
    void assignFiMinLvl();
};

inline Circuit::Circuit() {
    nl_         = NULL;
    npi_        = 0;
    nppi_       = 0;
    npo_        = 0;
    ncomb_      = 0;
    ngate_      = 0;
    nnet_       = 0;
    lvl_        = -1;
    nframe_     = 1;
    connType_   = CAPTURE;
    gates_      = NULL;
    cellToGate_ = NULL;
    portToGate_ = NULL;
}

inline Circuit::~Circuit() {
    delete [] gates_;
    delete [] fis_;
    delete [] fos_;
    delete [] cellToGate_;
    delete [] portToGate_;
}

};

#endif

