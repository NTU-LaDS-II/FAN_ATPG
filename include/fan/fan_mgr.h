// **************************************************************************
// File       [ fan_mgr.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/08/30 created ]
// **************************************************************************

#ifndef _FAN_FAN_MGR_H_
#define _FAN_FAN_MGR_H_

#include "common/tm_usage.h"

#include "interface/netlist.h"
#include "interface/techlib.h"

#include "core/atpg.h"

namespace FanNs {

class FanMgr {
public:
    FanMgr() {
        lib            = NULL;
        nl             = NULL;
        fListExtract          = NULL;
        pcoll          = NULL;
        cir            = NULL;
        sim            = NULL;
        atpg           = NULL;
        atpgStat.rTime = 0;
    }
    ~FanMgr() {}

    IntfNs::Techlib     *lib;
    IntfNs::Netlist     *nl;
    CoreNs::FaultListExtract   *fListExtract;;
    CoreNs::PatternProcessor *pcoll;
    CoreNs::Circuit     *cir;
    CoreNs::Simulator   *sim;
    CoreNs::Atpg        *atpg;
    CommonNs::TmUsage   tmusg;
    CommonNs::TmStat    atpgStat;
};

};

#endif


