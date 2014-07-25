#include "LadderExtensionTest.h"
#include "SearchSettings.h"
#include "BoardStruct.h"
#include "LibertyCountGoal.h"
#include "LadderGoal.h"

CPPUNIT_TEST_SUITE_REGISTRATION( LadderExtensionTest );

void LadderExtensionTest::setUp()
{
}

void LadderExtensionTest::tearDown()
{
}

/** @brief LadderExtension::validExtension() should always return true for 
  * targeted goals where the target string has 2 or less liberties. */
void LadderExtensionTest::testValidExtension()
{
	BoardStruct board(9);

	// Non-targeted goal
	Goal nontargetedGoal(new LibertyCountGoal(0, 0));
	SearchSettings nontargetedSettings(nontargetedGoal);

	// Targeted goal
	Goal targetedGoal(new LadderGoal(0, 0));
	SearchSettings targetedSettings(targetedGoal);

	LadderExtension lEx(targetedSettings);

	// Try with a 2 liberty ladder

	// www......
	// bb.......
	// .........
	board.setPoint(0, 0, WHITE);
	board.setPoint(1, 0, WHITE);
	board.setPoint(2, 0, WHITE);
	board.setPoint(0, 1, BLACK);
	board.setPoint(1, 1, BLACK);

	targetedSettings.setNextToPlay(BLACK);
	CPPUNIT_ASSERT( lEx.validExtension(board, targetedGoal) == true );

	targetedSettings.setNextToPlay(WHITE);
	CPPUNIT_ASSERT( lEx.validExtension(board, targetedGoal) == true );

	// Try with a non-targeted goal
	lEx.setSettings(nontargetedSettings);

	nontargetedSettings.setNextToPlay(BLACK);
	CPPUNIT_ASSERT( lEx.validExtension(board, nontargetedGoal) == false );

	nontargetedSettings.setNextToPlay(WHITE);
	CPPUNIT_ASSERT( lEx.validExtension(board, nontargetedGoal) == false );

	// Now try it with a 1 liberty ladder
	// wwwbb....
	// bb.......
	// .........
	board.setPoint(3, 0, BLACK);
	board.setPoint(4, 0, BLACK);

	// Try with a targeted goal
	lEx.setSettings(targetedSettings);

	targetedSettings.setNextToPlay(BLACK);
	CPPUNIT_ASSERT( lEx.validExtension(board, targetedGoal) == true );

	targetedSettings.setNextToPlay(WHITE);
	CPPUNIT_ASSERT( lEx.validExtension(board, targetedGoal) == true );

	// Try with a non-targeted goal
	lEx.setSettings(nontargetedSettings);

	nontargetedSettings.setNextToPlay(BLACK);
	CPPUNIT_ASSERT( lEx.validExtension(board, nontargetedGoal) == false );

	nontargetedSettings.setNextToPlay(WHITE);
	CPPUNIT_ASSERT( lEx.validExtension(board, nontargetedGoal) == false );
}
