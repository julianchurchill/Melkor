#include "GoTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( GoTest );

void GoTest::setUp()
{
}

void GoTest::tearDown()
{
}

void GoTest::testIsLegal()
{
	bool savedDontFillOwnEyes = Go::dontFillOwnEyes;
	
	BoardStruct test(7);
	test.setPoint(0, 0, BLACK);
	test.setPoint(3, 0, WHITE);
	test.setPoint(0, 1, BLACK);
	test.setPoint(1, 1, WHITE);
	test.setPoint(2, 1, WHITE);
	test.setPoint(3, 1, WHITE);
	test.setPoint(4, 1, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, WHITE);
	test.setPoint(0, 3, BLACK);
	test.setPoint(1, 3, WHITE);
	test.setPoint(3, 3, BLACK);
	test.setPoint(4, 3, WHITE);
	test.setPoint(2, 4, BLACK);
	test.setPoint(3, 4, BLACK);
	test.setPoint(5, 4, BLACK);
	test.setPoint(4, 5, BLACK);
	test.setPoint(6, 5, BLACK);
	test.setPoint(5, 6, BLACK);
	// setup suicide capture
	test.setPoint(0, 5, WHITE);
	test.setPoint(0, 6, BLACK);
	test.setPoint(1, 5, BLACK);
	test.setPoint(2, 6, BLACK);
	// setup Ko
	test.setPoint(0, 2, WHITE);
	Move m(1, 2, BLACK, test.getSize());
	test.addToBoard(m);
	//  b**w***
	//  bwwww**
	//  *bww***
	//  bw*bw**
	//  **bb*b*
	//  wb**b*b
	//  b*b**b*

	Go::resetIllegalMoveMemory(test, Go::getUseSuperKo());

	// Get all current illegal moves - should only be W(5,5) and W(6,6)
	vector<Move>& moves = test.getIllegalMoveMemory();
	CPPUNIT_ASSERT( moves.size() == 2 );

	// Run the tests once with illegal move memory and once without
	bool useIMM = true;
	vector<Move> cp;
	for(int i=0;i<2;i++) {

		if(i==1) useIMM = false;

		// suicide test
		CPPUNIT_ASSERT( Go::isLegal(test, 5, 5, WHITE, Go::getUseSuperKo(), useIMM) == false );
		CPPUNIT_ASSERT( Go::isLegal(test, 6, 6, WHITE, Go::getUseSuperKo(), useIMM) == false );
	
		// normal move
		CPPUNIT_ASSERT( Go::isLegal(test, 2, 3, WHITE, Go::getUseSuperKo(), useIMM) == true );

		// suicide capture test
		CPPUNIT_ASSERT( Go::isLegal(test, 1, 6, WHITE, Go::getUseSuperKo(), useIMM) == true );

		// ko test
		CPPUNIT_ASSERT( Go::isLegal(test, 0, 2, WHITE, Go::getUseSuperKo(), useIMM) == false );

		// superko test
		BoardStruct superKoTest(test);
		// Current board
		//  b**w***
		//  bwwww**
		//  *bww***
		//  bw*bw**
		//  **bb*b*
		//  wb**b*b
		//  b*b**b*
		superKoTest.setPoint(1, 4, WHITE);
		superKoTest.setPoint(2, 5, WHITE);
		superKoTest.setPoint(1, 6, WHITE);
		superKoTest.setPoint(4, 4, WHITE);
		superKoTest.setPoint(5, 3, WHITE);
		superKoTest.setPoint(6, 4, WHITE);
		superKoTest.setPoint(3, 5, BLACK);
		// New board
		//  b**w***
		//  bwwww**
		//  *bww***
		//  bw*bww*
		//  *wbbwbw
		//  w*wbb*b
		//  *wb**b*
		Go::resetMoveList();
		// white takes first Ko
		m.setCoords(0, 2);
		m.setColour(WHITE);
		Go::addToMoveList(m, superKoTest);
		superKoTest.addToBoard(m);
		// black takes second ko
		m.setCoords(1, 5);
		m.setColour(BLACK);
		Go::addToMoveList(m, superKoTest);
		superKoTest.addToBoard(m);
		// white takes third ko
		m.setCoords(5, 5);
		m.setColour(WHITE);
		Go::addToMoveList(m, superKoTest);
		superKoTest.addToBoard(m);
		// Board is now
		//  b**w***
		//  bwwww**
		//  w*ww***
		//  bw*bww*
		//  *wbbw*w
		//  wb*bbwb
		//  *wb**b*
		// black takes back first ko
		m.setCoords(1, 2);
		m.setColour(BLACK);
		Go::addToMoveList(m, superKoTest);
		superKoTest.addToBoard(m);
		// white takes back second ko
		m.setCoords(2, 5);
		m.setColour(WHITE);
		Go::addToMoveList(m, superKoTest);
		superKoTest.addToBoard(m);
		Go::resetIllegalMoveMemory(superKoTest, true);
		// can black take back third ko???
		// should be no, since this goes back to the start position, e.g. a super-ko situation
		CPPUNIT_ASSERT( Go::isLegal(superKoTest, 5, 4, BLACK, true, useIMM) == false );

		// Created surrounded one point test
		// should recognise the point as being illegal for the opposite colour
		m.setCoords(5, 2);
		m.setColour(WHITE);
		Go::addToMoveList(m, superKoTest);
		superKoTest.addToBoard(m);
		cp.clear();
		cp.push_back(m);
		CPPUNIT_ASSERT( Go::isLegal(superKoTest, 4, 2, BLACK, Go::getUseSuperKo(), useIMM, &cp) == false );

	} // end for i

	// more tricky tests

	// Single illegal suicide point test should be false
	// .b..
	// b...
	// .b..
	// ....
	test.resize(4);
	test.setPoint(1, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(1, 2, BLACK);
	isLegalHelper(test, 2, 1, BLACK, 1, 1, WHITE, false);

	// Single illegal suicide point test 2 should be false
	// .bb.
	// bw..
	// .bb.
	// ....
	test.clear();
	test.setPoint(1, 0, BLACK);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(1, 1, WHITE);
	test.setPoint(1, 2, BLACK);
	test.setPoint(2, 2, BLACK);
	// add black stone at 3, 1, should make 2,1 illegal for white
	isLegalHelper(test, 3, 1, BLACK, 2, 1, WHITE, false);
	
	// Suicide point with capture test should be true
	// .wb.
	// wb..
	// .wb.
	// ....
	test.clear();
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, WHITE);
	test.setPoint(1, 1, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, BLACK);
	// add black stone at 3,1, should make 2,1 legal for white
	isLegalHelper(test, 3, 1, BLACK, 2, 1, WHITE, true);

	// Suicide point with capture test 2 should be true
	// .bb.
	// bw..
	// wbbw
	// .ww.
	test.clear();
	test.setPoint(1, 0, BLACK);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(1, 1, WHITE);
	test.setPoint(0, 2, WHITE);
	test.setPoint(1, 2, BLACK);
	test.setPoint(2, 2, BLACK);
	test.setPoint(3, 2, WHITE);
	test.setPoint(1, 3, WHITE);
	test.setPoint(2, 3, WHITE);
	// add black stone at 3,1, should make 2,1 legal for white
	isLegalHelper(test, 3, 1, BLACK, 2, 1, WHITE, true);

	// Suicide point for side playing test should be false
	// .ww.
	// w..w
	// .ww.
	// ....
	test.clear();
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(3, 1, WHITE);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	// add black stone at 2,1, should make 1,1 illegal for black
	isLegalHelper(test, 2, 1, BLACK, 1, 1, BLACK, false);

	// Suicide point for side playing test 2 should be false
	// .www
	// w.b.
	// .www
	// ....
	test.clear();
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, WHITE);
	test.setPoint(3, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(2, 1, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, WHITE);
	// add black stone at 3,1, should make 1,1 illegal for black
	isLegalHelper(test, 3, 1, BLACK, 1, 1, BLACK, false);

	// Suicide-capture point for side playing test should be true
	// .ww.
	// w..w
	// bwwb
	// .bb.
	test.clear();
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(3, 1, WHITE);
	test.setPoint(0, 2, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, BLACK);
	test.setPoint(1, 3, BLACK);
	test.setPoint(2, 3, BLACK);
	// add black stone at 2,1, should make 1,1 legal for black
	isLegalHelper(test, 2, 1, BLACK, 1, 1, BLACK, true);

	// Enemy suicide point test should be false
	// .b..
	// b...
	// bwwb
	// .bb.
	test.clear();
	test.setPoint(1, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(0, 2, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, BLACK);
	test.setPoint(1, 3, BLACK);
	test.setPoint(2, 3, BLACK);
	// add black stone at 2,1, should make 1,1 illegal for white
	isLegalHelper(test, 2, 1, BLACK, 1, 1, WHITE, false);

	// Enemy suicide-capture point test should be true
	// .bw.
	// b..w
	// bwwb
	// .bb.
	test.clear();
	test.setPoint(1, 0, BLACK);
	test.setPoint(2, 0, WHITE);
	test.setPoint(0, 1, BLACK);
	test.setPoint(3, 1, WHITE);
	test.setPoint(0, 2, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, BLACK);
	test.setPoint(1, 3, BLACK);
	test.setPoint(2, 3, BLACK);
	// add black stone at 2,1, should make 1,1 legal for white
	isLegalHelper(test, 2, 1, BLACK, 1, 1, WHITE, true);

	// Suicide-capture point converted to suicide point test should be false
	// .wb.
	// wb.b
	// .wb.
	// ....
	test.clear();
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, WHITE);
	test.setPoint(1, 1, BLACK);
	test.setPoint(3, 1, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, BLACK);
	// add empty point at 0,1, should make 2,1 illegal for white
	isLegalHelper(test, 0, 1, EMPTY, 2, 1, WHITE, false);

	// Suicide-capture point converted to suicide point test 2 should be false
	// .wbw
	// wwbw
	// bb..
	// ....
	test.clear();
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, BLACK);
	test.setPoint(3, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(1, 1, WHITE);
	test.setPoint(2, 1, BLACK);
	test.setPoint(3, 1, WHITE);
	test.setPoint(0, 2, BLACK);
	test.setPoint(1, 2, BLACK);
	// add white stone at 2,2, should make 0,0 illegal for black
	isLegalHelper(test, 2, 2, WHITE, 0, 0, BLACK, false);

	// Test 12:
	// ....
	// ....
	// wb..
	// .wb.
	test.clear();
	test.setPoint(0, 2, WHITE);
	test.setPoint(1, 2, BLACK);
	test.setPoint(1, 3, WHITE);
	test.setPoint(2, 3, BLACK);
	// Filling in fake eye at 0,3 for white should always be legal, even with Go::dontFillOwnEyes
	Go::dontFillOwnEyes = true;
	isLegalHelper(test, -1, -1, BLACK, 0, 3, WHITE, true);

	// Filling in fake eye at 0,3 for black should also be legal
	isLegalHelper(test, -1, -1, BLACK, 0, 3, BLACK, true);

	Go::dontFillOwnEyes = savedDontFillOwnEyes;
}

void GoTest::isLegalHelper(BoardStruct& test, int x, int y, int colour, int testX, int testY, int testColour, bool result)
{
	Go::resetIllegalMoveMemory(test, Go::getUseSuperKo());
	Move m(x, y, colour, test.getSize());
	test.addToBoard(m);
	vector<Move> cp;
	cp.push_back(m);
	vector<Point<> > cs = test.getCapturedStonesList();
	for(int i=0;i<cs.size();i++)
		cp.push_back(Move(cs[i].x, cs[i].y, EMPTY, test.getSize()));

	CPPUNIT_ASSERT( Go::isLegal(test, testX, testY, testColour, Go::getUseSuperKo(), true, &cp) == result );
}
