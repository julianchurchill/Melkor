#pragma warning( disable : 4786 )

#include "Node.h"
#include "MTDFSearch.h"
#include "Move.h"
#include "Go.h"
#include "hash_set.h"
#include <stdlib.h>
#include <stdio.h>
//using namespace std;

int Node::InExistence = 0;

Node::Node() : haveBestNode(false), depthSearchedTo(0), isScoreCalculated(false), isUpperboundCalculated(false),
	isLowerboundCalculated(false), lowerbound((float)INT_MIN), upperbound((float)INT_MAX), nextToPlay(BLACK), 
	move(-1, -1, WHITE, 19), depth(0), terminal(false)
{
	key = 0;
	key2 = 0;
	previousKey = 0;
	previousKey2 = 0;
	setBestNode(NULL);
	InExistence++;
}

/** Create a new node from the board state given. 
@param b The board state this node should copy keys from.
@param toPlay The next colour to play. */
Node::Node(const BoardStruct& b, int toPlay) : haveBestNode(false), depthSearchedTo(0), isScoreCalculated(false), isUpperboundCalculated(false),
	isLowerboundCalculated(false), lowerbound((float)INT_MIN), upperbound((float)INT_MAX), nextToPlay(toPlay),
	depth(0), terminal(false), move(b.getLastMove())
{
	setKeys(b);
	setBestNode(NULL);
	InExistence++;
}

Node::Node(const Node& n) : bestKey(n.bestKey), bestKey2(n.bestKey2), bestPreviousKey(n.bestPreviousKey),
	bestPreviousKey2(n.bestPreviousKey2), bestNextToPlay(n.bestNextToPlay), bestX(n.bestX), bestY(n.bestY),
	haveBestNode(n.haveBestNode), move(n.move), depth(n.depth),
	depthSearchedTo(n.depthSearchedTo), lowerbound(n.lowerbound), upperbound(n.upperbound),
	nextToPlay(n.nextToPlay), isScoreCalculated(n.isScoreCalculated), isUpperboundCalculated(n.isUpperboundCalculated),
	isLowerboundCalculated(n.isLowerboundCalculated), key(n.key), key2(n.key2), previousKey(n.previousKey),
	previousKey2(n.previousKey2), terminal(n.terminal)
{
	InExistence++;
}

Node& Node::operator=(const Node& n)
{
	// beware self assignment, n = n
	if(this!=&n){
		bestKey = n.bestKey;
		bestKey2 = n.bestKey2;
		bestPreviousKey = n.bestPreviousKey;
		bestPreviousKey2 = n.bestPreviousKey2;
		bestNextToPlay = n.bestNextToPlay;
		bestX = n.bestX;
		bestY = n.bestY;
		haveBestNode = n.haveBestNode;
		key = n.key;
		key2 = n.key2;
		previousKey = n.previousKey;
		previousKey2 = n.previousKey2;
		depth = n.depth;
		depthSearchedTo = n.depthSearchedTo;
		lowerbound = n.lowerbound;
		upperbound = n.upperbound;
		nextToPlay = n.nextToPlay;
		isScoreCalculated = n.isScoreCalculated;
		isUpperboundCalculated = n.isUpperboundCalculated;
		isLowerboundCalculated = n.isLowerboundCalculated;
		move = n.move;
		terminal = n.terminal;
	}
	return *this;
}

bool Node::operator<(const Node& p1) const
{
	if(key==p1.key){
		if(key2==p1.key2){
			if(previousKey==p1.previousKey){
				if(previousKey2==p1.previousKey2)
					return (nextToPlay<p1.nextToPlay) ? true : false;
				else
					return (previousKey2<p1.previousKey2) ? true : false;
			}
			else
				return (previousKey<p1.previousKey) ? true : false;
		}
		else
			return (key2<p1.key2) ? true : false;
	}
	else
		return (key<p1.key) ? true : false;
}

// The move coordinates and the keys need to be compared since we may have
// two nodes with the same key, i.e. a move on a board, and a pass on the board after
// would give the same keys. So compare the actual moves too.
bool Node::operator==(const Node& p1) const
{
	if(key==p1.key && key2==p1.key2 && previousKey==p1.previousKey &&
		previousKey2==p1.previousKey2 && nextToPlay==p1.nextToPlay &&
		move.getX()==p1.move.getX() && move.getY()==p1.move.getY())
		return true;
	else
		return false;
}

void Node::reset(const BoardStruct& b, int toPlay)
{
	haveBestNode = false;
	depthSearchedTo = 0;
	isScoreCalculated = false;
	isUpperboundCalculated = false;
	isLowerboundCalculated = false;
	lowerbound = (float)INT_MIN;
	upperbound = (float)INT_MAX;

	move = b.getLastMove();

	nextToPlay = toPlay;
	depth = 0;
	terminal = false;
	setKeys(b);
	setBestNode(NULL);
}

void Node::setBestNode(const Node* best)
{
	if(best==NULL){
		bestKey = INT_MIN;
		bestKey2 = INT_MIN;
		bestPreviousKey = INT_MIN;
		bestPreviousKey2 = INT_MIN;
		bestNextToPlay = EMPTY;
		bestX = -1;
		bestY = -1;
		haveBestNode = false;
		return;
	}

	bestKey = best->getKey();
	bestKey2 = best->getKey2();
	bestPreviousKey = best->getPreviousKey();
	bestPreviousKey2 = best->getPreviousKey2();
	bestNextToPlay = best->getNextToPlay();
	bestX = best->getX();
	bestY = best->getY();
	haveBestNode = true;
}
