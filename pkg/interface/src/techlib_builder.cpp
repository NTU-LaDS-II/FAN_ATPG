// **************************************************************************
// File       [ techlib_builder.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/06/25 created ]
// **************************************************************************

#include "techlib_builder.h"

using namespace std;
using namespace IntfNs;

//{{{ bool MdtLibBuilder::read()
bool MdtLibBuilder::read(const char * const fname, const bool &verbose) {
    if (!MdtFile::read(fname, verbose))
        return false;
    if (!checkInstances())
        return false;
    removeFloatingNets();
    return true;
} //}}}
//{{{ void MdtLibBuilder::addModel()
void MdtLibBuilder::addModel(const char * const name) {
    if (!success_)
        return;
    if (lib_->getCell(name)) {
        if (verbose_) {
            fprintf(stderr, "**ERROR MdtLibBuilder::addModel(): cell ");
            fprintf(stderr, "`%s' already exists\n", name);
        }
        success_ = false;
        return;
    }
    curCell_ = new Cell(name);
    lib_->addCell(curCell_);
} //}}}
//{{{ void MdtLibBuilder::addPorts()
void MdtLibBuilder::addPorts(MdtNames * const ports) {
    if (!success_)
        return;
    MdtNames *port = ports->head;
    while (port) {
        Port *p = new Port(port->name);
        p->top_ = curCell_;
        if (!curCell_->addPort(p)) {
            if (verbose_) {
                fprintf(stderr, "**ERROR MdtLibBuilder::addPorts(): port ");
                fprintf(stderr, "`%s/%s' ", curCell_->name_, port->name);
                fprintf(stderr, "already exists\n");
            }
            success_ = false;
            return;
        }
        port = port->next;
    }
} //}}}
//{{{ void MdtLibBuilder::setInputNets()
void MdtLibBuilder::setInputNets(MdtNames * const nets) {
    if (!success_)
        return;
    if (!addPortNets(nets)) {
        success_ = false;
        return;
    }
    MdtNames *net = nets->head;
    while (net) {
        Port *p = curCell_->getPort(net->name);
        p->type_ = Port::INPUT;
        net = net->next;
    }
} //}}}
//{{{ void MdtLibBuilder::setOutputNets()
void MdtLibBuilder::setOutputNets(MdtNames * const nets) {
    if (!success_)
        return;
    if (!addPortNets(nets)) {
        success_ = false;
        return;
    }
    MdtNames *net = nets->head;
    while (net) {
        Port *p = curCell_->getPort(net->name);
        p->type_ = Port::OUTPUT;
        net = net->next;
    }
} //}}}
//{{{ void MdtLibBuilder::setInoutNets()
void MdtLibBuilder::setInoutNets(MdtNames * const nets) {
    if (!success_)
        return;
    if (!addPortNets(nets)) {
        success_ = false;
        return;
    }
    MdtNames *net = nets->head;
    while (net) {
        Port *p = curCell_->getPort(net->name);
        p->type_ = Port::OUTPUT;
        net = net->next;
    }
} //}}}
//{{{ void MdtLibBuilder::setInternNets()
void MdtLibBuilder::setInternNets(MdtNames * const nets) {
    if (!success_)
        return;
    MdtNames *net = nets->head;
    while (net) {
        Net *n = new Net(net->name);
        n->top_ = curCell_;
        if (!curCell_->addNet(n)) {
            if (verbose_) {
                fprintf(stderr, "**ERROR MdtLibBuilder::setInternNets(): ");
                fprintf(stderr, "net `%s/%s' ", curCell_->name_, net->name);
                fprintf(stderr, "already exists\n");
            }
            success_ = false;
            return;
        }
        net = net->next;
    }
} //}}}
//{{{ void MdtLibBuilder::addPrimitive()
void MdtLibBuilder::addPrimitive(const char * const type,
                                 const char * const name,
                                 MdtNames * const ports) {
    if (!success_)
        return;
    char cname[NAME_LEN];
    if (!strcmp(name, ""))
        sprintf(cname, "LOO%d", (int)curCell_->getNCell());
    else
        strcpy(cname, name);
    if (curCell_->getCell(cname)) {
        if (verbose_) {
            fprintf(stderr, "**ERROR MdtLibBuilder::addPrimitive(): ");
            fprintf(stderr, "primitive `%s/%s' ", curCell_->name_, cname);
            fprintf(stderr,  "already exists\n");
        }
        success_ = false;
        return;
    }
    MdtNames *port = ports->head;
    size_t portSize = 0;
    while (port) {
        portSize++;
        port = port->next;
    }

    Pmt *pmt = NULL;
    if (!strcmp(type, "_and"))
        pmt = new PmtAnd(cname, portSize - 1);
    else if (!strcmp(type, "_nand"))
        pmt = new PmtNand(cname, portSize - 1);
    else if (!strcmp(type, "_or"))
        pmt = new PmtOr(cname, portSize - 1);
    else if (!strcmp(type, "_nor"))
        pmt = new PmtNor(cname, portSize - 1);
    else if (!strcmp(type, "_inv"))
        pmt = new PmtInv(cname);
    else if (!strcmp(type, "_buf"))
        pmt = new PmtBuf(cname);
    else if (!strcmp(type, "_bufz"))
        pmt = new PmtBufz(cname);
    else if (!strcmp(type, "_xor"))
        pmt = new PmtXor(cname, portSize - 1);
    else if (!strcmp(type, "_xnor"))
        pmt = new PmtXnor(cname, portSize - 1);
    else if (!strcmp(type, "_tsl"))
        pmt = new PmtTsl(cname);
    else if (!strcmp(type, "_tsli"))
        pmt = new PmtTsli(cname);
    else if (!strcmp(type, "_tsh"))
        pmt = new PmtTsh(cname);
    else if (!strcmp(type, "_tshi"))
        pmt = new PmtTshi(cname);
    else if (!strcmp(type, "_mux"))
        pmt = new PmtMux(cname);
    else if (!strcmp(type, "_dff"))
        pmt = new PmtDff(cname, (portSize - 4)/2);
    else if (!strcmp(type, "_dlat"))
        pmt = new PmtDlat(cname, (portSize - 4)/2);
    else if (!strcmp(type, "_delay"))
        pmt = new PmtDelay(cname);
    else if (!strcmp(type, "_invf"))
        pmt = new PmtInvf(cname);
    else if (!strcmp(type, "_wire"))
        pmt = new PmtWire(cname, portSize - 1);
    else if (!strcmp(type, "_pull"))
        pmt = new PmtPull(cname);
    else if (!strcmp(type, "_tie1"))
        pmt = new PmtTie1(cname);
    else if (!strcmp(type, "_tie0"))
        pmt = new PmtTie0(cname);
    else if (!strcmp(type, "_tiex"))
        pmt = new PmtTieX(cname);
    else if (!strcmp(type, "_tiez"))
        pmt = new PmtTieZ(cname);
    else if (!strcmp(type, "_undefined"))
        pmt = new PmtUndefined(cname, portSize - 1);
    else if (!strcmp(type, "_nmos"))
        pmt = new PmtNmos(cname);
    else if (!strcmp(type, "_pmos"))
        pmt = new PmtPmos(cname);
    else if (!strcmp(type, "_rnmos"))
        pmt = new PmtRnmos(cname);
    else if (!strcmp(type, "_rpmos"))
        pmt = new PmtRpmos(cname);
    else if (!strcmp(type, "_nmosf"))
        pmt = new PmtNmosf(cname);
    else if (!strcmp(type, "_pmosf"))
        pmt = new PmtPmosf(cname);
    else if (!strcmp(type, "_cmos1"))
        pmt = new PmtCmos1(cname);
    else if (!strcmp(type, "_cmos2"))
        pmt = new PmtCmos2(cname);
    else if (!strcmp(type, "_rcmos1"))
        pmt = new PmtRcmos1(cname);
    else if (!strcmp(type, "_rcmos2"))
        pmt = new PmtRcmos2(cname);
    else if (!strcmp(type, "_cmos1f"))
        pmt = new PmtCmos1f(cname);
    else if (!strcmp(type, "_cmos2f"))
        pmt = new PmtCmos2f(cname);

    if (!pmt) {
        if (verbose_) {
            fprintf(stderr, "**ERROR MdtLibBuilder::addPrimitive(): ");
            fprintf(stderr, "invalid primitive `%s/", curCell_->name_);
            fprintf(stderr, "%s'\n", cname);
        }
        success_ = false;
        return;
    }
    pmt->top_ = curCell_;

    if (portSize != pmt->getNPort()) {
        if (verbose_) {
            fprintf(stderr, "**ERROR MdtLibBuilder::addPrimitive(): ");
            fprintf(stderr, "primitive `%s/%s' ", curCell_->name_, cname);
            fprintf(stderr, "has incorrect port size\n");
        }
        success_ = false;
        return;
    }

    curCell_->addCell(pmt);
    port = ports->head;
    size_t portNo = 0;
    while (port) {
        if (strlen(port->name) == 0) {
            port = port->next;
            portNo++;
            continue;
        }
        Port *p = pmt->getPort(portNo);

        Net *n = curCell_->getNet(port->name);
        if (!n) {
            if (verbose_) {
                fprintf(stderr, "**WARN MdtLibBuilder::addPrimitive(): net ");
                fprintf(stderr, "`%s/%s' ", curCell_->name_, port->name);
                fprintf(stderr, "set as wire\n");
            }
            n = new Net(port->name);
            n->top_ = curCell_;
            curCell_->addNet(n);
        }
        p->exNet_ = n;
        n->addPort(p);

        port = port->next;
        portNo++;
    }
} //}}}
//{{{ void MdtLibBuilder::addInstance()
void MdtLibBuilder::addInstance(const char * const type,
                                const char * const name,
                                MdtNames * const ports) {
    if (!success_)
        return;
    char cname[NAME_LEN];
    if (!strcmp(name, ""))
        sprintf(cname, "LOO%d", (int)curCell_->getNCell());
    else
        strcpy(cname, name);
    if (curCell_->getCell(cname)) {
        if (verbose_) {
            fprintf(stderr, "**ERROR MdtLibBuilder::addPrimitive(): ");
            fprintf(stderr, "primitive `%s/%s' ", curCell_->name_, cname);
            fprintf(stderr, "already exists\n");
        }
        success_ = false;
        return;
    }
    Cell *c = new Cell(cname);
    c->top_ = curCell_;
    strcpy(c->typeName_, type);
    curCell_->addCell(c);

    MdtNames *port = ports->head;
    size_t portNo = 0;
    char pname[NAME_LEN];
    while (port) {
        sprintf(pname, "P%d", (int)portNo);
        Port *p = new Port(pname);
        p->top_ = c;
        c->addPort(p);

        if (strlen(port->name) == 0) {
            port = port->next;
            portNo++;
            continue;
        }

        Net *n = curCell_->getNet(port->name);
        if (!n) {
            if (verbose_) {
                fprintf(stderr, "**WARN MdtLibBuilder::addPrimitive(): net ");
                fprintf(stderr, "`%s/%s' ", curCell_->name_, port->name);
                fprintf(stderr, "set as wire\n");
            }
            n = new Net(port->name);
            n->top_ = curCell_;
            curCell_->addNet(n);
        }
        p->exNet_ = n;
        n->addPort(p);

        port = port->next;
        portNo++;
    }
} //}}}
//{{{ void MdtLibBuilder::addInstance()
void MdtLibBuilder::addInstance(const char * const type,
                                const char * const name,
                                MdtPortToNet * const portToNet) {
    if (!success_)
        return;
    char cname[NAME_LEN];
    if (!strcmp(name, ""))
        sprintf(cname, "LOO%d", (int)curCell_->getNCell());
    else
        strcpy(cname, name);
    if (curCell_->getCell(cname)) {
        if (verbose_) {
            fprintf(stderr, "**ERROR MdtLibBuilder::addPrimitive(): ");
            fprintf(stderr, "primitive `%s/%s' ", curCell_->name_, cname);
            fprintf(stderr, "already exists\n");
        }
        success_ = false;
        return;
    }
    Cell *c = new Cell(cname);
    c->top_ = curCell_;
    strcpy(c->typeName_, type);
    curCell_->addCell(c);

    MdtPortToNet *p2n = portToNet->head;
    while (p2n) {
        Port *p = new Port(p2n->port);
        p->top_ = c;
        c->addPort(p);

        Net *n = curCell_->getNet(p2n->net);
        if (!n) {
            if (verbose_) {
                fprintf(stderr, "**WARN MdtLibBuilder::addPrimitive(): net ");
                fprintf(stderr, "`%s/%s' ", curCell_->name_, p2n->net);
                fprintf(stderr, "set as wire\n");
            }
            n = new Net(p2n->net);
            n->top_ = curCell_;
            curCell_->addNet(n);
        }
        p->exNet_ = n;
        n->addPort(p);

        p2n = p2n->next;
    }
} //}}}
//{{{ bool MdtLibBuilder::addPortNets()
bool MdtLibBuilder::addPortNets(MdtNames * const nets) {
    MdtNames *net = nets->head;
    while (net) {
        Port *p = curCell_->getPort(net->name);
        if (!p) {
            if (verbose_) {
                fprintf(stderr, "**ERROR MdtLibBuilder::addPortNets(): ");
                fprintf(stderr, "port `%s/%s' ", curCell_->name_, net->name);
                fprintf(stderr, "does not exist\n");
            }
            return false;
        }
        Net *n = curCell_->getNet(net->name);
        if (!n) {
            n = new Net(net->name);
            n->top_ = curCell_;
            curCell_->addNet(n);
        }
        n->addPort(p);
        p->inNet_ = n;
        net = net->next;
    }
    return true;
} //}}}
//{{{ bool MdtLibBuilder::checkInstances()
bool MdtLibBuilder::checkInstances() {
    for (size_t i = 0; i < lib_->getNCell(); ++i) {
        Cell *c = lib_->getCell(i);
        for (size_t j = 0; j < c->getNCell(); ++j) {
            Cell *inst = c->getCell(j);
            if (inst->isPmt_)
                continue;
            Cell *instc = lib_->getCell(inst->typeName_);
            if (!instc) {
                fprintf(stderr, "**ERROR MdtLibBuilder::checkInstances(): ");
                fprintf(stderr, "instance `%s' ", inst->name_);
                fprintf(stderr, "is not in the library\n");
                return false;
            }
            inst->libc_ = instc;
            Net *nets[instc->getNPort()];
            for (size_t k = 0; k < inst->getNPort(); ++k) {
                Port *p = inst->getPort(k);
                if (instc->getPort(p->name_))
                    nets[instc->getPort(p->name_)->id_] = p->exNet_;
                else
                    nets[k] = p->exNet_;
                if (p->exNet_)
                    p->exNet_->removePort(p);
                delete p;
            }
            inst->clearPorts();
            for (size_t k = 0; k < instc->getNPort(); ++k) {
                Port *p = new Port(instc->getPort(k)->name_);
                p->top_ = c;
                p->type_ = instc->getPort(k)->type_;
                inst->addPort(p);
                p->exNet_ = nets[k];
                nets[k]->addPort(p);
            }
        }
    }
    return true;
} //}}}
//{{{ bool MdtLibBuilder::removeFloatingNets()
bool MdtLibBuilder::removeFloatingNets() {
    for (size_t i = 0; i < lib_->getNCell(); ++i) {
        Cell *c = lib_->getCell(i);
        for (size_t j = 0; j < c->getNNet(); ++j) {
            Net *n = c->getNet(j);
            if (n->getNPort() < 2 && n->getNEqv() == 0) {
                if (n->getNPort() == 1) {
                    Port *p = n->getPort(0);
                    if (p->top_ == c)
                        p->inNet_ = NULL;
                    else
                        p->exNet_ = NULL;
                }
                c->removeNet(j);
                delete n;
                j--;
            }
        }
    }
    return true;
} //}}}

