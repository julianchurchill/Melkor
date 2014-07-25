#pragma warning( disable : 4786 )

#include "globals.h"
#include "SGFNode.h"
#include "LogWriter.h"
#include <cstring>
//using namespace std;

const string SGFNode::myName = "SGFNode";

SGFNode::SGFNode()
{
	parent = NULL;
	child = NULL;
	sibling = NULL;
}

SGFNode::SGFNode(const SGFNode& n)
{
	parent = NULL;
	child = NULL;
	sibling = NULL;
	if(n.child!=NULL)
		addChild(*(n.child));
	properties = n.properties;
}

SGFNode& SGFNode::operator=(const SGFNode& n)
{
	if(this!=&n) {
		parent = NULL;
		child = NULL;
		sibling = NULL;
		if(n.child!=NULL)
			addChild(*(n.child));
		properties = n.properties;
	}
	return *this;
}

SGFNode::~SGFNode()
{
	freeChildren();
}

/** @brief Deallocate all children of this node. */
void SGFNode::freeChildren()
{
	// Delete all child pointers recursively
	if(child!=NULL) {
		// Delete the child's siblings first
		child->freeSiblings();
		delete child;
		child = NULL;
	}
}

/** @brief Recursively call the destructor on this nodes siblings. */
void SGFNode::freeSiblings()
{
	if(sibling!=NULL) {
		sibling->freeSiblings();
		delete sibling;
		sibling = NULL;
	}
}

bool SGFNode::operator==(const SGFNode& p1)
{
	// Only base comparison on the properties
	// Any node finding operation is the repsonsibility of the Tree structure
	// which should compare parent/child relations as well, don't do that here!
	if(properties==p1.properties)
		return true;
	else
		return false;
}

/** @brief Add a child node to this node.
  * @param n The new child of this node to add. */
void SGFNode::addChild(const SGFNode& n)
{
	// Allocate new memory for this child
	// and copy n to our newly allocated child
	if(child!=NULL)
		delete child;
	child = new SGFNode(n);

	// Set child->parent to this
	child->setParent(this);

	// Copy siblings of n as new siblings of child
	child->addSiblings(n);
}

/** @brief Recursively add the siblings of n as siblings of this node. */
void SGFNode::addSiblings(const SGFNode& n)
{
	if(n.getSibling()==NULL)
		return;
	sibling = new SGFNode(*(n.getSibling()));

	// Siblings must also have a parent pointer otherwise we can't trace upwards
	sibling->setParent(this);
}

/** @brief Remove a specified child from amongst this nodes child and it's siblings. 
  * @param n The node to match amongst this nodes children. */
void SGFNode::removeChild(const SGFNode& n)
{
	// Search through child and it's siblings for the node n
	// and remove it from the chain
	SGFNode* currentChild = child;
	SGFNode* previousSibling;
	while(currentChild!=NULL) {
		if(*currentChild == n) {
			SGFNode* nextSibling = currentChild->sibling;
			if(currentChild==child)
				child = nextSibling;
			else
				previousSibling->sibling = nextSibling;
			delete currentChild;
			break;
		}
		previousSibling = currentChild;
		currentChild = currentChild->sibling;
	}
}

void SGFNode::getLookaheadMoves(int number, int colour, vector<Move>& moves)
{
	moves.clear();
	SGFNode* nextNode = this;
	
	// string rep of the move
	string m;
	vector<string> v;
	Move temp;

	// string representation of the colour moving		
	string col;
	if(colour==BLACK) {
		col = "B";
		if(!getBlackMove(temp)) return;
	}
	else {
		col = "W";
		if(!getWhiteMove(temp)) return;
	}

	// add this nodes move as the first move
	//Move temp(x, y);
	moves.push_back(temp);
	
	// start at 1 because we've already added this as the first move
	int i=1;
	
	while(i<number) {
		if(nextNode->nodeAvailable()==false) break;
		nextNode = nextNode->getChild();
		if(col=="B") {
			if(!nextNode->getBlackMove(temp)) continue;
		}
		else if(col=="W") {
			if(!nextNode->getWhiteMove(temp)) continue;
		}
		moves.push_back(temp);
		i++;
	}
}

bool SGFNode::nodeAvailable()
{
	return (child!=NULL); 
}

bool SGFNode::makeNode(string sb)
{
	// separate property string
	string propertyString;
	char c='\0';
	int insideSBracket=0;

	// read properties up to first '(' or ';'
	// allowing '(' ')' inside '[' ']'
	for(int i=0;i<sb.length();i++) {
		c = sb.at(i);
		if(!insideSBracket && c=='[') insideSBracket++;
		else if(c==']' && (!(i>0&&sb.at(i-1)=='\\'))) insideSBracket--;
		if (insideSBracket==0) {
			if (c=='(' || c==';' || c==')') {
				propertyString = sb.substr(0, i);
				sb.erase(0, i);
				break;
			}
		}
	}
	
	// extract properties
	if(!extractProperties(propertyString)) {
		string message = "Bad node: ";
		message+=propertyString;
		message+="\n";
		LogWriter::printerr(message, "SGF");
		return false;
	}

/*	SGFNode* node;
	try {
		node = new SGFNode();
	} catch(exception& e) {
		cerr << e.what();
		return false;
	}*/
	SGFNode node;

	if (c=='(') {
		string pb;
		int bcount, i;
		char b;
		
		while (c=='(') {
			// remove up to matching ')' from sb 
			// and put into pb
			bcount = 0;
			insideSBracket = 0;
			
			for(i=0;i<sb.length();i++) {
				// WARNING: Don't count '(' or ')' inside comments!
				// and check for escaped ']' i.e. "C[This is some comment \](blah blah) ]"
				b = sb.at(i);
				if(!insideSBracket && b=='[') 
					insideSBracket++;
				else if(b==']' && (!(i>0&&sb.at(i-1)=='\\'))) {
					insideSBracket--;
					// error check
					if(insideSBracket<0) {
						// The stupid program that wrote this sgf 
						// has not escaped a ']' inside a comment
						insideSBracket = 0;
						// NOTE: We should really just quit parsing this sgf
						// and say's it's got an invalid node
						string message = "Bad node (mismatched brackets '[', ']'): ";
						message+=sb.substr(0, i);
						message+="\n";
						LogWriter::printerr(message, "SGF");
						return false;
					}
				}
				if(!insideSBracket) {
					if(b=='(')
						bcount++;
					else if(b==')' && bcount<=1)
						break;
					else if(b==')')
						bcount--;
				}
			}
			
			// remove '(;' from start by starting after it
			//pb = *(new string(sb.substr(2, i).trim()));
			pb = string(sb.substr(2, i));
			sb.erase(0, i);
		
			if (pb.length()>0) {
				// run make node
				if(!node.makeNode(pb))
					return false;

				// add as child
				addChild(node);
			}
			
			// see whats next
			c = sb.at(0);
		}
	}
	else if (c==';') {
		// copy all sb from after ';' to end
		string pb = string(sb.substr(1));
		//pb.deleteCharAt(pb.length()-1);
		
		if (pb.length()>0) {
			// make sure this isn't the end of a variation
			// or an empty node e.g. '(;W[pp];B[pl];)'
			if(pb.at(0)!=')') {
				// run make node
				if(!node.makeNode(pb))
					return false;
				// add as child
				addChild(node);
			}
		}			
	}
	return true;
}

// separate a list of tags and values within a single node, between two ';'
// e.g. AB[10]B[pp]C[this is a comment]
bool SGFNode::extractProperties(string node)
{
	char c;
	vector<string> values;
	string temp;
	string key;
	int i=0;
	while(i<node.length()) {
		// ok must be a tag name then, so read in
		values.clear();
		
		// remove whitespace
		while(i<node.length() && ((c=node.at(i))=='\n' || c==' ' || c=='\t' || c=='\r'))
			node.erase(i, 1);

		// nothing here so return
		if(i>=node.length())
			return true;

		temp = "";

		// read in tag name, upto '['
		while(i<node.length() && (c=node.at(i))!='[') {
			temp.append(1, c);
			i++;
		}

		// if reached end of string
		// something wrong with this node
		// so quit
		if(i>=node.length())
			return false;

		key = temp.substr(0, temp.size());
		
		// continue if we have another value waiting
		// denoted by '[' upto ']'
		while (i<node.length() && (c=node.at(i)=='[')) {
			// step past '['
			i++;
	
			temp = "";

			while(i<node.length() && ((c=node.at(i))!=']' || (i>0 && node.at(i-1)=='\\'))) {
				temp.append(1, c);
				i++;
			}
			// if haven't found matching ']'
			if(i>=node.length())
				return false;
			values.push_back(temp);
			i++;
		}

		// if reached end of string without hitting ']'
		// something wrong with this node
		// so quit
		if(i>=node.length() && c!=']')
			return false;

		setProperty(key, values);
	}
	return true;
}

/////////////////////////////
// property access methods //
/////////////////////////////
	
bool SGFNode::getProperty(string p, vector<string>& props) const
{
	// return "" if no such property key
	set<SGFProperty, less<SGFProperty> >::const_iterator citer;
	//vector<string> v;
	props.clear();
	props.push_back("");
	SGFProperty prop(p, props);
	citer = properties.find(prop);
	if(citer==properties.end()) 
		return false;
	else
	{
		props.clear();
		props = citer->getValues();
	}
	return true;
}

void SGFNode::setProperty(string p, vector<string>& values)
{
	// DON'T do this because of pass moves e.g. B[]
//	if(value.size()==0)
//		return;
	SGFProperty s(p, values);
	// Watch out for properties already in the set
	// if it is then the property value won't be updated unless we force it
	pair<SGFPropertyTable::iterator ,bool> rValue = properties.insert(s);
	if(!rValue.second)
		rValue.first->setValues(values);
}

void SGFNode::setProperty(string p, string value)
{
	// DON'T do this because of pass moves e.g. B[]
//	if(value=="")
//		return;
	SGFProperty s(p, value);
	// Watch out for properties already in the set
	// if it is then the property value won't be updated unless we force it
	pair<SGFPropertyTable::iterator ,bool> rValue = properties.insert(s);
	if(!rValue.second) {
		rValue.first->clearValues();
		rValue.first->addValue(value);
	}
}

/** @brief Print the property information for this node. Recursively look at
  * children also, but only the primary child - siblings are not looked at. */
void SGFNode::printInfo() const
{
	LogWriter log;
	set<SGFProperty, less<SGFProperty> >::const_iterator citer;
	citer = properties.begin();
	while(citer!=properties.end()) {
		log.print(citer->getName());
		log.print(" = ");
		vector<string> v = citer->getValues();
		for(int i=0;i<v.size();i++) {
			log.print(v.at(i));
			log.print(" ");
		}
		log.print("\n");
		citer++;
	}

	/** @todo Print info on siblings also... */
	if(child!=NULL)
		child->printInfo();
}

///////////////////////////////////
// Specific info getting methods //
///////////////////////////////////
	
bool SGFNode::getSingleProperty(const string& prop, string& v) const
{
	vector<string> values;
	if(getProperty(prop, values))
	{
		v = values.at(0);
		return true;
	}
	return false;
}

bool SGFNode::getBlackMove(Move& m) const
{
	string p;
	if(!getSingleProperty(SGFProperty::blackMoveTag, p))
		return false;
	m.setX(Move::SGFToX(p));
	m.setY(Move::SGFToY(p));
	m.setColour(BLACK);
	return true;
}

bool SGFNode::getWhiteMove(Move& m) const
{
	string p;
	if(!getSingleProperty(SGFProperty::whiteMoveTag, p))
		return false;
	m.setX(Move::SGFToX(p));
	m.setY(Move::SGFToY(p));
	m.setColour(WHITE);
	return true;
}

bool SGFNode::getMoveProperty(vector<Move>& m, string prop, int colour) const
{
	vector<string> p;
	if(!getProperty(prop, p))
		return false;
	m.clear();
	Move move;
	for(int i=0;i<p.size();i++) {
		move.setX(Move::SGFToX(p.at(i)));
		move.setY(Move::SGFToY(p.at(i)));
		move.setColour(colour);
		m.push_back(move);
	}
	return true;
}

/** @brief Save this node and recursively call on children. */
void SGFNode::save(ofstream& out)
{
	// start with a semi-colon
	out << ";";
	
	// cycle through properties and write them
	set<SGFProperty, less<SGFProperty> >::iterator iter;
	for(iter=properties.begin();iter!=properties.end();iter++) {
		// skip this tag if we have no values to write with it
		vector<string>& v = iter->getValues();
		if(v.size()>0) {
			out << iter->getName().c_str();
			vector<string>::iterator viter = v.begin();
			for(;viter!=v.end();viter++)
				out << "[" << *viter << "]";
		}
	}

	// Check we have a primary child
	if(child==NULL)
		return;
	// If single child (i.e. child has no siblings) just call save on the child
	else if(child->sibling==NULL)
		child->save(out);
	// Otherwise save the siblings too
	else {
		SGFNode* nextSibling = child;
		while(nextSibling!=NULL) {
			out << "(";      // Variations (multiple node children) must all be enclosed in ()
			nextSibling->save(out);
			out << ")";      // Finish the variation
		}
	}
}
