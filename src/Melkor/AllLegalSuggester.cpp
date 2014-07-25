#include "AllLegalSuggester.h"
#include "Goal.h"
#include "Melkor.h"
#include "Misc.h"
#include "Rect.h"
#include <algorithm>
using namespace std;

void AllLegalSuggester::getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings, 
								const int nextToPlay, const int maximisingColour, 
								BPNGoAdapterInterface& bpn, int maxMoves /*=-1*/) const
{
	moves.clear();

	int size = board.getSize();

	// Start with a bounds that encompasses the whole board
	Rect dBounds(0, 0, size-1, size-1);
	int targetStringX, targetStringY;
	bool targetedGoal = false;
	Goal::GoalListType::const_iterator citer(settings.getGoal().getGoals().begin());
	for(;citer!=settings.getGoal().getGoals().end();citer++) {
		if(citer->goal->testFlags(GoalInterface::TARGETED)) {
			targetStringX = citer->goal->getArg1();
			targetStringY = citer->goal->getArg2();
			targetedGoal = true;
			break;
		}
	}

	// only use dynamic boundaries on targeted problems
	if(targetedGoal) {
		calculateBoundary(dBounds, board, targetStringX, targetStringY);
		expandBoundaryForExtraStrings(dBounds, board);
	}

	// Should we be passing? (Only check if it's _our_ turn to play, otherwise may assume opponent
	//                        will pass when they won't!)
	if(settings.getPassValue()>0.0f && nextToPlay==maximisingColour &&
	   Melkor::NNPassJudge(board, nextToPlay, dBounds, settings.getPassValue(), bpn, maximisingColour)) {
		// Return a pass only
		Move passMove;
		Move::getPassMove(passMove);
		passMove.setNNScore(0.0f);
		moves.push_back(passMove);
		return;
	}

	BoardStruct changeableBoard(board);
#ifdef USE_ILLEGAL_MOVE_MEMORY
	// update the illegal move memory to the current board state
	Go::resetIllegalMoveMemory(changeableBoard, Go::getUseSuperKo());
	Go::getLegalMoves(moves, nextToPlay, changeableBoard, Go::getUseSuperKo(), NULL, true, NULL, maximisingColour);
#else
	Go::getLegalMoves(moves, nextToPlay, changeableBoard, Go::getUseSuperKo(), NULL, false, NULL, maximisingColour);
#endif

	// Always include a pass in games where it is legal
	Move passMove;
	Move::getPassMove(passMove);
	passMove.setNNScore(0.0f);
	moves.push_back(passMove);
}
