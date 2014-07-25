#ifndef LADDEREXTENSION_H
#define LADDEREXTENSION_H

#include "SelectiveSearchExtension.h"
#include "BoardStruct.h"
#include "Goal.h"

class LadderExtension : public SelectiveSearchExtension
{
	public:
		LadderExtension(const LadderExtension& s) : SelectiveSearchExtension(s) {};
		LadderExtension(const SelectiveSearchExtension& s) : SelectiveSearchExtension(s) {};
		LadderExtension(const SearchSettings& s) : SelectiveSearchExtension(s) {};

		bool validExtension(BoardStruct& board, Goal& goal) 
		{
			if(goal.testFlags(GoalInterface::TARGETED)) {
				/** @todo Rig this to work for now and print a warning when we
				  * introduce meta-goal functionality. */
				if(goal.getGoals().size()>1)
					throw "Meta-Goals not supported by LadderExtension!";
				else {
					int targetX = goal.getGoals().begin()->goal->getArg1();
					int targetY = goal.getGoals().begin()->goal->getArg2();
					// Return true if target has <=2 liberties
					if(board.getStringConst(targetX, targetY)->getLibertyCount()<=2)
						return true;
				}
			}
			return false;
		}
};

#endif