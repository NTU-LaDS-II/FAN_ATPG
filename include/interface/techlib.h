// **************************************************************************
// File       [ techlib.h ]
// Author     [ littleshamoo ]
// Synopsis   [ define technology library ]
// Date       [ 2011/03/23 created ]
// **************************************************************************

#ifndef _INTF_TECHLIB_H_
#define _INTF_TECHLIB_H_

#include <queue>

#include "primitive.h"

namespace IntfNs {

class Techlib {
public:
           Techlib();
           ~Techlib();

    bool   addCell(Cell *c);
    size_t getNCell() const;
    Cell   *getCell(const size_t &i) const;
    Cell   *getCell(const char * const name) const;

    bool   hasPmt(const size_t &i, const Pmt::Type &type) const;
    bool   hasPmt(const char * const name, const Pmt::Type &type) const;

    bool   check(const bool &verbose = false) const;
    bool   levelize();

private:
    CellVec cells_;
    CellMap nameToCell_;
};


inline Techlib::Techlib() {}

inline Techlib::~Techlib() {}

inline bool Techlib::addCell(Cell *c) {
    CellMap::iterator it = nameToCell_.find(c->name_);
    if (it != nameToCell_.end())
        return false;
    cells_.push_back(c);
    nameToCell_[c->name_] = c;
    c->id_ = cells_.size() - 1;
    return true;
}

inline size_t Techlib::getNCell() const {
    return cells_.size();
}

inline Cell *Techlib::getCell(const size_t &i) const {
    return i < cells_.size() ? cells_[i] : NULL;
}

inline Cell *Techlib::getCell(const char * const name) const {
    CellMap::const_iterator it = nameToCell_.find(name);
    return it == nameToCell_.end() ? NULL : it->second;
}

inline bool Techlib::hasPmt(const size_t &i, const Pmt::Type &type) const {
    if (i >= cells_.size())
        return false;
    std::queue<Cell *> cque;
    cque.push(cells_[i]);
    while (!cque.empty()) {
        Cell *c = cque.front();
        cque.pop();
        for (size_t j = 0; j < c->getNCell(); ++j) {
            if (c->getCell(j)->libc_)
                cque.push(c->getCell(j)->libc_);
            else
                if (((Pmt *)c->getCell(j))->type_ == type)
                    return true;
        }
    }
    return false;
}

inline bool Techlib::hasPmt(const char * const name,
                            const Pmt::Type &type) const {
    CellMap::const_iterator it = nameToCell_.find(name);
    if (it == nameToCell_.end())
        return false;
    return hasPmt(it->second->id_, type);
}

};

#endif

