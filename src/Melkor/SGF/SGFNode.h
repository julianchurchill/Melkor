#ifndef SGFNODE_H
#define SGFNODE_H

#pragma warning( disable : 4786 )

#include "Move.h"
#include "SGFProperty.h"
#include "globals.h"
#include <string>
#include <vector>
#include <set>
#include <fstream>
using namespace std;

class SGFNode
{
	protected:

		static const string myName;

		// Only ever have one parent!
		SGFNode* parent;
		// Only have one child - in the future the sibling pointer will give access to multiple children
		// We do this instead of a children* because it may help to keep track of our navigation
		// through the sgf tree later if we have a separate primary child - (which we can swap round with
		// other siblings to maintain our primary variation)
		SGFNode* child;
		SGFNode* sibling;          // Pointer to the next sibling of this node

		// various bits of info about this node
		typedef set<SGFProperty, less<SGFProperty> > SGFPropertyTable;
		SGFPropertyTable properties;
		bool makeNode(string sb);
		bool extractProperties(string node);

	public:

		SGFNode();
		SGFNode(const SGFNode& n);
		SGFNode& operator=(const SGFNode& n);
		virtual ~SGFNode();
		bool operator==(const SGFNode& p1);

		void freeChildren();
		void freeSiblings();

		void setParent(SGFNode* p) { parent = p; };
		SGFNode* getParent() { return parent; };
		SGFNode* getChild() { return child; };
		const SGFNode* getChild() const { return child; };
		const SGFNode* getSibling() const { return sibling; };
		bool nodeAvailable();

		void addChild(const SGFNode& n);
		void addSiblings(const SGFNode& n);
		void removeChild(const SGFNode& n);

		void getLookaheadMoves(int number, int colour, vector<Move>& moves);

		bool getProperty(string p, vector<string>& props) const;
		void setProperty(string p, vector<string>& value);
		void setProperty(string p, string value);
		void printInfo() const;
		bool getSingleProperty(const string& prop, string& v) const;
		bool getSingleProperty(const char* prop, string& v) const { string t(prop); return getSingleProperty(t, v); };
		bool getComment(string& c) const { return getSingleProperty(SGFProperty::commentTag, c); };
		bool getName(string& c) const { return getSingleProperty(SGFProperty::nameTag, c); };
		bool getMoveProperty(vector<Move>& m, string prop, int colour) const;
		bool getBlackSetup(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::blackSetupTag, BLACK); };
		bool getWhiteSetup(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::whiteSetupTag, WHITE); };
		bool getEmptySetup(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::emptySetupTag, EMPTY); };
		bool getMarkers(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::markerTag, SGF_CROSS); };
		bool getCircles(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::circleTag, SGF_CIRCLE); };
		bool getSquares(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::squareTag, SGF_SQUARE); };
		bool getTriangles(vector<Move>& props) const {
			return getMoveProperty(props, SGFProperty::triangleTag, SGF_TRIANGLE);
		}
		bool getBlackTerritory(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::blackTerritoryTag, EMPTY); };
		bool getWhiteTerritory(vector<Move>& props) const { return getMoveProperty(props, SGFProperty::whiteTerritoryTag, EMPTY); };

		bool getBlackMove(Move& m) const;
		bool getWhiteMove(Move& m) const;

		void save(ofstream& out);
};

#endif