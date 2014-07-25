#pragma warning( disable : 4786 )

#include "globals.h"
#include "newBPN3GoAdapter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** Return a matrix containing appropriate representation of an area
of the board centred around the given move so it can be fed 
directly into a BPN network. The area size is determined by the
value of PATTERNWIDTH and PATTERNHEIGHT.
@param x The x coordinate to centre on.
@param y The y coordinate to centre on.
@param b The Board object from which to extract an area.
@param input A return parameter to store the converted, NN ready input matrix. 
@param colour The colour whose point of view we are creating this input pattern for. I.e. which colour
this move is being scored for. */
bool newBPN3GoAdapter::getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const
{
	const BoardStruct::contentsType& t = b.getContents();
	float act = 0;

	// board contents should be rotated so that nearest two board edges
	// are the top and left, this gets rid of all rotational symmetry
	int width = t.getWidth();
	int height = t.getHeight();
	int leftdist = x;
	int rightdist = width-x-1; 
	int topdist = y;
	int bottomdist = height-y-1;

	bool topcloser = true;
	bool leftcloser = true;

	// top left sides always preferred if equidistant from two edges
	if(topdist>bottomdist)
		topcloser = false;
	if(leftdist>rightdist)
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
		x = rightdist;
		y = bottomdist;
	}
	// if top right edges closer
	else if(topcloser && !leftcloser)
	{
		// rotate temp 270 clw
		temp2.doTransform(temp, Matrix_ROTATE270);
		x = topdist;
		y = rightdist;
	}
	// if bottom left edges closer
	else if(!topcloser && leftcloser)
	{
		// rotate temp 90 clw
		temp2.doTransform(temp, Matrix_ROTATE90);
		x = bottomdist;
		y = leftdist;
	}
	// no rotation
	else
		temp = temp2;

	// 3 units per point + 18 units for distance to nearest board edges
	input.resize(1, getBPN().getWeights()[0].getHeight());
	// now extract contents of board around this point

	// values to work around area centred on x,y
	// extending as PATTERNWIDTHxPATTERNHEIGHT
	int pWidth = getPatternWidth();
	int pHeight = getPatternHeight();
	int topleftx = x-(pWidth/2);
	int toplefty = y-(pHeight/2);

	int offsetx = 0;
	int offsety = 0;

	int count = 0;
	vector<SpecialPoint> points;
	getInputFieldPoints(temp, points, x, y);
	vector<SpecialPoint>::const_iterator citer = points.begin();
	for(;citer!=points.end();citer++)
	{
		if(citer->type==OFFBOARD)
			setPoint(input, count++, ACT_OFFBOARD);
		else
			setPoint(input, count++, getActivationValue(citer->type, colour));
	}
	
/*	// find equivalent input values
	for(int i=0;i<pHeight;i++)
	{
		offsety = i+toplefty;
		for(int j=0;j<pWidth;j++)
		{
			offsetx = j+topleftx;
			// check bounds	to see if this point is off the board
			if(offsetx<0 || offsety<0 || offsetx>=width || offsety>=height)
				setPoint(input, count, ACT_OFFBOARD);
			else
				setPoint(input, count, getActivationValue(temp.getValue(offsetx, offsety), colour));
			count++;
		} // end for j
	} // end for i
*/
	// add distance to edge values for last 18 neurons
	// 9 neurons for top edge distance
	// 9 neurons for left edge distance
	// use binary type system
	// all off except the nth neuron indicating the distance

	// the distance to top and left edges (now the nearest after rotation)
	// should now be x, y

	// top distance
	for(int i=0;i<9;i++)
	{
		if((i+1)==x)
			input.setValue(0, (count*4)+i, 1);
		else
			input.setValue(0, (count*4)+i, 0);
	}

	// left distance
	for(i=0;i<9;i++)
	{
		if((i+1)==y)
			input.setValue(0, ((count*4)+9)+i, 1);
		else
			input.setValue(0, ((count*4)+9)+i, 0);
	}

	//input.setValue(0, count*3, x);
	//input.setValue(0, (count*3)+1, y);
	return true;
}

// functions used to generate input, set 3 unit points etc...
void newBPN3GoAdapter::setPoint(Matrix<float>& m, int y, int value) const
{
	m.setValue(0, y*4, 0);
	m.setValue(0, y*4+1, 0);
	m.setValue(0, y*4+2, 0);
	m.setValue(0, y*4+3, 0);
	if(value==ACT_OURCOLOUR)
		m.setValue(0, y*4, 1);
	else if(value==ACT_THEIRCOLOUR)
		m.setValue(0, y*4+1, 1);
	else if(value==ACT_EMPTY)
		m.setValue(0, y*4+2, 1);
	else if(value==ACT_OFFBOARD)
		m.setValue(0, y*4+3, 1);
}