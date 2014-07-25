#pragma warning( disable : 4786 )

#include "globals.h"
#include "Safety1BPNGoTrainer.h"
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
void Safety1BPNGoTrainer::extractPairsFromDirectory(string path, NNDatabase* db, const GoRankRange* rankRange /*=NULL*/) const
{
	LogWriter log;
	char buffer[200];
	if(_getcwd(buffer, 200)==NULL) {
		log.println("Error with _getcwd");
		return;
	}
	if(_chdir(path.c_str())==-1) {
		log.println("Error changing directory");
		return;
	}

	vector<string> files;
	getFileList(files, "*.mgt");
	getFileList(files, "*.sgf");

	if(files.size()<=0) {
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
void Safety1BPNGoTrainer::extractPairs(vector<string>& files, 
		NNDatabase* db, int movesFrom /*=0*/, int movesTo /*=0*/, const GoRankRange* rankRange /*=NULL*/) const
{	
	LogWriter log;
	log.println("Processing file list");
	
	string message;
	string file;

	SGFReader sgf;
	for(int i=0;i<files.size();i++) {
		message = "Reading: ";
		message+=files[i];
		log.println("Reading: "+files[i], goAdapter->getBPN().getTypeString());
		//file = path;
		//file+="\\";
		//file+=files[i];
		if(!sgf.load(files[i])) {
			log.println("SGF failed to load");
			continue;
		}
		if(rankRange==NULL || rankRangeFilterSGF(sgf, *rankRange))
			extractTrainingPairs(&sgf, db, movesFrom, movesTo);
		else {
			message = "* Skipping - : ";
			message+=files[i];
			log.println(message);
		}
	}
	log.println("Finished processing file list");
}

/** @brief Extract all training pairs from an SGF file using the given
  * SGFReader object and add them to the given NNDatabase.
  * @param sgf An SGFReader object to use to read the file it represents.
  * @param database The NNDatabase to store the training pairs in. 
  * @param lookahead Lookahead is disable for Safety1BPNGoTrainer... */
void Safety1BPNGoTrainer::extractTrainingPairs(SGFReader* sgf, NNDatabase* database,
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
	
	string c;
	Matrix<float> input(1, goAdapter->getBPN().getWeights()[0].getHeight());
	Matrix<float> lastInput(1, goAdapter->getBPN().getWeights()[0].getHeight());
	Matrix<float> output(1, 1);
	// colour of new move
	int colour;
	int x, y, i, j, k;
	
	vector<string> vs;
	vector<vector<float> > tv;
	tv.resize(1);
	tv[0].resize(1);

	// Find the set of all strings that appear during this game
	// note the move number at which it appears and the stone of origin
	// also note the life span of the string - how many moves does it last?
	// If it is ALIVE then make moveLasted = -1
	vector<stringCreationInfo> stringCreations;

	// Loop through the sgf and note all the string instances
	SGFNode* nextNode = &(sgf->getRootNode());
	board.clear();
	while(nextNode!=NULL) {
		// determine colour and position of move	
		if (nextNode->getProperty(SGFProperty::blackMoveTag, vs)) {
			colour = BLACK;
			c = vs[0];
		}
		else if (nextNode->getProperty(SGFProperty::whiteMoveTag, vs)) {
			colour = WHITE;
			c = vs[0];
		}
		else break;

		x = Move::SGFToX(c);
		y = Move::SGFToY(c);
		board.setPoint(x, y, colour);

		// Skip passes
		if(x!=-1 && y!=-1) {
			// Check if this move has created a new string (i.e. is it a 1 stone string?)
			// Add an entry if it is
			if(board.getString(x, y)->getStoneCount()==1)
				stringCreations.push_back(stringCreationInfo(x, y, -1));
		}

		// Add one to all the moves lasted values
		for(i=0;i<stringCreations.size();i++)
			stringCreations[i].movesLasted++;

		nextNode = nextNode->getChild();
	}

	// Make sure we're at the dead/alive/seki annotations node
	// look for at least 1 territory mark (TB or TW)
	vector<Move> blackTerritory, whiteTerritory;
	nextNode->getBlackTerritory(blackTerritory);
	nextNode->getWhiteTerritory(whiteTerritory);

	// Check if we've got any territory markers at this node
	if(blackTerritory.size()==0 && whiteTerritory.size()==0) {
		// Find the annotations node
		nextNode = &(sgf->getRootNode());
		bool annotationsFound = false;
		while(nextNode!=NULL) {
			nextNode->getBlackTerritory(blackTerritory);
			nextNode->getWhiteTerritory(whiteTerritory);
			if(blackTerritory.size()!=0 || whiteTerritory.size()!=0) {
				annotationsFound = true;
				break;
			}

			nextNode = nextNode->getChild();
		}

		// Skip this sgf if we can't find one
		if(!annotationsFound) {
			if(!quiet)
				log.print("\n");
			return;
		}
	}

	// Get dead marks
	vector<Move> deadStones;
	nextNode->getMarkers(deadStones);

	// Go through the alive stones on the board and update
	// their movesLasted values to -1 for alive, in stringCreations
	bool deadStone;
	for(i=0;board.getSize();i++) {
		for(j=0;board.getSize();j++) {
			if(board.getPoint(j, i)!=EMPTY) {
				// Check it's not been marked dead (cross MA) (no mark for alive, circle CR for seki)
				deadStone = false;
				for(k=0;k<deadStones.size();k++) {
					if(deadStones[k].getX()==j && deadStones[k].getY()==i) {
						deadStone = true;
						break;
					}
				}
				if(!deadStone) {
					// Find the stringCreation info for this stone if it exists
					for(k=0;k<stringCreations.size();k++) {
						if(stringCreations[k].x==j && stringCreations[k].y==i) {
							stringCreations[k].movesLasted = -1;
							break;
						}
					}
				}
			}
		} // end for j
	} // end for i

	// Step through all the string instances
	while(!stringCreations.empty()) {
		// Continue until a null node forces the loop to break
		nextNode = &(sgf->getRootNode());
		board.clear();
		// add setup points
		setupBoardFromSGF(*sgf, board);
		bool useThisMove = true;
		bool activeString = false;
		int moveNumber=0;
		int lastString = stringCreations.size()-1;
		float score = 0.0f;
		
		// Was this string alive at the end?
		if(stringCreations.back().movesLasted==-1) 
			score = 1.0f;
		
		// Loop through the sgf from the beginning
		while (true) {
			// check bounds if necessary
			if(movesTo!=0) {
				// use ply instead of individual moves
				if(moveNumber/2 >= movesTo) break;
				else if(moveNumber/2 < movesFrom) useThisMove = false;
				else useThisMove = true;
			}
			
			// determine colour and position of move	
			if (nextNode->getProperty(SGFProperty::blackMoveTag, vs)) {
				colour = BLACK;
				c = vs[0];
			}
			else if (nextNode->getProperty(SGFProperty::whiteMoveTag, vs)) {
				colour = WHITE;
				c = vs[0];
			}
			else break;

			board.setPoint(Move::SGFToX(c), Move::SGFToY(c), colour);
			
			if(movesTo==0 || useThisMove) {
				
				
				// If we don't have an active string wait for the next one
				if(!activeString) {
					if(board.getPoint(stringCreations.back().x, stringCreations.back().y)!=EMPTY)
						activeString = false;
					
					// If we do have an active string then make the input/output pair
					if(activeString) {
						// Check the string is still on the board
						// break from while loop if it's dead
						if(board.getPoint(stringCreations.back().x, stringCreations.back().y)==EMPTY)
							break;
						
						// Add input/output to database
						goAdapter->getInput(stringCreations.back().x, stringCreations.back().y, board, input, colour);
						
						// Set output score
						output.setAllValues(score);
						
						// If this input is the same as the last skip it!
						// This avoids long sequences when nothing happens to the string
						if(lastInput!=input) {
							database->addTrainingPair(&input, &output);
							lastInput = input;
						}
					}
				}
			}
			
			if(!quiet)
				log.print(".");
			moveNumber++;
			
			// Go to next sgf node
			nextNode = nextNode->getChild();
			if(nextNode==NULL)
				break;

		} // end while(true)

		// Pop the string we've finished with
		stringCreations.erase(stringCreations.end()-1);

	} // end while(!stringCreations.empty())

	if(!quiet)
		log.print("\n");
	//database.save();
} // end extractTrainingPairs
