#include "SearchSettings.h"
#include "GeneralSuggester.h"
#include "StoneCountGoal.h"

#include "LadderExtension.h"

SearchSettings::SearchSettings(const Goal& g) : goal(g)
{
	setMoveSuggester(new GeneralSuggester());
	useIterativeDeepening = true;
	useMTDF = true;
	iterativeStep = 1;
	maximisingColour = BLACK;
	maxDepth = 1;
	useNChildren = 0;
	useNChildrenPercent = 100;
	nextToPlay = BLACK;
	pruneValue = 0.35f;
	passValue = 0.35f;
	useMoveQualityThreshold = false;
	bestMoveFirst = true;
	etc = true;
	useNullMovePruning = false;
	useTranspositionTable = true;
	timeLimit = 0;
}

SearchSettings::SearchSettings(const SearchSettings& s) : goal( new StoneCountGoal() )
{
	// Reallocate search extension memory
	for(int i=0;i<s.extensions.size();i++)
		registerSearchExtension(*s.extensions[i]);
	setMoveSuggester(s.moveSuggester);
	goal = s.goal;
	useIterativeDeepening = s.useIterativeDeepening;
	useMTDF = s.useMTDF;
	iterativeStep = s.iterativeStep;
	maximisingColour = s.maximisingColour;
	maxDepth = s.maxDepth;
	useNChildren = s.useNChildren;
	useNChildrenPercent = s.useNChildrenPercent;
	nextToPlay = s.nextToPlay;
	pruneValue = s.pruneValue;
	passValue = s.passValue;
	useMoveQualityThreshold = s.useMoveQualityThreshold;
	bestMoveFirst = s.bestMoveFirst;
	etc = s.etc;
	useNullMovePruning = s.useNullMovePruning;
	useTranspositionTable = s.useTranspositionTable;
	timeLimit = s.timeLimit;
}

SearchSettings& SearchSettings::operator=(const SearchSettings& s)
{
	if(this!=&s) {
		// Reallocate search extension memory
		clearSearchExtensions();
		for(int i=0;i<s.extensions.size();i++)
			registerSearchExtension(*s.extensions[i]);
		setMoveSuggester(s.moveSuggester);
		goal = s.goal;
		useIterativeDeepening = s.useIterativeDeepening;
		useMTDF = s.useMTDF;
		iterativeStep = s.iterativeStep;
		maximisingColour = s.maximisingColour;
		maxDepth = s.maxDepth;
		useNChildren = s.useNChildren;
		useNChildrenPercent = s.useNChildrenPercent;
		nextToPlay = s.nextToPlay;
		pruneValue = s.pruneValue;
		passValue = s.passValue;
		useMoveQualityThreshold = s.useMoveQualityThreshold;
		bestMoveFirst = s.bestMoveFirst;
		etc = s.etc;
		useNullMovePruning = s.useNullMovePruning;
		useTranspositionTable = s.useTranspositionTable;
		timeLimit = s.timeLimit;
	}
	return *this;
}

SearchSettings::~SearchSettings() 
{
	// Not needed anymore since SmartPtr takes care of deallocation for us
//	if(moveSuggester!=NULL) 
//		delete moveSuggester;
	clearSearchExtensions();
}

// This function must be specified here due to #include conflicts with MoveSuggester
const MoveSuggester& SearchSettings::getMoveSuggester() const
{
	return *moveSuggester;
}

/** @brief This function takes a MoveSuggester pointer and stores it. The SearchSettings object
  * takes ownership of the pointer and responsibility for deleting it. */
void SearchSettings::setMoveSuggester(MoveSuggester* m) 
{
	moveSuggester = m;
}

void SearchSettings::clearSearchExtensions() 
{
	for(int i=0;i<extensions.size();i++)
		delete extensions[i];
	extensions.clear();
}

void SearchSettings::registerSearchExtension(SelectiveSearchExtension& ex)
{
	LadderExtension* lPtr = NULL;

	if(lPtr = dynamic_cast<LadderExtension*>(&ex))
		extensions.push_back(new LadderExtension(ex));
	else {
		char buffer[2000];
		sprintf(buffer, "Bad SelectiveSearchExtension typeid. typeid.name = %s\n", typeid(&ex).name());
		throw buffer;
	}
}
