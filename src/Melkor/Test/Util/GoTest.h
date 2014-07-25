#ifndef GOTEST_H
#define GOTEST_H

#include "Go.h"
#include <cppunit/extensions/HelperMacros.h>

class GoTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( GoTest );
	CPPUNIT_TEST( testIsLegal );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testIsLegal();
		void isLegalHelper(BoardStruct& test, int x, int y, int colour, int testX, int testY, int testColour, bool result);
};

#endif