#ifndef GOSTRING_H
#define GOSTRING_H

#pragma warning( disable : 4786 )

#include "Point.h"
#include "globals.h"
// BoardStruct.h is included after GoString class is defined
// so that the compiler can compile a list<GoString> template
// that the BoardStruct definition needs!!!
//#include "BoardStruct.h"
#include <vector>
#include <list>
//#include <set>
using namespace std;

class BoardStruct;

class GoString
{
	public:
		/** @todo Would these be better as hash_sets? Do a timing test... */
		typedef list<Point<> > pointsTable;
		typedef list<Point<> > libertiesTable;
		typedef list<Point<> > eyesTable;
		typedef pointsTable::iterator pointsIterator;
		typedef libertiesTable::iterator libertiesIterator;
		typedef eyesTable::iterator eyesIterator;
		typedef pointsTable::const_iterator constPointsIterator;
		typedef libertiesTable::const_iterator constLibertiesIterator;
		typedef eyesTable::const_iterator constEyesIterator;

	private:
	
		static bool hashValuesInitialised;
		static vector<vector<int> > HASH;
		static vector<vector<int> > HASH2;

		int key, key2;
		char ourColour;
		int status;

		pointsTable points;
		libertiesTable liberties;
		eyesTable eyes;
		void addEye(Point<>& p);
		void removeEye(Point<>& p);

	public:

		GoString(int colour) : key(0), key2(0), ourColour(colour), status(UNKNOWN) {};
		GoString(int x, int y, int colour) : key(0), key2(0), ourColour(colour), status(UNKNOWN) { addPoint(x, y); };
		GoString(const GoString& g) : points(g.points), liberties(g.liberties), eyes(g.eyes), key(g.key),
			key2(g.key2), ourColour(g.ourColour), status(g.status) {};
		~GoString() {};
		GoString& operator=(const GoString& g);
		inline bool operator<(const GoString& p1) const
		{
			if(key==p1.getKey()) 
				return (key2<p1.getKey2()) ? true : false;
			return (key<p1.getKey()) ? true : false;
		}
		inline bool operator==(const GoString& p1) const
		{
			if(&p1==NULL) return false;
			if(ourColour==p1.ourColour && key==p1.getKey() && key2==p1.getKey2()) return true;
			else return false;
		}
		inline bool operator!=(const GoString& p1) const { return !((*this)==p1); };

		inline int updateKey(int x, int y)
		{
			// We must initialise the static HASH members only on the
			// first use. It cannot be done in the GoString constructor
			// since the static constructors (line 40 above) are called
			// at the beginning of the main() loop. In this program
			// the GoString constructor is called by static objects at the start
			// of main() and so the HASH values are initialised then. Then
			// they are initialised again as all the static variables are allocated one-by-one.
			// NOTE: Always initialise static member object variables on their first use!
			if(!hashValuesInitialised) initHashValues();
			// add the new point to the key without recalculating the entire key
			key^=HASH[x][y];
			key2^=HASH2[x][y];	
			return key;
		}
		int calculateKey();
		void initHashValues();
		void printInfo() const;
		void concatString(GoString& s, BoardStruct& b);
		inline int getKey() const { return key; };
		inline int getKey2() const { return key2; };
		inline int getColour() const { return ourColour; };
		inline int getStoneCount() const { return points.size(); };
		inline void setOurColour(char c) { ourColour = c; };
		inline int getStatus() const { return status; };
		inline void setStatus(int s) { status = s; };
		void clear();

		// points
		void addPoint(int x, int y);
		void removePoint(int x, int y);
		bool containsPoint(int x, int y) const;
		inline const pointsTable& getPoints() const { return points; };

		// liberties
		void addLiberty(Point<>& p, BoardStruct& b);
		void removeLiberty(Point<>& p);
		bool containsLiberty(const Point<>& p) const;
		inline int getLibertyCount() const { return liberties.size(); };
		inline const libertiesTable& getLibertyPoints() const { return liberties; };

		// eyes
		bool eyeCheck(Point<>& p, BoardStruct& b);
		inline const eyesTable& getEyes() const { return eyes; };
		inline int getRealEyeCount() const { return eyes.size(); };

		int getMemoryUsage(bool includeVectorReserved = false) const
		{
			//int key, key2;
			//char ourColour;
			//pointsTable points;
			//libertiesTable liberties;
			//eyesTable eyes;

			int bytesUsed = 0;
			bytesUsed+=sizeof(key);
			bytesUsed+=sizeof(key2);
			bytesUsed+=sizeof(ourColour);
//			if(includeVectorReserved)
//				bytesUsed+=sizeof(Point<>)*points.capacity();
//			else
				bytesUsed+=sizeof(Point<>)*points.size();
//			if(includeVectorReserved)
//				bytesUsed+=sizeof(Point<>)*liberties.capacity();
//			else
				bytesUsed+=sizeof(Point<>)*liberties.size();
//			if(includeVectorReserved)
//				bytesUsed+=sizeof(Point<>)*eyes.capacity();
//			else
				bytesUsed+=sizeof(Point<>)*eyes.size();
			return bytesUsed;
		}
};

#include "BoardStruct.h"

#endif
