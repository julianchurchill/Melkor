#include "CaptureSuggester.h"
#include "Go.h"
#include "Goal.h"
#include "Melkor.h"
#include "Misc.h"
#include "Rect.h"
#include <algorithm>
using namespace std;

void CaptureSuggester::getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings, 
								const int nextToPlay, const int maximisingColour, 
								BPNGoAdapterInterface& bpn, int maxMoves /*=-1*/) const
{
	moves.clear();

	int targetStringX, targetStringY;
	bool targetGoal = false;
	Goal::GoalListType::const_iterator gciter(settings.getGoal().getGoals().begin());
	for(;gciter!=settings.getGoal().getGoals().end();gciter++) {
		if(gciter->goal->testFlags(GoalInterface::TARGETED)) {
			targetStringX = gciter->goal->getArg1();
			targetStringY = gciter->goal->getArg2();
			targetGoal = true;
			break;
		}
	}

	if(!targetGoal) {
		LogWriter::printerr("CaptureSuggester::getMoves() function called without a targetted goal!\n");
		return;
	}

	BoardStruct changeableBoard(board);

	// Obviously if the target string is in atari and the opposite colour is next to move
	// we can return just a move on that liberty.
	// Must also check the final liberty is not a Ko point...
	BoardStruct::constStringsIterator target(board.getStringConst(targetStringX, targetStringY));
	if(target->getLibertyCount()==1 && target->getColour()!=nextToPlay && Go::isLegal(changeableBoard,
	  target->getLibertyPoints().begin()->x, target->getLibertyPoints().begin()->y, nextToPlay,
	  Go::getUseSuperKo())) {
		moves.push_back(
			Move(target->getLibertyPoints().begin()->x, target->getLibertyPoints().begin()->y, nextToPlay,
			  board.getSize(), 1.0f)
			);
		return;
	}

	// Generate a boundary around the target
	// Start with a bounds that encompasses the whole board
	Rect dBounds(0, 0, board.getSize()-1, board.getSize()-1);
	// calculate the boundary
	calculateBoundary(dBounds, board, targetStringX, targetStringY);
	// Note: Bounds is only used to choose NN moves so keep it tight by _not_
	//  expanding the bounds to incorporate surrounding strings.
//	expandBoundaryForExtraStrings(dBounds, board);

	// Allow the filling of our own eyes for capture problems
	bool oldDontFillOwnEyes = Go::dontFillOwnEyes;
	Go::dontFillOwnEyes = false;

	// Expand to all exLib moves ordering according to score
	vector<SpecialPoint> liberties;
	// This returns highest first, so add to moves in order
	Go::findLegalExtendedLiberties(liberties, changeableBoard, nextToPlay, targetStringX, targetStringY);

	vector<SpecialPoint>::const_iterator citer(liberties.begin());
	for(;citer!=liberties.end();citer++)
		moves.push_back( Move(citer->x, citer->y, nextToPlay, board.getSize(), citer->extra) );

	/** @todo Should also use a few tesuji/shape moves that look good for the enemy as well as us... */

	// Toss in a few tesuji/shape moves from the NN
	vector<Move> shapeMoves;
	Melkor::getScoredMoves(shapeMoves, changeableBoard, nextToPlay, bpn, false, false, NULL, 
		&dBounds, true, false, maximisingColour);
	// Does this sort highest or lowest first???
	// It sorts lowest first, but it doesn't matter because we
	// go through moves backwards later to pick our NN moves out
	sort(shapeMoves.begin(), shapeMoves.end());

	int count = 0;
	int maxNNMoves = 3;
	bool skip = false;
	int i = shapeMoves.size()-1;
	for(;i>=0;i--) {
		Move& m = shapeMoves[i];
		// Check NN move isn't already in the moves list
		vector<Move>::const_iterator mciter(moves.begin());
		for(;mciter!=moves.end();mciter++) {
			if(mciter->getX()==m.getX() && mciter->getY()==m.getY()) {
				skip = true;
				break;
			}
		}
		if(skip) {
			skip = false;
			continue;
		}
		count++;
		moves.push_back(m);
	}

	Go::dontFillOwnEyes = oldDontFillOwnEyes;

	// Always include a pass in games where it is legal
	Move passMove;
	Move::getPassMove(passMove);
	passMove.setNNScore(0.0f);
	moves.push_back(passMove);
}
