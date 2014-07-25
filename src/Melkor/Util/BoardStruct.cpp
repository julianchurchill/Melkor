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
#include "BoardStruct.h"
#include "LogWriter.h"
#include "Misc.h"
#include <algorithm>
#include <ctime>

bool BoardStruct::hashValuesInitialised = false;
vector<vector<int> > BoardStruct::B;
vector<vector<int> > BoardStruct::W;
vector<vector<int> > BoardStruct::KO;
vector<vector<int> > BoardStruct::B2;
vector<vector<int> > BoardStruct::W2;
vector<vector<int> > BoardStruct::KO2;

const string BoardStruct::myName = "BoardStruct";

#ifdef USE_BIT_BOARDS
#error Change all references to contents.getValue() or contents.setValue()
#error to contentsGetValue() and contentsSetValue()
#endif

#ifdef USE_BIT_BOARDS
BoardStruct::BoardStruct(int s /*=19*/, int oc /*=BLACK*/) : lastMove(-1, -1, EMPTY, s), kx(-1), ky(-1),
	key(0), key2(0), previousKey(0), previousKey2(0), blackStonesCaptured(0), whiteStonesCaptured(0), size(s),
	useLastMove(false)
#else
BoardStruct::BoardStruct(int s /*=19*/, int oc /*=BLACK*/) : contents(s, s, EMPTY), lastMove(-1, -1, EMPTY, s),
    kx(-1), ky(-1),	key(0), key2(0), previousKey(0), previousKey2(0), blackStonesCaptured(0), whiteStonesCaptured(0), size(s),
	useLastMove(false)
#endif
{
//	if(ourColour==BLACK)
//		theirColour = WHITE;
//	else
//		theirColour = BLACK;

#ifdef USE_HASHTABLE_FOR_STRINGS
	// Set the size of the strings table according to the size of the board
	// The maximum number of possible strings given size is:
	//           (size*size)/2   (+1 if size is odd)
	// Size must be set here otherwise when the strings object grows the
	// goStringPtrs will be invalidated!
	strings.resize((size*size)/2 + size%2);
#endif

#ifdef USE_BIT_BOARDS
#error Allocate memory for the bit boards based on the board size and set all to 0
	// Round the mask upto a multiple of 32 bits just like the bit boards
	bitBoardSize = 32 - ((size*size) % 32) + (size*size);
#endif

	initStrings();
}

#ifdef USE_BIT_BOARDS
BoardStruct::BoardStruct(const vector<vector<char> >& b, int s, int oc /*=BLACK*/) 
	: lastMove(-1, -1), size(s), useLastMove(false),
	kx(-1), ky(-1), key(0), key2(0), previousKey(0), previousKey2(0), blackStonesCaptured(0), whiteStonesCaptured(0)
#else
BoardStruct::BoardStruct(const vector<vector<char> >& b, int s, int oc /*=BLACK*/) 
	: contents(b, b.size(), b.size()), lastMove(-1, -1, EMPTY, s), size(s), useLastMove(false),
	kx(-1), ky(-1), key(0), key2(0), previousKey(0), previousKey2(0), blackStonesCaptured(0), whiteStonesCaptured(0)
#endif
{
//	if(ourColour==BLACK)
//		theirColour = WHITE;
//	else
//		theirColour = BLACK;

#ifdef USE_HASHTABLE_FOR_STRINGS
	// Set the size of the strings table according to the size of the board
	// The maximum number of possible strings given size is:
	//           (size*size)/2   (+1 if size is odd)
	// Size must be set here otherwise when the strings object grows the
	// goStringPtrs will be invalidated!
	strings.resize((size*size)/2 + size%2);
#endif

#ifdef USE_BIT_BOARDS
#error Allocate memory for the bit boards based on the board size and copy b
	// Round the mask upto a multiple of 32 bits just like the bit boards
	bitBoardSize = 32 - ((size*size) % 32) + (size*size);
#endif

	initStrings();
	calculateKey();
}

#ifdef USE_BIT_BOARDS
#error this function uses contentsType - is this function really needed???
BoardStruct::BoardStruct(const contentsType& b, int s, int oc /*=BLACK*/)
	: lastMove(-1, -1, EMPTY, s), size(s), useLastMove(false), kx(-1), ky(-1), key(0),
	key2(0), previousKey(0), previousKey2(0), blackStonesCaptured(0), whiteStonesCaptured(0)
#else
BoardStruct::BoardStruct(const contentsType& b, int s, int oc /*=BLACK*/)
	: contents(b), lastMove(-1, -1, EMPTY, s), size(s), useLastMove(false), kx(-1), ky(-1), key(0),
	key2(0), previousKey(0), previousKey2(0), blackStonesCaptured(0), whiteStonesCaptured(0)
#endif
{
//	if(ourColour==BLACK)
//		theirColour = WHITE;
//	else
//		theirColour = BLACK;

#ifdef USE_HASHTABLE_FOR_STRINGS
	// Set the size of the strings table according to the size of the board
	// The maximum number of possible strings given size is:
	//           (size*size)/2   (+1 if size is odd)
	// Size must be set here otherwise when the strings object grows the
	// goStringPtrs will be invalidated!
	strings.resize((size*size)/2 + size%2);
#endif

#ifdef USE_BIT_BOARDS
#error Allocate memory for the bit boards based on the board size and copy b
	// Round the mask upto a multiple of 32 bits just like the bit boards
	bitBoardSize = 32 - ((size*size) % 32) + (size*size);
#endif

	initStrings();
	calculateKey();
}

#ifdef USE_BIT_BOARDS
BoardStruct::BoardStruct(const BoardStruct& g)
	: strings(g.strings), lastMove(g.lastMove), size(g.size), useLastMove(g.useLastMove),
	key(g.key), key2(g.key2), previousKey(g.previousKey), previousKey2(g.previousKey2), kx(g.kx), ky(g.ky),
	blackStonesCaptured(g.blackStonesCaptured), whiteStonesCaptured(g.whiteStonesCaptured), illegalMoveMemory(g.illegalMoveMemory)
{
#error delete old and allocate new bitboards and copy them - use memcpy()
#error only redo memory de/allocation if size differs
	// Round the mask upto a multiple of 32 bits just like the bit boards
	bitBoardSize = 32 - ((size*size) % 32) + (size*size);
}
#else
BoardStruct::BoardStruct(const BoardStruct& g)
	: contents(g.contents), strings(g.strings), lastMove(g.lastMove), size(g.size), useLastMove(g.useLastMove),
	key(g.key), key2(g.key2), previousKey(g.previousKey), previousKey2(g.previousKey2), kx(g.kx), ky(g.ky),
	blackStonesCaptured(g.blackStonesCaptured), whiteStonesCaptured(g.whiteStonesCaptured), illegalMoveMemory(g.illegalMoveMemory)
{
}
#endif

BoardStruct& BoardStruct::operator=(const BoardStruct& g)
{
	if(this!=&g)
	{
		size = g.size;
//		ourColour = g.ourColour;
//		theirColour = g.theirColour;
		useLastMove = g.useLastMove;

#ifdef USE_BIT_BOARDS
#error delete old and allocate new bitboards and copy them - use memcpy()
#error only redo memory de/allocation if size differs
	// Round the mask upto a multiple of 32 bits just like the bit boards
	bitBoardSize = 32 - ((size*size) % 32) + (size*size);
#else
		contents = g.contents;
#endif
		strings = g.strings;
		key = g.key;
		key2 = g.key2;
		previousKey = g.previousKey;
		previousKey2 = g.previousKey2;
		kx = g.kx;
		ky = g.ky;
		blackStonesCaptured = g.blackStonesCaptured;
		whiteStonesCaptured = g.whiteStonesCaptured;

		lastMove = g.lastMove;
		illegalMoveMemory = g.illegalMoveMemory;
	}
	return *this;
}

bool BoardStruct::operator==(const BoardStruct& b)
{
#ifdef USE_GOSTRINGPTRS
#error == needs to be handled specially for GoString ptrs
#endif
#ifdef USE_BIT_BOARDS
#error == needs to be handled specially for bit boards
#endif

	if(size==b.size && contents==b.contents && strings==b.strings && key==b.key && key2==b.key2 && previousKey==b.previousKey &&
		previousKey2==b.previousKey2 && kx==b.kx && ky==b.ky && blackStonesCaptured==b.blackStonesCaptured &&
		whiteStonesCaptured==b.whiteStonesCaptured && lastMove==b.lastMove && illegalMoveMemory==b.illegalMoveMemory)
		return true;
	return false;
}

/** @todo The values rand() returns are from 0 to RAND_MAX.
  * RAND_MAX has a value of 0x7fff which means these values
  * are 15 bit values - we need at least 32 bit values for
  * Zobrist hashing so fill up the other 16 bits of each 
  * B[], W[], KO[], B2[], W2[], KO2[]. The more bits the
  * less chance of key collisions. */
void BoardStruct::initHashValues()
{
	// seed the random number generator
	if(random_seed()==0)
	{
		random_seed() = (unsigned)time(NULL);
		srand(random_seed());
	}

	B.resize(19);
	W.resize(19);
	KO.resize(19);
	B2.resize(19);
	W2.resize(19);
	KO2.resize(19);
	for(int i=0;i<19;i++)
	{
		B[i].resize(19);
		W[i].resize(19);
		KO[i].resize(19);
		B2[i].resize(19);
		W2[i].resize(19);
		KO2[i].resize(19);
	}
	for(i=0;i<B.size();i++)
	{
		for(int j=0;j<B[i].size();j++)
		{
			B[j][i] = rand();
			W[j][i] = rand();
			KO[j][i] = rand();
			B2[j][i] = rand();
			W2[j][i] = rand();
			KO2[j][i] = rand();
		}
	}
	hashValuesInitialised = true;
}

/* Function for updating zobrist hash keys.
*/
BoardStruct::key_type BoardStruct::updateZobristHashKey(int& key1, int& key2, int previousColour, int nowColour, int j, int i)
{
	// We must initialise the static HASH members only on the
	// first use. It cannot be done in the BoardStruct constructor
	// since the static constructors (line 40 above) are called
	// at the beginning of the main() loop. In this program
	// other constructors called by static objects at the start
	// of main() might initialise BoardStruct before the static constructors
	// are called and so the HASH values are initialised then. Then
	// they are initialised again as all the static variables are 
	// allocated one-by-one.
	// NOTE: Always initialise static member object variables on their first use!
	if(!hashValuesInitialised)
		initHashValues();
	if(previousColour==BLACK)
	{
		key1^=B[j][i];
		key2^=B2[j][i];
	}
	else if(previousColour==WHITE)
	{
		key1^=W[j][i];
		key2^=W2[j][i];
	}

	if(nowColour==BLACK)
	{
		key1^=B[j][i];
		key2^=B2[j][i];
	}
	else if(nowColour==WHITE)
	{
		key1^=W[j][i];
		key2^=W2[j][i];
	}
	return key1;
}

BoardStruct::key_type BoardStruct::updateKey(int previousColour, int nowColour, int j, int i)
{
	// special check for strange bug
	// which doesn't initialise static HASH values
//	if(B.size()==0)
//	{
//		initHashValues();
//	}
	// The above bug was caused by the order of static constructor calling
	// see updateZobristHashKey()
	return updateZobristHashKey(key, key2, previousColour, nowColour, j, i);
}

/** Return a Zobrist hash function key for this node. The key should
represent the contents of this board position present in this node.
@return A value representing the Zobrist hash function key for
this node. */
int BoardStruct::calculateKey()
{
	if(!hashValuesInitialised)
		initHashValues();

	// create key if necessary
	int result=0;
	int result2=0;
	for(int i=0;i<size;i++)
	{
		for(int j=0;j<size;j++)
		{
			if(getPoint(j, i)==BLACK)
			{
				result^=B[j][i];
				result2^=B2[j][i];
			}
			else if (getPoint(j, i)==WHITE)
			{
				result^=W[j][i];
				result2^=W2[j][i];
			}
		}
	}

	// NOTE: I don't think this is good since can produce
	// 2 board states with same configuration (but different
	// Ko points) which are visibly the same (so would violate
	// Super-Ko) but with different keys (so Super-Ko would 
	// not be detected by this program).

	// check for a ko point
//	if(kx!=-1 && ky!=-1)
//	{
//		result^=KO[kx][ky];
//		result2^=KO2[kx][ky];
//	}
	key = result;
	key2 = result2;
	return key;
}
	
// this function must always update the previous keys
// and the rest of this class should use internalSetPoint to 
// change points that must be changed without changing the previous keys
// return true if stones have been captured
bool BoardStruct::setPoint(int x, int y, int colour, bool removeDead /*=true*/)
{
	if(x < size && x >= 0 && y < size && y >= 0) {
		lastMove = Move(x, y, colour, size);
		// reset Ko point
		kx = -1;
		ky = -1;
		updatePreviousKeys();
		return internalSetPoint(x, y, colour, removeDead);
	}
	return false;
}

/** @brief Add the specified move to the board. The coordinates and colour
  *   are extracted from the Move object and set in the board object.
  * @param move The move to add to the board.
  * @param removeDead Remove dead stones from the board if this is true. 
  * @return True if stones have been captured. */
bool BoardStruct::addToBoard(const Move& move, bool removeDead /*=true*/)
{
	// Always save the last move
	lastMove = move;

	// If pass...
	if(Move::xyToGMP(move.getX(), move.getY(), size)==-1)
		return false;

	if(move.getX() < size && move.getX() >= 0 && move.getY() < size && move.getY() >= 0) {
		// reset Ko point
		kx = -1;
		ky = -1;
		updatePreviousKeys();
		return internalSetPoint(move.getX(), move.getY(), move.getColour(), removeDead);
	}
	return false;
}

// only for use by BoardStruct
bool BoardStruct::internalSetPoint(int x, int y, int colour, bool removeDead)
{
	// check bounds, also copes with pass moves
	if(x<0 || x>(size-1) || y<0 || y>(size-1))
		return false;
	short p = contents.getValue(x, y);

	// if we're replacing a stone with another stone
	// we must first set that point to EMPTY
	if(p!=EMPTY && colour!=EMPTY)
	{
		contents.setValue(x, y, EMPTY);
		updateKey(p, EMPTY, x, y);
		updateStrings(x, y, EMPTY, removeDead);
	}

	contents.setValue(x, y, colour);
	updateKey(p, colour, x, y);
	return updateStrings(x, y, colour, removeDead);
}

bool BoardStruct::updateStrings(int x, int y, int colour, bool removeDead /*=true*/)
{
	// the parent string(s) this point belongs to
	stringsIterator ourString[4];
	//GoString* s = NULL;
	stringsIterator s = NULL;
	// temp variable to get colour of adjacent points
	int p = EMPTY;

	/**** Special case *****/
	// if an empty point is being set, then 
	// give a liberty point to each unique string
	// connected to this point

	if(colour==EMPTY) {
		// We should check if this point
		// was in a string before it became empty
		// and if so then remove it from the string		
		bool captured = false;
		if((s=getString(x, y))!=NULL) {
			captured = true;
			// if this is the last stone in the string remove the string
			if(s->getPoints().size()==1)
				removeStringFromStringsTable(s);
			else {
#ifdef USE_GOSTRINGPTRS
				// Keep the goStringPtrs updated when removing points from strings
				goStringPtrs.setValue(x, y, NULL);
#endif
				s->removePoint(x, y);
				// calculate and update new strings
				stringsIterator s2;
				GoString newnorth(BLACK), newsouth(BLACK), neweast(BLACK), newwest(BLACK);
				bool northUpdate, southUpdate, eastUpdate, westUpdate = false;
				northUpdate = southUpdate = eastUpdate = westUpdate = false;
				bool eraseOldString = false;
				// north
				if(y>0 && (s2=getString(x, y-1))!=NULL) {
					calculateString(x, y-1, newnorth, &(*s2));
					northUpdate = eraseOldString = true;
				}
				// south
				if(y<(size-1) && (s2=getString(x, y+1))!=NULL && (!northUpdate || (*s2)!=newnorth)) {
					calculateString(x, y+1, newsouth, &(*s2));
					southUpdate = eraseOldString = true;
				}
				// west
				if(x>0 && (s2=getString(x-1, y))!=NULL && (!northUpdate || (*s2)!=newnorth)
					&& (!southUpdate || (*s2)!=newsouth)) {
					calculateString(x-1, y, newwest, &(*s2));
					westUpdate = eraseOldString = true;
				}
				// east
				if(x<(size-1) && (s2=getString(x+1, y))!=NULL && (!northUpdate || (*s2)!=newnorth)
					&& (!southUpdate || (*s2)!=newsouth) && (westUpdate || (*s2)!=newwest)) {
					calculateString(x+1, y, neweast, &(*s2));
					eastUpdate = eraseOldString = true;
				}
				if(eraseOldString)
					removeStringFromStringsTable(s);
				if(northUpdate) {
					if((s2=getString(x, y-1))!=NULL)
						removeStringFromStringsTable(s2);
					addString(newnorth);
				}
				if(southUpdate) {
					if((s2=getString(x, y+1))!=NULL)
						removeStringFromStringsTable(s2);
					addString(newsouth);
				}
				if(westUpdate) {
					if((s2=getString(x-1, y))!=NULL)
						removeStringFromStringsTable(s2);
					addString(newwest);
				}
				if(eastUpdate) {
					if((s2=getString(x+1, y))!=NULL)
						removeStringFromStringsTable(s2);
					addString(neweast);
				}
				// return now since calculateString does liberties for us
				return captured;
			}
		}
		ourString[0] = NULL;
		ourString[1] = NULL;
		ourString[2] = NULL;
		ourString[3] = NULL;
		// north
		if(y>0) {
			p = getPoint(x, y-1);
			if(p==BLACK || p==WHITE) {
				ourString[0] = getString(x, y-1);
				ourString[0]->addLiberty(Point<>(x, y), *this);
			}
		}
		// south
		if(y<(size-1)) {
			p = getPoint(x, y+1);
			if(p==BLACK || p==WHITE) {
				ourString[1] = getString(x, y+1);
				if(ourString[0]!=NULL && ourString[0]==ourString[1])
					ourString[1] = NULL;
				else
					ourString[1]->addLiberty(Point<>(x, y), *this);
			}
		}
		// west
		if(x>0) {
			p = getPoint(x-1, y);
			if(p==BLACK || p==WHITE) {
				ourString[2] = getString(x-1, y);
				// check if this string has already
				// been found just before
				if(ourString[0]!=NULL && ourString[0]==ourString[2])
					ourString[2] = NULL;
				else if(ourString[1]!=NULL && ourString[1]==ourString[2])
					ourString[2] = NULL;
				else
					ourString[2]->addLiberty(Point<>(x, y), *this);
			}
		}
		// east
		if(x<(size-1)) {
			p = getPoint(x+1, y);
			if(p==BLACK || p==WHITE) {
				ourString[3] = getString(x+1, y);
				// check if this string has already
				// been found just before
				if(ourString[0]!=NULL && ourString[0]==ourString[3])
					ourString[3] = NULL;
				else if(ourString[1]!=NULL && ourString[1]==ourString[3])
					ourString[3] = NULL;
				else if(ourString[2]!=NULL && ourString[2]==ourString[3])
					ourString[3] = NULL;
				else
					ourString[3]->addLiberty(Point<>(x, y), *this);
			}
		}
		return captured;
	}

	ourString[0] = NULL;
	ourString[1] = NULL;
	ourString[2] = NULL;
	ourString[3] = NULL;

	if(colour!=BLACK && colour!=WHITE)
		return false;
	// number of liberties this point is adding to it's 
	// parent string
	int opponentColour = BLACK;
	if(colour==BLACK) opponentColour = WHITE;
	// vector of strings to be removed
	stringsIterator forRemoval[4];
	forRemoval[0] = NULL;
	forRemoval[1] = NULL;
	forRemoval[2] = NULL;
	forRemoval[3] = NULL;
	bool alreadyIn = false;

	// remember each string we remove a liberty from
	// so if surrounded, perhaps an enemy north and east
	// but the same string, we must check if we have already
	// removed 1 liberty or not, and not do it 
	// twice or more.....
	stringsIterator removedLiberty[4];
	removedLiberty[0] = NULL;
	removedLiberty[1] = NULL;
	removedLiberty[2] = NULL;
	removedLiberty[3] = NULL;

	// north
	if(y>0) {
		p = getPoint(x, y-1);
		if(p==opponentColour) {
			s = getString(x, y-1);
			if(s->getLibertyCount()==1) {
				// mark for removal
				alreadyIn = false;
				// check if already marked for removal
				// and make this fail if we have already removed a liberty
				for(int i=0;i<4;i++)
					if((forRemoval[i]!=NULL && forRemoval[i]==s) ||
					   (removedLiberty[i]!=NULL && removedLiberty[i]==s)) alreadyIn = true;
				if(!alreadyIn)
					forRemoval[0] = s;
			}
			else {
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if(removedLiberty[i]!=NULL && removedLiberty[i]==s) alreadyIn = true;
				if(!alreadyIn) {
					s->removeLiberty(Point<>(x, y));
					removedLiberty[0] = s;
				}
			}
		}
		else if(p==colour) {
			ourString[0] = getString(x, y-1);
			// take off 1 liberty for string because of
			// connection point
			ourString[0]->removeLiberty(Point<>(x, y));
		}
	}// end north
	// south
	if(y<(size-1)) {
		p = getPoint(x, y+1);
		if(p==opponentColour) {
			s = getString(x, y+1);
			if(s->getLibertyCount()==1) {
				// mark for removal
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if((forRemoval[i]!=NULL && forRemoval[i]==s) ||
					   (removedLiberty[i]!=NULL && removedLiberty[i]==s)) alreadyIn = true;
				if(!alreadyIn)
					forRemoval[1] = s;
			}
			else {
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if(removedLiberty[i]!=NULL && removedLiberty[i]==s) alreadyIn = true;
				if(!alreadyIn) {
					s->removeLiberty(Point<>(x, y));
					removedLiberty[1] = s;
				}
			}
		}
		else if(p==colour) {
			ourString[1] = getString(x, y+1);
			// check if this string has already
			// been found just before
			if(ourString[0]!=NULL && ourString[0]==ourString[1])
				ourString[1] = NULL;
			else {
				// take off 1 liberty for string because of
				// connection point
				ourString[1]->removeLiberty(Point<>(x, y));
			}
		}
	}// end south
	// west
	if(x>0) {
		p = getPoint(x-1, y);
		if(p==opponentColour) {
			s = getString(x-1, y);
			if(s->getLibertyCount()==1) {
				// mark for removal
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if((forRemoval[i]!=NULL && forRemoval[i]==s) ||
					   (removedLiberty[i]!=NULL && removedLiberty[i]==s)) alreadyIn = true;
				if(!alreadyIn)
					forRemoval[2] = s;
			}
			else {
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if(removedLiberty[i]!=NULL && removedLiberty[i]==s) alreadyIn = true;
				if(!alreadyIn) {
					s->removeLiberty(Point<>(x, y));
					removedLiberty[2] = s;
				}
			}
		}
		else if(p==colour) {
			ourString[2] = getString(x-1, y);
			// check if this string has already
			// been found just before
			if(ourString[0]!=NULL && ourString[0]==ourString[2])
				ourString[2] = NULL;
			else if(ourString[1]!=NULL && ourString[1]==ourString[2])
				ourString[2] = NULL;
			else {
				// take off 1 liberty for string because of
				// connection point
				ourString[2]->removeLiberty(Point<>(x, y));
			}
		}
	}
	// east
	if(x<(size-1)) {
		p = getPoint(x+1, y);
		if(p==opponentColour) {
			s = getString(x+1, y);
			if(s->getLibertyCount()==1) {
				// mark for removal
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if((forRemoval[i]!=NULL && forRemoval[i]==s) ||
					   (removedLiberty[i]!=NULL && removedLiberty[i]==s)) alreadyIn = true;
				if(!alreadyIn)
					forRemoval[3] = s;
			}
			else {
				alreadyIn = false;
				for(int i=0;i<4;i++)
					if(removedLiberty[i]!=NULL && removedLiberty[i]==s) alreadyIn = true;
				if(!alreadyIn) {
					s->removeLiberty(Point<>(x, y));
					removedLiberty[3] = s;
				}
			}
		}
		else if(p==colour) {
			ourString[3] = getString(x+1, y);
			// check if this string has already
			// been found just before
			if(ourString[0]!=NULL && ourString[0]==ourString[3])
				ourString[3] = NULL;
			else if(ourString[1]!=NULL && ourString[1]==ourString[3])
				ourString[3] = NULL;
			else if(ourString[2]!=NULL && ourString[2]==ourString[3])
				ourString[3] = NULL;
			else {
				// take off 1 liberty for string because of
				// connection point
				ourString[3]->removeLiberty(Point<>(x, y));
			}
		}
	}

	// create a new string and add to this BoardStruct
	GoString tempGoString(BLACK);
	if((s=getString(x, y))!=NULL)
		calculateString(x, y, tempGoString, &(*s));
	else
		calculateString(x, y, tempGoString, NULL);

	// remember one point from each non-NULL ourString and getString again after temp=addString(temp)
	// because insertion into a vector invalidates previous iterators on that vector.....
	vector<Point<> > ourStringPoints;
	vector<Point<> > forRemovalPoints;
	for(int i=0;i<4;i++) {
		if(ourString[i]!=NULL)
			ourStringPoints.push_back((*ourString[i]->getPoints().begin()));
		// remember all points from each string for removal
		// so we can get the string containing each stone
		// when we need to remove the strings
		// since the stones will change strings during the removal phase
		if(forRemoval[i]!=NULL) {
			const GoString::pointsTable& ptable = forRemoval[i]->getPoints();
			GoString::constPointsIterator piter(ptable.begin());
			for(;piter!=ptable.end();piter++)
				forRemovalPoints.push_back((*piter));
		}
	}

	stringsIterator temp = addString(tempGoString);

	bool stonesCaptured = false;

	for(i=0;i<ourStringPoints.size();i++) {
		// The new string should inherit it's parent strings status
		stringsIterator str(getString(ourStringPoints[i].x, ourStringPoints[i].y));

#ifdef _DEBUG
		GoString* ptr = &(*str);
		GoString* tempPtr = &(*temp);
#endif

		// Note: Status must be updated before concatString() is called
		//  because concatString() deletes str in the process...
		// An ALIVE status overrides all others
		if(temp->getStatus()!=ALIVE)
			temp->setStatus(str->getStatus());
		temp = concatString(temp, str);
	}

	// remove strings that contain stones that are marked for removal
	if(removeDead) {
		/** Note: This removes the dead stones from the board but what about
		  * the GoString in the strings table? 
		  * Answer: When the last stone of a string is removed, this function
		  * through internalSetPoint() calls removeStringFromStringsTable()
		  * to remove the string from the strings table. */
		capturedStones.clear();
		vector<Point<> >::iterator frpiter(forRemovalPoints.begin());
		for(;frpiter!=forRemovalPoints.end();frpiter++) {
			stonesCaptured = true;
			if(getPoint(frpiter->x, frpiter->y)==BLACK)
				blackStonesCaptured++;
			else
				whiteStonesCaptured++;
			internalSetPoint(frpiter->x, frpiter->y, EMPTY, true);
			capturedStones.push_back(Point<>(frpiter->x, frpiter->y));
		}
		// check for Ko situation
		// if only one stone captured and the stone just placed
		// has three enemy stones around it then the captured
		// point is a Ko point
		if(capturedStones.size()==1) {
			int fails = 0;
			int c;
			bool ko = true;
			bool emptyPoint = false;
			// North
			if(y>0) {
				if((c=getPoint(x, y-1))==colour)
					fails++;
				else if(c==EMPTY)
					emptyPoint = true;
			}
			// South
			if(y<(size-1)) {
				if((c=getPoint(x, y+1))==colour) {
					fails++;
					if(fails>=2) ko = false;
				}
				else if(c==EMPTY) {
					if(!emptyPoint)
						emptyPoint = true;
					else {
						fails++;
						if(fails>=2) ko = false;
					}
				}
			}
			// West
			if(ko && x>0) {
				if((c=getPoint(x-1, y))==colour) {
					fails++;
					if(fails>=2) ko = false;
				}
				else if(c==EMPTY) {
					if(!emptyPoint)
						emptyPoint = true;
					else {
						fails++;
						if(fails>=2) ko = false;
					}
				}
			}
			// East
			if(ko && x<(size-1)) {
				if((c=getPoint(x+1, y))==colour) {
					fails++;
					if(fails>=2) ko = false;
				}
				else if(c==EMPTY) {
					if(!emptyPoint)
						emptyPoint = true;
					else {
						fails++;
						if(fails>=2) ko = false;
					}
				}
			}
			if(ko) {
				kx = capturedStones.at(0).x;
				ky = capturedStones.at(0).y;
			}
		}
		// if the stone/s we just placed are dead as laid remove them now
		temp = getString(x, y);
		if(temp->getLibertyCount()==0) {
			/** Note: This removes the dead stones from the board but what about
			  * the GoString in the strings table? 
			  * Answer: When the last stone of a string is removed, this function
			  * through internalSetPoint() calls removeStringFromStringsTable()
			  * to remove the string from the strings table. */
			/** @todo Also this could be more efficient... */
			const GoString::pointsTable& ptable = temp->getPoints();
			GoString::constPointsIterator piter(ptable.begin());
			for(;piter!=ptable.end();piter++)
				forRemovalPoints.push_back((*piter));
			vector<Point<> >::iterator frpiter(forRemovalPoints.begin());
			for(;frpiter!=forRemovalPoints.end();frpiter++) {
				stonesCaptured = true;
				if(getPoint(frpiter->x, frpiter->y)==BLACK)
					blackStonesCaptured++;
				else
					whiteStonesCaptured++;
				internalSetPoint(frpiter->x, frpiter->y, EMPTY, true);
				capturedStones.push_back(Point<>(frpiter->x, frpiter->y));
			}
		}
	}
	return stonesCaptured;
}

BoardStruct::stringsIterator BoardStruct::concatString(BoardStruct::stringsIterator s1, 
													   BoardStruct::stringsIterator s2)
{
	// concat two GoStrings and put into s1
	// NOTE: s1 and s2 are already in string table

	if(s2==NULL)
		return s1;
	/** @todo This if block seems to do nothing... ??? */
	if(s1==NULL)
	{
		// do not delete since s1 must contain the new string
		GoString gs(*s2);
		removeStringFromStringsTable(s2);
		return addString(gs);
	}

	/** @todo Why are we comparing the string objects and not the iterators here? */
	// if both arguments point to the same string
	if((*s1)==(*s2))
		return s1;

	(*s1).concatString((*s2), *this);

	// error checking
#ifdef _DEBUG
	if(count(strings.begin(), strings.end(), *s2)!=1)
	{
		LogWriter log;
		char buffer[50];
		string message = "\nError in BoardStruct::concatString(): \n";
		message+="Concating s1: ";
		log.println(message);
		(*s1).printInfo();
		message = "\nAnd s2: ";
		log.println(message);
		(*s2).printInfo();
		message = "\nCount = ";
		sprintf(buffer, "%d", count(strings.begin(), strings.end(), *s2));
		message+=buffer;
		log.println(message);
		printAsciiBoard(true);
	}
#endif

#ifdef USE_GOSTRINGPTRS	
	// For each point in s2 that was added to s1 we must update the goStringPtrs matrix
	GoString::constPointsIterator piter(s2->getPoints().begin());
	for(;piter!=s2->getPoints().end();piter++)
		goStringPtrs.setValue(piter->x, piter->y, s1);
	// NOTE: That when using goStringPtrs we always have a guaranteed big enough hash_set
	//  so we don't need to worry about pointers being invalidated...
	removeStringFromStringsTable(s2);
	return s1;
#else
	// Need to remember a point so that after erase(s2)
	// we can relookup s1, the iterator to s1 will be
	// invalidated after the erase call
	Point<> p = *(s1->getPoints().begin());
	removeStringFromStringsTable(s2);
	return getString(p.x, p.y);
#endif
}

BoardStruct::stringsIterator BoardStruct::addString(const GoString& s)
{
#ifdef USE_GOSTRINGPTRS	
	// Update the goStringPtrs matrix
	GoString* ptr = &(strings.insert(s));
	GoString::constPointsIterator citer(ptr->getPoints().begin());
	for(;citer!=ptr->getPoints().end();citer++)
		goStringPtrs.setValue(citer->x, citer->y, ptr);
	return *ptr;
#else
	strings.push_back(s);
	return find(strings.begin(), strings.end(), s);
#endif
}

void BoardStruct::removeStringFromStringsTable(BoardStruct::stringsIterator s)
{
#ifdef USE_GOSTRINGPTRS
	// Update the goStringPtrs matrix
	GoString::constPointsIterator citer(s.getPoints().begin());
	for(;citer!=s.getPoints().end();citer++)
		goStringPtrs.setValue(citer->x, citer->y, NULL);
#endif
	strings.erase(s);
}

BoardStruct::stringsIterator BoardStruct::getString(int x, int y)
{
	// return NULL if this point is offboard
	// Note: We must return NULL if it's not a colour _or_ EMPTY
	// because the liberty update mechanism relies on this returning
	// the GoString from the stringsTable rather than being associated
	// with the board contents, so only check for offboard here...
	int c = getPoint(x, y);
	if(c!=BLACK && c!=WHITE && c!=EMPTY)
		return NULL;

	// return the string that contains this point
	// or NULL if not in table
	stringsIterator iter(strings.begin());
	for(;iter!=strings.end();iter++)
	{
		if(iter->containsPoint(x, y))
			return iter;
	}
	return NULL;
}

/** Const version of getString() */
BoardStruct::constStringsIterator BoardStruct::getStringConst(int x, int y) const
{
	// return NULL if this point is offboard
	// Note: We must return NULL if it's not a colour _or_ EMPTY
	// because the liberty update mechanism relies on this returning
	// the GoString from the stringsTable rather than being associated
	// with the board contents, so only check for offboard here...
	int c = getPoint(x, y);
	if(c!=BLACK && c!=WHITE && c!=EMPTY)
		return NULL;

#ifdef USE_GOSTRINGPTRS
	return goStringPtrs.getValue(x, y);
#else
	// return the string that contains this point or NULL if not in table
	constStringsIterator iter(strings.begin());
	for(;iter!=strings.end();iter++)
	{
		if(iter->containsPoint(x, y))
			return iter;
	}
	return NULL;
#endif
}

bool BoardStruct::calculateString(int x, int y, GoString& s, const GoString* oldString)
{
	// return a GoString pointer to the string that contains
	// this point, except it must be worked out only
	// by looking at contents
	int c = getPoint(x, y);
	if(c!=BLACK && c!=WHITE)
		return NULL;

	int liberties=0;
	s.clear();
	s.setOurColour(c);
	s.addPoint(x, y);
	int bs = getSize();

	if(oldString!=NULL)
		s.setStatus(oldString->getStatus());
	
	// use to mark where you've already counted
	Matrix<int> marker(bs, bs, EMPTY);
	
	// check bounds of x and y
	if(x < 0 || x > bs-1 || y < 0 || y > bs-1)
		return false;

	// mark point as checked
	marker.setValue(x, y, BLACK);
	
	const GoString::pointsTable& points = s.getPoints();
	GoString::constPointsIterator piter(points.begin());
	// cycle through points
	while(piter!=points.end())
	{
		int px = piter->x;
		int py = piter->y;
		
		// check north
		if (py > 0 && marker.getValue(px, py-1)==EMPTY)
		{
			// if the same string
			if (getPoint(px, py-1)==c)
				s.addPoint(px, py-1);
			else if(getPoint(px, py-1)==EMPTY)
				s.addLiberty(Point<>(px, py-1), *this);
			// mark point as checked
			marker.setValue(px, py-1, BLACK);
		} // end north
		// check south
		if (py < bs-1 && marker.getValue(px, py+1)==EMPTY)
		{
			// if the same string
			if (getPoint(px, py+1)==c)
				s.addPoint(px, py+1);
			else if(getPoint(px, py+1)==EMPTY)
				s.addLiberty(Point<>(px, py+1), *this);
			// mark point as checked
			marker.setValue(px, py+1, BLACK);
		} // end south
		// check west
		if (px > 0 && marker.getValue(px-1, py)==EMPTY)
		{
			// if the same string
			if (getPoint(px-1, py)==c)
				s.addPoint(px-1, py);
			else if(getPoint(px-1, py)==EMPTY)
				s.addLiberty(Point<>(px-1, py), *this);
			// mark point as checked
			marker.setValue(px-1, py, BLACK);
		} // end west
		// check east
		if (px < bs-1 && marker.getValue(px+1, py)==EMPTY)
		{
			// if the same string
			if (getPoint(px+1, py)==c)
				s.addPoint(px+1, py);
			else if(getPoint(px+1, py)==EMPTY)
				s.addLiberty(Point<>(px+1, py), *this);
			// mark point as checked
			marker.setValue(px+1, py, BLACK);
		} // end east
		piter++;
	} // end while
	return true;
}

void BoardStruct::clear(int s)
{
	size = s;

#ifdef USE_BIT_BOARDS
	// Round the mask upto a multiple of 32 bits just like the bit boards
	bitBoardSize = 32 - ((size*size) % 32) + (size*size);
	// Deallocate and reallocate the bitboards if the size has changed
	// Reset them to 0 too
#endif

	clear();
}

void BoardStruct::clear()
{
#ifndef USE_BIT_BOARDS
	initContents(size);
#endif
	initStrings();
	key = 0;
	key2 = 0;
	previousKey = 0;
	previousKey2 = 0;
//	numberOfCapturedStones = 0;
	blackStonesCaptured = 0;
	whiteStonesCaptured = 0;
	lastMove.setCoords(-1, -1);
	illegalMoveMemory.clear();
}

#ifndef _USE_BIT_BOARDS
void BoardStruct::initContents(int s /*=-1*/)
{
	// don't resize if not specified
	if(s!=-1)
		contents.resize(s, s);
	contents.setAllValues(EMPTY);
}
#endif

void BoardStruct::initStrings()
{
	strings.clear();

#ifdef USE_GOSTRINGPTRS
	// Resize and clear the goStringPtrs matrix
#error todo
#endif

	GoString s(BLACK);
	int c, i, j;
	// count through all points
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			// if this point not already in a string
			// add to strings as new string
			if(((c=getPoint(j, i))==BLACK || c==WHITE) && getString(j, i)==NULL)
			{
				if(calculateString(j, i, s, NULL))
					addString(s);
			}
		}
	} // end for i
}

void BoardStruct::printStringInfo() const
{
	LogWriter log;
	char buffer[200];

	constStringsIterator iter = strings.begin();
	int i=0;
	for(;iter!=strings.end();iter++)
	{
		sprintf(buffer, "String %d : ", i++);
		log.println(buffer, myName);
		iter->printInfo();
	}
	sprintf(buffer, "Stones captured (w.r.t. black): %d", getNumberOfCapturedStones());
	log.println(buffer, myName);
}

void BoardStruct::printAsciiBoard(bool withMargins /* =false */, Matrix<int>* mask /*=NULL*/) const
{
	// use LogWriter::printerr() so we can print
	// board even during a GTP session
	LogWriter::printerr("\n");
	
	// print letters
	if(withMargins)
	{
		LogWriter::printerr("   ");
		string message = "  ";
		for(int i=0;i<size;i++)
		{
			// skip 'i'
			if(i>7) 
				message.at(0) = 'a'+i+1;
			else
				message.at(0) = 'a'+i;
			LogWriter::printerr(message);
		}
		LogWriter::printerr("\n");
	}

	for(int i=0;i<size;i++)
	{
		if(withMargins)
		{
			char buffer[50];
			string message;
			sprintf(buffer, "%d", size-i);
			message+=buffer;
			message+=" ";
			// extra space for 1-9
			if((size-i)<=9)
				message+=" ";
			LogWriter::printerr(message);
		}
		for(int j=0;j<size;j++)
		{
			if(mask!=NULL)
			{
				if(mask->getValue(j, i)==BLACK)
				{
					if(getPoint(j, i)==BLACK)
						LogWriter::printerr("B ");
					else
						LogWriter::printerr("b ");
				}
				else if(mask->getValue(j, i)==WHITE)
				{
					if(getPoint(j, i)==WHITE)
						LogWriter::printerr("W ");
					else
						LogWriter::printerr("w ");
				}
				else if(mask->getValue(j, i)==EMPTY)
				{
					// check for handicap points
					if(size==19&&
					  (i==3||i==9||i==15)&&(j==3||j==9||j==15))
						LogWriter::printerr("+ ");
					else if(size==13&&(i==3||i==9)&&(j==3||j==9))
						LogWriter::printerr("+ ");
					else if(size==9&&i==4&&j==4)
						LogWriter::printerr("+ ");
					else
						LogWriter::printerr(". ");
				}
			}
			else if(getPoint(j, i)==EMPTY)
			{
				// check for handicap points
				if(size==19&&
				  (i==3||i==9||i==15)&&(j==3||j==9||j==15))
					LogWriter::printerr("+ ");
				else if(size==13&&(i==3||i==9)&&(j==3||j==9))
					LogWriter::printerr("+ ");
				else if(size==9&&i==4&&j==4)
					LogWriter::printerr("+ ");
				else
					LogWriter::printerr(". ");
			}
			else if(getPoint(j, i)==WHITE)
				LogWriter::printerr("w ");
			else if(getPoint(j, i)==BLACK)
				LogWriter::printerr("b ");
		}
		LogWriter::printerr("\n");
	}
}

/** Set a point on the board using the string representation of a 
point, e.g.\ aa meaning the top left point. 
@param p The string representation of the point.
@param colour The colour to set the point to.
@param removeDead Remove dead stones from the board if this is true. */
void BoardStruct::setPointSGF(string p, int colour, bool removeDead /*=true*/)
{
	LogWriter log;
	// pass move
	if (p.length()==0 || p=="tt") {
		lastMove = Move(-1, -1, colour, size);
		return;
	}
	else if (p.length()<2) {
		string message = "Invalid move string: ";
		message+=p;
		log.println(message, myName);
		return;
	}
	char a = p[0];
	char b = p[1];
	int x = (int)a-'a';
	int y = (int)b-'a';
	setPoint(x, y, colour, removeDead);
}

/** @brief This function removes the stones in the specified string from
  * the board and from the strings table. */
void BoardStruct::removeString(BoardStruct::stringsIterator s)
{
	// Cycle through the stones in string and keep setting the
	// points to empty. UpdateStrings() takes care of removing
	// empty strings from the strings table

	// Copy the points table since the string will be vanishing
	// as we update the board contents...
	GoString::pointsTable points = s->getPoints();
	GoString::pointsIterator piter(points.begin());
	for(;piter!=points.end();piter++) 
		internalSetPoint(piter->x, piter->y, EMPTY, true);
}