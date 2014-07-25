#ifndef LADDEREXTENSIONTEST_H
#define LADDEREXTENSIONTEST_H

#include "LadderExtension.h"
#include <cppunit/extensions/HelperMacros.h>

class LadderExtensionTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LadderExtensionTest );
	CPPUNIT_TEST( testValidExtension );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testValidExtension();
};

#endif