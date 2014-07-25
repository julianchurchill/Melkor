// Miscellaneous functions...

#include "Misc.h"
#include "CaptureGoal.h"
#include "InfluenceGoal.h"
#include "LibertyCountGoal.h"
#include "LibCountCaptureGoal.h"
#include "StoneCountGoal.h"
#include "BMP.h"
#include "Go.h"
#include <algorithm>
#include <stack>

#ifndef _WIN32
#include <time.h>
#endif

/** Radix 10 only version of itoa, includes temporary buffer for conversion. */
char* itoa(int value)
{
	// NOTE: itoa() can only convert a number into buffer for upto 33 bytes
	char buffer[33];
	return itoa(value, buffer, 10);
}

#ifdef _WIN32
/*************************************************************/
/* void getFileList(vector<string>& files, char* filter)     */
/* Reads in all files that match the filter in the current   */
/* directory.                                                */
/*                                                           */
/* getFileList() requires direct.h to use:                   */
/* struct _finddata_t, long _findfirst(char*, _finddata_t*)  */
/* long _findnext(long, _finddata_t*)                        */
/* _findclose(long)                                          */
/*************************************************************/

void getFileList(vector<string>& files, const char* filter)
{
	struct _finddata_t c_file;
	long hFile;
	// Now do SGF files
	//if((hFile=_findfirst(filter, &c_file))==-1L)
	//	cout << "No " << filter << " files found." << endl;
	if((hFile=_findfirst(filter, &c_file))!=-1L)
	{
		files.push_back(c_file.name);
		while(_findnext(hFile, &c_file)==0)
		{
			files.push_back(c_file.name);
		}
	}
	_findclose(hFile);
}

#endif

#ifdef __unix__
// Unix file system version of getFileList()
#error No getFileList() currently defined for unix systems...
#error possible clues in c:\cygwin\usr\include\sys\dirent.h
#endif

// store the random seed variable inside a function
// makes it safer than a global variable and
// easier to detect bugs
unsigned long& random_seed()
{
	static unsigned long rs = 0;
	return rs;
}

bool readGNUGoResultsFile(string filename, vector<char>& results, int size)
{
	// resize results to correct boardsize
	// set default values for results to 'M' - neutral point
	// in case any points are not included in the following lists
	results.clear();
	results.resize(size*size, 'M');

	// read in results file
	ifstream in(filename.c_str(), ios::binary);
	if(!in)
		return false;

	// interpret results file and translate into:
	//	for each point it can be A|D|B|W|S|M
	//		A = Alive, D = Dead, B = black territory
	//		W = white territory, S = Seki, M = Dame

	// results file format:
	// = [black|white]
	//
	// = Alive coords (multiline)
	// 
	// = Dead coords (multiline)
	// 
	// = Seki coords (multiline)
	// 
	// = White territory coords (multiline)
	// 
	// = Black territory coords (multiline)
	//
	// = Dame coords (multiline)
	//
	string line;

	// get "= [black|white]" line
	if(!getLine(in, line))
		return false;

	// get "\r" line
	if(!getLine(in, line))
		return false;

	// get Alive coords
	if(!getCoords(in, 'A', results, size))
		return false;

	// get Dead coords
	if(!getCoords(in, 'D', results, size))
		return false;

	// get Seki coords
	if(!getCoords(in, 'S', results, size))
		return false;

	// get White territory coords
	if(!getCoords(in, 'W', results, size))
		return false;

	// get Black territory coords
	if(!getCoords(in, 'B', results, size))
		return false;

	// get Dame coords
	if(!getCoords(in, 'M', results, size))
		return false;

	return true;
}

// extract coordinates from the given line in GTP form
// e.g. "E4 B8 G18 A15"
int getCoords(ifstream& in, char value, vector<char>& results, int size)
{
	int x, y;
	char j;
	char i[3];
	string line;
	if(!getLine(in, line))
		return 0;
	while(line!="\r")
	{
		// delete "= " if at front of line
		if(line.at(0)=='=')
			line.erase(0, 2);
		// loop through the line getting the coords
		int count = 0;
		while((count+1)<line.size())
		{
			// work out column
			j = line.at(count);
			x = tolower((int)j) - 'a';
			if(tolower((int) j)>'i')
				x--;
			// work out row
			if(count+2>=line.size() || line.at(count+2)==' ')
			{
				//y = line.at(count+1)-'1';
				// GNUGO y coords are 1 at bottom to 9 at top - upside down
				// compared to our system, so reverse them
				i[0] = line.at(count+1);
				i[1] = '\0';
				y = size - atoi(i);
				count+=3;
			}
			else
			{
				i[0] = line.at(count+1);
				i[1] = line.at(count+2);
				i[2] = '\0';
				//y = atoi(i)-1;
				// GNUGO y coords are 1 at bottom to 9 at top - upside down
				// compared to our system, so reverse them
				y = size - atoi(i);
				count+=4;
			}
			results.at(y*size+x) = value;
		}
		if(!getLine(in, line))
			return 0;
	}
	return 1;
}

int getLine(ifstream& in, string& line)
{
	if(in.eof())
	{
		cout << "Bad results file." << endl;
		return 0;
	}
	//in >> line;
	char data[2000];
	in.getline(data, 2000);
	line = data;
	return 1;
}

/** @brief This function sets up a board from the given SGF using the 
  * setup information stored within. Note: This function clears the board first. 
  * This function is appropriate to use only at the first node of an SGF file.
  * @see setupBoardFromSGFNode */
void setupBoardFromSGF(SGFReader& sgf, BoardStruct& board)
{
	vector<Move> props;
	board.clear();
	SGFNode* start = &(sgf.getRootNode());
	if(start->getEmptySetup(props))	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), EMPTY, false);
	}
	if(start->getBlackSetup(props))	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), BLACK, false);
	}
	if(start->getWhiteSetup(props))	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), WHITE, false);
	}
}

/** @brief This function sets up a board from the given SGF using the 
  * setup information stored within. Note: This function does _not_ clear the board first. 
  * This function is appropriate to use during a traversal of an SGF file. */
void setupBoardFromSGFNode(SGFReader& sgf, BoardStruct& board, SGFNode* node /*=NULL*/)
{
	vector<Move> props;
	SGFNode* start = node;
	if(node==NULL)
		start = &(sgf.getRootNode());
	if(start->getEmptySetup(props))	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), EMPTY, false);
	}
	if(start->getBlackSetup(props))	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), BLACK, false);
	}
	if(start->getWhiteSetup(props))	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), WHITE, false);
	}
}

void addSGFNodeToBoard(Move* m, SGFReader& sgf, BoardStruct& board, SGFNode* node /*=NULL*/)
{
	// add setup points if any
	vector<Move> props;
	SGFNode* start = node;
	if(node==NULL)
		start = sgf.getCurrentNode();
	if(start->getEmptySetup(props)) {
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), EMPTY, false);
	}
	if(start->getBlackSetup(props)) {
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), BLACK, false);
	}
	if(start->getWhiteSetup(props)) {
		for(int i=0;i<props.size();i++)
			board.setPoint(props.at(i).getX(), props.at(i).getY(), WHITE, false);
	}
	// update board with the move
	if(m!=NULL)
		board.setPoint(m->getX(), m->getY(), m->getColour());
	else {
		Move move;
		if(start->getBlackMove(move) || start->getWhiteMove(move))
			board.setPoint(move.getX(), move.getY(), move.getColour());
	}
}

/** Generate a random value between the specified bounds. 
* @param min The lower bound of the valid range of random numbers required.
* @param max The upper bound of the valid range of random numbers required. 
* @return A random number between the specified bounds.
*/
float getRandom(float min, float max)
{
	int r = rand();
	float m = (float)r/(float)RAND_MAX;
	return (float)((m*((double)max-(double)min))+(double)min);
}

// I'm not sure why but this optimisation doesn't seem to make any
// difference - I guess the compiler must already unroll these
// loops (but how for specific values of size???)
#ifndef _DEBUG
#define LOOPUNROLLING
#endif
#ifdef LOOPUNROLLING
#define unrollInnerLoopErosions(j, jadd, jminus, i) if(in[j][i]>0) { \
		if(i>0 && in[j][i-1]<=0) work[j][i]--; \
		if(i<8 && in[j][i+1]<=0 && work[j][i]>0) work[j][i]--; \
		if(j>0 && in[jminus][i]<=0 && work[j][i]>0) work[j][i]--; \
		if(j<8 && in[jadd][i]<=0 && work[j][i]>0) work[j][i]--; \
	} \
	else if(in[j][i]<0) { \
		if(i>0 && in[j][i-1]>=0) work[j][i]++; \
		if(i<8 && in[j][i+1]>=0 && work[j][i]<0) work[j][i]++; \
		if(j>0 && in[jminus][i]>=0 && work[j][i]<0) work[j][i]++; \
		if(j<8 && in[jadd][i]>=0 && work[j][i]<0) work[j][i]++; \
	}

#define unrollOuterLoopErosions9(i) unrollInnerLoopErosions(0, 1, -1, i); \
	unrollInnerLoopErosions(1, 2, 0, i); \
	unrollInnerLoopErosions(2, 3, 1, i); \
	unrollInnerLoopErosions(3, 4, 2, i); \
	unrollInnerLoopErosions(4, 5, 3, i); \
	unrollInnerLoopErosions(5, 6, 4, i); \
	unrollInnerLoopErosions(6, 7, 5, i); \
	unrollInnerLoopErosions(7, 8, 6, i); \
	unrollInnerLoopErosions(8, 9, 7, i);
#endif

/** @brief This function runs Bouzy's 5/21 algorithm to determine influence.
  * This function assumes dead stones have been removed from the board. 
  * @param ourColour The score is always from blacks point of view. If this parameter
  * is WHITE the score will be negated before returning.
  * @param inCopy A 2 dimensional array will hold the influence values. Format
  * is influenceMap[x][y].
  * @param scaleToStrength If true then the initial influence generated by stones will
  * be scaled according to the GoString's status value. */
bool calculateInfluence(const BoardStruct& b, float& score, int ourColour, 
						string caller, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE] /*=NULL*/,
						int dilations /*=5*/, int erosions /*=21*/, bool scaleToStrength /*=false*/)
{
	/***********************************************************/
	/* Implementation of Bouzy's 5/21 algorithm for estimating */
	/* influence. Uses 5 dilations, 21 erosions as defined in  */
    /* those functions below.                                  */
	/***********************************************************/

	// From GNUGo docs:
	// Referring to page 86 of Bouzy's thesis, we start with a function 
	// taking a high value (ex : +128 for black, -128 for white) on stones 
	// on the goban, 0 to empty intersections. Then iterate dilation
	// and erosion.

	score = 0.0f;
	register int size = b.getSize();
	if(size>MAX_BOARD_SIZE) {
		string message = "calculateInfluence, boardsize exceeds MAX_BOARD_SIZE, returning ";
		LogWriter::printerr(message+"\n", caller);
		return false;
	}
	else if(size<1) {
		string message = "calculateInfluence, boardsize is too small (<1)!, returning ";
		LogWriter::printerr(message+"\n", caller);
		return false;
	}

	const BoardStruct::contentsType& contents = b.getContents();
	int sizeMinusOne = size - 1;
	static int memSize = MAX_BOARD_SIZE*MAX_BOARD_SIZE*4;

	static int in[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	static int work[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	register int k;
	register int p;
	register int i;
	register int j;

	// This value determines the maximum liberties before we assume 
	// a string is alive for the purposes of scaling the initial influence of a stone
	static const int maxStrength = 15;

	// Loop flipping optimisation
//	for(i=0;i<size;i++) {
//		for(j=0;j<size;j++)	{
	i = 0;
	do {
		j = 0;
		do {
			if((p=contents.getValue(j, i))==BLACK) {
				if(scaleToStrength) {
					int status = b.getStringConst(j, i)->getStatus();
					if(status==NOSTATUS)
						in[j][i] = Go::maxInfluenceBlack;
					else {
						// Scale for a maximum of maxStrength liberties - assume life
						if(status>maxStrength) status = maxStrength;
						if(status<DEAD) status = DEAD;
						in[j][i] = Go::maxInfluenceBlack * scaleValue(status, DEAD, maxStrength, 0.0f, 1.0f);;
					}
				}
				else
					in[j][i] = Go::maxInfluenceBlack;
			}
			else if(p==WHITE) {
				if(scaleToStrength) {
					int status = b.getStringConst(j, i)->getStatus();
					if(status==NOSTATUS)
						in[j][i] = Go::maxInfluenceWhite;
					else {
						// Scale for a maximum of maxStrength liberties - assume life
						if(status>maxStrength) status = maxStrength;
						if(status<DEAD) status = DEAD;
						in[j][i] = Go::maxInfluenceWhite * scaleValue(status, DEAD, maxStrength, 0.0f, 1.0f);;
					}
				}
				else
					in[j][i] = Go::maxInfluenceWhite;
			}
			else in[j][i] = 0;
		} while(++j<size);
	} while(++i<size);

	// copy in into work
	memcpy(work, in, memSize);

	for(k=0;k<dilations;k++) {
		// dilation: for each intersection of the goban, if the intersection is >= 0,
		// and not adjacent to a < 0 one, then add to the intersection the number of
		// adjacent >0 intersections. The same for other color : if the intersection 
		// is <= 0, and not adjacent to a > 0 one, then subtract the number of < 0 
		// intersections.
//		for(i=0;i<size;i++)
//		{
//			for(j=0;j<size;j++)
//			{
		// Loop flipping optimisation
		i = 0;
		do {
			j = 0;
			do {
				if(in[j][i]>=0 && 
				   (i==0 || in[j][i-1]>=0) &&
				   (i==sizeMinusOne || in[j][i+1]>=0)	&&
				   (j==0 || in[j-1][i]>=0) &&
				   (j==sizeMinusOne || in[j+1][i]>=0))
				{
					if(i>0 && in[j][i-1]>0)
						work[j][i]++;
					if(i<sizeMinusOne && in[j][i+1]>0)
						work[j][i]++;
					if(j>0 && in[j-1][i]>0)
						work[j][i]++;
					if(j<sizeMinusOne && in[j+1][i]>0)
						work[j][i]++;
				}
				if(in[j][i]<=0 &&
				   (i==0 || in[j][i-1]<=0) &&
				   (i==sizeMinusOne || in[j][i+1]<=0)	&&
				   (j==0 || in[j-1][i]<=0) &&
				   (j==sizeMinusOne || in[j+1][i]<=0))
				{
					if(i>0 && in[j][i-1]<0)
						work[j][i]--;
					if(i<sizeMinusOne && in[j][i+1]<0)
						work[j][i]--;
					if(j>0 && in[j-1][i]<0)
						work[j][i]--;
					if(j<sizeMinusOne && in[j+1][i]<0)
						work[j][i]--;
				}
			} while(++j<size); // end for j
		} while(++i<size); // end for i
		memcpy(in, work, memSize);
	} // end for k
	for(k=0;k<erosions;k++) {
		// erosion: for each intersection > 0 (or < 0), subtract (or add) the number 
		// of adjacent <= 0 (or >= 0) intersection. Stop at zero. The algorithm is just 
		// : 5 dilations, then 21 erosions. The number of erosions should be 1+n(n-1) 
		// where n=number of dilation, since this permit to have an isolated stone to 
		// give no territory. Thus the couple 4/13 also works, but it is often not good,
		// for example when there is territory on the 6th line.
//		for(i=0;i<size;i++)
//		{
//			for(j=0;j<size;j++)
//			{

		// Loop flipping optimisation
		if(size==9) {
#ifdef LOOPUNROLLING
			// Loop unrolling for size==9
			unrollOuterLoopErosions9(0);
			unrollOuterLoopErosions9(1);
			unrollOuterLoopErosions9(2);
			unrollOuterLoopErosions9(3);
			unrollOuterLoopErosions9(4);
			unrollOuterLoopErosions9(5);
			unrollOuterLoopErosions9(6);
			unrollOuterLoopErosions9(7);
			unrollOuterLoopErosions9(8);
#else
			i = 0;
			do {
				j = 0;
				do {
					if(in[j][i]>0) {
						if(i>0 && in[j][i-1]<=0)
							work[j][i]--;
						if(i<8 && in[j][i+1]<=0 && work[j][i]>0)
							work[j][i]--;
						if(j>0 && in[j-1][i]<=0 && work[j][i]>0)
							work[j][i]--;
						if(j<8 && in[j+1][i]<=0 && work[j][i]>0) 
							work[j][i]--;
					}
					else if(in[j][i]<0) {
						if(i>0 && in[j][i-1]>=0)
							work[j][i]++;
						if(i<8 && in[j][i+1]>=0 && work[j][i]<0) 
							work[j][i]++;
						if(j>0 && in[j-1][i]>=0 && work[j][i]<0)
							work[j][i]++;
						if(j<8 && in[j+1][i]>=0 && work[j][i]<0) 
							work[j][i]++;
					}
				} while(++j<9); // end for j
			} while(++i<9); // end for i
#endif
		}
		else {
			i = 0;
			do {
				j = 0;
				do {
					if(in[j][i]>0) {
						if(i>0 && in[j][i-1]<=0)
							work[j][i]--;
						if(i<sizeMinusOne && in[j][i+1]<=0 && work[j][i]>0)
							work[j][i]--;
						if(j>0 && in[j-1][i]<=0 && work[j][i]>0)
							work[j][i]--;
						if(j<sizeMinusOne && in[j+1][i]<=0 && work[j][i]>0)
							work[j][i]--;
					}
					else if(in[j][i]<0) {
						if(i>0 && in[j][i-1]>=0)
							work[j][i]++;
						if(i<sizeMinusOne && in[j][i+1]>=0 && work[j][i]<0)
							work[j][i]++;
						if(j>0 && in[j-1][i]>=0 && work[j][i]<0)
							work[j][i]++;
						if(j<sizeMinusOne && in[j+1][i]>=0 && work[j][i]<0)
							work[j][i]++;
					}
				} while(++j<size); // end for j
			} while(++i<size); // end for i
		}
		memcpy(in, work, memSize);
	} // end for k

	// NOTE: Use chinese scoring rules to get score from influence
	// i.e. each alive stone counts as one point
	// and each point of territory as one point
	// look at dilated/eroded board in 'in' and count score
	// according to BLACK
	register int s;
	if(Go::getRules()==Go::CHINESE_RULES) {

		//	for(i=0;i<size;i++)
		//	{
		//		for(j=0;j<size;j++)
		//		{
		// Loop flipping optimisation
		i = 0;
		do {
			j = 0;
			do {
				// if there is a stone at this point
				if((s = contents.getValue(j, i))==BLACK) score++;
				// white stone
				else if(s==WHITE) score--;
				// black territory
				else if(in[j][i]>0) score++;
				// white territory
				else if(in[j][i]<0)	score--;
			} while(++j<size);
		} while(++i<size);
	}
	else {
		i = 0;
		do {
			j = 0;
			do {
				if(in[j][i]>0) score++;
				// white territory
				else if(in[j][i]<0)	score--;
			} while(++j<size);
		} while(++i<size);
	}

	// negate the score if we're white, since we
	// just worked it out from BLACKs point of view
	if(ourColour==WHITE) score = -score;

	if(inCopy!=NULL) memcpy(inCopy, in, memSize);

	return true;
}

/** @brief Prints the supplied influence map in a board formation to stderr. 
  * @param influenceMap A 2 dimensional array that holds the influence values. Format
  * is influenceMap[x][y].
  */
void printInfluenceMap(int influenceMap[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int boardSize)
{
	LogWriter::printerr("\n");
	char buffer[100];
	for(int i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++) {
			// Print each value as 4 digits including sign
			sprintf(buffer, "%5d ", influenceMap[j][i]);
			LogWriter::printerr(buffer);
		} // end for j
		LogWriter::printerr("\n");
	} // end for i
	LogWriter::printerr("\n");
}

/** @brief This function runs a modified version of Bouzy's 5/21 algorithm to determine influence.
  * This function assumes dead stones have been removed from the board. 
  * In particular this function aims to generate influence values for liberties of stones
  * whereas Bouzy's 5/21 tends to yield neutral values for liberties influenced by enemy stones.
  * This function generates initial influence source values based on the number of stones
  * in the string. During dilation the requirement that a point must not be bordered by enemy
  * influence is ignored.
  *
  * @param ourColour The score is always from blacks point of view. If this parameter
  * is WHITE the score will be negated before returning.
  * @param inCopy A 2 dimensional array will hold the influence values. Format
  * is influenceMap[x][y].
  */
bool calculateLibertyInfluence(const BoardStruct& b, float& score, int ourColour, 
						string caller, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE] /*=NULL*/,
						int dilations /*=3*/, int erosions /*=0*/)
{
	score = 0.0f;
	register int size = b.getSize();
	if(size>MAX_BOARD_SIZE)
	{
		string message = "calculateInfluence, boardsize exceeds MAX_BOARD_SIZE, returning ";
		LogWriter::printerr(message+"\n", caller);
		return false;
	}
	else if(size<1)
	{
		string message = "calculateInfluence, boardsize is too small (<1)!, returning ";
		LogWriter::printerr(message+"\n", caller);
		return false;
	}

	const BoardStruct::contentsType& contents = b.getContents();
	int sizeMinusOne = size - 1;
	static int memSize = MAX_BOARD_SIZE*MAX_BOARD_SIZE*4;

	static int in[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	static int work[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
//	register int k;
	register int p;
	register int i;
	register int j;

	/** @todo Try this:
	  * Initalise points where stones are as maxInfluence value.
	  *
	  * For each stone
	  *   Clear marked points
	  *   For dilations
	  *      reduce influence value exponentially
	  *      radiate influence to adjacent unmarked points
	  *   Next
	  * Next
	  */

	// Initialise points
	Matrix<int> stoneMarkerBoard(size, size, 0);
	i = 0;
	do {
		j = 0;
		do {
			if((p=contents.getValue(j, i))==BLACK) {
				stoneMarkerBoard.setValue(j, i, 1);
				in[j][i] = Go::maxInfluenceBlack;
			}
			else if(p==WHITE) {
				stoneMarkerBoard.setValue(j, i, 1);
				in[j][i] = Go::maxInfluenceWhite;
			}
			else
				in[j][i] = 0;
		} while(++j<size);
	} while(++i<size);


	Matrix<int> markerBoard(size, size, 0);
	const BoardStruct::stringTable& strings = b.getStrings();
	BoardStruct::constStringsIterator citer(strings.begin());
	for(;citer!=strings.end();citer++) {
		const GoString::pointsTable& points = citer->getPoints();
		GoString::constPointsIterator piter(points.begin());
		for(;piter!=points.end();piter++) {
			// Clear marker board
			markerBoard = stoneMarkerBoard;
			int influenceValue = -128;
			if(citer->getColour()==BLACK)
				influenceValue = 128;
			// Add the stone as a seed point
			stack<Point<> > pointsToDilate;
#ifdef _DEBUG
//			int heapStatus;
//			if( (heapStatus=_heapchk()) != _HEAPOK )
//				heapStatus = heapStatus;
#endif
			pointsToDilate.push(*piter);
			for(i=0;i<dilations;i++) {
				// Decrease influence value exponentially
//				influenceValue = influenceValue/(i+2);
				// Halve at every dilation
				influenceValue = influenceValue/2;
				// Radiate influence from all points in pointsToDilate
				stack<Point<> > newPointsToDilate;
				while(!pointsToDilate.empty()) {
					Point<> pt = pointsToDilate.top();
					pointsToDilate.pop();
					// North
					if(pt.y>0 && markerBoard.getValue(pt.x, pt.y-1)==0) {
						in[pt.x][pt.y-1] += influenceValue;
						markerBoard.setValue(pt.x, pt.y-1, 1);
						newPointsToDilate.push(Point<>(pt.x, pt.y-1));
					}
					// South
					if(pt.y<sizeMinusOne && markerBoard.getValue(pt.x, pt.y+1)==0) {
						in[pt.x][pt.y+1] += influenceValue;
						markerBoard.setValue(pt.x, pt.y+1, 1);
						newPointsToDilate.push(Point<>(pt.x, pt.y+1));
					}
					// East
					if(pt.x<sizeMinusOne && markerBoard.getValue(pt.x+1, pt.y)==0) {
						in[pt.x+1][pt.y] += influenceValue;
						markerBoard.setValue(pt.x+1, pt.y, 1);
						newPointsToDilate.push(Point<>(pt.x+1, pt.y));
					}
					// West
					if(pt.x>0 && markerBoard.getValue(pt.x-1, pt.y)==0) {
						in[pt.x-1][pt.y] += influenceValue;
						markerBoard.setValue(pt.x-1, pt.y, 1);
						newPointsToDilate.push(Point<>(pt.x-1, pt.y));
					}
				} // end while !pointsToDilate.empty()
				pointsToDilate = newPointsToDilate;
			}
		}
	}
/*
	i = 0;
	do {
		j = 0;
		do {
			if((p=contents.getValue(j, i))==BLACK)
				in[j][i] = Go::maxInfluenceBlack - (Go::maxInfluenceBlack * (1/(float)(b.getStringConst(j, i)->getStoneCount()+1)));
			else if(p==WHITE)
				in[j][i] = Go::maxInfluenceWhite - (Go::maxInfluenceWhite * (1/(float)(b.getStringConst(j, i)->getStoneCount()+1)));
			else
				in[j][i] = 0;
		} while(++j<size);
	} while(++i<size);

	// copy in into work
	memcpy(work, in, memSize);

	for(k=0;k<dilations;k++)
	{
		i = 0;
		do {
			j = 0;
			do {
				// If intersection is positive, add number of positive neighbours
				if(in[j][i]>0) {
					if(i>0 && in[j][i-1]>0)
						work[j][i]++;
					if(i<sizeMinusOne && in[j][i+1]>0)
						work[j][i]++;
					if(j>0 && in[j-1][i]>0)
						work[j][i]++;
					if(j<sizeMinusOne && in[j+1][i]>0)
						work[j][i]++;
				}
				// If intersection is negative, subtract number of negative neighbours
				else if(in[j][i]<0) {
					if(i>0 && in[j][i-1]<0)
						work[j][i]--;
					if(i<sizeMinusOne && in[j][i+1]<0)
						work[j][i]--;
					if(j>0 && in[j-1][i]<0)
						work[j][i]--;
					if(j<sizeMinusOne && in[j+1][i]<0)
						work[j][i]--;
				}
				// If intersection is 0, add +ve neighbours, subtract -ve neighbours
				else {
					if(i>0) {
						if(in[j][i-1]>0) work[j][i]++;
						else if(in[j][i-1]<0) work[j][i]--;
					}
					if(i<sizeMinusOne) {
						if(in[j][i+1]>0) work[j][i]++;
						else if(in[j][i+1]<0) work[j][i]--;
					}
					if(j>0) {
						if(in[j-1][i]>0) work[j][i]++;
						else if(in[j-1][i]<0) work[j][i]--;
					}
					if(j<sizeMinusOne) {
						if(in[j+1][i]>0) work[j][i]++;
						else if(in[j+1][i]<0) work[j][i]--;
					}
				}
			} while(++j<size); // end for j
		} while(++i<size); // end for i
		memcpy(in, work, memSize);
	} // end for k
	for(k=0;k<erosions;k++)
	{
		// erosion: for each intersection > 0 (or < 0), subtract (or add) the number 
		// of adjacent <= 0 (or >= 0) intersection. Stop at zero. The algorithm is just 
		// : 5 dilations, then 21 erosions. The number of erosions should be 1+n(n-1) 
		// where n=number of dilation, since this permit to have an isolated stone to 
		// give no territory. Thus the couple 4/13 also works, but it is often not good,
		// for example when there is territory on the 6th line.
		if(size==9)
		{
#ifdef LOOPUNROLLING
			// Loop unrolling for size==9
			unrollOuterLoopErosions9(0);
			unrollOuterLoopErosions9(1);
			unrollOuterLoopErosions9(2);
			unrollOuterLoopErosions9(3);
			unrollOuterLoopErosions9(4);
			unrollOuterLoopErosions9(5);
			unrollOuterLoopErosions9(6);
			unrollOuterLoopErosions9(7);
			unrollOuterLoopErosions9(8);
#else
			i = 0;
			do {
				j = 0;
				do {
					if(in[j][i]>0)
					{
						if(i>0 && in[j][i-1]<=0) work[j][i]--;
						if(i<8 && in[j][i+1]<=0 && work[j][i]>0) work[j][i]--;
						if(j>0 && in[j-1][i]<=0 && work[j][i]>0) work[j][i]--;
						if(j<8 && in[j+1][i]<=0 && work[j][i]>0) work[j][i]--;
					}
					else if(in[j][i]<0)
					{
						if(i>0 && in[j][i-1]>=0) work[j][i]++;
						if(i<8 && in[j][i+1]>=0 && work[j][i]<0) work[j][i]++;
						if(j>0 && in[j-1][i]>=0 && work[j][i]<0) work[j][i]++;
						if(j<8 && in[j+1][i]>=0 && work[j][i]<0) work[j][i]++;
					}
				} while(++j<9); // end for j
			} while(++i<9); // end for i
#endif
		}
		else
		{
			i = 0;
			do {
				j = 0;
				do {
					if(in[j][i]>0)
					{
						if(i>0 && in[j][i-1]<=0)
							work[j][i]--;
						if(i<sizeMinusOne && in[j][i+1]<=0 && work[j][i]>0)
							work[j][i]--;
						if(j>0 && in[j-1][i]<=0 && work[j][i]>0)
							work[j][i]--;
						if(j<sizeMinusOne && in[j+1][i]<=0 && work[j][i]>0)
							work[j][i]--;
					}
					else if(in[j][i]<0)
					{
						if(i>0 && in[j][i-1]>=0)
							work[j][i]++;
						if(i<sizeMinusOne && in[j][i+1]>=0 && work[j][i]<0)
							work[j][i]++;
						if(j>0 && in[j-1][i]>=0 && work[j][i]<0)
							work[j][i]++;
						if(j<sizeMinusOne && in[j+1][i]>=0 && work[j][i]<0)
							work[j][i]++;
					}
				} while(++j<size); // end for j
			} while(++i<size); // end for i
		}
		memcpy(in, work, memSize);
	} // end for k
*/
	// NOTE: Use chinese scoring rules to get score from influence
	// i.e. each alive stone counts as one point
	// and each point of territory as one point
	// look at dilated/eroded board in 'in' and count score
	// according to BLACK
	register int s;
	if(Go::getRules()==Go::CHINESE_RULES) {

		i = 0;
		do {
			j = 0;
			do {
				// if there is a stone at this point
				if((s = contents.getValue(j, i))==BLACK) score++;
				// white stone
				else if(s==WHITE) score--;
				// black territory
				else if(in[j][i]>0) score++;
				// white territory
				else if(in[j][i]<0)	score--;
			} while(++j<size);
		} while(++i<size);
	}
	else {
		i = 0;
		do {
			j = 0;
			do {
				if(in[j][i]>0) score++;
				// white territory
				else if(in[j][i]<0)	score--;
			} while(++j<size);
		} while(++i<size);
	}

	// negate the score if we're white, since we
	// just worked it out from BLACKs point of view
	if(ourColour==WHITE) score = -score;

	if(inCopy!=NULL) memcpy(inCopy, in, memSize);

	return true;
}

void calculateBoundary(Rect& dBounds, const BoardStruct& tempBoard, int x, int y)
{
	if(tempBoard.getPoint(x, y)==EMPTY)
		return;
	BoardStruct::constStringsIterator ownerString = tempBoard.getStringConst(x, y);
	const GoString::pointsTable& points = ownerString->getPoints();
	GoString::constPointsIterator iter = points.begin();
	// initialise the bounds
	dBounds.bottom = iter->y;
	dBounds.top = iter->y;
	dBounds.left = iter->x;
	dBounds.right = iter->x;
	for(;iter!=points.end();iter++)
	{
		if(iter->y<dBounds.top)
			dBounds.top = iter->y;
		if(iter->y>dBounds.bottom)
			dBounds.bottom = iter->y;
		if(iter->x<dBounds.left)
			dBounds.left = iter->x;
		if(iter->x>dBounds.right)
			dBounds.right = iter->x;
	}
	// add 2 point border to search area
	dBounds.top-=2;
	dBounds.bottom+=2;
	dBounds.left-=2;
	dBounds.right+=2;
	if(dBounds.top<0) dBounds.top=0;
	if(dBounds.bottom>(tempBoard.getSize()-1)) dBounds.bottom = tempBoard.getSize()-1;
	if(dBounds.left<0) dBounds.left = 0;
	if(dBounds.right>(tempBoard.getSize()-1)) dBounds.right = tempBoard.getSize()-1;
}

void addBoundaries(Rect& bound1, const Rect& bound2)
{
	if(bound2.top<bound1.top) bound1.top = bound2.top;
	if(bound2.bottom>bound1.bottom) bound1.bottom = bound2.bottom;
	if(bound2.right>bound1.right) bound1.right = bound2.right;
	if(bound2.left<bound1.left) bound1.left = bound2.left;
}

void expandBoundaryForExtraStrings(Rect& dBounds, const BoardStruct& tempBoard)
{
	// find strings lying on the boundary edges, these may have
	// stones or relevant points outside the boundary, so expanding 
	// the boundary may be required
	int colour;
	Rect tBounds;
	while(true)
	{
		// check north edge
		if(dBounds.top>0)
		{
			// search edge for a stone on it
			for(int j=dBounds.left;j<=dBounds.right;j++)
			{
				// if we find a string, expand boundary and continue while loop
				if((colour=tempBoard.getPoint(j, dBounds.top))!=EMPTY)
				{
					calculateBoundary(tBounds, tempBoard, j, dBounds.top);
					// add the boundaries together
					addBoundaries(dBounds, tBounds);
					continue;
				}
			} // end for
		} // end check north edge
		
		// check south edge
		if(dBounds.bottom<(tempBoard.getSize()-1))
		{
			// search edge for a stone on it
			for(int j=dBounds.left;j<=dBounds.right;j++)
			{
				// if we find a string, expand boundary and continue while loop
				if((colour=tempBoard.getPoint(j, dBounds.bottom))!=EMPTY )
				{
					calculateBoundary(tBounds, tempBoard, j, dBounds.bottom);
					// add the boundaries together
					addBoundaries(dBounds, tBounds);
					continue;
				}
			} // end for
		} // end check south edge
		
		// check east edge
		if(dBounds.right<(tempBoard.getSize()-1))
		{
			// search edge for a stone on it
			for(int i=dBounds.top;i<=dBounds.bottom;i++)
			{
				// if we find a string, expand boundary and continue while loop
				if((colour=tempBoard.getPoint(dBounds.right, i))!=EMPTY)
				{
					calculateBoundary(tBounds, tempBoard, dBounds.right, i);
					// add the boundaries together
					addBoundaries(dBounds, tBounds);
					continue;
				}
			} // end for
		} // end check east edge
		
		// check west edge
		if(dBounds.left>0)
		{
			// search edge for a stone on it
			for(int i=dBounds.top;i<=dBounds.bottom;i++)
			{
				// if we find a string, expand boundary and continue while loop
				if((colour=tempBoard.getPoint(dBounds.left, i))!=EMPTY)
				{
					calculateBoundary(tBounds, tempBoard, dBounds.left, i);
					// add the boundaries together
					addBoundaries(dBounds, tBounds);
					continue;
				}
			} // end for
		} // end check east edge
		
		// no strings found so...
		break;
	} // end while(true)
}

const char* colourToString(int colour)
{
	if(colour==OFFBOARD)
		return "OFFBOARD";
	else if(colour==EMPTY)
		return "EMPTY";
	else if(colour==BLACK)
		return "BLACK";
	else if(colour==WHITE)
		return "WHITE";
	else if(colour==GOOD_BLACK_MOVE)
		return "GOOD_BLACK_MOVE";
	else if(colour==GOOD_WHITE_MOVE)
		return "GOOD_WHITE_MOVE";
	else if(colour==BAD_BLACK_MOVE)
		return "BAD_BLACK_MOVE";
	else if(colour==BAD_WHITE_MOVE)
		return "BAD_WHITE_MOVE";
	else if(colour==GOOD_BLACK_MOVE_AND_TOP_TEN)
		return "GOOD_BLACK_MOVE_AND_TOP_TEN";
	else if(colour==GOOD_WHITE_MOVE_AND_TOP_TEN)
		return "GOOD_WHITE_MOVE_AND_TOP_TEN";
	else
		return "UNKNOWN";
}

/** Return a value such that it is scaled to be within the 
specified upper and lower bounds of a certain range.
@param value The value to fit to a range.
@param min The lower bound of the range.
@param max The upper bound of the range.
@param newmin The lower bound of the range to scale value to.
@param newmax The upper bound of the range to scale value to.
@return Value scaled to fit within the given range. */
float scaleValue(float value, float min, float max, float newmin, float newmax)
{
	assert(max>=min && newmax>=newmin);

	float scaleFactor = (value-min)/(max-min);
	return scaleFactor*(newmax-newmin)+newmin;
}

myRGB getDistancePixelmyRGB(float v, float distancehigh, float distancelow)
{
	if(v>=0) {
		int c = (int)scaleValue(v, 0, distancehigh, (float)0, (float)255);
		return myRGB(0, 0, c);
	}
	else {
		int c = 255-(int)scaleValue(v, distancelow, 0, (float)0, (float)255);
		return myRGB(c, 0, 0);
	}
}


// create colour images of the weights
// one iamge file per layer
void createLayerImages(BPN& bpn, int sourceType, string& prefix, bool onePanelPerNodeType/*=false*/)
{
	vector<Matrix<float> > weights;
	bpn.getWeights(weights);
	float low = 0.0f;
	float high = 0.0f;
	float offboardlow = 0.0f;
	float offboardhigh = 0.0f;
	float outhigh = 0.0f;
	float outlow = 0.0f;
	float distancehigh = 0.0f;
	float distancelow = 0.0f;
	float v;
	short c;
	int imageWidth, imageHeight, weightsWidth, weightsHeightWithoutDistance;
	vector<myRGB> scanline;

	// if two layers of weights find output layer weights outhigh, outlow
	if(weights.size()==2)
	{
		outlow = outhigh = weights.at(1).getValue(0, 0);
		for(int j=0;j<weights.at(1).height;j++)
		{
			v = weights.at(1).getValue(0, j);
			if(v<outlow)
				outlow = v;
			if(v>outhigh)
				outhigh = v;
		}
	}

	int realType = bpn.getType();
	if(BPN::hasSourceType(realType))
		realType = sourceType;

	bool hasOffboardNodes = BPN::hasOffboardNodes(realType);
	bool has18DistanceNodes = BPN::has18DistanceNodes(realType);
	bool has2DistanceNodes = BPN::has2DistanceNodes(realType);

	for(int l=0;l<weights.size();l++)
	{
		int imageLineIndex = 0;
		int neuronsPerPoint = 1;

		weightsHeightWithoutDistance = weights.at(l).getHeight();
		weightsWidth = weights.at(l).getWidth();


		// if distance nodes here, skip them
		if(l==0 && has2DistanceNodes) weightsHeightWithoutDistance-=2;
		else if(l==0 && has18DistanceNodes) weightsHeightWithoutDistance-=18;

		neuronsPerPoint = BPN::getNeuronsPerPoint(realType);

		// special output format for 1st layer of newbpns
		if(l==0)
		{
			// sqrt(h/(neurons_per_point)) * neurons_per_point

			// height = width * number_hidden_neurons + hidden_neurons*2
			// + hidden_neurons because each hidden neuron picture has a white line 
			// after it to seperate them and a line to show the distance nodes weights
			if(onePanelPerNodeType)
				imageWidth = sqrt(weightsHeightWithoutDistance/neuronsPerPoint)*neuronsPerPoint;
			// width is doubled so we can have another NxM pixel square to 
			// show the offboard neuron weights into these hidden nodes
			else if(neuronsPerPoint==4)
				imageWidth = sqrt(weightsHeightWithoutDistance/neuronsPerPoint)*2;
			else
				imageWidth = sqrt(weightsHeightWithoutDistance/neuronsPerPoint);

			// 2 distance nodes means we one extra line of pixels, so *2
			if(has2DistanceNodes)
				imageHeight = sqrt(weightsHeightWithoutDistance/neuronsPerPoint)*weightsWidth+(weightsWidth*2);
			// for 18 distance nodes we need two lines of pixels, so *3
			else if(has18DistanceNodes)
				imageHeight = sqrt(weightsHeightWithoutDistance/neuronsPerPoint)*weightsWidth+(weightsWidth*3);
			else
				imageHeight = sqrt(weightsHeightWithoutDistance/neuronsPerPoint)*weightsWidth+weightsWidth;
		}
		else
		{
			imageHeight = weightsHeightWithoutDistance;
			imageWidth = weightsWidth;
		}

		BMP bmp;
		bmp.create(24, imageWidth, imageHeight);

		/** File Data **/

		// For 24-bit colour:
		// 3 bytes per pixel: b,g,r saturation levels 1 byte each, 0 for black, 1 for white

		// weight layers are w x h matrices
		// w = number of nodes leaving the layer
		// h = number of nodes entering it

		high = low = weights.at(l).getValue(0, 0);
		for(int i=0;i<weightsHeightWithoutDistance;i++)
		{
			for(int j=0;j<weightsWidth;j++)
			{
				// find highest and lowest value for this layer
				v = weights.at(l).getValue(j, i);
				
				if(v<low) low = v;
				if(v>high) high =v;
			} // end for j
		} // end for i

		// remember pixels are stored bottom-up, left-to-right

		// for the first layer only 
		if(l==0)
		{
			// if NEWBPNTYPE find distancelow and distancehigh from weights from distance nodes
			// is this a distance node?
			if((has18DistanceNodes || has2DistanceNodes) && i>=weightsHeightWithoutDistance)
			{
				distancelow = distancehigh = weights.at(l).getValue(0, weightsHeightWithoutDistance);
				for(int g=0;g<weightsWidth;g++)
				{
					for(int k=0;k<18;k++)
					{
						// only 2 distance nodes in a NEWBPNTYPE
						// 18 in the others
						if(has2DistanceNodes && k==2)
							break;
						v = weights.at(l).getValue(g, weightsHeightWithoutDistance+k);
						if(v<distancelow) distancelow = v;
						if(v>distancehigh) distancehigh = v;
					} // end for k
				} // end for g
			}

			// organise image as blocks for each exiting node from layer
			// for each hidden node
			// start with last hidden node
			for(int j=(weightsWidth-1);j>=0;j--)
			{	
				scanline.clear();
				for(int i=0;i<imageWidth;i++)
					scanline.push_back(myRGB(255, 255, 255));
				bmp.setScanLine(scanline, imageLineIndex++);
				scanline.clear();
				
				// check this is the first layer of weights
				if(l==0 && (has18DistanceNodes || has2DistanceNodes))
				{
					// Add a line(s) to show the distance nodes
					scanline.push_back(getDistancePixelmyRGB(weights.at(l).getValue(j, weightsHeightWithoutDistance), distancehigh, distancelow));
					scanline.push_back(getDistancePixelmyRGB(weights.at(l).getValue(j, weightsHeightWithoutDistance+1), distancehigh, distancelow));
					if(has18DistanceNodes)
					{
						// finish first distance neurons pixel line
						for(int z=2;z<9;z++)
							scanline.push_back(getDistancePixelmyRGB(weights.at(l).getValue(j, weightsHeightWithoutDistance+z), distancehigh, distancelow));
						// fill in rest of pixels in line
						for(int i=0;i<(imageWidth-9);i++)
							scanline.push_back(myRGB(0, 0, 0));
						bmp.setScanLine(scanline, imageLineIndex++);
						scanline.clear();
						// add second distance neurons pixel line
						for(z=9;z<18;z++)
							scanline.push_back(getDistancePixelmyRGB(weights.at(l).getValue(j, weightsHeightWithoutDistance+z), distancehigh, distancelow));
						// fill in rest of pixels in line
						for(i=0;i<(imageWidth-9);i++)
							scanline.push_back(myRGB(0, 0, 0));
						bmp.setScanLine(scanline, imageLineIndex++);
						scanline.clear();
					}
					else
					{
						// fill in rest of pixels in line
						for(int i=0;i<(imageWidth-2);i++)
							scanline.push_back(myRGB(0, 0, 0));
						bmp.setScanLine(scanline, imageLineIndex++);
						scanline.clear();
					}
				}
				// run through the connections from each entering neuron row
				// as on a Go board, starting with the last row then going up
				int neuronLineWidth = sqrt(weightsHeightWithoutDistance/neuronsPerPoint)*neuronsPerPoint;
				for(int start=(weightsHeightWithoutDistance-neuronLineWidth);start>=0;start-=neuronLineWidth)
				{
					int centre = (weightsHeightWithoutDistance/neuronsPerPoint)/2;
					if(!onePanelPerNodeType)
					{
						for(int i=start;i<(start+neuronLineWidth);i++)
						{
							int r, g, b;
							v = weights.at(l).getValue(j, i);
							if(neuronsPerPoint>1)
							{
								// is this the centre point of this block?
								// and this is a two layer network only
								if((i/neuronsPerPoint)==centre && weights.size()==2)
								{
									// only do this once for the 3 input
									// weights for this point, ignore the weights themselves
									if((i%neuronsPerPoint)==0)
									{
										// colour the middle point according to this
										// hidden nodes connection to the output layer
										v = weights.at(l+1).getValue(0, j);
										// if +.v.e. colour it shades of blue
										if(v>=0)
										{
											c = (int)scaleValue(v, 0, outhigh, (float)0, (float)255);
											scanline.push_back(myRGB(0, 0, c));
										}
										// if -.v.e. colour it red
										else
										{
											c = 255-(int)scaleValue(v, outlow, 0, (float)0, (float)255);
											scanline.push_back(myRGB(c, 0, 0));
										}
									}
								}
								// determine whether we are in an OURCOLOUR,THEIRCOLOUR
								// or EMPTY node
								// OURCOLOUR - blue
								else if((i%neuronsPerPoint)==0)
								{
									// b byte
									b = c = (int)scaleValue(v, low, high, (float)0, (float)255);
								}
								// THEIRCOLOUR - red
								else if((i%neuronsPerPoint)==1)
								{
									r = c = (int)scaleValue(v, low, high, (float)0, (float)255);
								}
								// EMPTY - green
								else if((i%neuronsPerPoint)==2)
								{
									g = c =(int)scaleValue(v, low, high, (float)0, (float)255);
									scanline.push_back(myRGB(r, g, b));
								}
								// OFFBOARD - ignore (only used by NEWBPNTYPE3)
								else if(hasOffboardNodes && (i%neuronsPerPoint)==3)
								{
									// do nothing since we only have 3 colours 
									// to play with not 4...
								}
							}
							else
							{
								// -.v.e value as lower case
								if(v<0)
								{
									// -.v.e so output red colour
									c = 255-(int)scaleValue(v, low, 0, (float)0, (float)255);
									scanline.push_back(myRGB(c, 0, 0));
								}
								else
								{
									// +.v.e so output blue colour
									c = (int)scaleValue(v, 0, high, (float)0, (float)255);
									scanline.push_back(myRGB(0, 0, c));
								}
							}
						} // end for i
						/** Handle offboard neurons if we have them **/
						if(hasOffboardNodes)
						{
							// now we're at the end of a pixel line
							// continue line to show offboard connections
							for(int n=start;n<(start+neuronLineWidth);n++)
							{
								if((n%neuronsPerPoint)==3)
								{
									v = weights.at(l).getValue(j, n);
									// +.v.e. colour it blue
									if(v>=0)
									{
										c = (int)scaleValue(v, 0, high, (float)0, (float)255);
										scanline.push_back(myRGB(0, 0, c));
									}
									// -.v.e. colour it red
									else
									{
										c = 255-(int)scaleValue(v, low, 0, (float)0, (float)255);
										scanline.push_back(myRGB(c, 0, 0));
									}
								}
							}
						}
						bmp.setScanLine(scanline, imageLineIndex++);
						scanline.clear();
					} // end if !onePanelPerNodeType
					else
					{
						// add all the node types in separate panels
						// one type at a time
						scanline.clear();
						for(int nodeType=0;nodeType<neuronsPerPoint;nodeType++)
						{
							for(int n=start;n<(start+neuronLineWidth);n++)
							{
								if((n%neuronsPerPoint)==nodeType)
								{
									v = weights.at(l).getValue(j, n);
									// +.v.e. colour it blue
									if(v>=0)
									{
										c = (int)scaleValue(v, 0, high, (float)0, (float)255);
										scanline.push_back(myRGB(0, 0, c));
									}
									// -.v.e. colour it red
									else
									{
										c = 255-(int)scaleValue(v, low, 0, (float)0, (float)255);
										scanline.push_back(myRGB(c, 0, 0));
									}
								}
							} // end for n
						} // end for nodeType
						bmp.setScanLine(scanline, imageLineIndex++);
						scanline.clear();
					} // end else (i.e. onePanelPerNodeType==true)
				} // end for start
			} // end for j
		}
		else {
			// now go through this layer again print the values
			// do the weights upside down since we printed the hidden nodes 
			// upside down too
			scanline.clear();
			for(i=(weightsHeightWithoutDistance-1);i>=0;i--)
			{
				for(int j=0;j<weightsWidth;j++)
				{
					v = weights.at(l).getValue(j, i);
					// -.v.e value as lower case
					if(v<0)
					{
						// -.v.e so output red colour
						c = 255-(int)scaleValue(v, outlow, 0, (float)0, (float)255);
						scanline.push_back(myRGB(c, 0, 0));
					}
					else
					{
						// +.v.e so output blue colour
						c = (int)scaleValue(v, 0, outhigh, (float)0, (float)255);
						scanline.push_back(myRGB(0, 0, c));
					}
				} // end for j
				bmp.setScanLine(scanline, imageLineIndex++);
				scanline.clear();
			} // end for i
		} // end else

		string filename = prefix;
		filename+="layer";
		char buffer[50];
		sprintf(buffer, "%d", l);
		filename+=buffer;
		filename+=".bmp";
		if(!bmp.save(filename.c_str()))
			return;
	} // end for l
}

// Create 8-bit greyscale .raw images of the weights
// one .raw file per layer
void createRawLayerImages(BPN& bpn)
{
	vector<Matrix<float> > weights;
	bpn.getWeights(weights);
	float low = 0;
	float high = 0;
	float v;
	char c;
	short w, h;
	for(int l=0;l<weights.size();l++)
	{
		// write 8-bit grey .raw file
		string filename = bpn.getFilename();
		filename+="layer";
		char buffer[50];
		sprintf(buffer, "%d", l);
		filename+=buffer;
		filename+=".raw";
		ofstream fos(filename.c_str(), ios::binary);
		if(!fos)
		{
			string message = "Could not open file for save: weights.raw";
			LogWriter::printerr(message+"\n");
			return;
		}

		// header size is space taken by width and height values in bytes
		//short headerSize = 4;
		h = weights.at(l).getHeight();
		w = weights.at(l).getWidth();
		// output width and height of raw image
		//fos.write(reinterpret_cast<char*>(&headerSize), sizeof(short));
		fos.write(reinterpret_cast<char*>(&w), sizeof(short));
		fos.write(reinterpret_cast<char*>(&h), sizeof(short));
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				// find highest and lowest value for this layer
				v = weights.at(l).getValue(j, i);
				if(v<low)
					low = v;
				if(v>high)
					high = v;
			} // end for j
		} // end for i
		// now go through this layer again print the values
		for(i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				v = weights.at(l).getValue(j, i);
				// -.v.e value as lower case
				if(v<0)
				{
					//c = (int)bpn.scaleValue(v, low, 0, (float)'0', (float)'4');
					//cout << (char)tolower((int)'Z'-(int)c+(int)'A');
					//cout << (char)tolower((int)c);
					c = (int)scaleValue(v, low, 0, (float)0, (float)127);
					fos.write(reinterpret_cast<char*>(&c), sizeof(char));
				}
				else
				{
					//c = (int)bpn.scaleValue(v, 0, high, (float)'5', (float)'9');
					//cout << c;
					c = (int)scaleValue(v, 0, high, (float)128, (float)255);
					fos.write(reinterpret_cast<char*>(&c), sizeof(char));
				}
			} // end for j
		} // end for i
		fos.close();
	}
}

void spliceWeightImages(string sourceDir, string outputFilename)
{
	char cwd[200];
	if(_getcwd(cwd, 200)==NULL)
	{
		cout << "Error with _getcwd" << endl;
		cout << "Trainnet finished" << endl;
		return;
	}
	if(_chdir(sourceDir.c_str())==-1)
	{
		string message("No directory: ");
		message+=sourceDir;
		LogWriter::printerr(message+="\n");
		return;
	}
	// get ordered list of .bmp files to splice
	vector<string> files;
	getFileList(files, "*layer0.bmp");
	if(files.size()==0)
	{
		string message("No source files");
		LogWriter::printerr(message+="\n");
		return;
	}

	sort(files.begin(), files.end());

	// load all .bmp source files and calculate width and height
	int newWidth = 0;
	int newHeight = 0;
	vector<BMP> bmpSourceFiles;
	BMP bmp;
	for(int i=0;i<files.size();i++)
	{
		if(!bmp.load(files[i].c_str()))
		{
			string message("Could not open file: ");
			message+=files[i];
			LogWriter::printerr(message+="\n");
			return;
		}
		bmpSourceFiles.push_back(bmp);
		// width of new bitmap is total width + 1 pixel per image
		// for a boundary white line between images
		newWidth+=bmpSourceFiles.back().getWidth()+1;
	}

	newHeight = bmpSourceFiles.back().getHeight();

	// change back to current working directory
	_chdir(cwd);

	/** File Header **/
	
	BMP output;
	if(!output.create(24, newWidth, newHeight))
	{
		string message("Failed to create output bitmap");
		LogWriter::printerr(message+"\n");
		return;
	}

	vector<myRGB> oldScanLine;
	for(i=0;i<newHeight;i++)
	{
		vector<BMP>::iterator iter = bmpSourceFiles.begin();
		vector<myRGB> newScanLine;
		for(;iter!=bmpSourceFiles.end();iter++)
		{
			(*iter).getScanLine(oldScanLine, i);
			newScanLine.insert(newScanLine.end(), oldScanLine.begin(), oldScanLine.end());
			// insert a white pixel to seperate the images
			newScanLine.push_back(myRGB(255, 255, 255));
		}
		output.setScanLine(newScanLine, i);
	}
	output.save(outputFilename.c_str());
}

/** Check if the game in the sgf is within the rank range given
@return True if the game is within the supplied range */
bool rankRangeFilterSGF(const SGFReader& sgf, const GoRankRange& range)
{
	if(range.rangeHigh!=-100)
	{
		string blackRank, whiteRank;
		char bGrade, wGrade;
		int bRank, wRank;

		sgf.getBlackPlayerRank(blackRank);
		sgf.getWhitePlayerRank(whiteRank);

		if(sscanf(blackRank.c_str(), "%d%c", &bRank, &bGrade)!=2 || (bGrade!='k' && bGrade!='d'))
			return false;;
		if(sscanf(whiteRank.c_str(), "%d%c", &wRank, &wGrade)!=2 || (wGrade!='k' && wGrade!='d'))
			return false;
		
		// convert rank/grade to int for easier comparison
		if(bGrade=='k')
			bRank=-bRank;
		if(wGrade=='k')
			wRank=-wRank;
		
		// now wRank and bRank can be from -30 to +10
		// -.v.e. indicates kyu level, +.v.e. is dan
		if(bRank<range.rangeLow || wRank<range.rangeLow || bRank>range.rangeHigh || wRank>range.rangeHigh)
			return false;
	}
	return true;
}
/** Used by MTDFSearch ::setup??Search() functions.
  * @param value The string to parse containing information about the goal or goal union to create.
  * @param g The goal object to create the goal in.
  * @param errorString If an error occurs an error message will be placed in this string.
  * @param b A pointer to a boardStruct. This is required so if a capture goal is specified
  * so the function can test if the capture string coordinates contain an actual string.
  * @return True if successful, false otherwise. */
bool parseGoalString(const string& value, Goal& g, string& errorString, const BoardStruct& b)
{
	g.clearGoals();
	
	// interpret value as a string of form:
	// A(x,y)
	// where A is a character indicating type of goal:
	// can be C = captureGoal, or L = LibertyCountGoal
	// and x,y are arg1 and arg2 for the goal.
	if(value.size()<6) {
		errorString = "Invalid goal string: ";
		errorString+=value;
		errorString+="\n";
		return false;
	}
	
	// extract goal type
	char goalType = value.at(0);
	if(value.at(1)!='(') {
		errorString = "Invalid goal string: ";
		errorString+=value;
		errorString+="\n";
		return false;
	}
	
	// extract args
	int t = value.find(',', 2);
	if(t<0) {
		errorString = "Invalid goal string: ";
		errorString+=value;
		errorString+="\n";
		return false;
	}
	
	int n;
	int arg1 = 0;
	int arg2 = 0;
	
	// Check if we have an alphanumeric coord or just a numeric one...
	// Take the first value, if it's a character between 'a'|'A' and 'z'|'Z'...
	string firstArg(value.substr(2, t-2));
	if(firstArg.size()==1 && (firstArg[0]>='A' && firstArg[0]<='Z') ||
		(firstArg[0]>='a' && firstArg[0]<='z')) {
		n = value.find(')', t);
		if(n<0) {
			errorString = "Invalid goal string: ";
			errorString+=value;
			errorString+="\n";
			return false;
		}
		string secondArg(value.substr(t+1, n-(t+1)));
		string coord(firstArg+secondArg);
		// Is the second argument numeric or a character?
		if(secondArg.size()==1 && (secondArg[0]>='A' && secondArg[0]<='Z') ||
			(secondArg[0]>='a' && secondArg[0]<='z')) {
			// try an 'aa' style coord interpretation (SGF)
			arg1 = Move::SGFToX(coord);
			arg2 = Move::SGFToY(coord);
		}
		else {
			// try an 'a1' style coord interpretation
			arg1 = Move::GoPosToX(coord);
			arg2 = Move::GoPosToY(coord, b.getSize());
		}
		// check range
		if(arg1<0 || arg2<0 || arg1>=b.getSize() || arg2>=b.getSize()) {
			errorString = "Invalid goal string: ";
			errorString+=value;
			errorString+="\n";
			return false;
		}
	}
	else {
		// If not character arguments must be a numerical coordinate
		arg1 = atoi(firstArg.c_str());
		if(arg1==0 && value.at(2)!='0') {
			errorString = "Invalid goal string: ";
			errorString+=value;
			errorString+="\n";
			return false;
		}
		n = value.find(')', t);
		if(n<0) {
			errorString = "Invalid goal string: ";
			errorString+=value;
			errorString+="\n";
			return false;
		}
		arg2 = atoi(value.substr(t+1, n-(t+1)).c_str());
		if(arg2==0 && value.at(t+1)!='0') {
			errorString = "Invalid goal string: ";
			errorString+=value;
			errorString+="\n";
			return false;
		}
	}
	
	
	// create new goal object
	if(goalType=='C') {
		// check arg1, arg2 point to an actual string on the board
		if(b.getPoint(arg1, arg2)==EMPTY) {
			errorString = "No string at capture arguments!\n";
			return false;
		}
		g.addGoal(new CaptureGoal(arg1, arg2));
	}
	else if(goalType=='X') {
		// check arg1, arg2 point to an actual string on the board
		if(b.getPoint(arg1, arg2)==EMPTY) {
			errorString = "No string at capture arguments!\n";
			return false;
		}
		g.addGoal(new LibCountCaptureGoal(arg1, arg2));
	}
	else if(goalType=='L')
		g.addGoal(new LibertyCountGoal(arg1, arg2));
	else if(goalType=='I')
		g.addGoal(new InfluenceGoal(arg1, arg2));
	else if(goalType=='S')
		g.addGoal(new StoneCountGoal(arg1, arg2));
	else {
		errorString = "Unrecognised goal type: ";
		errorString+=goalType;
		errorString+="\n";
		return false;
	}
	return true;
}

/** Timer functions **/
#ifdef _WIN32
static LARGE_INTEGER _tstart, _tend;
static LARGE_INTEGER _freq;

/** Start the static timer **/
void timerStart(void)
{
	static int first = 1;
	
	if(first) {
		QueryPerformanceFrequency(&_freq);
		first = 0;
	}
	QueryPerformanceCounter(&_tstart);
}

/** End the static timer **/
void timerEnd(void)
{
	QueryPerformanceCounter(&_tend);
}

/** @brief Get the value of the static timer in seconds passed between calls
 *  to timerStart() and timerEnd(). Resolution should be microseconds,
 * return in seconds.*/
double timerValue()
{
	return ((double)_tend.QuadPart -
		(double)_tstart.QuadPart)/((double)_freq.QuadPart);
}


/** Start the timer **/
void timerStart(LARGE_INTEGER* tstart, LARGE_INTEGER* freq)
{
	// Note: Always query the performance frequency for non-static timers
//	static int first = 1;
	
//	if(first) {
		QueryPerformanceFrequency(freq);
//		first = 0;
//	}
	QueryPerformanceCounter(tstart);
}

/** End the timer **/
void timerEnd(LARGE_INTEGER* tend)
{
	QueryPerformanceCounter(tend);
}

/** @brief Get the time in seconds passed between calls
 *  to timerStart() and timerEnd(). Resolution should be microseconds,
 * return in seconds.*/
double timerValue(LARGE_INTEGER& tstart, LARGE_INTEGER& tend, LARGE_INTEGER& freq)
{
	return ((double)tend.QuadPart -
		(double)tstart.QuadPart)/((double)freq.QuadPart);
}

#else // _WIN32

static struct timeval _tstart, _tend;
static struct timezone tz;

/** Start the static timer **/
void timerStart(void)
{
	gettimeofday(&_tstart, &tz);
}

/** End the static timer **/
void timerEnd(void)
{
	gettimeofday(&_tend,&tz);
}

/** @brief Get the value of the static timer in seconds passed between calls
 *  to timerStart() and timerEnd(). Resolution should be microseconds,
 * return in seconds.*/
double timerValue()
{
	double t1, t2;
	
	t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
	t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
	return t2-t1;
}

void timerStart(clock_t* tstart)
{
	*tstart = clock();
}

void timerEnd(clock_t* tend)
{
	*tend = clock();
}

/** @brief Get the time in seconds passed between calls
 *  to timerStart() and timerEnd(). Resolution should be microseconds,
 * return in seconds.*/
double timeValue(clock_t tstart, clock_t tend)
{
	return (double) (tend - tstart) / CLOCKS_PER_SEC;
}

#endif