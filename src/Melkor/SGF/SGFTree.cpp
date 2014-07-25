#pragma warning( disable : 4786 )

#include "SGFTree.h"
#include "LogWriter.h"
#include "Misc.h"
#include "Go.h"

void SGFTree::clear()
{
	parent = NULL;
	freeChildren();
	properties.clear();
}

// pre make node function special to root node of tree	
bool SGFTree::parseSGF(string sb)
{
	// read game information properties
	// should start with '(;<root node stuff>'
	// read off first node from string

	// remove white space first
	while(sb.at(0)==' ' || sb.at(0)=='\n' || sb.at(0)=='\t' || sb.at(0)=='\r')
		sb.erase(0, 1);

	// sgf must start with (;
	if (sb.at(0)=='(')
		sb.erase(0, 1);
	else
		return false;

	// remove white space first
	while(sb.at(0)==' ' || sb.at(0)=='\n' || sb.at(0)=='\t' || sb.at(0)=='\r')
		sb.erase(0, 1);

	if(sb.at(0)==';')
		sb.erase(0, 1);
	else
		return false;

	if(!makeNode(sb)) return false;
	return true;
}

// return all primary variation children until the bottom of the tree
bool SGFTree::getAllPrimaryMoves(vector<Move>& moves) const
{
	const SGFNode* nextNode = this;
	string m;

	moves.clear();

	vector<string> values;
	int x, y;
	Move temp;

	while(nextNode!=NULL) {
		if(nextNode->getProperty(SGFProperty::blackMoveTag, values))
			m = values.at(0);
		else if(nextNode->getProperty(SGFProperty::whiteMoveTag, values))
			m = values.at(0);
		else {
			// No move in this node so skip it and move onto next node
			nextNode = nextNode->getChild();
			continue;
		}
		// watch out for null moves meaning a pass
		if(m=="") {
			Move::getPassMove(temp);
			moves.push_back(temp);
		}
		else {
			x = m.at(0)-'a';
			y = m.at(1)-'a';
			temp.setX(x);
			temp.setY(y);
			moves.push_back(temp);
		}
		nextNode = nextNode->getChild();
	}
	return true;
}

void SGFTree::addHandicapMoves(int handicap, int boardSize)
{
	// if there is a handicap add setup moves
	if(handicap>0) {
		vector<Move> moves;
		Go::getFixedHandicapMoves(moves, handicap, boardSize);

		string m;
		vector<Move>::iterator iter = moves.begin();
		for(;iter!=moves.end();iter++){
			m = Move::xyToSGF(iter->getX(), iter->getY(), boardSize);
			setProperty("AB", m);
		}
	}
}

/** @brief Set the game information provided in the root node of this tree. 
  * @param info The game information. */
void SGFTree::setGameInfo(SGFGameInfo& info)
{
	char buffer[50];

	sprintf(buffer, "%d", info.gameType);
	setProperty(SGFProperty::gameTypeTag, buffer);

	sprintf(buffer, "%d", info.fileFormat);
	setProperty(SGFProperty::fileFormatTag, buffer);

	if(info.application!="")
		setProperty(SGFProperty::applicationTag, info.application);
	if(info.rules!="")
		setProperty(SGFProperty::rulesTag, info.rules);
	if(info.date!="")
		setProperty(SGFProperty::dateTag, info.date);

	if(info.time>0) {
		sprintf(buffer, "%d", info.time);
		setProperty(SGFProperty::timeTag, buffer);
	}
	if(info.overtime!="")
		setProperty(SGFProperty::overtimeTag, info.overtime);

	sprintf(buffer, "%d", info.size);
	setProperty(SGFProperty::sizeTag, buffer);

	if(info.handicap!=0) {
		sprintf(buffer, "%d", info.handicap);
		setProperty(SGFProperty::handicapTag, buffer);
	}

	sprintf(buffer, "%g", info.komi);
	setProperty(SGFProperty::komiTag, buffer);

	if(info.result!="")
		setProperty(SGFProperty::resultTag, info.result);
	if(info.blackPlayer!="")
		setProperty(SGFProperty::blackPlayerTag, info.blackPlayer);
	if(info.whitePlayer!="")
		setProperty(SGFProperty::whitePlayerTag, info.whitePlayer);
	if(info.blackRank!="")
		setProperty(SGFProperty::blackRankTag, info.blackRank);
	if(info.whiteRank!="")
		setProperty(SGFProperty::whiteRankTag, info.whiteRank);
}

/** @brief This function finds all the game info from this SGFTree and
  * returns it as a new object. This function is the brother of setGameInfo().
  * @return An SGFGameInfo object relating to this SGFTree. */
SGFGameInfo SGFTree::getGameInfo() const
{
	SGFGameInfo info;
	info.clear();
	string value;
	if(getSingleProperty(SGFProperty::gameTypeTag, value))
		info.gameType = atoi(value.c_str());
	if(getSingleProperty(SGFProperty::fileFormatTag, value))
		info.fileFormat = atoi(value.c_str());

	if(getSingleProperty(SGFProperty::applicationTag, value))
		info.application = value;
	if(getSingleProperty(SGFProperty::rulesTag, value))
		info.rules = value;
	if(getSingleProperty(SGFProperty::dateTag, value))
		info.date = value;

	if(getSingleProperty(SGFProperty::sizeTag, value))
		info.size = atoi(value.c_str());
	if(getSingleProperty(SGFProperty::handicapTag, value))
		info.handicap = atoi(value.c_str());
	if(getSingleProperty(SGFProperty::komiTag, value))
		info.komi = atof(value.c_str());

	if(getSingleProperty(SGFProperty::resultTag, value))
		info.result = value;
	if(getSingleProperty(SGFProperty::blackPlayerTag, value))
		info.blackPlayer = value;
	if(getSingleProperty(SGFProperty::whitePlayerTag, value))
		info.whitePlayer = value;
	if(getSingleProperty(SGFProperty::blackRankTag, value))
		info.blackRank = value;
	if(getSingleProperty(SGFProperty::whiteRankTag, value))
		info.whiteRank = value;

	return info;
}