#ifndef LIBERTYGOAL_H
#define LIBERTYGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

class LibertyCountGoal : public GoalInterface
{
	public: 
		LibertyCountGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::LIBERTYCOUNTGOAL, GoalInterface::SOLID, "LIBERTYCOUNTGOAL", a, b, c, true) {};
		LibertyCountGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~LibertyCountGoal() {};

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