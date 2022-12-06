// **************************************************************************
// File       [ decision_tree.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#ifndef _CORE_DECISION_TREE_H_
#define _CORE_DECISION_TREE_H_

#include <vector>

namespace CoreNs {

struct DecisionTreeNode {
public:
    DecisionTreeNode(const int &gid, const unsigned &startPoint);
    int      gid_;   //  the gate ID of the decision
    unsigned startPoint_; // starting point in backtrackList of gid_ ;
    bool     mark_;  // initially, mark=false;  when backtracked once, this is changed to true; when backtracked again, this decision is poped out. 
};

class DecisionTree {
public:

    static const unsigned InitSize = 5000;

         DecisionTree();
         ~DecisionTree();

    void clear();
    void put(const int &gid, const unsigned &startPoint);
    bool get(int &gid, unsigned &startPoint);
    bool empty() const;
    bool lastNodeMark() const;

private:
    std::vector<DecisionTreeNode> tree_;
};


inline DecisionTreeNode::DecisionTreeNode(const int &gid,
                                          const unsigned &startPoint) {
    gid_   = gid;
    startPoint_ = startPoint;
    mark_  = false;
}

//{{{ class DecisionTree inline
inline DecisionTree::DecisionTree() {
    tree_.reserve(InitSize);
}

inline DecisionTree::~DecisionTree() {}

inline void DecisionTree::clear() {
    tree_.clear();
}

inline void DecisionTree::put(const int &gid, const unsigned &startPoint) {
    tree_.push_back(DecisionTreeNode(gid, startPoint));
}

inline bool DecisionTree::get(int &gid, unsigned &startPoint) {
    DecisionTreeNode &node = tree_.back();
    gid = node.gid_;
    startPoint = node.startPoint_;

    if (node.mark_) {
        tree_.pop_back();
        return true;
    }
    else {
        node.mark_ = true; 
        return false;
    }
    return false;
}

inline bool DecisionTree::empty() const {
    return tree_.empty();
}

inline bool DecisionTree::lastNodeMark() const {
    return tree_.empty() ? false : tree_.back().mark_;
}
//}}}

};

#endif

