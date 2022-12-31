// **************************************************************************
// File       [ decision_tree.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/11/01 created ]
// **************************************************************************

#ifndef _CORE_DECISION_TREE_H_
#define _CORE_DECISION_TREE_H_

#include <vector>

namespace CoreNs
{
	struct DecisionTreeNode
	{
	public:
		DecisionTreeNode(const int &gateId, const int &startPoint);
		int gateId_;																// the gate ID of the decision
		int startPointInBacktrackImplicatedGateIDs; // starting point in backtrackList of gid_ ;
		bool mark_;																	// initially mark_=false, when backtracked once, mark_ is changed to true, when backtracked again, this decision is popped out.
	};

	class DecisionTree
	{
	public:
		static constexpr int InitSize = 5000;

		DecisionTree();

		void clear();
		void put(const int &gateId, const int &startPoint);
		bool get(int &gateId, int &startPoint);
		bool empty() const;
		bool lastNodeMarked() const;

	private:
		std::vector<DecisionTreeNode> tree_;
	};

	inline DecisionTreeNode::DecisionTreeNode(const int &gateId, const int &startPoint)
	{
		gateId_ = gateId;
		startPointInBacktrackImplicatedGateIDs = startPoint;
		mark_ = false;
	}

	inline DecisionTree::DecisionTree()
	{
		tree_.reserve(InitSize);
	}

	inline void DecisionTree::clear()
	{
		tree_.clear();
	}

	inline void DecisionTree::put(const int &gateId, const int &startPoint)
	{
		tree_.push_back(DecisionTreeNode(gateId, startPoint));
	}

	inline bool DecisionTree::get(int &gateId, int &startPoint)
	{
		DecisionTreeNode &node = tree_.back();
		gateId = node.gateId_;
		startPoint = node.startPointInBacktrackImplicatedGateIDs;

		if (node.mark_)
		{
			tree_.pop_back();
			return true;
		}
		else
		{
			node.mark_ = true;
			return false;
		}
		return false;
	}

	inline bool DecisionTree::empty() const
	{
		return tree_.empty();
	}

	inline bool DecisionTree::lastNodeMarked() const
	{
		return tree_.empty() ? false : tree_.back().mark_;
	}
};

#endif