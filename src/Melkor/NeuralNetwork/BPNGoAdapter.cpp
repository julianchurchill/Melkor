#pragma warning( disable : 4786 )

#include "globals.h"
#include "BPNGoAdapter.h"
#include "LogWriter.h"


/** Generate a matrix containing appropriate representation of an area
of the board centred around the given move so it can be fed 
directly into a BPN network. The area size is determined by the
value of PATTERNWIDTH and PATTERNHEIGHT.
@param x The x coordinate to centre on.
@param y The y coordinate to centre on.
@param b The Board object from which to extract an area.
@param input A Matrix<float>& into which to put the generated matrix.
@param colour The colour of the side to play in this board position.
@return A boolean indicating success (always true). */
bool BPNGoAdapter::getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const
{
	const BoardStruct::contentsType& t = b.getContents();
	float act = 0;
	// do not include the bias value for input
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
	int width = t.getWidth();
	int height = t.getHeight();

	int count = 0;
	vector<SpecialPoint> points;
	getInputFieldPoints(t, points, x, y);
	vector<SpecialPoint>::const_iterator citer = points.begin();
	for(;citer!=points.end();citer++)
	{
		if(citer->type==OFFBOARD)
			input.setValue(0, count++, ACT_OFFBOARD);
		else
			input.setValue(0, count++, getActivationValue(citer->type, colour));
	}

	// find equivalent input values
/*	for(int i=0;i<pHeight;i++)
	{
		offsety = i+toplefty;
		for(int j=0;j<pWidth;j++)
		{
			offsetx = j+topleftx;
			// check bounds
			if(offsetx<0 || offsety<0 || offsetx>=width || offsety>=height)
				input.setValue(0, count++, ACT_OFFBOARD);
			else
				input.setValue(0, count++, getActivationValue(t.getValue(offsetx, offsety), colour));
		} // end for j
	} // end for i
*/

	return true;
}
  
//float BPNGoAdapter::scorePosition(BoardStruct& b,
//							Matrix<int>* boardResult /*=NULL*/,
//							bool printScores /*=false*/)
/*{
	LogWriter log;
	string message;
	char buffer[50];
	float score = 0.0f;
	Matrix<float> input;
	vector<Matrix<float> > outputs;
	if(!getInput(0, 0, b, input, BLACK))
	{
		log.println("Error: The loaded BPN network does not match the size of this board.");
		return 0.0f;
	}
	getAnswer(input, outputs);

	int s = b.getContents()->getWidth();

	//score = outputs.at(getNumberOfLayers()-1).getValue(0, 0);

	// assuming output node for each point
	// add up point values and return as score
	Matrix<float>& output = outputs.at(getNumberOfLayers()-1);
	for(int i=0;i<output.getHeight();i++)
	{
		score+=output.getValue(0, i);
		// check if the caller wants details of who owns which point
		int y = i/s;
		int x = i-(y*s);
		if(boardResult!=NULL)
		{
			if(output.getValue(0, i)>NEUTRAL_HIGH_THRESHOLD)
				boardResult->setValue(x, y, BLACK);
			else if(output.getValue(0, i)<NEUTRAL_LOW_THRESHOLD)
				boardResult->setValue(x, y, WHITE);
			else
				boardResult->setValue(x, y, EMPTY);
		}
		if(printScores)
		{
			message = "[";
			message+=Move::xyToGoPosValue(x, y, s);
			message+="] = ";
			sprintf(buffer, "%*g", 4, output.getValue(0, i));
			message+=buffer;
			log.print(message);
		}
	}
	if(printScores)
		log.print("\n");
	return score;
}
*/