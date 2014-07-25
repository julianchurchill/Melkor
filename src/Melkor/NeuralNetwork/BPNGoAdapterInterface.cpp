#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
#include "SGFReader.h"
#include "Go.h"
#include "Misc.h"
#include <algorithm>

const float BPNGoAdapterInterface::ACT_OURCOLOUR = (float)1.0;
const float BPNGoAdapterInterface::ACT_THEIRCOLOUR = (float)-1.0;
const float BPNGoAdapterInterface::ACT_EMPTY = (float)2.0;
const float BPNGoAdapterInterface::ACT_OFFBOARD = (float)-2.0;

const float BPNGoAdapterInterface::ACT_BLACK = (float)0.9;
const float BPNGoAdapterInterface::ACT_WHITE = (float)-0.9;

const float BPNGoAdapterInterface::BLACK_TERRITORY_THRESHOLD = (float)0.9;
const float BPNGoAdapterInterface::WHITE_TERRITORY_THRESHOLD = (float)-0.9;
const float BPNGoAdapterInterface::NEUTRAL_HIGH_THRESHOLD = (float)0.0;
const float BPNGoAdapterInterface::NEUTRAL_LOW_THRESHOLD = (float)-0.0;

/* Return a list of points that are within the input field, given it's shape and size.
@param board The board to take the points from, passed as a BoardStruct::contentsType.
@param points A vector to put the points into. */
void BPNGoAdapterInterface::getInputFieldPoints(const BoardStruct::contentsType& board, vector<SpecialPoint>& points,
												int x, int y) const
{
	// set points.type to the colour or OFFBOARD if point is off the board

	points.clear();

	if(internalBPN->getInputFieldShape()==IFS_SQUARE) {
		// values to work around area centred on x,y
		// extending as PATTERNWIDTHxPATTERNHEIGHT
		int width = board.width;
		int height = board.height;

		int pWidth = getPatternWidth();
		int pHeight = getPatternHeight();
		int topleftx = x-(pWidth/2);
		int toplefty = y-(pHeight/2);

		int offsetx = 0;
		int offsety = 0;

		// scan from left to right, top to bottom
		int i, j;
		for(i=0;i<pHeight;i++) {
			offsety = toplefty + i;
			for(j=0;j<pWidth;j++) {
				offsetx = topleftx + j;
				if(offsetx<0 || offsetx>=width || offsety<0 || offsety>=height)
					points.push_back(SpecialPoint(offsetx, offsety, OFFBOARD, 0.0f));
				else
					points.push_back(SpecialPoint(offsetx, offsety, board.getValue(offsetx, offsety), 0.0f));
			}
		}
	}
	else if(internalBPN->getInputFieldShape()==IFS_DIAMOND)	{
		// values to work around area centred on x,y
		// extending as PATTERNWIDTHxPATTERNHEIGHT
		int width = board.width;
		int height = board.height;

		int pWidth = getPatternWidth();
		int pHeight = getPatternHeight();
		int topleftx = x-(pWidth/2);
		int toplefty = y-(pHeight/2);

		int offsetx = 0;
		int offsety = 0;

		// scan from left to right, top to bottom
		int i, j;
		for(i=0;i<pHeight;i++) {
			offsety = toplefty + i;

			// find absolute value of distance from point in y axis
			int yDist = 0;
			if(y>offsety)
				yDist = y-offsety;
			else
				yDist = offsety-y;

			// adding to the row start and subtracting from the row end
			// allows a diamond shape to be scanned
			//   .
			//  ...
			// .....
			//  ...
			//   .
			for(j=0+yDist;j<(pWidth-yDist);j++) {
				offsetx = topleftx + j;
				if(offsetx<0 || offsetx>=width || offsety<0 || offsety>=height)
					points.push_back(SpecialPoint(offsetx, offsety, OFFBOARD, 0.0f));
				else
					points.push_back(SpecialPoint(offsetx, offsety, board.getValue(offsetx, offsety), 0.0f));
			}
		}
	}
}

#include <functional>

/* Confirm that the number of proposed board points is a valid size for a particular input field shape.
@param numberOfBoardPoints The proposed number of board points to contain within the input field shape.
@param error A string containing text describing any validation problems.
@return true if valid, false otherwise. */
bool BPNGoAdapterInterface::validateInputFieldSize(int numberOfBoardPoints, string& error) const
{
	// only allow symmetrical sized fields, i.e. widht = height
	if(internalBPN->getInputFieldShape()==IFS_SQUARE) {
		// input nodes must be perfectly square rootable
		double intFloat;
		if(modf(sqrt((double)numberOfBoardPoints), &intFloat)!=0) {
			error = "numberOfBoardPoints must be a square number for IFS_SQUARE";
			return false;
		}
		return true;
	}
	// diamond width and height is from leftmost to rightmost and top to bottom
	else if(internalBPN->getInputFieldShape()==IFS_DIAMOND) {
		// numberOfBoardPoints should be ((patternWidth*patternHeight) + 1) / 2
		// and patternWidth and patternHeight should be the same

		// numberOfBoardPoints*2-1 should be a square number
		double test = numberOfBoardPoints*2-1;
		double intFloat;
		if(modf(sqrt(test), &intFloat)!=0) {
			error = "(numberOfBoardPoints * 2) - 1 must be a square number for IFS_DIAMOND\n i.e. input nodes should be = {((patternWidth*patternHeight)+1)/2} * nodesPerBoardPoint + extraNodes";
			return false;
		}
		return true;
	}

	error = "Unknown field shape!";
	return false;
}

bool BPNGoAdapterInterface::validateParameters(int inputNodes, vector<int>& hiddenNodes, int outputNodes, string& error) const
{
	// input nodes - extraNodes should be divisable by nodesPerPoint
	if(((inputNodes-extraNodes)%nodesPerBoardPoint)!=0) {
		char buffer[200];
		sprintf(buffer, "Number of input nodes - %d must be divisable by %d (nodesPerBoardPoint)", extraNodes, nodesPerBoardPoint);
		error = buffer;
		return false;
	}

	int numberOfBoardPoints = (inputNodes-extraNodes)/nodesPerBoardPoint;
	return validateInputFieldSize(numberOfBoardPoints, error);
}

/** Generate the appropriate input value for a BPN network, for a
given piece type. The activation values are defined by ACT_BLACK,
ACT_WHITE, ACT_EMPTY and ACT_OFFBOARD. 
@param c A value representing a type of piece; BLACK, WHITE or
EMPTY, as defined in the globals.h class.
@param colour The colour of the current player.
@return The activation value corresponding to the piece type.
@see globals.h */
float BPNGoAdapterInterface::getActivationValue(int c, int colour) const
{
	// always do it from BLACKs perspective

	if(c==colour)
		return ACT_OURCOLOUR;
	else if(c==BLACK || c==WHITE)
		return ACT_THEIRCOLOUR;
	else if(c==EMPTY)
		return ACT_EMPTY;
	else
		return ACT_OFFBOARD;
}

float BPNGoAdapterInterface::getOutput(int x, int y, const BoardStruct& b, int colour)
{
	vector<Matrix<float> > temp(getBPN().getNumberOfLayers());
	Matrix<float> input;
	getInput(x, y, b, input, colour);
	getBPN().getAnswer(input);
	return getBPN().getOutputs()[getBPN().getNumberOfLayers()-1].getValue(0, 0);
}

// result returned is the average percentage position of the actual
// move, within the NN scored moves.
// e.g. result = 20% means the actual move is on average scored as the 
// 20% best move...
bool BPNGoAdapterInterface::rankTest(const vector<string>& sgffiles, float& result, vector<pair<int,float> >* distrib /*=NULL*/, 
		bool printDetails /*=false*/, bool hiddenNodeInfo /*=false*/, vector<float>* hiddenInfo /*=NULL*/,
		int printEveryN /*=-1*/, int everyNPercent /*=-1*/)
{
	LogWriter log;
	string message;
	char buffer[200];

	int totalRank = 0;
	int totalMoves = 0;

	SGFReader sgf;
	BoardStruct board;
	Matrix<float> input;
	Move m;
	vector<Move> moves;
	Matrix<float>* answers;
	int totalNetworkRuns = 0;

	vector<Move> changedPoints;

	if(sgffiles.size()==0)
	{
		cout << "Error: no sgf files to test BPN against." << endl;
		return false;
	}

	if(distrib!=NULL)
	{
		if(everyNPercent<=0 || everyNPercent>100 || (100%everyNPercent)!=0)
		{
			cout << "Error: everyNPercent must be between 1 and 100 and exactly divisible into 100." << endl;
			return false;
		}
		distrib->clear();
		distrib->resize(100/everyNPercent);
	}

	int fileCount = 0;
	for(int fileNumber=0;fileNumber<sgffiles.size();fileNumber++)
	{
		message = "Testing: ";
		message+=sgffiles[fileNumber];
		log.println(message);
		if(!sgf.load(sgffiles[fileNumber]))
		{
			message = "Skipping ";
			message+=sgffiles[fileNumber];
			log.println(message);
			continue;
		}
		fileCount++;

		board.clear();

		vector<float> rankOverPastNMoves;
		vector<float> cumulativeRank;
		Move current;
		int count = 0;
		int totalNMoveRank = 0;
		int actualTotalMoves = 0;
		int gameRank = 0;
		int gameMoves = 0;

		// do a quick move count now
		while(sgf.peekNextMove(current))
		{
			sgf.getNextMove(current);
			actualTotalMoves++;
		}

		// reset sgf file
		sgf.load(sgffiles[fileNumber]);

		Go::resetIllegalMoveMemory(board, true);

		int k = -1;
		int kCount = 0;
		int i = 0;
		int currentColour = BLACK;
		int currentX, currentY;
		int numberOfLayers = getBPN().getNumberOfLayers();

		while(sgf.peekNextMove(current))
		{
			//log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);
			currentColour = current.getColour();
			currentX = current.getX();
			currentY = current.getY();
			// use bpn to rank all moves
			moves.clear();
			for(int y=0;y<board.getSize();y++)
			{
				for(int x=0;x<board.getSize();x++)
				{
//					if(Go::isLegal(board, x, y, current.getColour(), true))
					if(Go::isLegal(board, x, y, current.getColour(), true, true))
					{
						getInput(x, y, board, input, current.getColour());
						getBPN().getAnswer(input);
						answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);
						m.setNNScore(answers->getValue(0, 0));
						m.setX(x);
						m.setY(y);
						m.setColour(current.getColour());
						moves.push_back(m);
						// Hidden node info if requested
						if(hiddenNodeInfo)
						{
							// use temp which contains all the outputs for all layers
							// of network and sum score for each hidden node
							for(int i=0;i<getBPN().getOutputs()[1].getHeight();i++)
								hiddenInfo->push_back(getBPN().getOutputs()[1].getValue(0, i));
							totalNetworkRuns++;
						}
					} // end if EMPTY
				} // end for x
			} // end for y

			// sort the list in order of score
			// lowest first
			sort(moves.begin(), moves.end());

			// find actual move in the bpn list
			int size = moves.size();
			for(k=0;k<size;k++)
			{
				if(current.getX()==moves[k].getX() && current.getY()==moves[k].getY())
					break;
			}

			totalMoves++;
			// lowest score is first in moves vector
			float newpercent =((float)(size-k)/(float)size)*(float)100;
			totalRank+=(moves.size()-k);
			gameRank+=(moves.size()-k);
			totalNMoveRank+=(moves.size()-k);
			gameMoves++;
			cumulativeRank.push_back((float)gameRank/(float)gameMoves);
			count++;
			if(printEveryN>0 && count==printEveryN)
			{
				rankOverPastNMoves.push_back((float)totalNMoveRank/(float)printEveryN);
				count = 0;
				totalNMoveRank = 0;
			}
			// do we need to record the distribution?
			if(distrib!=NULL)
			{
				// save how many times a value
				// occurs in each distrib->size()%
				int bucket = newpercent/everyNPercent;
				// watch out for newpercent==100%
				assert(bucket<=distrib->size());
				if(bucket==distrib->size()) bucket = distrib->size()-1;
				(*distrib)[bucket].first = (*distrib)[bucket].first+1;
				// if we're printing details and this move
				// is one of the 50% worst moves...
				if(printDetails && bucket>=(distrib->size()-(distrib->size()/2)))
				{
					// print the move and move number
					sprintf(buffer, "Worst %d%% move found: (%d", (distrib->size()-bucket)*(100/distrib->size()), sgf.getCurrentDepth());
					message = buffer;
					message+=") ";
					message+=Move::xyToGoPosValue(moves[k].getX(), moves[k].getY(), moves[k].getBoardSize());
					message+=" : ";
					sprintf(buffer, "%*g", 9, moves[k].getNNScore());
					message+=buffer;
					log.println(message);
				}
			}
			// add move to board
			board.setPoint(currentX, currentY, currentColour);

			// update changedPoints
			changedPoints.clear();
			vector<Point<> >& cStones = board.getCapturedStonesList();
			for(int j=0;j<cStones.size();j++) {
				Point<>& p = cStones[j];
				changedPoints.push_back(Move(p.x, p.y, EMPTY, board.getSize()));
				// this checks that we've not just committed suicide
				assert(!(p.x==m.getX() && p.y==m.getY()));
			}
			// add stone just placed...
			changedPoints.push_back(Move(currentX, currentY, currentColour, board.getSize()));

			// update the illegalMoveMemory now with a call to isLegal() with changedPoints
			Go::isLegal(board, 0, 0, BLACK, true, true, &changedPoints);

		} // end while
		// output extra stats if needed
		if(printEveryN>0) {
			message = "  Average rank over every ";
			sprintf(buffer, "%d", printEveryN);
			message+=buffer;
			message+=" moves: ";
			for(int i=0;i<rankOverPastNMoves.size();i++) {
				sprintf(buffer, "%*g ", 2, rankOverPastNMoves[i]);
				message+=buffer;
			}
			log.println(message);
			message = "  Current average rank: ";
			for(i=0;i<cumulativeRank.size();i++)
			{
				if(i%printEveryN==0)
				{
					sprintf(buffer, "%*g ", 2, cumulativeRank[i]);
					message+=buffer;
				}
			}
			log.println(message);
		}
		log.print("\n");
	}

	// calculate averages for hiddenNodeInfo
	if(hiddenNodeInfo)
	{
		for(int i=0;i<hiddenInfo->size();i++)
			(*hiddenInfo)[i]/=(float)totalNetworkRuns;
	}

	// store average rank of pros move
	float average = (float)totalRank/(float)totalMoves;

	message = "Average rank for test set: ";
	sprintf(buffer, "%*g, totalMoves = %d, avg. moves per game = %d", 7, average, totalMoves, (totalMoves/fileCount));
	message+=buffer;
	log.println(message);

	result = average;

	// calculate probability of move being ranked in a particular percentile
	if(distrib!=NULL)
	{
		for(int i=0;i<distrib->size();i++)
			(*distrib)[i].second = ((float)(*distrib)[i].first/(float)totalMoves)*100;
	}

	return true;
}

float BPNGoAdapterInterface::localRankTest(const vector<string>& sgffiles, vector<pair<int,float> >* distrib /*=NULL*/,
		bool printDetails /*=false*/, bool hiddenNodeInfo /*=false*/,
		vector<float>* hiddenInfo /*=NULL*/, int printEveryN /*=-1*/, int everyNPercent /*=-1*/)
{
	LogWriter log;
	string message;
	char buffer[200];

	float totalRank = 0;
	int totalMoves = 0;

	SGFReader sgf;
	BoardStruct board;
	Matrix<float> input;
	Move m;
	vector<Move> moves;
	Matrix<float>* answers;
	int totalNetworkRuns = 0;
	int xPatternCentre = (getPatternWidth()-1)/2;
	int yPatternCentre = (getPatternHeight()-1)/2;

	vector<Move> changedPoints;

	if(sgffiles.size()==0)
	{
		cout << "Error: no sgf files to test BPN against." << endl;
		return false;
	}

	if(distrib!=NULL)
	{
		if(everyNPercent<=0 || everyNPercent>100 || (100%everyNPercent)!=0)
		{
			cout << "Error: everyNPercent must be between 1 and 100 and exactly divisible into 100." << endl;
			return false;
		}
		distrib->clear();
		distrib->resize(100/everyNPercent);
	}

	if(hiddenNodeInfo)
	{
		// resize hiddenInfo to the number of hidden nodes
		// = weights[0].width()
		hiddenInfo->resize(getBPN().getWeights()[0].getWidth());
	}

	int fileCount = 0;
	for(int fileNumber=0;fileNumber<sgffiles.size();fileNumber++)
	{
		message = "Testing: ";
		message+=sgffiles[fileNumber];
		log.println(message);
		if(!sgf.load(sgffiles[fileNumber]))
		{
			message = "Skipping ";
			message+=sgffiles[fileNumber];
			log.println(message);
			continue;
		}
		fileCount++;

		board.clear();

		vector<float> rankOverPastNMoves;
		vector<float> cumulativeRank;
		Move current;
		int count = 0;
		int k = 0;
		int totalNMoveRank = 0;
		int actualTotalMoves = 0;
		int gameRank = 0;
		int gameMoves = 0;

		// do a quick move count now
		while(sgf.peekNextMove(current))
		{
			sgf.getNextMove(current);
			actualTotalMoves++;
		}

		// reset sgf file
		sgf.load(sgffiles[fileNumber]);

		while(sgf.peekNextMove(current))
		{
			//log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);

			// use bpn to rank all moves
			// within the nxn area around the actual move
			// that this network covers
			moves.clear();

			// work out top left start point
			int starty = current.getY()-yPatternCentre;
			int startx = current.getX()-xPatternCentre;
			int endy = starty+getPatternHeight();
			int endx = startx+getPatternWidth();

			for(int y=starty;y<endy;y++)
			{
				for(int x=startx;x<endx;x++)
				{
					// check this point is still on the board and legal
//					if(x>=0 && y>=0 && x<board.getSize() && y<board.getSize() &&
//						Go::isLegal(board, x, y, current.getColour(), true))
					if(x>=0 && y>=0 && x<board.getSize() && y<board.getSize() &&
						Go::isLegal(board, x, y, current.getColour(), true, true))
					{
						getInput(x, y, board, input, current.getColour());
						getBPN().getAnswer(input);
						answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);
						m.setNNScore(answers->getValue(0, 0));
						m.setX(x);
						m.setY(y);
						m.setColour(current.getColour());
						moves.push_back(m);
						// Hidden node info if requested
						if(hiddenNodeInfo)
						{
							// use temp which contains all the outputs for all layers
							// of network and sum score for each hidden node
							for(int i=0;i<getBPN().getOutputs()[1].getHeight();i++)
								(*hiddenInfo)[i]+=getBPN().getOutputs()[1].getValue(0, i);
							totalNetworkRuns++;
						}
					} // end if EMPTY
				} // end for x
			} // end for y

			// sort the list in order of score
			// lowest first
			sort(moves.begin(), moves.end());

			// find actual move in the bpn list
			int size = moves.size();
			for(k=0;k<size;k++)
			{
				if(current.getX()==moves[k].getX() && current.getY()==moves[k].getY())
					break;
			}

			totalMoves++;
			// lowest score is first in moves vector
			// make rank a percentage position
			// totalRank+=moves.size()-i;
//#error why is this a percentage instead of an average rank, like in the global rank test?
			float newpercent =((float)(size-k)/(float)size)*(float)100;
//			totalRank+=newpercent;
			totalRank+=moves.size()-k;
			gameRank+=moves.size()-k;
			totalNMoveRank+=moves.size()-k;
			gameMoves++;
			cumulativeRank.push_back((float)gameRank/(float)gameMoves);
			count++;
			if(printEveryN>0 && count==printEveryN)
			{
				// output rank over the past N moves
//				sprintf(buffer, "%*g", 2, (float)totalNMoveRank/(float)printEveryN);
//				log.print(buffer);
				rankOverPastNMoves.push_back((float)totalNMoveRank/(float)printEveryN);
				// output cumulative rank score
//				sprintf(buffer, "(%*g) ", 2, (float)totalRank/(float)actualTotalMoves);
//				message = buffer;
//				log.print(message);
				count = 0;
				totalNMoveRank = 0;
			}
			// do we need to record the distribution?
			if(distrib!=NULL)
			{
				// save how many times a value
				// occurs in each distrib->size()%
				int bucket = newpercent/everyNPercent;
				// watch out for newpercent==100%
				assert(bucket<=distrib->size());
				if(bucket==distrib->size()) bucket = distrib->size()-1;
				(*distrib)[bucket].first = (*distrib)[bucket].first+1;
				// if we're printing details and this move
				// is one of the 50% worst moves...
				if(printDetails && bucket>=(distrib->size()-(distrib->size()/2)))
				{
					// print the move and move number
					sprintf(buffer, "Worst %d%% move found: (%d", (distrib->size()-bucket)*(100/distrib->size()), sgf.getCurrentDepth());
					message = buffer;
					message+=") ";
					message+=Move::xyToGoPosValue(moves[k].getX(), moves[k].getY(), moves[k].getBoardSize());
					message+=" : ";
					sprintf(buffer, "%*g", 9, moves[k].getNNScore());
					message+=buffer;
					log.println(message);
				}
			}
			// add move to board
			board.setPoint(current.getX(), current.getY(), current.getColour());

			// update changedPoints
			changedPoints.clear();
			vector<Point<> >& cStones = board.getCapturedStonesList();
			for(int j=0;j<cStones.size();j++)
			{
				Point<>& p = cStones[j];
				changedPoints.push_back(Move(p.x, p.y, EMPTY, board.getSize()));
				// this checks that we've not just committed suicide
				assert(!(p.x==m.getX() && p.y==m.getY()));
			}
			// add stone just placed...
			changedPoints.push_back(Move(current.getX(), current.getY(), current.getColour(), board.getSize()));

			// update the illegalMoveMemory now with a call to isLegal() with changedPoints
			Go::isLegal(board, 0, 0, BLACK, true, true, &changedPoints);
		}
		// output extra stats if needed
		if(printEveryN>0)
		{
			message = "  Average local rank over every ";
			sprintf(buffer, "%d", printEveryN);
			message+=buffer;
			message+=" moves: ";
			for(int i=0;i<rankOverPastNMoves.size();i++)
			{
				sprintf(buffer, "%*g ", 2, rankOverPastNMoves[i]);
				message+=buffer;
			}
			log.println(message);
			message = "  Current average local rank: ";
			for(i=0;i<cumulativeRank.size();i++)
			{
				if(i%printEveryN==0)
				{
					sprintf(buffer, "%*g ", 2, cumulativeRank[i]);
					message+=buffer;
				}
			}
			log.println(message);
		}		
		log.print("\n");
	}

	// calculate averages for hiddenNodeInfo
	if(hiddenNodeInfo)
	{
		for(int i=0;i<hiddenInfo->size();i++)
			(*hiddenInfo)[i]/=(float)totalNetworkRuns;
	}

	// store average rank of pros move
	float average = (float)totalRank/(float)totalMoves;

	message = "Average rank for test set locally: ";
	sprintf(buffer, "%*g, totalMoves = %d, avg. moves per game = %d", 7, average, totalMoves, (totalMoves/fileCount));
	message+=buffer;
	message+="\n";
	log.println(message);

	// calculate probability of move being ranked in a particular percentile
	if(distrib!=NULL)
	{
		for(int i=0;i<distrib->size();i++)
			(*distrib)[i].second = ((float)(*distrib)[i].first/(float)totalMoves)*100;
	}

	return average;
}

// find what percentage of the time we score the move
// actually played, as the top move
float BPNGoAdapterInterface::topNMoveFrequencyTest(const vector<string>& sgffiles, int topNMoves, int printEveryN /*=-1*/)
{
	float score = 0.0f;
	float totalScore = 0.0f;
	string message;
	LogWriter log;
	char buffer[200];

	SGFReader sgf;
	BoardStruct board;
	Matrix<float> input;
//	vector<Matrix<float> > temp(getBPN().getNumberOfLayers());
	Move m;
	vector<Move> moves;
	Matrix<float>* answers;
	int totalMoves = 0;
	int totalCorrect[361];

	int currentColour, targetX, targetY;

	if(topNMoves<1 || topNMoves>361)
	{
		cout << "Error: topNMoveFrequencyTest() must have topNMoves parameter >0 and <361." << endl;
		return 0.0f;
	}
	
	for(int i=0;i<topNMoves;i++)
		totalCorrect[i] = 0;

	if(sgffiles.size()==0)
	{
		cout << "Error: no sgf files to test BPN against." << endl;
		return 0.0f;
	}

	for(int fileNumber=0;fileNumber<sgffiles.size();fileNumber++)
	{
		message = "Testing: ";
		message+=sgffiles[fileNumber];
		log.println(message);
		if(!sgf.load(sgffiles[fileNumber]))
		{
			message = "Skipping ";
			message+=sgffiles[fileNumber];
			log.println(message);
			continue;
		}

		board.clear();

		vector<float> cumulativeRank;
		Move current;
		int actualTotalMoves = 0;
		int gameMoves = 0;
		int gameCorrect[361];
		for(int j=0;j<361;j++)
			gameCorrect[j] = 0;

		// do a quick move count now
		while(sgf.peekNextMove(current))
		{
			sgf.getNextMove(current);
			actualTotalMoves++;
		}

		// reset sgf file
		sgf.load(sgffiles[fileNumber]);

		while(sgf.peekNextMove(current))
		{
			log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);
			// use bpn to rank all moves
			moves.clear();
			currentColour = current.getColour();
			targetX = current.getX();
			targetY = current.getY();
			for(int y=0;y<board.getSize();y++)
			{
				for(int x=0;x<board.getSize();x++)
				{
					if(Go::isLegal(board, x, y, currentColour, true))
					{
						getInput(x, y, board, input, currentColour);
						getBPN().getAnswer(input);
						answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);
						m.setNNScore(answers->getValue(0, 0));
						m.setX(x);
						m.setY(y);
						m.setColour(currentColour);
						moves.push_back(m);
					} // end if EMPTY
				} // end for x
			} // end for y

			// sort the list in order of score
			// lowest first
			sort(moves.begin(), moves.end());

			// did we score the actual move in the top n?
			int size = moves.size();
/*			for(int j=0;j<topNMoves;j++)
			{
				if(size>j && moves[size-(j+1)].getX()==targetX && moves[size-(j+1)].getY()==targetY)
				{
					totalCorrect[j]++;
					gameCorrect[j]++;
				}
			}
*/

			// find actual move in the bpn list
			int j;
			for(j=0;j<size;j++)
			{
				if(targetX==moves[j].getX() && targetY==moves[j].getY())
					break;
			}
			// if we have found it amongst our moves i.e. it wasn't a pass...
			if(j<size)
			{
				j = (size - (j+1));
				// register move has been ranked at j
				if(j<topNMoves)
					totalCorrect[j]++;
				gameCorrect[j]++;
			}

			totalMoves++;
			gameMoves++;
			// add move to board
			board.setPoint(targetX, targetY, currentColour);
			cumulativeRank.push_back((float)gameCorrect[0]/(float)gameMoves);
		}
		// output extra stats if needed
		if(printEveryN>0)
		{
			message = "  Current average 1st place: ";
			for(i=0;i<cumulativeRank.size();i++)
			{
				if(i%printEveryN==0)
				{
					sprintf(buffer, "%*g ", 2, cumulativeRank[i]);
					message+=buffer;
				}
			}
			log.println(message);
		}
		log.print("\n");
	}

	message = "Rank\tNo.moves\tPercentage\tCumulative";
	log.println(message);

	for(i=0;i<topNMoves;i++)
	{
		score = ((float)totalCorrect[i]/(float)totalMoves)*(float)100;
		totalScore+=score;
		sprintf(buffer, "%d\t%d\t%*g%\t%*g%", i+1, totalCorrect[i], 2, score, 2, totalScore);
		message = buffer;
		log.println(message);
	}

	return score;
}

// find what percentage of the time we get score the move
// actually played, as the top move
float BPNGoAdapterInterface::localTopNMoveFrequencyTest(const vector<string>& sgffiles, int topNMoves, int manhattanSize, int printEveryN /*=-1*/)
{
	float score = 0.0f;
	float totalScore = 0.0f;
	string message;
	LogWriter log;
	char buffer[200];

	SGFReader sgf;
	BoardStruct board;
	Matrix<float> input;
//	vector<Matrix<float> > temp(getBPN().getNumberOfLayers());
	Move m;
	vector<Move> moves;
	Matrix<float>* answers;
	int totalMoves = 0;
	int totalCorrect[361];

	int targetX, targetY, startX, startY, endX, endY;
	int currentColour;

	if(topNMoves<1 || topNMoves>361)
	{
		cout << "Error: localTopNMoveFrequencyTest() must have topNMoves parameter >0 and <361." << endl;
		return 0.0f;
	}
	
	for(int i=0;i<topNMoves;i++)
		totalCorrect[i] = 0;

	if(sgffiles.size()==0)
	{
		cout << "Error: no sgf files to test BPN against." << endl;
		return 0.0f;
	}

	for(int fileNumber=0;fileNumber<sgffiles.size();fileNumber++)
	{
		message = "Testing: ";
		message+=sgffiles[fileNumber];
		log.println(message);
		if(!sgf.load(sgffiles[fileNumber]))
		{
			message = "Skipping ";
			message+=sgffiles[fileNumber];
			log.println(message);
			continue;
		}

		board.clear();

		vector<float> cumulativeRank;
		Move current;
		int actualTotalMoves = 0;
		int gameMoves = 0;
		int gameCorrect[361];
		for(int j=0;j<361;j++)
			gameCorrect[j] = 0;

		// do a quick move count now
		while(sgf.peekNextMove(current))
		{
			sgf.getNextMove(current);
			actualTotalMoves++;
		}

		// reset sgf file
		sgf.load(sgffiles[fileNumber]);

		while(sgf.peekNextMove(current))
		{
			log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);
			// use bpn to rank all legal moves around the one played
			// within a manhattan distance of manhattanSize
			moves.clear();
//			for(int y=0;y<board.getSize();y++)
//			{
//				for(int x=0;x<board.getSize();x++)
//				{
			currentColour = current.getColour();
			targetY = current.getY();
			startY = targetY-manhattanSize;
			endY = targetY+manhattanSize;
			for(int y=startY;y<endY;y++)
			{
				targetX = current.getX();
				startX = targetX-manhattanSize;
				endX = targetX+manhattanSize;
				for(int x=startX;x<endX;x++)
				{
//					if(board.getPoint(x, y)==EMPTY)

					// check bounds, manhattan distance and legality of move
					if(x>=0 && y>=0 && x<board.getSize() && y<board.getSize() && (abs(targetX-x)+abs(targetY-y))<=manhattanSize
						&& Go::isLegal(board, x, y, currentColour, true))
					{
						getInput(x, y, board, input, current.getColour());
						getBPN().getAnswer(input);
						answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);
						m.setNNScore(answers->getValue(0, 0));
						m.setX(x);
						m.setY(y);
						m.setColour(current.getColour());
						moves.push_back(m);
					} // end if EMPTY
				} // end for x
			} // end for y

			// sort the list in order of score
			// lowest first
			sort(moves.begin(), moves.end());

			// did we score the actual move in the top 10?
			int size = moves.size();
/*			for(int j=0;j<topNMoves;j++)
			{
				if(size>j && moves[size-(j+1)].getX()==current.getX() && moves[size-(j+1)].getY()==current.getY())
				{
					totalCorrect[j]++;
					gameCorrect[j]++;
				}
			}
*/

			// find actual move in the bpn list
			int j;
			for(j=0;j<size;j++)
			{
				if(targetX==moves[j].getX() && targetY==moves[j].getY())
					break;
			}
			// if we have found it amongst our moves i.e. it wasn't a pass...
			if(j<size)
			{
				j = (size - (j+1));
				// register move has been ranked at j
				if(j<topNMoves)
					totalCorrect[j]++;
				gameCorrect[j]++;
			}

			totalMoves++;
			gameMoves++;
			// add move to board
			board.setPoint(targetX, targetY, currentColour);
			cumulativeRank.push_back((float)gameCorrect[0]/(float)gameMoves);
		} // end while(sgf.peekNextMove(current))
		// output extra stats if needed
		if(printEveryN>0)
		{
			message = "  Current average local 1st place: ";
			for(int i=0;i<cumulativeRank.size();i++)
			{
				if(i%printEveryN==0)
				{
					sprintf(buffer, "%*g ", 2, cumulativeRank[i]);
					message+=buffer;
				}
			}
			log.println(message);
		}
		log.print("\n");
	}

	message = "Rank\tNo.moves\tPercentage\tCumulative";
	log.println(message);

	for(i=0;i<topNMoves;i++)
	{
		score = ((float)totalCorrect[i]/(float)totalMoves)*(float)100;
		totalScore+=score;
		sprintf(buffer, "%d\t%d\t%*g%\t%*g%", i+1, totalCorrect[i], 2, score, 2, totalScore);
		message = buffer;
		log.println(message);
	}

	return score;
}

// Use the neural network to score the move played
// and a random legal alternative within the manhattanSize distance
// and measure how often the neural net score the actual move higher
float BPNGoAdapterInterface::twoMoveChoiceTest(const vector<string>& sgffiles, int manhattanSize)
{
	float score = 0.0f;
	float totalScore = 0.0f;
	char buffer[200];
	LogWriter log;

	SGFReader sgf;
	BoardStruct board;
	Matrix<float> input;
//	vector<Matrix<float> > temp(getBPN().getNumberOfLayers());
	Move m;
	Matrix<float>* answers;
	int totalMoves = 0;
	int totalCorrect = 0;
	int gameMoves, gameCorrect;

	vector<Move> legalMoves;
	Move current;

	int targetX, targetY, startX, startY, endX, endY;
	int currentColour;

	if(sgffiles.size()==0)
	{
		cout << "Error: no sgf files to test BPN against." << endl;
		return 0.0f;
	}

	for(int i=0;i<sgffiles.size();i++)
	{
		sprintf(buffer, "Testing: %s", sgffiles[i].c_str());
		log.println(buffer);
		if(!sgf.load(sgffiles[i]))
		{
			string message = "Skipping ";
			message+=sgffiles[i];
			log.println(message);
			continue;
		}

		board.clear();

		gameMoves = 0;
		gameCorrect = 0;

		while(sgf.peekNextMove(current))
		{
			log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);

			// first count legal moves in manhattanSize manhattan distance
			// from actual move
			legalMoves.clear();
			currentColour = current.getColour();
			targetY = current.getY();
			startY = targetY-manhattanSize;
			endY = targetY+manhattanSize;
			for(int y=startY;y<endY;y++)
			{
				targetX = current.getX();
				startX = targetX-manhattanSize;
				endX = targetX+manhattanSize;
				for(int x=startX;x<endX;x++)
				{
					// check legality and that it's not the actual move itself
					if(Go::isLegal(board, x, y, currentColour, true) && !(x==targetX && y==targetY))
						legalMoves.push_back(Move(x, y, currentColour, board.getSize()));
				}
			}

			// now choose a random one from the moves selected
			int choice = rand()%legalMoves.size();

			// now score the random move
			getInput(legalMoves[choice].getX(), legalMoves[choice].getY(), board, input, legalMoves[choice].getColour());
			getBPN().getAnswer(input);
			answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);
			legalMoves[choice].setNNScore(answers->getValue(0, 0));

			// now score the actual move
			getInput(targetX, targetY, board, input, currentColour);
			getBPN().getAnswer(input);
			answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);

			// compare the two, did we score the actual move higher?
			if(answers->getValue(0, 0)>legalMoves[choice].getNNScore())
				gameCorrect++;

			gameMoves++;
			// add move to board
			board.setPoint(targetX, targetY, currentColour);
		}
		score = ((float)gameCorrect/(float)gameMoves)*(float)100;
		sprintf(buffer, "\nFor game %s NN chose the actual move %*g%% of the time.\n", sgffiles[i].c_str(), 2, score);
		log.println(buffer);

		totalCorrect+=gameCorrect;
		totalMoves+=gameMoves;
	}

	score = ((float)totalCorrect/(float)totalMoves)*(float)100;
	sprintf(buffer, "\nAveraged over all games NN chose the actual move %*g%% of the time.\n", 2, score);
	log.println(buffer);

	return score;
}

bool BPNGoAdapterInterface::safetyTest(const vector<string>& sgffiles, float& result,
							  bool printDetails /*=false*/)
{
	// use getOutput() to get properly arranged output from the net
	// +.v.e = alive, -.v.e = dead
//	BoardStruct board;
//	int x, y, colour;
//	x = 0;
//	y = 0;
//	colour = BLACK;
//	result = getOutput(x, y, *(board.getContents()), colour);
//#error write this test

	LogWriter log;
	char buffer[50];
	string message;

	BoardStruct board;
	SGFReader sgf;
	vector<SpecialPoint> moveResults;
	vector<SpecialPoint>::iterator iter;
	vector<Point<> > capturedStones;
	int size = 19;
	string v;
	Move m;
	float score;
	int correctDeathPredictions = 0;
	int incorrectLifePredictions = 0;
	int totalDeathPredictions = 0;
	int correctLifePredictions = 0;
	int incorrectDeathPredictions = 0;
	int totalLifePredictions = 0;

	// for each sgf test file:
	for(int i=0;i<sgffiles.size();i++)
	{
		moveResults.clear();
		// step through and record the result value of each move
		// by location as a SpecialPoint
		sgf.load(sgffiles[i]);
		if(!sgf.getBoardSize(v))
			size = 19;
		else
			size = atoi(v.c_str());
		board.clear(size);
		setupBoardFromSGF(sgf, board);
		while(sgf.getNextMove(m))
		{
			// put move on board
			addSGFNodeToBoard(&m, sgf, board);

			// if it's a pass move, skip it
			if(m.getX()==-1 || m.getY()==-1)
				continue;
			
			// find what safety net thinks of this move
			score = getOutput(m.getX(), m.getY(), board, m.getColour());
			moveResults.push_back(SpecialPoint(m.getX(), m.getY(), 0, score));

			// check the captured stones to see if the safety net predicted they would die
			// when placed...
			capturedStones = board.getCapturedStonesList();
			for(int j=0;j<capturedStones.size();j++)
			{
				// find our score for the original move at this point
				iter = moveResults.begin();
				for(;iter!=moveResults.end();iter++)
				{
					if(iter->x==capturedStones[j].x && iter->y==capturedStones[j].y)
						break;
				}
				// did  we predict this stone would die?
				if(iter->extra<=0.1)
					correctDeathPredictions++;
				totalDeathPredictions++;

				// did we predict this stone would live?
				if(iter->extra>=0.9)
				{
					if(printDetails)
					{
						message = "Incorrect life prediction in: ";
						message+=sgffiles[i];
						message+=" (";
						sprintf(buffer, "%d", sgf.getCurrentDepth());
						message+=buffer;
						message+=") ";
						message+=Move::xyToGoPosValue(capturedStones[j].x, 
							capturedStones[j].y, size);
						log.println(message);
					}
					incorrectLifePredictions++;
				}
				// remove iter from the moveResults list
				// since we have finished with it and don't want to 
				// mix it up with any later stones played at this point
				moveResults.erase(iter);
			} // end for j (capturedStonesList)
		} // end while(sgf.getNextMove(m))

		// now we have come to the end of the sgf
		// test how many of the remaining stones we predicted
		// would still be alive (assume all remaining stones are alive)
		for(int j=0;j<moveResults.size();j++)
		{
			if(moveResults[j].extra>=0.9)
				correctLifePredictions++;
			if(moveResults[j].extra<=0.1)
			{
				if(printDetails)
				{
					message = "Incorrect death prediction in: ";
					message+=sgffiles[i];
					message+=" (";
					sprintf(buffer, "%d", sgf.getCurrentDepth());
					message+=buffer;
					message+=") ";
					message+=Move::xyToGoPosValue(moveResults[j].x, 
						moveResults[j].y, size);
					log.println(message);
				}
				incorrectDeathPredictions++;
			}
		} // end for j
		totalLifePredictions+=moveResults.size();
	}

	// work out % of correct death predictions
	float deathResult = ((float)correctDeathPredictions/(float)totalDeathPredictions)*(float)100;
	message = "Correct death predicting (stones we said would die): ";
	sprintf(buffer, "%*g", 9, deathResult);
	message+=buffer;
	message+="%";
	message+=" (";
	sprintf(buffer, "%d", correctDeathPredictions);
	message+=buffer;
	message+="/";
	sprintf(buffer, "%d", totalDeathPredictions);
	message+=buffer;
	message+=")";
	log.println(message);
	float incorrectDeathResult = ((float)incorrectLifePredictions/(float)totalDeathPredictions)*(float)100;
	message = "Incorrect life predicting (stones we said would live, but died): ";
	sprintf(buffer, "%*g", 9, incorrectDeathResult);
	message+=buffer;
	message+="%";
	message+=" (";
	sprintf(buffer, "%d", incorrectLifePredictions);
	message+=buffer;
	message+="/";
	sprintf(buffer, "%d", totalDeathPredictions);
	message+=buffer;
	message+=")";
	log.println(message);
	int uncertain = (totalDeathPredictions-correctDeathPredictions)-incorrectLifePredictions;
	message = "Uncertain (stones that died and we weren't sure about): ";
	sprintf(buffer, "%d", uncertain);
	message+=buffer;
	log.println(message);

	// work out % of correct life predictions
	float lifeResult = ((float)correctLifePredictions/(float)totalLifePredictions)*(float)100;
	message = "Correct life predicting (stones we said would live): ";
	sprintf(buffer, "%*g", 9, lifeResult);
	message+=buffer;
	message+="%";
	message+=" (";
	sprintf(buffer, "%d", correctLifePredictions);
	message+=buffer;
	message+="/";
	sprintf(buffer, "%d", totalLifePredictions);
	message+=buffer;
	message+=")";
	log.println(message);
	float incorrectLifeResult = ((float)incorrectDeathPredictions/(float)totalLifePredictions)*(float)100;
	message = "Incorrect death predicting (stones we said would die, but lived): ";
	sprintf(buffer, "%*g", 9, incorrectLifeResult);
	message+=buffer;
	message+="%";
	message+=" (";
	sprintf(buffer, "%d", incorrectDeathPredictions);
	message+=buffer;
	message+="/";
	sprintf(buffer, "%d", totalLifePredictions);
	message+=buffer;
	message+=")";
	log.println(message);
	uncertain = (totalLifePredictions-correctLifePredictions)-incorrectDeathPredictions;
	message = "Uncertain (stones that lived and we weren't sure about): ";
	sprintf(buffer, "%d", uncertain);
	message+=buffer;
	log.println(message);


	// set result to average percentage of correct predictions
	//int correctPredictions = correctDeathPredictions+correctLifePredictions;
	//int totalPredictions = totalDeathPredictions+totalLifePredictions;
	//result = ((float)correctPredictions/(float)totalPredictions)*(float)100;
	result = (float)(deathResult+lifeResult)/(float)2;

	message = "Correct life and death predicting averaged: ";
	sprintf(buffer, "%*g", 9, result);
	message+=buffer;
	message+="%";
	log.println(message);

	return true;
}

bool BPNGoAdapterInterface::evalTest(const string& sgftestdir, float& result,
		bool hiddenNodeInfo /*=false*/, vector<float>* hiddenInfo /*=NULL*/)
{
	// get the file list
	char cwd[200];
	if(_getcwd(cwd, 200)==NULL)
	{
		cout << "BPNGoAdapter: Error with _getcwd" << endl;
		return false;
	}

	// change to test directory
	if(_chdir(sgftestdir.c_str())==-1)
	{
		cout << "BPNGoAdapter: Error changing directory" << endl;
		return false;
	}

	result = 0.0f;
	float scoreError = 0.0f;
	float predictions = 0.0f;

	// read in list of sgf files
	vector<string> sgffiles;
	getFileList(sgffiles, "*.mgt");
	getFileList(sgffiles, "*.sgf");

	// change back to original directory
	_chdir(cwd);

	if(sgffiles.size()<=0)
	{
		cout << "BPNGoAdapter: No appropriate files found in " << sgftestdir << endl;
		return false;
	}
	
	int totalNetworkRuns = 0;
	BoardStruct board;

	vector<char> results;

	for(int i=0;i<sgffiles.size();i++)
	{
		// change to test directory
		if(_chdir(sgftestdir.c_str())==-1)
		{
			cout << "BPNGoAdapter: Error changing directory" << endl;
			return false;
		}
		// load sgf file and step through to end
		SGFReader sgf;
		if(!sgf.load(sgffiles[i]))
		{
			cout << "BPNGoAdapter: SGF test file failed to load, skipping..." << endl;
			_chdir(cwd);
			continue;
		}
		int size = 19;
		string v;
		if(sgf.getBoardSize(v))
		{
			size = atoi(v.c_str());
			if(size<1)
			{
				cout << "BPNGoAdapter: Bad size for sgf test file: " << sgffiles[i] << endl;
				cout << "BPNGoAdapter: skipping..." << endl;
				_chdir(cwd);
				continue;
			}
		}
		// load sgfresults file
		string resultsfile = sgffiles[i];
		resultsfile+="output.txt";
		results.clear();
		if(!readGNUGoResultsFile(resultsfile, results, size))
		{
			// change back to original directory
			_chdir(cwd);
			cout << " Could not read file: " << resultsfile << ", skipping." << endl;
			continue;
		}

		_chdir(cwd);

		board.clear(size);
		Move current;

		/** @todo Use Misc::setupBoardFromSGF() */
		// play setup moves
		vector<Move> props;
		if(sgf.getRootNode().getEmptySetup(props))
		{
			for(int i=0;i<props.size();i++)
				board.setPoint(props[i].getX(), props[i].getY(), EMPTY, false);
		}
		if(sgf.getRootNode().getBlackSetup(props))
		{
			for(int i=0;i<props.size();i++)
				board.setPoint(props[i].getX(), props[i].getY(), BLACK, false);
		}
		if(sgf.getRootNode().getWhiteSetup(props))
		{
			for(int i=0;i<props.size();i++)
				board.setPoint(props[i].getX(), props[i].getY(), WHITE, false);
		}
		sgf.rewindToStart();

		while(sgf.peekNextMove(current))
		{
			//log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);
			board.setPoint(current.getX(), current.getY(), current.getColour());
		}

		// run net on final board state
		Matrix<float> input;
		vector<Matrix<float> > outputs;
		getInput(0, 0, board, input, BLACK);
		getBPN().getAnswer(input);
		Matrix<float>& output = getBPN().getOutputs()[getBPN().getNumberOfLayers()-1];

		// Hidden node info if requested
		if(hiddenNodeInfo)
		{
			// use temp which contains all the outputs for all layers
			// of network and sum score for each hidden node
			for(int i=0;i<getBPN().getOutputs()[1].getHeight();i++)
				hiddenInfo->push_back(getBPN().getOutputs()[1].getValue(0, i));
			totalNetworkRuns++;
		}

		// compare final board state with the corresponding results file
		int numCorrect = 0;
		float actualScore = 0.0f;
		float ourScore = 0.0f;
		// check the entire board point by point
		// remember how many we get right
		for(int i=0;i<size;i++)
		{
			for(int j=0;j<size;j++)
			{
				// alive 'a'
				if(results.at(i*size+j)=='A')
				{
					// if it's our colour and we think it's alive
					if(board.getPoint(j, i)==BLACK)
					{
						actualScore++;
						if(output.getValue(0, i*size+j)>=BLACK_TERRITORY_THRESHOLD)
						{
							ourScore++;
							numCorrect++;
						}
					}
					// if it's their colour and we think they are alive
					else if(board.getPoint(j, i)==WHITE)
					{
						actualScore--;
						if(output.getValue(0, i*size+j)<=WHITE_TERRITORY_THRESHOLD)
						{
							ourScore--;
							numCorrect++;
						}
					}
				}
				// dead 'd'
				else if(results.at(i*size+j)=='D')
				{
					// if it's our colour and we think it's dead
					if(board.getPoint(j, i)==BLACK)
					{
						actualScore--;
						if(output.getValue(0, i*size+j)<=WHITE_TERRITORY_THRESHOLD)
						{
							ourScore--;
							numCorrect++;
						}
					}
					// if it's their colour and we think it's dead
					else if(board.getPoint(j, i)==WHITE)
					{
						actualScore++;
						if(output.getValue(0, i*size+j)>=BLACK_TERRITORY_THRESHOLD)
						{
							ourScore++;
							numCorrect++;
						}
					}
				}
				// seki 's'
				else if(results.at(i*size+j)=='S')
				{
					// in chinese scoring each stone in seki counts as one point
					if(board.getPoint(j, i)==BLACK)
						actualScore++;
					else if(board.getPoint(j, i)==WHITE)
						actualScore--;
					// if we think this point is seki
					if(output.getValue(0, i*size+j)>=NEUTRAL_LOW_THRESHOLD && output.getValue(0, i*size+j)<=NEUTRAL_HIGH_THRESHOLD)
					{
						if(board.getPoint(j, i)==BLACK)
							ourScore++;
						else if(board.getPoint(j, i)==WHITE)
							ourScore--;
						numCorrect++;
					}
				}
				// dame 'm'
				else if(results.at(i*size+j)=='M')
				{
					// if we think this point is dame
					if(output.getValue(0, i*size+j)>=NEUTRAL_LOW_THRESHOLD && output.getValue(0, i*size+j)<=NEUTRAL_HIGH_THRESHOLD)
						numCorrect++;
				}
				// white territory 'w'
				else if(results.at(i*size+j)=='W')
				{
					actualScore--;
					// if we think this point is white territory
					if(output.getValue(0, i*size+j)<=WHITE_TERRITORY_THRESHOLD)
					{
						ourScore--;
						numCorrect++;
					}
				}
				// black territory 'b'
				else if(results.at(i*size+j)=='B')
				{
					actualScore++;
					// if we think this point is black territory
					if(output.getValue(0, i*size+j)>=BLACK_TERRITORY_THRESHOLD)
					{
						ourScore++;
						numCorrect++;
					}
				}
			}
		}
		// add komi to actual score
		// be careful of komi
		float komi;
		// take off komi since scoring from blacks perspective
		if(sgf.getKomi(komi))
		{
			ourScore-=komi;
			actualScore-=komi;
		}

		// compare score and network output
		// make sure score error is always positive
		if((ourScore-actualScore)<=0)
			scoreError-=(ourScore-actualScore);
		else
			scoreError+=(ourScore-actualScore);

		predictions+=((float)numCorrect/(float)(size*size))*(float)100;
	}

	// calculate averages for hiddenNodeInfo
	if(hiddenNodeInfo)
	{
		for(int i=0;i<hiddenInfo->size();i++)
			(*hiddenInfo)[i]/=(float)totalNetworkRuns;
	}

	// calculate average error
//	result = result/sgffiles.size();
	predictions = predictions/sgffiles.size();
	cout << "Average correct predictions: " << predictions << "%" << endl;
	result = scoreError/sgffiles.size();

	return true;
}

bool BPNGoAdapterInterface::urgencyTest(const vector<string>& sgffiles, float& result, bool hiddenNodeInfo/* = false*/,
	vector<float>* hiddenInfo/* = NULL*/)
{
	LogWriter log;
	string message;
	char buffer[200];

	int totalMoves = 0;
	int moveCount = 0;
	int totalNetworkRuns = 0;
	int totalCorrect = 0;
	int correct = 0;

	SGFReader sgf;
	BoardStruct board;
	Matrix<float> input;
//	vector<Matrix<float> > temp(getBPN().getNumberOfLayers());
	Matrix<float>* answers;
	vector<Move> moves;

	bool currentIsMoveA = false;
	bool getInputSuccess = false;

	if(sgffiles.size()==0)
	{
		cout << "Error: no sgf files to test BPN against." << endl;
		return false;
	}

	for(int i=0;i<sgffiles.size();i++)
	{
		message = "Testing: ";
		message+=sgffiles[i];
		log.println(message);
		if(!sgf.load(sgffiles[i]))
		{
			message = "Skipping ";
			message+=sgffiles[i];
			log.println(message);
			continue;
		}

		board.clear();

		Move current;
		int count = 0;
		correct = 0;
		moveCount = 0;

		while(sgf.peekNextMove(current))
		{
			//log.print(".");
			// get the actual move from SGF
			sgf.getNextMove(current);
			// get next move of the same colour
			moves.clear();
			if(sgf.getLookaheadMoves(2, current.getColour(), moves) && moves.size()==2)
			{
				// use bpn to rank this move compared to the next move of the same colour
				
				// randomise whether we present the current move first or last
				if(getRandom(0.0f, 1.0f)<=0.5)
				{
					getInputSuccess = getInput(current.getX(), current.getY(), moves[1].getX(), moves[1].getY(),
												board, input, current.getColour());
					currentIsMoveA = true;
				}
				else
				{
					getInputSuccess = getInput(moves[1].getX(), moves[1].getY(), current.getX(), current.getY(),
												board, input, current.getColour());
					currentIsMoveA = false;
				}
				if(getInputSuccess)
				{
					getBPN().getAnswer(input);
					answers = &(getBPN().getOutputs()[getBPN().getNumberOfLayers()-1]);
					// which is more urgent A or B?
					float moveA = answers->getValue(0, 0);
					float moveB = answers->getValue(0, 1);
					if(currentIsMoveA && (moveA>=0.5 && moveB<0.5))
						correct++;
					else if(!currentIsMoveA && (moveA<0.5 && moveB>=0.5))
						correct++;
					
					// Hidden node info if requested
					if(hiddenNodeInfo)
					{
						// use temp which contains all the outputs for all layers
						// of network and sum score for each hidden node
						for(int i=0;i<getBPN().getOutputs()[1].getHeight();i++)
							hiddenInfo->push_back(getBPN().getOutputs()[1].getValue(0, i));
						totalNetworkRuns++;
					}
				}
			}

			totalMoves++;
			moveCount++;
			count++;
			if(count==10)
			{
				sprintf(buffer, "%*g ", 2, ((float)correct/(float)moveCount)*(float)100);
				message = buffer;
				log.print(message);
				count = 0;
			}
			// add move to board
			board.setPoint(current.getX(), current.getY(), current.getColour());
		} // end while
		totalCorrect+=correct;
		log.print("\n");
	}

	// calculate averages for hiddenNodeInfo
	if(hiddenNodeInfo)
	{
		for(int i=0;i<hiddenInfo->size();i++)
			(*hiddenInfo)[i]/=(float)totalNetworkRuns;
	}

	// store average rank of pros move
	float average = ((float)totalCorrect/(float)totalMoves)*(float)100;

	message = "Average correct percentage estimation of urgency between two consecutive same colour moves: ";
	sprintf(buffer, "%*g", 7, average);
	message+=buffer;
	log.println(message);

	result = average;

	return true;
}