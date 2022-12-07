// **************************************************************************
// File       [ lht_pat_file.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/14 created ]
// **************************************************************************

#ifndef _INTF_LHT_PAT_FILE_H_
#define _INTF_LHT_PAT_FILE_H_

#include "pat_file.h"

namespace IntfNs {

class LhtPatFile {
public:
                 LhtPatFile();
    virtual      ~LhtPatFile();

    virtual bool read(const char * const fname, const bool &verbose = false);
    virtual void setPatternType(const PatType &type);
    virtual void addPattern(const char * const pi1, const char * const pi2,
                            const char * const ppi, const char * const si,
                            const char * const po1, const char * const po2,
                            const char * const ppo);

protected:
    bool success_;
    bool verbose_;
};

inline LhtPatFile::LhtPatFile() {
    success_ = true;
    verbose_ = false;
}
inline LhtPatFile::~LhtPatFile() {}

};

#endif


