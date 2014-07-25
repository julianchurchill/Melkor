#ifndef SELECTIVESEARCHEXTENSION_H
#define SELECTIVESEARCHEXTENSION_H

#include "BoardStruct.h"
#include "Goal.h"
#include "SearchSettings.h"

//class BoardStruct;
//class Goal;
//class SearchSettings;

/** @note Don't forget to add an entry to SearchSettings::registerSearchExtension() for
  * any new SelectiveSearchExtension derived classes. */
class SelectiveSearchExtension
{
	protected:
		SearchSettings settings;

	public:
		SelectiveSearchExtension(const SearchSettings& s) : settings(s) {};
		SelectiveSearchExtension(const SelectiveSearchExtension& s) : settings(s.settings) {};

		SearchSettings& getSettings() { return settings; };
		void setSettings(const SearchSettings& s) { settings = s; };
		virtual bool validExtension(BoardStruct& board, Goal& goal) = 0;
};

#endif