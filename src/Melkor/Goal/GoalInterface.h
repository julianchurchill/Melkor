#ifndef GOALINTERFACE_H
#define GOALINTERFACE_H

#pragma warning( disable : 4786 )

#include "GoalFunctions.h"
#include "BoardStruct.h"
#include "SmartPtr.h"

using namespace idllib;

/** @brief The main parent class for goal classes, containing functions and function definitions 
  * for classes that represent goals. For example CaptureGoal, InfluenceGoal, LadderGoal, 
  * LifeDeathGoal etc... all extend this class. */
class GoalInterface : public GoalFunctions, public IRefCountImpl<GoalFunctions>
{
	friend class Goal;

	public:
		// When adding a new goal don't forget to update Misc::parseGoalString() and Goal.h
		enum GoalType { NONE, CAPTUREGOAL, LIBERTYCOUNTGOAL, STONECOUNTGOAL, INFLUENCEGOAL, LIBCOUNTCAPTUREGOAL,
						LIFEDEATHGOAL, LADDERGOAL, LOOSETACTICALGOAL };
		// i.e. a solid goal would be capture this stone.
		// an abstract goal would be surround territory
		enum GoalStyle { SOLID, ABSTRACT };

		// Flag values must be distinct powers of 2, i.e. 1,2,4,8,16...
		enum Flags 
		{
			// Goals that are classed as terminal are _completely_ achieved
			// as soon as they are achieved, i.e. CAPTUREGOAL
			// This can be used by search algorithms to stop the search as early as possible
			// TERMINAL goals must define minScore and maxScore.
			TERMINAL = 1,
			// TARGETED goals have a specific x, y, coord for a stone or point the goal is concerned with.
			TARGETED = 2,
		};

	protected:
		int arg1, arg2, arg3;
		bool alwaysSuccessful;
		GoalStyle style;
		GoalType type;

		unsigned int flags;

		// These are used by goals with the BINARY flag
		float maxScore;
		float minScore;

		string name;

	public:
		GoalInterface() : type(NONE), style(SOLID), arg1(0), arg2(0), arg3(0), alwaysSuccessful(false),
			minScore(0.0f), maxScore(0.0f), flags(0), name("NONE") {};
		GoalInterface(GoalType t, GoalStyle s, string goalName, int a=0, int b=0, int c=0, bool as=false, 
			float mMinScore=0.0f, float mMaxScore=0.0f, unsigned int mFlags=0)
			: type(t), style(s), name(goalName), arg1(a), arg2(b), arg3(c), alwaysSuccessful(as),
			  minScore(mMinScore), maxScore(mMaxScore), flags(mFlags) {};
		GoalInterface(const GoalInterface& g)
		{
			type = g.type;
			style = g.style;
			arg1 = g.arg1;
			arg2 = g.arg2;
			arg3 = g.arg3;
			alwaysSuccessful = g.alwaysSuccessful;
			flags = g.flags;
			maxScore = g.maxScore;
			minScore = g.minScore;
			name = g.name;
		};
		virtual ~GoalInterface() {};
		
		/** Check that the score supplied is a terminal score for this goal. */
		bool isTerminalScore(float score) const
		{
			// Terminal scores only work on binary goals
			if(!testFlags(TERMINAL))
				return false;
			if(score<=minScore || score>=maxScore)
				return true;
			return false;
		}

		/** Returns true if the flags specified are active in this goal. */
		bool testFlags(const Flags& f) const { return (f & flags) > 0; };

		void printInfo(string* s = NULL) const
		{
			char buffer[200];
			char typeStr[20];
			char styleStr[20];
			char alwaysSuccessfulStr[20];

			sprintf(typeStr, "%s", getName().c_str());

			if(style==SOLID) sprintf(styleStr, "%s", "SOLID");
			else if(style==ABSTRACT) sprintf(styleStr, "%s", "ABSTRACT");

			if(alwaysSuccessful) sprintf(alwaysSuccessfulStr, "%s", "true");
			else sprintf(alwaysSuccessfulStr, "%s", "false");

			sprintf(buffer, "Type=%s, Style=%s, arg1=%d, arg2=%d, arg3=%d, alwaysSuccessful=%s\n", typeStr, styleStr,
				arg1, arg2, arg3, alwaysSuccessfulStr);
			if(s!=NULL)
				(*s) = buffer;
			else
				LogWriter::printerr(buffer);
		}

		int getArg1() const { return arg1; };
		int getArg2() const { return arg2; };
		int getArg3() const { return arg3; };

		void setArg1(int a) { arg1 = a; };
		void setArg2(int a) { arg2 = a; };
		void setArg3(int a) { arg3 = a; };

		float getMaxScore() const { return maxScore; };
		float getMinScore() const { return minScore; };

		string getName() const { return name; };

		virtual bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, const BoardStruct& b,
			bool factorInSolutionSize, const int nextToPlay) = 0;
		virtual bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
			bool factorInSolutionSize, const int nextToPlay) = 0;

		// for game tree search (minimax) users
		virtual bool evaluate(const int maximisingColour, float& score, const BoardStruct& b,
			const int nextToPlay, int* bestX = NULL, int* bestY = NULL) = 0;

		// Some goals will care about the maximising colour
		// For most it won't matter what the colour is, i.e. LibertyCount (must be consistent through search)
		virtual bool caresAboutMaximisingColour() const = 0;
		// What colour does this goal need as the MAX colour?
		virtual int getRequiredMaximisingColour(const BoardStruct& b, const int nextToPlay) const = 0;

		bool isGoalType(const GoalType& t) const { return (type==t); };
		GoalType getGoalType() const { return type; };
};

#endif