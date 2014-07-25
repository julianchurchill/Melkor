#ifndef TREENODE_H
#define TREENODE_H

#pragma warning( disable : 4786 )

//#include <set>
#include <vector>
using namespace std;

/** @brief A generic template implementation of a TreeNode used by the Tree class. 
  * @see Tree. */
template <class T>
class TreeNode
{
	private:
//		typedef set<TreeNode> ChildrenType;
		typedef vector<TreeNode*> ChildrenType;
		typedef pair<TreeNode*, bool> gcReturnType;

		T value;
		ChildrenType children;

	public:
		typedef pair<const TreeNode*, bool> gcConstReturnType;

		TreeNode( const T& v ) : value( v ) {};
		TreeNode( const TreeNode& t ) : value( t.value )
		{
			ChildrenType::const_iterator citer( t.children.begin() );
			for( ; citer != t.children.end(); citer++) {
				children.push_back( new TreeNode( **citer ) );
			}
		}
		TreeNode& operator=( const TreeNode& t )
		{
			if( this != &t ) {
				value = t.value;
				ChildrenType::const_iterator citer( t.children.begin() );
				for( ; citer != t.children.end(); citer++) {
					children.push_back( new TreeNode( **citer ) );
				}
			}
			return *this;
		}
		~TreeNode()
		{
			clear();
		}
		
		/** @brief Equality of TreeNodes must be based on equality of the item they represent. 
		  * '==' must be implemented in T also. */
		bool operator==( const TreeNode& t ) const { return value == t.value; };
		bool operator!=( const TreeNode& t ) const { return !(*this == t); };
		
		/** @brief This operator is defined so <set> can be used to contain an ordered list of
		  * the children of each TreeNode. The '<' operator must be implemented in T also. */
		bool operator<( const TreeNode& t ) const { return value < t.value; };

		const T& getValue() const { return value; };
		
		/** @brief Add this item as a child of this TreeNode if it is unique.
		  * @return The new child TreeNode or a pre-exisiting one if the child was not unique. */
		TreeNode* addChild( const T& child ) 
		{
			gcReturnType gcrt = getChildNonConst( child );
			if( gcrt.second )
				return gcrt.first;
			return *children.insert( children.end(), new TreeNode( child ) );
		}

		// Remove all children
		void clear()
		{
			ChildrenType::iterator iter( children.begin() );
			for( ; iter != children.end(); iter++) {
				(*iter)->clear();
				delete *iter;
			}
			children.clear();

		}

		/** @brief Get the child TreeNode that contains an item that is equal to the one supplied. 
		  * @return If the child is found then the first pair element is a reference to the child TreeNode,
		  *  and the second element is true. If the child is not found the first element is a reference to
		  *  this TreeNode and the second element is false. */
		gcConstReturnType getChild( const T& item ) const
		{
			ChildrenType::const_iterator citer( children.begin() );
			for( ; citer != children.end() ; citer++) {
				if( (*citer)->value == item )
					return gcConstReturnType( *citer, true );
			}
			return gcConstReturnType( this, false );
		}

	private:
		/** @brief A private non-const version of getChild() for internal use. */
		gcReturnType getChildNonConst( const T& item )
		{
			ChildrenType::iterator iter( children.begin() );
			for( ; iter != children.end() ; iter++) {
				if( (*iter)->value == item )
					return gcReturnType( *iter, true );
			}
			return gcReturnType( this, false );
		}
};

#endif // TREENODE_H