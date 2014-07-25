#ifndef GO_H
#define GO_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "SpecialPoint.h"
#include "Move.h"
#include "BoardStruct.h"
#include "Rect.h"
#include <vector>
#include <string>
using namespace std;


/** @brief A static class containing information about the current game setup such as
  * rules, handicap, time system. This class also contains the main board object represented
  * as a BoardStruct. Functions included in here are all related to Go, such as isLegal(),
  * and functions to test the status of points and strings and their relation to one another, see
  * isAdjacentToEdge() and findAdjacentStrings(). */
class Go
{
	public:
		enum Rules { JAPANESE_RULES=1, CHINESE_RULES };
		// CANADIAN_BYO_YOMI is main time plus n stones per byo-yomi period
		enum TimeSystem { CANADIAN_BYO_YOMI };

	private: 
		
		// container class
		struct BoardKeys {
			public:
				BoardStruct::key_type key;
				BoardStruct::key_type key2;
				BoardStruct::key_type previousKey;
				BoardStruct::key_type previousKey2;
		};
		
		static vector<Move> moveList;
		static int currentMoveNumber;
		// for checking positional super-ko
		typedef vector<BoardKeys> boardKeysType;
		static boardKeysType boardKeys;
		static vector<BoardStruct> boardList;
		
		static int nextToPlay;
		// even handicap
		static int handicap;
		// japanese rules
		static Rules rules;
		static float komi;
		static bool useSuperKo;
		
		// Time rules - all times in seconds
		static TimeSystem timeSystem;
		static int mainTime, currentMainTimeBlack, currentMainTimeWhite;
		static int byoyomiStones, currentByoyomiStonesBlack, currentByoyomiStonesWhite;
		static int byoyomiTime, currentByoyomiTimeBlack, currentByoyomiTimeWhite;

		static BoardStruct board;
		
		static bool printExtraDebuggingInfo;
		
	public: 

#ifdef INCLUDE_TIMING_CODE
		static int IsLegalCalled;
		static double IsLegalTotalTimeUpdatePoints;
		static double IsLegalTotalTimeSearch;
		static double IsLegalTotalTimeSuperKo;
		static double IsLegalTotalTimeBoardCopy;
		static time_t changedPointsStart, changedPointsEnd;
		static time_t searchStart, searchEnd;
		static time_t koStart, koEnd;
		static time_t boardCopyStart, boardCopyEnd;
#endif
			
		static const int maxInfluenceBlack;
		static const int maxInfluenceWhite;

		static const bool defaultDontFillOwnEyes;
		static bool dontFillOwnEyes;
		
		static BoardStruct& getBoard() { return board; };
		static const vector<BoardStruct>& getBoardList() { return boardList; };
		static void reset() { reset(0); };
		static void reset(int newSize) 
		{ 
			resetMoveList();
			currentMoveNumber = 0; 
			if(newSize==0)
				board.clear();
			else
				board.clear(newSize);
			nextToPlay = BLACK;
		};

		// Time system functions
		static TimeSystem getTimeSystem() { return timeSystem; };
		static void setTimeSystem(TimeSystem t) { timeSystem = t; };

		/** @return The main time period in seconds. */
		static int getMainTime() { return mainTime; };
		static void setMainTime(int t) { mainTime = t; };
		static int getByoyomiStones() { return byoyomiStones; };
		static void setByoyomiStones(int b) { byoyomiStones = b; };
		static int getByoyomiTime() { return byoyomiTime; };
		static void setByoyomiTime(int b) { byoyomiTime = b; };
		
		/** @return The main time left for black in seconds. */
		static int getCurrentMainTimeBlack() { return currentMainTimeBlack; };
		/** @return The main time left for white in seconds. */
		static int getCurrentMainTimeWhite() { return currentMainTimeWhite; };
		static void setCurrentMainTimeBlack(int t) { currentMainTimeBlack = t; };
		static void setCurrentMainTimeWhite(int t) { currentMainTimeWhite = t; };
		static int getCurrentByoyomiStonesBlack() { return currentByoyomiStonesBlack; };
		static int getCurrentByoyomiStonesWhite() { return currentByoyomiStonesWhite; };
		static void setCurrentByoyomiStonesBlack(int b) { currentByoyomiStonesBlack = b; };
		static void setCurrentByoyomiStonesWhite(int b) { currentByoyomiStonesWhite = b; };
		static int getCurrentByoyomiTimeBlack() { return currentByoyomiTimeBlack; };
		static int getCurrentByoyomiTimeWhite() { return currentByoyomiTimeWhite; };
		static void setCurrentByoyomiTimeBlack(int b) { currentByoyomiTimeBlack = b; };
		static void setCurrentByoyomiTimeWhite(int b) { currentByoyomiTimeWhite = b; };
		
		static void setPrintExtraDebuggingInfo(bool t) { printExtraDebuggingInfo = t; };
		static bool getPrintExtraDebuggingInfo() { return printExtraDebuggingInfo; };
		
		static void setUseSuperKo(bool s) { useSuperKo = s; };
		static bool getUseSuperKo() { return useSuperKo; };
		
		// Add the given move to the move list
		static void addToMoveList(Move &m, const BoardStruct& board);			
		static void resetMoveList() { moveList.clear(); currentMoveNumber = 0; boardKeys.clear(); boardList.clear(); };
		static vector<Move>& getMoveList() { return moveList; };
		static void undo(BoardStruct& board);			
		static void setKomi(float k) { komi=k; };
		static float getKomi() { return komi; };
		static void setRules(Rules r) { rules = r; };
		static Rules getRules() { return rules; };
		static void setHandicap(int h) { handicap=h; };
		static int getHandicap() { return handicap; };
		// Add a handicap move to the board
		static void addHandicapMove(int x, int y, BoardStruct &b);			
		// Setup the board given, according to the handicap settings
		static bool doFixedHandicap(BoardStruct &b);
		static bool getFixedHandicapMoves(vector<Move>& moves, int handicap, int size);
		static bool doFreeHandicap(vector<Move>& stones, BoardStruct &b);
		static int getCurrentMoveNumber() { return currentMoveNumber; };
		static int getNextToPlay() { return nextToPlay; };
		static void setNextToPlay(int n) { nextToPlay = n; };
		
		static void findExtendedLiberties(vector<SpecialPoint>& liberties, const BoardStruct& board, int toPlay, int tX, int tY);
		static void findLegalExtendedLiberties(vector<SpecialPoint>& liberties, BoardStruct& board, int toPlay, int tX, int tY);
		static bool getBestExtendedLibertyMove(Move& m, BoardStruct& b, int toPlay, int targetX, int targetY);
		
		// Get a list of all possible moves, including illegal ones
		static void getAllMoves(vector<Move> &m, const BoardStruct &b);

		/***************************/
		/* Move legality Functions */
		/***************************/
		
		// Get a list of all possible moves, excluding illegal ones
		static void getLegalMoves(vector<Move> &m, const int colour, BoardStruct &b, const bool useSuperKo,
			const Rect* bounds = NULL, const bool useIllegalMoveMemory = false,
			const vector<Move>* changedPoints = NULL, const int ourColour = EMPTY);
		
		static void resetIllegalMoveMemory(BoardStruct& tempBoard, bool useSuperKo);
			
		static bool isLegal(BoardStruct &board, const int j, const int i, const int colour, const bool useSuperKo, 
			const bool useIllegalMoveMemory = false, const vector<Move>* changedPoints = NULL,
			const bool reset = false, const int ourColour = EMPTY);
		static bool isLegalSequence(const vector<Move>& pVar, const BoardStruct& b, int& failedPos);
		static bool addToIllegalMoveMemory(Move& m, vector<Move>& illegalMoveMemory);
		static bool isSuicide(int ourColour, int enemyColour, int x, int y, const BoardStruct& b);
		
		// Get a list of all possible moves, excluding illegal ones and some
		// we choose to filter out
		static void getFilteredLegalMoves(vector<Move> &m, int colour, const BoardStruct &b,
			bool useSuperKo, const Rect* bounds = NULL);
		
		// used with getFilteredLegalMoves to remove ones we don't like
		static bool filteredIsLegal(const BoardStruct &board, int j, int i, int colour, bool useSuperKo);

		/********************************************/
		/* Point info and string relation functions */
		/********************************************/

		static int countLibs(int x, int y, const BoardStruct& board);
		static void findAdjacentStrings(int tX, int tY, const BoardStruct& temp, 
								vector<BoardStruct::constStringsIterator>& adjStrings);
//		void findAdjacentStrings(BoardStruct::constStringsIterator& str, const BoardStruct& temp, 
//								vector<BoardStruct::constStringsIterator>& adjStrings);
		static bool isAdjacentToDeadStringOrLiberty(int mx, int my, const BoardStruct& board);
		static bool isAdjacentToEdge(const Point<> point, const int boardSize);
		static bool hasFriendlySharedLiberty(const BoardStruct& board, BoardStruct::constStringsIterator& str);
		static void getFriendlySharedLibertyStrings(const BoardStruct& board, BoardStruct::constStringsIterator& citer, 
									 vector<BoardStruct::constStringsIterator>& strings);

};

#endif