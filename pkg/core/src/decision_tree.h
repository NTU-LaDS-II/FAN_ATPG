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
		int gateId_;																 // the gate ID of the decision
		int startPointInBacktrackImplicatedGateIDs_; // starting point in backtrackList of gid_ ;
		bool mark_;																	 // initially mark_=false, when backtracked once, mark_ is changed to true, when backtracked again, this decision is popped out.
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
		startPointInBacktrackImplicatedGateIDs_ = startPoint;
		mark_ = false;
	}

	inline DecisionTree::DecisionTree()
	{
		tree_.reserve(InitSize);
	}

	// **************************************************************************
	// Function   [ DecisionTree::clear ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Initial assignment of DecisionTree.
	//										 Clear tree_ of the DecisionTree.
	//							description:
	//								tree_ is a vector storing all the DecisionTreeNodes
	//								of the DecisionTree. Clear the vector tree_.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline void DecisionTree::clear()
	{
		tree_.clear();
	}

	// **************************************************************************
	// Function   [ DecisionTree::put ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Add a new DecisionTreeNode to the tree_ of DecisionTree.
	//							description:
	//								Create a new DecisionTreeNode initiated with given arguments,
	//								and push_back to the tree_ vector.
	//							arguments:
	// 								[in] gateId : The gate Id to be assigned to the new DecisionTreeNode.
	//								[in] startPoint : The startPointInBacktrackImplicatedGateIDs_
	//																	to be assigned to the new DecisionTreeNode.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline void DecisionTree::put(const int &gateId, const int &startPoint)
	{
		tree_.push_back(DecisionTreeNode(gateId, startPoint));
	}

	// **************************************************************************
	// Function   [ DecisionTree::get ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Get the gateId and startPoint of the last DecisionTreeNode
	//										 and check if it is marked.
	//							description:
	//								Find the last DecisionTreeNode of tree_ and:
	//								1. Assign its gateId_ to the input argument gateId.
	//								2. Assign its startPointInBacktrackImplicatedGateIDs_
	//									 to the input argument startPoint.
	//								3. Return whether this node is marked (bool)
	//							arguments:
	// 								[in] gateId : Will be assigned to the gate Id of the last
	//															DecisionTreeNode in tree_.
	//								[in] startPoint : Will be assigned to the
	//																	startPointInBacktrackImplicatedGateIDs_
	//																	of the last DecisionTreeNode in tree_.
	//								[out] bool : Return whether the last DecisionTreeNode is marked.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline bool DecisionTree::get(int &gateId, int &startPoint)
	{
		DecisionTreeNode &node = tree_.back();
		gateId = node.gateId_;
		startPoint = node.startPointInBacktrackImplicatedGateIDs_;

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

	// **************************************************************************
	// Function   [ DecisionTree::empty ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Check if the vector<DecisionTreeNode> tree_ is empty or not.
	//							description:
	//								Check if the vector tree_ is empty or not.
	//							arguments:
	// 								[out] bool : Return whether the tree_ of DecisionTree is empty.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline bool DecisionTree::empty() const
	{
		return tree_.empty();
	}

	// **************************************************************************
	// Function   [ DecisionTree::lastNodeMarked ]
	// Commenter  [ CHT ]
	// Synopsis   [ usage: Check if the last element of tree_ is marked or not.
	//							description:
	//								First check if tree_ is empty. If yes, return false.
	//								Check if the last DecisionTreeNode in tree_ is marked or not.
	//								Used for backtracking in the algorithm.
	//							arguments:
	// 								[out] bool : Return whether the last element of tree_ is marked.
	//						]
	// Date       [ CHT Ver. 1.0 started 2023/01/05 ]
	// **************************************************************************
	inline bool DecisionTree::lastNodeMarked() const
	{
		return tree_.empty() ? false : tree_.back().mark_;
	}
};

#endif