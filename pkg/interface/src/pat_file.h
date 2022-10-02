// **************************************************************************
// File       [ pat_file.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/13 created ]
// **************************************************************************

#ifndef _INTF_PAT_FILE_H_
#define _INTF_PAT_FILE_H_

#include "global.h"

namespace IntfNs {

enum PatType { BASIC_SCAN, LAUNCH_CAPTURE, LAUNCH_SHIFT };

struct PatNames {
    char     name[NAME_LEN];
    PatNames *next;
    PatNames *head;
};

class PatFile {
public:
                 PatFile();
    virtual      ~PatFile();

    virtual bool read(const char * const fname, const bool &verbose = false);
    virtual void setPiOrder(const PatNames * const pis);
    virtual void setPpiOrder(const PatNames * const ppis);
    virtual void setPoOrder(const PatNames * const pos);
    virtual void setPatternType(const PatType &type);
    virtual void setPatternNum(const int &num);
    virtual void addPattern(const char * const pi1, const char * const pi2,
                            const char * const ppi, const char * const si,
                            const char * const po1, const char * const po2,
                            const char * const ppo);

protected:
    bool success_;
    bool verbose_;
};

inline PatFile::PatFile() {
    success_ = true;
    verbose_ = false;
}
inline PatFile::~PatFile() {}

};

#endif


