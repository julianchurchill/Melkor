#ifndef BOARDSTRUCT_H
#define BOARDSTRUCT_H

#pragma warning( disable : 4786 )

#include "GoString.h"
#include "Point.h"
#include "Matrix.h"
#include "Move.h"
#include "SpecialPoint.h"
#include <vector>
#include <list>
//#include <set>
using namespace std;

// Experimental - should use around half the amount of memory a Matrix<char> does:
//#define USE_BIT_BOARDS

/** @todo As well as a list of actual GoStrings perhaps this class could
  * hold a list of pointers to the GoStrings for easy access. This would be
  * a Matrix<GoString*>(boardsize, boardsize) so we only have to do 
  * goStringPtrs.getValue(x, y) to retrieve a pointer to the goString at that
  * point. Do some testing, this may or may not speed things up... */
//#define USE_GOSTRINGPTRS
#ifdef USE_GOSTRINGPTRS
#define USE_HASHTABLE_FOR_STRINGS
#include "hash_set.h"
#endif

class GoString;

class BoardStruct
{
	public:
		// Use list because of having to erase and move things around in the 
		// table, list is much faster than vector
		/** @todo Why don't we use a hash_set for faster retrieval?
		  * GoString objects even have their own key... */
#ifdef USE_HASHTABLE_FOR_STRINGS
		typedef hash_set<GoString> stringTable;
#else
		typedef list<GoString> stringTable;
#endif
		typedef stringTable::iterator stringsIterator;
		typedef stringTable::const_iterator constStringsIterator;

#ifdef USE_GOSTRINGPTRS
		typedef Matrix<GoString*> goStringPtrsList;
#endif

		typedef Matrix<char> contentsType;

		typedef BOARDSTRUCT_KEY_TYPE key_type;

	protected:

		static const string myName;

#ifdef USE_BIT_BOARDS
		// Use one board per colour, an on bit in a position means a stone
		// of that colour is present. Use an empty bit board too for quick checking.
		// Use 48 bytes for 19x19 = 361 bits to be allocated (rounded to 384 = 48 bytes)
		BitBoard *blackBoard, *whiteBoard, *emptyBoard;
		int bitBoardSize;
#else
		contentsType contents;
#endif

#ifdef USE_GOSTRINGPTRS
		goStringPtrsList goStringPtrs;
#endif

		stringTable strings;
		char size;

		// hash key and values for calculating it
		key_type key;
		// lock key, to help avoid collisions
		key_type key2;
		static bool hashValuesInitialised;
		static vector<vector<int> > B;
		static vector<vector<int> > W;
		static vector<vector<int> > KO;
		static vector<vector<int> > B2;
		static vector<vector<int> > W2;
		static vector<vector<int> > KO2;
		static void initHashValues();

		// for checking Ko situations, save
		// previous board state
		key_type previousKey, previousKey2;
		// Ko point coords - NOT USED YET
		char kx, ky;

		// with respect to BLACK (e.g. if white has captured 10 and black 5 then this value will be -5).
//		short numberOfCapturedStones;
		short blackStonesCaptured, whiteStonesCaptured;
		// This list keeps the coordinates of stones removed due to the _last_ move only,
		// not all moves leading to this board state.
		vector<Point<> > capturedStones;

		/** @todo These could be chars */
//		short ourColour;
//		short theirColour;
		// use this to check when removing captured stones
		bool useLastMove;

		void updatePreviousKeys() { previousKey = key; previousKey2 = key2; };
		bool internalSetPoint(int x, int y, int colour, bool removeDead);

		// Define contentsGetValue() and contentsSetValue() according to internal contents type
#ifdef USE_BIT_BOARDS
		inline int contentsGetValue(int x, int y) const
		{
			/** @todo Is this the best way of implementing this function? */
			BitBoard[bitBoardSize] maskBoard = 2^(y*size)+x;

			if( blackBoard & maskBoard ) return BLACK;
			else if( whiteBoard & maskBoard ) return WHITE;
			else if( emptyBoard & maskBoard ) return EMPTY;

			// ANDing all the boards should be 0 because
			// they should all be mutually exclusive
			// i.e. if we have a black stone at one point we 
			// can't have a white stone or an empty point there too!
			assert((blackBoard & whiteBoard & emptyBoard) == 0);
		}
		inline void contentsSetValue(int x, int y, int colour) {
#error todo
		}
#else
		inline int contentsGetValue(int x, int y) const { return contents.getValue(x, y); };
		inline void contentsSetValue(int x, int y, int colour) { contents.setValue(x, y, colour); };
#endif

		// GoString manipulation functions
		bool updateStrings(int x, int y, int colour, bool removeDead = true);
		BoardStruct::stringsIterator concatString(BoardStruct::stringsIterator s1, 
			BoardStruct::stringsIterator s2);
		BoardStruct::stringsIterator addString(const GoString& s);
		bool calculateString(int x, int y, GoString& s, const GoString* oldString);

	private:
		Move lastMove;
		// keep a list of illegal moves for fast checking
		// which is updated every time one of the isLegal or
		// getLegalMoves functions is called with a 
		// 'vector<Point<> >* changedPoints' argument.
		vector<Move> illegalMoveMemory;

		void removeStringFromStringsTable(BoardStruct::stringsIterator s);

	public:
	
		BoardStruct(int s = 19, int oc = BLACK);
		BoardStruct(const vector<vector<char> >& b, int s, int oc = BLACK);
		BoardStruct(const contentsType& b, int s, int oc = BLACK);
		BoardStruct(const BoardStruct& g);
		BoardStruct& operator=(const BoardStruct& g);
//		virtual ~BoardStruct() {};
#ifdef USE_BIT_BOARDS
		~BoardStruct()
		{
#error delete the allocated bit boards
		}
#endif

		bool operator==(const BoardStruct& b);

//		inline short getOurColour() { return ourColour; };

		static key_type updateZobristHashKey(int& key1, int& key2, int previousColour, int nowColour, int j, int i);
		key_type updateKey(int previous, int now, int j, int i);
		inline key_type calculateKey();
		inline key_type getKey() const { return key; };
		inline key_type getKey2() const { return key2; };
		inline key_type getPreviousKey() const { return previousKey; };
		inline key_type getPreviousKey2() const { return previousKey2; };
		inline void setKey(key_type k) { key = k; };
		inline void setKey2(key_type k) { key2 = k; };
		inline void setPreviousKey(key_type k) { previousKey = k; };
		inline void setPreviousKey2(key_type k) { previousKey2 = k; };
		inline int getPoint(int x, int y) const
		{
			if(x<0 || x>=size || y<0 || y>=size)
				return OFFBOARD;
			return contentsGetValue(x, y);
		}
		bool setPoint(const int x, const int y, const int colour, const bool removeDead = true);
		bool addToBoard(const Move& move, const bool removeDead = true);

		// Const versions
#ifdef USE_GOSTRINGPTRS
	public:
		inline const GoString* getString(int x, int y) const { return goStringPtrs.getValue(x, y); };
		inline GoString* getString(int x, int y) { return goStringPtrs.getValue(x, y); };
#else
//		inline BoardStruct::stringsIterator getString(Point<> p) { return getString(p.x, p.y); };
//		BoardStruct::stringsIterator getString(int x, int y);

//		inline const BoardStruct::constStringsIterator getString(Point<> p) const { return getString(p.x, p.y); };
//		const GoString& getString(int x, int y) const;
	public:
		BoardStruct::constStringsIterator getStringConst(int x, int y) const;
		BoardStruct::stringsIterator getString(int x, int y);
#endif

	public:
		inline int getSize() const { return size; };
		inline int getHeight() const { return size; };
		inline int getWidth() const { return size; };
		void clear();
		void clear(int s);
		// resize() is just for compatibility with previous versions
		inline void resize(int s) { clear(s); };
		void initStrings();
#ifndef USE_BIT_BOARDS
		void initContents(int s = -1);
#endif

#ifdef USE_BIT_BOARDS
#error need alternative getContents() functions for bit boards
#error Check if these are _actually_ needed anywhere - ideally
		all access of the contents should be through getPoint() and setPoint()
#else
		inline contentsType& getContents() { return contents; };
		inline const contentsType& getContents() const { return contents; };
#endif

		inline stringTable& getStrings() { return strings; };
		inline const stringTable& getStrings() const { return strings; };

		void printStringInfo() const;
		void printAsciiBoard(bool withMargins = false, Matrix<int>* mask = NULL) const;
		/** @return The number of captured stones so far with respect to BLACK 
		/*  (e.g. if white has captured 10 and black 5 then this value will be -5). */
		inline int getNumberOfCapturedStones() const { return whiteStonesCaptured - blackStonesCaptured; };
//		inline int getNumberOfCapturedStones() const { return numberOfCapturedStones; };
		inline int getCapturedBlackStones() const { return blackStonesCaptured; };
		inline int getCapturedWhiteStones() const { return whiteStonesCaptured; };
		inline vector<Point<> >& getCapturedStonesList() { return capturedStones; };
		inline int getLastX() const { return lastMove.getX(); };
		inline int getLastY() const { return lastMove.getY(); };
		inline Move& getLastMove() { return lastMove; };
		inline const Move& getLastMove() const { return lastMove; };
		inline void setLastMove(int x, int y, int colour)
		{
			lastMove.setX(x);
			lastMove.setY(y);
			lastMove.setColour(colour);
		}
		inline char getKoX() const { return kx; };
		inline char getKoY() const { return ky; };
		inline vector<Move>& getIllegalMoveMemory() { return illegalMoveMemory; };
		/** Set our colour. Useful to pass who is what colour within a board state.
		  * @param c Our colour. */
/*		inline void setOurColour(int c)
		{
			ourColour = c;
			if(ourColour==BLACK)
				theirColour = WHITE;
			else
				theirColour = BLACK;	
		}*/
		
		/** Set their colour. 
		  * @param c Their colour. */
/*		inline void setTheirColour(int c)
		{
			theirColour = c;
			if(theirColour==BLACK)
				ourColour = WHITE;
			else
				ourColour = BLACK;	
		}*/
		
		void setPointSGF(string p, int colour, bool removeDead = true);
		inline bool enableMarkLastMove(bool l)
		{
			bool t = useLastMove;
			useLastMove = l;
			return t;
		}

		int getMemoryUsage(bool includeVectorReserved = false) const
		{
			//stringTable strings;
			//contentsType contents;
			//char size;
			//key_type key;
			//key_type key2;
			//key_type previousKey, previousKey2;
			//char kx, ky;
			//short numberOfCapturedStones;
			//vector<Point<> > capturedStones;
			//int ourColour;
			//int theirColour;
			//bool useLastMove;
			//Move lastMove;
			//vector<Move> illegalMoveMemory;

			int bytesUsed = 0;
#ifdef USE_BIT_BOARDS
#error add bit board memory use here
#else
			bytesUsed+=contents.getMemoryUsage(includeVectorReserved);
#endif
			constStringsIterator citer(strings.begin());
			for(;citer!=strings.end();citer++)
				bytesUsed+=citer->getMemoryUsage(includeVectorReserved);
			bytesUsed+=sizeof(size);
			bytesUsed+=sizeof(key);
			bytesUsed+=sizeof(key2);
			bytesUsed+=sizeof(previousKey);
			bytesUsed+=sizeof(previousKey2);
			bytesUsed+=sizeof(kx);
			bytesUsed+=sizeof(ky);
//			bytesUsed+=sizeof(numberOfCapturedStones);
			bytesUsed+=sizeof(blackStonesCaptured);
			bytesUsed+=sizeof(whiteStonesCaptured);
			if(includeVectorReserved)
				bytesUsed+=sizeof(Point<>)*capturedStones.capacity();
			else
				bytesUsed+=sizeof(Point<>)*capturedStones.size();

//			bytesUsed+=sizeof(ourColour);
//			bytesUsed+=sizeof(theirColour);
			bytesUsed+=sizeof(useLastMove);
			bytesUsed+=sizeof(lastMove);
			if(includeVectorReserved)
				bytesUsed+=sizeof(Move)*illegalMoveMemory.capacity();
			else
				bytesUsed+=sizeof(Move)*illegalMoveMemory.size();
			
			return bytesUsed;
		}

		void removeString(BoardStruct::stringsIterator s);
};

#endif
