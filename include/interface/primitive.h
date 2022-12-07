// **************************************************************************
// File       [ primitive.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/04/19 created ]
// **************************************************************************

#ifndef _INTF_PRIMITIVE_H_
#define _INTF_PRIMITIVE_H_

#include <cstdio>
#include "cell.h"

namespace IntfNs {

//{{{ class Pmt
class Pmt : public Cell {
public:
         Pmt(const char * const name);
         ~Pmt();

    enum Type { NA,
                AND,   NAND,  OR,     NOR,    INV,    BUF,  BUFZ, XOR, XNOR,
                TSL,   TSLI,  TSH,    TSHI,
                MUX,   DFF,   DLAT,   DELAY,  INVF,   WIRE, PULL,
                TIE1,  TIE0,  TIEX,   TIEZ,   UNDEF,
                NMOS,  PMOS,  RNMOS,  RPMOS,  NMOSF,  PMOSF,
                CMOS1, CMOS2, RCMOS1, RCMOS2, CMOS1F, CMOS2F
              };

    Type type_;
}; //}}}

//{{{ class PmtAnd
class PmtAnd : public Pmt {
public:
    PmtAnd(const char * const name, const size_t &nIn);
    ~PmtAnd();
}; //}}}
//{{{ class PmtNand
class PmtNand : public Pmt {
public:
    PmtNand(const char * const name, const size_t &nIn);
    ~PmtNand();
}; //}}}
//{{{ class PmtOr
class PmtOr : public Pmt {
public:
    PmtOr(const char * const name, const size_t &nIn);
    ~PmtOr();
}; //}}}
//{{{ class PmtNor
class PmtNor : public Pmt {
public:
    PmtNor(const char * const name, const size_t &nIn);
    ~PmtNor();
}; //}}}
//{{{ class PmtInv
class PmtInv : public Pmt {
public:
    PmtInv(const char * const name);
    ~PmtInv();
}; //}}}
//{{{ class PmtBuf
class PmtBuf : public Pmt {
public:
    PmtBuf(const char * const name);
    ~PmtBuf();
}; //}}}
//{{{ class PmtBufz
class PmtBufz : public Pmt {
public:
    PmtBufz(const char * const name);
    ~PmtBufz();
}; //}}}
//{{{ class PmtXor
class PmtXor : public Pmt {
public:
    PmtXor(const char * const name, const size_t &nIn);
    ~PmtXor();
}; //}}}
//{{{ class PmtXnor
class PmtXnor : public Pmt {
public:
    PmtXnor(const char * const name, const size_t &nIn);
    ~PmtXnor();
}; //}}}
//{{{ class PmtTsl
class PmtTsl : public Pmt {
public:
    PmtTsl(const char * const name);
    ~PmtTsl();
}; //}}}
//{{{ class PmtTsli
class PmtTsli : public Pmt {
public:
    PmtTsli(const char * const name);
    ~PmtTsli();
}; //}}}
//{{{ class PmtTsh
class PmtTsh : public Pmt {
public:
    PmtTsh(const char * const name);
    ~PmtTsh();
}; //}}}
//{{{ class PmtTshi
class PmtTshi : public Pmt {
public:
    PmtTshi(const char * const name);
    ~PmtTshi();
}; //}}}
//{{{ class PmtMux
class PmtMux : public Pmt {
public:
    PmtMux(const char * const name);
    ~PmtMux();
}; //}}}
//{{{ class PmtDff
class PmtDff : public Pmt {
public:
    PmtDff(const char * const name, const size_t &nClk = 1);
    ~PmtDff();
}; //}}}
//{{{ class PmtDlat
class PmtDlat : public Pmt {
public:
    PmtDlat(const char * const name, const size_t &nClk = 1);
    ~PmtDlat();
}; //}}}
//{{{ class PmtDelay
class PmtDelay : public Pmt {
public:
    PmtDelay(const char * const name);
    ~PmtDelay();
}; //}}}
//{{{ class PmtInvf
class PmtInvf : public Pmt {
public:
    PmtInvf(const char * const name);
    ~PmtInvf();
}; //}}}
//{{{ class PmtWire
class PmtWire : public Pmt {
public:
    PmtWire(const char * const name, const size_t &nIn);
    ~PmtWire();
}; //}}}
//{{{ class PmtPull
class PmtPull : public Pmt {
public:
    PmtPull(const char * const name);
    ~PmtPull();
}; //}}}
//{{{ class PmtTie1
class PmtTie1 : public Pmt {
public:
    PmtTie1(const char * const name);
    ~PmtTie1();
}; //}}}
//{{{ class PmtTie0
class PmtTie0 : public Pmt {
public:
    PmtTie0(const char * const name);
    ~PmtTie0();
}; //}}}
//{{{ class PmtTieX
class PmtTieX : public Pmt {
public:
    PmtTieX(const char * const name);
    ~PmtTieX();
}; //}}}
//{{{ class PmtTieZ
class PmtTieZ : public Pmt {
public:
    PmtTieZ(const char * const name);
    ~PmtTieZ();
}; //}}}
//{{{ class PmtUndefined
class PmtUndefined : public Pmt {
public:
    PmtUndefined(const char * const name, const size_t &nIn);
    ~PmtUndefined();
}; //}}}
//{{{ class PmtNmos
class PmtNmos : public Pmt {
public:
    PmtNmos(const char * const name);
    ~PmtNmos();
}; //}}}
//{{{ class PmtPmos
class PmtPmos : public Pmt {
public:
    PmtPmos(const char * const name);
    ~PmtPmos();
}; //}}}
//{{{ class PmtRnmos
class PmtRnmos : public Pmt {
public:
    PmtRnmos(const char * const name);
    ~PmtRnmos();
}; //}}}
//{{{ class PmtRpmos
class PmtRpmos : public Pmt {
public:
    PmtRpmos(const char * const name);
    ~PmtRpmos();
}; //}}}
//{{{ class PmtNmosf
class PmtNmosf : public Pmt {
public:
    PmtNmosf(const char * const name);
    ~PmtNmosf();
}; //}}}
//{{{ class PmtPmosf
class PmtPmosf : public Pmt {
public:
    PmtPmosf(const char * const name);
    ~PmtPmosf();
}; //}}}
//{{{ class PmtCmos1
class PmtCmos1 : public Pmt {
public:
    PmtCmos1(const char * const name);
    ~PmtCmos1();
}; //}}}
//{{{ class PmtCmos2
class PmtCmos2 : public Pmt {
public:
    PmtCmos2(const char * const name);
    ~PmtCmos2();
}; //}}}
//{{{ class PmtRcmos1
class PmtRcmos1 : public Pmt {
public:
    PmtRcmos1(const char * const name);
    ~PmtRcmos1();
}; //}}}
//{{{ class PmtRcmos2
class PmtRcmos2 : public Pmt {
public:
    PmtRcmos2(const char * const name);
    ~PmtRcmos2();
}; //}}}
//{{{ class PmtCmos1f
class PmtCmos1f : public Pmt {
public:
    PmtCmos1f(const char * const name);
    ~PmtCmos1f();
}; //}}}
//{{{ class PmtCmos2f
class PmtCmos2f : public Pmt {
public:
    PmtCmos2f(const char * const name);
    ~PmtCmos2f();
}; //}}}


// inline methods
//{{{ class Pmt
inline Pmt::Pmt(const char * const name) : Cell(name) {
    type_  = NA;
    isPmt_ = true;
}
inline Pmt::~Pmt() {}
//}}}

//{{{ class PmtAnd
inline PmtAnd::PmtAnd(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = AND;
    strcpy(typeName_, "AND");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN %d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtAnd::~PmtAnd() {}
//}}}
//{{{ class PmtNand
inline PmtNand::PmtNand(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = NAND;
    strcpy(typeName_, "NAND");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtNand::~PmtNand() {}
//}}}
//{{{ class PmtOr
inline PmtOr::PmtOr(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = OR;
    strcpy(typeName_, "OR");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtOr::~PmtOr() {}
//}}}
//{{{ class PmtNor
inline PmtNor::PmtNor(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = NOR;
    strcpy(typeName_, "NOR");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtNor::~PmtNor() {}
//}}}
//{{{ class PmtInv
inline PmtInv::PmtInv(const char * const name) : Pmt(name) {
    type_ = INV;
    strcpy(typeName_, "INV");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtInv::~PmtInv() {}
//}}}
//{{{ class PmtBuf
inline PmtBuf::PmtBuf(const char * const name) : Pmt(name) {
    type_ = BUF;
    strcpy(typeName_, "BUF");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtBuf::~PmtBuf() {}
//}}}
//{{{ class PmtBufz
inline PmtBufz::PmtBufz(const char * const name) : Pmt(name) {
    type_ = BUFZ;
    strcpy(typeName_, "BUFZ");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtBufz::~PmtBufz() {}
//}}}
//{{{ class PmtXor
inline PmtXor::PmtXor(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = XOR;
    strcpy(typeName_, "XOR");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtXor::~PmtXor() {}
//}}}
//{{{ class PmtXnor
inline PmtXnor::PmtXnor(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = XNOR;
    strcpy(typeName_, "XNOR");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtXnor::~PmtXnor() {}
//}}}
//{{{ class PmtTsl
inline PmtTsl::PmtTsl(const char * const name) : Pmt(name) {
    type_ = TSL;
    strcpy(typeName_, "TSL");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTsl::~PmtTsl() {}
//}}}
//{{{ class PmtTsli
inline PmtTsli::PmtTsli(const char * const name) : Pmt(name) {
    type_ = TSLI;
    strcpy(typeName_, "TSLI");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTsli::~PmtTsli() {}
//}}}
//{{{ class PmtTsh
inline PmtTsh::PmtTsh(const char * const name) : Pmt(name) {
    type_ = TSH;
    strcpy(typeName_, "TSH");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTsh::~PmtTsh() {}
//}}}
//{{{ class PmtTshi
inline PmtTshi::PmtTshi(const char * const name) : Pmt(name) {
    type_ = TSHI;
    strcpy(typeName_, "TSHI");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTshi::~PmtTshi() {}
//}}}
//{{{ class PmtMux
inline PmtMux::PmtMux(const char * const name) : Pmt(name) {
    type_ = MUX;
    strcpy(typeName_, "MUX");
    // add ports
    Port *in0 = new Port("IN0");
    in0->type_ = Port::INPUT;
    addPort(in0);
    Port *in1 = new Port("IN1");
    in1->type_ = Port::INPUT;
    addPort(in1);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtMux::~PmtMux() {}
//}}}
//{{{ class PmtDff
inline PmtDff::PmtDff(const char * const name, const size_t &nClk)
  : Pmt(name) {
    type_ = DFF;
    strcpy(typeName_, "DFF");
    // add ports
    Port *set = new Port("SET");
    set->type_ = Port::INPUT;
    addPort(set);
    Port *reset = new Port("RESET");
    reset->type_ = Port::INPUT;
    addPort(reset);
    char pname[NAME_LEN];
    for (size_t i = 0; i < nClk; ++i) {
        sprintf(pname, "CLK%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
        sprintf(pname, "D%d", (int)i);
        in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *q = new Port("Q");
    q->type_ = Port::OUTPUT;
    addPort(q);
    Port *qn = new Port("QN");
    qn->type_ = Port::OUTPUT;
    addPort(qn);
}

inline PmtDff::~PmtDff() {}
//}}}
//{{{ class PmtDlat
inline PmtDlat::PmtDlat(const char * const name, const size_t &nClk)
  : Pmt(name) {
    type_ = DLAT;
    strcpy(typeName_, "DLAT");
    // add ports
    Port *set = new Port("SET");
    set->type_ = Port::INPUT;
    addPort(set);
    Port *reset = new Port("RESET");
    reset->type_ = Port::INPUT;
    addPort(reset);
    char pname[NAME_LEN];
    for (size_t i = 0; i < nClk; ++i) {
        sprintf(pname, "CLK%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
        sprintf(pname, "D%d", (int)i);
        in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *q = new Port("Q");
    q->type_ = Port::OUTPUT;
    addPort(q);
    Port *qn = new Port("QN");
    qn->type_ = Port::OUTPUT;
    addPort(qn);
}

inline PmtDlat::~PmtDlat() {}
//}}}
//{{{ class PmtDelay
inline PmtDelay::PmtDelay(const char * const name) : Pmt(name) {
    type_ = DELAY;
    strcpy(typeName_, "DELAY");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtDelay::~PmtDelay() {}
//}}}
//{{{ class PmtInvf
inline PmtInvf::PmtInvf(const char * const name) : Pmt(name) {
    type_ = INVF;
    strcpy(typeName_, "INVF");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtInvf::~PmtInvf() {}
//}}}
//{{{ class PmtWire
inline PmtWire::PmtWire(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = WIRE;
    strcpy(typeName_, "WIRE");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtWire::~PmtWire() {}
//}}}
//{{{ class PmtPull
inline PmtPull::PmtPull(const char * const name) : Pmt(name) {
    type_ = PULL;
    strcpy(typeName_, "PULL");
    // add ports
    Port *in = new Port("IN");
    in->type_ = Port::INPUT;
    addPort(in);
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtPull::~PmtPull() {}
//}}}
//{{{ class PmtTie1
inline PmtTie1::PmtTie1(const char * const name) : Pmt(name) {
    type_ = TIE1;
    strcpy(typeName_, "TIE1");
    // add ports
    Port *out = new Port("out");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTie1::~PmtTie1() {}
//}}}
//{{{ class PmtTie0
inline PmtTie0::PmtTie0(const char * const name) : Pmt(name) {
    type_ = TIE0;
    strcpy(typeName_, "TIE0");
    // add ports
    Port *out = new Port("out");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTie0::~PmtTie0() {}
//}}}
//{{{ class PmtTieX
inline PmtTieX::PmtTieX(const char * const name) : Pmt(name) {
    type_ = TIEX;
    strcpy(typeName_, "TIEX");
    // add ports
    Port *out = new Port("out");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTieX::~PmtTieX() {}
//}}}
//{{{ class PmtTieZ
inline PmtTieZ::PmtTieZ(const char * const name) : Pmt(name) {
    type_ = TIEZ;
    strcpy(typeName_, "TIEZ");
    // add ports
    Port *out = new Port("out");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtTieZ::~PmtTieZ() {}
//}}}
//{{{ class PmtUndefined
inline PmtUndefined::PmtUndefined(const char * const name, const size_t &nIn) :
  Pmt(name) {
    type_ = UNDEF;
    strcpy(typeName_, "UNDEF");
    // add ports
    char pname[NAME_LEN];
    for (size_t i = 0; i < nIn; ++i) {
        sprintf(pname, "IN%d", (int)i);
        Port *in = new Port(pname);
        in->type_ = Port::INPUT;
        addPort(in);
    }
    Port *out = new Port("OUT");
    out->type_ = Port::OUTPUT;
    addPort(out);
}

inline PmtUndefined::~PmtUndefined() {}
//}}}
//{{{ class PmtNmos
inline PmtNmos::PmtNmos(const char * const name) : Pmt(name) {
    type_ = NMOS;
    strcpy(typeName_, "NMOS");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *en = new Port("EN");
    en->type_ = Port::INPUT;
    addPort(en);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtNmos::~PmtNmos() {}
//}}}
//{{{ class PmtPmos
inline PmtPmos::PmtPmos(const char * const name) : Pmt(name) {
    type_ = PMOS;
    strcpy(typeName_, "PMOS");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *en = new Port("EN");
    en->type_ = Port::INPUT;
    addPort(en);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtPmos::~PmtPmos() {}
//}}}
//{{{ class PmtRnmos
inline PmtRnmos::PmtRnmos(const char * const name) : Pmt(name) {
    type_ = RNMOS;
    strcpy(typeName_, "RNMOS");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *en = new Port("EN");
    en->type_ = Port::INPUT;
    addPort(en);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtRnmos::~PmtRnmos() {}
//}}}
//{{{ class PmtRpmos
inline PmtRpmos::PmtRpmos(const char * const name) : Pmt(name) {
    type_ = RPMOS;
    strcpy(typeName_, "RPMOS");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *pcnt = new Port("PCNT");
    pcnt->type_ = Port::INPUT;
    addPort(pcnt);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtRpmos::~PmtRpmos() {}
//}}}
//{{{ class PmtNmosf
inline PmtNmosf::PmtNmosf(const char * const name) : Pmt(name) {
    type_ = NMOSF;
    strcpy(typeName_, "NMOSF");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtNmosf::~PmtNmosf() {}
//}}}
//{{{ class PmtPmosf
inline PmtPmosf::PmtPmosf(const char * const name) : Pmt(name) {
    type_ = PMOSF;
    strcpy(typeName_, "PMOSF");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *cnt = new Port("CNT");
    cnt->type_ = Port::INPUT;
    addPort(cnt);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtPmosf::~PmtPmosf() {}
//}}}
//{{{ class PmtCmos1
inline PmtCmos1::PmtCmos1(const char * const name) : Pmt(name) {
    type_ = CMOS1;
    strcpy(typeName_, "CMOS1");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *e = new Port("E");
    e->type_ = Port::INPUT;
    addPort(e);
    Port *p = new Port("P");
    p->type_ = Port::INPUT;
    addPort(p);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtCmos1::~PmtCmos1() {}
//}}}
//{{{ class PmtCmos2
inline PmtCmos2::PmtCmos2(const char * const name) : Pmt(name) {
    type_ = CMOS2;
    strcpy(typeName_, "CMOS2");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *e = new Port("E");
    e->type_ = Port::INPUT;
    addPort(e);
    Port *p = new Port("P");
    p->type_ = Port::INPUT;
    addPort(p);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtCmos2::~PmtCmos2() {}
//}}}
//{{{ class PmtRcmos1
inline PmtRcmos1::PmtRcmos1(const char * const name) : Pmt(name) {
    type_ = RCMOS1;
    strcpy(typeName_, "RCMOS1");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *e = new Port("E");
    e->type_ = Port::INPUT;
    addPort(e);
    Port *p = new Port("P");
    p->type_ = Port::INPUT;
    addPort(p);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtRcmos1::~PmtRcmos1() {}
//}}}
//{{{ class PmtRcmos2
inline PmtRcmos2::PmtRcmos2(const char * const name) : Pmt(name) {
    type_ = RCMOS2;
    strcpy(typeName_, "RCMOS2");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *e = new Port("E");
    e->type_ = Port::INPUT;
    addPort(e);
    Port *p = new Port("P");
    p->type_ = Port::INPUT;
    addPort(p);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtRcmos2::~PmtRcmos2() {}
//}}}
//{{{ class PmtCmos1f
inline PmtCmos1f::PmtCmos1f(const char * const name) : Pmt(name) {
    type_ = CMOS1F;
    strcpy(typeName_, "CMOS1F");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *ncnt = new Port("NCNT");
    ncnt->type_ = Port::INPUT;
    addPort(ncnt);
    Port *pcnt = new Port("PCNT");
    pcnt->type_ = Port::INPUT;
    addPort(pcnt);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtCmos1f::~PmtCmos1f() {}
//}}}
//{{{ class PmtCmos2f
inline PmtCmos2f::PmtCmos2f(const char * const name) : Pmt(name) {
    type_ = CMOS2F;
    strcpy(typeName_, "CMOS2F");
    // add ports
    Port *i = new Port("I");
    i->type_ = Port::INPUT;
    addPort(i);
    Port *ncnt = new Port("NCNT");
    ncnt->type_ = Port::INPUT;
    addPort(ncnt);
    Port *pcnt = new Port("PCNT");
    pcnt->type_ = Port::INPUT;
    addPort(pcnt);
    Port *o = new Port("O");
    o->type_ = Port::OUTPUT;
    addPort(o);
}

inline PmtCmos2f::~PmtCmos2f() {}
//}}}

};

#endif

