// **************************************************************************
// File       [ vlog_file.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/06/24 created ]
// **************************************************************************


#ifndef _INTF_VLOG_FILE_H_
#define _INTF_VLOG_FILE_H_

#include "global.h"

namespace IntfNs {

enum VlogNetType { VLOG_NET_INPUT,   VLOG_NET_OUTPUT, VLOG_NET_INOUT,
                   VLOG_NET_WIRE,    VLOG_NET_REG,    VLOG_NET_SUPPLY_L,
                   VLOG_NET_SUPPLY_H };

struct VlogNames {
    char      name[NAME_LEN];
    VlogNames *next;
    VlogNames *head;
};

struct VlogPortToNet {
    char          port[NAME_LEN];
    char          net[NAME_LEN];
    VlogPortToNet *next;
    VlogPortToNet *head;
};

class VlogFile {
public:
                 VlogFile();
    virtual      ~VlogFile();

    virtual bool read(const char * const fname, const bool &verbose = false);

    virtual void addModule(const char * const name);
    virtual void addPorts(VlogNames * const ports);
    virtual void setInputNets(VlogNames * const nets);
    virtual void setOutputNets(VlogNames * const nets);
    virtual void setInoutNets(VlogNames * const nets);
    virtual void setWireNets(VlogNames * const nets);
    virtual void setRegNets(VlogNames * const nets);
    virtual void setSupplyLNets(VlogNames * const nets);
    virtual void setSupplyHNets(VlogNames * const nets);
    virtual void addCell(const char * const type, const char * const name,
                         VlogNames * const ports);
    virtual void addCell(const char * const type, const char * const name,
                         VlogPortToNet * const portToNet);
    virtual void addCell(const char * const type,
                         VlogNames * const strengths,
                         const char * const name,
                         VlogNames * const ports);
    virtual void addAssign(const char * const n1, const char * const n2);

protected:
    bool success_;
    bool verbose_;
};

inline VlogFile::VlogFile() {
    success_ = true;
    verbose_ = false;
}
inline VlogFile::~VlogFile() {}

};

#endif

