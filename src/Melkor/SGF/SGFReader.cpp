#pragma warning( disable : 4786 )

#include "SGFReader.h"
#include "Misc.h"
#include "LogWriter.h"
#include "globals.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
//using namespace std;

SGFReader::SGFReader()
{
//	st = NULL;
	nextToPlay = BLACK;
	current = &st;
	depth = 0;
}

SGFReader::~SGFReader()
{
//	if(st!=NULL)
//		delete st;
//	st = NULL;
	current = NULL;
}

bool SGFReader::getLookaheadMoves(int number, int colour, vector<Move>& moves)
{
	if(current==NULL)
		return false;
	current->getLookaheadMoves(number, colour, moves);
	return true;
}

void SGFReader::rewindToStart()
{
	current = &st;
	depth = 0;
}

SGFNode* SGFReader::peekNextNode()
{
	// return false if at end of file
	if(current==NULL || current->getChild()==NULL)
		return NULL;
	return current->getChild();
}

SGFNode* SGFReader::peekPreviousNode()
{
	// return false if at end of file
	if(current==NULL || current->getParent()==NULL)
		return NULL;
	return current->getParent();
}

SGFNode* SGFReader::getNextNode()
{
	// return false if at end of file
	if(current==NULL || current->getChild()==NULL)
		return NULL;
	// update current pointer
	current = current->getChild();
	depth++;
	return current;
}

SGFNode* SGFReader::getPreviousNode()
{
	// return false if at beginning of file
	if(current==NULL || current->getParent()==NULL)
		return NULL;
	// update current pointer
	current = current->getParent();
	depth--;
	return current;
}

bool SGFReader::peekNextMove(Move& m)
{
	// return false if at end of file
	if(current==NULL || current->getChild()==NULL)
		return false;
	// put info in move m
	return getMove(current->getChild(), m);
}

bool SGFReader::peekPreviousMove(Move& m)
{
	// return false if at end of file
	if(current==NULL || current->getParent()==NULL)
		return false;
	// put info in move m
	// careful - check if we are at root again
	// in which case there is no move here
	// so return true without a move
	if(current->getParent()==&st)
		return true;
	return getMove(current->getParent(), m);
}

bool SGFReader::getNextMove(Move& m)
{
	// return false if at end of file
	if(current==NULL || current->getChild()==NULL)
		return false;
	// update current pointer
	current = current->getChild();
	depth++;
	// put info in move m
	return getMove(current, m);

}

bool SGFReader::getPreviousMove(Move& m)
{
	// return false if at beginning of file
	if(current==NULL || current->getParent()==NULL)
		return false;
	// update current pointer
	current = current->getParent();
	depth--;
	// careful - check if we are at root again
	// in which case there is no move here
	// so return true without a move
	if(current==&st)
		return true;
	return getMove(current, m);
}

bool SGFReader::getMove(SGFNode* p, Move& m)
{
	// put info in move m
	string bs;
	if(!getBoardSize(bs))
		m.setBoardSize(19);
	else
		m.setBoardSize(atoi(bs.c_str()));
	vector<string> values;
	if(p->getProperty(SGFProperty::blackMoveTag, values))	{
		if(values.at(0)=="" || values.at(0)=="tt")
			Move::getPassMove(m);
		else {
			m.setX(Move::SGFToX(values.at(0)));
			m.setY(Move::SGFToY(values.at(0)));
		}
		m.setColour(BLACK);
	}
	else if(p->getProperty(SGFProperty::whiteMoveTag, values)) {
		if(values.at(0)=="" || values.at(0)=="tt")
			Move::getPassMove(m);
		else {
			m.setX(Move::SGFToX(values.at(0)));
			m.setY(Move::SGFToY(values.at(0)));
		}
		m.setColour(WHITE);
	}
	else
		return false;
	return true;
}

SGFNode& SGFReader::getRootNode()
{
	return st;
}	

SGFTree& SGFReader::getTree()
{
	return st;
}

/** @brief Add a child node to the current node using the specified move. 
  * Overwrite any other children of current for now until we can set up proper tree editing for sgfs. 
  * @param Move The move to add as a node to the sgf. */
void SGFReader::addMoveNode(Move& m)
{
	SGFNode node;

	if(m.getColour()==BLACK)
		node.setProperty(SGFProperty::blackMoveTag, Move::xyToSGF(m.getX(), m.getY(), m.getBoardSize()));
	else
		node.setProperty(SGFProperty::whiteMoveTag, Move::xyToSGF(m.getX(), m.getY(), m.getBoardSize()));

	// This is a failsafe - normally current shouldn't ever be null
	if(current!=NULL) {
		current->freeChildren();
		current->addChild(node);

		// Now update the current point and counters
		current = current->getChild();
		depth++;
	}
}

/** @brief Add a child node to the current node using the specified board. 
  * Overwrite any other children of current for now until we can set up proper tree editing for sgfs. 
  * @param board The board position to create a new node from for the sgf. */
void SGFReader::addSetupNode(const BoardStruct& board)
{
	SGFNode node;

	// This is a failsafe - normally current shouldn't ever be null
	if(current!=NULL) {
		// Add the board points as a series of tags (AB,AW,AE) to node
		// only record a property if a point has changed from the previous node
		
		// Add stones as properties if we have no parent to compare with
		if(current->getParent()==NULL) {
			for(int i=0;i<board.getSize();i++) {
				for(int j=0;j<board.getSize();j++) {
					if(board.getPoint(j, i)==BLACK)
						node.setProperty(SGFProperty::blackSetupTag, Move::xyToSGF(j, i, board.getSize()));
					else if(board.getPoint(j, i)==WHITE)
						node.setProperty(SGFProperty::whiteSetupTag, Move::xyToSGF(j, i, board.getSize()));
				}
			}
		}
		// Otherwise compare with the parent node and only record differences
		else {
			// Make a parent board from the beginning of the SGF to here
			vector<SGFNode*> previousNodes;
			SGFNode* parentNode = current->getParent();
			while(parentNode!=&st) {
				previousNodes.push_back(parentNode);
				parentNode = parentNode->getParent();
			}

			// Reverse the nodes so they're in the correct forward order
			reverse(previousNodes.begin(), previousNodes.end());

			BoardStruct parentBoard;
			for(int k=0;k<previousNodes.size();k++)
				addSGFNodeToBoard(NULL, *this, parentBoard, previousNodes[k]);

			for(int i=0;i<board.getSize();i++) {
				for(int j=0;j<board.getSize();j++) {
					if(parentBoard.getPoint(j, i)!=board.getPoint(j, i)) {
						if(board.getPoint(j, i)==BLACK)
							node.setProperty(SGFProperty::blackSetupTag, Move::xyToSGF(j, i, board.getSize()));
						else if(board.getPoint(j, i)==WHITE)
							node.setProperty(SGFProperty::whiteSetupTag, Move::xyToSGF(j, i, board.getSize()));
						else if(board.getPoint(j, i)==EMPTY)
							node.setProperty(SGFProperty::emptySetupTag, Move::xyToSGF(j, i, board.getSize()));
					}
				}
			}
		}

		// Store the new node
		current->freeChildren();
		current->addChild(node);
	}
}

/** @brief Remove the last node added to this sgf. */
void SGFReader::removeNode()
{
	SGFNode* nodeToRemove = current;
	// If getPreviousNode() returns a pointer to a valid node
	// then current will have been updated to point at that previous node
	if(getPreviousNode()!=NULL)
		current->removeChild(*nodeToRemove);
}

// load the file 'filename'
bool SGFReader::load(string f)
{
	// create file handle
	ifstream in(f.c_str());
	
	if(!in)	{
		LogWriter log;
		string message = "SGFReader: Could not open file: ";
		message+=f;
		log.println(message);
		return false;
	}

	filename = "";
	st.clear();

	string sb;
	while(in)
		sb.append(1, (char)in.get());
	
	// fill SGFTree object with the data
	if(!st.parseSGF(sb))
		return false;
	current = &st;
	filename = f;
	depth = 0;
	return true;
}

// print info on the SGF file
void SGFReader::printInfo() const
{
	LogWriter log;
	
	// print out hashtable of properties
	string message = "Signature: ";
	message+=getSignature();
	log.println(message);
	st.printInfo();
	
}

string SGFReader::getSignature() const
{
	// moves 20, 40, 60
	string A = "";
	// moves 31, 51, 71
	string B = "";

	// walk through the game picking up the move 'aa' values
	vector<Move> m;
	if(!st.getAllPrimaryMoves(m))
		return "";
	if(m.size()>=20) {
		A+=m[19].getX()+'a';
		A+=m[19].getY()+'a';
	}
	else
		A+="??";
	if(m.size()>=31) {
		B+=m[30].getX()+'a';
		B+=m[30].getY()+'a';
	}
	else
		B+="??";
	if(m.size()>=40) {
		A+=m[39].getX()+'a';
		A+=m[39].getY()+'a';
	}
	else
		A+="??";
	if(m.size()>=51) {
		B+=m[50].getX()+'a';
		B+=m[50].getY()+'a';
	}
	else
		B+="??";
	if(m.size()>=60) {
		A+=m[59].getX()+'a';
		A+=m[59].getY()+'a';
	}
	else
		A+="??";
	if(m.size()>=71) {
		B+=m[70].getX()+'a';
		B+=m[70].getY()+'a';
	}
	else
		B+="??";

	return A+B;
}

////////////////////////////////////////
// Specific game info getting methods //
//                                    //
// A few convenience methods are      //
// provided, but there are many more  //
// possible properties, see the spec. //
////////////////////////////////////////

bool SGFReader::getSingleProperty(string& prop, string& v) const
{
//	if(st==NULL)
//		return false;
	vector<string> values;
	if(st.getProperty(prop, values)) {
		v = values.at(0);
		return true;
	}
	return false;
}

bool SGFReader::getKomi(float& f) const 
{
	string v;
	if(getSingleProperty("KM", v)) {
		f = atof(v.c_str());
		return true;
	}
	return false;
}

bool SGFReader::getHandicap(int& f) const 
{
	string v;
	if(getSingleProperty("HA", v)) {
		f = atoi(v.c_str());
		return true;
	}
	return false;
}
