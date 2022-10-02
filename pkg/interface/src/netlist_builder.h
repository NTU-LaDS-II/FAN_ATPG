// **************************************************************************
// File       [ netlist_builder.h ]
// Author     [ littleshamoo ]
// Synopsis   [ read in netlist and build into netlist data structure ]
// Date       [ 2011/03/01 created ]
// **************************************************************************

#ifndef _INTF_NETLIST_BUILDER_H_
#define _INTF_NETLIST_BUILDER_H_

#include "vlog_file.h"
#include "netlist.h"

namespace IntfNs {

class VlogNlBuilder : public VlogFile {
public:
                 VlogNlBuilder(Netlist * const nl);
    virtual      ~VlogNlBuilder();

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
    bool    addPortNets(VlogNames * const nets);
    Netlist *nl_;
    Cell    *curModule_;
};


inline VlogNlBuilder::VlogNlBuilder(Netlist *nl) {
    nl_        = nl;
    curModule_ = NULL;
}

inline VlogNlBuilder::~VlogNlBuilder() {}

};

#endif

