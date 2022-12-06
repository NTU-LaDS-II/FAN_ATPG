// **************************************************************************
// File       [ vlog2pmt.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ Flatten verilog netlist to primitives ]
// Date       [ 2011/04/19 created ]
// **************************************************************************

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "netlist_builder.h"
#include "techlib_builder.h"

using namespace std;
using namespace IntfNs;

void printCell(Cell *c, ofstream &fout);
void printPmt(Pmt *pmt, Cell *c, ofstream &fout);
void printInst(Cell *inst, Cell *c, ofstream &fout);
//void printMux(Pmt *pmt, Cell *c, ofstream &fout);
string getPmtNetName(Port *p, Pmt *pmt, Cell *c);
void printDff(Cell *c, ofstream &fout);

const char hier = '/';
//const string muxS0 = "S0";
//const string muxA = "A";
//const string muxB = "B";
//const string muxY = "Y";

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "**ERROR main(): need netlist, MDT lib";
        cerr << endl;
        exit(0);
    }

    // open output file
    /*
    ofstream fout(argv[3]);
    if (!fout.good()) {
        cerr << "**ERROR main(): file `" << argv[3] << "' cannot be opened";
        cerr << endl;
        exit(0);
    }
    */

    // read technology library
    Techlib lib;
    MdtFile *libBlder = new MdtLibBuilder(&lib);

    cout << "> Reading technology library..." << endl;
    if (!libBlder->read(argv[2], true)) {
        cerr << "**ERROR main(): MDT lib builder error" << endl;
        delete libBlder;
        exit(0);
    }
    cout << "> Checking technology library..." << endl;
    if (!lib.check(true)) {
        cerr << "**ERROR main(): MDT lib error" << endl;
        delete libBlder;
        exit(0);
    }

    // read netlist
    Netlist nl;
    nl.setTechlib(&lib);
    VlogFile *nlBlder = new VlogNlBuilder(&nl);

    cout << "> Reading netlist..." << endl;
    if (!nlBlder->read(argv[1], true)) {
        cerr << "**ERROR main(): verilog builder error" << endl;
        delete nlBlder;
        exit(0);
    }

    cout << "> Removing floating nets..." << endl;
    nl.removeFloatingNets();

    cout << "> Checking netlist..." << endl;
    if (!nl.check(true)) {
        cerr << "**ERROR main(): netlist error" << endl;
        delete nlBlder;
        exit(0);
    }

    /*
    cout << "> Writing netlist..." << endl;
    // print module
    fout << "module " << nl.getTop()->name_ << "(";
    for (size_t i = 0; i < nl.getTop()->getNPort(); ++i) {
        Port *p = nl.getTop()->getPort(i);
        fout << p->name_;
        if (i + 1 != nl.getTop()->getNPort())
            fout << ", ";
    }
    fout << ");" << endl << endl;

    // print ports
    for (size_t i = 0; i < nl.getTop()->getNPort(); ++i) {
        Port *p = nl.getTop()->getPort(i);
        switch (p->type_) {
            case Port::INPUT:
                fout << "input ";
                break;
            case Port::OUTPUT:
                fout << "output ";
                break;
            case Port::INOUT:
                fout << "inout ";
                break;
            default:
                fout << "NA ";
        }
        fout << p->name_ << ";" << endl;
    }
    fout << endl;


    // print cells
    for (size_t i = 0; i < nl.getTop()->getNCell(); ++i) {
        Cell *c = nl.getTop()->getCell(i);
        if (!lib.hasDff(c->libc_->id_) && !lib.hasDlat(c->libc_->id_))
            printCell(c, fout);
        else
            printDff(c, fout);
    }

    // print assigns
    for (size_t i = 0; i < nl.getTop()->getNNet(); ++i) {
        Net *n = nl.getTop()->getNet(i);
        if (n->getNEqv() == 0)
            continue;
        bool outNet = true;
        for (size_t j = 0; j < n->getNPort(); ++j) {
            Port *p = n->getPort(j);
            if (p->top_ == nl.getTop() && p->type_ == Port::INPUT
                || p->top_ != nl.getTop() && p->type_ == Port::OUTPUT) {
                outNet = false;
                break;
            }
        }
        if (!outNet)
            continue;
        Net *nin = NULL;
        NetSet eqv = nl.getTop()->getEqvNets(i);
        NetSet::iterator it = eqv.begin();
        for ( ; it != eqv.end(); ++it) {
            for (size_t j = 0; j < (*it)->getNPort(); ++j) {
                Port *p = (*it)->getPort(j);
                if (p->top_ == nl.getTop() && p->type_ == Port::INPUT
                    || p->top_ != nl.getTop() && p->type_ == Port::OUTPUT) {
                    nin = (*it);
                    break;
                }
            }
        }
        if (!nin)
            continue;
        fout << "assign " << n->name_ << " = " << nin->name_ << ";" << endl;
    }

    // print endmodule
    fout << endl << "endmodule" << endl << endl;
    */

    delete nlBlder;
    delete libBlder;

    return 0;
}

//{{{ void printCell()
void printCell(Cell *c, ofstream &fout) {
    Cell *libc = c->libc_;
    for (size_t i = 0; i < libc->getNCell(); ++i) {
        Cell *in = libc->getCell(i);
        if (in->isPmt_)
            printPmt((Pmt *)in, c, fout);
        else
            printInst(in, c, fout);
    }
}
//}}}
//{{{ void printPmt()
void printPmt(Pmt *pmt, Cell *c, ofstream &fout) {
    int inSize = (int)pmt->getNPort() - 1;
    char buf[8];
    switch (pmt->type_) {
        case Pmt::BUF:
            fout << "BUFX1";
            break;
        case Pmt::INV:
        case Pmt::INVF:
            fout << "INVX1";
            break;
        case Pmt::AND:
            fout << "AND";
            sprintf(buf, "%d", inSize);
            fout << buf << "X1";
            break;
        case Pmt::NAND:
            fout << "NAND";
            sprintf(buf, "%d", inSize);
            fout << buf << "X1";
            break;
        case Pmt::OR:
            fout << "OR";
            sprintf(buf, "%d", inSize);
            fout << buf << "X1";
            break;
        case Pmt::NOR:
            fout << "NOR";
            sprintf(buf, "%d", inSize);
            fout << buf << "X1";
            break;
        case Pmt::XOR:
            fout << "XOR";
            sprintf(buf, "%d", inSize);
            fout << buf << "X1";
            break;
        case Pmt::XNOR:
            fout << "XNOR";
            sprintf(buf, "%d", inSize);
            fout << buf << "X1";
            break;
        default:
            fout << "NA";
            break;
    }
    fout << " " << c->name_ << hier << pmt->name_ << " (";

    for (size_t j = 0; j < pmt->getNPort(); ++j) {
        fout << "." << pmt->getPort(j)->name_ << "(";
        fout << getPmtNetName(pmt->getPort(j), pmt, c) << ")";
        if (j + 1 != pmt->getNPort())
            fout << ", ";
    }
    fout << ");" << endl;
} //}}}
//{{{ void printInst()
void printInst(Cell *inst, Cell *c, ofstream &fout) {
}
//}}}
//{{{ string getPmtNetName()
string getPmtNetName(Port *p, Pmt *pmt, Cell *c) {
    Cell *libc = c->libc_;
    Net *n = p->exNet_;
    if (!n)
        return "";
    bool intern = true;
    string netName = "";
    for (size_t k = 0; k < n->getNPort(); ++k) {
        Port *p = n->getPort(k);
        if (p->top_ == libc) {
            if (c->getPort(p->name_)->exNet_)
                netName = c->getPort(p->name_)->exNet_->name_;
            intern = false;
            break;
        }
    }
    if (intern)
        netName = string(c->name_) + hier + string(n->name_);
    return netName;
} //}}}
//{{{ void printDff()
void printDff(Cell *c, ofstream &fout) {
    fout << c->libc_->name_ << " " << c->name_ << " (";
    bool hasPort = false;
    for (size_t i = 0; i < c->getNPort(); ++i) {
        Net *n = c->getPort(i)->exNet_;
        if (!n)
            continue;
        if (hasPort)
            fout << ", " << flush;
        fout << "." << c->getPort(i)->name_ << "(";
        fout << n->name_ << ")";
        hasPort = true;
    }
    fout << ");" << endl;
} //}}}


//{{{ void printMux()
/*
void printMux(Pmt *pmt, Cell *c, ofstream &fout) {
    string internInv = c->name_ + hier + pmt->name_ + "_inv";
    string internAnd1 = c->name_ + hier + pmt->name_ + "_and1";
    string internAnd2 = c->name_ + hier + pmt->name_ + "_and2";

    fout << "INVX1 " << c->name_ << hier << pmt->name_ << hier;
    fout << "INV (";
    fout << ".A(" << getPmtNetName(pmt->getPort(muxS0), pmt, c);
    fout << "), ";
    fout << ".Y(" << internInv << ") );" << endl;

    fout << "AND2X1 " << c->name_ << hier << pmt->name_ << hier;
    fout << "AND1 (";
    fout << ".A(" << internInv << "), ";
    fout << ".B(" << getPmtNetName(pmt->getPort(muxA), pmt, c);
    fout << "), ";
    fout << ".Y(" << internAnd1 << ") );" << endl;

    fout << "AND2X1 " << c->name_ << hier << pmt->name_ << hier;
    fout << "AND2 (";
    fout << ".A(" << getPmtNetName(pmt->getPort(muxS0), pmt, c);
    fout << "), ";
    fout << ".B(" << getPmtNetName(pmt->getPort(muxB), pmt, c);
    fout << "), ";
    fout << ".Y(" << internAnd2 << ") );" << endl;

    fout << "OR2X1 " << c->name_ << hier << pmt->name_ << hier;
    fout << "OR (";
    fout << ".A(" << internAnd1 << "), ";
    fout << ".B(" << internAnd2 << "), ";
    fout << ".Y(" << getPmtNetName(pmt->getPort(muxY), pmt, c);
    fout << ") );" << endl;

} */
//}}}

