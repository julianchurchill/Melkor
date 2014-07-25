#include "LadderSuggester.h"
#include "Go.h"
#include "Rect.h"
using namespace std;

void LadderSuggester::getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings, 
								const int nextToPlay, const int maximisingColour, 
								BPNGoAdapterInterface& bpn, int maxMoves /*=-1*/) const
{
	moves.clear();

	// This function expands the child node dependent on the colour to play next
	// If nextToPlay == target colour expand to these moves:
	//   1. Liberties of the target.
	//   2. Liberties of adjacent enemy strings with 1 liberty.
	//
	// If nextToPlay == enemy colour:
	//   1. Liberties of the target.

	int anchorX, anchorY;
	bool targetedGoal = false;
	Goal::GoalListType::const_iterator gciter(settings.getGoal().getGoals().begin());
	for(;gciter!=settings.getGoal().getGoals().end();gciter++) {
		if(gciter->goal->testFlags(GoalInterface::TARGETED)) {
			anchorX = gciter->goal->getArg1();
			anchorY = gciter->goal->getArg2();
			targetedGoal = true;
			break;
		}
	}

	if(!targetedGoal) {
		LogWriter::printerr("LadderSuggester::getMoves() function called without a targeted goal!\n");
		return;
	}

	/** @todo Is this bug still current? */
	// Failsafe
	/** @bug We have an unknown bug here. The search does not end when the ladder is captured.
	  * It should end before getting to this expand function. For now, detect if the ladder
	  * has been captured and returned 0 children if so. */
	if(board.getPoint(anchorX, anchorY)==EMPTY)
		return;

	BoardStruct::constStringsIterator citer(board.getStringConst(anchorX, anchorY));

	// Obviously if the target string is in atari and the opposite colour is next to move
	// we can return just a move on that liberty.
	// Must also check the final liberty is not a Ko point...
	BoardStruct changeableBoard(board);
	if(citer->getLibertyCount()==1 && citer->getColour()!=nextToPlay && Go::isLegal(changeableBoard,
	  citer->getLibertyPoints().begin()->x, citer->getLibertyPoints().begin()->y, nextToPlay,
	  Go::getUseSuperKo())) {
		moves.push_back(
			Move(citer->getLibertyPoints().begin()->x, citer->getLibertyPoints().begin()->y, 
			     nextToPlay, board.getSize(), 1.0f)
			);
		return;
	}

	// For ladder colour and enemy add a move for each ladder liberty
	const GoString::libertiesTable& liberties = citer->getLibertyPoints();
	GoString::constLibertiesIterator liter(liberties.begin());
	for(;liter!=liberties.end();liter++) {
		// Check legality
		if(Go::isLegal(changeableBoard, liter->x, liter->y, nextToPlay, Go::getUseSuperKo()))
			moves.push_back(Move(liter->x, liter->y, nextToPlay, board.getSize(), 1.0f));
	}

	// For ladder colour add moves that capture adjacent stones. So 1 liberty strings.
	if(nextToPlay==citer->getColour()) {
		int enemyColour = BLACK;
		if(citer->getColour()==BLACK)
			enemyColour = WHITE;
	
		const GoString::pointsTable& points = citer->getPoints();
		GoString::constPointsIterator piter(points.begin());
		int x, y, dir;
		int size = board.getSize();
		BoardStruct::constStringsIterator str;
		
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
				
				// if point is enemy colour and has 1 liberty...
				if(board.getPoint(x, y)==enemyColour && (str=board.getStringConst(x, y))->getLibertyCount()==1) {
					const GoString::libertiesTable& libs = str->getLibertyPoints();
					if(Go::isLegal(changeableBoard, libs.begin()->x, libs.begin()->y, nextToPlay, Go::getUseSuperKo()))
						moves.push_back(Move(libs.begin()->x, libs.begin()->y, nextToPlay, board.getSize(), 1.0f));
				} // end if enemy colour && libs==1
			} // end for dir
		} // end for points
	} // end if ladder colour next to play

	// Always include a pass in games where it is legal
	Move passMove;
	Move::getPassMove(passMove);
	passMove.setNNScore(0.0f);
	moves.push_back(passMove);
}
