#ifndef GOALINTERFACETEST_H
#define GOALINTERFACETEST_H

#include "Goal.h"
#include <cppunit/extensions/HelperMacros.h>

class GoalInterfaceTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( GoalInterfaceTest );
	CPPUNIT_TEST( testGoalInterfaceAssign );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void tearDown();

		void testGoalInterfaceAssign();
};

#endif