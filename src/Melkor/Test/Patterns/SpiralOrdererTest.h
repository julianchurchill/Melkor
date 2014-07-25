#ifndef SPIRALORDERERTEST_H
#define SPIRALORDERERTEST_H

#include "SpiralOrderer.h"
#include <cppunit/extensions/HelperMacros.h>

class SpiralOrdererTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( SpiralOrdererTest );
	CPPUNIT_TEST( testOrderer );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testOrderer();
};

#endif