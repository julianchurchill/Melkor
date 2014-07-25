#include "TreeNodeTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TreeNodeTest );

void TreeNodeTest::setUp()
{
}

void TreeNodeTest::tearDown()
{
}

void TreeNodeTest::testLessThan()
{
	TreeNode<int> node1( 1 );
	TreeNode<int> node2( 2 );
	TreeNode<int> node3( 3 );

	CPPUNIT_ASSERT( node1 < node2 );
	CPPUNIT_ASSERT( !(node2 < node1) );
	CPPUNIT_ASSERT( node2 < node3 );
	CPPUNIT_ASSERT( !(node3 < node2) );
	CPPUNIT_ASSERT( node1 < node3 );
	CPPUNIT_ASSERT( !(node3 < node1) );
}

void TreeNodeTest::testEquality()
{
	TreeNode<int> node1( 1 );
	TreeNode<int> node2( 2 );
	TreeNode<int> node3( 1 );

	CPPUNIT_ASSERT( node1 == node1 );
	CPPUNIT_ASSERT( node1 != node2 );
	CPPUNIT_ASSERT( node2 != node1 );
	CPPUNIT_ASSERT( node1 == node3 );
}

void TreeNodeTest::testConstructor()
{
	TreeNode<DummyInt> tree1( DummyInt( 1 ) );

	CPPUNIT_ASSERT( tree1.getValue() == DummyInt( 1 ) );
	CPPUNIT_ASSERT( tree1 == tree1 );

	TreeNode<DummyInt> tree2( DummyInt( 1 ) );

	CPPUNIT_ASSERT( tree1 == tree2 );

	TreeNode<DummyInt> tree3( DummyInt( 2 ) );

	CPPUNIT_ASSERT( tree2 != tree3 );

	TreeNode<DummyInt> tree4( tree1 );

	CPPUNIT_ASSERT( tree1 == tree4 );

	tree4 = tree1;

	CPPUNIT_ASSERT( tree1 == tree4 );
}

void TreeNodeTest::testAddChild()
{
	TreeNode<DummyInt> tree1( DummyInt( 1 ) );
	DummyInt dummyInt2( 2 );

	TreeNode<DummyInt>* current = &tree1;
	TreeNode<DummyInt>* acrt1 = current->addChild( dummyInt2 );

	CPPUNIT_ASSERT( acrt1->getValue() == dummyInt2 );

	// Try adding the child again when it's already present
	TreeNode<DummyInt>* acrt2 = current->addChild( dummyInt2 );

	CPPUNIT_ASSERT( acrt1 == acrt2 );
	CPPUNIT_ASSERT( *acrt1 == *acrt2 );
	CPPUNIT_ASSERT( acrt2 != current );

	current = acrt2;
}

void TreeNodeTest::testGetChild()
{
	DummyInt dummyInt1( 1 );
	DummyInt dummyInt2( 2 );
	TreeNode<DummyInt> tree1( dummyInt1 );
	TreeNode<DummyInt>* current = &tree1;

	// dummyInt1 should not be found as a _child_ of tree1
	TreeNode<DummyInt>::gcConstReturnType gcrt = current->getChild( dummyInt1 );
	
	CPPUNIT_ASSERT( gcrt.second == false );

	current = current->addChild( dummyInt2 );

	// dummyInt2 should be found as a child of tree1
	gcrt = tree1.getChild( dummyInt2 );

	CPPUNIT_ASSERT( gcrt.second == true );
	CPPUNIT_ASSERT( gcrt.first->getValue() == dummyInt2 );
}

void TreeNodeTest::testClear()
{
	DummyInt dummyInt1( 1 );
	DummyInt dummyInt2( 2 );
	TreeNode<DummyInt> tree1( dummyInt1 );
	TreeNode<DummyInt>* current = &tree1;

	current = current->addChild( dummyInt2 );
	current = current->addChild( dummyInt1 );
	current = current->addChild( dummyInt2 );

	// Total tree size == 4
	tree1.clear();
	TreeNode<DummyInt>::gcConstReturnType gcrt = tree1.getChild( dummyInt2 );

	CPPUNIT_ASSERT( gcrt.second == false );

	gcrt = tree1.getChild( dummyInt1 );

	CPPUNIT_ASSERT( gcrt.second == false );
}
