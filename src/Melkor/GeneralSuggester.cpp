#include "GeneralSuggester.h"
#include "Goal.h"
#include "Melkor.h"
#include "Misc.h"
#include "Rect.h"
#include <algorithm>
using namespace std;

void GeneralSuggester::getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings, 
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
	bool useIllegalMoveMemory = false;
#ifdef USE_ILLEGAL_MOVE_MEMORY
	// update the illegal move memory to the current board state
	Go::resetIllegalMoveMemory(changeableBoard, Go::getUseSuperKo());
	useIllegalMoveMemory = true;
#endif
	// only use dynamic boundaries on targeted problems
	if(targetedGoal)
		Melkor::getScoredMoves(moves, changeableBoard, nextToPlay, bpn, false, useIllegalMoveMemory, NULL, &dBounds, true, false, maximisingColour);
	else
		Melkor::getScoredMoves(moves, changeableBoard, nextToPlay, bpn, false, useIllegalMoveMemory, NULL, NULL, true, false, maximisingColour);
	
	// Prune moves using the NN score if required
	vector<Move>::iterator miter(moves.begin());
	for(;miter!=moves.end();miter++) {
		if(settings.getUseMoveQualityThreshold() && miter->getNNScore()<settings.getPruneValue()) {
			moves.erase(miter);
			miter--;
		}
	}
	
	// This sorts highest first, lowest last
	sort<vector<Move>::iterator>(moves.begin(), moves.end(), greater<Move>());
	
	int end = moves.size();
	// if specified select only top n% of moves
	if(settings.getUseNChildrenPercent()>0)
		end = moves.size()-((float)moves.size()*settings.getUseNChildrenPercent()+1);
	// otherwise select only top n moves
	else if(settings.getUseNChildren()>0)
		end = settings.getUseNChildren();
	
	// Remove all moves after end
	if(end<moves.size())
		moves.erase(moves.begin()+end, moves.end());

	/** @todo This is probably a waste - this part was a leftover from the research
	  * to allow a fair comparison between minimax and the Genetic Algorithm. */
	// Promote the best extended liberty move to the front of the ordering
	// Only do this if we have a capture goal to achieve and make sure we have not already achieved it somehow!
	if(targetedGoal && board.getPoint(targetStringX, targetStringY)!=EMPTY) {
		vector<SpecialPoint> exLibs;
		// Returns exLibs sorted high to low
		Go::findLegalExtendedLiberties(exLibs, changeableBoard, nextToPlay, targetStringX, targetStringY);
		if(exLibs.size()>0) {
			// find this move in the list
			int exLibsX = exLibs[0].x;
			int exLibsY = exLibs[0].y;
			for(miter=moves.begin();miter!=moves.end();miter++) {
				if(miter->getX()==exLibsX && miter->getY()==exLibsY)
					break;
			}
			Move tm;
			// if we've found this move in the expanded node set copy and erase it first
			if(miter!=moves.end()) {
				tm = *miter;
				moves.erase(miter);
			}
			else {
				tm.setX(exLibs[0].x);
				tm.setY(exLibs[0].y);
				tm.setColour(nextToPlay);
				tm.setBoardSize(size);
				tm.setNNScore(0.0f);
				tm.setAlphaBetaScore(0);
			}

			// push this ex. liberty move to the front of the moves list
			moves.insert(moves.begin(), tm);
		}
	}

	// Always include a pass in games where it is legal
	Move passMove;
	Move::getPassMove(passMove);
	passMove.setNNScore(0.0f);
	moves.push_back(passMove);
}
