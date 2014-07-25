#ifndef LOOSETACTICALGOAL_H
#define LOOSETACTICALGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

/** @brief This goal is designed to allow status search on medium complexity strings,
  * i.e. ones that aren't ladders! */
class LooseTacticalGoal : public GoalInterface
{
	public: 
		LooseTacticalGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::LOOSETACTICALGOAL, GoalInterface::SOLID, "LOOSETACTICALGOAL", a, b, c, false, 
							DEAD, ALIVE, GoalInterface::TERMINAL|GoalInterface::TARGETED) {};
		LooseTacticalGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~LooseTacticalGoal() {};


		virtual bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, const BoardStruct& b,
			bool factorInSolutionSize, const int nextToPlay);
		virtual bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
			bool factorInSolutionSize, const int nextToPlay);

		// for game tree search (minimax) users
		virtual bool evaluate(const int maximisingColour, float& score, const BoardStruct& b,
			const int nextToPlay, int* bestX = NULL, int* bestY = NULL);
		// Capture goal must have the maximising colour in a minimax tree as the target colour
		// since ALIVE is +v.e. and DEAD is -v.e. in globals.h
		virtual bool caresAboutMaximisingColour() const { return true; };
		virtual int getRequiredMaximisingColour(const BoardStruct& b, const int nextToPlay) const { return b.getPoint(arg1, arg2); };
};

#endif