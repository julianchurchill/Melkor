#ifndef GOSTRINGTEST_H
#define GOSTRINGTEST_H

#include "GoString.h"
#include <cppunit/extensions/HelperMacros.h>

class GoStringTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( GoStringTest );
	CPPUNIT_TEST( testCopyEquality );
	CPPUNIT_TEST( testContainsPoint );
	CPPUNIT_TEST( testEyes );
	CPPUNIT_TEST_SUITE_END();

	private:
		BoardStruct twoWhiteStringBoard;

	public:
		void setUp();
		void tearDown();

		void testCopyEquality();
		void testContainsPoint();
		void testEyes();
};

#endif