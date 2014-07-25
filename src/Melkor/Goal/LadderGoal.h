#ifndef LADDERGOAL_H
#define LADDERGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

/** @todo Test this goal against CaptureGoal */

/** @brief This goal is an enhanced version of CaptureGoal. It includes
  * eye pattern matching and analysis to better determine string status. */
class LadderGoal : public GoalInterface
{
	public: 
		LadderGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::LADDERGOAL, GoalInterface::SOLID, "LADDERGOAL", a, b, c, false, 
							DEAD, LADDER_ESCAPED, GoalInterface::TERMINAL|GoalInterface::TARGETED) {};
		LadderGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~LadderGoal() {};


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

		float traceLadder(Point<> order3Lib, Point<> order2Lib, const BoardStruct& board);
		void getNextLadderPointDirection(int ladderDX, int ladderDY, int& pointDX, int& pointDY);
		void findInitialLadderPointDirection(Point<> order3Lib, int ladderDX, int ladderDY, int& pointDX, int& pointDY, 
												 const BoardStruct& board);
		void getLadderDirection(const Point<> order3Lib, const Point<> order2Lib, int& ladderDX, int& ladderDY, 
												 const BoardStruct& board);
};

#endif