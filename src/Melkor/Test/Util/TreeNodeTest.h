#ifndef TREENODETEST_H
#define TREENODETEST_H

#include "TreeNode.h"
#include <cppunit/extensions/HelperMacros.h>

class TreeNodeTest : public CppUnit::TestFixture
{
	class DummyInt
	{
		public:
			int value;
			DummyInt( int i ) : value( i ) {};
			bool operator==( const DummyInt& d ) const { return value == d.value; };
	};

	CPPUNIT_TEST_SUITE( TreeNodeTest );
	CPPUNIT_TEST( testLessThan );
	CPPUNIT_TEST( testEquality );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testAddChild );
	CPPUNIT_TEST( testGetChild );
	CPPUNIT_TEST( testClear );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testLessThan();
		void testEquality();
		void testConstructor();
		void testAddChild();
		void testGetChild();
		void testClear();
};

#endif