#ifndef SGFREADER_H
#define SGFREADER_H

#pragma warning( disable : 4786 )

#include "SGFTree.h"
#include "SGFNode.h"
#include "BoardStruct.h"
#include "Move.h"
#include <string>
using namespace std;

//class SGFReader : public SGFTree
class SGFReader
{
	protected:

		SGFTree st;
		int nextToPlay;
		SGFNode* current;
		string filename;
		int depth;

	public:
		
		SGFReader();
		virtual ~SGFReader();
		string getFilename() { return filename; };
		bool getLookaheadMoves(int number, int colour, vector<Move>& moves);
		void rewindToStart();
		SGFNode* peekNextNode();
		SGFNode* peekPreviousNode();
		SGFNode* getNextNode();
		SGFNode* getPreviousNode();
		bool peekNextMove(Move& m);
		bool peekPreviousMove(Move& m);
		bool getNextMove(Move& m);
		bool getPreviousMove(Move& m);
		bool getMove(SGFNode* p, Move& m);
		int getCurrentDepth() { return depth; };
		SGFNode* getCurrentNode() { return current; };
		SGFNode& getRootNode();
		SGFTree& getTree();

		// Add a child node to the current one (or to the parent if current is null)
		// using the specified move. Overwrite any other children of current for now
		// until we can set up proper tree editing for sgfs.
		void addMoveNode(Move& m);
		// Add a child node to the current one (or to the parent if current is null)
		// using the current board. Overwrite any other children of current for now
		// until we can set up proper tree editing for sgfs.
		void addSetupNode(const BoardStruct& board);
		// Remove the last node added to this sgf
		void removeNode();

		void clear() { st.clear(); };

		void setGameInfo(SGFGameInfo& sgi) { st.setGameInfo(sgi); };
		SGFGameInfo getGameInfo() const { return st.getGameInfo(); };

		void init(SGFGameInfo& info) {
			st.clear();
			setGameInfo(info);
			st.addHandicapMoves(info.handicap, info.size);
			rewindToStart();
		}

		//bool initBoard(BoardStruct& b);
		bool load(string filename);
		void printInfo() const;
		//int getNextToPlay() { return nextToPlay; };
		string getSignature() const;
		bool getSingleProperty(string& prop, string& v) const;
		bool getSingleProperty(char* c, string& v) const { string t(c); return getSingleProperty(t, v); };
		bool getApplication(string& v) const { return getSingleProperty("AP", v); };
		// 1-4 current SGF format is 4
		bool getFileFormat(string& v) const { return getSingleProperty("FF", v); };
		bool getBoardSize(string& v) const { return getSingleProperty("SZ", v); };
		// return the result of the game
		// "0" or "Draw
		// "B+" for a black win
		// "W+" for a white win
		// "B+12.5"  score example (score is optional)
		// "B+R"     black wins, white resigned
		// "B+Resign" as above
		// "B+T" or "B+Time" for black win on time
		// "B+F" or "B+Forfeit" for black win by forfeit
		// "Void" for no result or suspended play
		// "?" for an unknown result
		bool getResult(string& v) { return getSingleProperty("RE", v); };
		// return rules used for this game
		// "AGA" - American Go Association
		// "GOE" - Ing rules
		// "Japanese" - Nihon-Kiin rule set
		// "NZ"  - New Zealand rules
		bool getRules(string& v) const { return getSingleProperty("RU", v); };
		bool getAnnotator(string& v) const { return getSingleProperty("AN", v); };
		bool getBlackPlayerRank(string& v) const { return getSingleProperty("BR", v); };
		bool getBlackPlayerName(string& v) const { return getSingleProperty("PB", v); };
		bool getWhitePlayerRank(string& v) const { return getSingleProperty("WR", v); };
		bool getWhitePlayerName(string& v) const { return getSingleProperty("PW", v); };
		bool getDate(string& v) const { return getSingleProperty("DT", v); };
		bool getEventName(string& v) const { return getSingleProperty("EV", v); };
		bool getGameName(string& v) const { return getSingleProperty("GN", v); };
		bool getKomi(float& f) const;
		bool getHandicap(int& f) const;
};

#endif