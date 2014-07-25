#pragma warning( disable : 4786 )

#include "globals.h"
#include "BPNGoTrainer.h"
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
void BPNGoTrainer::extractPairsFromDirectory(string path, NNDatabase* db, const GoRankRange* rankRange /*=NULL*/) const
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

/** Process the file list and extract training pairs from each file in the current directory.
@params files A list of SGF files to extract patterns from.
@params db The NNDatabase to store the training pairs in. 
NOTE: The files list should be full path names and files, e.g.
"c:\Docs\Cheese\Toe.txt" */
void BPNGoTrainer::extractPairs(vector<string>& files, 
		NNDatabase* db, int movesFrom /*=0*/, int movesTo /*=0*/, const GoRankRange* rankRange /*=NULL*/) const
{	
	LogWriter log;
	log.println("Processing file list");
	
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
	log.println("Finished processing file list");
}

/** Extract all training pairs from an SGF file using the given
SGFReader object and add them to the given NNDatabase.
@params sgf An SGFReader object to use to read the file it 
represents.
@params database The NNDatabase to store the training pairs in. */
void BPNGoTrainer::extractTrainingPairs(SGFReader* sgf, NNDatabase* database,
			int movesFrom /*=0*/, int movesTo /*=0*/, int lookahead /*=0*/, bool quiet /*=false*/) const
{
	LogWriter log;
	string message;
	//char buffer[50];

	if(movesFrom > movesTo) {
		log.println("Start move is greater than end move.");
		return;
	}

	vector<Move> moves;
	sgf->getTree().getAllPrimaryMoves(moves);
	if(moves.size()==0)
		return;

	// if file already in the database skip it
	if(!database->addSignature(sgf->getSignature())) {
		log.println("File already in database.");
		return;
	}

	int size = 19;
	string v;
	if(sgf->getBoardSize(v))
		size = atoi(v.c_str());
	if(size>19 || size<5) {
		message = "Boardsize too small or large to handle: ";
		message+=v;
		log.println(message);
		return;
	}
	BoardStruct board(size);
	
	vector<Move> futureMoves;
	vector<Move> badMoves;
	float count, total;
	float s;
	float scoreDecrement;
	Move* m;
	int moveNumber=0;
	
	string c;
	Matrix<float> input(1, goAdapter->getBPN().getWeights()[0].getHeight());
	Matrix<float> output;
	// colour of new move
	int colour;
	
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
	}*/

	SGFNode* nextNode = &(sgf->getRootNode());
	bool useThisMove = true;
	
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
		
			// get LOOKAHEAD moves into the future from
			// the current playing colour
			nextNode->getLookaheadMoves(lookahead, colour, futureMoves);

			// get all possible moves including illegal ones
			// for this board position
			badMoves.clear();

			//badMoves = Go::getAllMoves(&board);

			for(int i=0;i<board.getSize();i++) {
				for(int j=0;j<board.getSize();j++) {
					if(board.getPoint(j, i)==EMPTY) {
						Move mt(j, i);
						badMoves.push_back(mt);
					}
				}
			}

			// and remove all the future moves from this
			for(i=0;i<futureMoves.size();i++) {
				for(int j=0;j<badMoves.size();j++) {
					if(futureMoves[i]==badMoves[j])
						badMoves.erase(&badMoves[j]);
				}
			}
		
			count = 1;
			total = 0;
			s = (float)futureMoves.size();
			//s = (double)futureMoves.size()+1;
			scoreDecrement = 1/s;

			// add the good moves to database
			for(i=0;i<s;i++) {
				m = &(futureMoves[i]);
				m->setNNScore(scaleValue(count, 0, 1, (float)0.1, (float)0.9));
				//message = "futureMove[";
				//sprintf(buffer, "%d", i);
				//message+=buffer;
				//message+="].score=";
				//sprintf(buffer, "%*g", 7, m->getScore());
				//message+=buffer;
				//log.write(message, type);
				total+=count;
				count-=scoreDecrement;
				goAdapter->getInput(m->getX(), m->getY(), board, input, colour);
				vector<vector<float> > tv;
				tv.resize(1);
				tv[0].resize(1);
				tv[0][0] = m->getNNScore();
				output.resize(1, 1);
				output.setValues(tv);
				database->addTrainingPair(&input, &output);
			}			

			// take LOOKAHEAD random badMoves
			// so the proportion of good/bad is the same
			vector<Move> temp;
			try {
				temp.reserve(badMoves.size());
			} catch(exception& e) {
				cerr << e.what();
				return;
			}
			int v;
			for(i=0;i<futureMoves.size()&&badMoves.size()>0;i++)
			{
				// pick random move from badMoves
				// between 0 and badMoves.size()
				v = (int)getRandom(0, (float)((float)badMoves.size()-0.01));
				temp.push_back(badMoves[v]);
				badMoves.erase(&badMoves[v]);
			}
			badMoves = temp;
			
			s = (float)badMoves.size();

			// add the good moves to database			
			for(i=0;i<s;i++)
			{
				m = &badMoves[i];
				m->setNNScore((float)0.1);
				goAdapter->getInput(m->getX(), m->getY(), board, input, colour);
				vector<vector<float> > tv;
				tv.resize(1);
				tv[0].resize(1);
				tv[0][0] = m->getNNScore();
				output.resize(1, 1);
				output.setValues(tv);
				database->addTrainingPair(&input, &output);
			}
		}
		
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
