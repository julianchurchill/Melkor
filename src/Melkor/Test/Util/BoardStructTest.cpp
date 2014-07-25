#include "BoardStructTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( BoardStructTest );

void BoardStructTest::setUp()
{
	// .....
	// b.www
	// b.wbb
	// ....b
	// ....w
	baseTestBoard5.resize(5);
	// black string bottom right
	baseTestBoard5.setPoint(3, 2, BLACK);
	baseTestBoard5.setPoint(4, 2, BLACK);
	baseTestBoard5.setPoint(4, 3, BLACK);
	// white string middle
	baseTestBoard5.setPoint(2, 1, WHITE);
	baseTestBoard5.setPoint(3, 1, WHITE);
	baseTestBoard5.setPoint(4, 1, WHITE);
	baseTestBoard5.setPoint(2, 2, WHITE);
	// white string bottom right corner
	baseTestBoard5.setPoint(4, 4, WHITE);
	// black string top left
	baseTestBoard5.setPoint(0, 1, BLACK);
	baseTestBoard5.setPoint(0, 2, BLACK);

	//  b..w...
	//  b.www..
	//  .b.w...
	//  .w.bw..
	//  ..bb.b.
	//  ....b.b
	//  .....b.
	libertyCountBoard.resize(7);
	libertyCountBoard.setPoint(0, 0, BLACK);
	libertyCountBoard.setPoint(3, 0, WHITE);
	libertyCountBoard.setPoint(0, 1, BLACK);
	libertyCountBoard.setPoint(2, 1, WHITE);
	libertyCountBoard.setPoint(3, 1, WHITE);
	libertyCountBoard.setPoint(4, 1, WHITE);
	libertyCountBoard.setPoint(1, 2, BLACK);
	libertyCountBoard.setPoint(3, 2, WHITE);
	libertyCountBoard.setPoint(1, 3, WHITE);
	libertyCountBoard.setPoint(3, 3, BLACK);
	libertyCountBoard.setPoint(4, 3, WHITE);
	libertyCountBoard.setPoint(2, 4, BLACK);
	libertyCountBoard.setPoint(3, 4, BLACK);
	libertyCountBoard.setPoint(5, 4, BLACK);
	libertyCountBoard.setPoint(4, 5, BLACK);
	libertyCountBoard.setPoint(6, 5, BLACK);
	libertyCountBoard.setPoint(5, 6, BLACK);
}

void BoardStructTest::tearDown()
{
}

void BoardStructTest::testSimpleSetPoint()
{
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(3, 2) == BLACK );
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(4, 2) == BLACK );
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(4, 3) == BLACK );

	CPPUNIT_ASSERT( baseTestBoard5.getPoint(2, 1) == WHITE );
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(3, 1) == WHITE );
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(4, 1) == WHITE );
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(2, 2) == WHITE );

	CPPUNIT_ASSERT( baseTestBoard5.getPoint(4, 4) == WHITE );

	CPPUNIT_ASSERT( baseTestBoard5.getPoint(0, 1) == BLACK );
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(0, 2) == BLACK );

	// Try to set a point to it's current value
	baseTestBoard5.setPoint(3, 2, BLACK);
	CPPUNIT_ASSERT( baseTestBoard5.getPoint(3, 2) == BLACK );

	// Try to set a point to EMPTY
	BoardStruct test(baseTestBoard5);
	test.setPoint(3, 2, EMPTY);
	CPPUNIT_ASSERT( test.getPoint(3, 2) == EMPTY );
}

void BoardStructTest::testCapture()
{
	BoardStruct test(baseTestBoard5);

	// Add point 3,3 White to cause capture
	test.setPoint(3, 3, WHITE);
	CPPUNIT_ASSERT( test.getPoint(3, 2) == EMPTY );
	CPPUNIT_ASSERT( test.getStringConst(3, 2) == NULL );

	// Add a stone where one was just removed
	test.setPoint(4, 3, BLACK);
	CPPUNIT_ASSERT( test.getPoint(4, 3) == BLACK );

	// .....
	// b.www
	// b.w.w
	// ...wb
	// ...b.
	test.setPoint(3, 4, BLACK);
	test.setPoint(4, 2, WHITE);

	// Try a suicide move
	test.setPoint(3, 2, BLACK);
	CPPUNIT_ASSERT( test.getPoint(3, 2) == EMPTY );

	// .....
	// b.www
	// b.w.w
	// ...w.
	// ...bw
	test.setPoint(4, 4, WHITE);

	// Try a suicide capture move
	CPPUNIT_ASSERT( test.getPoint(4, 3) == EMPTY );
	CPPUNIT_ASSERT( test.getPoint(4, 4) == WHITE );
}

void BoardStructTest::testUpdateStrings()
{
	BoardStruct test(5);
	test.setPoint(4, 3, BLACK);

	CPPUNIT_ASSERT( test.getPoint(4, 3) == BLACK );

	// Retrieve a GoString object and validate it
	CPPUNIT_ASSERT( test.getStringConst(4, 3) != NULL );

	GoString g = *(test.getStringConst(4, 3));
	CPPUNIT_ASSERT( g.getLibertyCount() == 3 );
	CPPUNIT_ASSERT( g.getStoneCount() == 1 );
	CPPUNIT_ASSERT( g.getRealEyeCount() == 0 );
	CPPUNIT_ASSERT( g.getColour() == BLACK );

	// add some more points
	// .....
	// b.www
	// b.wbb
	// ....b
	// ....w
	// black string bottom right
	test.setPoint(3, 2, BLACK);
	test.setPoint(4, 2, BLACK);
	// white string middle
	test.setPoint(2, 1, WHITE);
	test.setPoint(3, 1, WHITE);
	test.setPoint(4, 1, WHITE);
	test.setPoint(2, 2, WHITE);
	// white string bottom right corner
	test.setPoint(4, 4, WHITE);
	// black string top left
	test.setPoint(0, 1, BLACK);
	test.setPoint(0, 2, BLACK);

	// Retrieve a GoString object and validate it
	CPPUNIT_ASSERT( test.getStringConst(3, 2) != NULL );

	g = *(test.getStringConst(3, 2));
	CPPUNIT_ASSERT( g.getLibertyCount() == 1 );
	CPPUNIT_ASSERT( g.getStoneCount() == 3 );
	CPPUNIT_ASSERT( g.getRealEyeCount() == 0 );
	CPPUNIT_ASSERT( g.getColour() == BLACK );

	// Retrieve a GoString object and validate it
	CPPUNIT_ASSERT( test.getStringConst(0, 1) != NULL );

	g = *(test.getStringConst(0, 1));
	CPPUNIT_ASSERT( g.getLibertyCount() == 4 );
	CPPUNIT_ASSERT( g.getStoneCount() == 2 );
	CPPUNIT_ASSERT( g.getRealEyeCount() == 0 );
	CPPUNIT_ASSERT( g.getColour() == BLACK );

	// Retrieve a GoString object and validate it
	CPPUNIT_ASSERT( test.getStringConst(2, 1) != NULL );

	g = *(test.getStringConst(2, 1));
	CPPUNIT_ASSERT( g.getLibertyCount() == 6 );
	CPPUNIT_ASSERT( g.getStoneCount() == 4 );
	CPPUNIT_ASSERT( g.getRealEyeCount() == 0 );
	CPPUNIT_ASSERT( g.getColour() == WHITE );

	// Retrieve a GoString object and validate it
	CPPUNIT_ASSERT( test.getStringConst(4, 4) != NULL );

	g = *(test.getStringConst(4, 4));
	CPPUNIT_ASSERT( g.getLibertyCount() == 1 );
	CPPUNIT_ASSERT( g.getStoneCount() == 1 );
	CPPUNIT_ASSERT( g.getRealEyeCount() == 0 );
	CPPUNIT_ASSERT( g.getColour() == WHITE );

	// Test the liberty count updating mechanism

	//  b*
	//  bw
	BoardStruct test2(2);		
	test2.setPoint(0, 0, BLACK);
	test2.setPoint(0, 1, BLACK);
	test2.setPoint(1, 1, WHITE);

	CPPUNIT_ASSERT( test2.getStringConst(0, 0) != NULL );
	g = *(test2.getStringConst(0, 0));
	CPPUNIT_ASSERT( g.getLibertyCount() == 1 );

	CPPUNIT_ASSERT( libertyCountBoard.getStringConst(0, 0) != NULL );
	g = *(libertyCountBoard.getStringConst(0, 0));
	CPPUNIT_ASSERT( g.getLibertyCount() == 3 );

	CPPUNIT_ASSERT( libertyCountBoard.getStringConst(3, 0) != NULL );
	g = *(libertyCountBoard.getStringConst(3, 0));
	CPPUNIT_ASSERT( g.getLibertyCount() == 6 );

	CPPUNIT_ASSERT( libertyCountBoard.getStringConst(1, 2) != NULL );
	g = *(libertyCountBoard.getStringConst(1, 2));
	CPPUNIT_ASSERT( g.getLibertyCount() == 3 );

	CPPUNIT_ASSERT( libertyCountBoard.getStringConst(1, 3) != NULL );
	g = *(libertyCountBoard.getStringConst(1, 3));
	CPPUNIT_ASSERT( g.getLibertyCount() == 3 );

	CPPUNIT_ASSERT( libertyCountBoard.getStringConst(3, 3) != NULL );
	g = *(libertyCountBoard.getStringConst(3, 3));
	CPPUNIT_ASSERT( g.getLibertyCount() == 5 );

	CPPUNIT_ASSERT( libertyCountBoard.getStringConst(4, 3) != NULL );
	g = *(libertyCountBoard.getStringConst(4, 3));
	CPPUNIT_ASSERT( g.getLibertyCount() == 3 );
}