#ifndef MOVE_H
#define MOVE_H

#pragma warning( disable : 4786 )

#include "globals.h"
//#include "Misc.h"
#include <string>
#include <assert.h>
using namespace std;

// defined in misc.h
//const char* colourToString(int colour);

/* NOTE Coordinate systems:
	GoPos:  a-s, no i, 1-19, top left corner is a1, pass is ""
	x,y:    0-18, top left is 0,0, pass is -1, -1
	GMP:    0-360, top left is 0, pass it -1
*/

class Move
{
	protected:
		float NNScore;
		int alphaBetaScore;
		signed char colour;
		signed char boardSize;
		// cartesian coords
		signed char x, y;

	public:

		Move() : colour(EMPTY), boardSize(19), x(-1), y(-1), NNScore(0), alphaBetaScore(0) {};
		Move(const int cx, const int cy, const int ccolour, const int boardsize)
			: colour(ccolour), boardSize(boardsize), x(cx), y(cy), NNScore(0), alphaBetaScore(0) {};
		/* This constructor allows you to additionally specify a NNScore value. */
		Move(const int cx, const int cy, const int ccolour, const int boardsize, const float score)
			: colour(ccolour), boardSize(19), x(cx), y(cy), NNScore(score), alphaBetaScore(0) {};
//		Move(int cx, int cy, int boardsize) : colour(EMPTY), boardSize(boardsize), x(cx), y(cy), NNScore(0), 
//			alphaBetaScore(0) {};
//		Move(int cx, int cy, float s) : colour(EMPTY), boardSize(19), x(cx), y(cy), NNScore(s), alphaBetaScore(0) {};
		Move(const Move& move)
		{
			NNScore = move.NNScore;
			alphaBetaScore = move.alphaBetaScore;
			colour = move.colour;
			boardSize = move.boardSize;
			// cartesian coords
			x = move.x;
			y = move.y;
		}
//		~Move() {};

		Move& operator=(const Move &m)
		{
			if(this!=&m)
			{
				x = m.x;
				y = m.y;
				colour = m.colour;
				boardSize = m.boardSize;
				NNScore = m.NNScore;
				alphaBetaScore = m.alphaBetaScore;
			}
			return *this;
		}
		// return memory used by this object in bytes
		// NOTE: This is usually only an estimate
		int getMemoryUsage() const 
		{
			int bytesUsed = 0;
			// NNScore
			bytesUsed+=sizeof(NNScore);
			// alphaBetaScore, colour, boardSize, x, y
//			bytesUsed+=sizeof(int)*6;
			bytesUsed+=sizeof(alphaBetaScore);
			bytesUsed+=sizeof(colour);
			bytesUsed+=sizeof(boardSize);
			bytesUsed+=sizeof(x);
			bytesUsed+=sizeof(y);
			return bytesUsed;
		}
		void clear() { colour = EMPTY; boardSize = 19; x = -1; y = -1; NNScore = 0;	alphaBetaScore = 0; };
		inline int getX() const { return x; };
		inline void setX(int cx) { x = cx; };
		inline int getY() const { return y; };
		inline void setY(int cy) { y = cy; };
		inline void setCoords(int cx, int cy) { x = cx; y = cy; };
		inline float getNNScore() const { return NNScore; };
		inline void setNNScore(float aScore) { NNScore = aScore; };
		inline void setAlphaBetaScore(int aScore) { alphaBetaScore = aScore; };
		inline int getAlphaBetaScore() const { return alphaBetaScore; };
		inline void setBoardSize(int bs) { assert(bs>0); boardSize = bs; };
		inline int getBoardSize() const { return boardSize; };
		inline void setColour(int c) { assert(c>=EMPTY&&c<=SGF_SQUARE); colour = c; };
		inline int getColour() const { return colour; };

		void printInfo(bool verbose, string* printTo = NULL) const;

		inline bool operator>(const Move& p1) const { return (NNScore>p1.NNScore) ? true : false; };
		inline bool operator<(const Move& p1) const { return (NNScore<p1.NNScore) ? true : false; };
		inline bool operator==(const Move& p1) const {	return (x==p1.x&&y==p1.y&&colour==p1.colour) ? true : false; };
		/****************************************/
		/** Static format conversion functions **/
		/****************************************/
		static int GMPToX(int g, int boardSize) { if(g==-1) return -1; return g%boardSize; };
		static int GMPToY(int g, int boardSize) { return (g==-1) ? -1 : g/boardSize; };
		static int xyToGMP(int x, int y, int boardSize) { return x+(y*boardSize); };
		
		static string xyToColourAndGoPosValue(const Move& m)
		{
			return xyToColourAndGoPosValue(m.getX(), m.getY(), m.getBoardSize(), m.getColour());
		}

		static string xyToColourAndGoPosValue(int x, int y, int size, int colour)
		{
			string s("");
			
			if(colour==BLACK)
				s+="B[";
			else if(colour==WHITE)
				s+="W[";
			else
				s+="?[";

			// check for pass
			if(x<0 || y<0) {
				s+="]";
				return s;
			}

			s+=xyToGoPosValue(x, y, size);
			s+="]";

			return s;
		}

		// Convert a Move object into a String representation of the form
		// [letter][0-18] e.g. 'a0' would be the top left corner. 'i' is skipped.
		// Note: the y coordinate starts at 0
		static string xyToGTPValue(const Move& m)
		{
			return xyToGTPValue(m.getX(), m.getY(), m.getBoardSize());
		}

		static string xyToGTPValue(int x, int y, int size)
		{
			// if pass
			if (x<0||y<0)
				return string("");

			string s;
			if (x>7)
				s.append(1, (char)(((int)'a')+x+1));
			else
				s.append(1, (char)(((int)'a')+x));
			char buffer[50];
			sprintf(buffer, "%d", y);
			s+=buffer;
			return s;
		}

		// Convert a Move object into a String representation of the form
		// [letter][1-19] e.g. 'a1' would be the bottom left corner. 'i' is skipped.
		static string xyToGoPosValue(const Move& m)
		{
			return xyToGoPosValue(m.getX(), m.getY(), m.getBoardSize());
		}

		static string xyToGoPosValue(int x, int y, int size)
		{
			// if pass
			if (x<0||y<0)
				return string("");

			string s;
			if (x>7)
				s.append(1, (char)(((int)'a')+x+1));
			else
				s.append(1, (char)(((int)'a')+x));
			char buffer[50];
			sprintf(buffer, "%d", size-y);
			s+=buffer;
			return s;
		}
		static int GoPosToX(string c)
		{
			// watch out for 'i'
			if(c.size()<1 || c.at(0)=='i') return -1;
			if(c.at(0)>'i') return c.at(0)-'a'-1;
			return c.at(0)-'a';
		}
		static int GoPosToY(string c, int size)
		{
			char col;
			int row, n;
			if(sscanf(c.c_str(), "%c%d%n", &col, &row, &n)!=2)
				return -1;
			return size-row;
		}
		static int SGFToX(string c) 
		{	
			if(c=="" || c=="tt") 
				return -1;
			else
				return c[0] - 'a'; 
		}
		static int SGFToY(string c)
		{ 
			if(c=="" || c=="tt")
				return -1;
			else
				return c[1] - 'a'; 
		}
		static string xyToSGF(int x, int y, int size)
		{
			if(x==-1 || y==-1)
				return "";
			char c[3];
			c[0] = 'a' + x;
			c[1] = 'a' + y;
			c[2] = '\0';
			return string(c);
		}
		static void getPassMove(Move &m) { m.setX(-1); m.setY(-1); };
};

#endif