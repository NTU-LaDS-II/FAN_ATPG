// **************************************************************************
// File       [ techlib_builder.h ]
// Author     [ littleshamoo ]
// Synopsis   [ read in technology library and build into netlist data
//              structure ]
// Date       [ 2011/03/18 created ]
// **************************************************************************

#ifndef _INTF_TECHLIB_BUILDER_H_
#define _INTF_TECHLIB_BUILDER_H_

#include "mdt_file.h"
#include "techlib.h"

namespace IntfNs {

class MdtLibBuilder : public MdtFile {
public:
                 MdtLibBuilder(Techlib *lib);
    virtual      ~MdtLibBuilder();

    virtual bool read(const char * const fname, const bool &verbose = false);
    virtual void addModel(const char * const name);
    virtual void addPorts(MdtNames * const ports);
    virtual void setInputNets(MdtNames * const nets);
    virtual void setOutputNets(MdtNames * const nets);
    virtual void setInoutNets(MdtNames * const nets);
    virtual void setInternNets(MdtNames * const nets);
    virtual void addPrimitive(const char * const type,
                              const char * const name,
                              MdtNames * const ports);
    virtual void addInstance(const char * const type,
                             const char * const name,
                             MdtNames * const ports);
    virtual void addInstance(const char * const type,
                             const char * const name,
                             MdtPortToNet * const portToNet);

protected:
    bool    addPortNets(MdtNames * const nets);
    bool    checkInstances();
    bool    removeFloatingNets();
    Cell    *curCell_;
    Techlib *lib_;
};

inline MdtLibBuilder::MdtLibBuilder(Techlib *lib) {
    curCell_ = NULL;
    lib_     = lib;
}
inline MdtLibBuilder::~MdtLibBuilder() {}

};

#endif

