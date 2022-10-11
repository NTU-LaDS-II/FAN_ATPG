// **************************************************************************
// File       [ netlist.h ]
// Author     [ littleshamoo ]
// Synopsis   [ define netlist structure ]
// Date       [ 2010/12/14 created ]
// **************************************************************************

#ifndef _INTF_NETLIST_H_
#define _INTF_NETLIST_H_

#include "cell.h"
#include "techlib.h"

namespace IntfNs {

class Netlist {
public:

            Netlist();
            ~Netlist();

    // info
    bool    check(const bool &verbose = false) const;
    bool    setTechlib(Techlib * const lib);
    Techlib *getTechlib() const;

    // top module
    Cell    *getTop() const;
    bool    setTop(const size_t &i);
    bool    setTop(const char * const name);

    // modules
    bool    addModule(Cell *m);
    size_t  getNModule() const;
    Cell    *getModule(const char * const name) const;
    Cell    *getModule(const size_t &i) const;

    // operations
    bool    removeFloatingNets();
    bool    levelize();

private:
    Cell    *top_;
    CellVec modules_;
    CellMap nameToModule_;
    Techlib *lib_;
    int     lvl_;
};


// inline methods
inline Netlist::Netlist() {
    top_ = NULL;
    lib_ = NULL;
    lvl_ = -1;
}

inline Netlist::~Netlist() {
}

inline bool Netlist::setTechlib(Techlib * const lib) {
    lib_ = lib;
    return true;
}

inline Techlib *Netlist::getTechlib() const {
    return lib_;
}

inline Cell *Netlist::getTop() const {
    return top_;
}

inline bool Netlist::setTop(const char * const name) {
    CellMap::const_iterator it = nameToModule_.find(name);
    if (it != nameToModule_.end()) {
        top_ = it->second;
        lvl_ = -1;
    }
    return it != nameToModule_.end();
}

inline bool Netlist::setTop(const size_t &i) {
    if (i < modules_.size()) {
        top_ = modules_[i];
        lvl_ = -1;
    }
    return i < modules_.size();
}

inline bool Netlist::addModule(Cell *m) {
    CellMap::iterator it = nameToModule_.find(m->name_);
    if (it != nameToModule_.end())
        return false;
    modules_.push_back(m);
    nameToModule_[m->name_] = m;
    return true;
}

inline size_t Netlist::getNModule() const {
    return modules_.size();
}

inline Cell *Netlist::getModule(const char * const name) const {
    CellMap::const_iterator it = nameToModule_.find(name);
    return it == nameToModule_.end() ? NULL : it->second;
}

inline Cell *Netlist::getModule(const size_t &i) const {
    return i < modules_.size() ? modules_[i] : NULL;
}

};

#endif

