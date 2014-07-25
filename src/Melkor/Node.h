#ifndef NODE_H
#define NODE_H

#pragma warning( disable : 4786 )

#include "BoardStruct.h"
using namespace std;

class BoardStruct;

/** A class to represent a node in a search tree.
This class is specific to Go and alpha-beta type searches.
@see MTDFSearch */
class Node
{
	public:

		static int InExistence;
		typedef BOARDSTRUCT_KEY_TYPE key_type;

	protected:

		/** Alpha-beta value of this node. */
//		float score;
		/** Score assigned by neural network. */
//		float NNScore;
		/** Coordinates of the move represented by this node. */
//		short x, y;
		Move move;

		/** Running total of the NNScore including moves played previous to this node.*/
//		float totalMoveScore;
		/** The tree depth at which this node was found. */
		short depth;
		/** The depth to which this node has been expanded. */
		short depthSearchedTo;
		/** Bounds for alpha-beta. */
		float lowerbound;
		float upperbound;
		bool isScoreCalculated;
		bool isLowerboundCalculated;
		bool isUpperboundCalculated;
		/** The colour to play _after_ this node has played. */
		short nextToPlay;
		/** A boolean to indicate if this node should have no children generated for it. */
		bool terminal;

		/** Keys. */
		key_type key;
		key_type key2;
		key_type previousKey;
		key_type previousKey2;

		/** Best child node keys to allow us to fetch it from the transposition table. */
		key_type bestKey;
		key_type bestKey2;
		key_type bestPreviousKey;
		key_type bestPreviousKey2;
		short bestNextToPlay;
		short bestX;
		short bestY;
		bool haveBestNode;

	public:

		Node();
		Node(const BoardStruct& b, int toPlay);
		Node(const Node& n);
		~Node() { InExistence--; };

		Node& operator=(const Node& n);

		/** @return The colour to play _after_ this node has played. */
		inline int getNextToPlay() const { return nextToPlay; };
		/** @param n The colour to play _after_ this node has played. */
		inline void setNextToPlay(int n) { nextToPlay = n; };
		inline float getUpperbound() const { return upperbound; };
		inline void setUpperbound(float u) { upperbound = u; };
		inline float getLowerbound() const { return lowerbound; };
		inline void setLowerbound(float l) { lowerbound = l; };
		inline int getDepthSearchedTo() const { return depthSearchedTo; };
		inline void setDepthSearchedTo(int d) { depthSearchedTo = d; };
		inline float getAlphaBetaScore() const { return move.getAlphaBetaScore(); };
		inline void setAlphaBetaScore(float s) { move.setAlphaBetaScore(s); };
		inline bool scoreCalculated() const { return isScoreCalculated; };
		inline void setScoreCalculated(bool b) { isScoreCalculated = b; };
		inline int getDepth() const { return depth; };
		inline void setDepth(int d) { depth = d; };
		inline bool isTerminal() const { return terminal; };
		inline void setTerminal(bool t) { terminal = t; };

		inline key_type getKey() const { return key; };
		inline key_type getKey2() const { return key2; };
		inline key_type getPreviousKey() const { return previousKey; };
		inline key_type getPreviousKey2() const { return previousKey2; };
		inline void setKey(key_type k) { key = k; };
		inline void setKey2(key_type k) { key2 = k; };
		inline void setPreviousKey(key_type k) { previousKey = k; };
		inline void setPreviousKey2(key_type k) { previousKey2 = k; };
		inline void setKeys(const BoardStruct& board)
		{
			key = board.getKey();
			key2 = board.getKey2();
			previousKey = board.getPreviousKey();
			previousKey2 = board.getPreviousKey2();
		}

		inline key_type getBestKey() const { return bestKey; };
		inline key_type getBestKey2() const { return bestKey2; };
		inline key_type getBestPreviousKey() const { return bestPreviousKey; };
		inline key_type getBestPreviousKey2() const { return bestPreviousKey2; };
		inline void setBestKey(key_type k) { bestKey = k; };
		inline void setBestKey2(key_type k) { bestKey2 = k; };
		inline void setBestPreviousKey(key_type k) { bestPreviousKey = k; };
		inline void setBestPreviousKey2(key_type k) { bestPreviousKey2 = k; };
		inline short getBestNextToPlay() const { return bestNextToPlay; };
		inline void setBestNextToPlay(short n) { bestNextToPlay = n; };
		inline short getBestX() const { return bestX; };
		inline void setBestX(short n) { bestX = n; };
		inline short getBestY() const { return bestY; };
		inline void setBestY(short n) { bestY = n; };
		inline bool getHaveBestNode() const { return haveBestNode; };
		inline void setHaveBestNode(bool l) { haveBestNode = l; };

		inline float getNNScore() const { return move.getNNScore(); };
		inline void setNNScore(float f) { move.setNNScore(f); };
//		inline float getTotalMoveScore() { return totalMoveScore; };
//		inline void setTotalMoveScore(float f) { totalMoveScore = f; };
		inline bool upperboundCalculated() const { return isUpperboundCalculated; };
		inline void setUpperboundCalculated(bool f) { isUpperboundCalculated = f; };
		inline bool lowerboundCalculated() const { return isLowerboundCalculated; };
		inline void setLowerboundCalculated(bool f) { isLowerboundCalculated = f; };

		bool operator<(const Node& p1) const;
		bool operator==(const Node& p1) const;

		void reset(const BoardStruct& b, int toPlay);
		void setBestNode(const Node* best);

		inline void getMove(Move &m) const { m = move; };
		inline Move getMove() const { return move; };
		inline void setMove(const Move& m) { move = m; };
		inline void setX(int mx) { move.setX(mx); };
		inline short getX() const { return move.getX(); };
		inline void setY(int my) { move.setY(my); };
		inline short getY() const { return move.getY(); };

		int getMemoryUsage(bool includeVectorReserved = false) const
		{
			//float totalMoveScore;
			//short depth;
			//short depthSearchedTo;
			//float lowerbound;
			//float upperbound;
			//short nextToPlay;
			//bool isScoreCalculated;
			//bool isLowerboundCalculated;
			//bool isUpperboundCalculated;
			//key_type bestKey;
			//key_type bestKey2;
			//key_type bestPreviousKey;
			//key_type bestPreviousKey2;
			//short bestNextToPlay;
			//bool haveBestNode;

			int bytesUsed = 0;
			bytesUsed+=move.getMemoryUsage();
			bytesUsed+=sizeof(depth);
			bytesUsed+=sizeof(depthSearchedTo);
			bytesUsed+=sizeof(lowerbound);
			bytesUsed+=sizeof(upperbound);
			bytesUsed+=sizeof(nextToPlay);
			bytesUsed+=sizeof(isScoreCalculated);
			bytesUsed+=sizeof(isLowerboundCalculated);
			bytesUsed+=sizeof(isUpperboundCalculated);
			bytesUsed+=sizeof(key);
			bytesUsed+=sizeof(key2);
			bytesUsed+=sizeof(previousKey);
			bytesUsed+=sizeof(previousKey2);
			bytesUsed+=sizeof(bestKey);
			bytesUsed+=sizeof(bestKey2);
			bytesUsed+=sizeof(bestPreviousKey);
			bytesUsed+=sizeof(bestPreviousKey2);
			bytesUsed+=sizeof(bestNextToPlay);
			bytesUsed+=sizeof(haveBestNode);
			bytesUsed+=sizeof(terminal);
			return bytesUsed;
		}
};

#endif