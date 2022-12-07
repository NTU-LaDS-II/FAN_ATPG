// **************************************************************************
// File       [ mdt_file.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/06/23 created ]
// **************************************************************************

#ifndef _INTF_MDT_FILE_H_
#define _INTF_MDT_FILE_H_

#include "global.h"

namespace IntfNs {

enum MdtPortType { MDT_PORT_INPUT, MDT_PORT_OUTPUT, MDT_PORT_INOUT,
                   MDT_PORT_INTERN };

struct MdtNames {
    char     name[NAME_LEN];
    MdtNames *next;
    MdtNames *head;
};

struct MdtPortToNet {
    char         port[NAME_LEN];
    char         net[NAME_LEN];
    MdtPortToNet *next;
    MdtPortToNet *head;
};

class MdtFile {
public:
                 MdtFile();
    virtual      ~MdtFile();

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
    bool success_;
    bool verbose_;
};

inline MdtFile::MdtFile() {
    success_ = true;
    verbose_ = false;
}
inline MdtFile::~MdtFile() {}

};

#endif

