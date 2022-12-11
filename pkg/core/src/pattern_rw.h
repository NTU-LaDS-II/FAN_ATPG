// **************************************************************************
// File       [ pattern_rw.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/10 created ]
// **************************************************************************

#ifndef _CORE_PATTERN_RW_H_
#define _CORE_PATTERN_RW_H_

#include "interface/pat_file.h"

#include "circuit.h"
#include "pattern.h"

namespace CoreNs {

// This is actually a patternReader
// B.9
class PatternReader : public IntfNs::PatFile {
public:
                 PatternReader(PatternProcessor *pcoll, Circuit *cir);
                 ~PatternReader();
    virtual void setPiOrder(const IntfNs::PatNames * const pis);
    virtual void setPpiOrder(const IntfNs::PatNames * const ppis);
    virtual void setPoOrder(const IntfNs::PatNames * const pos);
    virtual void setPatternType(const IntfNs::PatType &type);
    virtual void setPatternNum(const int &num);
    virtual void addPattern(const char * const pi1, const char * const pi2,
                            const char * const ppi, const char * const si,
                            const char * const po1, const char * const po2,
                            const char * const ppo);

protected:
    void            assignValue(std::vector<Value> &v, const char * const pat,
                                const int &size);
    int             curPat_;
    PatternProcessor     *pcoll_;
    Circuit         *cir_;
};

inline PatternReader::PatternReader(PatternProcessor *pcoll, Circuit *cir) {
    curPat_ = 0;
    pcoll_  = pcoll;
    cir_    = cir;
}

inline PatternReader::~PatternReader() {}


class PatternWriter {
public:
         PatternWriter(PatternProcessor *pcoll, Circuit *cir);
         ~PatternWriter();

    bool writePat(const char * const fname);
    bool writeLht(const char * const fname);
    bool writeAscii(const char * const fname);
	bool writeSTIL(const char * const fname); //2016 summer train

protected:
    PatternProcessor *pcoll_;
    Circuit     *cir_;
};

inline PatternWriter::PatternWriter(PatternProcessor *pcoll, Circuit *cir) {
    pcoll_ = pcoll;
    cir_   = cir;
}

inline PatternWriter::~PatternWriter() {}


class ProcedureWriter {
    public:
             ProcedureWriter(Circuit *cir);
             ~ProcedureWriter();
        bool writeProc(const char * const fname);
    protected:
            Circuit *cir_;
};

inline ProcedureWriter::ProcedureWriter(Circuit *cir) {
    cir_ = cir;
}

inline ProcedureWriter::~ProcedureWriter() {}
};




#endif
