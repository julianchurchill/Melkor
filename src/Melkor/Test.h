#ifndef NEURALTEST_H
#define NEURALTEST_H

#pragma warning( disable : 4786 )

#include "Go.h"
#include "BoardStruct.h"
#include "BPNGoAdapter.h"
#include "MoveSuggester.h"
#include "MTDFSearch.h"
#include "LogWriter.h"
#include <string>
using namespace std;

class Test
{
	public:

		static const string myName;
		
		void* melkor;

//		int nodesExpanded;
//		int nodesEvaluated;

		Test(void* m, bool useMelkorType);
		virtual ~Test() {};

		/** @todo Deprecated by UnitTest in GoTest - remove it from here. */
		void IsLegalTest(bool verbose=false);
		bool isLegalHelper(const char* titleBoard, BoardStruct& test, int x, int y, int colour,
			int testX, int testY, int testColour, bool result, bool verbose);

		void LibertyCountTest(bool verbose=false);
		void GoStringsTest(bool verbose=false);
		void SGFStepthroughTest(BoardStruct& board, string sgfFilename, bool verbose = false);
		void TrainingSpeedTest();
		void BPNTest();

		// Alpha Beta test 1
		void AlphaBetaTest(bool verbose  = false);
		void outputInfo(const float score, const float exScore, const int nodesTraversed,
				const int exNodesTraversed, const int nodesEvaluated, const int exNodesEvaluated);
//		static MTDFSearch::EXPAND_FN alphaBetaTestExpand;
//		void addChildren(vector<Node>& nodes, int x1, int y1, float f1, 
//			int x2, int y2, float f2, const Node& n);
//		float updateNodeScore(Node* n, float s);
//		static MTDFSearch::EVALUATE_FN alphaBetaTestEvaluate;

		// Alpha Beta test 2
		void AlphaBetaTest2(bool verbose = false);
		void AlphaBetaTest3(bool verbose = false);
		bool AlphaBetaHelper(const string& info, const bool bmf, const bool etc, const bool moveQualityThreshold, 
			   const float pruneValue, const bool useTTable, const float passValue, 
			   MoveSuggester* suggester, const Goal& goal, const BoardStruct& board,
			   const bool useMTDF, const bool useID, const int depth, const int IDStep, 
			   const int nextToPlay, const bool verbose, const float rScore, const int rX, 
			   const int rY, const int rNodesTraversed, const int rNodesEvaluated);

		// Memory tests
		void TranspositionTableTest(bool verbose = false);

		struct ResultsEntry
		{
			ResultsEntry(int n, string na, double t, bool s, int be, int ec, bool sa)
				: number(n), name(na), timeTaken(t), success(s), boardsExamined(be),
				evaluationCalls(ec), successfulAnswer(sa) {};
			int number;
			string name;
			double timeTaken;
			bool success;
			int boardsExamined;
			int evaluationCalls;
			bool successfulAnswer;
		};

		void MTDFSearchTest(bool verbose = false);
//		void stringLifeTest(bool verbose = false);
		void influenceFunctionTest(bool verbose = false);
		void ladderTest(bool verbose = false);
		void ladderTestHelper(const BoardStruct& board, const Point<>& ladderCoords, const Point<>& order3Lib, 
							const Point<>& order2Lib, const bool verbose);
		void getAnswerTest(bool verbose = false);

		/** @todo This should be private - need to remove #include from SearchSettings class first. */
		/** @brief This class is used by AlphaBetaTest(). Don't use or extend this class outside
		  * of AlphaBetaTest(). */
		class AlphaBetaTestSuggester : public MoveSuggester
		{
		public:
			virtual ~AlphaBetaTestSuggester() {};

			void getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings,
				const int nextToPlay, const int maximisingColour, BPNGoAdapterInterface& bpn, 
				int maxMoves = -1) const
			{
				// test case based on 
				// http://www.cis.temple.edu/~ingargio/cis587/readings/alpha-beta-example.html
				
				// match child moves to move name (extraScore) in example
				// Example tree (numbers for move names instead of letters):
				//
				//                        0									
				//                  /          \
				//            1                       16					BLACK
				//         /     \                  /     \
				//      2           9          17            24				WHITE
				//    /  \        /  \        /  \          /  \
				//   3    6      10   13    18    21      25    28			BLACK
				//  /\    /\    /\    /\    /\    /\     /\      /\
				// 4  5  7  8  11 12 14 15 19 20 22 23  26 27  29 30		WHITE
				
				if(board.getSize() < 9) 
					throw "AlphaBetaTestSuggester: This test suggester requires a board at least 9x9!";

				moves.clear();
				Move parentMove( board.getLastMove() );
				
				// For the purposes of transposition table lookup each move must be a unique Go move
				// and must be legal for the board position at that point in the tree
				if( parentMove.getNNScore() == 0.0f ) addChildMoves(moves, 1, 1, BLACK, 1.0f, 16.0f);
				else if( parentMove.getNNScore() == 1.0f ) addChildMoves(moves, 3, 1, WHITE, 2.0f, 9.0f);
				else if( parentMove.getNNScore() == 2.0f ) addChildMoves(moves, 5, 1, BLACK, 3.0f, 6.0f);
				else if( parentMove.getNNScore() == 3.0f ) addChildMoves(moves, 7, 1, WHITE, 4.0f, 5.0f);
				else if( parentMove.getNNScore() == 6.0f ) addChildMoves(moves, 1, 3, BLACK, 7.0f, 8.0f);
				else if( parentMove.getNNScore() == 9.0f ) addChildMoves(moves, 3, 3, WHITE, 10.0f, 13.0f);
				else if( parentMove.getNNScore() == 10.0f ) addChildMoves(moves, 5, 3, BLACK, 11.0f, 12.0f);
				else if( parentMove.getNNScore() == 13.0f ) addChildMoves(moves, 7, 3, BLACK, 14.0f, 15.0f);
				else if( parentMove.getNNScore() == 16.0f ) addChildMoves(moves, 1, 5, BLACK, 17.0f, 24.0f);
				else if( parentMove.getNNScore() == 17.0f ) addChildMoves(moves, 3, 5, WHITE, 18.0f, 21.0f);
				else if( parentMove.getNNScore() == 18.0f ) addChildMoves(moves, 5, 5, BLACK, 19.0f, 20.0f);
				else if( parentMove.getNNScore() == 21.0f ) addChildMoves(moves, 7, 5, BLACK, 22.0f, 23.0f);
				else if( parentMove.getNNScore() == 24.0f ) addChildMoves(moves, 1, 7, WHITE, 25.0f, 285.0f);
				else if( parentMove.getNNScore() == 25.0f ) addChildMoves(moves, 3, 7, BLACK, 26.0f, 27.0f);
				else if( parentMove.getNNScore() == 28.0f ) addChildMoves(moves, 5, 7, BLACK, 29.0f, 30.0f);
			}
			
			void getMovesUnsorted(vector<Move>& moves, const BoardStruct& board, 
				const SearchSettings& settings,	const int nextToPlay, const int maximisingColour, 
				BPNGoAdapterInterface& bpn, int maxMoves = -1) const
			{
				getMoves(moves, board, settings, nextToPlay, maximisingColour, bpn, maxMoves);
			}
			
		private:
			void addChildMoves(vector<Move>& moves, const int x, const int y, const int colour, 
				const float name1, const float name2) const
			{
				moves.push_back( Move(x, y, colour, 19, name1) );
				moves.push_back( Move(x+1, y, colour, 19, name2) );
			}
		};

	private:

		/** @brief This class is used by AlphaBetaTest(). Don't use or extend this class outside
		  * of AlphaBetaTest(). */
		class AlphaBetaTestGoal : public GoalInterface
		{
		public: 
			AlphaBetaTestGoal(int a=0, int b=0, int c=0)
				: GoalInterface(GoalInterface::NONE, GoalInterface::SOLID, "ALPHABETATESTGOAL", a, b, c, false, 
				DEAD, ALIVE, GoalInterface::TERMINAL|GoalInterface::TARGETED) {};
			AlphaBetaTestGoal(const GoalInterface& g) : GoalInterface(g) {};
			virtual ~AlphaBetaTestGoal() {};
			
			
			bool successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score, const BoardStruct& b,
				bool factorInSolutionSize, const int nextToPlay)
			{
				throw "This function is not implemented for AlphaBetaTestGoal.";
			}
			bool successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
				bool factorInSolutionSize, const int nextToPlay)
			{
				throw "This function is not implemented for AlphaBetaTestGoal.";
			}
			
			// for game tree search (minimax) users
			bool evaluate(const int maximisingColour, float& score, const BoardStruct& b, 
				const int nextToPlay, int* bestX = NULL, int* bestY = NULL)
			{
				// test case based on 
				// http://www.cis.temple.edu/~ingargio/cis587/readings/alpha-beta-example.html
				// Node names:
				// 4  5  7  8  11 12 14 15 19 20 22 23 26 27  29 30	
				// Evaluation scores: 
				// 10 11 9 12  14 15 13 14 15  2  4  1  3 22  24 25
				Move parentMove( b.getLastMove() );

				if( parentMove.getNNScore() == 4.0f ) score = 10.0f;
				else if( parentMove.getNNScore() == 5.0f ) score = 11.0f;
				else if( parentMove.getNNScore() == 7.0f ) score = 9.0f;
				else if( parentMove.getNNScore() == 8.0f ) score = 12.0f;
				else if( parentMove.getNNScore() == 11.0f ) score = 14.0f;
				else if( parentMove.getNNScore() == 12.0f ) score = 15.0f;
				else if( parentMove.getNNScore() == 14.0f ) score = 13.0f;
				else if( parentMove.getNNScore() == 15.0f ) score = 14.0f;
				else if( parentMove.getNNScore() == 19.0f ) score = 15.0f;
				else if( parentMove.getNNScore() == 20.0f ) score = 2.0f;
				else if( parentMove.getNNScore() == 22.0f ) score = 4.0f;
				else if( parentMove.getNNScore() == 23.0f ) score = 1.0f;
				else if( parentMove.getNNScore() == 26.0f ) score = 3.0f;
				else if( parentMove.getNNScore() == 27.0f ) score = 22.0f;
				else if( parentMove.getNNScore() == 29.0f ) score = 24.0f;
				else if( parentMove.getNNScore() == 30.0f ) score = 25.0f;
				return true;
			}

			// Capture goal must have the maximising colour in a minimax tree as the target colour
			// since ALIVE is +v.e. and DEAD is -v.e. in globals.h
			bool caresAboutMaximisingColour() const 
			{
				throw "This function is not implemented for AlphaBetaTestGoal.";
			}

			int getRequiredMaximisingColour(const BoardStruct& b, const int nextToPlay) const
			{
				throw "This function is not implemented for AlphaBetaTestGoal.";
			}
		};
};

#endif