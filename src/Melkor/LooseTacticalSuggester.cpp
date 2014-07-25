#include "Go.h"
#include "LooseTacticalSuggester.h"
#include "Rect.h"
using namespace std;

void LooseTacticalSuggester::getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings, 
								const int nextToPlay, const int maximisingColour, 
								BPNGoAdapterInterface& bpn, int maxMoves /*=-1*/) const
{
	moves.clear();

	// This function expands the child node dependent on the colour to play next
	// If nextToPlay == target colour expand to these moves:
	//   1. Liberties of the target.
	//   2. Liberties of adjacent enemy strings.
	//
	// If nextToPlay == enemy colour:
	//   1. Liberties of the target.
	//   2. Liberties of target colour strings that have a shared liberty with the target.

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
		LogWriter::printerr("LooseTacticalSuggester::getMoves() function called without a targeted goal!\n");
		return;
	}

	/** @todo Is this bug still current? */
	// Failsafe
	/** @bug We have an unknown bug here. The search does not end when the ladder is captured.
	  * It should end before getting to this expand function. For now, detect if the ladder
	  * has been captured and returned 0 children if so. */
	if(board.getPoint(anchorX, anchorY)==EMPTY)
		return;

	/** @todo Order the liberties by second order value. I.e. how many liberties does
	  * this liberty have itself? Make sure this extra calculation is worth it before implementing */

	BoardStruct::constStringsIterator citer(board.getStringConst(anchorX, anchorY));

	// Obviously if the target string is in atari and the opposite colour is next to move
	// we can return just a move on that liberty.
	// Must also check the final liberty is not a Ko point...
	BoardStruct changeableBoard(board);
	if(citer->getLibertyCount()==1 && citer->getColour()!=nextToPlay && Go::isLegal(changeableBoard,
	  citer->getLibertyPoints().begin()->x, citer->getLibertyPoints().begin()->y, nextToPlay,
	  Go::getUseSuperKo())) {
		moves.push_back(Move(citer->getLibertyPoints().begin()->x, citer->getLibertyPoints().begin()->y, 
			nextToPlay, board.getSize(), 1.0f));
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

	/** @todo Change this bit to use Misc::findAdjacentStrings() */

	// For ladder colour add moves on liberties of adjacent enemy strings
	if(nextToPlay==citer->getColour()) {
		int enemyColour = BLACK;
		if(citer->getColour()==BLACK)
			enemyColour = WHITE;
		
		const GoString::pointsTable& points = citer->getPoints();
		GoString::constPointsIterator piter(points.begin());
		int x, y, dir;
		int size = board.getSize();
		bool unique;
		vector<BoardStruct::constStringsIterator> processedStrings;
		vector<BoardStruct::constStringsIterator>::const_iterator psiter;
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
				
				// if point is enemy colour add all it's liberties as potential moves
				if(board.getPoint(x, y)==enemyColour && (str=board.getStringConst(x, y))!=NULL) {
					// Check we haven't gathered this strings liberties already!
					psiter = processedStrings.begin();
					unique = true;
					for(;psiter!=processedStrings.end();psiter++) {
						if((*str)==(*(*psiter))) {
							unique = false;
							break;
						}
					}
					if(unique) {
						const GoString::libertiesTable& libs = str->getLibertyPoints();
						GoString::constLibertiesIterator liter(libs.begin());
						for(;liter!=libs.end();liter++) {
							// Check legality
							if(Go::isLegal(changeableBoard, liter->x, liter->y, nextToPlay, Go::getUseSuperKo()))
								moves.push_back(Move(liter->x, liter->y, nextToPlay, board.getSize(), 1.0f));
						}
						processedStrings.push_back(str);
					}
				} // end if stone == enemyColour 
			} // end for dir
		} // end for points in target
	} // end if target colour next to play
	// For enemy colour add moves which play on:
	//   2. Liberties of target colour strings that have a shared liberty with the target.
	else if(Go::hasFriendlySharedLiberty(board, citer)) {
		// Find strings which have at least 1 shared liberty with the target
		vector<BoardStruct::constStringsIterator> strings;
		Go::getFriendlySharedLibertyStrings(board, citer, strings);

		for(int i=0;i<strings.size();i++) {
			const GoString::libertiesTable& libs = strings[i]->getLibertyPoints();
			GoString::constLibertiesIterator liter(libs.begin());
			for(;liter!=libs.end();liter++) {
				if(Go::isLegal(changeableBoard, liter->x, liter->y, nextToPlay, Go::getUseSuperKo()))
					moves.push_back(Move(liter->x, liter->y, nextToPlay, board.getSize(), 1.0f));
			}
		}
	}

	// Always include a pass in games where it is legal
	Move passMove;
	Move::getPassMove(passMove);
	passMove.setNNScore(0.0f);
	moves.push_back(passMove);
}
