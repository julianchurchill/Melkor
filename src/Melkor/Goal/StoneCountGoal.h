#ifndef STONECOUNTGOAL_H
#define STONECOUNTGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

class StoneCountGoal : public GoalInterface
{
	public: 
		StoneCountGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::STONECOUNTGOAL, GoalInterface::SOLID, "STONECOUNTGOAL", a, b, c, true) {};
		StoneCountGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~StoneCountGoal() {};

		virtual bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, const BoardStruct& b,
			bool factorInSolutionSize, const int nextToPlay);
		virtual bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
			bool factorInSolutionSize, const int nextToPlay);

		// for game tree search (minimax) users
		virtual bool evaluate(const int ourColour, float& score, const BoardStruct& b,
			const int nextToPlay, int* bestX = NULL, int* bestY = NULL);
		// We don't care what the maximising colour is for minimax
		virtual bool caresAboutMaximisingColour() const { return false; };
		virtual int getRequiredMaximisingColour(const BoardStruct& b, const int nextToPlay) const { return nextToPlay; };
};

#endif