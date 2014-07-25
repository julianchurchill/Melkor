#ifndef ALLLEGALSUGGESTERTEST_H
#define ALLLEGALSUGGESTERTEST_H

#include "AllLegalSuggester.h"
#include <cppunit/extensions/HelperMacros.h>

class AllLegalSuggesterTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( AllLegalSuggesterTest );
	CPPUNIT_TEST( testGetMoves );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testGetMoves();
};

#endif