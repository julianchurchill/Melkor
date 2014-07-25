/**
Notes: The combination of GoString and BoardStruct will keep
	update information about the strings of stones present
	within a Board object, as long as stones are placed as they
	would in a game of Go. e.g. if an occupied point is changed 
	to an empty point, without it being a capture (perhaps the
	programmer just wants to change the position of the stones)
	then the GoString info is not guaranteed to remain uncorrupted.
	To get all the GoString info uptodate after such changes
	run BoardStruct.initStrings()

**/

#pragma warning( disable : 4786 )

#include "globals.h"
#include "GoString.h"
#include "Point.h"
#include "LogWriter.h"
#include "Misc.h"
#include <algorithm>
#include <ctime>

bool GoString::hashValuesInitialised = false;
vector<vector<int> > GoString::HASH;
vector<vector<int> > GoString::HASH2;

GoString& GoString::operator=(const GoString& g)
{
	// watch out for self assignment
	if(this!=&g) {
		points = g.points;
		liberties = g.liberties;
		eyes = g.eyes;

		key = g.key;
		key2 = g.key2;
		ourColour = g.ourColour;
		status = g.status;
	}
	return *this;
}

void GoString::clear()
{
	points.clear();
	liberties.clear();
	eyes.clear();

	key = 0;
	key2 = 0;
	status = UNKNOWN;
}

void GoString::addPoint(int x, int y)
{
	if(!containsPoint(x, y)) {
		points.push_back(Point<>(x, y));
		updateKey(x, y);
	}
}

void GoString::removePoint(int x, int y)
{
	GoString::pointsIterator piter(points.begin());
	for(;piter!=points.end();piter++) {
		if(piter->x==x && piter->y==y) {
			points.erase(piter);
			// remove the point from the key by xor-ing it with the key again
			updateKey(x, y);
			return;
		}
	}
}

/** @brief Return a Zobrist hash function key for this node. The key should
  * represent the contents of this board position present in this node.
  * @return A value representing the Zobrist hash function key for this node. */
int GoString::calculateKey()
{
	// We must initialise the static HASH members only on the
	// first use. It cannot be done in the GoString constructor
	// since the static constructors (line 40 above) are called
	// at the beginning of the main() loop. In this program
	// the GoString constructor is called by static objects at the start
	// of main() and so the HASH values are initialised then. Then
	// they are initialised again as all the static variables are 
	// allocated one-by-one.
	// NOTE: Always initialise static member object variables on their first use!
	if(!hashValuesInitialised)
		initHashValues();

	// return a hash key for this string based on the points in it
	key = 0;
	key2 = 0;
	GoString::constPointsIterator piter(points.begin());
	for(;piter!=points.end();piter++)
	{
		key^=HASH[piter->x][piter->y];
		key2^=HASH2[piter->x][piter->y];
	}
	return key;
}

void GoString::initHashValues()
{
	// seed the random number generator
	if(random_seed()==0) {
		random_seed() = (unsigned)time(NULL);
		srand(random_seed());
	}

	HASH.resize(19);
	HASH2.resize(19);
	for(int i=0;i<19;i++) {
		HASH[i].resize(19);
		HASH2[i].resize(19);
	}
	for(i=0;i<HASH.size();i++) {
		for(int j=0;j<HASH[i].size();j++) {
			HASH[j][i] = rand();
			HASH2[j][i] = rand();
		}
	}
	hashValuesInitialised = true;
}

bool GoString::containsPoint(int x, int y) const
{
	/** @todo Check the hash key to see if this point is in it,
	  * I am sure this is a way to do this with XOR but I can't figure it out. */
	
	// for now just loop through the points
	GoString::constPointsIterator piter(points.begin());
	for(;piter!=points.end();piter++) {
		if(piter->x==x && piter->y==y)
			return true;
	}
	return false;
}

void GoString::printInfo() const
{
	string message;
	char buffer[200];

	// print the number of liberties
	if(ourColour==BLACK)
		LogWriter::printerr("OurColour = BLACK\n", "GoString");
	else
		LogWriter::printerr("OurColour = WHITE\n", "GoString");

	// print key values
	sprintf(buffer, "Key1 : %d\n", key);
	LogWriter::printerr(buffer, "GoString");
	sprintf(buffer, "Key2 : %d\n", key2);
	LogWriter::printerr(buffer, "GoString");

	// print liberties
	sprintf(buffer, "Liberties (%d) :", liberties.size());
	message = buffer;

	GoString::constLibertiesIterator liter(liberties.begin());
	for(;liter!=liberties.end();liter++) {
		sprintf(buffer, " [%d,%d]", liter->x, liter->y);
		message+=buffer;
	}
	LogWriter::printerr(message+"\n", "GoString");

	// print eyes
	sprintf(buffer, "eyes (%d) :", eyes.size());
	message = buffer;

	GoString::constEyesIterator eiter(eyes.begin());
	for(;eiter!=eyes.end();eiter++)
	{
		sprintf(buffer, " [%d,%d]", eiter->x, eiter->y);
		message+=buffer;
	}
	LogWriter::printerr(message+"\n", "GoString");

	// print stones
	sprintf(buffer, "Stones (%d) :", points.size());
	message = buffer;

	// print a list of points contained in this string
	GoString::constPointsIterator piter(points.begin());
	for(;piter!=points.end();piter++)
	{
		sprintf(buffer, " [%d,%d]", piter->x, piter->y);
		message+=buffer;
	}
	LogWriter::printerr(message+"\n", "GoString");

	if(status==ALIVE)
		sprintf(buffer, "Status : ALIVE\n");
	else if(status==DEAD)
		sprintf(buffer, "Status : DEAD\n");
	else if(status==URGENT)
		sprintf(buffer, "Status : URGENT\n");
	else
		sprintf(buffer, "Status : UNKNOWN (%d)\n", status);
	LogWriter::printerr(buffer, "GoString");
}

void GoString::concatString(GoString& s, BoardStruct& b)
{
	// add the points from s into this string
	const GoString::pointsTable& ps = s.getPoints();
	GoString::constPointsIterator piter(ps.begin());
	for(;piter!=ps.end();piter++)
		addPoint(piter->x, piter->y);

	// add the liberties from s to this string
	const GoString::libertiesTable& ls = s.getLibertyPoints();
	GoString::constLibertiesIterator liter(ls.begin());
	for(;liter!=ls.end();liter++)
		addLiberty(Point<>(liter->x, liter->y), b);
}

// add a liberty point to the list if not in already
// and check if it's an eye too
void GoString::addLiberty(Point<>& p, BoardStruct& b)
{
	GoString::constLibertiesIterator liter(liberties.begin());
	for(;liter!=liberties.end();liter++) {
		if(liter->x==p.x && liter->y==p.y)
			return;
	}
	liberties.push_back(p);
	eyeCheck(p, b);
}

// remove the point from the list if it is there
// and from the eye list too if necessary
void GoString::removeLiberty(Point<>& p)
{
	GoString::libertiesIterator liter(liberties.begin());
	for(;liter!=liberties.end();liter++) {
		if(liter->x==p.x && liter->y==p.y) {
			liberties.erase(liter);
			return;
		}
	}
	removeEye(p);
}

bool GoString::containsLiberty(const Point<>& p) const
{
	// for now just loop through the points
	GoString::constLibertiesIterator piter(liberties.begin());
	for(;piter!=liberties.end();piter++) {
		if(piter->x==p.x && piter->y==p.y)
			return true;
	}
	return false;
}

// is this liberty a real eye
bool GoString::eyeCheck(Point<>& p, BoardStruct& b)
{
	int size = b.getSize();
	int dsize = size-1;

	// check for real eyes:
	// add to eye list if the liberty is:
	// in the corner and surrounded by 3 of our stones
	if((p.x==0 || p.x==dsize) && (p.y==0 || p.y==dsize )) {
		int dsize2 = size-2;
		// which corner are we in?
		// top left
		if(p.x==0 && p.y==0) {
			if(b.getPoint(0, 1)!=ourColour || b.getPoint(1, 1)!=ourColour ||
			   b.getPoint(1, 0)!=ourColour)
				return false;
		}
		// bottom left
		else if(p.x==0 && p.y==dsize ) {
			if(b.getPoint(0, dsize2)!=ourColour || b.getPoint(1, dsize2)!=ourColour ||
			   b.getPoint(1, dsize)!=ourColour)
				return false;
		}
		// top right
		else if(p.x==dsize && p.y==0) {
			if(b.getPoint(dsize, 1)!=ourColour || b.getPoint(dsize2, 1)!=ourColour ||
			   b.getPoint(dsize2, 0)!=ourColour)
				return false;
		}
		// bottom right
		else {
			if(b.getPoint(dsize, dsize2)!=ourColour || b.getPoint(dsize2, dsize2)!=ourColour ||
			   b.getPoint(dsize2, dsize)!=ourColour)
				return false;
		}
		// if we haven't returned yet then add this liberty to the eye list
		addEye(p);
	}
	// at the edge and surrounded by 5 of our stones
	else if(p.x==0 || p.x==dsize || p.y==0 || p.y==dsize) {
		// which edge are we on?
		// left edge
		if(p.x==0) {
			if(b.getPoint(0, p.y-1)!=ourColour || b.getPoint(1, p.y-1)!=ourColour ||
			   b.getPoint(1, p.y)!=ourColour || b.getPoint(1, p.y+1)!=ourColour ||
			   b.getPoint(0, p.y+1)!=ourColour)
				return false;
		}
		// right edge
		else if(p.x==dsize) {
			if(b.getPoint(p.x, p.y-1)!=ourColour || b.getPoint(p.x-1, p.y-1)!=ourColour ||
			   b.getPoint(p.x-1, p.y)!=ourColour || b.getPoint(p.x-1, p.y+1)!=ourColour ||
			   b.getPoint(p.x, p.y+1)!=ourColour)
				return false;
		}
		// top edge
		else if(p.y==0) {
			if(b.getPoint(p.x-1, 0)!=ourColour || b.getPoint(p.x-1, 1)!=ourColour ||
			   b.getPoint(p.x, 1)!=ourColour || b.getPoint(p.x+1, 1)!=ourColour ||
			   b.getPoint(p.x+1, 0)!=ourColour)
				return false;
		}
		// bottom edge
		else if(p.y==dsize) {
			if(b.getPoint(p.x-1, p.y)!=ourColour || b.getPoint(p.x-1, p.y-1)!=ourColour ||
			   b.getPoint(p.x, p.y-1)!=ourColour || b.getPoint(p.x+1, p.y-1)!=ourColour ||
			   b.getPoint(p.x+1, p.y)!=ourColour)
				return false;
		}

		// if we haven't returned yet then add this liberty to the eye list
		addEye(p);
	}
	// in the middle somewhere and surrounded by 7 or 8 of our stones
	// where only the diagonals can be empty and only 1 of them
	else {
		// if any of the n,s,e,w points aren't ours this is not a real eye
		if(b.getPoint(p.x, p.y-1)!=ourColour || b.getPoint(p.x, p.y+1)!=ourColour ||
		   b.getPoint(p.x-1, p.y)!=ourColour || b.getPoint(p.x+1, p.y)!=ourColour)
			return false;
		// if two or more diagonals are empty this is not a real eye
		int count = 0;
		if(b.getPoint(p.x-1, p.y-1)!=ourColour) count++;
		if(b.getPoint(p.x-1, p.y+1)!=ourColour) count++;
		if(b.getPoint(p.x+1, p.y-1)!=ourColour) count++;
		if(b.getPoint(p.x+1, p.y+1)!=ourColour) count++;
		if(count>1) return false;

		// if we haven't returned yet then add this liberty to the eye list
		addEye(p);		
	}
	return true;
}

void GoString::addEye(Point<>& p)
{
	GoString::constEyesIterator eiter = eyes.begin();
	for(;eiter!=eyes.end();eiter++)
	{
		if(eiter->x==p.x && eiter->y==p.y)
			return;
	}
	eyes.push_back(p);
}

void GoString::removeEye(Point<>& p)
{
	GoString::eyesIterator eiter = eyes.begin();
	for(;eiter!=eyes.end();eiter++)
	{
		if(eiter->x==p.x && eiter->y==p.y)
		{
			eyes.erase(eiter);
			return;
		}
	}
}
