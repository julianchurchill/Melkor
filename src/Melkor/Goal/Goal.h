#ifndef GOAL_H
#define GOAL_H

#pragma warning( disable : 4786 )

#include "GoalFunctions.h"
#include "GoalInterface.h"
#include "BoardStruct.h"

using namespace idllib;

class Goal : public GoalFunctions
{
	private:
		struct GoalEntry {
			SmartPtr<GoalInterface> goal;
			float weight;
			GoalEntry(GoalInterface* g, float w) : goal(g), weight(w) {};
			GoalEntry(const GoalEntry& ge)
			{
				goal = ge.goal;
				weight = ge.weight;
			}
		};

	public:
		typedef vector<GoalEntry> GoalListType;

	private:
		GoalListType goals;
		int numberOfCalls;

	public:
		/** @brief Initialise this multi-goal container with the pre-allocated object initialGoal.
		  *   Goal takes responsibility for the GoalInterface pointer and its deallocation.
		  * @param initialGoal A pointer to a pre-allocated GoalInterface derived object which
		  *   will be deleted by Goal when necessary. 
		  * @param weight An optional value to assign a weight to the initial goal compared to 
		  *   other goals resident in this multi-goal container. */
		Goal(GoalInterface* initialGoal, float weight = 1.0f)	: numberOfCalls(0)
		{
			addGoal(initialGoal, weight);
		}
		Goal(const Goal& g)
		{
//			for(int i=0;i<g.goals.size();i++)
//				addGoal(g.goals[i].goal, g.goals[i].weight);
			goals = g.goals;
			numberOfCalls = g.numberOfCalls;
		}
		virtual ~Goal()
		{
			 clearGoals();
		}

		Goal& operator=(const Goal &g)
		{
			if(this!=&g)
			{
				// delete original goals
				clearGoals();
				// now copy the new ones
//				for(int i=0;i<g.goals.size();i++)
//					addGoal(g.goals[i].goal, g.goals[i].weight);
				goals = g.goals;
				numberOfCalls = g.numberOfCalls;
			}
			return *this;
		}

		void printInfo(string* s = NULL) const
		{
			char buffer[50];
			if(s!=NULL)
				(*s) = "";
			for(int i=0;i<goals.size();i++) {
				sprintf(buffer, "Goal %d: ", i);
				if(s!=NULL) {
					(*s)+=buffer;
					string s1;
					goals[i].goal->printInfo(&s1);
					(*s)+=s1;
				}
				else {
					LogWriter::printerr(buffer);
					goals[i].goal->printInfo();
				}
			}
		}

		/** @brief Add a pre-allocated goal to this multi-goal container with an associated weight.
		  *   Goal takes responsibility for the GoalInterface pointer and its deallocation.
		  * @param g A pointer to a pre-allocated GoalInterface derived object which
		  *   will be deleted by Goal when necessary. 
		  * @param weight An optional value to assign a weight to the goal compared to 
		  *   other goals resident in this multi-goal container. */
		bool addGoal(GoalInterface* g, float weight = 1.0f)
		{
			if( g != NULL )
				goals.push_back(GoalEntry(g, weight));
			return true;
		}

		static string getGoalStringParameterSyntax()
		{
			return "g(x,y) where g can be one of C=capture, L=libertycount, S=stonecount, I=influence, X=libcountCapture";
		}

		void clearGoals()
		{
			// Not needed because SmartPtr destructor is called by vector<>.clear()
			// and that deletes the pointer it represents!
//			for(int i=0;i<goals.size();i++)
//				delete (goals[i].goal);
#ifdef _DEBUG
			_ASSERTE( _CrtCheckMemory() );
			if( goals.size() > 0 )
				_ASSERTE( _CrtIsValidHeapPointer( goals.begin()->goal ) );
#endif

			goals.clear();
		}

		bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, const BoardStruct& b,
			bool factorInSolutionSize, const int nextToPlay)
		{
			numberOfCalls++;
			float tempScore = 0.0f;
			score = 0.0f;
			bool returnValue = true;
			for(int i=0;i<goals.size();i++) {
				if(!goals[i].goal->successWithBoardBeforeMoves(target, ourColour, tempScore, b, factorInSolutionSize,
				   nextToPlay))
					returnValue = false;
				score+=tempScore*goals[i].weight;
			}
			// now make sure it averages out over the goals,
			// so we don't get more goals giving higher scores...
			if(goals.size()!=0)
				score = score/goals.size();
			// it only takes one partial goal to fail for us to return false
			return returnValue;
		}
		
		bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
			bool factorInSolutionSize, const int nextToPlay)
		{
			numberOfCalls++;
			float tempScore = 0.0f;
			score = 0.0f;
			bool returnValue = true;
			for(int i=0;i<goals.size();i++) {
				if(!goals[i].goal->successWithBoardAfterMoves(ourColour, tempScore, b, targetSize, factorInSolutionSize, 
				   nextToPlay))
					returnValue = false;
				score+=tempScore*goals[i].weight;
			}
			// now make sure it averages out over the goals,
			// so we don't get more goals giving higher scores...
			if(goals.size()!=0)
				score = score/goals.size();
			// it only takes one partial goal to fail for us to return false
			return returnValue;
		}

		// for minimax users
		bool evaluate(const int ourColour, float& score, const BoardStruct& b, const int nextToPlay,
			int* bestX = NULL, int* bestY = NULL)
		{
			numberOfCalls++;
			float tempScore = 0.0f;
			score = 0.0f;
			bool returnValue = true;
			for(int i=0;i<goals.size();i++) {
				if(!goals[i].goal->evaluate(ourColour, tempScore, b, nextToPlay, bestX, bestY))
					returnValue = false;
				score+=tempScore*goals[i].weight;
			}
			// now make sure it averages out over the goals,
			// so we don't get more goals giving higher scores...
			if(goals.size()!=0)
				score = score/goals.size();
			return returnValue;
		}

		/** Go through the goals and ask if each requires a particular colour to be
		the max colour for minimax and work out conflicts. 
		@param b BoardStruct on which the search is to take place.
		@param nextToPlay The colour to play next.
		@return The required maximising colour, nextToPlay if no required colour. */
		int getRequiredMaximisingColour(const BoardStruct& b, const int nextToPlay) const 
		{
			int requiredColour = EMPTY;
			for(int i=0;i<goals.size();i++) {
				if(goals[i].goal->caresAboutMaximisingColour()) {
					int newRequiredColour = goals[i].goal->getRequiredMaximisingColour(b, nextToPlay);
					if(requiredColour!=EMPTY && requiredColour!=newRequiredColour) {
						// handle clashes
						throw "Two or more goals in multiple goal object require different colours to be the maximum colour for minimax!";
					}
					else
						requiredColour = newRequiredColour;
				}
			}

			// if no required colour return next to play
			if(requiredColour==EMPTY)
				requiredColour = nextToPlay;
			return requiredColour;
		}

		// copied from GoalInterface to cover multiple goals
		// must check all goals in list
		bool isGoalType(const GoalInterface::GoalType& t) const
		{
			for(int i=0;i<goals.size();i++) {
				if(goals[i].goal->type==t)
					return true;
			}
			return false;
		}

		/** Check that the score supplied is a terminal score for all goals. */
		bool isTerminalScore(float score) const
		{
			for(int i=0;i<goals.size();i++) {
				if(!goals[i].goal->isTerminalScore(score))
					return false;
			}
			return true;
		}

		// copied from GoalInterface to cover multiple goals
		bool testFlags(const GoalInterface::Flags& f) const 
		{
			GoalListType::const_iterator citer(goals.begin());
			for(;citer!=goals.end();citer++) {
				if(citer->goal->testFlags(f))
					return true;
			}
			return false;
		}

		int getNumberOfCalls() const { return numberOfCalls; };
		void setNumberOfCalls(int n) { numberOfCalls = n; };

		// return true if all partial goals are always successful, false otherwise
		bool getAlwaysSuccessful()
		{
			for(int i=0;i<goals.size();i++) {
				if(!goals[i].goal->alwaysSuccessful)
					return false;
			}
			return true;
		}

		const GoalListType& getGoals() const { return goals; };
		GoalListType getGoals(const GoalInterface::Flags& f) const 
		{ 
			GoalListType goalList;
			GoalListType::const_iterator citer(goals.begin());
			for(;citer!=goals.end();citer++) {
				if(citer->goal->testFlags(f))
					goalList.push_back(*citer);
			}
			return goalList; 
		}

		/** Return a list of goals which have the specified flags enabled. */
/*		bool getGoals(GoalListType& goalList, const GoalInterface::Flags& f) const
		{
			GoalListType::const_iterator citer(goals.begin());
			for(;citer!=goals.end();citer++) {
				if(citer->goal->testFlags(f))
					goalList.push_back(*citer);
			}
			return (goalList.size()>0);
		}*/
};

#endif