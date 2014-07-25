#include "SpiralOrdererTest.h"
#include "SpiralOrderer.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SpiralOrdererTest );

void SpiralOrdererTest::setUp()
{
}

void SpiralOrdererTest::tearDown()
{
}

void SpiralOrdererTest::testOrderer()
{
	SpiralOrderer spiralOrderer;

	// Expected point ordering, starting at 2,2
	//
	//  -3-2-1 0 1 2 3
	//
	//-3 . . . M . . .
	//-2 . . N B L . .
	//-1 . O C 4 A K .
	// 0 P D 5 1 3 9 J
	// 1 Q E 6 2 8 I .
	// 2 . R F 7 H . .
	// 3 . . S G . . .

	// Set a start point
	spiralOrderer.initialise(0, 0);

	// 1
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 0 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 0 );

	spiralOrderer.advance();

	// 2
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 0 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 1 );

	spiralOrderer.advance();

	// 3
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 0 );

	spiralOrderer.advance();

	// 4
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 0 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == -1 );

	spiralOrderer.advance();

	// 5
	CPPUNIT_ASSERT( spiralOrderer.currentX() == -1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 0 );

	spiralOrderer.advance();

	// 6
	CPPUNIT_ASSERT( spiralOrderer.currentX() == -1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 1 );

	spiralOrderer.advance();

	// 7
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 0 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 2 );

	spiralOrderer.advance();

	// 8
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 1 );

	spiralOrderer.advance();

	// 9
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 2 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 0 );

	spiralOrderer.advance();

	// A
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == -1 );

	spiralOrderer.advance();

	// B
	CPPUNIT_ASSERT( spiralOrderer.currentX() == 0 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == -2 );

	spiralOrderer.advance();

	// C
	CPPUNIT_ASSERT( spiralOrderer.currentX() == -1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == -1 );

	spiralOrderer.advance();

	// D
	CPPUNIT_ASSERT( spiralOrderer.currentX() == -2 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 0 );

	spiralOrderer.advance();

	// E
	CPPUNIT_ASSERT( spiralOrderer.currentX() == -2 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 1 );

	spiralOrderer.advance();

	// F
	CPPUNIT_ASSERT( spiralOrderer.currentX() == -1 );
	CPPUNIT_ASSERT( spiralOrderer.currentY() == 2 );

}
