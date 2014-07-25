#pragma warning( disable : 4786 )

#include "globals.h"
#include "Urgency3BPNGoTrainer.h"
#include "NeuralNetwork.h"
#include "LogWriter.h"
#include "BoardStruct.h"
#include "Misc.h"
#include <iostream>
#include <string>
#include <io.h>
#include <direct.h>
#include <cmath>
#include <algorithm>
//using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** Extract training pairs from all sgf files found in the
specified directory.
@params path The directory to search for SGF files.
@params db The NNDatabase to store the training pairs in. */
void Urgency3BPNGoTrainer::extractPairsFromDirectory(string path, NNDatabase* db, const GoRankRange* rankRange /*=NULL*/) const
{
	LogWriter log;
	char buffer[200];
	if(_getcwd(buffer, 200)==NULL)
	{
		log.println("Error with _getcwd");
		return;
	}
	if(_chdir(path.c_str())==-1)
	{
		log.println("Error changing directory");
		return;
	}

	vector<string> files;
	getFileList(files, "*.mgt");
	getFileList(files, "*.sgf");


	if(files.size()<=0)
	{
		string message = "No appropriate files found in ";
		message+=path;
		log.println(message);
		// change back to original directory
		_chdir(buffer);
		return;
	}
	
	extractPairs(files, db, 0, 0, rankRange);
	// change back to original directory
	_chdir(buffer);
}

/** Process the file list and extract training pairs from each file.
@params files A list of SGF files to extract patterns from.
@params db The NNDatabase to store the training pairs in. 
NOTE: The files list should be full path names and files, e.g.
"c:\Docs\Cheese\Toe.txt" */
void Urgency3BPNGoTrainer::extractPairs(vector<string>& files, 
		NNDatabase* db, int movesFrom /*=0*/, int movesTo /*=0*/, const GoRankRange* rankRange /*=NULL*/) const
{	
	LogWriter log;
	log.println("Processing file list", goAdapter->getBPN().getTypeString());
	
	string message;
	string file;

	for(int i=0;i<files.size();i++)
	{
		message = "Reading: ";
		message+=files[i];
		log.println("Reading: "+files[i], goAdapter->getBPN().getTypeString());
		//file = path;
		//file+="\\";
		//file+=files[i];
		SGFReader sgf;
		if(!sgf.load(files[i]))
		{
			log.println("SGF failed to load");
			continue;
		}
		if(rankRange==NULL || rankRangeFilterSGF(sgf, *rankRange))
			extractTrainingPairs(&sgf, db, movesFrom, movesTo);
		else
		{
			message = "* Skipping - : ";
			message+=files[i];
			log.println(message);
		}
	}
	log.println("Finished processing file ", goAdapter->getBPN().getTypeString());
}

/** Extract all training pairs from an SGF file using the given
SGFReader object and add them to the given NNDatabase.
@params sgf An SGFReader object to use to read the file it 
represents.
@params database The NNDatabase to store the training pairs in. */
void Urgency3BPNGoTrainer::extractTrainingPairs(SGFReader* sgf, NNDatabase* database,
			int movesFrom /*=0*/, int movesTo /*=0*/, int lookahead /*=0*/, bool quiet /*=false*/) const
{
	// at each step in sgf file, score current move with current urgency net
	// then score next move of same colour
	// if first move doesn't score at least 0.2 higher than second
	// add as training pairs:
	// 1. First move output should be second move score +0.2 (0.9 max)
	// 2. Second move output should be first move score -0.2 (0.1 min)

	LogWriter log;
	string message;
	//char buffer[50];

	if(movesFrom > movesTo)
	{
		log.println("Start move is greater than end move.");
		return;
	}

	vector<Move> moves;
	sgf->getTree().getAllPrimaryMoves(moves);
	if(moves.size()==0)
		return;

	// if file already in the database skip it
	if(!database->addSignature(sgf->getSignature()))
	{
		log.println("File already in database.");
		return;
	}

	int size = 19;
	string v;
	if(sgf->getBoardSize(v))
		size = atoi(v.c_str());
	if(size>19 || size<5) 
	{
		message = "Boardsize too small or large to handle: ";
		message+=v;
		log.println(message);
		return;
	}
	BoardStruct board(size);
	
	vector<Move> futureMoves;
	int moveNumber=0;
	
	string c;
	Matrix<float> currentInput(1, goAdapter->getBPN().getWeights()[0].getHeight());
	Matrix<float> nextInput(1, goAdapter->getBPN().getWeights()[0].getHeight());
	Matrix<float> output(1, 1);
	Matrix<float>* answers;
//	vector<Matrix<float> > temp(goAdapter->getBPN().getNumberOfLayers());
	vector<vector<float> > tv;
	tv.resize(1);
	tv[0].resize(1);
	// colour of new move
	int colour;
	
	//sgf->initBoard(board);
	board.clear();
	// add setup points
	setupBoardFromSGF(*sgf, board);
/*	vector<Move> props;
	if(sgf->getRootNode()->getEmptySetup(props))
	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props[i].getX(), props[i].getY(), EMPTY, false);
	}
	if(sgf->getRootNode()->getBlackSetup(props))
	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props[i].getX(), props[i].getY(), BLACK, false);
	}
	if(sgf->getRootNode()->getWhiteSetup(props))
	{
		for(int i=0;i<props.size();i++)
			board.setPoint(props[i].getX(), props[i].getY(), WHITE, false);
	} */

	SGFNode* nextNode = &(sgf->getRootNode());
	bool useThisMove = true;
	
	bool currentIsMoveA = true;
	bool getInputSuccess = false;

	// continue until a null node forces the loop to break
	while (true) {
		// check bounds if necessary
		if(movesTo!=0) {
			// use ply instead of individual moves
			if(moveNumber/2 >= movesTo) break;
			else if(moveNumber/2 < movesFrom) useThisMove = false;
			else useThisMove = true;
		}

		if(movesTo==0 || useThisMove) {
			nextNode = nextNode->getChild();
			if(nextNode==NULL)
				break;
			
			// determine colour of move	
			vector<string> vs;
			if (nextNode->getProperty(SGFProperty::blackMoveTag, vs)) {
				colour = BLACK;
				c = vs[0];
			}
			else if (nextNode->getProperty(SGFProperty::whiteMoveTag, vs)) {
				colour = WHITE;
				c = vs[0];
			}
			else break;
		
			// find our next move to compare with
			// check 
			futureMoves.clear();
			// NOTE: The first move returned by getLookaheadMoves() is
			// always the current one, so we need 2 lookahead moves
			nextNode->getLookaheadMoves(2, colour, futureMoves);
			if(futureMoves.size()==2 && Move::SGFToX(c)!=-1 && Move::SGFToY(c)!=-1
				&& futureMoves[1].getX()!=-1 && futureMoves[1].getY()!=-1) {
				// At each step in sgf file, score current move with current urgency net
				// then score next move of same colour
				// if first move doesn't score at least 0.2 higher than second
				// add as training pairs:
				// 1. First move output should be second move score +0.2 (0.9 max)
				// 2. Second move output should be first move score -0.2 (0.1 min)

				// get current move score
				goAdapter->getInput(Move::SGFToX(c), Move::SGFToY(c), board, currentInput, colour);
				goAdapter->getBPN().getAnswer(currentInput);
				answers = &(goAdapter->getBPN().getOutputs()[goAdapter->getBPN().getNumberOfLayers()-1]);
				float currentMoveScore = answers->getValue(0, 0);

				// get next move score
				goAdapter->getInput(futureMoves[1].getX(), futureMoves[1].getY(), board, nextInput, colour);
				goAdapter->getBPN().getAnswer(nextInput);
				answers = &(goAdapter->getBPN().getOutputs()[goAdapter->getBPN().getNumberOfLayers()-1]);
				float nextMoveScore = answers->getValue(0, 0);

				// if current scores higher than next move by 0.2
				// don't train otherwise do train
				if(currentMoveScore<=(nextMoveScore+0.2)) {
					if((nextMoveScore+0.2)>0.9)
						tv[0][0] = 0.9f;
					else
						tv[0][0] = nextMoveScore+0.2;
					output.setValues(tv);
					database->addTrainingPair(&currentInput, &output);

					if((currentMoveScore-0.2)<0.1)
						tv[0][0] = 0.1f;
					else
						tv[0][0] = currentMoveScore-0.2;
					output.setValues(tv);
					database->addTrainingPair(&nextInput, &output);
				}
			}
		} // end if(movesTo==0 || useThisMove)
		
		board.setPoint(Move::SGFToX(c), Move::SGFToY(c), colour);
		if(!quiet)
			log.print(".");
		moveNumber++;
		// save after every board position has been
		// analysed and moves extracted
		// database.save();
	} // end while(true)
	if(!quiet)
		log.print("\n");
	//database.save();
} // end extractTrainingPairs
