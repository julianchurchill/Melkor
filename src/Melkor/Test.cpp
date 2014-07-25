/**************************************************/
/* A collection of functions for feature testing. */
/**************************************************/

#pragma warning( disable : 4786 )

#include "globals.h"
#include "Test.h"
#include "BoardStruct.h"
#include "BPNGoAdapter.h"
#include "BPNTrainer.h"
#include "RPROPBPNTrainer.h"
#include "NNDatabase.h"
#include "Matrix.h"
#include "Go.h"
#include "SGFReader.h"
#include "BPN.h"
#include "Melkor.h"
#include "CaptureGoal.h"
#include "InfluenceGoal.h"
#include "LadderGoal.h"
#include "LibertyCountGoal.h"
#include "StoneCountGoal.h"
#include "Misc.h"
#include "AllLegalSuggester.h"
#include "GeneralSuggester.h"
#include "LadderSuggester.h"
#include <windows.h>
#include <algorithm>
#include <ctime>

// DO_TIMING stops all messages for gamewalkthrough
// and causes average time per move and average rank
// of actual move to be printed at the end of the play through
// It also automatically steps through the SGF
// without waiting for the user to press forward.
//#define DO_TIMING

const string Test::myName = "Test";

// the second argument is a failsafe so we don't get a type mismatch
// when specifying the void*
Test::Test(void* m, bool useMelkorType)
{
	melkor = m;
	// check morgoth type is Melkor*
	if(((Melkor*)melkor)->getName()!="Melkor") {
		LogWriter::printerr("Invalid Melkor* in Test()\n", "");
		exit(0);
	}
}

/** @note This function has been deprecated and will be removed sometime in the future.
  * The test has been changed into a unit test, see Test\Util\GoTest.cpp */
void Test::IsLegalTest(bool verbose /*=false*/)
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
	//test.setPoint(1, 2, BLACK);
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
	LogWriter log;
	log.println("IsLegal test", myName);

	string message;
	char buffer[50];
	// number of tests passed
	int passes = 0;
	int totalTests = 0;

	Go::resetIllegalMoveMemory(test, Go::getUseSuperKo());

	// suicide test
	if(verbose) {
		// print the board
		test.printAsciiBoard();
		// print all legal moves atm
		vector<Move>& m = test.getIllegalMoveMemory();
		message = "Current illegal moves: ";
		for(int i=0;i<m.size();i++) {
			if(m[i].getColour()==BLACK)
				message+="B";
			else
				message+="W";
			message+="[";
			sprintf(buffer, "%d", m[i].getX());
			message+=buffer;
			message+=",";
			sprintf(buffer, "%d", m[i].getY());
			message+=buffer;
			message+="] ";
		}
		log.println(message, myName);
		log.println("Should be W[5,5] and W[6,6] only.", myName);
	}

	bool useIMM = true;
	vector<Move> cp;
	for(int i=0;i<2;i++) {
		if(i==0)
			log.println("Testing with illegalMoveMemory...", myName);
		else {
			log.println("Testing without illegalMoveMemory...", myName);
			useIMM = false;
		}

		// suicide test
		if(verbose) 
			message = "Suicide test result should be false: Result = ";
		totalTests++;
		if(Go::isLegal(test, 5, 5, WHITE, Go::getUseSuperKo(), useIMM))			
			message+="true";  // test failed
		else {
			// test passed
			passes++;
			message+="false";
		}
	
		// normal move
		if(verbose) {
			log.println(message, myName);
			message = "Normal move test result should be true: Result = ";
		}
		totalTests++;
		if(Go::isLegal(test, 2, 3, WHITE, Go::getUseSuperKo(), useIMM)) {
			// test passed
			passes++;
			message+="true";
		}
		else
			message+="false";   // test failed

		// suicide capture test
		if(verbose) {
			log.println(message, myName);
			message = "Suicide capture test result should be true: Result = ";
		}
		totalTests++;
		if(Go::isLegal(test, 1, 6, WHITE, Go::getUseSuperKo(), useIMM)) {
			// test passed
			passes++;
			message+="true";
		}
		else			
			message+="false";   // test failed

		// ko test
		if(verbose) {
			log.println(message, myName);
			message = "Ko test result should be false: Result = ";
		}
		totalTests++;
		if(Go::isLegal(test, 0, 2, WHITE, Go::getUseSuperKo(), useIMM))			
			message+="true";   // test failed
		else {
			// test passed
			passes++;
			message+="false";
		}
		// superko test
		if(verbose) {
			log.println(message, myName);
			message = "Super ko test should be false: Result = ";
		}
		// Current board
		//  b**w***
		//  bwwww**
		//  *bww***
		//  bw*bw**
		//  **bb*b*
		//  wb**b*b
		//  b*b**b*
		((Melkor*)melkor)->getBoard() = test;
		((Melkor*)melkor)->getBoard().setPoint(1, 4, WHITE);
		((Melkor*)melkor)->getBoard().setPoint(2, 5, WHITE);
		((Melkor*)melkor)->getBoard().setPoint(1, 6, WHITE);
		((Melkor*)melkor)->getBoard().setPoint(4, 4, WHITE);
		((Melkor*)melkor)->getBoard().setPoint(5, 3, WHITE);
		((Melkor*)melkor)->getBoard().setPoint(6, 4, WHITE);
		((Melkor*)melkor)->getBoard().setPoint(3, 5, BLACK);
		// print the board
		((Melkor*)melkor)->getBoard().printAsciiBoard();
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
		Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(m);
		// black takes second ko
		m.setCoords(1, 5);
		m.setColour(BLACK);
		Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(m);
		// white takes third ko
		m.setCoords(5, 5);
		m.setColour(WHITE);
		Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(m);
		// print the board
		((Melkor*)melkor)->getBoard().printAsciiBoard();
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
		Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(m);
		// white takes back second ko
		m.setCoords(2, 5);
		m.setColour(WHITE);
		Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(m);
		Go::resetIllegalMoveMemory(((Melkor*)melkor)->getBoard(), Go::getUseSuperKo());
		// can black take back third ko???
		// should be no, since this goes back to the start
		// position, e.g. a super-ko situation
		totalTests++;
		if(Go::isLegal(((Melkor*)melkor)->getBoard(), 5, 4, BLACK, Go::getUseSuperKo(), useIMM)) {
			// print the board
			((Melkor*)melkor)->getBoard().printAsciiBoard();
			// test failed
			message+="true";
		}
		else {
			// test passed
			passes++;
			message+="false";
		}
		if(verbose) {
			log.println(message, myName);
			message = "Created surrounded one point test should be false: Result = ";
		}

		// Created surrounded one point test
		// should recognise the point as being illegal for the opposite colour
		m.setCoords(5, 2);
		m.setColour(WHITE);
		Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(m);
		//((Melkor*)melkor)->getBoard().printAsciiBoard();
		cp.clear();
		cp.push_back(m);
		totalTests++;
		if(Go::isLegal(((Melkor*)melkor)->getBoard(), 4, 2, BLACK, Go::getUseSuperKo(), useIMM, &cp))
			message+="true";   // test failed
		else {
			// test passed
			passes++;
			message+="false";
		}
		if(verbose) log.println(message, myName);
	} // end for i

	// more tricky tests
	test.resize(4);
	// .b..
	// b...
	// .b..
	// ....
	test.setPoint(1, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(1, 2, BLACK);
	totalTests++;
	if(isLegalHelper("Single illegal suicide point test should be false", 
					 test, 2, 1, BLACK, 1, 1, WHITE, false, verbose))
		passes++;

	// Test 2:
	test.clear();
	// .bb.
	// bw..
	// .bb.
	// ....
	test.setPoint(1, 0, BLACK);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(1, 1, WHITE);
	test.setPoint(1, 2, BLACK);
	test.setPoint(2, 2, BLACK);
	// add black stone at 3, 1
	// should make 2,1 illegal for white
	totalTests++;
	if(isLegalHelper("Single illegal suicide point test 2 should be false",
					 test, 3, 1, BLACK, 2, 1, WHITE, false, verbose))
		passes++;
	
	// Test 3:
	test.clear();
	// .wb.
	// wb..
	// .wb.
	// ....
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, WHITE);
	test.setPoint(1, 1, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, BLACK);
	// add black stone at 3,1
	// should make 2,1 legal for white
	totalTests++;
	if(isLegalHelper("Suicide point with capture test should be true",
					 test, 3, 1, BLACK, 2, 1, WHITE, true, verbose))
		passes++;

	// Test 4:
	test.clear();
	// .bb.
	// bw..
	// wbbw
	// .ww.
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
	// add black stone at 3,1
	// should make 2,1 legal for white
	totalTests++;
	if(isLegalHelper("Suicide point with capture test 2 should be true",
					 test, 3, 1, BLACK, 2, 1, WHITE, true, verbose))
		passes++;

	// Test 5:
	test.clear();
	// .ww.
	// w..w
	// .ww.
	// ....
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(3, 1, WHITE);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	// add black stone at 2,1
	// should make 1,1 illegal for black
	totalTests++;
	if(isLegalHelper("Suicide point for side playing test should be false",
					 test, 2, 1, BLACK, 1, 1, BLACK, false, verbose))
		passes++;

	// Test 6:
	test.clear();
	// .www
	// w.b.
	// .www
	// ....
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, WHITE);
	test.setPoint(3, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(2, 1, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, WHITE);
	// add black stone at 3,1
	// should make 1,1 illegal for black
	totalTests++;
	if(isLegalHelper("Suicide point for side playing test 2 should be false",
					 test, 3, 1, BLACK, 1, 1, BLACK, false, verbose))
		passes++;

	// Test 7:
	test.clear();
	// .ww.
	// w..w
	// bwwb
	// .bb.
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
	// add black stone at 2,1
	// should make 1,1 legal for black
	totalTests++;
	if(isLegalHelper("Suicide-capture point for side playing test should be true",
					 test, 2, 1, BLACK, 1, 1, BLACK, true, verbose))
		passes++;

	// Test 8:
	test.clear();
	// .b..
	// b...
	// bwwb
	// .bb.
	test.setPoint(1, 0, BLACK);
	test.setPoint(0, 1, BLACK);
	test.setPoint(0, 2, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, WHITE);
	test.setPoint(3, 2, BLACK);
	test.setPoint(1, 3, BLACK);
	test.setPoint(2, 3, BLACK);
	// add black stone at 2,1
	// should make 1,1 illegal for white
	totalTests++;
	if(isLegalHelper("Enemy suicide point test should be false",
					 test, 2, 1, BLACK, 1, 1, WHITE, false, verbose))
		passes++;

	// Test 9:
	test.clear();
	// .bw.
	// b..w
	// bwwb
	// .bb.
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
	// add black stone at 2,1
	// should make 1,1 legal for white
	totalTests++;
	if(isLegalHelper("Enemy suicide-capture point test should be true",
					 test, 2, 1, BLACK, 1, 1, WHITE, true, verbose))
		passes++;

	// Test 10:
	test.clear();
	// .wb.
	// wb.b
	// .wb.
	// ....
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, BLACK);
	test.setPoint(0, 1, WHITE);
	test.setPoint(1, 1, BLACK);
	test.setPoint(3, 1, BLACK);
	test.setPoint(1, 2, WHITE);
	test.setPoint(2, 2, BLACK);
	// add empty point at 0,1
	// should make 2,1 illegal for white
	totalTests++;
	if(isLegalHelper("Suicide-capture point converted to suicide point test should be false",
					 test, 0, 1, EMPTY, 2, 1, WHITE, false, verbose))
		passes++;

	// Test 11:
	test.clear();
	// .wbw
	// wwbw
	// bb..
	// ....
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 0, BLACK);
	test.setPoint(3, 0, WHITE);
	test.setPoint(0, 1, WHITE);
	test.setPoint(1, 1, WHITE);
	test.setPoint(2, 1, BLACK);
	test.setPoint(3, 1, WHITE);
	test.setPoint(0, 2, BLACK);
	test.setPoint(1, 2, BLACK);
	// add white stone at 2,2
	// should make 0,0 illegal for black
	totalTests++;
	if(isLegalHelper("Suicide-capture point converted to suicide point test 2 should be false",
					 test, 2, 2, WHITE, 0, 0, BLACK, false, verbose))
		passes++;

	// Test 12:
	test.clear();
	// ....
	// ....
	// wb..
	// .wb.
	test.setPoint(0, 2, WHITE);
	test.setPoint(1, 2, BLACK);
	test.setPoint(1, 3, WHITE);
	test.setPoint(2, 3, BLACK);
	// white at 0,3 should always be legal, even with Go::dontFillOwnEyes
	Go::dontFillOwnEyes = true;
	totalTests++;
	if(isLegalHelper("Filling in fake eye at 0,3 for white should be true",
					test, -1, -1, BLACK, 0, 3, WHITE, true, verbose))
		passes++;
	// black at 0,3 should also be legal
	totalTests++;
	if(isLegalHelper("Filling in fake eye at 0,3 for black should be true",
					test, -1, -1, BLACK, 0, 3, BLACK, true, verbose))
		passes++;

	message = "Passes : ";
	sprintf(buffer, "%d", passes);
	message+=buffer;
	message+="/";
	// out of how many tests?
	sprintf(buffer, "%d", totalTests);
	message+=buffer;
	log.println(message, myName);

	Go::dontFillOwnEyes = savedDontFillOwnEyes;

	// wait for user to proceed
	log.println("Press enter.");

	char temp;
	cin.get(temp);
}

bool Test::isLegalHelper(const char* title, BoardStruct& test, int x, int y, int colour, 
						 int testX, int testY, int testColour, bool result, bool verbose)
{
	Go::resetIllegalMoveMemory(test, Go::getUseSuperKo());
	Move m(x, y, colour, test.getSize());
	test.addToBoard(m);
	vector<Move> cp;
	cp.push_back(m);
	vector<Point<> > cs = test.getCapturedStonesList();
	for(int i=0;i<cs.size();i++)
		cp.push_back(Move(cs[i].x, cs[i].y, EMPTY, test.getSize()));

	string message = title;
	LogWriter log;
	message+=": Result = ";
	bool r = Go::isLegal(test, testX, testY, testColour, Go::getUseSuperKo(), true, &cp);
	if(r)
		message+="true";
	else
		message+="false";
	if(verbose)
		log.println(message, myName);
	// test passed
	if(r==result)
		return true;
	// test failed
	else 
	{
		if(verbose)
			log.println("***Failed test ***", myName);
		return false;
	}
}

/** @note This function has been deprecated and will be removed sometime in the future.
  * The test has been changed into a unit test, see Test\Util\BoardStructTest.cpp */
void Test::LibertyCountTest(bool verbose /*=false*/)
{
	//  b*
	//  bw
	BoardStruct test2(2);		
	test2.setPoint(0, 0, BLACK);
	test2.setPoint(0, 1, BLACK);
	test2.setPoint(1, 1, WHITE);
	LogWriter log;
	string message;
	char buffer[50];
	// number of tests passed
	int passes = 0;
	
	log.println("LibertyCount test", myName);

	if(verbose) {
		log.println("Board 1 count should be 1", myName);
		message = "Test result: ";
	}
	
	GoString s = *(test2.getStringConst(0, 0));
	
	if(verbose) {
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==1) passes++;
	
	BoardStruct test(7);
	test.setPoint(0, 0, BLACK);
	test.setPoint(3, 0, WHITE);
	test.setPoint(0, 1, BLACK);
	test.setPoint(2, 1, WHITE);
	test.setPoint(3, 1, WHITE);
	test.setPoint(4, 1, WHITE);
	test.setPoint(1, 2, BLACK);
	test.setPoint(3, 2, WHITE);
	test.setPoint(1, 3, WHITE);
	test.setPoint(3, 3, BLACK);
	test.setPoint(4, 3, WHITE);
	test.setPoint(2, 4, BLACK);
	test.setPoint(3, 4, BLACK);
	test.setPoint(5, 4, BLACK);
	test.setPoint(4, 5, BLACK);
	test.setPoint(5, 5, WHITE);
	test.setPoint(6, 5, BLACK);
	test.setPoint(5, 6, BLACK);
	//  b**w***
	//  b*www**
	//  *b*w***
	//  *w*bw**
	//  **bb*b*
	//  ****bwb
	//  *****b*
	s = *(test.getStringConst(0, 0));
	if(verbose)	{
		log.println("Board 2 count should be 3", myName);
		message = "Test result: ";
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==3) passes++;
	s = *(test.getStringConst(3, 0));
	if(verbose)	{
		log.println("Board 2 count should be 6", myName);
		message = "Test result: ";
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==6) passes++;
	s = *(test.getStringConst(1, 2));
	if(verbose)	{
		log.println("Board 2 count should be 3", myName);
		message = "Test result: ";
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==3) passes++;
	s = *(test.getStringConst(1, 3));
	if(verbose)	{
		log.println("Board 2 count should be 3", myName);
		message = "Test result: ";
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==3) passes++;
	s = *(test.getStringConst(3, 3));
	if(verbose)	{
		log.println("Board 2 count should be 5", myName);
		message = "Test result: ";
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==5) passes++;
	s = *(test.getStringConst(4, 3));
	if(verbose)	{
		log.println("Board 2 count should be 3", myName);
		message = "Test result: ";
		sprintf(buffer, "%d", s.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
	}
	if(s.getLibertyCount()==3) passes++;

	message = "Passes : ";
	sprintf(buffer, "%d", passes);
	message+=buffer;
	message+="/";
	// out of how many tests?
	sprintf(buffer, "%d", 7);
	message+=buffer;
	log.println(message, myName);

	// wait for user to proceed
	log.println("Press enter.");
	char temp;
	cin.get(temp);
}

/** @note This function has been deprecated and will be removed sometime in the future.
  * The test has been changed into a unit test, see Test\Util\GoStringsTest.cpp */
void Test::GoStringsTest(bool verbose /*=false*/)
{
	LogWriter log;
	string message;
	char buffer[50];
	// number of tests passed
	int passes = 0;
	char temp;

	log.println("GoStrings test", myName);

	BoardStruct test(5);
	if(verbose)	{
		log.println("Press a key at each pause to proceed.", myName);
		log.println("Created 5x5 test board", myName);
		cin.get(temp);
	}

	test.setPoint(4, 3, BLACK);
	if(verbose)	{
		log.println("Set a point on the board at (4, 3)" ,myName);
		cin.get(temp);
	}

	GoString g = *(test.getStringConst(4, 3));
	if(verbose)	{
		log.println("Retrieved string at (4, 3) ok", myName);
		message = "String at 4,3 liberties = ";
		sprintf(buffer, "%d", g.getLibertyCount());
		message+=buffer;
		log.println(message, myName);
		cin.get(temp);
	}
	if(g.getLibertyCount()==3) passes++;

	// add some more points
	/*
	-----
	B-WWW
	B-WBB
	----B
	----W
	*/
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

	if(verbose) test.printAsciiBoard();

	GoString b1 = *(test.getStringConst(3, 2));
	if(verbose)	{
		log.println("String b1, bottom right: ", myName);
		b1.printInfo();
		cin.get(temp);
	}
	if(b1.getColour()==BLACK && b1.getLibertyCount()==1) passes++;

	GoString b2 = *(test.getStringConst(0, 1));
	if(verbose)	{
		log.println("String b2, top left: ", myName);
		b2.printInfo();
		cin.get(temp);
	}
	if(b2.getColour()==BLACK && b2.getLibertyCount()==4) passes++;

	GoString w1 = *(test.getStringConst(2, 1));
	if(verbose)	{
		log.println("String w1, middle: ", myName);
		w1.printInfo();
		cin.get(temp);
	}
	if(w1.getColour()==WHITE && w1.getLibertyCount()==6) passes++;

	GoString w2 = *(test.getStringConst(4, 4));
	if(verbose)	{
		log.println("String w2, bottom right corner: ", myName);
		w2.printInfo();
		cin.get(temp);
	}
	if(w2.getColour()==WHITE && w2.getLibertyCount()==1) passes++;

	// try a capture
	test.setPoint(3, 3, WHITE);
	if(verbose)	{
		log.println("Adding point 3,3 White to cause capture", myName);
		test.printAsciiBoard();
	}

	if(test.getPoint(3, 2)==EMPTY) {
		passes++;
		if(verbose) {
			log.println("Old black string has been removed from string table.", myName);
			cin.get(temp);
		}
	}
	else {
		if(verbose) {
			log.println("Error: Old black string has NOT been removed from string table.", myName);
			cin.get(temp);
		}
	}

	if(verbose) {
		test.printStringInfo();
		cin.get(temp);
	}

	// try suicide capture
	test.setPoint(4, 3, BLACK);
	if(verbose)	{
		log.println("Adding 4,3 for BLACK.", myName);
		test.printAsciiBoard();
		cin.get(temp);
	}
	if(test.getPoint(4, 3)==EMPTY) {
		if(verbose) {
			log.println("String at 4,3 was removed by mistake....", myName);
			cin.get(temp);
		}
	}
	else {
		passes++;
		if(verbose)	{
			b1.printInfo();
			cin.get(temp);
		}
	}
	if(verbose)	log.println("Setting up board...", myName);
	test.setPoint(3, 4, BLACK);
	test.setPoint(4, 2, WHITE);

	// suicide move
	if(verbose) {
		test.printAsciiBoard();
		cin.get(temp);
		log.println("Attempting suicide capture test, White move at 4,4", myName);
	}
	test.setPoint(4, 4, WHITE);
	// .....
	// b.www
	// b.w.w
	// ...w.
	// ...bw
	if(verbose)	{
		test.printAsciiBoard();
		cin.get(temp);
		test.printStringInfo();
		cin.get(temp);
	}

	/** Eye Counting Test **/

	if(verbose)	log.println("Setting up board for eye test.", myName);

	test.setPoint(3, 0, WHITE);
	test.setPoint(1, 0, WHITE);
	test.setPoint(2, 3, WHITE);
	test.setPoint(4, 3, WHITE);
	test.setPoint(2, 4, WHITE);

	// .w.w.
	// b.www
	// b.w.w
	// ..www
	// ..w.w

	b1 = *(test.getStringConst(2, 4));
	if(b1.getEyes().size()==3)
		passes++;

	if(verbose) {
		test.printAsciiBoard();
		cin.get(temp);
		b1.printInfo();
		cin.get(temp);
	}

	message = "Passes : ";
	sprintf(buffer, "%d", passes);
	message+=buffer;
	message+="/";
	// out of how many tests?
	sprintf(buffer, "%d", 8);
	message+=buffer;
	log.println(message, myName);

	// wait for user to proceed
	log.println("Press enter.");
	cin.get(temp);
}

void Test::SGFStepthroughTest(BoardStruct& board, string sgfFilename, bool verbose /*=false*/)
{
	LogWriter log;
	string message;
	char buffer[200];
	char temp;
	// number of tests passed
	int passes = 0;

	log.println("SGFStepthrough test.",myName);

	SGFReader sgf;
	if(!sgf.load(sgfFilename)) {
		log.println("SGF failed to load, press enter.");
		cin.get(temp);
		return;
	}
	
	board.clear();
	board.enableMarkLastMove(true);
	// draw empty board and update all pieces from b

	SGFNode* nextNode = &(sgf.getRootNode());
	// next move in 'aa' form
	string c = "";
	int colour = EMPTY;
	int move = 0;
	int jumpToMove = 0;
	int step = 0;
	
	if(verbose)	{
		log.println("Enter or number of moves to step [1-9], 'p' to step back,");
		log.println("'s' to get a move suggestion, 'b' to show the board");
		log.println("or 'q' to quit.");
	}

	Move m;
	while (true) {
		// wait for key to be pressed
		while(true)	{
			char t[3];
			cin.getline(t, 3);
			if(t[0]=='q')
				return;
			else if(t[0]=='b')
				board.printAsciiBoard(true);
			else if(t[0]=='p') {
				// step back a move
				if(move>0)
					jumpToMove = move-1;
				move = 0;
				// clear board
				board.clear();
				nextNode = &(sgf.getRootNode());
				break;
			}
			else if(t[0]=='s' && (t[1]=='b' || t[1]=='w')) {
				// suggest the best move for this position
		
				// make sure we are actually looking at
				// the next colour to move
				//if(nextNode->getProperty("B")!="")
				if(t[1]=='b') {
					((Melkor*)melkor)->getMove(m, board, BLACK);
					message = "Move suggested = B[";
				}
				//else if(nextNode->getProperty("W")!="")
				else if(t[1]=='w') {
					((Melkor*)melkor)->getMove(m, board, WHITE);
					message = "Move suggested = W[";
				}
				else continue;
				message+=Move::xyToGoPosValue(m.getX(), m.getY(),
					board.getSize());
				log.println(message+"]", myName);
			}
			else if(t[0]=='\0' || atoi(t)!=0) {
				// otherwise step forward a single move
				// or n moves if specified
				if((step=atoi(t))!=0)
					jumpToMove = move+step;
				else
					jumpToMove = move+1;
				break;
			}
			else {
				log.println("Enter a number of moves to step [1-9], ");
				log.println("'p' to step back, ");
				log.println("'sb' to get a black move suggestion, ");
				log.println("'sw' to get a white move suggestion, ");
				log.println("'b' to show the board, ");
				log.println("'h' to print this message or 'q' to quit.");
			}
		} // end inner while(true)

		// add next move to board
		vector<string> props;
		while(move<jumpToMove) {
			if(nextNode->getChild()==NULL) {
				log.println("End of SGF file.");
				break;
			}
			nextNode = nextNode->getChild();
			if(nextNode==NULL)
				break;
			int oldColour = colour;
			if(verbose) {
				sprintf(buffer, "%d", move);
				message+=buffer;
				message+=": ";
			}
			nextNode->getProperty(SGFProperty::blackMoveTag, props);
			if ((c=props[0])!="") {
				colour = BLACK;
				if(verbose) {
					message+= "B [";
					message+=c;
					message+="]";
					log.println(message, myName);
				}
			}
			else {	
				nextNode->getProperty(SGFProperty::whiteMoveTag, props);
				if ((c=props[0])!="") {
					colour = WHITE;
					if(verbose) {
						message+= "W [";
						message+=c;
						message+="]";
						log.println(message, myName);
					}
				}
				// in case of a B[] or W[] pass node...
				else {
					if(oldColour==BLACK) {
						message+="W ";
						colour = WHITE;
					}
					else {
						message+="B ";
						colour = BLACK;
					}
					c = "tt";
					message+="[]";
					log.println(message, myName);
				}
			}
			
			// add move to board
			board.setPointSGF(c, colour);
			move++;
		} // end while(move<jumpToMove)

		// print board
		if(verbose)
			board.printAsciiBoard(true);
	} // end while(true)

	// wait for user to proceed
	log.println("Test finished. Press enter.");
	cin.get(temp);
}

void Test::TrainingSpeedTest()
{
	LogWriter log;
	log.println("TrainingSpeedTest", myName);

	// specify NN architecture without bias neurons
	vector<int> hidden;
	hidden.push_back(2);
//	BPN bpn(2, hidden, 1, 0.25f, 0.7f, BPN::BPNTYPE);
	BPN bpn(2, hidden, 1, 0.25f, 0.7f, BPN::BPNTYPE);
	BPNTrainer trainer(&bpn, 0, 0);

	vector<vector<float> > v;
	v.resize(2);
	for(int i=0;i<v.size();i++)
		v[i].resize(2);
	// weight from input neuron 1 to hidden neuron 1
	v[0][0] = (float)-0.06907;
	// weight from input neuron 1 to hidden neuron 2
	v[1][0] = (float)-0.40446;
	// weight from input neuron 2 to hidden neuron 1
	v[0][1] = (float)-0.89526;
	// weight from input neuron 2 to hidden neuron 2
	v[1][1] = (float)0.54714;
	bpn.setWeights(0, Matrix<float>(v, 2, 2));
	vector<vector<float> > v2;
	v2.resize(1);
	v2[0].resize(2);
	// weight from hidden neuron 1 to output neuron
	v2[0][0] = (float)0.75056;
	// weight from hidden neuron 2 to output neuron
	v2[0][1] = (float)0.83220;
	bpn.setWeights(1, Matrix<float>(v2, 1, 2));

	vector<vector<float> > v3;
	v3.resize(2);
	v3[0].resize(1);
	v3[1].resize(1);
	// bias weight into hidden neuron 1
	v3[0][0] = (float)0.38270;
	// bias weight into hidden neuron 2
	v3[1][0] = (float)0.65653;
	bpn.setBiasWeights(0, Matrix<float>(v3, 2, 1));

	vector<vector<float> > v4;
	v4.resize(1);
	v4[0].resize(1);
	// bias weight into output neuron
	v4[0][0] = (float)0.01045;
	bpn.setBiasWeights(1, Matrix<float>(v4, 1, 1));
	
	bpn.printInfo();
	
	// set input and output patterns
	// do not add constant value of 1.0 for first input for bias
	vector<Matrix<float> > input;
	vector<Matrix<float> > output;
	vector<vector<float> > temp;
	temp.resize(1);
	for(i=0;i<temp.size();i++)
		temp[i].resize(2);
	temp[0][0] = (float)0.0;
	temp[0][1] = (float)0.0;
	input.push_back(Matrix<float>(temp, 1, 2));
	temp[0][0] = (float)0.0;
	temp[0][1] = (float)1.0;	
	input.push_back(Matrix<float>(temp, 1, 2));
	temp[0][0] = (float)1.0;
	temp[0][1] = (float)0.0;	
	input.push_back(Matrix<float>(temp, 1, 2));
	temp[0][0] = (float)1.0;
	temp[0][1] = (float)1.0;	
	input.push_back(Matrix<float>(temp, 1, 2));

	temp.resize(1);
	temp[0].resize(1);
	temp[0][0] = (float)0.0;
	output.push_back(Matrix<float>(temp, 1, 1));
	temp[0][0] = (float)1.0;
	output.push_back(Matrix<float>(temp, 1, 1));
	temp[0][0] = (float)1.0;
	output.push_back(Matrix<float>(temp, 1, 1));	
	temp[0][0] = (float)0.0;
	output.push_back(Matrix<float>(temp, 1, 1));
	
	// run test

	// train for one epoch
	vector<int> freeze;
	char buffer[200];
	string message;
	double t;
//	double t = trainer.trainAndTime(input, output, freeze, false, 10000);
//	string message = "(1) 10,000 epochs - Time taken: ";
//	sprintf(buffer, "%*g", 7, t);
//	message+=buffer;
//	message+="s";
//	log.println(message);

	log.println("Training without batch update or momentum.", myName);

	// now train to convergence
	// input, output, freeze, print, epochs, useSymmetry, doMomentum, batchUpdate, convergenceCheck, printProgress
	t = trainer.trainAndTime(input, output, freeze, false, 0, false, false, false, true, false);
	message = "(1) To Convergence - Time taken: ";
	sprintf(buffer, "%*g", 7, t);
	message+=buffer;
	message+="s";
	log.println(message);
	log.println("Should take 3743 epochs.");
	
	log.println("Training finished, press a key.", myName);
	char cTemp;
	cin.get(cTemp);

//	vector<Matrix<float> > m(3);
	vector<Matrix<float> >& m = bpn.getOutputs();
	bpn.getAnswer(input[0]);
	log.println("Test input 1:", myName);		
	input[0].printInfo();
	log.println("Output 1:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[1]);
	log.println("Test input 2:", myName);
	input[1].printInfo();
	log.println("Output 2:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[2]);
	log.println("Test input 3:", myName);
	input[2].printInfo();
	log.println("Output 3:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[3]);
	log.println("Test input 4:", myName);
	input[3].printInfo();
	log.println("Output 4:", myName);		
	m[m.size()-1].printInfo();
	bpn.save("xor.bpn");

	// now try with batch update
	log.println("Training with batch update without momentum (note batch update may be slower or faster, \
		this depends greatly on initial weights etc...), press a key.", myName);
	cin.get(cTemp);

	// weight from input neuron 1 to hidden neuron 1
	v[0][0] = (float)-0.56;
	// weight from input neuron 1 to hidden neuron 2
	v[1][0] = (float)0.94381;
	// weight from input neuron 2 to hidden neuron 1
	v[0][1] = (float)0.18189;
	// weight from input neuron 2 to hidden neuron 2
	v[1][1] = (float)-0.91368;
	bpn.setWeights(0, Matrix<float>(v, 2, 2));
	// weight from hidden neuron 1 to output neuron
	v2[0][0] = (float)0.514;
	// weight from hidden neuron 2 to output neuron
	v2[0][1] = (float)-0.51387;
	bpn.setWeights(1, Matrix<float>(v2, 1, 2));

	// bias weight into hidden neuron 1
	v3[0][0] = (float)-0.47493;
	// bias weight into hidden neuron 2
	v3[1][0] = (float)-0.5158;
	bpn.setBiasWeights(0, Matrix<float>(v3, 2, 1));

	// bias weight into output neuron
	v4[0][0] = (float)0.36254;
	bpn.setBiasWeights(1, Matrix<float>(v4, 1, 1));
	
	bpn.setEpochsCompleted(0);
	bpn.setPatternsCompleted(0);
	bpn.printInfo();

	// input, output, freeze, print, epochs, useSymmetry, doMomentum, batchUpdate, convergenceCheck, printProgress
	t = trainer.trainAndTime(input, output, freeze, false, 0, false, false, true, true, false);
	message = "(2) To Convergence - Time taken: ";
	sprintf(buffer, "%*g", 7, t);
	message+=buffer;
	message+="s";
	log.println(message);
	log.println("Should take 3103 epochs.");

	log.println("Training finished, press a key.", myName);
	cin.get(cTemp);

	bpn.printInfo();

	bpn.getAnswer(input[0]);
	log.println("Test input 1:", myName);		
	input[0].printInfo();
	log.println("Output 1:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[1]);
	log.println("Test input 2:", myName);
	input[1].printInfo();
	log.println("Output 2:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[2]);
	log.println("Test input 3:", myName);
	input[2].printInfo();
	log.println("Output 3:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[3]);
	log.println("Test input 4:", myName);
	input[3].printInfo();
	log.println("Output 4:", myName);		
	m[m.size()-1].printInfo();
	bpn.save("xor.bpn");

	// setup for RPROP test
	bpn.setEpochsCompleted(0);
	bpn.setPatternsCompleted(0);
	bpn.printInfo();

	// now try with RPROP
	log.println("Training with RPROP, press a key.", myName);
	cin.get(cTemp);

	// weight from input neuron 1 to hidden neuron 1
	v[0][0] = (float)0.18189;
	// weight from input neuron 1 to hidden neuron 2
	v[1][0] = (float)-0.91368;
	// weight from input neuron 2 to hidden neuron 1
	v[0][1] = (float)-0.56000;
	// weight from input neuron 2 to hidden neuron 2
	v[1][1] = (float)0.94381;
	bpn.setWeights(0, Matrix<float>(v, 2, 2));
	// weight from hidden neuron 1 to output neuron
	v2[0][0] = (float)-0.51387;
	// weight from hidden neuron 2 to output neuron
	v2[0][1] = (float)0.5140;
	bpn.setWeights(1, Matrix<float>(v2, 1, 2));

	// bias weight into hidden neuron 1
	v3[0][0] = (float)-0.47493;
	// bias weight into hidden neuron 2
	v3[1][0] = (float)-0.51580;
	bpn.setBiasWeights(0, Matrix<float>(v3, 2, 1));

	// bias weight into output neuron
	v4[0][0] = (float)0.36254;
	bpn.setBiasWeights(1, Matrix<float>(v4, 1, 1));
	
	bpn.setEpochsCompleted(0);
	bpn.setPatternsCompleted(0);
	bpn.printInfo();

	RPROPBPNTrainer rpropTrainer(&bpn, 0, 0);

	// input, output, freeze, print, epochs, useSymmetry, doMomentum, batchUpdate, convergenceCheck, printProgress
	t = rpropTrainer.trainAndTime(input, output, freeze, false, 0, false, true, true, true, false);
	message = "(3) To Convergence - Time taken: ";
	sprintf(buffer, "%*g", 7, t);
	message+=buffer;
	message+="s";
	log.println(message);

	log.println("Training finished, press a key.", myName);
	cin.get(cTemp);

	bpn.printInfo();

	bpn.getAnswer(input[0]);
	log.println("Test input 1:", myName);		
	input[0].printInfo();
	log.println("Output 1:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[1]);
	log.println("Test input 2:", myName);
	input[1].printInfo();
	log.println("Output 2:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[2]);
	log.println("Test input 3:", myName);
	input[2].printInfo();
	log.println("Output 3:", myName);		
	m[m.size()-1].printInfo();
	bpn.getAnswer(input[3]);
	log.println("Test input 4:", myName);
	input[3].printInfo();
	log.println("Output 4:", myName);		
	m[m.size()-1].printInfo();
	bpn.save("xor.bpn");
}

// follow example from essence of NN book p.37 example 2.3
void Test::BPNTest()
{
	LogWriter log;
	// 2-2-1 architecture
	// extra neuron input to each layer for bias
	// hence height of 3 for matrices
	vector<int> hidden;
	hidden.push_back(2);
	BPN bpn(2, hidden, 1, 0.25f, 0.7f, BPN::BPNTYPE);
	BPNTrainer trainer(&bpn, 0, 0);

	//  0.1  0.1
	// -0.2 -0.1
	//  0.1  0.3
	vector<vector<float> > v;
	v.resize(2);
	for(int i=0;i<v.size();i++)
		v[i].resize(2);
	v[0][0] = (float)-0.2;
	v[0][1] = (float)0.1;
	v[1][0] = (float)-0.1;
	v[1][1] = (float)0.3;
	bpn.setWeights(0, Matrix<float>(v, 2, 2));
	vector<vector<float> > v2;
	v2.resize(1);
	v2[0].resize(2);
	v2[0][0] = (float)0.2;
	v2[0][1] = (float)0.3;
	bpn.setWeights(1, Matrix<float>(v2, 1, 2));

	// set bias weights
	v[0].resize(1);
	v[1].resize(1);
	v[0][0] = (float)0.1;
	v[1][0] = (float)0.1;
	bpn.setBiasWeights(0, Matrix<float>(v, 2, 1)); 
	v2[0].resize(1);
	v2[0][0] = (float)0.2;
	bpn.setBiasWeights(1, Matrix<float>(v2, 1, 1));

	// set input and output patterns
	// do not include constant value of 1.0 for first input for bias
	vector<vector<float> > inputValues;
	inputValues.resize(1);
	inputValues[0].resize(2);
	inputValues[0][0] = (float)0.1;
	inputValues[0][1] = (float)0.9;
	Matrix<float> input(inputValues, 1, 2);
	
	vector<vector<float> > outputValues;
	outputValues.resize(1);
	outputValues[0].resize(1);
	outputValues[0][0] = (float)0.9;
	Matrix<float> output(outputValues, 1, 1);
	
	// run test and time it
	time_t start;
	time_t finish;
	time(&start);
	// doMomentum = false
	// just for this example
	vector<int> freeze;
	float pMSE = 0.0f;
	trainer.trainPattern(input, output, freeze, pMSE, false);
	time(&finish);
	double t = difftime(finish, start);
	log.println("Result: ", myName);
	bpn.printInfo();
	char temp;
	cout << "Press enter" << endl;
	cin.get(temp);
	log.println("Values should be: ", myName);
	log.println("Bias weight (layer 1): [0.217]", myName);
	log.println("Weights (layer 1):     [0.209]", myName);
	log.println("                       [0.31 ]", myName);
	string message = "Time taken: ";
	char buffer[50];
	sprintf(buffer, "%*g", 7, t);
	message+=buffer;
	message+=" ms";
	log.println(message, myName);
	cout << "Press enter" << endl;
	cin.get(temp);

	log.println("Load and save test", myName);
	bpn.save("test.bpn");
	bpn.load(string("test.bpn"));
	bpn.printInfo();
	cout << "Press enter" << endl;
	cin.get(temp);
}

void Test::AlphaBetaTest(bool verbose /*=false*/)
{
	// test case based on 
	// http://www.cis.temple.edu/~ingargio/cis587/readings/alpha-beta-example.html

	// Example tree (numbers for node names instead of letters):
	//
	//                        0
	//                  /          \
	//            1                       16
	//         /     \                  /     \
	//      2           9          17            24
	//    /  \        /  \        /  \          /  \
	//   3    6      10   13    18    21      25    28
	//  /\    /\    /\    /\    /\    /\     /\      /\
	// 4  5  7  8  11 12 14 15 19 20 22 23  26 27  29 30
	
	// Evaluation scores: 
	// 10 11 9 12  14 15 13 14 15  2  4  1   3 22  24 25

	// NODE	TYPE A	B ALPHA	BETA SCORE

	// 0	Max	-I	+I	-I	+I
	// 1	Min	-I	+I	-I	+I
	// 2	Max	-I	+I	-I	+I
	// 3	Min	-I	+I	-I	+I
	// 4	Max	-I	+I			10
	// 3	Min	-I	+I	-I	10
	// 5	Max	-I	10			11
	// 3	Min	-I	+I	-I	10	10
	// 2	Max	-I	+I	10	+I
	// 6	Min	10	+I	-I	+I
	// 7	Max	10	+I			9
	// 6	Min	10	+I	-I	9	9
	// 2	Max	-I	+I	10	+I	10
	// 1	Min	-I	+I	-I	10
	// 9	Max	-I	10	-I	+I
	// 10	Min	-I	10	-I	+I
	// 11	Max	-I	10			14
	// 10	Min	-I	10	-I	14
	// 12	Max	-I	10			15
	// 10	Min	-I	10	-I	14	14
	// 9	Max	-I	10	14	+I	14
	// 1	Min	-I	+I	-I	10	10
	// 0	Max	-I	+I	10	+I
	// 16	Min	10	+I	-I	+I
	// 17	Max	10	+I	-I	+I
	// 18	Min	10	+I	-I	+I
	// 19	Max	10	+I			15
	// 18	Min	10	+I	-I	15
	// 20	Max	10	+I			2
	// 18	Min	10	+I	-I	2	2
	// 17	Max	10	+I	2	+I
	// 21	Min	10	+I	-I	+I
	// 22	Max	10	+I			4
	// 21	Min	10	+I	-I	4	4
	// 17	Max	10	+I	4	+I	4
	// 16	Min	10	+I	-I	4	4
	// 0	Max	-I	+I	10	4	10

	LogWriter log;
	string message;
	char temp;

	log.println("AlphaBeta test", myName);
	if(verbose) {
		log.println("Press a key at each pause to proceed.", myName);
		cin.get(temp);
	}

	int correct=0;

	Node root;
	float alpha = MTDFSearch::NEGINFINITY;
	float beta = MTDFSearch::POSINFINITY;
	int depth = 4;
	int ourColour = BLACK;
	bool rootLevel = true;

	// setup root node
	root.setNextToPlay(BLACK);

	BoardStruct initialBoard;

	// We must name and add the first move to the board so AlphaBetaTestSuggester/Goal can
	// both pick up and respond correctly to different tree nodes
	initialBoard.addToBoard( Move(0, 0, EMPTY, initialBoard.getSize(), 0.0f) );

	/*******************************/
	/* Alpha-Beta with memory test */
	/*******************************/

	// turn off optimisations
	// so we have vanilla alpha-beta with memory
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(false, false, false, 0.0f, true, 0.0f, false, false, false);
	((Melkor*)melkor)->mtdfSearch->setMoveSuggester( new Test::AlphaBetaTestSuggester() );
	((Melkor*)melkor)->mtdfSearch->setGoal( Goal( new Test::AlphaBetaTestGoal() ) );
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	float score = ((Melkor*)melkor)->mtdfSearch->AlphaBetaWithMemory(initialBoard, &root, alpha, beta, depth, 
		0, ourColour, rootLevel, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==19 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 19,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/********************************************************/
	/* Alpha-Beta with memory and Best Move First extension */
	/********************************************************/

	if(verbose)
		log.println("Testing best move first extension.", myName);

	// BestMoveFirst needs the transposition table to have been used already to make any difference
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(true, false, false, 0.0f, true, 0.0f, false, false, false);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->AlphaBetaWithMemory(initialBoard, &root, alpha, beta, depth, 0,
		ourColour, rootLevel, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==19 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 19,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/***********************************************************************/
	/* Alpha-Beta with memory and Enhanced Transposition Cutoffs extension */
	/***********************************************************************/

	if(verbose)
		log.println("Testing enhanced transposition cutoffs extension.", myName);

	// ETC needs the transposition table to have been used already to make any difference
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(false, true, false, 0.0f, true, 0.0f, false, false, false);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->AlphaBetaWithMemory(initialBoard, &root, alpha, beta, depth, 0,
		ourColour, rootLevel, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==19 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 19,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/*****************************************************************/
	/* Alpha-Beta with memory and ETC and Best Move First extensions */
	/*****************************************************************/

	if(verbose)
		log.println("Testing best move first and ETC extensions.", myName);

	// ETC and bestMoveFirst need the transposition table to have been used already to make any difference
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(true, true, false, 0.0f, true, 0.0f, false, false, false);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->AlphaBetaWithMemory(initialBoard, &root, alpha, beta, depth, 0,
		ourColour, rootLevel, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==19 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 19,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/**************************************************/
	/* MTDF test, no extensions (just Alpha-Beta+mem) */
	/**************************************************/

	if(verbose)
		log.println("Testing MTDF algorithm.", myName);

	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(false, false, false, 0.0f, true, 0.0f, false, false, true);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	float firstguess = 0;
	score = ((Melkor*)melkor)->mtdfSearch->MTDF(initialBoard, &root, firstguess, depth, ourColour, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==24 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 24,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/**********************************/
	/* MTDF test with Best Move First */
	/**********************************/

	if(verbose)
		log.println("Testing MTDF + best move first.", myName);

	// BestMoveFirst needs the transposition table to have been used already to make any difference
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(true, false, false, 0.0f, true, 0.0f, false, false, true);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->MTDF(initialBoard, &root, firstguess, depth, ourColour, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==24 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 24,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/**********************/
	/* MTDF test with ETC */
	/**********************/

	if(verbose)
		log.println("Testing MTDF + ETC.", myName);

	// ETC needs the transposition table to have been used already to make any difference
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(false, true, false, 0.0f, true, 0.0f, false, false, true);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->MTDF(initialBoard, &root, firstguess, depth, ourColour, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==24 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 24,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/******************************************/
	/* MTDF test with ETC and Best Move First */
	/******************************************/

	if(verbose)
		log.println("Testing MTDF + best move first + ETC.", myName);

	// BestMoveFirst and ETC need the transposition table to have been used already to make any difference
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(true, true, false, 0.0f, true, 0.0f, false, false, true);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->MTDF(initialBoard, &root, firstguess, depth, ourColour, NULL, false);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==24 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==8)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 24,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 8);

	/*********************************/
	/* MTDF with iterative deepening */
	/*********************************/

	if(verbose)
		log.println("Testing MTDF + iterative deepening.", myName);

	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(false, false, false, 0.0f, true, 0.0f, false, true, true);
	((Melkor*)melkor)->mtdfSearch->init();

	// run test
	score = ((Melkor*)melkor)->mtdfSearch->iterativeDeepening(initialBoard, &root, 4, ourColour);

	// check results
	if(score==10 && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==53 && 
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==18)
		correct++;
	else
		log.println("Test failed!", myName);

	// print score
	if(verbose)
		outputInfo(score, 10, ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 53,
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), 18);

	char buffer[50];
	message = "Passes : ";
	sprintf(buffer, "%d", correct);
	message+=buffer;
	message+="/";
	sprintf(buffer, "%d", 9);
	message+=buffer;
	log.println(message, myName);
	// wait for user to proceed
	log.println("Press enter.");
	cin.get(temp);
}

/** @brief Print the alpha beta score and how many nodes have been traversed and expanded. Compare
  * them to the expected values and print the expected values in '{}' if they differ. */
void Test::outputInfo(const float score, const float exScore, const int nodesTraversed,
					  const int exNodesTraversed, const int nodesEvaluated, const int exNodesEvaluated)
{
	LogWriter log;
	char buffer[50];
	
	if(score!=exScore)
		sprintf(buffer, "AlphaBeta returned score: %*g {%g}", 7, score, exScore);
	else
		sprintf(buffer, "AlphaBeta returned score: %*g", 7, score);
	log.println(buffer, myName);
	
	if(nodesTraversed != exNodesTraversed)
		sprintf(buffer, "Nodes traversed: %d {%d}", nodesTraversed, exNodesTraversed);
	else
		sprintf(buffer, "Nodes traversed: %d", nodesTraversed);
	log.println(buffer, myName);
	
	if(nodesEvaluated != exNodesEvaluated)
		sprintf(buffer, "Nodes evaluated: %d {%d}", nodesEvaluated, exNodesEvaluated);
	else
		sprintf(buffer, "Nodes evaluated: %d", nodesEvaluated);
	log.println(buffer, myName);
	
	// wait for user to proceed
	log.println("Press enter.");
	char temp;
	cin.get(temp);
}

/** @bug Convert this function to a MoveSuggester derived class so we can verify alpha beta works ok. */
/*
void Test::alphaBetaTestExpand(vector<Node>& nodes, const Node& n, const BoardStruct& board, Goal& goal, MTDFSearch* mtdf, const int maximisingColour)
{
	LogWriter log;
	log.println("This test needs to be fixed!!!");
	return;

	// test case based on 
	// http://www.cis.temple.edu/~ingargio/cis587/readings/alpha-beta-example.html

	// match child nodes to node name in example
	// Example tree (numbers for node names instead of letters):
	//
	//                        0
	//                  /          \
	//            1                       16
	//         /     \                  /     \
	//      2           9          17            24
	//    /  \        /  \        /  \          /  \
	//   3    6      10   13    18    21      25    28
	//  /\    /\    /\    /\    /\    /\     /\      /\
	// 4  5  7  8  11 12 14 15 19 20 22 23  26 27  29 30

	Node temp;

	nodes.clear();

	float index = n.getNNScore();
	if(index==0.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 0, 0, 1.0f, 1, 0, 16.0f, n);
	else if(index==1.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 2, 0, 2.0f, 3, 0, 9.0f, n);
	else if(index==2.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 4, 0, 3.0f, 5, 0, 6.0f, n);
	else if(index==3.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 6, 0, 4.0f, 7, 0, 5.0f, n);
	else if(index==4.0f)
		return;
	else if(index==5.0f)
		return;
	else if(index==6.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 8, 0, 7.0f, 0, 2, 8.0f, n);
	else if(index==7.0f)
		return;
	else if(index==8.0f)
		return;
	else if(index==9.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 1, 2, 10.0f, 2, 2, 13.0f, n);
	else if(index==10.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 3, 2, 11.0f, 4, 2, 12.0f, n);
	else if(index==11.0f)
		return;
	else if(index==12.0f)
		return;
	else if(index==13.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 5, 2, 14.0f, 6, 2, 15.0f, n);
	else if(index==14.0f)
		return;
	else if(index==15.0f)
		return;
	else if(index==16.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 7, 2, 17.0f, 8, 2, 24.0f, n);
	else if(index==17.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 0, 4, 18.0f, 1, 4, 21.0f, n);
	else if(index==18.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 2, 4, 19.0f, 3, 4, 20.0f, n);
	else if(index==19.0f)
		return;
	else if(index==20.0f)
		return;
	else if(index==21.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 4, 4, 22.0f, 5, 4, 3.0f, n);
	else if(index==22.0f)
		return;
	else if(index==23.0f)
		return;
	else if(index==24.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 6, 4, 25.0f, 7, 4, 28.0f, n);
	else if(index==25.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 8, 4, 26.0f, 0, 6, 27.0f, n);
	else if(index==26.0f)
		return;
	else if(index==27.0f)
		return;
	else if(index==28.0f)
		((Melkor*)mtdf->getParent())->test->addChildren(nodes, 1, 6, 29.0f, 2, 6, 30.0f, n);
	else if(index==29.0f)
		return;
	else if(index==30.0f)
		return;
	else
	{
		LogWriter log;
		char buffer[50];
		string message = "Unknown test node: ";
		sprintf(buffer, "%*g", 4, index);
		message+=buffer;
		log.println(message, myName);
	}
}*/

/*void Test::addChildren(vector<Node>& nodes, int x1, int y1, float f1, 
					   int x2, int y2, float f2, const Node& n)
{
	Node temp(((Melkor*)melkor)->mtdfSearch->getChildNode(n, x1, y1, f1));
	nodes.push_back(temp);
	temp = ((Melkor*)melkor)->mtdfSearch->getChildNode(n, x2, y2, f2);
	nodes.push_back(temp);
	nodesExpanded++;
}*/

/*float Test::updateNodeScore(Node* n, float s)
{
	n->setAlphaBetaScore(s);
	n->setScoreCalculated(true);
	nodesEvaluated++;
	return s;
}*/

/** @bug Fix this test so we can verify alpha beta works ok. */
/** @todo This function needs to be transformed into a GoalInterface derived class. */
/*float Test::alphaBetaTestEvaluate(const BoardStruct& board, const int ourColour, Goal& goal, const int nextToPlay)
{


	// test case based on 
	// http://www.cis.temple.edu/~ingargio/cis587/readings/alpha-beta-example.html

	// match node name to score in example

	// Node names:
	// 4  5  7  8  11 12 14 15 19 20 22 23 26 27  29 30	
	// Evaluation scores: 
	// 10 11 9 12  14 15 13 14 15  2  4  1  3 22  24 25
	float index = n.getNNScore();
	if(index==4.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 10);
	else if(index==5.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 11);
	else if(index==7.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 9);
	else if(index==8.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 12);
	else if(index==11.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 14);
	else if(index==12.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 15);
	else if(index==14.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 13);
	else if(index==15.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 14);
	else if(index==19.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 15);
	else if(index==20.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 2);
	else if(index==22.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 4);
	else if(index==23.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 1);
	else if(index==26.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 3);
	else if(index==27.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 22);
	else if(index==29.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 24);
	else if(index==30.0f)
		return ((Melkor*)mtdf->getParent())->test->updateNodeScore(n, 25);

	return 0;
} */

void Test::AlphaBetaTest2(bool verbose /*=false*/)
{
	bool savedDontFillOwnEyes = Go::dontFillOwnEyes;
	
	LogWriter log;
	char buffer[500];
	char temp;
	log.println("AlphaBeta test", myName);
	if(verbose)
	{
		log.println("Press a key at each pause to proceed.", myName);
		cin.get(temp);
	}

	int correct = 0;
	int totalTests = 0;

	/****************************************/
	/* First run a test with an empty board */
	/****************************************/

	BoardStruct board(9);

	totalTests++;
	if(AlphaBetaHelper("Running alpha-beta test, empty board 9x9, depth=1...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 1, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			1, 0, 0, 83, 82))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
		correct++;

	/***************************************/
	/* Run a test with an endgame position */
	/* We're looking for a pass move from  */
	/* black here.                         */
	/***************************************/

	// Board position
	// *w*w*
	// w*www
	// *wwbb
	// wwbb*
	// *wb*b

	board.clear(5);
	board.setPoint(0, 1, WHITE);
	board.setPoint(0, 3, WHITE);
	board.setPoint(1, 0, WHITE);
	board.setPoint(1, 2, WHITE);
	board.setPoint(1, 3, WHITE);
	board.setPoint(1, 4, WHITE);
	board.setPoint(2, 1, WHITE);
	board.setPoint(2, 2, WHITE);
	board.setPoint(2, 3, BLACK);
	board.setPoint(2, 4, BLACK);
	board.setPoint(3, 0, WHITE);
	board.setPoint(3, 1, WHITE);
	board.setPoint(3, 2, BLACK);
	board.setPoint(3, 3, BLACK);
	board.setPoint(4, 1, WHITE);
	board.setPoint(4, 2, BLACK);
	board.setPoint(4, 4, BLACK);

	if(verbose)
		board.printAsciiBoard();

	totalTests++;
	if(AlphaBetaHelper("Running alpha-beta test, endgame board 5x5, depth=2...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 2, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			-6, -1, -1, 25, 21))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
		correct++;

	// Board position
	//
	//   0 1 2 3 4 
	// 0 . b b w .
	// 1 b w w b w
	// 2 b w + b w
	// 3 w b b b w
	// 4 . w w w .
	board.clear();
	board.setPoint(1, 0, BLACK);
	board.setPoint(2, 0, BLACK);
	board.setPoint(3, 0, WHITE);
	board.setPoint(0, 1, BLACK);
	board.setPoint(1, 1, WHITE);
	board.setPoint(2, 1, WHITE);
	board.setPoint(3, 1, BLACK);
	board.setPoint(4, 1, WHITE);
	board.setPoint(0, 2, BLACK);
	board.setPoint(1, 2, WHITE);
	board.setPoint(3, 2, BLACK);
	board.setPoint(4, 2, WHITE);
	board.setPoint(0, 3, WHITE);
	board.setPoint(1, 3, BLACK);
	board.setPoint(2, 3, BLACK);
	board.setPoint(3, 3, BLACK);
	board.setPoint(4, 3, WHITE);
	board.setPoint(1, 4, WHITE);
	board.setPoint(2, 4, WHITE);
	board.setPoint(3, 4, WHITE);

	if(verbose)
		board.printAsciiBoard();

	// Alpha-beta tree with Black to play, depth = 1 (move coords in brackets)
	//					   0
	//				/	   |	 \       \
	//			(4,0)	(2,2)	(0,4)   (-1,-1)
	// Eval:  	  0	  	  +2	  0        -2

	// Alpha-beta tree with Black to play, depth = 2 (move coords in brackets)
	//					                  0
	//				/	                              |
	//			(4,0)	                            (2,2)
	//     /    /    \     \         /    /     /     |     \     \     \
	// (0,0) (0,4) (4,4) (-1,-1)  (4,0) (1,1) (2,1) (1,2) (0,4) (4,4) (-1,-1)
	//   -3    -1    -1     0       +1    +1    +1    +1    +1    +1     +2
	//
	//            \                                \
	//          (0,4)                           (-1,-1)
	//    /     /    \      \        /     /    /     \     \      \
	// (0,0) (4,0) (4,4) (-1,-1)  (0,0) (4,0) (2,2) (0,4) (4,4) (-1,-1)
	//   -3    -1    -1     0       -7    -3    -8    -3    -3     -2

	/**************************************/
	/* Alpha-Beta test with Black to play */
	/**************************************/

	/*************/
	/* Depth = 1 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running alpha-beta test, depth=1...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 1, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			2, 2, 2, 5, 4))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
		correct++;

	/*************/
	/* Depth = 2 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running alpha-beta test, depth=2...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 2, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			1, 2, 2, 18, 13))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
		correct++;

	/*************/
	/* Depth = 4 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running alpha-beta test, depth=4...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 4, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			2, 2, 2, 207, 136))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/*************/
	/* Depth = 5 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running alpha-beta test, depth=5...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 721, 393))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;
	
	/*********************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 1 */
	/*********************************************************/

	string info = "\n** Note that for testing iterative deepening, it needs best move first to be enabled to allow\n";
	info+="   the advantage of ID (being able to use previous max_depth iterations best moves first) to be\n";
	info+="   shown. So only with best move first can valid comparisons be made with alpha-beta\n";
	info+="Timing test using plain iterative deepening, depth=5, step=1, stoneCount evaluation with Best Move First...";
	totalTests++;
	if(AlphaBetaHelper(info,
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, true, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 962, 576))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;
		
	/*********************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 2 */
	/*********************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using plain iterative deepening, depth=5, step=2, stoneCount evaluation with Best Move First...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, true, 5, 2, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 785, 462))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;
		
	/*********************************************************/
	/* Timing test, iterative deepening, depth = 4, step = 2 */
	/*********************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using plain iterative deepening, depth=4, step=2, stoneCount evaluation with Best Move First...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, true, 4, 2, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			2, 2, 2, 242, 160))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;
		
	/*********************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 1 */
	/*********************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using iterative deepening (MTD(f)), depth=5, step=1, stoneCount evaluation with Best Move First...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, true, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 860, 333))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/*********************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 2 */
	/*********************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using iterative deepening (MTD(f)), depth=5, step=2, stoneCount evaluation with Best Move First...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, true, 5, 2, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 732, 262))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/***************************************************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 1 with NN move suggester sorter */
	/***************************************************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using iterative deepening (MTD(f)), depth=5, step=1, stoneCount evaluation with the NN move suggester\n to sort moves with Best Move First...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new GeneralSuggester(), Goal(new StoneCountGoal()), board,
			true, true, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 663, 296))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;


	/**************************************************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 1 with Best Move First and ETC */
	/**************************************************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using iterative deepening (MTD(f)), depth=5, step=1, stoneCount evaluation with Best Move First and ETC...",
			true, true, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, true, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 860, 333))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/*********************************************************************/
	/* Use INFLUENCE eval and output number of expanded/evaluated nodes */
	/*********************************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using iterative deepening (MTD(f)), depth=5, influence evaluation with Best Move First...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new InfluenceGoal()), board,
			true, true, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			10, 2, 2, 884, 315))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/********************************/
	/* MTDF test with Black to play */
	/********************************/

	/*************/
	/* Depth = 1 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running MTDF test, depth=1...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, false, 1, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			2, 2, 2, 10, 4))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
		correct++;

	/*************/
	/* Depth = 2 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running MTDF test, depth=2...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, false, 2, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			1, 2, 2, 19, 10))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
		correct++;

	/*************/
	/* Depth = 5 */
	/*************/

	totalTests++;
	if(AlphaBetaHelper("Running MTDF test, depth=5...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, false, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 951, 329))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/**********************************/
	/* Depth = 5 with Best Move First */
	/**********************************/

	totalTests++;
	if(AlphaBetaHelper("Running MTDF test with Best Move First, depth=5...",
			true, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, false, 5, 1, BLACK, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			6, 2, 2, 930, 329))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/**************************************/
	/* Alpha-Beta test with White to play */
	/**************************************/

	// With White to play
	//							   0
	//				/		/	   |	\		\	
	//			(0,0)	(4,0)	(2,2)	(0,4)	(4,4)
	// Eval:	+7		+3		+8		+3		+3
	// reset root
	totalTests++;
	if(AlphaBetaHelper("Running Alpha-Beta test with white to play...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			false, false, 1, 1, WHITE, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			8, 2, 2, 7, 6))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	/********************************/
	/* MTDF test with White to play */
	/********************************/

	totalTests++;
	if(AlphaBetaHelper("Running MTDF test...",
			false, false, true, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
			new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
			true, false, 1, 1, WHITE, verbose,         /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
			8, 2, 2, 13, 6))                       /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */
			correct++;

	// End of tests
	sprintf(buffer, "Passes : %d/%d", correct, totalTests);
	log.println(buffer, myName);
	// wait for user to proceed
	log.println("Press enter.");
	cin.get(temp);

	Go::dontFillOwnEyes = savedDontFillOwnEyes;
}

void Test::AlphaBetaTest3(bool verbose /*=false*/)
{
	bool savedDontFillOwnEyes = Go::dontFillOwnEyes;
	
	LogWriter log;
	char buffer[500];
	char temp;
	log.println("AlphaBeta test", myName);
	if(verbose) {
		log.println("Press a key at each pause to proceed.", myName);
		cin.get(temp);
	}

	int correct = 0;
	int totalTests = 0;

	/*******************************************************/
	/* Now try a couple more difficult, deeper searches to */
	/* really compare alpha-beta, ID and MTD(f)            */
	/*******************************************************/

	// Board position - For a stone count eval BLACK can capture 7,4 in 5 moves
	//                  starting with 7,3
	//
	//   0 1 2 3 4 5 6 7 8
	// 0 . . . . . . . . . 
	// 1 . . . . . . . . .
	// 2 . . . . . . w . .
	// 3 . . . . . . b . .
	// 4 . . . . . b w w .
	// 5 . . . . b w w b .
	// 6 . . . . b w b . .
	// 7 . . . . . b . . .
	// 8 . . . . . . . . .
	BoardStruct board(9);
	board.setPoint(6, 2, WHITE);
	board.setPoint(6, 3, BLACK);
	board.setPoint(5, 4, BLACK);
	board.setPoint(6, 4, WHITE);
	board.setPoint(7, 4, WHITE);
	board.setPoint(4, 5, BLACK);
	board.setPoint(5, 5, WHITE);
	board.setPoint(6, 5, WHITE);
	board.setPoint(7, 5, BLACK);
	board.setPoint(4, 6, BLACK);
	board.setPoint(5, 6, WHITE);
	board.setPoint(6, 6, BLACK);
	board.setPoint(5, 7, BLACK);
	
	if(verbose)
		board.printAsciiBoard();

	/*********************************************************/
	/* Timing test, iterative deepening, depth = 5, step = 1 */
	/*********************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using iterative deepening (MTD(f)), depth=5, step=1, stoneCount evaluation with Best Move First...",
		true, false, true, 0.0f, true, 0.0f, new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
		true, true, 5, 1, BLACK, verbose, 7, 7, 3, 1239990, 1144912))
		correct++;

	/*********************************************************************/
	/* Timing test, iterative deepening - no MTD(f), depth = 5, step = 1 */
	/*********************************************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using plain iterative deepening, depth=5, step=1, stoneCount evaluation with Best Move First...",
		true, false, true, 0.0f, true, 0.0f, new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
		false, true, 5, 1, BLACK, verbose, 7, 7, 3, 1322010, 1216992))
		correct++;

	/********************************************/
	/* Timing test, MTD(f), depth = 5, step = 1 */
	/********************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using MTD(f), depth=5, step=1, stoneCount evaluation with Best Move First...",
		true, false, true, 0.0f, true, 0.0f, new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
		true, false, 5, 1, BLACK, verbose, 7, 7, 3, 2815321, 1984208))
		correct++;

	/***********************************************/
	/* Timing test, AlphaBeta, depth = 5, step = 1 */
	/***********************************************/

	totalTests++;
	if(AlphaBetaHelper("Timing test using AlphaBeta, depth=5, step=1, stoneCount evaluation with Best Move First...",
		true, false, true, 0.0f, true, 0.0f, new AllLegalSuggester(), Goal(new StoneCountGoal()), board,
		false, false, 5, 1, BLACK, verbose, 7, 7, 3, 3155631, 2124424))
		correct++;

	// End of tests
	sprintf(buffer, "Passes : %d/%d", correct, totalTests);
	log.println(buffer, myName);
	// wait for user to proceed
	log.println("Press enter.");
	cin.get(temp);

	Go::dontFillOwnEyes = savedDontFillOwnEyes;
}

/** Parameters beginning with r are values that the test must achieve to count as a pass.
  * @return True if the test was passed. */
bool Test::AlphaBetaHelper(const string& info, const bool bmf, const bool etc, const bool moveQualityThreshold, 
						   const float pruneValue, const bool useTTable, const float passValue, 
						   MoveSuggester* suggester, const Goal& goal, const BoardStruct& board,
						   const bool useMTDF, const bool useID, const int depth, const int IDStep, 
						   const int nextToPlay, const bool verbose, const float rScore, const int rX, 
						   const int rY, const int rNodesTraversed, const int rNodesEvaluated)
{
	float alpha = MTDFSearch::NEGINFINITY;
	float beta = MTDFSearch::POSINFINITY;
	float score;
	bool passed = false;
	char temp;
	Move m;
	LogWriter log;
	if(verbose)	{
		log.println(info, myName);
		cin.get(temp);
	}

	((Melkor*)melkor)->mtdfSearch->init();
	((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(bmf, etc, moveQualityThreshold, pruneValue, useTTable, passValue, false,
		useID, useMTDF);
	((Melkor*)melkor)->mtdfSearch->setMoveSuggester(suggester);
	((Melkor*)melkor)->mtdfSearch->setGoal(goal);

	int maximisingColour = goal.getRequiredMaximisingColour(board, nextToPlay);

	// reset root
	Node root(board, nextToPlay);

	bool testForTerminalNodes = false;
	if(goal.testFlags(GoalInterface::TERMINAL))
		testForTerminalNodes = true;

	timerStart();
	/** @todo Use MTDFSearch::runSearchAsSettings instead of this */
	if(useID)
		score = ((Melkor*)melkor)->mtdfSearch->iterativeDeepening(board, &root, depth, maximisingColour, IDStep, useMTDF);
	else if(useMTDF)
		score = ((Melkor*)melkor)->mtdfSearch->MTDF(board, &root, 0.0, depth, maximisingColour, NULL, testForTerminalNodes);
	else {
		BoardStruct tempBoard(board);
		score = ((Melkor*)melkor)->mtdfSearch->AlphaBetaWithMemory(tempBoard, &root, alpha, beta, depth, 0,
			maximisingColour, true, NULL, testForTerminalNodes);
	}
	timerEnd();
	
	
	if(useTTable) {
		if(!((Melkor*)melkor)->mtdfSearch->lookup(root, root)) {
			log.println("Error: Transposition table broken", myName);
			return false;
		}
	}
	if(root.getHaveBestNode()) {
		if(useTTable)
			((Melkor*)melkor)->mtdfSearch->getBestNode(root)->getMove(m);
		else {
			m.setX(root.getBestX());
			m.setY(root.getBestY());
		}
	}
	else
		log.println("No best node available, may be no best move.");

	// work out time in milliseconds
	double t = timerValue()*1000;

	if(verbose) {
		char buffer[2000];
		sprintf(buffer, "Time taken: %*gms", 7, t);
		log.println(buffer, myName);
		char badValue[2000];
		badValue[0] = '\0';
		if(score!=rScore)
			sprintf(badValue, "{%g}", rScore);
		if(nextToPlay == BLACK)
			sprintf(buffer, "Black to play, AlphaBeta returned score: %*g %s", 7, score, badValue);
		else
			sprintf(buffer, "White to play, AlphaBeta returned score: %*g %s", 7, score, badValue);
		log.println(buffer, myName);
		if(m.getX()!=rX || m.getY()!=rY)
			sprintf(buffer, "Move suggested: (%d, %d) {%d, %d}", m.getX(), m.getY(), rX, rY);
		else
			sprintf(buffer, "Move suggested: (%d, %d)", m.getX(), m.getY());
		log.println(buffer, myName);
		if(((Melkor*)melkor)->mtdfSearch->getNodesTraversed()!=rNodesTraversed)
			sprintf(buffer, "Nodes traversed: %d {%d}", ((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), rNodesTraversed);
		else
			sprintf(buffer, "Nodes traversed: %d", ((Melkor*)melkor)->mtdfSearch->getNodesTraversed());
		log.println(buffer, myName);
		if(((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()!=rNodesEvaluated)
			sprintf(buffer, "Nodes evaluated: %d {%d}", ((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), rNodesEvaluated);
		else
			sprintf(buffer, "Nodes evaluated: %d", ((Melkor*)melkor)->mtdfSearch->getNodesEvaluated());
		log.println(buffer, myName);
	}
	if(score==rScore && m.getX()==rX && m.getY()==rY && ((Melkor*)melkor)->mtdfSearch->getNodesTraversed()==rNodesTraversed &&
		((Melkor*)melkor)->mtdfSearch->getNodesEvaluated()==rNodesEvaluated)
		passed = true;
	else
		log.println("** Test failed!", myName);
	if(verbose)
		cin.get(temp);
	return passed;
}

void Test::TranspositionTableTest(bool verbose /*=false*/)
{
	MEMORYSTATUS mem;
	int phbefore;
	int pabefore;
	int vbefore;
	GlobalMemoryStatus(&mem);
	phbefore = mem.dwAvailPhys;
	pabefore = mem.dwAvailPageFile;
	vbefore = mem.dwAvailVirtual;

	// create a transposition table, like in MTDFSearch
	// but with a vector and fill it with lots of nodes
	vector<Node> tTable;
	Node n;
	for(int i=0;i<10000;i++)
		tTable.push_back(n);

	// measure and output the size of the transposition table
	// and approximate the size of one Node object
	GlobalMemoryStatus(&mem);
	string message;
	char buffer[50];
	LogWriter log;
	log.println("Mem used: ", myName);
	message = "Phys: ";
	sprintf(buffer, "%d", (phbefore-mem.dwAvailPhys));
	message+=buffer;
	log.println(message, myName);
	message = "Page: ";
	sprintf(buffer, "%d", (pabefore-mem.dwAvailPageFile));
	message+=buffer;
	log.println(message, myName);
	message = "Virt: ";
	sprintf(buffer, "%d", (vbefore-mem.dwAvailVirtual));
	message+=buffer;
	log.println(message, myName);

	message = "sizeof(Node) = ";
	sprintf(buffer, "%d", sizeof(Node));
	message+=buffer;
	message+=" bytes";
	log.println(message, myName);
	message = "My calculated sizeof(Node) = ";
	int size = (vbefore-mem.dwAvailVirtual)/tTable.size();
	sprintf(buffer, "%d", size);
	message+=buffer;
	message+=" bytes";
	log.println(message, myName);
}

void Test::MTDFSearchTest(bool verbose/* = false*/)
{
	// Runs each position according to settings
	// Find "!setupMTDFSearch \""

	// save MTDFSearch setup for restoration after tests
	((Melkor*)melkor)->mtdfSearch->saveSettings();

	// step through the nodes doing setup and adding moves
	BoardStruct b;
	LogWriter log;
	string message;
	char buffer[500];

	// table of results, holding time
	// and test success (true/false)
	vector<ResultsEntry> results; 

	bool waitForUser = true;

	int n, end;
	SGFReader sgf;
	sgf.load("searchTests.sgf");

	string command("!setupMTDFSearch \"");

	int testNumber = 1;
	string testName;
	int boardsExamined = 0;
	int evaluationCalls = 0;

	Move m;
	string comment;
	SGFNode* next = &(sgf.getRootNode());
	// resize the board according to board size in root node
	if(sgf.getBoardSize(comment))
	{
		if((n=atoi(comment.c_str()))>0)
			b.resize(n);
	}
	while(next!=NULL)
	{
		// if there is no move for this node use a pass move
		// so it gets ignored by the BoardStruct::setPoint() routine
		if(!sgf.getMove(next, m))
			Move::getPassMove(m);
		// this function also places any setup stones for this node
		// so we don't need to run the above setupBoardFromSGF function
		addSGFNodeToBoard(&m, sgf, b);
		// if a node comment says:
		// #!setupMTDFSearch "args"
		// then run the setup function
		if(next->getComment(comment))
		{
			// output comment
			log.println(comment, "SGF Comment");
			if((n=comment.find(command))>=0)
			{
				// find closing "
				if((end=comment.find("\"", n+command.size()))<=0)
				{
					message = "No closing double quote found in searchTests.sgf comment for node: ";
					sprintf(buffer, "%d", sgf.getCurrentDepth());
					message+=buffer;
					log.println(message, "Test::MTDFSearchTest");
					message = "Comment is: ";
					message+=comment.substr(n, comment.size());
					log.println(message, "Test::MTDFSearchTest");
					// restore MTDFSearch settings
					((Melkor*)melkor)->mtdfSearch->restoreSettings();
					return;
				}

				if(verbose)
					b.printAsciiBoard(true);

				// don't forget to remove "!setupMTDFSearch" before the args
				if(!((Melkor*)melkor)->mtdfSearch->setupMTDFSearch((&comment.substr(n+command.size(), end-(n+command.size()))), b))
				{
					message = "Error in searchTests.sgf comment for node: ";
					sprintf(buffer, "%d", sgf.getCurrentDepth());
					message+=buffer;
					log.println(message, "Test::MTDFSearchTest");
					message = "Comment is: ";
					message+=comment.substr(n, comment.size());
					log.println(message, "Test::MTDFSearchTest");
					// restore MTDFSearch settings
					((Melkor*)melkor)->mtdfSearch->restoreSettings();
					return;
				}
				bool quit = false;
				bool skip = false;
				// wait for user to proceed
				if(waitForUser) {
					log.println("Press enter to start test (n for no confirmations, s to skip, q to quit).");
					cin.getline(buffer, 50);
					if(strcmp("n", buffer)==0)
						waitForUser = false;
					else if(strcmp("s", buffer)==0)
						skip = true;
					else if(strcmp("q", buffer)==0)
						quit = true;
				}

				if(quit)
					break;
				else if(skip) {
					next = sgf.getNextNode();
					continue;
				}

				bool success = false;

				time_t start;
				time_t finish;
				time(&start);

				int ourColour = ((Melkor*)melkor)->mtdfSearch->getNextToPlay();

// This code is unnecessary since the maximising colour is now determined in suggestMove() using the goals themselves
				// NOTE: ourColour should be the same as the capture string if we have a capture string eval
//				if(((Melkor*)melkor)->mtdfSearch->getEvalFunction()==CAPTURESTRINGEVAL)
//					ourColour = ((Melkor*)melkor)->mtdfSearch->getCaptureStringColour();
				Goal::GoalListType::const_iterator citer(((Melkor*)melkor)->mtdfSearch->getGoal().getGoals().begin());
				for(;citer!=((Melkor*)melkor)->mtdfSearch->getGoal().getGoals().end();citer++) {
					if(citer->goal->testFlags(GoalInterface::TARGETED)) {
						ourColour = b.getPoint(citer->goal->getArg1(), citer->goal->getArg2());
						break;
					}
				}

				float result = ((Melkor*)melkor)->mtdfSearch->suggestMove(m, b, ((Melkor*)melkor)->mtdfSearch->getNextToPlay());

				time(&finish);
				double t = difftime(finish, start);

				vector<Move> l;
				((Melkor*)melkor)->mtdfSearch->getBestLineOfPlay(l);

				// compare with our result was the test passed?
				// read in !answer comment
				if((n=comment.find("!answer \""))>=0) {
					// find closing "
					if((end=comment.find("\"", n+9))<=0) {
						message = "No closing double quote found in searchTests.sgf comment for node: ";
						sprintf(buffer, "%d", sgf.getCurrentDepth());
						message+=buffer;
						log.println(message, "Test::MTDFSearchTest");
						message = "Comment is: ";
						message+=comment.substr(n, comment.size());
						log.println(message, "Test::MTDFSearchTest");
						// restore MTDFSearch settings
						((Melkor*)melkor)->mtdfSearch->restoreSettings();
						return;
					}

					string answer = comment.substr(n+9, end-(n+9));

					// NOTE: !answer string is a list of moves i.e., B[b7]W[a17]
					// multiple correct lines of play maybe separated by '|' (OR) sign
					// e.g. !answer "B[b7]W[a17]|B[d4]W[*]B[q14]"
					// a '*' instead of a move coordinate matches any move by the same colour
					bool error = false;
					bool lookForNewLine = false;
					int current = 0;
					success = false;
					int moveToCheck = 0;
					string tempMove;
					char temp;
					int tempColour;
					int tempX;
					int tempY;
					int tempN;
					
					// remove all '\' in the answer string (SGF uses '\' to escape
					// the use of ']' in comments...
					while((tempN=answer.find("\\"))>0)
						answer.erase(tempN, 1);

					while(current<answer.size() && moveToCheck<l.size())
					{
						if(lookForNewLine)
						{
							// find next '|' and set current to it
							if((current=answer.find("|", current))<=0)
								break;
							lookForNewLine = false;
						}

						// remove whitespace
						if((temp=answer.at(current))==' ' || temp=='\t' || temp=='\n' || temp=='\r')
						{
							current++;
							continue;
						}
						else if(temp=='|')
						{
							// if the previous answer matched
							if(success)
								break;
							moveToCheck = 0;
							current++;
							continue;
						}
						else if(temp=='B' || temp=='W')
						{
							// read in move
							if(temp=='B') tempColour = BLACK;
							else tempColour = WHITE;
							// read in '['
							current++;
							if(current>=answer.size() || answer.at(current)!='[')
							{
								error = true;
								break;
							}

							current++;
							if((end=answer.find("]", current))<=0)
							{
								error = true;
								break;
							}
							tempMove = answer.substr(current, end-current);
					
							// check for '*' wildcard
							if(tempMove=="*")
							{
								if(l.at(moveToCheck).getColour()==tempColour)
								{
									// matching move
									success = true;
									moveToCheck++;
									current+=2;
									continue;
								}
								else
								{
									success = false;
									current+=2;
									lookForNewLine = true;
									continue;
								}
							}

							// otherwise must be a coordinate
							if(tempMove.size()<2 || tempMove.size()>3)
							{
								error = true;
								break;
							}
							tempX = Move::GoPosToX(tempMove);
							tempY = Move::GoPosToY(tempMove, b.getSize());
							if(tempX==-1 || tempY==-1)
							{
								error = true;
								break;
							}
							// compare with MoveToCheck
							if(tempX==l.at(moveToCheck).getX() && tempY==l.at(moveToCheck).getY() &&
								tempColour==l.at(moveToCheck).getColour())
							{
								// matching move
								success = true;
								moveToCheck++;
								current+=(tempMove.size()+1);
								continue;
							}
							else
							{
								success = false;
								current+=(tempMove.size()+1);
								lookForNewLine = true;
								continue;
							}
						}
						else
						{
							error = true;
							break;
						}

						current++;
					}

					if(error)
					{
						message = "Error found in !answer section of searchTests.sgf comment for node: ";
						sprintf(buffer, "%d", sgf.getCurrentDepth());
						message+=buffer;
						log.println(message, "Test::MTDFSearchTest");
						message = "Comment is: ";
						message+=comment.substr(n, comment.size());
						log.println(message, "Test::MTDFSearchTest");
						// restore MTDFSearch settings
						((Melkor*)melkor)->mtdfSearch->restoreSettings();
						return;
					}
				}

				// read test name if available
				if((n=comment.find("!testname \""))>=0)
				{
					// find closing "
					if((end=comment.find("\"", n+11))<=0)
					{
						message = "No closing double quote found in searchTests.sgf comment for node: ";
						sprintf(buffer, "%d", sgf.getCurrentDepth());
						message+=buffer;
						log.println(message, "Test::MTDFSearchTest");
						message = "Comment is: ";
						message+=comment.substr(n, comment.size());
						log.println(message, "Test::MTDFSearchTest");
						// restore MTDFSearch settings
						((Melkor*)melkor)->mtdfSearch->restoreSettings();
						return;
					}
					testName = comment.substr(n+11, end-(n+11));
				}

				bool foundGoodAnswer = true;
				if(((Melkor*)melkor)->mtdfSearch->getTimeRanOut())
				{
					log.println("Time ran out!");
					foundGoodAnswer = false;
				}

				results.push_back(ResultsEntry(testNumber++, testName, t, success,
					((Melkor*)melkor)->mtdfSearch->getNodesTraversed(), 
					((Melkor*)melkor)->mtdfSearch->getNodesEvaluated(), foundGoodAnswer));

				message = "Best move (AB-score, NNScore): ";
				if(m.getColour()==BLACK)
					message+="B[";
				else
					message+="W[";
				message+=m.xyToGoPosValue(m.getX(), m.getY(), m.getBoardSize());
				message+="] ";
				sprintf(buffer, "%d", m.getAlphaBetaScore());
				message+=buffer;
				message+=" ";
				sprintf(buffer, "%*g", 7, m.getNNScore());
				message+=buffer;
				log.println(message);

				message = "Best line of play: ";
				for(int i=0;i<l.size();i++)
				{
					if(l[i].getColour()==BLACK)
						message+="B[";
					else
						message+="W[";
					message+=l[i].xyToGoPosValue(l[i].getX(), l[i].getY(), l[i].getBoardSize());
					message+="] ";
				}
				log.println(message);
				message = "Time taken: ";
				sprintf(buffer, "%*g", 7, t);
				message+=buffer;
				message+=" s";
				log.println(message);

				// wait for user to proceed
				if(waitForUser)
				{
					log.println("Press enter to continue (type s for no confirmations).");
					cin.getline(buffer, 50);
					if(strcmp("s", buffer)==0)
						waitForUser = false;
				}
			}
		}
		// and then doSearch
		next = sgf.getNextNode();
	}

#ifdef PRINT_TEST_SUMMARY
	log.println("\nSUMMARY START");
	char tmpbuf[128];
	time_t ltime;
	time(&ltime);
	struct tm *today;
	today = localtime(&ltime);
	strftime(tmpbuf, 128, "%d/%m/%y", today);
	log.println(tmpbuf);
	log.println(TEST_SUMMARY_HARDWARE_STR);
#ifdef _DEBUG
	log.println("DEBUG version");
#else
	log.println("RELEASE version");
#endif
	sprintf(buffer, "Seed = %ld", random_seed());
	log.println(buffer);
	log.print("\n");
#endif // PRINT_TEST_SUMMARY

	int longestNameLength = 0;
	vector<ResultsEntry>::iterator iter;
	for(iter=results.begin();iter!=results.end();iter++)
	{
		if(iter->name.size()>longestNameLength)
			longestNameLength = iter->name.size();
	}

	log.println("NOTE: A '*' in the success column indicates solution believes it has achieved the goal.\n");

//	message = "Test\tName\t\t\tTime(s)\tSuccess\tBoardsExamined\tEvaluationCalls";
	sprintf(buffer, "%-3s\t%-*s\t%-7s\t%-7s\t%-9s\t%-9s", "No.", longestNameLength, "Name", "Time(s)", "Success",
		"Boards", "EvalCalls");
	log.println(buffer);
	string successString;
	int passes = 0;
	float timeTotal = 0.0f;
	int boardTotal = 0;
	int evalTotal = 0;
	int testTotal = 0;
	for(iter=results.begin();iter!=results.end();iter++)
	{
		testTotal++;
		if(iter->success)
		{
			successString = "Pass";
			passes++;
		}
		else
			successString = "Fail";
		if(iter->successfulAnswer)
			successString+="*";

		sprintf(buffer,  "%-3d\t%-*s\t%-7g\t%-7s\t%-9d\t%-9d", iter->number, longestNameLength, iter->name.c_str(),
			iter->timeTaken, successString.c_str(), iter->boardsExamined, iter->evaluationCalls);
		log.println(buffer);

		timeTotal+=(*iter).timeTaken;
		boardTotal+=(*iter).boardsExamined;
		evalTotal+=(*iter).evaluationCalls;
	}
	char passesTotal[50];
	sprintf(passesTotal, "%d/%d", passes, testTotal);
	sprintf(buffer,  "\n%-3s\t%-*s\t%-7g\t%-7s\t%-9d\t%-9d\n", "Totals", longestNameLength, "", timeTotal, passesTotal,
		boardTotal, evalTotal);
	log.println(buffer);

	// restore MTDFSearch settings
	((Melkor*)melkor)->mtdfSearch->restoreSettings();

	// wait for user to proceed
	log.println("Press enter to continue.");
	cin.getline(buffer, 50);
}

void Test::influenceFunctionTest(bool verbose/* = false*/)
{
	LogWriter log;
	char buffer[50];

	bool pauseAndPrint = false;

	SGFReader sgf;
	sgf.load("influenceTests.sgf");

	// ask if we should pause and print each position and result (if wrong)
	log.println("Press enter to continue (type p followed by enter to pause and print after each position).");
	cin.getline(buffer, 50);
	if(strcmp("p", buffer)==0)
		pauseAndPrint = true;

	// read in a board position
	// run the influence function
	// and compare with the correct answer
	// a triangle indicating black territory and a square white territory

	// step through the nodes doing setup and adding moves
	BoardStruct b(19);
	BoardStruct result(19);
	BoardStruct firstBoard(19);
	Move m;
	string comment;
	int boardSize = 19;
	// resize the board according to board size in root node
	if(sgf.getBoardSize(comment))
	{
		if((boardSize=atoi(comment.c_str()))>0)
		{
			b.resize(boardSize);
			result.resize(boardSize);
		}
	}
	int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	int territory[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	vector<Move> triangles, squares;
	bool error = false;
	bool firstBoardFound = false;
	float score = 0.0f;
	SGFNode* next = &(sgf.getRootNode());
	while(next!=NULL)
	{
		// if there is no move for this node use a pass move
		// so it gets ignored by the BoardStruct::setPoint() routine
		if(!sgf.getMove(next, m))
			Move::getPassMove(m);
		// this function also places any setup stones for this node
		// so we don't need to run the above setupBoardFromSGF function
		addSGFNodeToBoard(&m, sgf, b);

		if(!firstBoardFound)
		{
			firstBoardFound = true;
			firstBoard = b;
		}

		if(pauseAndPrint)
		{
			b.printAsciiBoard(true);
			log.println("Press enter to continue");
			cin.getline(buffer, 50);
		}

		// Fill in territory array with BLACK, WHITE or EMPTY according to SGF contents
		// triangle means black territory
		// square means white territory
		next->getTriangles(triangles);
		next->getSquares(squares);
		for(int y=0;y<boardSize;y++)
		{
			for(int x=0;x<boardSize;x++)
				territory[x][y] = EMPTY;
		}

		for(int i=0;i<triangles.size();i++)
			territory[triangles[i].getX()][triangles[i].getY()] = BLACK;

		for(i=0;i<squares.size();i++)
			territory[squares[i].getX()][squares[i].getY()] = WHITE;

		// run the influence function on the current board state
		error = false;
		// NOTE: The example taken from the bouzy paper "Bouzy-IJPRAI.pdf" uses 4 dilations and
		// 13 erosions instead of 5/21
		if(calculateInfluence(b, score, BLACK, "influenceFunctionTest()", inCopy, 4, 13))
		{
			// compare inCopy with the SGF territory encoding

			// inCopy[x][y] > 0 means black territory
			// inCopy[x][y] < 0 means white territory
			for(int y=0;y<boardSize;y++)
			{
				for(int x=0;x<boardSize;x++)
				{
					// black territory
					if(inCopy[x][y]>0)
					{
						result.setPoint(x, y, BLACK);
						// check if correct
						if(b.getPoint(x, y)!=BLACK && territory[x][y]!=BLACK)
						{
							sprintf(buffer, "Output disagrees with answer at %s", Move::xyToGoPosValue(x, y, boardSize).c_str());
							log.println(buffer);
							error = true;
						}
					}
					// white territory
					if(inCopy[x][y]<0)
					{
						result.setPoint(x, y, WHITE);
						if(b.getPoint(x, y)!=WHITE && territory[x][y]!=WHITE)
						{
							sprintf(buffer, "Output disagrees with answer at %s", Move::xyToGoPosValue(x, y, boardSize).c_str());
							log.println(buffer);
							error = true;
						}
					}
				}
			}
			if(pauseAndPrint)
			{
				result.printAsciiBoard(true);
				if(error)
					log.println("Error occurred, difference between output and correct answer.");
				log.println("Press enter to continue");
				cin.getline(buffer, 50);
			}
		}

		// goto next sgf node
		next = sgf.getNextNode();
	} // end while(next!=NULL)

	// Now run the influence function timing test
	int runNTimes = 1000;
	sprintf(buffer, "Running influence function timing test %d times...", runNTimes);
	log.println(buffer);
	log.println("Press enter to continue");
	cin.getline(buffer, 50);

	timerStart();
	for(int i=0;i<runNTimes;i++)
		calculateInfluence(firstBoard, score, BLACK, "influenceFunctionTest()", inCopy, 5, 21);

	timerEnd();

	sprintf(buffer, "It took %g seconds to run the influence function %d times.", timerValue(),
		runNTimes);
	log.println(buffer);
	float millisecondsPerRun = 0;
	if(runNTimes > 0)
		millisecondsPerRun = (timerValue()/runNTimes)*1000;
	sprintf(buffer, "Average is %g ms per run or %g uSec.", millisecondsPerRun, millisecondsPerRun*1000);
	log.println(buffer);

	// Try the Liberty count function for comparison
	Goal goal(new LibertyCountGoal());
	// Liberty count is much faster so must run more to get accurate result
	runNTimes = 100000;
	sprintf(buffer, "Running liberty count function timing test %d times...", runNTimes);
	log.println(buffer);
	log.println("Press enter to continue");
	cin.getline(buffer, 50);

	timerStart();
	for(i=0;i<runNTimes;i++)
		goal.successWithBoardAfterMoves(BLACK, score, firstBoard, 0, false, BLACK);

	timerEnd();

	sprintf(buffer, "It took %g seconds to run the liberty count function %d times.", timerValue(),
		runNTimes);
	log.println(buffer);
	millisecondsPerRun = 0;
	if(runNTimes > 0)
		millisecondsPerRun = (timerValue()/runNTimes)*1000;
	sprintf(buffer, "Average is %g ms per run or %g uSec.", millisecondsPerRun, millisecondsPerRun*1000);
	log.println(buffer);

	// Try the stone count function for comparison
	goal.clearGoals();
	goal.addGoal(new StoneCountGoal());
	// Stone count is much faster so must run more to get accurate result
	runNTimes = 100000;
	sprintf(buffer, "Running stone count function timing test %d times...", runNTimes);
	log.println(buffer);
	log.println("Press enter to continue");
	cin.getline(buffer, 50);

	timerStart();
	for(i=0;i<runNTimes;i++)
		goal.successWithBoardAfterMoves(BLACK, score, firstBoard, 0, false, BLACK);

	timerEnd();

	sprintf(buffer, "It took %g seconds to run the stone count function %d times.", timerValue(),
		runNTimes);
	log.println(buffer);
	millisecondsPerRun = 0;
	if(runNTimes > 0)
		millisecondsPerRun = (timerValue()/runNTimes)*1000;
	sprintf(buffer, "Average is %g ms per run or %g uSec.", millisecondsPerRun, millisecondsPerRun*1000);
	log.println(buffer);

	log.println("Press enter to continue");
	cin.getline(buffer, 50);
}

void Test::ladderTest(bool verbose /*=false*/)
{
	LogWriter log;

	// Setup a small ladder in the corner
	// wwbb...
	// bwwb...
	// bbw....
	// .......
	BoardStruct b(19);
	b.setPoint(0, 0, WHITE);
	b.setPoint(1, 0, WHITE);
	b.setPoint(2, 0, BLACK);
	b.setPoint(3, 0, BLACK);
	b.setPoint(0, 1, BLACK);
	b.setPoint(1, 1, WHITE);
	b.setPoint(2, 1, WHITE);
	b.setPoint(3, 1, BLACK);
	b.setPoint(0, 2, BLACK);
	b.setPoint(1, 2, BLACK);
	b.setPoint(2, 2, WHITE);

	b.printAsciiBoard();

	float alpha = MTDFSearch::NEGINFINITY;
	float beta = MTDFSearch::POSINFINITY;

	int maxDepth = 10;
	int nextToPlay = BLACK;
	Node root(b, nextToPlay);

	// Save MTDFSearch setup for restoration after tests
	((Melkor*)melkor)->mtdfSearch->saveSettings();

	// Set the goal
	((Melkor*)melkor)->mtdfSearch->setGoal(Goal(new LadderGoal(0, 0)));
	((Melkor*)melkor)->mtdfSearch->setMoveSuggester( new LadderSuggester() );

	/********************/
	/* Plain Alpha-Beta */
	/********************/

	AlphaBetaHelper("Testing ladder search with Iterative Deepening, BLACK to play", 
		false, false, false, 0.0f, false, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
		new LadderSuggester(), Goal(new LadderGoal(0,0)), b, 
		false, true, maxDepth, 1, BLACK, verbose,     /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
		-1.0f, -1, -1, 1, 1);                          /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */

	AlphaBetaHelper("Testing ladder search with Iterative Deepening, WHITE to play", 
		false, false, false, 0.0f, false, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
		new LadderSuggester(), Goal(new LadderGoal(0,0)), b, 
		false, true, maxDepth, 1, WHITE, verbose,     /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
		4.0f, 2, 3, 4, 3);                          /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */

	/************************************/
	/* Alpha-Beta with all enhancements */
	/************************************/

	AlphaBetaHelper("Testing ladder search with Iterative Deepening, TT, ETC, BMF, BLACK to play", 
		true, true, false, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
		new LadderSuggester(), Goal(new LadderGoal(0,0)), b, 
		false, true, maxDepth, 1, BLACK, verbose,     /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
		-1.0f, -1, -1, 1, 1);                          /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */

	AlphaBetaHelper("Testing ladder search with Iterative Deepening, TT, ETC, BMF, WHITE to play", 
		true, true, false, 0.0f, true, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
		new LadderSuggester(), Goal(new LadderGoal(0,0)), b, 
		false, true, maxDepth, 1, WHITE, verbose,     /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
		4.0f, 2, 3, 4, 3);                          /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */

	/**********************/
	/* Trace ladder tests */
	/**********************/

	log.println("Testing LadderGoal::traceLadder()...");

	/**********************/
	/* No ladder blockers */
	/**********************/

	// Setup a ladder in the corner
	//
	//   ABCDEFG
	//19 .......
	//18 ..bb...
	//17 .bww...
	//16 ..b....
	//15 .......
	//14 .......

	b.clear(19);
	b.setPoint(2, 1, BLACK);
	b.setPoint(3, 1, BLACK);
	b.setPoint(1, 2, BLACK);
	b.setPoint(2, 2, WHITE);
	b.setPoint(3, 2, WHITE);
	b.setPoint(2, 3, BLACK);

	b.printAsciiBoard();

	ladderTestHelper(b, Point<>(2, 2), Point<>(4, 2), Point<>(3, 3), verbose);

	/***************************/
	/* Friendly ladder blocker */
	/***************************/

	b.setPoint(6, 4, WHITE);
	b.printAsciiBoard();

	ladderTestHelper(b, Point<>(2, 2), Point<>(4, 2), Point<>(3, 3), verbose);

	/************************/
	/* Enemy ladder blocker */
	/************************/

	b.setPoint(6, 4, BLACK);
	b.printAsciiBoard();

	ladderTestHelper(b, Point<>(2, 2), Point<>(4, 2), Point<>(3, 3), verbose);

	/***********************************************************/
	/* Diagonal pair ladder blockers, inner friendly so ignore */
	/***********************************************************/

	b.setPoint(6, 4, BLACK);
	b.setPoint(5, 5, WHITE);
	b.printAsciiBoard();

	ladderTestHelper(b, Point<>(2, 2), Point<>(4, 2), Point<>(3, 3), verbose);

	/******************************************************************/
	/* Diagonal pair ladder blockers, inner enemy so change direction */
	/******************************************************************/

	b.setPoint(6, 4, WHITE);
	b.setPoint(5, 5, BLACK);
	b.printAsciiBoard();

	ladderTestHelper(b, Point<>(2, 2), Point<>(4, 2), Point<>(3, 3), verbose);

	/****************************/
	/* Ladder by the board edge */
	/****************************/

	// Setup a ladder in the corner
	//
	//   ABCDEFGHI
	//19 .........
	//18 ....wb...
	//17 .....w...
	//16 .........
	//15 .........
	//14 .........

	b.clear(19);
	b.setPoint(4, 1, WHITE);
	b.setPoint(5, 1, BLACK);
	b.setPoint(5, 2, WHITE);

	b.printAsciiBoard();

	AlphaBetaHelper("Testing ladder search by the edge with Iterative Deepening, BLACK to play", 
		false, false, false, 0.0f, false, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
		new LadderSuggester(), Goal(new LadderGoal(5,1)), b, 
		false, true, maxDepth, 1, BLACK, verbose,     /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
		4, 6, 1, 4, 3);                          /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */

	AlphaBetaHelper("Testing ladder search by the edge with Iterative Deepening, WHITE to play", 
		false, false, false, 0.0f, false, 0.0f,     /* bmf, etc, moveQualityThreshold, pruneValue, ttable, passValue */
		new LadderSuggester(), Goal(new LadderGoal(5,1)), b, 
		false, true, maxDepth, 1, WHITE, verbose,     /* useMTDF, useID, depth, IDStep, ourColour, verbosity */
		-1.0f, -1, -1, 1, 1);                          /* rSCore, rX, rY, rNodesTraversed, rNodesEvaluated */

	// Restore the old settings before leaving
	((Melkor*)melkor)->mtdfSearch->restoreSettings();
}

void Test::ladderTestHelper(const BoardStruct& board, const Point<>& ladderCoords, const Point<>& order3Lib, 
							const Point<>& order2Lib, const bool verbose)
{
	LogWriter log;
	char buffer[2000];
	LadderGoal ladderGoal(ladderCoords.x, ladderCoords.y);
	int ladderDX, ladderDY;

	ladderGoal.getLadderDirection(order3Lib, order2Lib, ladderDX, ladderDY, board);
	sprintf(buffer, "\nInitial ladderDX=%d ladderDY=%d", ladderDX, ladderDY);
	log.println(buffer);

	// Get the initial ladder direction
	int pointDX, pointDY;
	ladderGoal.findInitialLadderPointDirection(order3Lib, ladderDX, ladderDY, pointDX, pointDY, board);
	sprintf(buffer, "Initial pointDX=%d pointDY=%d", pointDX, pointDY);
	log.println(buffer);

	// Print a list of points in the potential ladder
	log.print("Ladder points:");
	Point<> currentPoint(order3Lib);
	while(currentPoint.x>=0 && currentPoint.x<board.getSize() && currentPoint.y>=0 && currentPoint.y<board.getSize()) {
		sprintf(buffer, " (%d, %d),", currentPoint.x, currentPoint.y);
		log.print(buffer);
		ladderGoal.getNextLadderPointDirection(ladderDX, ladderDY, pointDX, pointDY);
		currentPoint.x+=pointDX;
		currentPoint.y+=pointDY;
	}
	log.println("");

	// Actually trace the ladder
	float score = ladderGoal.traceLadder(order3Lib, order2Lib, board);
	sprintf(buffer, "Ladder score = %g", score);
	log.println(buffer);

	char temp;
	if(verbose)
		cin.get(temp);
}

void Test::getAnswerTest(bool verbose /*=false*/)
{
	double t;
	register int i;

	LogWriter log;
	char buffer[2000];

	BPNGoAdapterInterface& bpnGo = *((Melkor*)melkor)->getBPN();
	BPN& bpn = bpnGo.getBPN();

	// setup a fairly full board to run the bpn on
	// Board position
	// *w*w*
	// w*www
	// *w*bb
	// wwbb*
	// *wb*b

	BoardStruct board(5);
	board.setPoint(0, 1, WHITE);
	board.setPoint(0, 3, WHITE);
	board.setPoint(1, 0, WHITE);
	board.setPoint(1, 2, WHITE);
	board.setPoint(1, 3, WHITE);
	board.setPoint(1, 4, WHITE);
	board.setPoint(2, 1, WHITE);
	board.setPoint(2, 3, BLACK);
	board.setPoint(2, 4, BLACK);
	board.setPoint(3, 0, WHITE);
	board.setPoint(3, 1, WHITE);
	board.setPoint(3, 2, BLACK);
	board.setPoint(3, 3, BLACK);
	board.setPoint(4, 1, WHITE);
	board.setPoint(4, 2, BLACK);
	board.setPoint(4, 4, BLACK);

	Matrix<float> input;
	bpnGo.getInput(2, 2, board, input, BLACK);

	int numberOfTests;
#ifdef _DEBUG
	numberOfTests = 100;
#else
	numberOfTests = 100000;
#endif

	sprintf(buffer, "Testing BPN::unoptimisedGetAnswer(), running %d times...", numberOfTests);
	log.println(buffer);

	timerStart();
	for(i=0;i<numberOfTests;i++)
		bpn.unoptimisedGetAnswer(input);
	timerEnd();
	
	sprintf(buffer, "NNScore for move at 2,2 = %g\n", bpn.getOutputs()[bpn.getOutputs().size()-1].getValue(0, 0));
	log.println(buffer);
	t = timerValue();
	sprintf(buffer, "Time taken: %*gs\n", 7, t);
	log.println(buffer);

	sprintf(buffer, "Testing BPN::highPerformanceGetAnswer(), running %d times...", numberOfTests);
	log.println(buffer);

	timerStart();
	for(i=0;i<numberOfTests;i++)
		bpn.highPerformanceGetAnswer(input);
	timerEnd();
	
	sprintf(buffer, "NNScore for move at 2,2 = %g\n", bpn.getOutputs()[bpn.getOutputs().size()-1].getValue(0, 0));
	log.println(buffer);
	t = timerValue();
	sprintf(buffer, "Time taken: %*gs\n", 7, t);
	log.println(buffer);

	// Do the network specific getAnswer tests
	if(bpn.getWeights().size()==2 && bpn.getWeights()[0].getHeight()==342 && bpn.getWeights()[1].getHeight()==40)
	{
		sprintf(buffer, "Testing network specific BPN::highPerformanceGetAnswer342-40-1(), running %d times...", numberOfTests);
		log.println(buffer);
		
		timerStart();
		for(i=0;i<numberOfTests;i++)
			bpn.highPerformanceGetAnswer342_40_1_SIGMOID(input);
		timerEnd();
		
		sprintf(buffer, "NNScore for move at 2,2 = %g\n", bpn.getOutputs()[bpn.getOutputs().size()-1].getValue(0, 0));
		log.println(buffer);
		t = timerValue();
		sprintf(buffer, "Time taken: %*gs\n", 7, t);
		log.println(buffer);
		
	}
}