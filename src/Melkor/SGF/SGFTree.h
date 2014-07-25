#ifndef SGFTREE_H
#define SGFTREE_H

#pragma warning( disable : 4786 )

#include "SGFNode.h"
#include "SGFGameInfo.h"
#include <string>
using namespace std;

/** @brief A class to represent a tree of SGFNodes. Note: This object will
  * be the root node of the tree. */
class SGFTree : public SGFNode
{
	public:

		SGFTree() : SGFNode() {};
		virtual ~SGFTree() {};
		void clear();
		bool parseSGF(string sb);
		bool getAllPrimaryMoves(vector<Move>& moves) const;
		void addHandicapMoves(int handicap, int boardSize);
		void setGameInfo(SGFGameInfo& info);
		SGFGameInfo getGameInfo() const;
};

#endif