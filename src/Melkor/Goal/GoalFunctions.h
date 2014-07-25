#ifndef GOALFUNCTIONS_H
#define GOALFUNCTIONS_H

#pragma warning( disable : 4786 )

#include "BoardStruct.h"

/** @brief A purely virtual class containing function definitions shared between the Meta-Goal class
  * 'Goal' and goal classes that expand GoalInterface. */
class GoalFunctions
{
	public:
		GoalFunctions() {};
		virtual ~GoalFunctions() {};

		virtual void printInfo(string* s = NULL) const = 0;

		virtual bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, 
			const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay) = 0;
		virtual bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, 
			const int targetSize, bool factorInSolutionSize, const int nextToPlay) = 0;

		// for game tree search (minimax) users
		/** @todo What does the return value do? Is it used anywhere? */
		virtual bool evaluate(const int ourColour, float& score, const BoardStruct& b,
			const int nextToPlay, int* bestX = NULL, int* bestY = NULL) = 0;
};

#endif