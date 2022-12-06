// **************************************************************************
// File       [ pattern_rw.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/10 created ]
// **************************************************************************

#include "pattern_rw.h"
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;
using namespace IntfNs;
using namespace CoreNs;

// PatternReader
//{{{ void PatternReader::setPiOrder(const PatNames * const)
// this method map the PI order to the circuit order
// 
void PatternReader::setPiOrder(const PatNames * const pis) {
    if (!success_)
        return;
    if (!cir_ || !cir_->nl_) {
        success_ = false;
        return;
    }
    pcoll_->npi_ = 0;
    PatNames *pi = pis->head;
    while (pi) {
        pcoll_->npi_++;
        pi = pi->next;
    }
    delete [] pcoll_->piOrder_;
    pcoll_->piOrder_ = new int[pcoll_->npi_];
    pi = pis->head;
    int i = 0;
    while (pi) {
        Port *p = cir_->nl_->getTop()->getPort(pi->name);
        if (!p) {
            fprintf(stderr, "**ERROR PatternReader::setPiOrder(): port ");
            fprintf(stderr, "`%s' not found\n", pi->name);
            success_ = false;
            delete [] pcoll_->piOrder_;
            pcoll_->piOrder_ = NULL;
            return;
        }
        pcoll_->piOrder_[i] = cir_->portToGate_[p->id_];
        i++;
        pi = pi->next;
    }
} //}}}
//{{{ void PatternReader::setPpiOrder(const PatNames * const)
void PatternReader::setPpiOrder(const PatNames * const ppis) {
    if (!success_)
        return;
    if (!cir_ || !cir_->nl_) {
        success_ = false;
        return;
    }
    pcoll_->nppi_ = 0;
    PatNames *ppi = ppis->head;
    while (ppi) {
        pcoll_->nppi_++;
        ppi = ppi->next;
    }
    delete [] pcoll_->ppiOrder_;
    pcoll_->ppiOrder_ = new int[pcoll_->nppi_];
    ppi = ppis->head;
    int i = 0;
    while (ppi) {
        Cell *c = cir_->nl_->getTop()->getCell(ppi->name);
        if (!c) {
            fprintf(stderr, "**ERROR PatternReader::setPpiOrder(): gate ");
            fprintf(stderr, "`%s' not found\n", ppi->name);
            success_ = false;
            delete [] pcoll_->ppiOrder_;
            pcoll_->ppiOrder_ = NULL;
            return;
        }
        pcoll_->ppiOrder_[i] = cir_->cellToGate_[c->id_];
        i++;
        ppi = ppi->next;
    }
} //}}}
//{{{ void PatternReader::setPoOrder(const PatNames * const)
void PatternReader::setPoOrder(const PatNames * const pos) {
    if (!success_)
        return;
    if (!cir_ || !cir_->nl_) {
        success_ = false;
        return;
    }
    pcoll_->npo_ = 0;
    PatNames *po = pos->head;
    while (po) {
        pcoll_->npo_++;
        po = po->next;
    }
    delete [] pcoll_->poOrder_;
    pcoll_->poOrder_ = new int[pcoll_->npo_];
    po = pos->head;
    int i = 0;
    while (po) {
        Port *p = cir_->nl_->getTop()->getPort(po->name);
        if (!p) {
            fprintf(stderr, "**ERROR PatternReader::setPoOrder(): port ");
            fprintf(stderr, "`%s' not found\n", po->name);
            success_ = false;
            delete [] pcoll_->poOrder_;
            pcoll_->poOrder_ = NULL;
            return;
        }
        pcoll_->poOrder_[i] = cir_->portToGate_[p->id_];
        i++;
        po = po->next;
    }
} //}}}
//{{{ void PatternReader::setPatternType(const PatType &)
void PatternReader::setPatternType(const PatType &type) {
    if (!success_)
        return;
    switch (type) {
        case IntfNs::BASIC_SCAN:
            pcoll_->type_ = PatternProcessor::BASIC_SCAN;
            break;
        case IntfNs::LAUNCH_CAPTURE:
            pcoll_->type_ = PatternProcessor::LAUNCH_CAPTURE;
            break;
        case IntfNs::LAUNCH_SHIFT:
            pcoll_->type_ = PatternProcessor::LAUNCH_SHIFT;
            pcoll_->nsi_ = 1;
            break;
    }
} //}}}
//{{{ void PatternReader::setPatternNum(const int &)
void PatternReader::setPatternNum(const int &num) {
    if (!success_)
        return;
    pcoll_->pats_.resize((size_t)num);
    Pattern *pats = new Pattern[pcoll_->pats_.size()];
    for (size_t i = 0; i < pcoll_->pats_.size(); ++i)
        pcoll_->pats_[i] = &pats[i];
    curPat_ = 0;
} //}}}
//{{{ void PatternReader::addPattern()
// read in a pattern
void PatternReader::addPattern(const char * const pi1,
                                const char * const pi2,
                                const char * const ppi,
                                const char * const si,
                                const char * const po1,
                                const char * const po2,
                                const char * const ppo) {
    if (!success_)
        return;
    if (pi1 && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->pi1_ = new Value[pcoll_->npi_];
        assignValue(pcoll_->pats_[curPat_]->pi1_, pi1, pcoll_->npi_);
    }
    if (pi2 && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->pi2_ = new Value[pcoll_->npi_];
        assignValue(pcoll_->pats_[curPat_]->pi2_, pi2, pcoll_->npi_);
    }
    if (ppi && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->ppi_ = new Value[pcoll_->nppi_];
        assignValue(pcoll_->pats_[curPat_]->ppi_, ppi, pcoll_->nppi_);
    }
    if (si && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->si_ = new Value[pcoll_->nsi_];
        assignValue(pcoll_->pats_[curPat_]->si_, si, pcoll_->nsi_);
    }
    if (po1 && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->po1_ = new Value[pcoll_->npo_];
        assignValue(pcoll_->pats_[curPat_]->po1_, po1, pcoll_->npo_);
    }
    if (po2 && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->po2_ = new Value[pcoll_->npo_];
        assignValue(pcoll_->pats_[curPat_]->po2_, po2, pcoll_->npo_);
    }
    if (ppo && curPat_ < (int)pcoll_->pats_.size()) {
        pcoll_->pats_[curPat_]->ppo_ = new Value[pcoll_->nppi_];
        assignValue(pcoll_->pats_[curPat_]->ppo_, ppo, pcoll_->nppi_);
    }
    curPat_++;
} //}}}
//{{{ void PatternReader::assignValue()
void PatternReader::assignValue(Value *v, const char * const pat,
                                 const int &size) {
    for (int i = 0; i < size; ++i) {
        if (pat[i] == '0')
            v[i] = L;
        else if (pat[i] == '1')
            v[i] = H;
        else
            v[i] = X;
    }
} //}}}

// PatternWriter
//{{{ bool PatternWriter::writePat(const char * const)

// write to LaDS's own *.pat  pattern format 
// support 2 time frames, but no more than 2 time frames
//
bool PatternWriter::writePat(const char * const fname) {
    FILE *fout = fopen(fname, "w");
    if (!fout) {
        fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
        fprintf(stderr, "`%s' cannot be opened\n", fname);
        return false;
    }

    for (int i = 0; i < cir_->npi_; ++i) {
        fprintf(fout, "%s ",
                cir_->nl_->getTop()->getPort(cir_->gates_[i].cid_)->name_);
    }
    fprintf(fout, " |\n");

    for (int i = cir_->npi_; i < cir_->npi_ + cir_->nppi_; ++i) {
        fprintf(fout, "%s ",
                cir_->nl_->getTop()->getCell(cir_->gates_[i].cid_)->name_);
    }
    fprintf(fout, " |\n");

    int start = cir_->npi_ + cir_->nppi_ + cir_->ncomb_;
    for (int i = start; i < start + cir_->npo_; ++i) {
        fprintf(fout, "%s ",
                cir_->nl_->getTop()->getPort(cir_->gates_[i].cid_)->name_);
    }
    fprintf(fout, "\n");

    switch (pcoll_->type_) {
        case PatternProcessor::BASIC_SCAN:
            fprintf(fout, "BASIC_SCAN\n");
            break;
        case PatternProcessor::LAUNCH_CAPTURE:
            fprintf(fout, "LAUNCH_ON_CAPTURE\n");
            break;
        case PatternProcessor::LAUNCH_SHIFT:
            fprintf(fout, "LAUNCH_ON_SHIFT\n");
            break;
    }

    fprintf(fout, "_num_of_pattern_%d\n", (int)pcoll_->pats_.size());

    for (int i = 0; i < (int)pcoll_->pats_.size(); ++i) {
        fprintf(fout, "_pattern_%d ", i + 1);
        if (pcoll_->pats_[i]->pi1_) {
            for (int j = 0; j < pcoll_->npi_; ++j) {
                if (pcoll_->pats_[i]->pi1_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->pi1_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->pi2_) {
            for (int j = 0; j < pcoll_->npi_; ++j) {
                if (pcoll_->pats_[i]->pi2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->pi2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->ppi_) {
            for (int j = 0; j < pcoll_->nppi_; ++j) {
                if (pcoll_->pats_[i]->ppi_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->ppi_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");    
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->si_) {
            for (int j = 0; j < pcoll_->nsi_; ++j) {
                if (pcoll_->pats_[i]->si_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->si_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->po1_) {
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po1_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po1_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->po2_) {
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->ppi_) {
            for (int j = 0; j < pcoll_->nppi_; ++j) {
                if (pcoll_->pats_[i]->ppo_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->ppo_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n");
    fclose(fout);

    return true;
} //}}}

//{{{ bool PatternWriter::writeLht(const char * const)

// write to Ling Hsio-Ting's pattern format
// This format is OBSOLETE, no longer supported
//  
bool PatternWriter::writeLht(const char * const fname) {
    FILE *fout = fopen(fname, "w");
    if (!fout) {
        fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
        fprintf(stderr, "`%s' cannot be opened\n", fname);
        return false;
    }

    for (size_t i = 0; i < pcoll_->pats_.size(); ++i) {
        fprintf(fout, "%d: ", (int)i + 1);
        if (pcoll_->pats_[i]->pi1_) {
            for (int j = 0; j < pcoll_->npi_; ++j) {
                if (pcoll_->pats_[i]->pi1_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->pi1_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        if (pcoll_->pats_[i]->pi2_) {
            fprintf(fout, "->");
            for (int j = 0; j < pcoll_->npi_; ++j) {
                if (pcoll_->pats_[i]->pi2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->pi2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, "_");
        if (pcoll_->pats_[i]->ppi_) {
            fprintf(fout, "->");
            for (int j = 0; j < pcoll_->nppi_; ++j) {
                if (pcoll_->pats_[i]->ppi_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->ppi_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        if (pcoll_->pats_[i]->si_) {
            fprintf(fout, "@");
            for (int j = 0; j < pcoll_->nsi_; ++j) {
                if (pcoll_->pats_[i]->si_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->si_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, " | ");
        if (pcoll_->pats_[i]->po1_
            && pcoll_->type_ == PatternProcessor::BASIC_SCAN) {
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po1_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po1_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        if (pcoll_->pats_[i]->po2_
            && (pcoll_->type_ == PatternProcessor::LAUNCH_CAPTURE
                ||pcoll_->type_ == PatternProcessor::LAUNCH_SHIFT)) {
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, "_");
        if (pcoll_->pats_[i]->ppo_) {
            fprintf(fout, "->");
            for (int j = 0; j < pcoll_->nppi_; ++j) {
                if (pcoll_->pats_[i]->ppo_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->ppo_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n");
    fclose(fout);

    return true;
} //}}}

// write to Mentor ASCii
// E.5 problem
// must test with mentor fastscan
bool PatternWriter::writeAscii(const char * const fname) {
    FILE *fout = fopen(fname, "w");
    int first_flag = 1;
    int seqCircuitCheck = 0;
    if (!fout) {
        fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
        fprintf(stderr, "`%s' cannot be opened\n", fname);
        return false;
    }
    fprintf(fout, "ASCII_PATTERN_FILE_VERSION = 2;\n");
    fprintf(fout, "SETUP =\n");

    // input
    fprintf(fout, "declare input bus \"PI\" = ");
    //fprintf(fout, "\"/CK\", \"/test_si\", \"/test_se\"");
    for (size_t i = 0; i < cir_->nl_->getTop()->getNPort(); ++i) {
        Port *p = cir_->nl_->getTop()->getPort(i);
        if (p->type_ != Port::INPUT) continue;
        if (first_flag) {
            fprintf(fout, "\"/%s\"",
                    cir_->nl_->getTop()->getPort(i)->name_);
            first_flag = 0;
        } else { 
            fprintf(fout, ", \"/%s\"",
                    cir_->nl_->getTop()->getPort(i)->name_);
        }
        if (!strcmp(p->name_,"CK"))
            seqCircuitCheck = 1;
    }
    fprintf(fout, ";\n");
    first_flag = 1;
    // output
    fprintf(fout, "declare output bus \"PO\" = "); 
    for (size_t i = 0; i < cir_->nl_->getTop()->getNPort(); ++i) {
        Port *p = cir_->nl_->getTop()->getPort(i);
        if (p->type_ != Port::OUTPUT) continue;
        if (first_flag) {
            fprintf(fout, "\"/%s\"",
                    cir_->nl_->getTop()->getPort(i)->name_);
            first_flag = 0;
        } else { 
            fprintf(fout, ", \"/%s\"",
                    cir_->nl_->getTop()->getPort(i)->name_);
        }
    }
    fprintf(fout, ";\n");

    // clock
    if (seqCircuitCheck) {
        fprintf(fout, "clock \"/CK\" =\n");
        fprintf(fout, "    off_state = 0;\n");
        fprintf(fout, "    pulse_width = 1;\n");
        fprintf(fout, "end;\n");
    
        // test setup
        fprintf(fout, "procedure test_setup \"test_setup\" =\n");
        fprintf(fout, "    force \"/CK\" 0 0;\n");
        fprintf(fout, "    force \"/test_si\" 0 0;\n");
        fprintf(fout, "    force \"/test_se\" 0 0;\n");
        fprintf(fout, "end;\n");

        // scan group
        fprintf(fout, "scan_group \"group1\" =\n");
        fprintf(fout, "    scan_chain \"chain1\" =\n");
        fprintf(fout, "    scan_in = \"/test_si\";\n");
        fprintf(fout, "    scan_out = \"/test_so\";\n");
        fprintf(fout, "    length = %d;\n", cir_->nppi_);
        fprintf(fout, "    end;\n");

        fprintf(fout, "    procedure shift \"group1_load_shift\" =\n");
        fprintf(fout, "    force_sci \"chain1\" 0;\n");
        fprintf(fout, "    force \"/CK\" 1 16;\n");
        fprintf(fout, "    force \"/CK\" 0 19;\n");
        fprintf(fout, "    period 32;\n");
        fprintf(fout, "    end;\n");

        fprintf(fout, "    procedure shift \"group1_unload_shift\" =\n");
        fprintf(fout, "    measure_sco \"chain1\" 0;\n");
        fprintf(fout, "    force \"/CK\" 1 16;\n");
        fprintf(fout, "    force \"/CK\" 0 19;\n");
        fprintf(fout, "    period 32;\n");
        fprintf(fout, "    end;\n");

        fprintf(fout, "    procedure load \"group1_load\" =\n");
        fprintf(fout, "    force \"/CK\" 0 0;\n");
        fprintf(fout, "    force \"/test_se\" 1 0;\n");
        fprintf(fout, "    force \"/test_si\" 0 0;\n");
        fprintf(fout, "    apply \"group1_load_shift\" %d 32;\n", cir_->nppi_);
        fprintf(fout, "    end;\n");

        fprintf(fout, "    procedure unload \"group1_unload\" =\n");
        fprintf(fout, "    force \"/CK\" 0 0;\n");
        fprintf(fout, "    force \"/test_se\" 1 0;\n");
        fprintf(fout, "    force \"/test_si\" 0 0;\n");
        fprintf(fout, "    apply \"group1_unload_shift\" %d 32;\n", cir_->nppi_);
        fprintf(fout, "    end;\n");

        fprintf(fout, "end;\n");
    }
    fprintf(fout, "end;\n\n");

    fprintf(fout, "SCAN_TEST =\n");
    //fprintf(fout, "apply \"test_setup\" 1 0;\n");

    for (int i = 0; i < (int)pcoll_->pats_.size(); ++i) {
        fprintf(fout, "pattern = %d", i);
        if (pcoll_->pats_[i]->pi2_)
            fprintf(fout, " clock_sequential;\n");
        else
            fprintf(fout, ";\n");

        if (pcoll_->nppi_) {
            fprintf(fout, "apply  \"group1_load\" 0 =\n");
            fprintf(fout, "chain \"chain1\" = \"");
            for (int j = pcoll_->nppi_ -1; j >= 0; --j) {
                if (pcoll_->pats_[i]->ppi_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->ppi_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\";\n");
            fprintf(fout, "end;\n");
        }
        if (pcoll_->pats_[i]->pi1_) {
            fprintf(fout, "force \"PI\" \"");
            if (seqCircuitCheck) 
                fprintf(fout, "000");
            for (int j = 0; j < pcoll_->npi_; ++j) {
                if (pcoll_->pats_[i]->pi1_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->pi1_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\" 1;\n");
        }
        //fprintf(fout, "pulse \"/CK\" 2;\n");
        if (pcoll_->pats_[i]->pi2_) {
            if (seqCircuitCheck)
                fprintf(fout, "pulse \"/CK\" 2;\n");
            fprintf(fout, "force \"PI\" \"");
            if (seqCircuitCheck) 
                fprintf(fout, "000");
            for (int j = 0; j < pcoll_->npi_; ++j) {
                if (pcoll_->pats_[i]->pi2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->pi2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\" 3;\n");
        }
        if (pcoll_->pats_[i]->po2_) {
            fprintf(fout, "measure \"PO\" \"");
            if (seqCircuitCheck) 
                fprintf(fout, "X");
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\" 4;\n");
        }
        else if (pcoll_->pats_[i]->po1_) {
            fprintf(fout, "measure \"PO\" \"");
            if (seqCircuitCheck) 
                fprintf(fout, "X");
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po1_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po1_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\" 4;\n");
        }
        /*if (pcoll_->pats_[i]->po2_) {
            fprintf(fout, "pulse \"/CK\" 5;\n");
            fprintf(fout, "measure \"PO\" \"");
            if (seqCircuitCheck) 
                fprintf(fout, "X");
            for (int j = 0; j < pcoll_->npo_; ++j) {
                if (pcoll_->pats_[i]->po2_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->po2_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\" 6;\n");
        }*/

        if (pcoll_->nppi_) {
            fprintf(fout, "pulse \"/CK\" 5;\n");
            fprintf(fout, "apply  \"group1_unload\" 6 =\n");
            fprintf(fout, "chain \"chain1\" = \"");
            for (int j = pcoll_->nppi_ - 1; j >= 0; --j) {
                if (pcoll_->pats_[i]->ppo_[j] == L)
                    fprintf(fout, "0");
                else if (pcoll_->pats_[i]->ppo_[j] == H)
                    fprintf(fout, "1");
                else
                    fprintf(fout, "X");
            }
            fprintf(fout, "\";\n");
            fprintf(fout, "end;\n");
        }
    }
    fprintf(fout, "end;\n");

    // scan cells
    if(seqCircuitCheck) {    
        fprintf(fout, "SCAN_CELLS =\n");
        fprintf(fout, "scan_group \"group1\" =\n");
        fprintf(fout, "scan_chain \"chain1\" =\n");
        for (int i = cir_->npi_ + cir_->nppi_ - 1; i >= cir_->npi_; --i) {
            fprintf(fout,
                    "scan_cell = %d MASTER FFFF \"/%s\" \"I1\" \"SI\" \"Q\";\n",
                    cir_->npi_ + cir_->nppi_ - 1 - i, cir_->nl_->getTop()->getCell(cir_->gates_[i].cid_)->name_);
        }
    

    fprintf(fout, "end;\n");
    fprintf(fout, "end;\n");
    fprintf(fout, "end;\n");
    }
    fprintf(fout, "\n");
    fclose(fout);

    return true;
}

//2016 summer train
// write to STIL
// must test with tetramax
bool PatternWriter::writeSTIL(const char * const fname) {
	
	//TODO
	
	//
	vector<string> PI_Order;
	vector<string> SCAN_Order;
	vector<string> PO_Order;
	
	cout<<"=========="<<endl;//
	
	PI_Order.push_back("CK");
	PI_Order.push_back("test_si");
	PI_Order.push_back("test_se");
	PO_Order.push_back("test_so");
	
	for (int i = 0; i < cir_->npi_; ++i) {		
		PI_Order.push_back(cir_->nl_->getTop()->getPort(cir_->gates_[i].cid_)->name_);
    }
	
	for (int i = cir_->npi_; i < cir_->npi_ + cir_->nppi_; ++i) {
		SCAN_Order.push_back(cir_->nl_->getTop()->getCell(cir_->gates_[i].cid_)->name_);
    }
	
	int start = cir_->npi_ + cir_->nppi_ + cir_->ncomb_;
    for (int i = start; i < start + cir_->npo_; ++i) {
        PO_Order.push_back(cir_->nl_->getTop()->getPort(cir_->gates_[i].cid_)->name_);             
    }
	
	//
	cout<<"PI_ORDER ";
	for (unsigned i=0; i<PI_Order.size(); i++)
		cout<<PI_Order[i]<<" ";
	cout<<endl;
	
	cout<<"SCAN_ORDER ";
	for (unsigned i=0; i<SCAN_Order.size(); i++)
		cout<<SCAN_Order[i]<<" ";
	cout<<endl;
	
	cout<<"PO_ORDER ";
	for (unsigned i=0; i<PO_Order.size(); i++)
		cout<<PO_Order[i]<<" ";
	cout<<endl;
	//
	
	string Processor_Mode = "";
	int pattern_size = (int)pcoll_->pats_.size();
	
	switch (pcoll_->type_) {
		case PatternProcessor::BASIC_SCAN:
			Processor_Mode = "BASIC_SCAN";
			break;
		case PatternProcessor::LAUNCH_CAPTURE:
			Processor_Mode = "LAUNCH_ON_CAPTURE";
			break;
		case PatternProcessor::LAUNCH_SHIFT:
			Processor_Mode = "LAUNCH_ON_SHIFT";
			break;
    }
	
	cout<<Processor_Mode<<" "<<pattern_size<<endl;//
	
	vector<map<string, string> > patternList;
	
	for (int i = 0; i < (int)pcoll_->pats_.size(); ++i){
		map<string, string> map_pattern;
		
		if (pcoll_->pats_[i]->pi1_) {
			map_pattern["pi1"] = "";
			for (int j = 0; j < pcoll_->npi_; ++j) {
				if (pcoll_->pats_[i]->pi1_[j] == L)
					map_pattern["pi1"] += "0";
				else if (pcoll_->pats_[i]->pi1_[j] == H)
					map_pattern["pi1"] += "1";
				else
					map_pattern["pi1"] += "N";
			}
		}
		
		if (pcoll_->pats_[i]->ppi_) {
			map_pattern["ppi1"] = "";
			for (int j = 0; j < pcoll_->nppi_; ++j) {
				if (pcoll_->pats_[i]->ppi_[j] == L)
					map_pattern["ppi1"] += "0";
				else if (pcoll_->pats_[i]->ppi_[j] == H)
					map_pattern["ppi1"] += "1";
				else
					map_pattern["ppi1"] += "N";
			}
            reverse(map_pattern["ppi1"].begin(), map_pattern["ppi1"].end());

		}
		if (pcoll_->pats_[i]->po1_) {
			map_pattern["po1"] = "";
			for (int j = 0; j < pcoll_->npo_; ++j) {
				if (pcoll_->pats_[i]->po1_[j] == L)
					map_pattern["po1"] += "L";
				else if (pcoll_->pats_[i]->po1_[j] == H)
					map_pattern["po1"] += "H";
				else
					map_pattern["po1"] += "N";
			}
		}
		
		if (pcoll_->pats_[i]->ppi_) {
			map_pattern["ppo"] = "";
			for (int j = 0; j < pcoll_->nppi_; ++j) {
				if (pcoll_->pats_[i]->ppo_[j] == L)
					map_pattern["ppo"] += "L";
				else if (pcoll_->pats_[i]->ppo_[j] == H)
					map_pattern["ppo"] += "H";
				else
					map_pattern["ppo"] += "N";
			}
            reverse(map_pattern["ppo"].begin(), map_pattern["ppo"].end());

		}
		patternList.push_back(map_pattern);
		
	}
	
	for (unsigned i=0; i<patternList.size(); i++){//
		cout<<endl<<"pattern_"<<i+1<<endl;
		map<string, string>::iterator iter;
		for(iter = patternList[i].begin(); iter != patternList[i].end(); iter++)
			cout<<iter->first<<" "<<iter->second<<endl;
	}//
	
	cout<<"=========="<<endl;//
	
    ofstream os(fname);
    os<<"STIL 1.0;"<<endl<<endl;
    /////signals/////
    os<<"Signals {"<<endl;
    for (unsigned i=0; i<PI_Order.size(); i++){
        os<<"   \""+PI_Order[i]+"\" "+"In";
        if (PI_Order[i] == "test_si") 
            os<<" { ScanIn; }"<<endl;
        else
            os<<";"<<endl;

    }
    for (unsigned i=0; i<PO_Order.size(); i++){
        os<<"   \""+PO_Order[i]+"\" "+"Out";
        if (PO_Order[i] == "test_so") 
            os<<" { ScanOut; }"<<endl;
        else
            os<<";"<<endl;

    }
    os<<"}"<<endl<<endl;
    /////SignalGroups/////
    os<<"SignalGroups {"<<endl;
    string _pi_in = "";
    string _po_out = "";
    string _in_timing = "";
    for (unsigned i=0; i<PI_Order.size(); i++){
        _pi_in = _pi_in + "\"" + PI_Order[i] + "\"";
        if (i != PI_Order.size() - 1) 
            _pi_in += " + ";
        if (PI_Order[i] != "CK"){
            _in_timing = _in_timing + "\"" + PI_Order[i] + "\"";
            if (i != PI_Order.size() - 1) 
                _in_timing += " + ";
        }

    }
    for (unsigned i=0; i<PO_Order.size(); i++){
        _po_out = _po_out + "\"" + PO_Order[i] + "\"";
        if (i != PO_Order.size() - 1) 
            _po_out += " + ";
    }
    os<<"   \"_pi\" = \'" + _pi_in + "\';"<<endl;
    os<<"   \"_in\" = \'" + _pi_in + "\';"<<endl;
    os<<"   \"_si\" = \'\"test_si\"\' { ScanIn; }"<<endl;
    os<<"   \"_po\" = \'" + _po_out + "\';"<<endl;
    os<<"   \"_out\" = \'" + _po_out + "\';"<<endl;
    os<<"   \"_so\" = \'\"test_so\"\' { ScanOut; }"<<endl;
    os<<"   \"_default_In_Timing_\" = \'" + _in_timing + "\';"<<endl;
    os<<"   \"_default_Out_Timing_\" = \'" + _po_out + "\';"<<endl;
    os<<"}"<<endl<<endl;
    /////Timing/////
    os<<"Timing {"<<endl;
    os<<"   WaveformTable \"_default_WFT_\" {"<<endl;
    os<<"       Period \'100ns\';"<<endl;
    os<<"       Waveforms {"<<endl;
    os<<"           \"CK\" { 0 { \'0ns\' D; } }"<<endl;
    os<<"           \"CK\" { P { \'0ns\' D; \'50ns\' U; \'75ns\' D; } }"<<endl;
    os<<"           \"CK\" { 1 { \'0ns\' U; } }"<<endl;
    os<<"           \"CK\" { Z { \'0ns\' Z; } }"<<endl;
    os<<"           \"_default_In_Timing_\" { 0 { \'0ns\' D; } }"<<endl;
    os<<"           \"_default_In_Timing_\" { 1 { \'0ns\' U; } }"<<endl;
    os<<"           \"_default_In_Timing_\" { Z { \'0ns\' Z; } }"<<endl;
    os<<"           \"_default_In_Timing_\" { N { \'0ns\' N; } }"<<endl;
    os<<"           \"_default_Out_Timing_\" { X { \'0ns\' X; } }"<<endl;
    os<<"           \"_default_Out_Timing_\" { H { \'0ns\' X; \' 90 ns\' H; } }"<<endl;
    os<<"           \"_default_Out_Timing_\" { T { \'0ns\' X; \' 90 ns\' T; } }"<<endl;
    os<<"           \"_default_Out_Timing_\" { L { \'0ns\' X; \' 90 ns\' L; } }"<<endl;
    os<<"       }"<<endl;
    os<<"   }"<<endl;
    os<<"}"<<endl<<endl;

    /////ScanStructures/////
    os<<"ScanStructures {"<<endl;
    os<<"   ScanChain \"chain1\" {"<<endl;//
    os<<"       ScanLength "<<SCAN_Order.size()<<";"<<endl;
    os<<"       ScanIn \"test_si\";"<<endl;
    os<<"       ScanOut \"test_so\";"<<endl;
    os<<"       ScanInversion 0;"<<endl;
    os<<"       ScanCells";
    for (unsigned i=0; i<SCAN_Order.size(); i++)
        os<<" \"TOP."<<SCAN_Order[i]<<".SI\"";
    os<<";"<<endl;
    os<<"       ScanMasterClock \"CK\" ;"<<endl;
    os<<"   }"<<endl;
    os<<"}"<<endl<<endl;

    /////PatternBurst PatternExec/////
    os<<"PatternBurst \"_burst_\" {"<<endl;
    os<<"   PatList { \"_pattern_\" { } }"<<endl;
    os<<"}"<<endl<<endl;
    os<<"PatternExec {"<<endl;
    os<<"   PatternBurst \"_burst_\";"<<endl;
    os<<"}"<<endl<<endl;

    /////Procedures/////
    os<<"Procedures {"<<endl;
    os<<"   \"load_unload\" {"<<endl;
    os<<"       W \"_default_WFT_\";"<<endl;
    os<<"       C { \"test_si\"=0; \"CK\"=0; \"test_se\"=1; }"<<endl;
    os<<"       V { \"_so\"=#; }"<<endl;
    os<<"       Shift {"<<endl;
    os<<"           W \"_default_WFT_\";"<<endl;
    os<<"           V { \"_si\"=#; \"_so\"=#; \"CK\"=P; }"<<endl;
    os<<"       }"<<endl;
    os<<"   }"<<endl;
    os<<"   \"capture_CK\" {"<<endl;
    os<<"       W \"_default_WFT_\";"<<endl;
    os<<"       F { \"test_se\"=0; }"<<endl;
    os<<"       C { \"_po\"=\\r"<<PO_Order.size()<<" X ; }"<<endl;
    os<<"       \"forcePI\": V { \"_pi\"=\\r"<<PI_Order.size()<<" # ; }"<<endl;
    os<<"       \"measurePO\": V { \"_po\"=\\r"<<PO_Order.size()<<" # ; }"<<endl;
    os<<"       C { \"_po\"=\\r"<<PO_Order.size()<<" X ; }"<<endl;
    os<<"       \"pulse\": V { \"CK\"=P; }"<<endl;
    os<<"   }"<<endl;
    os<<"   \"capture\" {"<<endl;
    os<<"       W \"_default_WFT_\";"<<endl;
    os<<"       F { \"test_se\"=0; }"<<endl;
    os<<"       C { \"_po\"=\\r"<<PO_Order.size()<<" X ; }"<<endl;
    os<<"       \"forcePI\": V { \"_pi\"=\\r"<<PI_Order.size()<<" # ; }"<<endl;
    os<<"       \"measurePO\": V { \"_po\"=\\r"<<PO_Order.size()<<" # ; }"<<endl;
    os<<"   }"<<endl;
    os<<"}"<<endl<<endl;

    /////MacroDefs/////
    os<<"MacroDefs {"<<endl;
    os<<"   \"test_setup\" {"<<endl;
    os<<"       W \"_default_WFT_\";"<<endl;
    os<<"       V { \"test_se\"=0; \"CK\"=0; }"<<endl;
    os<<"   }"<<endl;
    os<<"}"<<endl<<endl;

    /////Pattern/////
    os<<"Pattern \"_pattern_\" {"<<endl;
    os<<"   W \"_default_WFT_\";"<<endl;
    os<<"   \"precondition all Signals\": C { \"_pi\"=\\r"<<PI_Order.size()<<" 0 ; \"_po\"=\\r"<<PO_Order.size()<<" X ; }"<<endl;
    os<<"   Macro \"test_setup\";"<<endl;

    for (int i=0; i<pattern_size; i++){
        os<<"   \"pattern "<<i<<"\":"<<endl;
        os<<"       Call \"load_unload\" {"<<endl;
        if (i > 0)
            os<<"           \"test_so\"="<<patternList[i-1]["ppo"]<<";"<<endl;
        os<<"           \"test_si\"="<<patternList[i]["ppi1"]<<";"<<endl;
        os<<"       }"<<endl;
        os<<"       Call \"capture_CK\" {"<<endl;
        os<<"           \"_pi\"=";
        for (unsigned j=patternList[i]["pi1"].length(); j<PI_Order.size(); j++){
            os<<"0";
        }
        os<<patternList[i]["pi1"]<<";"<<endl;
        os<<"           \"_po\"=";
        for (unsigned j=patternList[i]["po1"].length(); j<PO_Order.size(); j++){
            os<<"L";
        }
        os<<patternList[i]["po1"]<<";"<<endl;
        os<<"       }"<<endl;
        
    }

    os<<"   \"end "<<pattern_size-1<<" unload\":"<<endl;
    os<<"       Call \"load_unload\" {"<<endl;
    os<<"           \"test_so\"="<<patternList[pattern_size-1]["ppo"]<<";"<<endl;
    os<<"       }"<<endl;
    os<<"}";



    os.close();
	
    return true;
	
	
}



bool ProcedureWriter::writeProc(const char * const fname) {
    FILE *fout = fopen(fname, "w");
    if (!fout) {
        fprintf(stderr, "**ERROR PatternWriter::writePat(): file");
        fprintf(stderr, "`%s' cannot be opened\n", fname);
        return false;
    }

    fprintf(fout, "set time scale 1.000000 ns ;\n");
    fprintf(fout, "set strobe_window time 3 ;\n\n");
    fprintf(fout, "timeplate _default_WFT_ =\n");
    fprintf(fout, "force_pi 0 ;\n");
    fprintf(fout, "measure_po 29 ;\n");
    fprintf(fout, "pulse CK 16 3;\n");
    fprintf(fout, "period 32 ;\n");
    fprintf(fout, "end;\n\n");
    fprintf(fout, "procedure shift =\n");
    fprintf(fout, "scan_group group1 ;\n");
    fprintf(fout, "timeplate _default_WFT_ ;\n");
    fprintf(fout, "cycle =\n");
    fprintf(fout, "force_sci ;\n");
    fprintf(fout, "pulse CK ;\n");
    fprintf(fout, "measure_sco ;\n");
    fprintf(fout, "end;\n");
    fprintf(fout, "end;\n\n");
    fprintf(fout, "procedure load_unload =\n");
    fprintf(fout, "scan_group group1 ;\n");
    fprintf(fout, "timeplate _default_WFT_ ;\n");
    fprintf(fout, "cycle =\n");
    fprintf(fout, "force CK 0 ;\n");
    fprintf(fout, "force test_se 1 ;\n");
    fprintf(fout, "force test_si 0 ;\n");
    fprintf(fout, "measure_sco ;\n");
    fprintf(fout, "end ;\n");
    fprintf(fout, "apply shift %d;\n", cir_->nppi_);
    fprintf(fout, "end;\n");
    fprintf(fout, "procedure test_setup =\n");
    fprintf(fout, "timeplate _default_WFT_ ;\n");
    fprintf(fout, "cycle =\n");
    fprintf(fout, "force CK 0 ;\n");
    fprintf(fout, "force test_se 0 ;\n");
    fprintf(fout, "end;\n");
    fprintf(fout, "end;\n");
    fclose(fout);
    return true;
}


