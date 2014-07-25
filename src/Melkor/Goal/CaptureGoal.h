#ifndef CAPTUREGOAL_H
#define CAPTUREGOAL_H

#pragma warning( disable : 4786 )

#include "GoalInterface.h"
#include "globals.h"

class CaptureGoal : public GoalInterface
{
	public: 
		CaptureGoal(int a=0, int b=0, int c=0)
			: GoalInterface(GoalInterface::CAPTUREGOAL, GoalInterface::SOLID, "CAPTUREGOAL", a, b, c, false, 
							DEAD, ALIVE, GoalInterface::TERMINAL|GoalInterface::TARGETED) {};
		CaptureGoal(const GoalInterface& g) : GoalInterface(g) {};
		virtual ~CaptureGoal() {};


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