#pragma warning( disable : 4786 )

/** A class to represent a Go move. It implements Comparable so that a
list of moves may be ordered by their plausibility score.
@see Comparable
@see Move#compareTo */

#include "globals.h"
#include "Move.h"
#include "Misc.h"
//using namespace std;

/* NOTE Coordinate systems:
	GoPos (standard):	a-s, no i, 1-19, bottom left corner is a1, pass is ""
	x,y				:	0-18, top left is 0,0, pass is -1, -1
	GMP				:	0-360, top left is 0, pass it -1
*/

void Move::printInfo(bool verbose, string* printTo /*=NULL*/) const 
{
	if(verbose)
	{
		char buffer[150];
		sprintf(buffer, "x=%d y=%d colour=%s boardSize=%d, NNScore=%*g, alphaBetaScore=%d\n",
			x, y, colourToString(colour), boardSize, 7, NNScore, alphaBetaScore);
		if(printTo!=NULL)
			(*printTo)+=buffer;
		else
			LogWriter::printerr(buffer);
	}
	else
	{
		char buffer[50];
		if(colour==BLACK)
			sprintf(buffer, "B[%s] ", Move::xyToGoPosValue(x, y, boardSize).c_str());
		else if(colour==WHITE)
			sprintf(buffer, "W[%s] ", Move::xyToGoPosValue(x, y, boardSize).c_str());
		else
			sprintf(buffer, "?[%s] ", Move::xyToGoPosValue(x, y, boardSize).c_str());
		if(printTo!=NULL)
			(*printTo)+=buffer;
		else
			LogWriter::printerr(buffer);
	}
}

