#ifndef MISC_H
#define MISC_H

#pragma warning( disable : 4786 )

#include <string>
#include <io.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "SGFReader.h"
#include "BoardStruct.h"
#include "Goal.h"
#include "Move.h"
#include "Rect.h"
#include "GoString.h"
#include "SpecialPoint.h"
#include "BPN.h"
#include "BMP.h"
#include "globals.h"

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif

using namespace std;

//#ifndef _DEBUG
unsigned long& random_seed();
//#endif

struct GoRankRange
{
	// range is from -30 to +10
	// -.v.e. is kyu, +.v.e. is dan
	int rangeLow; // -100 means ignore ranges
	int rangeHigh; // i.e. 25k-15k
	GoRankRange()
	{
		rangeLow = -100;
		rangeHigh = -100;
	}
	GoRankRange(int rl, int rh)
	{
		rangeLow = rl;
		rangeHigh = rh;
	}
};

/** Radix 10 only version of itoa, includes temporary buffer for conversion. */
char* itoa(int value);

void getFileList(vector<string>& files, const char* filter);
bool readGNUGoResultsFile(string filename, vector<char>& results, int size);
// extract coordinates from the given line in GTP form
// e.g. "E4 B8 G18 A15"
int getCoords(ifstream& in, char value, vector<char>& results, int size);
int getLine(ifstream& in, string& line);
void setupBoardFromSGF(SGFReader& sgf, BoardStruct& board);
void setupBoardFromSGFNode(SGFReader& sgf, BoardStruct& board, SGFNode* node = NULL);
void addSGFNodeToBoard(Move* m, SGFReader& sgf, BoardStruct& board, SGFNode* node = NULL);
float getRandom(float min, float max);

bool calculateInfluence(const BoardStruct& b, float& score, int ourColour,
						string caller, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = NULL,
						int dilations=5, int erosions=21, bool scaleToStrength = false);
void printInfluenceMap(int influenceMap[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int boardSize);
bool calculateLibertyInfluence(const BoardStruct& b, float& score, int ourColour,
						string caller, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = NULL,
						int dilations=5, int erosions=21);
void calculateBoundary(Rect& dBounds, const BoardStruct& tempBoard, int x, int y);
void addBoundaries(Rect& bound1, const Rect& bound2);
void expandBoundaryForExtraStrings(Rect& dBounds, const BoardStruct& tempBoard);

const char* colourToString(int colour);

/** Return the higher value out of two values. */
inline float maximum(float a, float b) { return (a>=b) ? a : b; };
/** Return the lower value out of two values. */
inline float minimum(float a, float b) { return (a<=b) ? a : b; };
float scaleValue(float value, float min, float max, float newmin, float newmax);

myRGB getDistancePixelmyRGB(float v, float distancehigh, float distancelow);
void createLayerImages(BPN& bpn, int sourceType, string& prefix, bool onePanelPerNodeType = false);
void createRawLayerImages(BPN& bpn);
void spliceWeightImages(string sourceDir, string outputFilename);

bool rankRangeFilterSGF(const SGFReader& sgf, const GoRankRange& range);

bool parseGoalString(const string& value, Goal& g, string& errorString, const BoardStruct& b);

/** OS independent timing routines **/
// These are for a static timer
void timerStart();
void timerEnd();
double timerValue();

#ifdef _WIN32

// These functions work with arguments to determine start, frequency and end of timer
void timerStart(LARGE_INTEGER* tstart, LARGE_INTEGER* freq);
void timerEnd(LARGE_INTEGER* tend);
double timerValue(LARGE_INTEGER& tstart, LARGE_INTEGER& tend, LARGE_INTEGER& freq);

#else

void timerStart(clock_t* tstart);
void timerEnd(clock_t* tend);
double timeValue(clock_t tstart, clock_t tend);

#endif

#endif
