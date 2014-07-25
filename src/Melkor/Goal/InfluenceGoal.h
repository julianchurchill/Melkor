#ifndef INFLUENCEGOAL_H
#define INFLUENCEGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

class InfluenceGoal : public GoalInterface
{
	public: 
		InfluenceGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::INFLUENCEGOAL, GoalInterface::ABSTRACT, "INFLUENCEGOAL", a, b, c, true) {};
		InfluenceGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~InfluenceGoal() {};

		virtual bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, const BoardStruct& b,
			bool factorInSolutionSize, const int nextToPlay);
		virtual bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
			bool factorInSolutionSize, const int nextToPlay);

		// for game tree search (minimax) users
		virtual bool evaluate(const int ourColour, float& score, const BoardStruct& b,
			const int nextToPlay, int* bestX = NULL, int* bestY = NULL);
		// Influence is calculated from BLACK's perspective, therefore maximising colour
		// must always be BLACK!
		virtual bool caresAboutMaximisingColour() const { return true; };
		virtual int getRequiredMaximisingColour(const BoardStruct& b, const int nextToPlay) const { return BLACK; };
};

#endif