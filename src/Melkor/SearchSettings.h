#ifndef SEARCH_SETTINGS_H
#define SEARCH_SETTINGS_H

#include "Goal.h"
#include "SmartPtr.h"
//#include "MoveSuggester.h"
//#include "SelectiveSearchExtension.h"

using namespace idllib;

class MoveSuggester;
class SelectiveSearchExtension;

class SearchSettings
{
	public:
		typedef vector<SelectiveSearchExtension*> SearchExtensionsList;

	private:

		SearchExtensionsList extensions;

		SmartPtr<MoveSuggester> moveSuggester;
		Goal goal;

		bool useIterativeDeepening, useMTDF;
		int iterativeStep;

		int maximisingColour;

		int maxDepth, useNChildren, nextToPlay;
		float useNChildrenPercent, pruneValue, passValue;
		bool useMoveQualityThreshold, bestMoveFirst, etc, useNullMovePruning, useTranspositionTable;
		double timeLimit;

	public:

		SearchSettings(const Goal& g);
		SearchSettings(const SearchSettings& s);
		SearchSettings& operator=(const SearchSettings& s);
		~SearchSettings();

		inline SearchExtensionsList& getSearchExtensions() { return extensions; }; 
		inline const SearchExtensionsList& getSearchExtensions() const { return extensions; }; 
		void clearSearchExtensions();
		void registerSearchExtension(SelectiveSearchExtension& ex);

		const MoveSuggester& getMoveSuggester() const;
		void setMoveSuggester(MoveSuggester* m);

		inline const Goal& getGoal() const { return goal; }; 
		inline Goal& getGoal() { return goal; }; 
		inline void setGoal(const Goal& g) { goal = g; };

		inline bool getUseIterativeDeepening() const { return useIterativeDeepening; };
		inline void setUseIterativeDeepening(bool b) { useIterativeDeepening = b; };
		inline bool getUseMTDF() const { return useMTDF; };
		inline void setUseMTDF(bool b) { useMTDF = b; };
		inline int getIterativeStep() const { return iterativeStep; };
		inline void setIterativeStep(int i) { iterativeStep = i; };

		inline int getMaximisingColour() const { return maximisingColour; };
		inline void setMaximisingColour(int d) { maximisingColour = d; };

		inline int getDepth() const { return maxDepth; };
		inline void setDepth(int d) { maxDepth = d; };
		inline int getUseNChildren() const { return useNChildren; };
		inline void setUseNChildren(int d) { useNChildren = d; };
		inline int getNextToPlay() const { return nextToPlay; };
		inline void setNextToPlay(int d) { nextToPlay = d; };
		inline float getUseNChildrenPercent() const { return useNChildrenPercent; };
		inline void setUseNChildrenPercent(float d) { useNChildrenPercent = d; };
		inline float getPruneValue() const { return pruneValue; };
		inline void setPruneValue(float d) { pruneValue = d; };
		inline float getPassValue() const { return passValue; };
		inline void setPassValue(float d) { passValue = d; };
		inline bool getUseMoveQualityThreshold() const { return useMoveQualityThreshold; };
		inline void setUseMoveQualityThreshold(bool d) { useMoveQualityThreshold = d; };
		inline bool getUseBestMoveFirst() const { return bestMoveFirst; };
		inline void setUseBestMoveFirst(bool d) { bestMoveFirst = d; };
		inline bool getUseEtc() const { return etc; };
		inline void setUseEtc(bool d) { etc = d; };
		inline bool getUseNullMovePruning() const { return useNullMovePruning; };
		inline void setUseNullMovePruning(bool d) { useNullMovePruning = d; };
		inline bool getUseTranspositionTable() const { return useTranspositionTable; };
		inline void setUseTranspositionTable(bool d) { useTranspositionTable = d; };
		inline double getTimeLimit() const { return timeLimit; };
		inline void setTimeLimit(double d) { timeLimit = d; };
};

#endif