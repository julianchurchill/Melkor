#include "Go.h"
#include "Misc.h"
#include <algorithm>

#ifdef INCLUDE_TIMING_CODE
// set this to true to get extra timing info
// from IsLegal() function
#include <ctime>

int Go::IsLegalCalled = 0;
double Go::IsLegalTotalTimeUpdatePoints = 0;
double Go::IsLegalTotalTimeSearch = 0;
double Go::IsLegalTotalTimeSuperKo = 0;
double Go::IsLegalTotalTimeBoardCopy = 0;
time_t Go::changedPointsStart;
time_t Go::changedPointsEnd;
time_t Go::searchStart;
time_t Go::searchEnd;
time_t Go::koStart;
time_t Go::koEnd;
time_t Go::boardCopyStart;
time_t Go::boardCopyEnd;
#endif

vector<Move> Go::moveList;
int Go::currentMoveNumber = 0;
// for checking positional super-ko
vector<Go::BoardKeys> Go::boardKeys;
vector<BoardStruct> Go::boardList;

int Go::nextToPlay = BLACK;
// even handicap
int Go::handicap = 0;
// Chinese rules by default
Go::Rules Go::rules = CHINESE_RULES;
float Go::komi = 5.5;
bool Go::useSuperKo = true;

// Time variables
Go::TimeSystem Go::timeSystem = Go::CANADIAN_BYO_YOMI;
int Go::mainTime = 1200;			// 20 mins
int Go::currentMainTimeBlack = 1200;
int Go::currentMainTimeWhite = 1200;
int Go::byoyomiStones = 25;
int Go::currentByoyomiStonesBlack = 25;
int Go::currentByoyomiStonesWhite = 25;
int Go::byoyomiTime = 600;			// 10 mins
int Go::currentByoyomiTimeBlack = 600;
int Go::currentByoyomiTimeWhite = 600;

BoardStruct Go::board;

bool Go::printExtraDebuggingInfo = false;

const int Go::maxInfluenceBlack = 128;
const int Go::maxInfluenceWhite = -128;

const bool Go::defaultDontFillOwnEyes = false;
bool Go::dontFillOwnEyes = Go::defaultDontFillOwnEyes;

/** Add the specified move to a list of the moves that have been 
played in the game so far. 
@param m The move to add to the end of the list.
@param board A reference boardStruct before the move has been added to it. */
void Go::addToMoveList(Move &m, const BoardStruct& board)
{
	// store board position keys to allow us to check for super-ko
	BoardKeys keys;
	keys.key = board.getKey();
	keys.key2 = board.getKey2();
	keys.previousKey = board.getPreviousKey();
	keys.previousKey2 = board.getPreviousKey2();
	boardKeys.push_back(keys);
	moveList.push_back(m);
	boardList.push_back(board);
	currentMoveNumber++;
}

void Go::undo(BoardStruct& board)
{
	if(moveList.size() == 0) return;

	// copy the old move list
	vector<Move> ml = moveList;
	// reset the board
	board.clear();
	// reset the old move list
	resetMoveList();

	// place the handicap stones
	//GTPFunctions::setHandicap(Go::handicap);
	doFixedHandicap(board);

	// use move list to update board
	// to one move back
	int i = 0;
	while(i<ml.size()-1) {
		//moveAddedSilent(ml[i].x(), ml[i].y(), ml[i].colour());
		board.addToBoard(ml[i]);
		addToMoveList(ml[i], board);
		i++;
	}
}

// NOTE: GTP 2.2 states handicap moves should NOT be added to the
// move list.
void Go::addHandicapMove(int x, int y, BoardStruct &b)
{
	// handicap moves are always black
	b.addToBoard(Move(x, y, BLACK, b.getSize()));
}

bool Go::doFixedHandicap(BoardStruct &b)
{
	vector<Move> moves;
	if(!getFixedHandicapMoves(moves, handicap, b.getSize()))
		return false;

	nextToPlay = WHITE;

	vector<Move>::iterator iter = moves.begin();
	for(;iter!=moves.end();iter++)
		addHandicapMove(iter->getX(), iter->getY(), b);

	return true;
}

// This conforms with the handicap stone placement order
// of the GTP specification 2, draft 2 for boardsizes 9x9, 13x13 and 19x19
bool Go::getFixedHandicapMoves(vector<Move>& moves, int handicap, int size)
{
	// add blacks handicap stones to the board
	if(handicap>0)
	{
		moves.clear();
		if(size==19)
		{
			switch(handicap)
			{
				case 9:
					moves.push_back(Move(9, 9, BLACK, size)); // K10
				case 8:
					moves.push_back(Move(9, 3, BLACK, size)); // K16
					moves.push_back(Move(9, 15, BLACK, size)); // K4
					moves.push_back(Move(15, 9, BLACK, size)); // Q10
					moves.push_back(Move(3, 9, BLACK, size)); // D10
					moves.push_back(Move(15, 15, BLACK, size)); // Q4
					moves.push_back(Move(3, 3, BLACK, size)); // D16
					moves.push_back(Move(15, 3, BLACK, size)); // Q16
					moves.push_back(Move(3, 15, BLACK, size)); // D4
					break;
				case 7:
					moves.push_back(Move(9, 9, BLACK, size)); // K10
				case 6:
					moves.push_back(Move(15, 9, BLACK, size)); // Q10
					moves.push_back(Move(3, 9, BLACK, size)); // D10
					moves.push_back(Move(15, 15, BLACK, size)); // Q4
					moves.push_back(Move(3, 3, BLACK, size)); // D16
					moves.push_back(Move(15, 3, BLACK, size)); // Q16
					moves.push_back(Move(3, 15, BLACK, size)); // D4
					break;
				case 5:
					moves.push_back(Move(9, 9, BLACK, size)); // K10
				case 4:
					moves.push_back(Move(15, 15, BLACK, size)); // Q4
				case 3:
					moves.push_back(Move(3, 3, BLACK, size)); // D16
				case 2:
					moves.push_back(Move(15, 3, BLACK, size)); // Q16
					moves.push_back(Move(3, 15, BLACK, size)); // D4
					break;
				default:
					return false;
			}
			return true;
		} // end if boardsize = 19
		else if(size==13)
		{
			switch(handicap)
			{
				case 9:
					moves.push_back(Move(6, 6, BLACK, size)); // G7
				case 8:
					moves.push_back(Move(6, 3, BLACK, size)); // G10
					moves.push_back(Move(6, 9, BLACK, size)); // G4
					moves.push_back(Move(9, 6, BLACK, size)); // K7
					moves.push_back(Move(3, 6, BLACK, size)); // D7
					moves.push_back(Move(9, 3, BLACK, size)); // K4
					moves.push_back(Move(3, 3, BLACK, size)); // D10
					moves.push_back(Move(9, 3, BLACK, size)); // K10
					moves.push_back(Move(3, 9, BLACK, size)); // D4
					break;
				case 7:
					moves.push_back(Move(6, 6, BLACK, size)); // G7
				case 6:
					moves.push_back(Move(9, 6, BLACK, size)); // K7
					moves.push_back(Move(3, 6, BLACK, size)); // D7
					moves.push_back(Move(9, 3, BLACK, size)); // K4
					moves.push_back(Move(3, 3, BLACK, size)); // D10
					moves.push_back(Move(9, 3, BLACK, size)); // K10
					moves.push_back(Move(3, 9, BLACK, size)); // D4
					break;
				case 5:
					moves.push_back(Move(6, 6, BLACK, size)); // G7
				case 4:
					moves.push_back(Move(9, 3, BLACK, size)); // K4
				case 3:
					moves.push_back(Move(3, 3, BLACK, size)); // D10
				case 2:
					moves.push_back(Move(9, 3, BLACK, size)); // K10
					moves.push_back(Move(3, 9, BLACK, size)); // D4
					break;
				default:
					return false;
			}
			return true;
		}
		else if(size==9)
		{
			switch(handicap)
			{
				case 9:
					moves.push_back(Move(4, 4, BLACK, size)); // E5
				case 8:
					moves.push_back(Move(4, 2, BLACK, size)); // E7
					moves.push_back(Move(4, 6, BLACK, size)); // E3
					moves.push_back(Move(6, 4, BLACK, size)); // G5
					moves.push_back(Move(2, 4, BLACK, size)); // C5
					moves.push_back(Move(6, 6, BLACK, size)); // G3
					moves.push_back(Move(2, 2, BLACK, size)); // C7
					moves.push_back(Move(6, 2, BLACK, size)); // G7
					moves.push_back(Move(2, 6, BLACK, size)); // C3
					break;
				case 7:
					moves.push_back(Move(4, 4, BLACK, size)); // E5
				case 6:
					moves.push_back(Move(6, 4, BLACK, size)); // G5
					moves.push_back(Move(2, 4, BLACK, size)); // C5
					moves.push_back(Move(6, 6, BLACK, size)); // G3
					moves.push_back(Move(2, 2, BLACK, size)); // C7
					moves.push_back(Move(6, 2, BLACK, size)); // G7
					moves.push_back(Move(2, 6, BLACK, size)); // C3
					break;
				case 5:
					moves.push_back(Move(4, 4, BLACK, size)); // E5
				case 4:
					moves.push_back(Move(6, 6, BLACK, size)); // G3
				case 3:
					moves.push_back(Move(2, 2, BLACK, size)); // C7
				case 2:
					moves.push_back(Move(6, 2, BLACK, size)); // G7
					moves.push_back(Move(2, 6, BLACK, size)); // C3
					break;
				default:
					return false;
			}
			return true;
		} // end if boardsize = 9
	}
	return false;
}

bool Go::doFreeHandicap(vector<Move>& stones, BoardStruct &b)
{
	// just do a fixed handicap if offered a free handicap...
	if(!getFixedHandicapMoves(stones, handicap, b.getSize()))
		return false;

	nextToPlay = WHITE;

	vector<Move>::iterator iter = stones.begin();
	for(;iter!=stones.end();iter++)
		addHandicapMove(iter->getX(), iter->getY(), b);

	return true;
}

/** @brief Find and score all liberties of string pointed to by the coordinates. 
 *  Score represents how important this liberty is for attacking and defending.
 *  If toPlay is the same as the target string colour then moves that increase the
 *  targets liberties by capturing adjacent enemy strings will be included.
 *  Moves are sorted high to low.
 *  Score as detailed below:
 *  1. Number of liberties string has when playing the point as target string colour,
 *     if more than one liberty has the highest score according to this then go to 2...
 *  2. Play on exLib with target string colour. Best move is the one that can do the most
 *     damage to us, therefore we should play it.
 *     libsTakenFromUs = 1/'adjacent our colour string libs before'
 *     score = targetStringLibs + libertiesTakenFromUs
 *
 * @param liberties A vector to place each candidate liberty affecting move, including
 *        the exLib score associated with it. @see SpecialPoint
 * @param board The board that contains the target string and it's situation.
 * @param toPlay The colour next to play.
 * @param tX The X coordinate of the target string in board.
 * @param tY The Y coordinate of the target string in board.
 */
void Go::findExtendedLiberties(vector<SpecialPoint>& liberties, const BoardStruct& board, int toPlay, int tX, int tY)
{
	liberties.clear();

	if(board.getPoint(tX, tY)==EMPTY) return;
	BoardStruct::constStringsIterator targetString = board.getStringConst(tX, tY);
	int tColour = targetString->getColour();
	int enemyColour = BLACK;
	if(tColour==BLACK) enemyColour = WHITE;
	const GoString::libertiesTable& libs = targetString->getLibertyPoints();
				
	if(printExtraDebuggingInfo)
	{
		board.printAsciiBoard(true);
		targetString->printInfo();
	}

	// Count liberties of liberties (extended liberties)
	GoString::constLibertiesIterator lIter = libs.begin();
	SpecialPoint p;
	if(lIter!=libs.end())
	{
		BoardStruct otherBoard(board);
		while(lIter!=libs.end())
		{
			p.x = lIter->x;
			p.y = lIter->y;

			// Add a move of the target strings colour at the liberty
			// This allows us to see if liberties are gained from
			// connection to friendly strings or capturing adjacent strings
			otherBoard.setPoint(lIter->x, lIter->y, tColour);
			// if the move is suicide the string may disappear...
			if(otherBoard.getPoint(tX, tY)!=EMPTY)
				p.extra = otherBoard.getStringConst(tX, tY)->getLibertyCount();
			// still add as a liberty but with 0 exLibs
			else
				p.extra = 0;
			liberties.push_back(p);

			// goto the next liberty
			lIter++;
			// reset otherboard if we are going around the loop again
			if(lIter!=libs.end())
				otherBoard = board;
		}
	}

	// The next bit of code deals with this problem:
	// What about moves that capture adjacent enemy strings
	// but the moves are not liberties of the target string
	// and so the target indirectly has it's liberties increased?
	// i.e. (move marked by '*'):
	// bww
	// *..

	// this section only counts if it's the target strings turn to play
	// i.e. it can't save itself by capturing an adjacent enemy string
	// unless it is it's own turn to play!

	if(toPlay==tColour)
	{	
		// find adjacent strings (perhaps write a function to return
		// list of adjacent strings, which must all be enemy strings,
		// since adjacent friendly strings will be connected)
		vector<BoardStruct::constStringsIterator> adjStrings;
		findAdjacentStrings(tX, tY, board, adjStrings);

		if(adjStrings.size()>0)
		{
			BoardStruct otherBoard(board);
			for(int i=0;i<adjStrings.size();i++)
			{
				// find adjacent strings with 1 liberty which is not a liberty
				// of the target string
				const Point<>& pRef = *(adjStrings[i]->getLibertyPoints().begin());
				if(adjStrings[i]->getLibertyCount()==1 && !targetString->containsLiberty(pRef))
				{
					p.x = pRef.x;
					p.y = pRef.y;
					
					// play the capturing move and 
					otherBoard.setPoint(p.x, p.y, tColour);
					// if the move is suicide the string may disappear...
//					BoardStruct::constStringsIterator tString = otherBoard.getString(tX, tY);
//					if(tString!=NULL)
					if(otherBoard.getPoint(tX, tY)!=EMPTY)
					{
						p.extra = otherBoard.getStringConst(tX, tY)->getLibertyCount();
						liberties.push_back(p);
					}
					// if we have to go round this loop at least once more, reset otherBoard
					if(i+1<adjStrings.size())
						otherBoard = board;
				} // end if enemy with 1 liberty and not our liberty
			} // end for adjStrings
		}
	}

	// Count how many moves we have that have the equal top score
	float topScore = -1.0;
	int topScoreCount = 0;
	for(int i=0;i<liberties.size();i++)
	{
		if(liberties[i].extra>topScore)
		{
			topScore = liberties[i].extra;
			topScoreCount = 1;
		}
		else if(liberties[i].extra==topScore)
			topScoreCount++;
	}

	// If we have more than one top scoring move we need to refine the scores some more...
	if(topScoreCount>1)
	{
		// We have more than 1 exLib move with the highest number of extended liberties
		// so the next score to go on is:

		// 2. Play on exLib with target string colour. Best move is the one that can do the most
		//    damage to us, therefore we should play it first.
		//    libsTakenFromUs = 1/'adjacent our colour string libs' (divide by zero check)
		//    score = targetStringLibs + libertiesTakenFromUs

		int targetColour = board.getPoint(tX, tY);
		int otherColour = BLACK;
		if(targetColour==BLACK)
			otherColour = WHITE;
		float libsTakenFromUs = 0.0f;
		float adjStringLibs = 0.0f;

		for(int j=0;j<liberties.size();j++)
		{
			// make temp board and add exLib move in target colour to it
			BoardStruct libBoard(board);
			libBoard.setPoint(liberties[j].x, liberties[j].y, targetColour);
			BoardStruct::constStringsIterator l(libBoard.getStringConst(tX, tY));
			if(l==NULL) 
			{
				// just some low score since it appears we have commited suicide
				// with the target string
				liberties[j].extra = -127;
				continue;
			}

			// find all adjacent opposite colour strings to the exLib we're scoring
			// and add up their liberties
			libsTakenFromUs = 0.0f;
			BoardStruct::constStringsIterator adj[4];
			// North
			if((liberties[j].y-1)>=0 && (adj[0]=libBoard.getStringConst(liberties[j].x, liberties[j].y-1))!=NULL &&
				adj[0]->getColour()==otherColour)
			{
				// divide by zero check
				if((adjStringLibs=(*adj[0]).getLibertyCount())==0)
					libsTakenFromUs+=2;
				else
					libsTakenFromUs+=1/adjStringLibs;
			}
			// South
			if((liberties[j].y+1)<libBoard.getSize() && (adj[1]=libBoard.getStringConst(liberties[j].x, liberties[j].y+1))!=NULL &&
				adj[1]!=adj[0] && adj[1]->getColour()==otherColour)
			{
				// divide by zero check
				if((adjStringLibs=adj[1]->getLibertyCount())==0)
					libsTakenFromUs+=2;
				else
					libsTakenFromUs+=1/adjStringLibs;
			}
			// West
			if((liberties[j].x-1)>=0 && (adj[2]=libBoard.getStringConst(liberties[j].x-1, liberties[j].y))!=NULL &&
				adj[2]!=adj[1] && adj[2]!=adj[0] && adj[2]->getColour()==otherColour)
			{
				// divide by zero check
				if((adjStringLibs=(*adj[2]).getLibertyCount())==0)
					libsTakenFromUs+=2;
				else
					libsTakenFromUs+=1/adjStringLibs;
			}
			// East 
			if((liberties[j].x+1)<libBoard.getSize() && (adj[3]=libBoard.getStringConst(liberties[j].x+1, liberties[j].y))!=NULL &&
				adj[3]!=adj[2] && adj[3]!=adj[1] && adj[3]!=adj[0] && adj[3]->getColour()==otherColour)
			{
				// divide by zero check
				if((adjStringLibs=adj[3]->getLibertyCount())==0)
					libsTakenFromUs+=2;
				else
					libsTakenFromUs+=1/adjStringLibs;
			}

			// calculate score
			liberties[j].extra = l->getLibertyCount() + libsTakenFromUs;
		}
	} // end if liberties.size()>1
	
	// order them so they are in ascending ex. libs order
	sort<vector<SpecialPoint>::iterator>(liberties.begin(), liberties.end(), greater<SpecialPoint>());
}

/** @brief Return a vector filled with all extended liberties of the string at point tX, tY
 *  excluding illegal moves. The extended liberties are valued by findExtendedLiberties()
 *  and sorted high to low.
 *
 * @param liberties A vector to place each candidate liberty affecting move, including
 *        the exLib score associated with it. @see SpecialPoint
 * @param board The board that contains the target string and it's situation.
 * @param toPlay The colour next to play.
 * @param tX The X coordinate of the target string in board.
 * @param tY The Y coordinate of the target string in board.
 */
void Go::findLegalExtendedLiberties(vector<SpecialPoint>& liberties, BoardStruct& board, int toPlay, int tX, int tY)
{
	liberties.clear();
	vector<SpecialPoint> filter;
	findExtendedLiberties(filter, board, toPlay, tX, tY);

	if(printExtraDebuggingInfo)
	{
		// print out contents of liberties vector
		string message = "Filter: ";
		char buffer[50];
		for(int i=0;i<filter.size();i++)
		{
			sprintf(buffer, "[%d,%d]=%g ", filter[i].x, filter[i].y, filter[i].extra);
			message+=buffer;
		}
		message+="\n";
		LogWriter::printerr(message, "Go");
	}

	// get extended liberty moves one at a time
	for(int j=0;j<filter.size();j++)
	{
		// is it a legal move?
		if(isLegal(board, filter[j].x, filter[j].y, toPlay, useSuperKo))
			liberties.push_back(filter[j]);
	}
}

// Return an extended liberty attack based on these scores, in order of precendence
// 1. Number of extended liberties (i.e. empty adjacent points to that liberty).
// 2. Play on exLib with target string colour. Score is 
//    'target string libs' - 'adjacent opposite colour strings libs'.
bool Go::getBestExtendedLibertyMove(Move& m, BoardStruct& b, int toPlay,
								int targetX, int targetY)
{
	vector<SpecialPoint> liberties;
	findLegalExtendedLiberties(liberties, b, toPlay, targetX, targetY);

	int index = 0;
	if(liberties.size()==0)
		return false;
	else if(liberties.size()>1)
	{
		// find highest scoring filtered move and return it
		float hiscore = liberties[0].extra;
		for(int j=1;j<liberties.size();j++)
		{
			if(liberties[j].extra>hiscore)
			{
				hiscore = liberties[j].extra;
				index = j;
			}
		}
	}

	m.setCoords(liberties[index].x, liberties[index].y);
	m.setColour(toPlay);
	m.setBoardSize(b.getSize());
	return true;
}

void Go::getAllMoves(vector<Move> &m, const BoardStruct &b)
{
	int h = b.getSize();
	int w = b.getSize();
		
	for(int i=0;i<h;i++) {
		for(int j=0;j<w;j++) {
			if(b.getPoint(j, i)==EMPTY)
				m.push_back(Move(j, i, EMPTY, b.getSize()));
		}
	}
}

void Go::getLegalMoves(vector<Move> &m, const int colour, BoardStruct &b, const bool useSuperKo,
								const Rect* bounds/*=NULL*/, const bool useIllegalMoveMemory /*=false*/,
								const vector<Move>* changedPoints /*=NULL*/, const int ourColour /*=EMPTY*/)
{
	int height = b.getSize();
	int width = b.getSize();
	Move t;
	t.setBoardSize(height);
	
	int top, left, bottom, right;
	if(bounds!=NULL)
	{
		top = bounds->top;
		left = bounds->left;
		bottom = bounds->bottom;
		right = bounds->right;
	}
	else
	{
		top = 0;
		left = 0;
		bottom = height-1;
		right = width-1;
	}

	// only do the changedPoints update once
	isLegal(b, 0, 0, colour, useSuperKo, useIllegalMoveMemory, changedPoints);

	for(int i=top;i<=bottom;i++)
	{
		for(int j=left;j<=right;j++)
		{
			// check if legal move
			if(isLegal(b, j, i, colour, useSuperKo, useIllegalMoveMemory, NULL, false, ourColour))
			{
				t.setX(j);
				t.setY(i);
				t.setColour(colour);
				m.push_back(t);
			}
		} // end for j
	} // end for i
}

void Go::resetIllegalMoveMemory(BoardStruct& tempBoard, const bool useSuperKo)
{
	vector<Move> changedPoints;
	// to start off with changedPoints must contain all the board points
	int bsize = tempBoard.getSize();
	Move tmove;
	tmove.setBoardSize(bsize);
	for(int by=0;by<bsize;by++)
	{
		for(int bx=0;bx<bsize;bx++)
		{
			tmove.setCoords(bx, by);
			tmove.setColour(tempBoard.getPoint(bx, by));
			changedPoints.push_back(tmove);
		}
	}
	// Call isLegal() to actually reset the move memory
	isLegal(tempBoard, 0, 0, BLACK, useSuperKo, true, &changedPoints, true);
}

/** @brief This function returns a list of legal moves given a board state.
  * Until real eye recognition is implemented if Go::dontFillOwnEyes is true
  * this function will exclude real eyes _and_ false eyes from the legal move list until
  * a string connected to the false eye is reduced to 1 liberty, i.e. it can be captured.
  * @param colour The colour of the side trying to play this move.
  * @param ourColour If set to EMPTY indicates we don't care what 'ourColour' is. Otherwise will be
  * used by the dontFillOwnEyes section to decide if we should be excluding moves here. For example
  * we could be using this function to determine what legal moves are available for our enemy, 
  * in which case we don't want to exclude the possibility they will fill their own eyes!
  */
bool Go::isLegal(BoardStruct &b, const int j, const int i, const int colour, const bool useSuperKo, const bool useIllegalMoveMemory /*=false*/,
				 const vector<Move>* changedPoints /*=NULL*/, const bool reset /*=false*/, const int ourColour /*=EMPTY*/)
{
	/** @todo The isLegal() functions need to be decoupled from the illegalMoveMemory
	  *  updating code. A separate function should be used to update the illegalMoveMemory
	  *  for a particular board and then isLegal() can have 'const BoardStruct&' parameter.
	  *  Code will be much safer and less error prone.
	  */

#ifdef INCLUDE_TIMING_CODE
	Go::IsLegalCalled++;
	time(&Go::changedPointsStart);
#endif

	// check for a pass move
	if(j==-1||i==-1)
		return true;

	int height = b.getSize();
	int width = b.getSize();
	
	// if using move memory...
	vector<Move>& illegalMoveMemory = b.getIllegalMoveMemory();
	if(useIllegalMoveMemory && reset)
		illegalMoveMemory.clear();

	if(useIllegalMoveMemory && changedPoints!=NULL) {
		// update the illegal move memory
		int x, y;
		int lx, ly;
		bool suicide = true;
		bool capture = false;

		// step through the changed points
		for(int ci=0;ci<changedPoints->size();ci++) {
			const Move& mr = (*changedPoints)[ci];

			// check for pass move
			if(mr.getX()==-1 || mr.getY()==-1)
				continue;

			// if changed point not empty
			if(mr.getColour()!=EMPTY) {
				bool legal = true;
				int illegalColour = BLACK;

				// check illegalMoveMemory to see if the point on which
				// this stone has just been placed was illegal.
				// it should now be legal in case stone gets captured
				// and we have to recheck this point
				if(!reset) {
					for(int i=0;i<illegalMoveMemory.size();i++) {
						if(illegalMoveMemory[i].getX()==mr.getX() &&
							illegalMoveMemory[i].getY()==mr.getY()) {
							illegalMoveMemory.erase(illegalMoveMemory.begin()+i);
							break;
						}
					} // end for i
				}

				// get string at mr
				// and determine it's colour + enemy colour
				BoardStruct::constStringsIterator s = b.getStringConst(mr.getX(), mr.getY());
				int friendlyColour = s->getColour();
				int enemyColour;
				if(friendlyColour==BLACK) enemyColour = WHITE;
				else if(friendlyColour==WHITE) enemyColour = BLACK;

				// check nsew and
				// if empty:
					// it is legal if for enemy to play here if:
					// i) there is at least one black string with >1 liberty
					//    next to this point
					// ii) or suicide capture available for enemy
					// iii) or empty adjacent point

					// it is illegal to play here for us if:
					// i)  suicide without capture
					// ii) or suicide + capture + one stone + ko point
				// else if enemy string
					// if 1 liberty with no captures for enemy from that liberty
					// liberty is illegal for enemy

				// A liberty of the string, the stone just placed belongs to,
				// may become a suicide move
				if(s->getLibertyCount()==1) {
					// in danger of suiciding on last remaining liberty
					// if the last liberty is also the last liberty of
					// an enemy string (suicide-capture) it's ok to play
					// there, otherwise, it's just suicide...
					int libx = s->getLibertyPoints().begin()->x;
					int liby = s->getLibertyPoints().begin()->y;
					capture = false;
					for(int k=BEGIN_NSEW_DIR;k<=END_NSEW_DIR;k++) {
						if(k==NORTH) { x = 0; y = -1; }
						else if(k==SOUTH) { x = 0; y = 1; }
						else if(k==EAST) { x = 1; y = 0; }
						else { x = -1; y = 0; }
						x+=libx;
						y+=liby;
						if(x<0||y<0||x>=width||y>=height)
							continue;
						int c = b.getPoint(x, y);
						if(c==enemyColour && b.getStringConst(x, y)->getLibertyCount()==1) {
							capture = true;
							// this liberty must be legal for the enemy too, so make
							// sure it is not listed in the illegal move memory anymore
							for(int il=0;il<illegalMoveMemory.size();il++) {
								Move& imm = illegalMoveMemory[il];
								if(imm.getX()==libx && imm.getY()==liby && imm.getColour()==enemyColour) {
									illegalMoveMemory.erase(illegalMoveMemory.begin()+il);
									break;
								}
							}
							break;
						}
						if(c==EMPTY || (c==friendlyColour && b.getStringConst(x, y)->getLibertyCount()>1)) {
							
							// if there is an friendlyColour string (which isn't the one
							// we're investigating!) with greater than 1 liberty
							// then it is ok for us to play here
							// or if there is an adjacent empty point

							// NOTE: It can't be the string we're investigating if
							// it has more than one liberty, we're here because ours
							// has _only_ one liberty
							capture = true;
							break;
						}
					}
					if(!capture)
						addToIllegalMoveMemory(Move(libx, liby, friendlyColour, b.getSize()), illegalMoveMemory);
						//illegalMoveMemory.push_back(Move(libx, liby, friendlyColour));
				}

				// check nsew
				for(int l=BEGIN_NSEW_DIR;l<=END_NSEW_DIR;l++) {
					if(l==NORTH) { x = 0; y = -1; }
					else if(l==SOUTH) { x = 0; y = 1; }
					else if(l==EAST) { x = 1; y = 0; }
					else { x = -1; y = 0; }
					x+=mr.getX();
					y+=mr.getY();
					if(x<0||y<0||x>=width||y>=height)
						continue;

					int c = b.getPoint(x, y);

					// if empty
					if(c==EMPTY) {
						// need to check first if this empty point is illegal
						// for enemy
						if(isSuicide(friendlyColour, enemyColour, x, y, b))
							addToIllegalMoveMemory(Move(x, y, enemyColour, b.getSize()), illegalMoveMemory);
					} // end if c == EMPTY
					// if enemy string
					else if(c==enemyColour) {
						// if enemy string has 1 liberty with no captures for enemy 
						// from that liberty, liberty is illegal for enemy
						BoardStruct::constStringsIterator es = b.getStringConst(x, y);
						if(es->getLibertyCount()==1) {
							Point<> elib = *(es->getLibertyPoints().begin());

							// check if the enemy can capture by playing on that liberty
							capture = false;
				
							// check nsew
							for(int k=BEGIN_NSEW_DIR;k<=END_NSEW_DIR;k++) {
								if(k==NORTH) { lx = 0; ly = -1; }
								else if(k==SOUTH) { lx = 0; ly = 1; }
								else if(k==EAST) { lx = 1; ly = 0; }
								else { lx = -1; ly = 0; }
								lx+=elib.x;
								ly+=elib.y;
								if(lx<0||ly<0||lx>=width||ly>=height)
									continue;
								int rc = b.getPoint(lx, ly);
								if((rc==friendlyColour && b.getStringConst(lx, ly)->getLibertyCount()==1)) {
									// if the enemy can capture from this liberty
									// then it is ok for the enemy 
									// to play here
									capture = true;
									// check if we can capture him also
									// this point may have been previously made illegal
									// as a suicide move but now might be a suicide-capture 
									// move and therefore legal...
									// NOTE: Must be true, we have one liberty
									// he has one liberty, and we're adjacent,
									// therefore shared liberty, so legal for both colours
									// Check illegalMoveMemory
									for(int il=0;il<illegalMoveMemory.size();il++) {
										Move& imm = illegalMoveMemory[il];
										if(imm.getX()==lx && imm.getY()==ly && imm.getColour()==friendlyColour) {
											illegalMoveMemory.erase(illegalMoveMemory.begin()+il);
											break;
										}
									}
									break;
								}
								if(rc==EMPTY || (rc==enemyColour && b.getStringConst(lx, ly)->getLibertyCount()>1)) {
									
									// if there is an enemy string (which isn't the one
									// we're investigating!) with greater than 1 liberty
									// then it is ok for enemy to play here
									// or if there is an adjacent empty point

									// NOTE: It can't be the string we're investigating if
									// it has more than one liberty, we're here because ours
									// has _only_ one liberty
									capture = true;
									break;
								}
							}
							if(!capture)
								addToIllegalMoveMemory(Move(elib.x, elib.y, enemyColour, b.getSize()), illegalMoveMemory);
								//illegalMoveMemory.push_back(Move(elib.x, elib.y, enemyColour));
						} // end if enemy string has 1 liberty
					} // end else
				} // end for l
			} // end if colour != EMPTY
			// else changed point must be an empty point
			else
			{
				// need to check for strings which previously had 1 liberty (now 2 thanks
				// to this empty point) so could have had that liberty being a legal
				// suicide capture point for the enemy, but it isn't now
		
				// check nsew
				int libCount;
				for(int l=BEGIN_NSEW_DIR;l<=END_NSEW_DIR;l++) {
					if(l==NORTH) { x = 0; y = -1; }
					else if(l==SOUTH) { x = 0; y = 1; }
					else if(l==EAST) { x = 1; y = 0; }
					else { x = -1; y = 0; }
					x+=mr.getX();
					y+=mr.getY();
					if(x<0||y<0||x>=width||y>=height)
						continue;
					int c = b.getPoint(x, y);
					if(c!=EMPTY && (libCount=b.getStringConst(x, y)->getLibertyCount())>1) {
//						GoString::libertiesTable* libs = b.getString(x, y)->getLibertyPoints();
						// find liberty that was here before the new one
						GoString::constLibertiesIterator liter(b.getStringConst(x, y)->getLibertyPoints().begin());
						int libx=liter->x;
						int liby=liter->y;
						int start = 0;
						if(libx==mr.getX() && liby==mr.getY()) {
							liter++;
							start++;
							libx = liter->x;
							liby = liter->y;
						}
						// is the old liberty a suicide point for the enemy now?
						// (it may have been a suicide capture point before...)
						int friendlyColour = c;
						int enemyColour = BLACK;
						if(friendlyColour==BLACK) enemyColour = WHITE;
						// loop through liberties checking each
						// for illegal suicide point for enemy
						for(int i=start;i<libCount;i++) {
							libx = liter->x;
							liby = liter->y;
							liter++;
							if(libx==mr.getX() && liby==mr.getY())
								continue;
							if(isSuicide(friendlyColour, enemyColour, libx, liby, b))
								addToIllegalMoveMemory(Move(libx, liby, enemyColour, b.getSize()), illegalMoveMemory);
						}
					}
				} // end for l

			} // end else if colour == EMPTY
		} // end for changedPoints
	} // end if changedPoints != NULL

	// check point is empty first after the above updating
	// has had a chance to trigger
	if(b.getPoint(j, i)!=EMPTY && !reset)
		return false;

	// If Go::dontFillOwnEyes, then this move is illegal if j,i is a real eye
	// NOTE: If the enemy colour played here and captures some stones
	//       then this move should be legal for us
	// NOTE: This won't be saved in the illegal move memory list
	// NOTE: Don't do this check if it's our opponent we are finding legal moves for...
	if(Go::dontFillOwnEyes && (ourColour==EMPTY || colour == ourColour)) {
		// Check each direction has a stone of our colour or a board edge
		// NOTE: This compound if tests:
		//   i) That the potential eye is empty.
		//   ii) If _all_ the adjacent points are ourColour AND
		//       each has adjacent string has >1 liberty (i.e. can't get captured this move!)
		
		/** @todo This needs implementing - until then Go::dontFillOwnEyes will exclude
		  * real eyes and false eyes from the legal move list until a string connected to the
		  * false eye is reduced to 1 liberty, i.e. it can be captured.
		  */
		//   iii) To make sure it's a _real_ eye we must additionally have 3 or more
		//        diagonal points of 'colour' for mid board points,
		//        2 diagonals for edge points and 1 for corner points.
		//        - For each of these 1 or more of the diagonal points may be a real eye instead of
		//          a 'colour' stone.
		if( b.getPoint(j, i)==EMPTY &&
			((i-1)<0 || (b.getPoint(j,i-1)==colour && b.getStringConst(j,i-1)->getLibertyCount()>1)) &&
			((i+1)>=b.getSize() || (b.getPoint(j,i+1)==colour && b.getStringConst(j,i+1)->getLibertyCount()>1)) &&
			((j-1)<0 || (b.getPoint(j-1,i)==colour && b.getStringConst(j-1,i)->getLibertyCount()>1)) &&
			((j+1)>=b.getSize() || (b.getPoint(j+1,i)==colour && b.getStringConst(j+1,i)->getLibertyCount()>1)))
		{
			// Otherwise it's illegal
			return false;
		}
	}

#ifdef INCLUDE_TIMING_CODE
	time(&Go::changedPointsEnd);
	Go::IsLegalTotalTimeUpdatePoints+=difftime(Go::changedPointsEnd, Go::changedPointsStart);
#endif

	if(reset)
		return false;

	if(useIllegalMoveMemory) {
#ifdef INCLUDE_TIMING_CODE
		time(&Go::searchStart);
#endif
		// Alternative Ko checking code
		// if move not illegal return true
		Move temp(j, i, colour, b.getSize());
		if(find(illegalMoveMemory.begin(), illegalMoveMemory.end(), temp)
			== illegalMoveMemory.end()) {
			// check for Ko
			if(b.getKoX()==j && b.getKoY()==i) {
#ifdef INCLUDE_TIMING_CODE
				time(&Go::searchEnd);
				Go::IsLegalTotalTimeSearch+=difftime(Go::searchEnd, Go::searchStart);
#endif
				return false;
			}
		}
		else {
#ifdef INCLUDE_TIMING_CODE
			time(&Go::searchEnd);
			Go::IsLegalTotalTimeSearch+=difftime(Go::searchEnd, Go::searchStart);
#endif
			return false;
		}
		// otherwise (if not Ko)...
		// check positional super-ko if required
		if(useSuperKo) {
#ifdef INCLUDE_TIMING_CODE
			time(&Go::koStart);
			time(&Go::boardCopyStart);
#endif
			// NOTE: It is a lot faster to update and compare the keys like this
			// rather than creating a BoardStruct and doing setPoint()

			// if board position has occurred already in the move list
			// this is an illegal super-ko position
			BOARDSTRUCT_KEY_TYPE key1 = b.getKey();
			BOARDSTRUCT_KEY_TYPE key2 = b.getKey2();
			BoardStruct::updateZobristHashKey(key1, key2, b.getPoint(j, i), colour, j, i);

			// check if anything gets captured by playing at j,i...
			// if it does then all the captured points need to be updateZobristKey(b.getPoint(x,y), EMPTY)
			// to update the keys properly!!!
			
			const GoString* checked[4];
			checked[0] = checked[1] = checked[2] = checked[3] = NULL;
			// NOTE: Suicide moves will have been caught by the illegal move memory
			int enemyColour = WHITE;
			if(colour==WHITE)
				enemyColour = BLACK;
			// west
			if((j+1)<width && b.getPoint(j+1, i)==enemyColour && (checked[0]=&(*b.getStringConst(j+1, i)))->getLibertyCount()==1) {
				// update hash
				GoString::constPointsIterator citer(checked[0]->getPoints().begin());
				for(;citer!=checked[0]->getPoints().end();citer++)
					BoardStruct::updateZobristHashKey(key1, key2, enemyColour, EMPTY, citer->x, citer->y);
			}
			// east
			if((j-1)>0 && b.getPoint(j-1, i)==enemyColour && (checked[1]=&(*b.getStringConst(j-1, i)))->getLibertyCount()==1
				&& checked[0]!=checked[1]) {
				// update hash
				GoString::constPointsIterator citer(checked[1]->getPoints().begin());
				for(;citer!=checked[1]->getPoints().end();citer++)
					BoardStruct::updateZobristHashKey(key1, key2, enemyColour, EMPTY, citer->x, citer->y);
			}
			// south
			if((i+1)<height && b.getPoint(j, i+1)==enemyColour && (checked[2]=&(*b.getStringConst(j, i+1)))->getLibertyCount()==1
				&& checked[0]!=checked[2] && checked[1]!=checked[2]) {
				// update hash
				GoString::constPointsIterator citer(checked[2]->getPoints().begin());
				for(;citer!=checked[2]->getPoints().end();citer++)
					BoardStruct::updateZobristHashKey(key1, key2, enemyColour, EMPTY, citer->x, citer->y);
			}
			// north
			if((i-1)>0 && b.getPoint(j, i-1)==enemyColour && (checked[3]=&(*b.getStringConst(j, i-1)))->getLibertyCount()==1
				&& checked[0]!=checked[3] && checked[1]!=checked[3] && checked[2]!=checked[3]) {
				// update hash
				GoString::constPointsIterator citer(checked[3]->getPoints().begin());
				for(;citer!=checked[3]->getPoints().end();citer++)
					BoardStruct::updateZobristHashKey(key1, key2, enemyColour, EMPTY, citer->x, citer->y);
			}

//			BoardStruct nb(b);
//			nb.setPoint(j, i, colour);

#ifdef INCLUDE_TIMING_CODE
			time(&Go::boardCopyEnd);
			Go::IsLegalTotalTimeBoardCopy+=difftime(Go::boardCopyEnd, Go::boardCopyStart);
#endif
			// if board position has occurred already in the move list
			// this is an illegal super-ko position
			Go::boardKeysType::const_iterator citer(Go::boardKeys.begin());
			for(;citer!=Go::boardKeys.end();citer++) {
				if(citer->key==key1 && citer->key2==key2) {
#ifdef INCLUDE_TIMING_CODE
					time(&Go::searchEnd);
					Go::IsLegalTotalTimeSearch+=difftime(Go::searchEnd, Go::searchStart);
					time(&Go::koEnd);
					Go::IsLegalTotalTimeSuperKo+=difftime(Go::koEnd, Go::koStart);
#endif
					return false;
				}
			}
#ifdef INCLUDE_TIMING_CODE
			time(&Go::koEnd);
			Go::IsLegalTotalTimeSuperKo+=difftime(Go::koEnd, Go::koStart);
#endif
		}
#ifdef INCLUDE_TIMING_CODE
		time(&Go::searchEnd);
		Go::IsLegalTotalTimeSearch+=difftime(Go::searchEnd, Go::searchStart);
#endif
		return true;
	}

	// not using illegal move memory...

	bool result = false;
	int opponent;
	if(colour==BLACK)
		opponent = WHITE;
	else
		opponent = BLACK;
	
	// copy board, very important
	// otherwise GoStrings get modified on 'b'
	BoardStruct nb(b);

	// add move
	nb.setPoint(j, i, colour);
	
	// check for Ko board position repetition
	int pKey = b.getPreviousKey();
	int pKey2 = b.getPreviousKey2();
	if(pKey!=0 && pKey==nb.getKey() && pKey2!=0 && pKey2==nb.getKey2()) {
		// check the previous move was not a pass
		// if it was the next player can break the Ko legally
		// but a pass move doesn't change the board key so 
		// this explicit check is necessary
		if(b.getLastX()!=-1 && b.getLastY()!=-1)
			return false;
	}
	// otherwise (if not Ko)...
	// check positional super-ko if required
	if(useSuperKo) {
		// if board position has occurred already in the move list
		// this is an illegal super-ko position
		Go::boardKeysType::const_iterator citer(Go::boardKeys.begin());
		for(;citer!=Go::boardKeys.end();citer++) {
			if(citer->key==nb.getKey() && citer->key2==nb.getKey2())
				return false;
		}
	}

	// check for suicide capture moves
	// if stone is still on board, then it is still alive
	if(nb.getPoint(j, i)!=EMPTY)
		result = true;
	
	return result;
}

// Return the position at which the sequence is no longer legal
// in failedPos
bool Go::isLegalSequence(const vector<Move>& pVar, const BoardStruct& b, int& failedPos)
{
	BoardStruct tboard(b);
	for(int j=0;j<pVar.size();j++)
	{
		const Move& cm = pVar[j];
		// play on that point if its legal
		if(Go::isLegal(tboard, cm.getX(), cm.getY(), cm.getColour(), Go::getUseSuperKo()))
		{
			Move m(cm.getX(), cm.getY(), cm.getColour(), b.getSize());
			tboard.addToBoard(m);
		}
		else
		{
			failedPos = j;
			return false;
		}
	}
	failedPos = -1;
	return true;
}

bool Go::addToIllegalMoveMemory(Move& m, vector<Move>& illegalMoveMemory)
{
	// check for duplicates
	// NOTE: We are not using set or map to allow for 
	// automatic duplicate checking because they require the
	// use of '<' and '==' on Move.
	// These are already used by MTD(f) etc... to order the moves
	// according to scores.
	// We need to check them by {x,y,colour}
	for(int i=0;i<illegalMoveMemory.size();i++)
	{
		Move& imm = illegalMoveMemory[i];
		if(imm.getX()==m.getX() && imm.getY()==m.getY() && imm.getColour()==m.getColour())
			return false;
	}
	illegalMoveMemory.push_back(m);
	return true;
}

bool Go::isSuicide(int ourColour, int enemyColour, int x, int y, const BoardStruct& b)
{
	int kx, ky, kc;
	bool result = true;
	for(int k=BEGIN_NSEW_DIR;k<=END_NSEW_DIR;k++)
	{
		if(k==NORTH) { kx = 0; ky = -1; }
		else if(k==SOUTH) { kx = 0; ky = 1; }
		else if(k==EAST) { kx = 1; ky = 0; }
		else { kx = -1; ky = 0; }
		kx+=x;
		ky+=y;
		if(kx<0||ky<0||kx>=b.getSize()||ky>=b.getSize())
			continue;
		kc = b.getPoint(kx, ky);
		// it is legal if for enemy to play here if:
		// i) there is at least one enemy string with >1 liberty
		//    next to this point
		if(kc==enemyColour && b.getStringConst(kx, ky)->getLibertyCount()>1)
		{
			result = false;
			break;
		}
		// ii) suicide capture for enemy
		else if(kc==ourColour && b.getStringConst(kx, ky)->getLibertyCount()==1)
		{
			result = false;
			break;
		}
		// iii) or empty adjacent point
		else if(kc==EMPTY)
		{
			result = false;
			break;
		}
	} // end for k
	return result;
}

// get all legal moves and pass them through a filter
void Go::getFilteredLegalMoves(vector<Move> &m, int colour, const BoardStruct &b, bool useSuperKo,
								const Rect* bounds/*=NULL*/)
{
	// get legal moves
	int height = b.getSize();
	int width = b.getSize();
	Move t;
	t.setBoardSize(height);
	
	int top, left, bottom, right;
	if(bounds!=NULL)
	{
		top = bounds->top;
		left = bounds->left;
		bottom = bounds->bottom;
		right = bounds->right;
	}
	else
	{
		top = 0;
		left = 0;
		bottom = height-1;
		right = width-1;
	}

	for(int i=top;i<=bottom;i++)
	{
		for(int j=left;j<=right;j++)
		{
			// check if legal move
			// remove ones of our choosing 
			if(filteredIsLegal(b, j, i, colour, useSuperKo))
			{
				t.setX(j);
				t.setY(i);
				t.setColour(colour);
				m.push_back(t);
			}
		} // end for j
	} // end for i
}

bool Go::filteredIsLegal(const BoardStruct &b, int j, int i, int colour, bool useSuperKo)
{
	// check for a pass move
	if(j==-1||i==-1)
		return true;

	bool result = false;
	int opponent;
	if(colour==BLACK)
		opponent = WHITE;
	else
		opponent = BLACK;	
	
	int height = b.getSize();
	int width = b.getSize();
	
	// copy board, very important
	// otherwise GoStrings get modified on 'b'
	BoardStruct nb(b);

	// check point is empty first
	if(nb.getPoint(j, i)!=EMPTY)
		return false;
	
	// add move
	nb.setPoint(j, i, colour);
	
	// check for Ko board position repetition
	int pKey = b.getPreviousKey();
	int pKey2 = b.getPreviousKey2();
	if(pKey!=0 && pKey==nb.getKey() && pKey2!=0 && pKey2==nb.getKey2())
	{
		// check the previous move was not a pass
		// if it was the next player can break the Ko legally
		// but a pass move doesn't change the board key so 
		// this explicit check is necessary
		if(b.getLastX()!=-1 && b.getLastY()!=-1)
			return false;
	}
	// otherwise (if not Ko)...
	// check positional super-ko if required
	if(useSuperKo)
	{
		// if board position has occurred already in the move list
		// this is an illegal super-ko position
		for(int i=0;i<Go::boardKeys.size();i++)
		{
			if(Go::boardKeys[i].key==nb.getKey() && Go::boardKeys[i].key2==nb.getKey2())
				return false;
		}
	}

	// check for suicide capture moves
	// if stone is still on board, then it is still alive
	if(nb.getPoint(j, i)!=EMPTY)
		result = true;

	/********************************************/
	/* Add our filter conditions here for moves */
	/* we don't like.                           */
	/********************************************/

	// make sure we don't put ourselves into atari (problem
	// with creating snapback situations though...)
	BoardStruct::constStringsIterator s(nb.getStringConst(j, i));
	if(s->getLibertyCount()==1)
		result = false;

	// NOTE: Not necessary to remove move since
	// nb is a temporary copy anyway
	// remove move
	//nb.setPoint(j, i, EMPTY);
	
	return result;	
}

/** @brief Return a count of the liberties of the point x,y even if it is empty. 
  * @param x The x coordinate of the point.
  * @param y The y coordinate of the point.
  * @param board The board to look on.
  * @return The number of liberties of x,y. */
int Go::countLibs(int x, int y, const BoardStruct& board)
{
	int libs = 0;
	int size = board.getSize();
	int ym1 = y-1;
	int ya1 = y+1;
	int xm1 = x-1;
	int xa1 = x+1;

	// north
	if(ym1>=0 && board.getPoint(x, ym1)==EMPTY) libs++;
	// south
	if(ya1<size && board.getPoint(x, ya1)==EMPTY) libs++;
	// west
	if(xm1>=0 && board.getPoint(xm1, y)==EMPTY) libs++;
	// east
	if(xa1<size && board.getPoint(xa1, y)==EMPTY) libs++;

	return libs;
}

/** @brief Return any adjacent strings to the string at tX, tY in temp. 
  * This function will only return strings of opposite colour to the one at
  * tX, tY because like-colour adjacent strings would be part of the string at tX, tY,
  * unless tX, tY is empty. */
void Go::findAdjacentStrings(int tX, int tY, const BoardStruct& temp, vector<BoardStruct::constStringsIterator>& adjStrings)
{
	// Find any GoStrings directly adjacent to the point at tX, tY in BoardStruct temp

	int x, y, dir;
	int size = temp.getSize();
	bool unique;
	if(temp.getPoint(tX, tY)==EMPTY) {
		// look NSEW
		for(dir = BEGIN_NSEW_DIR;dir<=END_NSEW_DIR;dir++)	{
			if(dir==NORTH) {
				x = tX;
				y = tY-1;
			}
			else if(dir==SOUTH) {
				x = tX;
				y = tY+1;
			}
			else if(dir==EAST) {
				x = tX+1;
				y = tY;
			}
			else if(dir==WEST) {
				x = tX-1;
				y = tY;
			}
			if(x<0 || y<0 || x>=size || y>=size) continue;

			// if point is enemy colour
			if(temp.getPoint(x, y)!=EMPTY) {
				// get GoString pointer from temp
				BoardStruct::constStringsIterator gString = temp.getStringConst(x, y);
				// check it's not already in adjStrings
				unique = true;
				for(int i=0;i<adjStrings.size();i++) {
					if(adjStrings[i]==gString) {
						unique = false;
						break;
					}
				}
				// add it if not
				if(unique)
					adjStrings.push_back(gString);
			}
		}
		return;
	}

	// If it's a string return all adjacent strings

	BoardStruct::constStringsIterator target = temp.getStringConst(tX, tY);
	int enemyColour = BLACK;
	if(target->getColour()==BLACK) enemyColour = WHITE;

	// loop through stones in target string
	const GoString::pointsTable& points = target->getPoints();
	GoString::constPointsIterator piter = points.begin();

	for(;piter!=points.end();piter++) {
		// look NSEW
		for(dir = BEGIN_NSEW_DIR;dir<=END_NSEW_DIR;dir++)	{
			if(dir==NORTH) {
				x = piter->x;
				y = piter->y-1;
			}
			else if(dir==SOUTH) {
				x = piter->x;
				y = piter->y+1;
			}
			else if(dir==EAST) {
				x = piter->x+1;
				y = piter->y;
			}
			else if(dir==WEST) {
				x = piter->x-1;
				y = piter->y;
			}
			if(x<0 || y<0 || x>=size || y>=size) continue;

			// if point is enemy colour
			if(temp.getPoint(x, y)==enemyColour) {
				// get GoString pointer from temp
				BoardStruct::constStringsIterator gString = temp.getStringConst(x, y);
				// check it's not already in adjStrings
				unique = true;
				for(int i=0;i<adjStrings.size();i++) {
					if(adjStrings[i]==gString) {
						unique = false;
						break;
					}
				}
				// add it if not
				if(unique)
					adjStrings.push_back(gString);
			}
		}
	}
}

/** @brief This function returns true if the coordinates are adjacent to a
  * DEAD string or the liberty of a DEAD string. */
bool Go::isAdjacentToDeadStringOrLiberty(int mx, int my, const BoardStruct& board) 
{
	vector<BoardStruct::constStringsIterator> adjStrings;
	vector<BoardStruct::constStringsIterator>::const_iterator aiter;
	BoardStruct::constStringsIterator str;
	// North
	if((my-1)>=0) {
		// Is the north point a liberty of a dead string?
		if(board.getPoint(mx, my-1)==EMPTY) {
			// Find adjacent DEAD strings to this empty point
			findAdjacentStrings(mx, my-1, board, adjStrings);
			bool deadStringFound = false;
			for(aiter=adjStrings.begin();aiter!=adjStrings.end();aiter++) {
				if((*aiter)->getStatus()==DEAD)
					return true;
			}
		}
		else if((str=board.getStringConst(mx, my-1))->getStatus()==DEAD)
			return true;
	}
	// South
	if((my+1)<(board.getSize()-1)) {
		if(board.getPoint(mx, my+1)==EMPTY) {
			// Find adjacent DEAD strings to this empty point
			findAdjacentStrings(mx, my+1, board, adjStrings);
			bool deadStringFound = false;
			for(aiter=adjStrings.begin();aiter!=adjStrings.end();aiter++) {
				if((*aiter)->getStatus()==DEAD)
					return true;
			}
		}
		else if((str=board.getStringConst(mx, my+1))->getStatus()==DEAD)
			return true;
	}
	// East
	if((mx+1)<(board.getSize()-1)) {
		if(board.getPoint(mx+1, my)==EMPTY) {
			// Find adjacent DEAD strings to this empty point
			findAdjacentStrings(mx+1, my, board, adjStrings);
			bool deadStringFound = false;
			for(aiter=adjStrings.begin();aiter!=adjStrings.end();aiter++) {
				if((*aiter)->getStatus()==DEAD)
					return true;
			}
		}
		else if((str=board.getStringConst(mx+1, my))->getStatus()==DEAD)
			return true;
	}
	// West
	if((mx-1)>=0) {
		if(board.getPoint(mx-1, my)==EMPTY) {
			// Find adjacent DEAD strings to this empty point
			findAdjacentStrings(mx-1, my, board, adjStrings);
			bool deadStringFound = false;
			for(aiter=adjStrings.begin();aiter!=adjStrings.end();aiter++) {
				if((*aiter)->getStatus()==DEAD)
					return true;
			}
		}
		else if((str=board.getStringConst(mx-1, my))->getStatus()==DEAD)
			return true;
	}
	return false;
}

/** @brief Find if this point is at the edge of the board.
  * @param point The point coordinates.
  * @param boardSize The size of the board.
  * @return True if point is at the edge of the board, false otherwise. */
bool Go::isAdjacentToEdge(const Point<> point, const int boardSize) {
	return (point.x==0 || point.y==0 || point.x==boardSize-1 || point.y==boardSize-1);
}

/** @brief Look at a string and return as soon as we find it has at least 1 
  *  shared liberty with a friendly string. 
  * @param board The board on which the string lies.
  * @param str An iterator to the string on the board we're looking at.
  * @return True if a shared friendly liberty is found, false otherwise. */
bool Go::hasFriendlySharedLiberty(const BoardStruct& board, BoardStruct::constStringsIterator& str)
{
	GoString::constLibertiesIterator liter(str->getLibertyPoints().begin());
	for(;liter!=str->getLibertyPoints().end();liter++) {
		// North
		if((liter->y-1)>=0 && board.getPoint(liter->x, liter->y-1)==str->getColour()
		   && board.getStringConst(liter->x, liter->y-1)!=str)
			return true;
		// South
		if((liter->y+1)<board.getSize() && board.getPoint(liter->x, liter->y+1)==str->getColour()
		   && board.getStringConst(liter->x, liter->y+1)!=str)
			return true;
		// East
		if((liter->x+1)<board.getSize() && board.getPoint(liter->x+1, liter->y)==str->getColour()
		   && board.getStringConst(liter->x+1, liter->y)!=str)
			return true;
		// West
		if((liter->x-1)>=0 && board.getPoint(liter->x-1, liter->y)==str->getColour()
		   && board.getStringConst(liter->x-1, liter->y)!=str)
			return true;
	}
	return false;
}

/** @brief Get a unique list of all the friendly strings that share a liberty with the
  * string pointed to by citer. 
  * @param board The board on which we are searching.
  * @param citer A constant strings iterator, pointing to the target string.
  * @param strings A vector to store iterators to the strings we find share a liberty with
  *  the target string. */
void Go::getFriendlySharedLibertyStrings(const BoardStruct& board, BoardStruct::constStringsIterator& citer, 
									 vector<BoardStruct::constStringsIterator>& strings)
{
	// Cycle through the target strings liberties
	const GoString::libertiesTable libs = citer->getLibertyPoints();
	GoString::constLibertiesIterator liter = libs.begin();
	int targetColour = citer->getColour();
	int boardSize = board.getSize();
	BoardStruct::constStringsIterator newString;
	for(;liter!=libs.end();liter++) {
		// Are there any friendly strings adjacent to this liberty?
		// If it's unique, add it to our list
		// North
		if((liter->y-1)>=0 && board.getPoint(liter->x, liter->y)==targetColour
		   && (newString=board.getStringConst(liter->x, liter->y))!=citer) {
			if(find(strings.begin(), strings.end(), newString)!=strings.end())
				strings.push_back(newString);
		}
		// South
		if((liter->y+1)<boardSize && board.getPoint(liter->x, liter->y+1)==targetColour
		   && (newString=board.getStringConst(liter->x, liter->y+1))!=citer) {
			if(find(strings.begin(), strings.end(), newString)!=strings.end())
				strings.push_back(newString);
		}
		// East
		if((liter->x+1)<boardSize && board.getPoint(liter->x+1, liter->y)==targetColour
		   && (newString=board.getStringConst(liter->x+1, liter->y))!=citer) {
			if(find(strings.begin(), strings.end(), newString)!=strings.end())
				strings.push_back(newString);
		}
		// West
		if((liter->x-1)>=0 && board.getPoint(liter->x-1, liter->y)==targetColour
		   && (newString=board.getStringConst(liter->x-1, liter->y))!=citer) {
			if(find(strings.begin(), strings.end(), newString)!=strings.end())
				strings.push_back(newString);
		}
	}
}
