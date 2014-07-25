#ifndef PATTERNTEST_H
#define PATTERNTEST_H

#include "Pattern.h"
#include <cppunit/extensions/HelperMacros.h>

class PatternTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( PatternTest );
	CPPUNIT_TEST( testSetAndGetPoint );
	CPPUNIT_TEST( testOriginal );
	CPPUNIT_TEST( testLoadAndSave );
	CPPUNIT_TEST( testSymmetry );
	CPPUNIT_TEST( testDataSize );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testSetAndGetPoint();
		void testOriginal();
		void testLoadAndSave();
		void testSymmetry();
		void testDataSize();
};

#endif // PATTERNTEST_H