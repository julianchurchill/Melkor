#ifndef MTDFSEARCH_H
#define MTDFSEARCH_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "BoardStruct.h"
#include "Node.h"
#include "BPNGoAdapter.h"
#include "SearchAlgorithm.h"
#include "Rect.h"
#include "hash_set.h"
#include "Goal.h"
#include "SearchSettings.h"
#include "SelectiveSearchExtension.h"
#include <vector>
#include <stack>
#include <string>
using namespace std;

#ifndef _DEBUG
/** Define this to make the code as fast as possible. */
#define MTDFSEARCH_LEAN_AND_MEAN
#endif

/** A class that implements Aske Plaat's MTD(f) algorithm. It is a
minimax search with alpha-beta pruning and a few other enhancements. */
class MTDFSearch : public SearchAlgorithm
{
	private:

		static const string name;

		/** @todo This variable is used to call:
		  * melkor->verbose once, melkor->getBPN() once.
		  * We should remove this variable and change the function calls to function parameters. */
		void* melkor;

		bool debugFlag;

		// for statistics
		int nodesTraversed;
		int nodesEvaluated;
		/** The depth reached by the last call to iterativeDeepening. */
		int lastIDDepth;
		/** This flag allows any part of the search to check if there's still time left. */
		bool timeRanOut;

		// generated after each search
		vector<Move> bestLineOfPlay;

		// search options
		SearchSettings settings;
		stack<SearchSettings> savedSettings;

		/** @brief A vector of boardStructs to be used by alpha-beta for saving and restoring boards
		  * when adding moves. This saves us allocating new boards at each alpha-beta call.
		  * e.g. oldBoard[currentDepth] = board; makeMove(); alpha-beta(); board = oldBoard[currentDepth]; */
		vector<BoardStruct> oldBoard;

		// TRANSPOSITION_TABLE is defined in globals.h
		/** @todo Why is this type-defined in a global header file? */
		TRANSPOSITION_TABLE* transpositionTable;

		// Private for debugging
		Node* lookupPtr(const Node& findMe);
	
	public:

		static const float defaultPruneValue;
		static const float defaultPassValue;
		static const bool defaultUseMoveQualityThreshold;
		static const float defaultUseNChildrenPercent;
		static const int defaultUseNChildren;
		static const int defaultMaxDepth;
		static const int defaultNextToPlay;
		static const int defaultMaxStatusDepth;

		static const float POSINFINITY;
		static const float NEGINFINITY;

		MTDFSearch(bool debugFlag, void* m, bool useMelkorType);
		virtual ~MTDFSearch() {	delete transpositionTable; };

		static const string getName() { return name; };
		inline void setMoveSuggester(MoveSuggester* m) { settings.setMoveSuggester(m); };
		inline void setMaximisingColour(int d) { settings.setMaximisingColour(d); };
		inline int getMaximisingColour() const { return settings.getMaximisingColour(); };
		inline void setUseIterativeDeepening(bool t) { settings.setUseIterativeDeepening(t); };
		inline bool getUseIterativeDeepening() const { return settings.getUseIterativeDeepening(); };
		inline void setUseMTDF(bool t) { settings.setUseMTDF(t); };
		inline bool getUseMTDF() const { return settings.getUseMTDF(); };
		inline void setUseEtc(bool t) { settings.setUseEtc(t); };
		inline bool getUseEtc() const { return settings.getUseEtc(); };
		inline void setUseNullMovePruning(bool t) { settings.setUseNullMovePruning(t); };
		inline bool getUseNullMovePruning() const { return settings.getUseNullMovePruning(); };
		inline void setUseBestMoveFirst(bool t) { settings.setUseBestMoveFirst(t); };
		inline bool getUseBestMoveFirst() const { return settings.getUseBestMoveFirst(); };
		inline void setUseTranspositionTable(bool t) { settings.setUseTranspositionTable(t); };
		inline bool getUseTranspositionTable() const { return settings.getUseTranspositionTable(); };
		inline void setPruneValue(float p) { settings.setPruneValue(p); };
		inline float getPruneValue() const { return settings.getPruneValue(); };
		inline void setPassValue(float p) { settings.setPassValue(p); };
		inline float getPassValue() const { return settings.getPassValue(); };
		inline void setUseMoveQualityThreshold(bool b) { settings.setUseMoveQualityThreshold(b); };
		inline bool getUseMoveQualityThreshold() const { return settings.getUseMoveQualityThreshold(); };
		inline void setUseNChildrenPercent(float p) { settings.setUseNChildrenPercent(p); };
		inline float getUseNChildrenPercent() const { return settings.getUseNChildrenPercent(); };
		inline void setUseNChildren(int d) { settings.setUseNChildren(d); };
		inline int getUseNChildren() const { return settings.getUseNChildren(); };
		inline void setDepth(int d) { settings.setDepth(d); };
		inline int getDepth() const { return settings.getDepth(); };
		inline void setNextToPlay(int b) { settings.setNextToPlay(b); };
		inline int getNextToPlay() const { return settings.getNextToPlay(); };
		inline void setTimeLimit(double t) { settings.setTimeLimit(t); timeRanOut = false; };
		inline double getTimeLimit() const { return settings.getTimeLimit(); };
		inline bool getTimeRanOut() const { return timeRanOut; };
		inline int getNodesTraversed() const { return nodesTraversed; };
		inline int getNodesEvaluated() const { return nodesEvaluated; };
		inline int getLastIDDepth() const { return lastIDDepth; };
		inline void setDebugFlag(const bool df) { debugFlag = df; };
		inline bool getDebugFlag() const { return debugFlag; };

		// From SearchAlgorithm interface
		inline void setGoal(const Goal& g) { settings.setGoal(g); };
		inline const Goal& getGoal() const { return settings.getGoal(); };
		inline Goal& getGoal() { return settings.getGoal(); };

		inline const SearchSettings& getSettings() { return settings; };

		void clearSearchExtensions() { settings.clearSearchExtensions(); };
		void registerSearchExtension(SelectiveSearchExtension& ex) 
		{
			settings.registerSearchExtension(ex);
		}

		// From SearchAlgorithm interface
		inline void getBestLineOfPlay(vector<Move>& l) const { l = bestLineOfPlay; };
		inline const vector<Move>& getBestLineOfPlay() const { return bestLineOfPlay; };

		void init();

		/** @todo Is this function necessary/used at all? */
		float suggestMove(Move &m, const BoardStruct &b, const int nextToPlay);
		void cleanTranspositionTable();

		float doLocalSearch(const BoardStruct& board, Move &max, Node* root, const int maxDepth, const int maximisingColour);
		float runSearchAsSettings(Node* root, const BoardStruct& board, const float firstGuess = NOSTATUS,
								  time_t* start = NULL, const int parentDepth = 0);

		float iterativeDeepening(const BoardStruct& board, Node* root, const int maxDepth, const int maximisingColour,
			const int step = 1, const float initialFirstGuess = NOSTATUS, const int parentDepth = 0);

		float MTDF(const BoardStruct& board, Node* root, const float f, const int d, const int maximisingColour, 
					time_t* start, const bool testForTerminalNodes, const int parentDepth = 0);

		// Non NegaMax version
		float AlphaBetaWithMemory(BoardStruct& board, Node* n, float alpha, float beta, const int depthToSearch, const int currentDepth,
					const int maximisingColour, const bool rootLevel, time_t* start, const bool testForTerminalNodes);
		// NegaMax version, WARNING: This function is unstable and untested.
		float NegaMaxAlphaBetaWithMemory(BoardStruct& board, Node* n, float alpha, float beta, const int depthToSearch, const int currentDepth,
					const int maximisingColour, const bool rootLevel, time_t* start, const bool testForTerminalNodes);

		inline void makeMove(BoardStruct& board, Node& node)
		{
			board.addToBoard(node.getMove());
//			if(node.getNextToPlay()==BLACK)
//				board.addToBoard(node.getX(), node.getY(), WHITE);
//			else
//				board.addToBoard(node.getX(), node.getY(), BLACK);
			// Associate this node with this board position, i.e. the position
			// after the move in Node has been played
			node.setKeys(board);
		}

		void expand(vector<Node>& nodes, const Node& n, const BoardStruct& board, const int maximisingColour);
		Node* updateTTable(const Node& lookupNode, const Node& bestNode);
	
	protected:
		enum BoundType { UNSET, NO_CUTOFF, EXACT_VALUE, LOWER_BOUND, UPPER_BOUND };
		bool lookupTranspositionTableBounds(float& boundValue, BoundType& boundType, Node* n,
			const int currentDepth, const int depthToSearch, float& alpha, float& beta);

	public:
		static Node getChildNode(const Node& parent, const Move& move);

		static void outputNodeDetails(const Node& n, const int maximisingColour, const int depth, const int boardSize,
			const float alpha, const float beta);

		void setupMTDFSearch(const bool bestMoveFirst, const bool etc, const bool moveQualityThreshold,
			const float prune, const bool transpositionTable, const float pass, const bool nullMovePruning,
			const bool useIterativeDeepening, const bool useMTDF);
		bool setupMTDFSearch(const string* args, const BoardStruct& b);
		static string getSetupStringSyntax();

		// functions to save MTDFSearch setup states and restore them
		void saveSettings();    // SearchAlgorithm interface
		void restoreSettings();      // SearchAlgorithm interface
		string printSearchSettings() const;      // SearchAlgorithm interface

		void printSearchSettings(ostream& out) const;

		static void printDebugMessage(const char* buffer, const int currentDepth);

		bool lookup(const Node& findMe, Node& returnMe);
		Node* getBestNode(const Node& parent) const;

		void updateBestLineOfPlay(Node& root, const int boardSize);
};

#endif