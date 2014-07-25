#ifndef TREE_H
#define TREE_H

#pragma warning( disable : 4786 )

#include "TreeNode.h"

/** @brief This class implements a generic strict tree structure. It is directed, has no cycles
  *  and each node only has a single parent. It uses the TreeNode class as nodes. 
  * @see TreeNode. */
template <class T>
class Tree
{
	private:
		TreeNode root;

	public:
		Tree() : root( T() ) {};

		// Return the item at the root of the tree
		const TreeNode& getRoot() const { return root; };

		// Empty to the tree
		void clear() { root.clear(); };
};

#endif // TREE_H