#ifndef SEARCHALGORITHM_H
#define SEARCHALGORITHM_H

#include "Move.h"
#include "Goal.h"
#include <vector>
using namespace std;

// Abstract interface
class SearchAlgorithm
{
	public:
		virtual void getBestLineOfPlay(vector<Move>& l) const = 0;
		virtual string printSearchSettings() const = 0;
		virtual void setGoal(const Goal& g) = 0;
		virtual void saveSettings() = 0;
		virtual void restoreSettings() = 0;
};

#endif