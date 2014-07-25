#ifndef MELKOR_H
#define MELKOR_H

#pragma warning( disable : 4786 )

#include "LogWriter.h"
#include "BoardStruct.h"
#include "Test.h"
#include "Go.h"
#include "GTPFunctions.h"
#include "PatternMatcher.h"
#include "Misc.h"
#include <ctime>
#include <string>
using namespace std;

#define APPLICATION_NAME "Melkor"
#define MAJOR_VERSION "0"
#define MINOR_VERSION "6"

/** @brief The main class in this package. It is a manager class, holds
several Go utility functions and provides a user interface to the user. */
class Melkor
{
	public:

		// Data structures for storing strategic and tactical information during play
		struct UrgentMoveInfo {
			UrgentMoveInfo(int toPlay, const vector<Move>& bl, const vector<Move>& wl, GoString* str) {
				nextToPlay = toPlay;
				bestLineOfPlay = bl;
				worstLineOfPlay = wl;
				string = str;
				worth = 0.0f;
				useFirstGuess = false;
				firstGuessNextToPlay = 0.0f;
				firstGuessEnemyToPlay = 0.0f;
			}
			// Who is next to play
			int nextToPlay;
			// This sequence of moves starts with nextToPlay
			vector<Move> bestLineOfPlay;
			// This sequence of moves starts with the opposite of nextToPlay
			vector<Move> worstLineOfPlay;
			/** @todo This should be an anchor stone and then we do Board.getString(anchor.x, anchor.y)
			  * to retrieve the correct string from the board. */
			// A pointer to the string which the above lines of play concern
			GoString* string;
			// The estimated worth of this move, in points
			float worth;
			// This can be fed back into successive tree searches as the firstguess
			// if using MTD(f)...
			float firstGuessNextToPlay;
			float firstGuessEnemyToPlay;
			// If this is true we have valid usable first guess values
			bool useFirstGuess;
		};

		struct BigMoveInfo {
			BigMoveInfo(int toPlay, const Move& m) {
				nextToPlay = toPlay;
				move = m;
				worth = 0.0f;
			}
			inline bool operator>(const BigMoveInfo& p1) const { return (worth>p1.worth) ? true : false; };
			// Who is next to play
			int nextToPlay;
			Move move;
			float worth;
		};

		struct LineOfPlay {
			LineOfPlay(const vector<Move>& bl, const int ax, const int ay, const int ac) {
				anchorX = ax;
				anchorY = ay;
				anchorColour = ac;
				lineOfPlay = bl;
			}
			int anchorX, anchorY, anchorColour;
			vector<Move> lineOfPlay;
		};

		/** @todo Public pointers to our neural networks - shouldn't these be private? */
		BPNGoAdapterInterface* bpn;

	private:	

		enum SearchType { LADDER, LOOSE_TACTICAL, STATUS, BIG_SEQUENCE };

		BPN* internalBPN;

		// These two vectors are the persistent move caches for getBestGlobalMove()
		vector<BigMoveInfo> bigMoves;
		vector<UrgentMoveInfo> urgentMoves;

		int useNBigMoves;

		// Used by getBestGlobalMove() to monitor what to keep in the
		// urgent and big move caches from call to call
		BoardStruct::key_type latestBoardKey;

		int opponentColour;

		// Time limits - in milliseconds
		double timePerMove;
		bool timePerMoveRanOut;

		// Totalled average 1 ply times for each search type in speedConstant units (not ms!).
		double totalLadder1PlyTime, totalLooseTactical1PlyTime, totalStatus1PlyTime, totalBigSequence1PlyTime;
		int totalLadderSearches, totalLooseTacticalSearches, totalStatusSearches, totalBigSequenceSearches;

		int getBestGlobalMoveCalls;

		// Depth limits
		int maxLadderDepth;
		int maxLooseTacticalDepth;
		int maxStatusDepth;
		int maxBigSequenceDepth;     // Controls depth of search for follow-up sequences to a big move

		double speedConstant;

		/** @brief If this is true then don't generate a pass
		  *  as a move unless there are no moves left. */
		bool noPasses;

#ifdef _WIN32
		LARGE_INTEGER startTime;
		LARGE_INTEGER timerFreq;
#else
		clock_t startTime;
#endif
		bool playTimedGame;

		///////////////////////
		// Internal SGF tree //
		///////////////////////

		// Indicates whether the sgf tree has been modified since loading
		bool sgfModified;
		SGFReader sgf;

		///////////////
		// Log stuff //
		///////////////	
		int charCount;
		bool useLog;
	
	public:
		static const string myName;
		// default command line values
		static const string defaultNetwork;
		static const bool defaultUseDynamicSearchDepth;
		static const bool defaultUseGlobalMoveFunction;
		static const bool defaultUseSuperKo;
		static const bool defaultUseGameTreeSearch;
		static const bool defaultUseGTP;
		static const int defaultComputerColour;
		static const bool defaultDebuggo;
		static const bool defaultVerbose;
		static const bool defaultCheat;

		// command line options
		bool useDynamicSearchDepth;
		bool useGlobalMoveFunction;
		bool useGameTreeSearch;
		bool useGTP;
		int computerColour;
		bool cheat;
		bool debuggo;
		// ordinary messaging flags
		bool verbose;

		// error handling
		string errorText;
		bool error;

		MTDFSearch* mtdfSearch;
		Test* test;
		GTPFunctions* gtp;

		// Eye space pattern matcher
		PatternMatcher eyeSpacePM;

		Melkor(unsigned long& seed, const string* network, const bool debugmtdf);
		virtual ~Melkor();

		static string getVersionString()
		{
			char buffer[50];
			sprintf(buffer, "%s.%s", MAJOR_VERSION, MINOR_VERSION);
			return string(buffer);
		}

		void setError(bool e, string m) { error = e; errorText = m; };
		void setUseSuperKo(bool useSuperKo) { Go::setUseSuperKo(useSuperKo); };
		const string getName() { return myName; };

		/** @return The time allowed per move in seconds. */
		const double getTimePerMove() const { return timePerMove; };
		/** @param t Time allowed per move in seconds. */
		void setTimePerMove(const double t) { timePerMove = t; };

		void setPlayTimedGame(bool p) { playTimedGame = p; };
		bool getPlayTimedGame() const { return playTimedGame; };

		void setNoPasses(bool p) { noPasses = p; };
		bool getNoPasses() const { return noPasses; };

		bool getUseDynamicSearchDepth() const { return useDynamicSearchDepth; };
		int getMaxStatusDepth() const { return maxStatusDepth; };
		void setMaxStatusDepth(int m) { maxStatusDepth = m; };
		int getMaxLadderDepth() const { return maxLadderDepth; };
		int getMaxLooseTacticalDepth() const { return maxLooseTacticalDepth; };

		// Interface functions
		bool DoMainMenu(LogWriter& log);
		bool DoTestMenu(LogWriter& log);
		void playGTP(string gtpScriptFilename);  /** @todo Why is this function repeated later? */

		// The following methods require any search methods used in Morgoth
		// i.e. LineOfPlayGS and MTDFSearch to implement the interface 'SearchAlgorithm'
		bool setSearchGoal(const Goal& g);
		void getBestLineOfPlay(vector<Move>& l) const;
		void saveSearchSettings();
		void restoreSearchSettings();
		string printSearchSettings() const;

		// Initialise this object
		bool init(const string* network);
		bool loadBPN(const string& filename);
		static BPNGoAdapterInterface* loadBPNStatic(const string& filename);
		BPNGoAdapterInterface* getBPN() { return bpn; };

		/*********************/
		/* Playing functions */
		/*********************/

		float getFinalScore();

		// estimate the score from black's perspective, i.e. total territories and captures
		// therefore score is negative if white wins
		float estimateScore(const BoardStruct &b, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = NULL,
							int dilations = 5, int erosions = 21, bool scaleToStrength = true);
		void generateLibertyInfluenceMap(const BoardStruct &b, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
										int dilations, int erosions);

		// Play Go against a human with a text interface
		void playGo(BoardStruct &b, int compColour=BLACK);
		// Use GTP to play as a client, not server
		void playGTP(BoardStruct &b, string gtpScriptFilename, int compColour=BLACK);

		// Return the best move available for the given board state
		// using NN and alpha-beta and if requested
		void getMove(Move &m, BoardStruct &b, int nextToPlay);

		// Functions uesd by and related to getBestGlobalMove()
		void getBestGlobalMove(Move& m, BoardStruct& b, const int nextToPlay, const BoardStruct* previousBoard);
		void updateStringsStatus(BoardStruct& b, const int nextToPlay, vector<UrgentMoveInfo>* urgentMoves,
								vector<Point<> >* stringsToUpdate, const int maxDepth,
								vector<LineOfPlay>* bestLinesOfPlay);
		void estimateWorthOfUrgentMoves(const BoardStruct& board, const int nextToPlay, 
										   vector<UrgentMoveInfo>& urgentMoves);
		void estimateWorthOfBigMoves(const BoardStruct& board, const int nextToPlay, 
										   vector<BigMoveInfo>& bigMoves);
		void exchangeUrgentStrings(BoardStruct& board, const int nextToPlay) const;
		void selectChangedStrings(vector<Point<> >& changedStrings, const BoardStruct& oldBoard,
									 const BoardStruct& newBoard);
		void selectAdjacentEnemyStrings(vector<Point<> >& stringsToUpdate, const BoardStruct& board,
										const int x, const int y);
		void getBigMoves(vector<BigMoveInfo>& bigMoves, const vector<UrgentMoveInfo>* urgentMoves,
						const BoardStruct& board, const int nextToPlay,	const int maxBigMoves);

	private:
		void calculateSearchDepths(int timePerMove, int numberOfBigMoves);
		int calculateTimePerMove(const int colour, const BoardStruct& board) const;
		void updateSearchTimingStats(const double searchTime, const SearchType searchType, const int searchDepth);

		/** @return The time left from now on our allocated time per move, in seconds. */
		double getTimeLeftPerMove()
		{
//			time_t end;
//			time(&end);
//			return timePerMove - difftime(end, startTime);
#ifdef _WIN32
			LARGE_INTEGER tend;
			timerEnd(&tend);
			// timerValue is in seconds
			return timePerMove - (timerValue(startTime, tend, timerFreq));
#else
			clock_t tend;
			timerEnd(&tend);
			return timePerMove - (double)(tend - startTime) / CLOCKS_PER_SEC;
#endif
		}
#ifdef _WIN32
		bool checkTimePerMove(LARGE_INTEGER *localStart, double &lastTime);
		void startSearchTimer(LARGE_INTEGER* tstart, LARGE_INTEGER* freq);
		double endSearchTimer(LARGE_INTEGER* tstart, LARGE_INTEGER* freq);
#else
		bool checkTimePerMove(clock_t *localStart, double &lastTime);
#endif
		void addStringIfUnique(const BoardStruct& board, vector<Point<> >& strings, const int x, const int y) const;

	public:
		void clearBoard() { Go::reset(); };
		void resizeBoard(int s) { Go::reset(s); };
		BoardStruct& getBoard() { return Go::getBoard(); };
		const BoardStruct& getBoard() const { return Go::getBoard(); };

		// Log functions
		void startLog();
		void stopLog();

		// Get the best move available just using NN
		void getBestMoveWithoutAlphaBeta(Move &m, BoardStruct &b, int nextToPlay);

		// Get the best move available using NN and alpha-beta
		// together
		void getBestMoveWithAlphaBeta(Move &m, BoardStruct &b, int nextToPlay);

		// Get a list of all legal moves and their associated NN scores
		static void getScoredMoves(vector<Move> &m, BoardStruct &b, int colour, BPNGoAdapterInterface& bpn, 
			bool useOldScores = false, bool useIllegalMoveMemory = false, vector<Move>* changedPoints = NULL, 
			const Rect* bounds = NULL, bool useNeuralNetwork = true, bool printDebuggingInfo = false, 
			const int maximisingColour = EMPTY);
		// Just score the specified move
		static float getScoredMove(int j, int i, const BoardStruct &b, int colour, BPNGoAdapterInterface& bpn,
			vector<Matrix<float> >* outputs = NULL);

		// Given a board state return a new board pointer
		// to a board with all the colours reversed
		static void reverseColours(const BoardStruct &in, BoardStruct &out);

		static bool NNPassJudge(const BoardStruct& board, int nextToPlay, const Rect& bounds, float passValue,
			BPNGoAdapterInterface& bpn, const int maximisingColour = EMPTY);

		/***********************************************/
		/* Functions that modify the internal SGF tree */
		/***********************************************/

		bool getSGFModified() const { return sgfModified; };
		void setSGFModified(bool s) { sgfModified = s; };
		SGFReader& getInternalSGF() { return sgf; };

		void initialiseSGFTree(int boardSize, int handicap = 0);

		void setBoardSize(int bs);
		void setBlackName(string name);
		void setWhiteName(string name);
		void setSGFResultString(string result);
		void setKomi(float komi);
		bool setHandicap(int handicap);
		void setTimeSettings(int mainTime, int byoyomiTime, int byoyomiStones);
		void setCurrentTimeSettings(int colour, int mainTime, int byoyomiTime, int byoyomiStones);
};

#endif