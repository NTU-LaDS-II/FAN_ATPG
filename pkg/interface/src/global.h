// **************************************************************************
// File       [ global.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/29 created ]
// **************************************************************************

#ifndef _INTF_GLOBAL_H_
#define _INTF_GLOBAL_H_

#include <cstring>

namespace IntfNs {

const int NAME_LEN = 256;

struct NameKey {
         NameKey(const NameKey &key);
         NameKey(const char * const val);
    bool operator<(const NameKey &key) const;
    char val_[NAME_LEN];
};

inline NameKey::NameKey(const NameKey &key) {
    std::memmove(val_, key.val_, NAME_LEN);
}

inline NameKey::NameKey(const char * const val) {
    std::memset(val_, '\0', NAME_LEN);
    if (val)
        strncpy(val_, val, NAME_LEN - 1);
}

inline bool NameKey::operator<(const NameKey &key) const {
    return strcmp(val_, key.val_) < 0;
}

};

#endif

