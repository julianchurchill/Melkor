#include "GoStringTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( GoStringTest );

void GoStringTest::setUp()
{
	// .w.w.
	// ..www
	// ..w.w
	// ..www
	// ..w.w
	twoWhiteStringBoard.resize(5);
	twoWhiteStringBoard.setPoint(1, 0, WHITE);
	twoWhiteStringBoard.setPoint(3, 0, WHITE);
	twoWhiteStringBoard.setPoint(2, 1, WHITE);
	twoWhiteStringBoard.setPoint(3, 1, WHITE);
	twoWhiteStringBoard.setPoint(4, 1, WHITE);
	twoWhiteStringBoard.setPoint(2, 2, WHITE);
	twoWhiteStringBoard.setPoint(4, 2, WHITE);
	twoWhiteStringBoard.setPoint(2, 3, WHITE);
	twoWhiteStringBoard.setPoint(3, 3, WHITE);
	twoWhiteStringBoard.setPoint(4, 3, WHITE);
	twoWhiteStringBoard.setPoint(2, 4, WHITE);
	twoWhiteStringBoard.setPoint(4, 4, WHITE);
}

void GoStringTest::tearDown()
{
}

void GoStringTest::testContainsPoint()
{
	GoString g( *(twoWhiteStringBoard.getStringConst(3, 0)) );
	CPPUNIT_ASSERT( g.containsPoint(3, 0) );
	CPPUNIT_ASSERT( g.containsPoint(2, 1) );
	CPPUNIT_ASSERT( g.containsPoint(3, 1) );
	CPPUNIT_ASSERT( g.containsPoint(4, 1) );
	CPPUNIT_ASSERT( g.containsPoint(2, 2) );
	CPPUNIT_ASSERT( g.containsPoint(4, 2) );
	CPPUNIT_ASSERT( g.containsPoint(2, 3) );
	CPPUNIT_ASSERT( g.containsPoint(3, 3) );
	CPPUNIT_ASSERT( g.containsPoint(4, 3) );
	CPPUNIT_ASSERT( g.containsPoint(2, 4) );
	CPPUNIT_ASSERT( g.containsPoint(4, 4) );
}

void GoStringTest::testCopyEquality()
{
	BoardStruct test(5);
	test.setPoint(4, 3, BLACK);

	// Test copy constructors
	GoString g1( *(test.getStringConst(4, 3)) );
	GoString g2( *(test.getStringConst(4, 3)) );
	CPPUNIT_ASSERT( g1 == g2 );

	// Test =operator
	g1 = g2;
	CPPUNIT_ASSERT( g1 == g2 );
}

void GoStringTest::testEyes()
{
	CPPUNIT_ASSERT( twoWhiteStringBoard.getStringConst(2, 4) != NULL );
	GoString g = *(twoWhiteStringBoard.getStringConst(2, 4));
	CPPUNIT_ASSERT( g.getEyes().size() == 3 );
	CPPUNIT_ASSERT( g.getRealEyeCount() == 3 );

	CPPUNIT_ASSERT( g.eyeCheck(Point<>(4, 0), twoWhiteStringBoard) );
	CPPUNIT_ASSERT( g.eyeCheck(Point<>(3, 2), twoWhiteStringBoard) );
	CPPUNIT_ASSERT( g.eyeCheck(Point<>(3, 4), twoWhiteStringBoard) );

	// False eye
	CPPUNIT_ASSERT( g.eyeCheck(Point<>(2, 0), twoWhiteStringBoard) == false );
}