#ifndef BOARDSTRUCTTEST_H
#define BOARDSTRUCTTEST_H

#include "BoardStruct.h"
#include <cppunit/extensions/HelperMacros.h>

class BoardStructTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( BoardStructTest );
	CPPUNIT_TEST( testSimpleSetPoint );
	CPPUNIT_TEST( testCapture );
	CPPUNIT_TEST( testUpdateStrings );
	CPPUNIT_TEST_SUITE_END();

	private:
		BoardStruct baseTestBoard5;
		BoardStruct libertyCountBoard;

	public:
		void setUp();
		void tearDown();

		void testSimpleSetPoint();
		void testCapture();
		void testUpdateStrings();
};

#endif