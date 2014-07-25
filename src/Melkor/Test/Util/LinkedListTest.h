#ifndef LINKEDLISTTEST_H
#define LINKEDLISTTEST_H

#include "LinkedList.h"
#include <cppunit/extensions/HelperMacros.h>

class LinkedListTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LinkedListTest );
	CPPUNIT_TEST( testCapacity );
	CPPUNIT_TEST( testPointerIntegrity );
	CPPUNIT_TEST( testIteration );
	CPPUNIT_TEST( testPushBack );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testCapacity();
		void testPointerIntegrity();
		void testIteration();
		void testPushBack();
		void testErase();
};

#endif