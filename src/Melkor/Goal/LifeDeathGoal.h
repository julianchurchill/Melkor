#ifndef LIFEDEATHGOAL_H
#define LIFEDEATHGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

/** @todo Test this goal against CaptureGoal */

/** @brief This goal is an enhanced version of CaptureGoal. It includes
  * eye pattern matching and analysis to better determine string status. */
class LifeDeathGoal : public GoalInterface
{
	public: 
		LifeDeathGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::LIFEDEATHGOAL, GoalInterface::SOLID, "LIFEDEATHGOAL", a, b, c, false, 
							DEAD, ALIVE, GoalInterface::TERMINAL|GoalInterface::TARGETED) {};
		LifeDeathGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~LifeDeathGoal() {};


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