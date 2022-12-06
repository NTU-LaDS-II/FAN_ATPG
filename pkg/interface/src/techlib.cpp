// **************************************************************************
// File       [ techlib.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ define technology library structure ]
// Date       [ 2010/12/21 created ]
// **************************************************************************

#include <iomanip>
#include <algorithm>

#include "techlib.h"

using namespace std;
using namespace IntfNs;

//{{{ bool Techlib::check()
bool Techlib::check(const bool &verbose) const {
    bool res = true;
    for (size_t i = 0; i < getNCell(); ++i) {
        Cell *c = cells_[i];
        //{{{ check ports
        for (size_t i = 0; i < c->getNPort(); ++i) {
            Port *p = c->getPort(i);
            if (p->type_ == Port::NA) {
                if (verbose) {
                    fprintf(stderr, "**WARN Techlib::check(): port ");
                    fprintf(stderr, "`%s/%s' ", c->name_, p->name_);
                    fprintf(stderr, "has no type\n");
                }
            }
            if (p->top_ != c) {
                res = false;
                if (verbose) {
                    if (!p->top_) {
                        fprintf(stderr, "**ERROR Techlib::check(): port ");
                        fprintf(stderr, "`%s/%s' ", c->name_, p->name_);
                        fprintf(stderr, "has no top cell\n");
                    }
                    else {
                        fprintf(stderr, "**ERROR Techlib::check(): port ");
                        fprintf(stderr, "`%s/%s' ", c->name_, p->name_);
                        fprintf(stderr, "has invalid top cell ");
                        fprintf(stderr, "`%s'\n", p->top_->name_);
                    }
                }
            }
            Net *n = p->inNet_;
            if (!n) {
                if (verbose) {
                    fprintf(stderr, "**WARN Techlib::check(): port ");
                    fprintf(stderr, "`%s/%s' ", c->name_, p->name_);
                    fprintf(stderr, "unconnected\n");
                }
            }
            else if (!c->getNet(n->name_)) {
                res = false;
                if (verbose) {
                    fprintf(stderr, "**ERROR Techlib::check(): port ");
                    fprintf(stderr, "`%s/%s' ", c->name_, p->name_);
                    fprintf(stderr, "connected to invalid net ");
                    fprintf(stderr, "`%s'\n", n->name_);
                }
            }
        } //}}}
        //{{{ check nets
        for (size_t i = 0; i < c->getNNet(); ++i) {
            Net *n = c->getNet(i);
            if (n->getNPort() < 2) {
                res = false;
                if (verbose) {
                    fprintf(stderr, "**ERROR Techlib::check(): net ");
                    fprintf(stderr, "`%s/%s' ", c->name_, n->name_);
                    fprintf(stderr, "is floating\n");
                }
            }
            if (n->top_ != c) {
                res = false;
                if (verbose) {
                    if (!n->top_) {
                        fprintf(stderr, "**ERROR Techlib::check(): net ");
                        fprintf(stderr, "`%s/%s' ", c->name_, n->name_);
                        fprintf(stderr, "has no top cell\n");
                    }
                    else {
                        fprintf(stderr, "**ERROR Techlib::check(): net ");
                        fprintf(stderr, "`%s/%s' ", c->name_, n->name_);
                        fprintf(stderr, "has invalid top cell ");
                        fprintf(stderr, "`%s'\n", n->top_->name_);
                    }
                }
            }
            for (size_t j = 0; j < n->getNPort(); ++j) {
                Port *p = n->getPort(j);
                if ((p->top_ != n->top_ && p->exNet_ != n)
                     || (p->top_ == n->top_ && p->inNet_ != n)) {
                    res = false;
                    if (verbose) {
                        fprintf(stderr, "**ERROR Techlib::check(): net ");
                        fprintf(stderr, "`%s/%s' ", c->name_, n->name_);
                        fprintf(stderr, "not connected to port ");
                        fprintf(stderr, "`%s/%s'\n", c->name_, p->name_);
                    }
                }
            }
        } //}}}
        //{{{ check primitives
        for (size_t i = 0; i < c->getNCell(); ++i) {
            Cell *pmt = c->getCell(i);
            if (pmt->top_ != c) {
                res = false;
                if (verbose) {
                    if (!pmt->top_) {
                        fprintf(stderr, "**ERROR Techlib::check(): ");
                        fprintf(stderr, "primitive `%s/", c->name_);
                        fprintf(stderr, "%s' ", pmt->name_);
                        fprintf(stderr, "has no top cell\n");
                    }
                    else {
                        fprintf(stderr, "**ERROR Techlib::check(): ");
                        fprintf(stderr, "primitive `%s/", c->name_);
                        fprintf(stderr, "%s' ", pmt->name_);
                        fprintf(stderr, "has invalid top cell ");
                        fprintf(stderr, "`%s'\n", pmt->top_->name_);
                    }
                }
            }
            for (size_t j = 0; j < pmt->getNPort(); ++j) {
                Port *p = pmt->getPort(j);
                if (!p->exNet_) {
                    if (verbose) {
                        fprintf(stderr, "**WARN Techlib::check(): port ");
                        fprintf(stderr, "`%s/%s", c->name_, pmt->name_);
                        fprintf(stderr, "/%s' unconnected\n", p->name_);
                    }
                }
                else if (p->exNet_->top_ != c) {
                    res = false;
                    if (verbose) {
                        fprintf(stderr, "**ERROR Techlib::check(): port ");
                        fprintf(stderr, "`%s/%s", c->name_, pmt->name_);
                        fprintf(stderr, "/%s' ", p->name_);
                        fprintf(stderr, "connected to invalid net ");
                        fprintf(stderr, "`%s'\n", p->exNet_->name_);
                    }
                }
            }
        }
        //}}}
    }

    return res;
} //}}}
//{{{ bool Techlib::levelize()
bool Techlib::levelize() {
    size_t maxNPmt = 0;
    for (size_t i = 0; i < cells_.size(); ++i)
        if (cells_[i]->getNCell() > maxNPmt)
            maxNPmt = cells_[i]->getNCell();

    bool processed[maxNPmt];
    bool levelized[maxNPmt];

    for (size_t i = 0; i < cells_.size(); ++i) {
        if (hasPmt(i, Pmt::DFF) || hasPmt(i, Pmt::DLAT))
            continue;
        Cell *top = cells_[i];

        for (size_t j = 0; j < maxNPmt; ++j) {
            processed[j] = false;
            levelized[j] = false;
        }

        queue<Cell *> cque;

        // process input cells
        for (size_t j = 0; j < top->getNPort(); ++j) {
            Port *p = top->getPort(j);
            if (p->type_ != Port::INPUT)
                continue;
            CellSet cells = top->getPortCells(j);
            CellSet::iterator it = cells.begin();
            for ( ; it != cells.end(); ++it) {
                if (processed[(*it)->id_])
                    continue;
                processed[(*it)->id_] = true;
                cque.push((*it));
            }
        }

        // process cells with no inputs
        for (size_t i = 0; i < top->getNCell(); ++i) {
            Cell *c = top->getCell(i);
            bool hasInput = false;
            for (size_t j = 0; j < c->getNPort(); ++j) {
                if (c->getPort(j)->type_ == Port::INPUT) {
                    hasInput = true;
                    break;
                }
            }
            if (hasInput)
                continue;
            c->lvl_ = 0;
            processed[i] = true;
            levelized[i] = true;
            CellSet cells = top->getFanout(i);
            for (CellSet::iterator it = cells.begin(); it != cells.end();
                 ++it) {
                if (processed[(*it)->id_])
                    continue;
                processed[(*it)->id_] = true;
                cque.push((*it));
            }
        }

        while (!cque.empty()) {
            Cell *c = cque.front();
            cque.pop();

            // check if cell is ready to process
            int maxLvl = 0;
            bool ready = true;
            CellSet fis = top->getFanin(c->id_);
            for (CellSet::iterator it = fis.begin(); it != fis.end(); ++it) {
                if (!levelized[(*it)->id_]) {
                    ready = false;
                    break;
                }
                if ((*it)->lvl_ > maxLvl)
                    maxLvl = (*it)->lvl_;
            }
            if (!ready) {
                cque.push(c);
                continue;
            }

            // set cell level
            c->lvl_ = maxLvl + 1;
            levelized[c->id_] = true;

            // put fanout into the queue
            CellSet fos = top->getFanout(c->id_);
            for (CellSet::iterator it = fos.begin(); it != fos.end(); ++it) {
                if (processed[(*it)->id_])
                    continue;
                cque.push((*it));
                processed[(*it)->id_] = true;
            }
        }

        // sort the vector
        stable_sort(top->getCells()->begin(), top->getCells()->end(),
                    cmpCellLvl);

        // reassign ID
        for (size_t i = 0; i < top->getNCell(); ++i)
            top->getCell(i)->id_ = i;

    }

    return true;
} //}}}

