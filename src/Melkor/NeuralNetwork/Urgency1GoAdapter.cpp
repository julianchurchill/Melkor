#pragma warning( disable : 4786 )

#include "globals.h"
#include "Urgency1GoAdapter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** Return a matrix containing appropriate representation of an area
of the board centred around the given move so it can be fed 
directly into a BPN network. The area size is determined by the
value of PATTERNWIDTH and PATTERNHEIGHT.
@params x The x coordinate to centre on.
@params y The y coordinate to centre on.
@params board The Board object from which to extract an area. */
//Matrix* BPNGoAdapter::getInput(int x, int y, Board* b)
bool Urgency1GoAdapter::getInput(int x1, int y1, int x2, int y2, const BoardStruct& b, Matrix<float>& input,
								 int colour) const
{
	if(x1<0 || y1<0 || x2<0 || y2<0)
		return false;

	const BoardStruct::contentsType& t = b.getContents();
	float act = 0;

	// board contents should be rotated so that nearest two board edges
	// are the top and left, this gets rid of all rotational symmetry
	int width = t.getWidth();
	int height = t.getHeight();
	int leftdist1 = x1;
	int rightdist1 = width-x1-1; 
	int topdist1 = y1;
	int bottomdist1 = height-y1-1;
	int leftdist2 = x2;
	int rightdist2 = width-x2-1;
	int topdist2 = y2;
	int bottomdist2 = height-y2-1;

	bool topcloser = true;
	bool leftcloser = true;

	// top left sides always preferred if equidistant from two edges
	if(topdist1>bottomdist1)
		topcloser = false;
	if(leftdist1>rightdist1)
		leftcloser = false;

	// copy board into a matrix so we can rotate later
	BoardStruct::contentsType temp2(t);
	BoardStruct::contentsType temp(temp2.width, temp2.height);

	// if bottom right edges closer
	if(!topcloser && !leftcloser)
	{
		// rotate temp 180 clw
		temp2.doTransform(temp, Matrix_ROTATE180);
		// translate x and y now to match newly rotated board
		x1 = rightdist1;
		y1 = bottomdist1;
		x2 = rightdist2;
		y2 = bottomdist2;
	}
	// if top right edges closer
	else if(topcloser && !leftcloser)
	{
		// rotate temp 270 clw
		temp2.doTransform(temp, Matrix_ROTATE270);
		x1 = topdist1;
		y1 = rightdist1;
		x2 = topdist2;
		y2 = rightdist2;
	}
	// if bottom left edges closer
	else if(!topcloser && leftcloser)
	{
		// rotate temp 90 clw
		temp2.doTransform(temp, Matrix_ROTATE90);
		x1 = bottomdist1;
		y1 = leftdist1;
		x2 = bottomdist2;
		y2 = leftdist2;
	}
	// no rotation
	else
		temp = temp2;

	// 5 units per point
	input.resize(1, getBPN().getWeights()[0].getHeight());
	
	int count = 0;

	// NOTE: getInputFieldPoints() is not needed here since urgency nets require the whole board as input
/*	vector<SpecialPoint> points;
	getInputFieldPoints(temp, points, x, y);
	vector<SpecialPoint>::const_iterator citer = points.begin();
	for(;citer!=points.end();citer++)
	{
		setPoint(input, count++, getActivationValue(citer->type, colour));
	}
*/	
	// find equivalent input values
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			setPoint(input, count, getActivationValue(temp.getValue(j, i), colour));
			count++;
		}
	} // end for i

	// set MOVE A and MOVE B
	input.setValue(0, (y1*height+x1)*5+3, 1);
	input.setValue(0, (y2*height+x2)*5+3, 1);

	return true;
}

// functions used to generate input, set 3 unit points etc...
void Urgency1GoAdapter::setPoint(Matrix<float>& m, int y, int value) const
{
	m.setValue(0, y*5, 0);
	m.setValue(0, y*5+1, 0);
	m.setValue(0, y*5+2, 0);
	m.setValue(0, y*5+3, 0);
	m.setValue(0, y*5+4, 0);
	if(value==ACT_OURCOLOUR)
		m.setValue(0, y*5, 1);
	else if(value==ACT_THEIRCOLOUR)
		m.setValue(0, y*5+1, 1);
	else if(value==ACT_EMPTY)
		m.setValue(0, y*5+2, 1);
}