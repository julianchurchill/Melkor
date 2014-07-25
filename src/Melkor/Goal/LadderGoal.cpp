#include "LadderGoal.h"
#include "Misc.h"
#include "Go.h"

bool LadderGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
											  const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	int targetSize = target.size();
	for(int i=0;i<targetSize;i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, targetSize, factorInSolutionSize, nextToPlay);
}

bool LadderGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
											 bool factorInSolutionSize, const int nextToPlay)
{
	bool returnValue = evaluate(ourColour, score, b, nextToPlay);

	if(factorInSolutionSize)
		score = score/targetSize;

	return returnValue;
}

// game tree search can handle +.v.e. and -.v.e. evaluations
// NOTE: Game tree searches with capture goals should _always_ be
// from the point of view of the string being attacked,
// i.e. maximisingColour = b.getString(arg1, arg2)
// ALIVE is positive and DEAD is negative.
// Note: ALIVE is guaranteed to be greater than the maximum possible
// liberties for a string on a board size == to b.size.
/** @brief Evaluate the position. 
  * @return The return value appears to be unused - confirm... */
bool LadderGoal::evaluate(const int maximisingColour, float &score, const BoardStruct &b,
										   const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	// Remember, this goal is terminal therefore the alpha-beta search will end when
	// DEAD or LADDER_ESCAPED is reached, whether or not you return ALIVE here.

	BoardStruct currentBoard(b);
	int currentNextToPlay = nextToPlay;
	BoardStruct::constStringsIterator citer(currentBoard.getStringConst(arg1, arg2));
	if(b.getPoint(arg1, arg2)==EMPTY) {
		score = DEAD;
		return true;
	}
	// If we've connected to an unconditionally alive string or we've made 2 eyes...
	else if(citer->getStatus()==ALIVE || citer->getRealEyeCount()>=2) {
		score = ALIVE;
		return true;
	}

	/* This looks ahead in the path of the ladder for ladder blockers.
	 * Return alive or dead if a ladder blocked found, depending on blocker colour. */

	// Is this a traceable ladder position, i.e. like 1) or 4) but not 2) or 3)?:
	//
	//    ........       .......        .....bb..      ........
	// 1) .bbb....    2) .bb....     3) ....bwb..   4) .bbb....
	//    .bwb....       .bw....        .....wb..      .bwb....
	//    .bw.....       .......        ......b..      .bw.....
	//    ........       .......        ......b..      ..b.....
	//    ........       .......        ......b..      ........
	//
	// A traceable ladder string must have:
    //  1. Target must have precisely 2 liberties.
    //  2. Target must have 1 liberty of order 3 (has 3 liberties of its own) _or_
    //     1 liberty of order 2 next to a board edge - see example a) below.
    //  3. Target must have 1 liberty of order 2 (has 2 liberties of its own).
    //  4. The target's two liberties must be diagonally adjacent to each other.
    
	// Trace the string if:
	//    a) The target string is traceable AND nextToPlay==enemy, see 1).
	// or b) The target string has 1 liberty total AND it is order 2 AND nextToPlay == target,
	//       see 4). These strings are changed to assume that the target will play on their
	//       last remaining liberty, thereby converting the string into a traceable one like a).
	//
	// - 2 libs of order 2 implies a choice of attack for enemy as in 3), ignore these.

	// If it is the ladders turn to play and it has only 1 liberty, play on it
	// and assess it as if it's now the enemies turn...
	if(currentNextToPlay==citer->getColour() && citer->getLibertyCount()==1) {
		Point<> lib = *(citer->getLibertyPoints().begin());
		if(Go::isLegal(currentBoard, lib.x, lib.y, currentNextToPlay, Go::getUseSuperKo(), false)) {
			currentBoard.addToBoard( Move(lib.x, lib.y, currentNextToPlay, b.getSize()) );
			if(currentNextToPlay==BLACK)
				currentNextToPlay = WHITE;
			else
				currentNextToPlay = BLACK;
		}
	}

	citer = currentBoard.getStringConst(arg1, arg2);
	if(currentNextToPlay!=citer->getColour() && citer->getLibertyCount()==2) {
		const GoString::libertiesTable& liberties = citer->getLibertyPoints();
		GoString::constLibertiesIterator liter(liberties.begin());
		Point<> firstLib = *liter;
		int firstLibOrder = Go::countLibs(firstLib.x, firstLib.y, currentBoard);
		liter++;
		Point<> secondLib = *liter;
		int secondLibOrder = Go::countLibs(secondLib.x, secondLib.y, currentBoard);
		// Now check that the liberties are diagonally adjacent
		if((firstLib.x==secondLib.x-1 || firstLib.x==secondLib.x+1) &&
		   (firstLib.y==secondLib.y-1 || firstLib.y==secondLib.y+1)) {
			// Check the liberty orders - we need an order 2 and an order 3 (or order 2 next to an edge)
			if((firstLibOrder==3 || (firstLibOrder==2 && Go::isAdjacentToEdge(firstLib, b.getSize())))
			   && secondLibOrder==2) {
				score = traceLadder(firstLib, secondLib, currentBoard);
				// The only first move for ladders is the 3rd order liberty
				if(bestX!=NULL) *bestX = firstLib.x;
				if(bestY!=NULL) *bestY = firstLib.y;
				return true;
			}
			else if((secondLibOrder==3 || (secondLibOrder==2 && Go::isAdjacentToEdge(secondLib, b.getSize())))
			   && firstLibOrder==2) {
				score = traceLadder(secondLib, firstLib, currentBoard);
				// The only first move for ladders is the 3rd order liberty
				if(bestX!=NULL) *bestX = secondLib.x;
				if(bestY!=NULL) *bestY = secondLib.y;
				return true;
			}
		}
	}

	// Failing any other return value - just return the number of liberties of the string
	citer = b.getStringConst(arg1, arg2);
	score = citer->getLibertyCount();
	return false;
}

/** @brief Trace the path of the ladder and determine its outcome.
  * See Design\LadderReading.txt. Note: The ladder initiating liberty
  * should always have exactly 3 liberties of its own and should be diagonally
  * adjacent to order2Lib.
  * @param order3Lib The coordinates of the ladder initiating liberty. Must have 3 liberties of it's own.
  * @param order2Lib The coordinates of the 2nd liberty in the ladder trace. Must have 2 liberties of it's own.
  * @param board The board on which this ladder lies.
  * @return One of LADDER_ESCAPED or DEAD. */
float LadderGoal::traceLadder(Point<> order3Lib, Point<> order2Lib, const BoardStruct& board)
{
	int friendlyColour = board.getStringConst(arg1, arg2)->getColour();
	int enemyColour = BLACK;
	if(friendlyColour==BLACK) enemyColour = WHITE;

	// Determine the current ladder direction
		// Check the liberties' direction from the string
	int ladderDX;
	int ladderDY;
	getLadderDirection(order3Lib, order2Lib, ladderDX, ladderDY, board);

	// What direction is the tracer going in at present?
	int pointDX = 0;
	int pointDY = 0;
	findInitialLadderPointDirection(order3Lib, ladderDX, ladderDY, pointDX, pointDY, board);

	// Trace it according to the standard order of points
	Point<> currentPoint(order3Lib);
	int stoneColour;

	// Make sure that the direction is never 0,0 otherwise it will never move!
	assert(!(pointDX==0 && pointDY==0));

	// The while loop below follows these rules:
		// If the trace hits the board edge, return DEAD.
		// If the trace hits a stone
			// If it's part of a diagonal pair of differing colours
				// If the inner stone is friendly, skip the diagonal pair
				// If the inner stone is enemy, change direction
			// If the stone is friendly, return LADDER_ESCAPED
			// If the stone is enemy, return DEAD.
	while(true) {
		if(currentPoint.x<0 || currentPoint.x>=board.getSize() || currentPoint.y<0 || currentPoint.y>=board.getSize())
			return DEAD;
		else if((stoneColour=board.getPoint(currentPoint.x, currentPoint.y))!=EMPTY) {
			getNextLadderPointDirection(ladderDX, ladderDY, pointDX, pointDY);
			Point<> nextPoint(currentPoint);
			nextPoint.x+=pointDX;
			nextPoint.y+=pointDY;
			int oppositeColour = BLACK;
			if(stoneColour==BLACK) oppositeColour = WHITE;
			// Is this stone part of a diagonal pair of different colours?
			if(pointDX!=0 && pointDY!=0 && board.getPoint(nextPoint.x, nextPoint.y)==oppositeColour) {
				// NOTE: Inner stone is always the second in the diagonal pair, i.e. oppositeColour
				// If the inner stone is friendly, skip the diagonal pair
				if(oppositeColour==friendlyColour) {
					getNextLadderPointDirection(ladderDX, ladderDY, pointDX, pointDY);
					nextPoint.x+=pointDX;
					nextPoint.y+=pointDY;
				}
				// If the inner stone is enemy, change direction
				else {
					// New ladder direction is the same as the direction of the diagonal pair
					int newLadderDX = pointDX;
					int newLadderDY = pointDY;
					// Current point will be one step further from nextPoint in pointDX/pointDY direction
					nextPoint.x+=pointDX;
					nextPoint.y+=pointDY;
					// New point direction is the stable ladder direction:
					// i.e. the ladder direction will have changed by 90 degrees so may have been
					// SE, now SW, but is still S - that is the new point direction!
					pointDX = 0;
					pointDY = 0;
					if(newLadderDX==ladderDX) pointDX = newLadderDX;
					if(newLadderDY==ladderDY) pointDY = newLadderDY;
					ladderDX = newLadderDX;
					ladderDY = newLadderDY;
				}
			}
			else if(stoneColour==friendlyColour)
				return LADDER_ESCAPED;
			else if(stoneColour==enemyColour)
				return DEAD;
			// Update the current point position
			currentPoint.x = nextPoint.x;
			currentPoint.y = nextPoint.y;
		}
		else {
			// Update the current point position based on the point and ladder direction
			getNextLadderPointDirection(ladderDX, ladderDY, pointDX, pointDY);
			currentPoint.x+=pointDX;
			currentPoint.y+=pointDY;
		}
	}
}

/** @brief Find the next direction for the internal ladder points given the current direction and 
  * the actual ladder direction.
  * @param ladderDX The ladder x gradient.
  * @param ladderDY The ladder y gradient.
  * @param pointDX The current internal ladder points x gradient.
  * @param pointDY The current internal ladder points y gradient. */
void LadderGoal::getNextLadderPointDirection(int ladderDX, int ladderDY, int& pointDX, int& pointDY)
{
	// South east
	if(ladderDX==1 && ladderDY==1) {
		if(pointDX==1 && pointDY==0) {  // East
			pointDX = -1;
			pointDY = 1;
		}
		else if(pointDX==-1 && pointDY==1) {  // South west
			pointDX = 0;
			pointDY = 1;
		}
		else if(pointDX==0 && pointDY==1) {  // South
			pointDX = 1;
			pointDY = -1;
		}
		else if(pointDX==1 && pointDY==-1) {  // North east
			pointDX = 1;
			pointDY = 0;
		}
	}
	// North east
	else if(ladderDX==1 && ladderDY==-1) {
		if(pointDX==0 && pointDY==-1) {  // North
			pointDX = 1;
			pointDY = 1;
		}
		else if(pointDX==1 && pointDY==1) { // South east
			pointDX = 1;
			pointDY = 0;
		}
		else if(pointDX==1 && pointDY==0) { // East
			pointDX = -1;
			pointDY = -1;
		}
		else if(pointDX==-1 && pointDY==-1) { // North west
			pointDX = 0;
			pointDY = -1;
		}
	}
	// South west
	else if(ladderDX==-1 && ladderDY==1) {
		if(pointDX==0 && pointDY==1) { // South
			pointDX = -1;
			pointDY = -1;
		}
		else if(pointDX==-1 && pointDY==-1) { // North west
			pointDX = -1;
			pointDY = 0;
		}
		else if(pointDX==-1 && pointDY==0) { // West
			pointDX = 1;
			pointDY = 1;
		}
		else if(pointDX==1 && pointDY==1) { // South east
			pointDX = 0;
			pointDY = 1;
		}
	}
	// North west
	else if(ladderDX==-1 && ladderDY==-1) {
		if(pointDX==-1 && pointDY==0) { // West
			pointDX = 1;
			pointDY = -1;
		}
		else if(pointDX==1 && pointDY==-1) { // North east
			pointDX = 0;
			pointDY = -1;
		}
		else if(pointDX==0 && pointDY==-1) { // North
			pointDX = -1;
			pointDY = 1;
		}
		else if(pointDX==-1 && pointDY==1) { // South west
			pointDX = -1;
			pointDY = 0;
		}
	}
}

/** @brief Find the initial internal direction of the ladder points.
  * @param order3Lib The initiating liberty of the ladder, must have 3 liberties itself.
  * @param ladderDX The ladder x gradient.
  * @param ladderDY The ladder y gradient.
  * @param pointDX The current internal ladder points x gradient.
  * @param pointDY The current internal ladder points y gradient.
  * @param board The board on which this ladder lies. */
void LadderGoal::findInitialLadderPointDirection(Point<> order3Lib, int ladderDX, int ladderDY, int& pointDX, int& pointDY, 
												 const BoardStruct& board)
{
	// Determine the initial direction the ladders points are going in
	// Will be either ladderDX or ladderDY.
	pointDX = 0;
	pointDY = 0;
	BoardStruct::constStringsIterator citer(board.getStringConst(arg1, arg2));
	if(citer->containsPoint(order3Lib.x-ladderDX, order3Lib.y))
		pointDX = ladderDX;
	else if(citer->containsPoint(order3Lib.x, order3Lib.y-ladderDY))
		pointDY = ladderDY;
}

/** @brief Find the current ladder direction from the 2nd and 3rd order liberties. */
void LadderGoal::getLadderDirection(const Point<> order3Lib, const Point<> order2Lib, int& ladderDX, int& ladderDY, 
												 const BoardStruct& board)
{
	ladderDX = ladderDY = -1;

	// Find the ladder head
	BoardStruct::constStringsIterator citer(board.getStringConst(arg1, arg2));

	// 3a
	// b2
	if(order3Lib.y<order2Lib.y && order3Lib.x<order2Lib.x) {
		if(citer->containsPoint(order3Lib.x+1, order3Lib.y)) // Head at 'a'
			ladderDY = 1; // South west
		else                                                 // Head at 'b'
			ladderDX = 1; // North east
	}
	// 2a
	// b3
	else if(order3Lib.y>order2Lib.y && order3Lib.x>order2Lib.x) {
		if(citer->containsPoint(order2Lib.x+1, order2Lib.y)) // Head at 'a'
			ladderDY = 1; // South west
		else                                                 // Head at 'b'
			ladderDX = 1; // North east
	}
	// a3
	// 2b
	else if(order3Lib.y<order2Lib.y && order3Lib.x>order2Lib.x) {
		if(citer->containsPoint(order3Lib.x-1, order3Lib.y)) // Head at 'a'
			ladderDX = ladderDY = 1; // South east
		else                                                 // Head at 'b'
			ladderDX = ladderDY = -1; // North west
	}
	// a2
	// 3b
	else if(order3Lib.y>order2Lib.y && order3Lib.x<order2Lib.x) {
		if(citer->containsPoint(order2Lib.x-1, order2Lib.y)) // Head at 'a'
			ladderDX = ladderDY = 1; // South east
		else                                                 // Head at 'b'
			ladderDX = ladderDY = -1; // North west
	}
}

