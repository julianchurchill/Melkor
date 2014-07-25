#include "Melkor.h"
#include "LogWriter.h"
#include "Misc.h"
#include "Test.h"
#include "CN.h"
#include "BPNGoAdapter.h"
#include "newBPNGoAdapter.h"
#include "newBPN2GoAdapter.h"
#include "newBPN3GoAdapter.h"
#include "newBPN4GoAdapter.h"
#include "newBPN5GoAdapter.h"
#include "newBPN6GoAdapter.h"
#include "newBPN7GoAdapter.h"
#include "newBPN8GoAdapter.h"
#include "newBPN9GoAdapter.h"
#include "newBPN10GoAdapter.h"
#include "newBPN11GoAdapter.h"
#include "newBPN12GoAdapter.h"
#include "CNGoAdapter.h"
#include "SpiralOrderer.h"
#include "GeneralSuggester.h"
#include "LadderSuggester.h"
#include "LooseTacticalSuggester.h"
#include "CaptureSuggester.h"
#include "InfluenceGoal.h"
#include "LadderExtension.h"
#include "LadderGoal.h"
#include "LooseTacticalGoal.h"
#include "LifeDeathGoal.h"
#include <algorithm>
#include <iostream>

// if this is defined then Go::testStringLife
// will limit the search area to the string concerned
// plus 2 points on each side
#define USE_LIFEDEATHTEST_AREALIMIT

// This define includes code to print out the time it takes for individual
// game tree searches. We use this information to calibrate calculateSearchDepths().
#define TIME_SEARCHES

const string Melkor::myName = "Melkor";
const string Melkor::defaultNetwork = "newBPN9-7x7-optimum1.bpn";
const bool Melkor::defaultUseSuperKo = true;
const bool Melkor::defaultUseDynamicSearchDepth = false;
const bool Melkor::defaultUseGlobalMoveFunction = true;
const bool Melkor::defaultUseGameTreeSearch = true;
const bool Melkor::defaultUseGTP = false;
const int Melkor::defaultComputerColour = WHITE;
const bool Melkor::defaultDebuggo = false;
const bool Melkor::defaultVerbose = false;
const bool Melkor::defaultCheat = false;

Melkor::Melkor(unsigned long& seed, const string* network, const bool debugmtdf) : 
		charCount(0), useLog(false), computerColour(BLACK), opponentColour(WHITE), 
		bpn(NULL), latestBoardKey(-1)
{
	error = false;

	// set random seed
	if(seed==0) seed = (unsigned)time(NULL);
	srand(seed);

	// initialise member variables
	Go::setUseSuperKo(defaultUseSuperKo);
	useDynamicSearchDepth = defaultUseDynamicSearchDepth;
	useGlobalMoveFunction = defaultUseGlobalMoveFunction;
	useGameTreeSearch = defaultUseGameTreeSearch;
	useGTP = defaultUseGTP;
	computerColour = defaultComputerColour;
	debuggo = defaultDebuggo;
	verbose = defaultVerbose;
	cheat = defaultCheat;

	// Default is no time limit per move
	timePerMove = 0;
	playTimedGame = false;
	noPasses = false;

	totalLadder1PlyTime = totalLooseTactical1PlyTime = totalStatus1PlyTime = totalBigSequence1PlyTime = 0;
	totalLadderSearches = totalLooseTacticalSearches = totalStatusSearches = totalBigSequenceSearches = 0;

	getBestGlobalMoveCalls = 0;

#ifdef _WIN32
	LARGE_INTEGER freq, tstart, tend;
	timerStart(&tstart, &freq);
#else
	clock_t	tstart = clock();
#endif
	// Generate speed constant
	int j;
	for(int i=0;i<100000;i++)
		j = i*i;
#ifdef _WIN32
	timerEnd(&tend);
	// work out time in milliseconds
	speedConstant = timerValue(tstart, tend, freq)*1000;
#else
#error non win32 timer end needs to be written
#endif

	// Set default depth limits at 4s per move
	useNBigMoves = 8;
	calculateSearchDepths(4, useNBigMoves);

	// create dynamic objects
	if(!init(network)) {
		string message = "Melkor object failed to initialise.\n";
		LogWriter::printerr(message, "");
		setError(true, message);
	}

	mtdfSearch = new MTDFSearch(debugmtdf, this, true);
	test = new Test(this, true);
	gtp = new GTPFunctions(this, true);

	/** @todo look into Dave Dyers eye shape library to see how it works...
	/* http://www.andromeda.com/people/ddyer/go/shape-library.html */

	eyeSpacePM.loadPatterns("EyeShape.pat");
	eyeSpacePM.makePatternTree(eyeSpacePM.getPatterns(), eyeSpacePM.getPatternTree(), SpiralOrderer());
}

Melkor::~Melkor()
{
	delete mtdfSearch;
	delete test;
	delete gtp;
	if(bpn!=NULL)
		delete bpn;
	stopLog();
}

bool Melkor::DoMainMenu(LogWriter& log)
{
	log.println("");
	log.println("1. Play Go");
	log.println("2. Run Tests");
	log.println("3. Quit");
	char temp[3];
	cin.getline(temp, 2);
	if(temp[0]=='1') {
		int size;

		while(true) {
			log.print("Your colour? [b|w] : ");
			cin.getline(temp, 2);
	
			if(temp[0]=='b')
				computerColour = WHITE;
			else if(temp[0]=='w')
				computerColour = BLACK;
			else
				continue;

			log.print("Boardsize? [9|19] : ");
			cin.getline(temp, 3);

			if(strcmp(temp, "9")==0)
				size = 9;
			else if(strcmp(temp, "19")==0)
				size = 19;
			else
				log.println("Bad boardsize");

			log.print("Handicap? [0-9]: ");
			cin.getline(temp, 2);

			int handicap = atoi(temp);
			if(handicap>=0 && handicap<=9) {
				Go::setHandicap(handicap);
				break;
			}
		} // end while(true)

		getBoard().resize(size);
		playGo(getBoard(), computerColour);
//			mtdfSearch->getEvalFunction());
		return true;
	}
	else if(temp[0]=='2') {
		while(DoTestMenu(log));
		return true;
	}
	else if(temp[0]=='3')
		return false;

	return true;
}

bool Melkor::DoTestMenu(LogWriter& log)
{
	log.println("");
	log.println("1. IsLegalTest (deprecated - is now implemented as unit test)");
	log.println("2. LibertyCountTest (deprecated - is now implemented as unit test)");
	log.println("3. GoStringsTest (deprecated - is now implemented as unit test)");
	log.println("4. SGFStepThroughTest");
	log.println("5. BPNTest");
	log.println("6. TrainingSpeedTest (incl Rprop)");
	log.println("7. AlphaBetaTest");
	log.println("8. AlphaBetaTest2 - Using Go board");
	log.println("9. AlphaBetaTest3 - Timed using Go board");
	log.println("10. TranspositionTableTest");
	log.println("11. MTDFSearchTest");
	log.println("12. influenceFunctionTest and eval function timing tests");
	log.println("13. Ladder search test");
	log.println("14. getAnswerTest");
	log.println("15. All Tests");
	log.println("16. Main Menu");

	char temp[3];
	cin.getline(temp, 3);
	int choice = atoi(temp);
	if(choice==0) {
		log.println("Please enter a number from the menu list");
		return true;
	}
	else if(choice==1) {
		test->IsLegalTest(verbose);
		return true;
	}
	else if(choice==2) {
		test->LibertyCountTest(verbose);
		return true;
	}
	else if(choice==3) {
		test->GoStringsTest(verbose);
		return true;
	}
	else if(choice==4) {
		BoardStruct b;
		test->SGFStepthroughTest(b, "twogtp001.sgf", true);
		return true;
	}
	else if(choice==5) {
		test->BPNTest();
		return true;
	}
	else if(choice==6) {
		test->TrainingSpeedTest();
		return true;
	}
	else if(choice==7) {
		test->AlphaBetaTest(verbose);
		return true;
	}
	else if(choice==8) {
		test->AlphaBetaTest2(verbose);
		return true;
	}
	else if(choice==9) {
		test->AlphaBetaTest3(verbose);
		return true;
	}
	else if(choice==10) {
		test->TranspositionTableTest(verbose);
		return true;
	}
	else if(choice==11) {
		test->MTDFSearchTest(verbose);
		return true;
	}
	else if(choice==12) {
		test->influenceFunctionTest(verbose);
		return true;
	}
	else if(choice==13) {
		test->ladderTest(verbose);
		return true;
	}
	else if(choice==14) {
		test->getAnswerTest(verbose);
		return true;
	}
	else if(choice==15) {
		test->IsLegalTest(verbose);
		test->LibertyCountTest(verbose);
		test->GoStringsTest(verbose);
		BoardStruct b;
		test->SGFStepthroughTest(b, "Nih-73-1.mgt", true);
		test->BPNTest();
		test->TrainingSpeedTest();
		test->AlphaBetaTest(verbose);
		test->AlphaBetaTest2(verbose);
		test->AlphaBetaTest3(verbose);
		test->TranspositionTableTest(verbose);
		test->MTDFSearchTest(verbose);
		test->influenceFunctionTest(verbose);
		test->ladderTest(verbose);
		test->getAnswerTest(verbose);
		return true;
	}
	else if(choice==16)
		return false;

	return true;
}

void Melkor::playGTP(string gtpScriptFilename)
{
	getBoard().clear();

	gtp->gtp_internal_set_boardsize(19);

	// to avoid crosstalk over stdout while using GTP
	LogWriter::setOutputStream(cerr);

	playGTP(getBoard(), gtpScriptFilename, computerColour);
}

bool Melkor::setSearchGoal(const Goal& g)
{ 
	if(useGameTreeSearch)
		mtdfSearch->setGoal(g);
	else
		return false;
	return true;
}

void Melkor::getBestLineOfPlay(vector<Move>& l) const
{
	if(useGameTreeSearch)
		mtdfSearch->getBestLineOfPlay(l);
}

void Melkor::saveSearchSettings()
{
	if(useGameTreeSearch)
		mtdfSearch->saveSettings();
}

void Melkor::restoreSearchSettings()
{
	if(useGameTreeSearch)
		mtdfSearch->restoreSettings();
}

string Melkor::printSearchSettings() const
{
	string message;
	if(useGameTreeSearch) {
		message = "MTDFSearch settings: \n";
		message+=mtdfSearch->printSearchSettings();
	}
	else
		message = "MTDFSearch not enabled! (may be NN only)";
	return message;
}


bool Melkor::init(const string* network)
{
	string tnetwork;
	if(network==NULL || *network=="") tnetwork = defaultNetwork;
	else tnetwork = *network;

	if(!loadBPN(tnetwork)) {
		string message = "Default network failed to load: ";
		message+=tnetwork;
		message+="\n";
		LogWriter::printerr(message, "");
		return false;
	}
	
	return true;
}

bool Melkor::loadBPN(const string& filename)
{
	bpn = Melkor::loadBPNStatic(filename);
	return (bpn==NULL) ? false : true;
}

/** @brief This is a static function to load a bpn, requiring multiple objects to be created
  * of dynamic types. */
BPNGoAdapterInterface* Melkor::loadBPNStatic(const string& filename)
{
	int id = BPN::getType(filename);
	if(id==-1)
		return NULL;

	BPNGoAdapterInterface* bpn = NULL;

	if(id==BPN::BPNTYPE)
		bpn = new BPNGoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE)
		bpn = new newBPNGoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE2)
		bpn = new newBPN2GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE3)
		bpn = new newBPN3GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE4)
		bpn = new newBPN4GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE5)
		bpn = new newBPN5GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE6)
		bpn = new newBPN6GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE7)
		bpn = new newBPN7GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE8)
		bpn = new newBPN8GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE9)
		bpn = new newBPN9GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE10)
		bpn = new newBPN10GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE11)
		bpn = new newBPN11GoAdapter(filename);
	else if(id==BPN::NEWBPNTYPE12)
		bpn = new newBPN12GoAdapter(filename);
	else if(id==BPN::CNTYPE)
		bpn = new CNGoAdapter(filename);

	if(bpn->getBPN().getErrorCode()!=BPN::NONE)
		return NULL;

	return bpn;
}

/** @brief Calculate a final score with the board and strings statuses updated.
  * Allow 10 seconds to complete this.
  * @return The final score with respect to Black. */
float Melkor::getFinalScore()
{
	const int timeAllowed = 10;
	calculateSearchDepths(timeAllowed, useNBigMoves);

	timePerMoveRanOut = false;
	int oldTimePerMove = timePerMove;
	timePerMove = timeAllowed;
#ifdef _WIN32
	timerStart(&startTime, &timerFreq);
#else
	startTime = clock();
#endif
	updateStringsStatus(getBoard(), BLACK, NULL, NULL, getMaxStatusDepth(), NULL);
	timePerMove = oldTimePerMove;

	return estimateScore(getBoard(), NULL, 5, 21, false);
}

/** @brief Generate an estimate of the score for the given board.
  * @param b The board to score.
  * @param inCopy An optional pointer to an integer array to store the influence map. Format
  * is influenceMap[x][y].
  * @param scaleToStrength If true then the initial influence generated by stones will
  * be scaled according to the GoString's status value. */
float Melkor::estimateScore(const BoardStruct &b, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE] /*=NULL*/,
							   int dilations /*=5*/, int erosions /*=21*/, bool scaleToStrength /*=true*/)
{
	// Remove strings from the board that are marked dead and run calculateInfluence()
	BoardStruct test(b);

	BoardStruct::stringsIterator iter(test.getStrings().begin());
	while(iter!=test.getStrings().end()) {
#ifdef _DEBUG
		GoString* ptr = &(*iter);
#endif
		if(iter->getStatus()==DEAD) {
			test.removeString(iter);
			// Reset it because the strings list will have been reorganised now
			iter = test.getStrings().begin();
		}
		else
			iter++;
	}

	float score = 0.0f;
	if(inCopy!=NULL)
		calculateInfluence(test, score, BLACK, "Melkor::estimateScore()", inCopy, dilations, erosions, scaleToStrength);
	else
		calculateInfluence(test, score, BLACK, "Melkor::estimateScore()", NULL, dilations, erosions, scaleToStrength);

	// Add on the number of stones captured from blacks point of view
	// Note: In chinese rules captured stones are counted because
	//  they are included through stone counting...
	if(Go::getRules()!=Go::CHINESE_RULES)
		score+=b.getNumberOfCapturedStones();

	// Subtract the value of komi
	score-=Go::getKomi();

	return score;
}

/** @brief Generate a liberty influence map for the given board, removing DEAD strings first.
  * @param b The board to score.
  * @param inCopy An optional pointer to an integer array to store the influence map. Format
  * is influenceMap[x][y].
  */
void Melkor::generateLibertyInfluenceMap(const BoardStruct &b, int inCopy[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
											 int dilations, int erosions)
{
	// Remove strings from the board that are marked dead and run calculateInfluence()
	BoardStruct test(b);

	BoardStruct::stringsIterator iter(test.getStrings().begin());
	while(iter!=test.getStrings().end()) {
#ifdef _DEBUG
		GoString* ptr = &(*iter);
#endif
		if(iter->getStatus()==DEAD) {
			test.removeString(iter);
			// Reset it because the strings list will have been reorganised now
			iter = test.getStrings().begin();
		}
		else
			iter++;
	}

	float score = 0.0f;
	calculateLibertyInfluence(test, score, BLACK, "Melkor::estimateScore()", inCopy, dilations, erosions);
}

/** Enter into a move exchange and confirmation/rejection loop
with the current opponent. This function assumes the game is setup 
appropriately. */
void Melkor::playGo(BoardStruct &b, int compColour /*=BLACK*/)
{
	LogWriter log;
	
	Move m(-1, -1, EMPTY, b.getSize());
	
	computerColour = compColour;
	if(computerColour==BLACK)
		opponentColour = WHITE;
	else
		opponentColour = BLACK;

	b.clear();
	b.enableMarkLastMove(true);
	Go::setNextToPlay(BLACK);
	Go::doFixedHandicap(b);

	mtdfSearch->init();
	
	bool endOfGame = false;
	Go::resetMoveList();

	// do stuff
	while (!endOfGame) {
		b.printAsciiBoard(true);
		// if it's the opponents turn wait for command
		if(Go::getNextToPlay()==opponentColour) {
			log.println("Waiting for your move...");
			string command;
			while(true) {
				cin >> command;
				// check for special commands
				if(command.length()==1) {
					// 'b' to print the board
					if(command[0]=='b')
						b.printAsciiBoard(true);
					// 'h' for help
					else if(command[0]=='h') {
						log.println(" -- Help --");
						log.println("");
						log.println("'b' - To print the board.");
						log.println("'h' - To display help.");
						log.println("'p' - To pass.");
						log.println("'q' - To quit game.");
						log.println("");
						log.println("Enter a move of the form [a-s][1-19]. e.g. r15");
						log.println("");
					}
					// 'p' for pass
					else if(command[0]=='p') {
						Move::getPassMove(m);
						break;
					}
					// 'q' for quit
					else if(command[0]=='q') {
						// ask for confirmation
						log.println("Are you sure you want to quit? (y/n)");
						char temp[2];
						// get rid of '\n' left from previous commands
						cin.getline(temp, 2);
						// read in confirmation of quit
						cin.getline(temp, 2);
						if(temp[0]=='y') {
							endOfGame=true;
							break;
						}
					}
					else
						log.println("Invalid command, type 'h' for help.");
				} // end if special command
				// check for move command
				else if(command.length()==2 || command.length()==3)	{
					// verify letter
					if(command[0]<'a' || command[0]>'s') {
						log.println("Invalid letter, type 'h' for help.");
						continue;
					}
					// verify number, check first number, then second
					// if there is one
					if((command[1]<'1' || command[1]>'9') ||
					   (command.length()==3 && (command[2]<'0' || command[2]>'9'))) {
						log.println("Invalid number, type ''h for help.");
						continue;
					}
					//int x = command[0]-'a';
					//int y = atoi(command.substr(1, 2).c_str())-1;
					m.setX(Move::GoPosToX(command));
					m.setY(Move::GoPosToY(command, b.getSize()));
					break;
				} // end if move command
				else
					log.println("Invalid command, type 'h' for help.");
			} // end while (true)
			
			// check if user quitted
			if(endOfGame) continue;

			m.setColour(opponentColour);
			Go::setNextToPlay(computerColour);

		} // end if opponent's move
		// computers move
		else if (Go::getNextToPlay()==computerColour) {
			log.println("Calculating our move...");
			getMove(m, b, computerColour);
			m.setColour(computerColour);
			Go::setNextToPlay(opponentColour);
		} // end if computer's move

		// check for two passes in a row
		if(Move::xyToGMP(m.getX(), m.getY(), b.getSize())==0) {
			// if it is a pass and the last move was a pass
			// break the loop and end the game
			if(Go::getCurrentMoveNumber()>0 && 
				Move::xyToGMP(Go::getMoveList()[Go::getCurrentMoveNumber()-1].getX(),
							  Go::getMoveList()[Go::getCurrentMoveNumber()-1].getY(),
							  b.getSize())==0) {
				log.println("Two consecutive passes....ending game.", myName);
				endOfGame = true;
			}
		}
		
/** @todo Check legality of move e.g. :
  *		// NOTE: We must specify our colour to be the opposite of the stone being played
  *		// so we avoid the shortcuts that our colour uses such as Go::dontFillOwnEyes
  *		if(!Go::isLegal(go->getBoard(), j, i, BLACK, Go::getUseSuperKo(), false, NULL, false, WHITE))
  *			return gtp_failure(id, "illegal move");
  */

		// add move to the board
		m.setBoardSize(b.getSize());
		Go::addToMoveList(m, b);
		b.addToBoard(m);

	} // end while(!endOfGame)

	/** @todo Add end of game scoring. */
	//score(b);
	//displayScores();
}

// Use GTP to play as a client, not server
void Melkor::playGTP(BoardStruct &b, string gtpScriptFilename, int compColour/*=BLACK*/)
{
	// set up stdout buffering
#ifdef HAVE_SETLINEBUF
	setlinebuf(stdout);
#else
	setbuf(stdout, NULL);
#endif

	// do Melkor specific setup then run play_gtp()
	mtdfSearch->init();
	gtp->play_gtp(gtpScriptFilename);
}

void Melkor::getMove(Move &m, BoardStruct &b, int nextToPlay)
{
	// This option should override the default useGameTreeSearch
	if(useGlobalMoveFunction) {
		// Find the last board position and feed it in
		// Feed NULL if there is no pervious board
		if(Go::getMoveList().size()>0)
			getBestGlobalMove(m, b, nextToPlay, &(Go::getBoardList().back()));
		else
			getBestGlobalMove(m, b, nextToPlay, NULL);
	}
	else if(useGameTreeSearch)
		getBestMoveWithAlphaBeta(m, b, nextToPlay);
	else
		getBestMoveWithoutAlphaBeta(m, b, nextToPlay);	
}

void Melkor::getBestGlobalMove(Move& m, BoardStruct& board, const int nextToPlay, const BoardStruct* previousBoard)
{
#ifdef _WIN32
	LARGE_INTEGER freq, tstart, tend;
	timerStart(&tstart, &freq);
#endif

	/** @todo
	  * A naive method of determining string status might do a narrow, deep search (~8 ply deep)
	  *  (or possibly variable width, i.e. width reduces with depth...) using alpha-beta with
	  *  exLib moves + shape/tesuji moves (a handful top scoring NN moves) to perform a capture goal
	  *  search, assigning dead, alive or unknown to each string.
	  *
	  * A better method would do the above and use that info to estimate
	  *  the status of undecided strings. Or use a TD net to estimate safety
	  *  of UNKNOWN strings. See updateStringStatus().
	  *
	  */

	/** @todo
	  * Create a ConnectGoal and connectExpand. Should test if two like colour strings can connect.
	  * connectExpand should generate all moves, but order them so the ones that reach the
	  * closest to the opposing string are top.
	  *
	  * Use connection search to determine which strings can connect and which strings belong
	  * to which group. Determine group safety rather than string safety. 
	  * 
	  * Possibly use a neural network to guess safety and use life and death search for 
	  * close ALIVE or DEAD guesses. Use 5 neurons per point {ourColour, theirColour, empty,
	  * offboard, target} where target indicates the group we're estimating safety for.
	  * Additionally include no. eyes and liberties as extra features.
	  * - Can we avoid inputting the whole board area point by point by using better features?
	  * Include number of stones, use the influence calculations to determine enemy influence over
	  * the groups liberties (see winhonte)
	  * 
	  * Train the network wth TD, using finished, scored (not time lost or resigned),
	  * 9x9 games from NNGS. Use GnuGo to finished them with --remove-all-dead.
	  * As you train backwards through a game decrease the safety estimate from 0.99
	  * to 0.5 upon when the stone(s) are placed. And from 0.00 to 0.5 for dead stones re-emerging.
	  *
	  * See Melkor/docs/safetyNetworkDesign.txt for more details.
	  */
			
	static string version("v5.04");
	LogWriter log;
	char buffer[1000];
	if(verbose) {
		sprintf(buffer, "Melkor::getBestGlobalMove() version = %s", version.c_str());
		log.println(buffer);
		board.printAsciiBoard();
	}

	useNBigMoves = 8;

	if(getPlayTimedGame()) {
		int t = calculateTimePerMove(nextToPlay, board);
		if(timePerMove==0 || timePerMove>t) {
			timePerMove = t;
			if(verbose) {
				sprintf(buffer, "timePerMove overridden by calculated time - new timePerMove=%gs", timePerMove);
				log.println(buffer);
			}
		}
		else if(verbose) {
			sprintf(buffer, "Calculated time overridden by timePerMove - new timePerMove=%gs", timePerMove);
			log.println(buffer);
		}
	}
	else if(verbose) {
		sprintf(buffer, "timePerMove=%gs", timePerMove);
		log.println(buffer);
	}

	if(getUseDynamicSearchDepth())
		calculateSearchDepths(timePerMove, useNBigMoves);

	double currentTime = 0;
	// Reset the time ran out flag
	timePerMoveRanOut = false;
#ifdef _WIN32
	timerStart(&startTime, &timerFreq);
#else
	startTime = clock();
#endif

	// Get an initial influence map
	float score = 0.0f;
//	int influenceMap[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
//	calculateLibertyInfluence(board, score, BLACK, "Melkor::getBestGlobalMove()", influenceMap);

	/** @todo The liberty influence map could be a good estimate of string safety. Total
	  * the influence values for the strings liberties. Remember +ve indicates BLACK control
	  * and -ve WHITE control. Safety estimate would be based on strength of control... */

	// Check if we've just run getBestGlobalMove() on this board state
	// if so then we don't need to clear the caches...
	if(latestBoardKey != board.getKey()) {

		latestBoardKey = board.getKey();
	
		/** @todo Purge transposition table. 
		  *  Remove positions in the ttable that are impossible to reach from here. */
		mtdfSearch->init();
		
		/** @todo Purge big and urgent move caches. 
		  * Decide which big and urgent moves are still valid and if the scores/lines of play
		  * are still valid or need to be updated... */
		bigMoves.clear();
		urgentMoves.clear();
	}

	// Select only the changed strings and update them...
	vector<Point<> > stringsToUpdate;
	if(previousBoard!=NULL) {
		stringsToUpdate.clear();
		selectChangedStrings(stringsToUpdate, *previousBoard, board);

		selectAdjacentEnemyStrings(stringsToUpdate, board, board.getLastX(), board.getLastY());

		// Also add any strings that haven't ever been updated, i.e. status = UNKNOWN (0)
		const BoardStruct::stringTable& strings = board.getStrings();
		BoardStruct::constStringsIterator citer(strings.begin());
		for(;citer!=strings.end();citer++) {
			if(citer->getStatus()==UNKNOWN)
				addStringIfUnique(board, stringsToUpdate, citer->getPoints().begin()->x, citer->getPoints().begin()->y);
		}
	}

	if(verbose) {
		LogWriter log;
		log.println("\nUpdating initial status of strings...");
	}
	// If this isn't the very first move, update only the strings necessary
	if(previousBoard!=NULL)
		updateStringsStatus(board, nextToPlay, &urgentMoves, &stringsToUpdate, getMaxStatusDepth(), NULL);
	else
		updateStringsStatus(board, nextToPlay, &urgentMoves, NULL, getMaxStatusDepth(), NULL);

	/** @todo Instead of global 1 ply influence eval, try this:
	  *
	  *  updateStringsStatus()
	  *  estimateWorthOfUrgentMoves()       // Perhaps by influence eval with and without the string
	  *  getBigMoves()                      // i.e. n top scoring NN moves, estimate worth by influence eval
	  *  selectBiggestMove()                // Choose the biggest move out of NN moves and urgent moves
	  *
	  */

	// Get a current score estimate
	float currentScore = estimateScore(board);

	/** @todo Here we should try a pass move and see if the score changes if we do nothing
	  * and give the enemy a free go, if the score doesn't change then just pass. 
	  * Use ordered NN scores to select white moves to try, until we find one that
	  * causes a change in score. This might be expensive on calls to estimateScore()
	  * and updateStringsStatus() for the white move being added... */

	// This changes the worth value in urgentMoves[n]
	// to a measure of the worth of the move - rather than the NN score of the move
	estimateWorthOfUrgentMoves(board, nextToPlay, urgentMoves);

	// time check - choose best urgent move if time run out
	if(timePerMove==0 || !timePerMoveRanOut)
		getBigMoves(bigMoves, &urgentMoves, board, nextToPlay, useNBigMoves);

	/** @todo Here we should trade urgent and big moves with the enemy, in order of worth
	  * to some depth (e.g. 3). Then possibly update the worths relative to each other.
	  * Iterate this process a few times to increase the worth accuracy... */

	// Find best urgent move
	float bestUrgentMoveScore;
	if(nextToPlay==BLACK)
		bestUrgentMoveScore = MTDFSearch::NEGINFINITY;
	else
		bestUrgentMoveScore = MTDFSearch::POSINFINITY;
	int bestUrgentIndex = -1;
	int count = 0;
	vector<UrgentMoveInfo>::const_iterator citer(urgentMoves.begin());
	for(;citer!=urgentMoves.end();citer++) {
		// Remember change in score is from blacks p.o.v. so negate the change if nextToPlay == WHITE
		if(nextToPlay==BLACK && citer->worth > bestUrgentMoveScore) {
			bestUrgentMoveScore = citer->worth;
			bestUrgentIndex = count;
		}
		else if(nextToPlay==WHITE && citer->worth < bestUrgentMoveScore) {
			bestUrgentMoveScore = citer->worth;
			bestUrgentIndex = count;
		}
		count++;
	}

	// Find best big move
	float bestBigMoveScore;
	if(nextToPlay==BLACK)
		bestBigMoveScore = MTDFSearch::NEGINFINITY;
	else
		bestBigMoveScore = MTDFSearch::POSINFINITY;
	int bestBigIndex = -1;
	count = 0;
	vector<BigMoveInfo>::const_iterator biter(bigMoves.begin());
	for(;biter!=bigMoves.end();biter++) {
		// Remember change in score is from blacks p.o.v. so negate the change if nextToPlay == WHITE
		if(nextToPlay==BLACK && biter->worth > bestBigMoveScore) {
			bestBigMoveScore = biter->worth;
			bestBigIndex = count;
		}
		else if(nextToPlay==WHITE && biter->worth < bestBigMoveScore) {
			bestBigMoveScore = biter->worth;
			bestBigIndex = count;
		}
		count++;
	}

	// Check we actually have a move to choose from
	if(bestBigIndex==-1 && bestUrgentIndex==-1) {
		if(noPasses) {
			// See if there are any legal moves available
			vector<Move> allMoves;
			getScoredMoves(allMoves, board, nextToPlay, *getBPN(), false, false, NULL, NULL);
			if(allMoves.size()==0)
				Move::getPassMove(m);
			else
				m = allMoves[0];
		}
		else
			Move::getPassMove(m);
	}
	// Otherwise select best move 
	else if(nextToPlay==BLACK) {
		// If our potential move worth is worth 0 or less, pass instead...
		// However if this is because we have run out of time keep the chosen move as a last resort
		if(!noPasses && (timePerMove==0 || !timePerMoveRanOut) && maximum(bestBigMoveScore, bestUrgentMoveScore)<=0.0f)
			Move::getPassMove(m);
		// Is the big move better than the urgent move?
		else if(bestBigMoveScore > bestUrgentMoveScore)
			m = bigMoves[bestBigIndex].move;
		else
			m = urgentMoves[bestUrgentIndex].bestLineOfPlay[0];
	}
	else if(nextToPlay==WHITE) {
		// If our potential move worth is 0 or more (from blacks pov), pass instead...
		// However if this is because we have run out of time keep the chosen move as a last resort
		if(!noPasses && (timePerMove==0 || !timePerMoveRanOut) && minimum(bestBigMoveScore, bestUrgentMoveScore)>=0.0f)
			Move::getPassMove(m);
		// Is the big move better than the urgent move?
		else if(bestBigMoveScore < bestUrgentMoveScore)
			m = bigMoves[bestBigIndex].move;
		else
			m = urgentMoves[bestUrgentIndex].bestLineOfPlay[0];
	}
	m.setBoardSize(board.getSize());
	m.setColour(nextToPlay);

	// Print a list of all the urgent and big moves with scores
	if(verbose) {
		LogWriter log;
		string message;
		char buffer[2000];
		// Print the urgent moves first
		if(urgentMoves.size()>0)
			log.println("\nUrgent moves: \n");
		else
			log.println("\nNo urgent moves.\n");
		for(int i=0;i<urgentMoves.size();i++) {
			message = "";
			for(int j=0;j<urgentMoves[i].bestLineOfPlay.size();j++) {
				message+=Move::xyToColourAndGoPosValue(urgentMoves[i].bestLineOfPlay[j]);
				message+=" ";
			}
			message+=" {";
			int x = urgentMoves[i].string->getPoints().begin()->x;
			int y = urgentMoves[i].string->getPoints().begin()->y;
			message+= Move::xyToGoPosValue(x, y, board.getSize());
			message+="} = ";
			sprintf(buffer, "%g", urgentMoves[i].worth);
			message+=buffer;
			log.println(message);
		}

		// Now print the big moves
		if(bigMoves.size()>0)
			log.println("\nBig moves: \n");
		else
			log.println("\nNo big moves.\n");
		for(i=0;i<bigMoves.size();i++) {
			message = Move::xyToColourAndGoPosValue(bigMoves[i].move);
			message+=" = ";
			sprintf(buffer, "%g", bigMoves[i].worth);
			message+=buffer;
			log.println(message);
		}

#ifdef _WIN32
		timerEnd(&tend);
		getBestGlobalMoveCalls++;
		// work out time in milliseconds
		double t = timerValue(tstart, tend, freq)*1000;
		sprintf(buffer, "Time taken for this move: %*gms", 7, t);
		log.println(buffer);
		sprintf(buffer, "Search-Type\tAvg-1-ply(speedConstants)\tTotal-Searches\tAvg-Searches-Per-Call");
		log.println(buffer);
		sprintf(buffer, "Ladder\t\t%g\t\t\t%d\t\t%d", totalLadder1PlyTime/totalLadderSearches, totalLadderSearches, totalLadderSearches/getBestGlobalMoveCalls);
		log.println(buffer);
		sprintf(buffer, "LooseTactical\t%g\t\t\t%d\t\t%d", totalLooseTactical1PlyTime/totalLooseTacticalSearches, totalLooseTacticalSearches, totalLooseTacticalSearches/getBestGlobalMoveCalls);
		log.println(buffer);
		sprintf(buffer, "Status\t\t%g\t\t\t%d\t\t%d", totalStatus1PlyTime/totalStatusSearches, totalStatusSearches, totalStatusSearches/getBestGlobalMoveCalls);
		log.println(buffer);
		sprintf(buffer, "BigSequence\t%g\t\t\t%d\t\t%d", totalBigSequence1PlyTime/totalBigSequenceSearches, totalBigSequenceSearches, totalBigSequenceSearches/getBestGlobalMoveCalls);
		log.println(buffer);
#endif
	}
}

/** @brief Update the status of the specified strings in board, identifying urgent moves if required.
  * @param board The board to look at.
  * @param nextToPlay The colour next to play.
  * @param urgentMoves If this is NULL don't store any urgent move information. Otherwise, store it.
  * @param stringsToUpdate Update the strings pointed to in this vector. If NULL update all strings.
  * @param linesOfPlay Store the lines of play found. If NULL don't save any lines of play.
  */
void Melkor::updateStringsStatus(BoardStruct& board, const int nextToPlay, vector<UrgentMoveInfo>* urgentMoves,
									vector<Point<> >* stringsToUpdate, const int maxDepth,
									vector<LineOfPlay>* linesOfPlay)
{
	LogWriter log;
	string message;
	char buffer[2000];

#ifdef TIME_SEARCHES
	LARGE_INTEGER freq, tstart;
#endif

	if(urgentMoves!=NULL)
		urgentMoves->clear();

	if(linesOfPlay!=NULL)
		linesOfPlay->clear();

	int notNextToPlay = BLACK;
	if(nextToPlay==BLACK)
		notNextToPlay = WHITE;

	vector<Move> worstLineOfPlay;
	vector<Move> bestLineOfPlay;
	BoardStruct::stringsIterator iter(board.getStrings().begin());
	Node root(board, nextToPlay);
	Node newRoot;
	float score;
	int searchDepth = maxDepth;

	if(verbose) {
		sprintf(buffer, "maxDepth=%d maxLadderDepth=%d", maxDepth, maxLadderDepth);
		log.println(buffer);
	}

	// Generate a liberty influence map for this board
	int influenceMap[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
//	calculateLibertyInfluence(board, score, nextToPlay, "Melkor::updateStringsStatus()",
//		influenceMap, 3, 0);
	generateLibertyInfluenceMap(board, influenceMap, 3, 0);

	double lastTime;
#ifdef _WIN32
	LARGE_INTEGER localStart;
	timerStart(&localStart, &timerFreq);
#else
	clock_t localStart = clock();
#endif

	for(;iter!=board.getStrings().end();iter++)	{
		// Before updating a strings status check the time limit
		if(checkTimePerMove(&localStart, lastTime)) {
			if(verbose)
				log.println("Time has run out in updateStringsStatus()...");
			return;
		}

		// Use stringsToUpdate if specified
		if(stringsToUpdate!=NULL) {
			// Is this string (iter) one of the strings to update?
			bool update = false;
			vector<Point<> >::const_iterator citer(stringsToUpdate->begin());
			for(;citer!=stringsToUpdate->end();citer++) {
				if(iter->containsPoint(citer->x, citer->y)) {
					update = true;
					break;
				}
			}

			if(!update)
				continue;
		}

		int anchorX = iter->getPoints().begin()->x;
		int anchorY = iter->getPoints().begin()->y;
		int anchorColour = iter->getColour();

		SearchType searchType = LADDER;
		double searchTime = 0.0;

		// If the status has not been estimated yet, give it
		// an initial value equal to the number of liberties the string has
		if(iter->getStatus()==UNKNOWN)
			iter->setStatus(iter->getLibertyCount());

		// Ignore definitely dead or alive strings
		// Capture goals only return ALIVE, UNKNOWN or DEAD atm...
		if(verbose) {
			message = "{";
			message+=Move::xyToGoPosValue(iter->getPoints().begin()->x, iter->getPoints().begin()->y, board.getSize());
			message+="} is currently ";
			if(iter->getStatus()==ALIVE)
				message+="ALIVE";
			else if(iter->getStatus()==DEAD)
				message+="DEAD";
			else if(iter->getStatus()==URGENT)
				message+="URGENT ";
			else {
				char buffer[1000];
				int status = iter->getStatus();
				sprintf(buffer, "UNKNOWN (%d)", status);
				message+=buffer;
			}
			log.println(message);
		}

		/* Use board influence to determine strength of threat from enemy stones.
		 * In a sparse area there is less threat to a string than in an enemy area.
		 * For lesser threatened strings we can skip the status update. */
		// For strings with 1, 2 or 3 stones, use the enemies influence over their liberties
		// from the influence map to estimate life and death. Exclude strings that can be read as ladders
		// i.e. 1 or 2 liberties!
		if(iter->getStoneCount()<4 && iter->getLibertyCount()>2) {
			/* Why do we do this? 
			 * We check for shared friendly liberties because they indicate the string
			 * may have an easy connection option, relating to its status, therefore
			 * a more complex search will be required. */
			// Also exclude strings that share a liberty with a friendly string!

			if(!Go::hasFriendlySharedLiberty(board, iter)) {
				// Note: Black influence is +v.e, white is -v.e
				// Use Go::maxInfluenceBlack and Go::maxInfluenceWhite to determine how heavy the influence is...
				// Note: influenceMap format is [x][y]

				// Total up the surrounding influence values for all liberties
				// If total is -v.e for BLACK or +v.e for WHITE, string is dead
				// A total of 0 implies balance, perhaps dependent on who plays first
				int libertyInfluence = 0;
				GoString::constPointsIterator piter(iter->getPoints().begin());
				for(;piter!=iter->getPoints().end();piter++) {
					// North
					if((piter->y-1)>=0 && board.getPoint(piter->x, piter->y-1)==EMPTY)
						libertyInfluence+=influenceMap[piter->x][piter->y-1];
					// South
					if((piter->y+1)<board.getSize() && board.getPoint(piter->x, piter->y+1)==EMPTY)
						libertyInfluence+=influenceMap[piter->x][piter->y+1];
					// East
					if((piter->x+1)<board.getSize() && board.getPoint(piter->x+1, piter->y)==EMPTY)
						libertyInfluence+=influenceMap[piter->x+1][piter->y];
					// West
					if((piter->x-1)>=0 && board.getPoint(piter->x-1, piter->y)==EMPTY)
						libertyInfluence+=influenceMap[piter->x-1][piter->y];
				}
				bool isDead = false;
				if(anchorColour==BLACK && libertyInfluence < 0)
					isDead = true;
				if(anchorColour==WHITE && libertyInfluence > 0)
					isDead = true;
				
				if(isDead) {
					if(verbose) {
						log.println("String has 1, 2 or 3 stones, so estimating life and death from influence map...");
						log.println("Majority of liberties are controlled by enemy, so assuming DEAD");
						log.println("Status updated to DEAD");
					}
					iter->setStatus(DEAD);
					continue;
				}
				// Also do the converse; if libertyInfluence is favourable set status to liberty count.
				else if((anchorColour==BLACK && libertyInfluence > (Go::maxInfluenceBlack/20)) ||
					(anchorColour==WHITE && libertyInfluence < (Go::maxInfluenceWhite/20))) {
					if(verbose) {
						log.println("String has 1, 2 or 3 stones, so estimating life and death from influence map...");
						log.println("Majority of liberties are controlled by string, so assuming UNKNOWN(liberties)");
						int libs = iter->getLibertyCount();
						sprintf(buffer, "Status updated to UNKNOWN (%d)", libs);
						log.println(buffer);
					}
					iter->setStatus(iter->getLibertyCount());
					continue;
				}
			} // end if !sharedLiberty
		}

		// Only update strings with 5 or less liberties to save time
		if(iter->getLibertyCount()>5) {
			if(verbose)
				log.println("Skipping string since it has greater than 5 liberties.\n");
			continue;
		}

		if(iter->getStatus()!=ALIVE && iter->getStatus()!=DEAD) {
			int stringColour = iter->getColour();
			mtdfSearch->saveSettings();
			mtdfSearch->setupMTDFSearch(true, true, true, 0.0f, true, 0.0f, false, true, true);

			mtdfSearch->clearSearchExtensions();
			LadderExtension lEx(mtdfSearch->getSettings());
			// Avoid recursive search extension calls
			lEx.getSettings().clearSearchExtensions();
			lEx.getSettings().setMoveSuggester( new LadderSuggester() );
			lEx.getSettings().setGoal(Goal(new LadderGoal(anchorX, anchorY)));
			lEx.getSettings().setDepth(maxLadderDepth);

			// Decide what sort of tactical search we need to do:
			//   1-2 liberties, no shared friendly liberties - Do a ladder search.
			//   1-3 liberties                               - Do a loose tactical search.
			//   4-5 liberties                               - Do a full-width tactical search.
			if(iter->getLibertyCount()<=2 && !Go::hasFriendlySharedLiberty(board, iter)) {
				mtdfSearch->setMoveSuggester( new LadderSuggester() );
				mtdfSearch->setGoal(Goal(new LadderGoal(anchorX, anchorY)));
				searchType = LADDER;
				searchDepth = maxLadderDepth;
				if(verbose)
					log.println("Ladder search initialised.");
			}
			else if(iter->getLibertyCount()<=3) {
				mtdfSearch->registerSearchExtension(lEx);
				mtdfSearch->setMoveSuggester( new LooseTacticalSuggester() );
				mtdfSearch->setGoal(Goal(new LooseTacticalGoal(anchorX, anchorY)));
				searchType = LOOSE_TACTICAL;
				searchDepth = maxLooseTacticalDepth;
				if(verbose)
					log.println("Loose tactical search initialised.");
			}
			else {
				mtdfSearch->registerSearchExtension(lEx);
				mtdfSearch->setMoveSuggester( new CaptureSuggester() );
				mtdfSearch->setGoal(Goal(new LifeDeathGoal(anchorX, anchorY)));
				searchType = STATUS;
				searchDepth = maxDepth;
				if(verbose)
					log.println("Full tactical search initialised.");
			}

			// The transposition table must be cleaned if the goal changes
			mtdfSearch->cleanTranspositionTable();

			// Set the time limit for the alpha-beta functions
			// How much time do we have left?
			if(timePerMove!=0) {
				// Give the alpha-beta search only 90% of the time
				// we have left to allow for overheads...
				mtdfSearch->setTimeLimit(getTimeLeftPerMove()*0.9);
			}
			else
				mtdfSearch->setTimeLimit(0);

			// If it's our string see if the enemy can kill it
			if(stringColour==nextToPlay) {
				/** @todo If we have a line of play for this move with enemy going first,
				  * setup the best node chain in transposition table... 
				  * Need to check legality of moves first, but if not much local change,
				  * would be worth it. */

				startSearchTimer(&tstart, &freq);

				// Note: This is effectively making nextToPlay pass, therefore we
				// only need to search to maxDepth - 1 !!!
				mtdfSearch->setDepth(searchDepth-1);
				mtdfSearch->setMaximisingColour(stringColour);
				mtdfSearch->setNextToPlay(notNextToPlay);
				root.reset(board, mtdfSearch->getNextToPlay());
				score = mtdfSearch->runSearchAsSettings(&root, board, iter->getStatus());

				searchTime = endSearchTimer(&tstart, &freq);
				updateSearchTimingStats(searchTime, searchType, mtdfSearch->getLastIDDepth());

				// Find the upto-date root in the transposition table
				mtdfSearch->lookup(root, newRoot);
				mtdfSearch->updateBestLineOfPlay(newRoot, board.getSize());
				worstLineOfPlay = mtdfSearch->getBestLineOfPlay();
				
				if(linesOfPlay!=NULL)
					linesOfPlay->push_back(LineOfPlay(worstLineOfPlay, anchorX, anchorY, anchorColour));

				// Is it unconditionally dead or can we save it now?
				if(score==DEAD) {
					/** @todo If we have a line of play for this move with string going first,
					  * setup the best node chain in transposition table... 
					  * Need to check legality of moves first, but if not much local change,
					  * would be worth it. */

					startSearchTimer(&tstart, &freq);

					mtdfSearch->setDepth(searchDepth);
					mtdfSearch->setMaximisingColour(stringColour);
					mtdfSearch->setNextToPlay(nextToPlay);
					root.reset(board, mtdfSearch->getNextToPlay());
					score = mtdfSearch->runSearchAsSettings(&root, board, iter->getStatus());

					searchTime = endSearchTimer(&tstart, &freq);
					updateSearchTimingStats(searchTime, searchType, mtdfSearch->getLastIDDepth());

					// Find the upto-date root in the transposition table
					mtdfSearch->lookup(root, newRoot);
					mtdfSearch->updateBestLineOfPlay(newRoot, board.getSize());
					bestLineOfPlay = mtdfSearch->getBestLineOfPlay();
					
					if(linesOfPlay!=NULL)
						linesOfPlay->push_back(LineOfPlay(bestLineOfPlay, anchorX, anchorY, anchorColour));
					
					// If it's not DEAD now save the move as urgent
					if(score!=DEAD) {
						// Save the urgent move info
						if(urgentMoves!=NULL)
							urgentMoves->push_back(UrgentMoveInfo(nextToPlay, bestLineOfPlay, worstLineOfPlay, &(*iter)));
						iter->setStatus(URGENT);

						if(verbose) {
							message = "Status updated to URGENT, strings best line of play: ";
							for(int j=0;j<bestLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(bestLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
							message = "   Attackers best line of play: ";
							for(j=0;j<worstLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(worstLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
						}
					}
					else {
						iter->setStatus(DEAD);
						if(verbose) {
							message = "Status updated to DEAD, strings best line of play: ";
							for(int j=0;j<bestLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(bestLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
						}
					}
				}
				else {
					iter->setStatus(score);
					if(verbose) {
						message = "Status updated to ";
						if(score==ALIVE) {
							message+="ALIVE, worst the string enemy can do: ";
							for(int j=0;j<worstLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(worstLineOfPlay[j]);
								message+=" ";
							}
						}
						else if(score==URGENT)
							message+="URGENT";
						else {
							sprintf(buffer, "UNKNOWN (%g)", score);
							message+=buffer;
							message+=", worst the string enemy can do: ";
							for(int j=0;j<worstLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(worstLineOfPlay[j]);
								message+=" ";
							}
						}
						log.println(message);
					}
				}
			} // end if stringColour==nextToPlay
			// Otherwise the string is the enemies, so can we kill it?
			else {
				/** @todo If we have a line of play for this move with string going first,
				  * setup the best node chain in transposition table... 
				  * Need to check legality of moves first, but if not much local change,
				  * would be worth it. */

				startSearchTimer(&tstart, &freq);

				// Is the string dead if we go first?
				mtdfSearch->setDepth(searchDepth);
				mtdfSearch->setMaximisingColour(stringColour);
				mtdfSearch->setNextToPlay(nextToPlay);
				root.reset(board, mtdfSearch->getNextToPlay());
				score = mtdfSearch->runSearchAsSettings(&root, board, iter->getStatus());

				searchTime = endSearchTimer(&tstart, &freq);
				updateSearchTimingStats(searchTime, searchType, mtdfSearch->getLastIDDepth());

				// Save the line of play here, since we need to know what we should do
				// if this string turns out to be urgent....
				
				// Find the upto-date root in the transposition table
				mtdfSearch->lookup(root, newRoot);
				mtdfSearch->updateBestLineOfPlay(newRoot, board.getSize());
				bestLineOfPlay = mtdfSearch->getBestLineOfPlay();
				
				if(linesOfPlay!=NULL)
					linesOfPlay->push_back(LineOfPlay(bestLineOfPlay, anchorX, anchorY, anchorColour));

				// If it's dead anyway, can they save it by going first?
				if(score==DEAD) {
					/** @todo If we have a line of play for this move with enemy going first,
					  * setup the best node chain in transposition table... 
					  * Need to check legality of moves first, but if not much local change,
					  * would be worth it. */

					startSearchTimer(&tstart, &freq);

					// Note: This is effectively making nextToPlay pass, therefore we
					// only need to search to maxDepth - 1 !!!
					mtdfSearch->setDepth(searchDepth-1);
					mtdfSearch->setMaximisingColour(stringColour);
					mtdfSearch->setNextToPlay(notNextToPlay);
					root.reset(board, mtdfSearch->getNextToPlay());
					score = mtdfSearch->runSearchAsSettings(&root, board, iter->getStatus());

					searchTime = endSearchTimer(&tstart, &freq);
					updateSearchTimingStats(searchTime, searchType, mtdfSearch->getLastIDDepth());

					// Find the upto-date root in the transposition table
					mtdfSearch->lookup(root, newRoot);
					mtdfSearch->updateBestLineOfPlay(newRoot, board.getSize());
					worstLineOfPlay = mtdfSearch->getBestLineOfPlay();
					
					if(linesOfPlay!=NULL)
						linesOfPlay->push_back(LineOfPlay(worstLineOfPlay, anchorX, anchorY, anchorColour));
					
					// If it's not DEAD now save the move as urgent
					if(score!=DEAD) {

						// Save the urgent move info
						if(urgentMoves!=NULL)
							urgentMoves->push_back(UrgentMoveInfo(nextToPlay, bestLineOfPlay, worstLineOfPlay, &(*iter)));
						iter->setStatus(URGENT);

						if(verbose) {
							message = "Status updated to URGENT, attackers best line of play: ";
							for(int j=0;j<bestLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(bestLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
							message = "   Best line of play for string: ";
							for(j=0;j<worstLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(worstLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
						}
					}
					else {
						iter->setStatus(DEAD);
						if(verbose) {
							message = "Status updated to DEAD, attackers best line of play: ";
							for(int j=0;j<bestLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(bestLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
							message = "   Best line of play for string: ";
							for(j=0;j<worstLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(worstLineOfPlay[j]);
								message+=" ";
							}
							log.println(message);
						}
					}
				} // end if score==DEAD
				else {
					iter->setStatus(score);
					if(verbose) {
						message = "Status updated to ";
						if(score==ALIVE) {
							message+="ALIVE, the worst the string enemy can do: ";
							for(int j=0;j<bestLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(bestLineOfPlay[j]);
								message+=" ";
							}
						}
						else if(score==URGENT)
							message+="URGENT";
						else {
							sprintf(buffer, "UNKNOWN (%g)", score);
							message+=buffer;
							message+=", the worst the string enemy can do: ";
							for(int j=0;j<bestLineOfPlay.size();j++) {
								message+=Move::xyToColourAndGoPosValue(bestLineOfPlay[j]);
								message+=" ";
							}
						}
						log.println(message);
					}
				} // end else from (if score==DEAD)
				// Don't forget to restore the previous search settings
				mtdfSearch->restoreSettings();
			} // end else from (if stringColour==nextToPlay)
		}
	}
}

/** @brief This changes the worth value in urgentMoves[n]
  * to a measure of the worth of the move - rather than the NN score of the move.
  * Note: Move worth is always from Blacks point of view.
  * @param board The board position these urgent moves apply to.
  * @param nextToPlay The colour to play next.
  * @param urgentMoves A vector containing information about the urgent moves.
  * @see UrgentMoveInfo */
void Melkor::estimateWorthOfUrgentMoves(const BoardStruct& board, const int nextToPlay, 
										   vector<UrgentMoveInfo>& urgentMoves)
{
	LogWriter log;
	char buffer[1000];
	string message;
	// Use influence eval with and without the string to estimate worth of move
	if(verbose)
		log.println("\nEstimating worth of urgent moves...");
	
	// Copy board and change urgent tag to DEAD for each string
	BoardStruct boardCopy(board);
	vector<UrgentMoveInfo>::iterator iter(urgentMoves.begin());

	double lastTime;
#ifdef _WIN32
	LARGE_INTEGER localStart;
	timerStart(&localStart, &timerFreq);
#else
	clock_t localStart = clock();
#endif

	for(;iter!=urgentMoves.end();iter++) {
		// Before updating a strings status check the time limit
		if(checkTimePerMove(&localStart, lastTime))
			return;

#ifdef _DEBUG
		GoString* ptr = iter->string;
		vector<Move>& bestLineOfPlay = iter->bestLineOfPlay;
		vector<Move>& worstLineOfPlay = iter->worstLineOfPlay;

		// Get string in board copy
		GoString::constPointsIterator piter(iter->string->getPoints().begin());
		// piter must not equal end() otherwise there are not stones in this string!!!
		assert(piter!=iter->string->getPoints().end());
#endif
	
		if(verbose) {
			message = "Urgent move ";
			message+=Move::xyToColourAndGoPosValue((*iter->bestLineOfPlay.begin()));
			message+=" for string {";
			message+=Move::xyToGoPosValue(iter->string->getPoints().begin()->x,
				iter->string->getPoints().begin()->y, board.getSize());
			message+="}";
			log.println(message);
		}
		
		// Use the _best_ line of play and the worst line of play (i.e. enemy moves 
		// in updateStringStatus()) and use that to compare to, to get a fair estimate
		// of score and therefore how urgent this string is to save
		// i.e. What's our best position (best line of play) and worst position (worst line of play)?
		//    worth of move = bestPositionScore - worstPositionScore

		/**************************/
		/* Find the best score... */
		/**************************/
		if(verbose)
			log.println("Finding best score...");

		// Reset board copy
		boardCopy = board;
		// The urgent moves must be played as far as we know them
		// otherwise if we just remove stones, without adding the capturing ones...
		int currentNextToPlay = nextToPlay;
		message = "Urgent best line of play: ";
		for(int i=0;i<iter->bestLineOfPlay.size();i++) {
			boardCopy.addToBoard(iter->bestLineOfPlay[i]);
			if(currentNextToPlay==BLACK)
				currentNextToPlay = WHITE;
			else
				currentNextToPlay = BLACK;
			if(verbose) {
				message+=Move::xyToColourAndGoPosValue(iter->bestLineOfPlay[i]);
				message+=" ";
			}
		}
		if(verbose)
			log.println(message);

		// Select new or changed strings only and update their status
		if(verbose)
			log.println("Updating status after adding string's urgent best line of play...");
		vector<Point<> > changedStrings;
		selectChangedStrings(changedStrings, board, boardCopy);
		updateStringsStatus(boardCopy, currentNextToPlay, NULL, &changedStrings, getMaxStatusDepth(), NULL);

		if(timePerMove!=0 && timePerMoveRanOut)
			return;

		// Find the score for this best line of play
		float bestScore = estimateScore(boardCopy);
		if(verbose) {
			sprintf(buffer, "Best score for urgent move %s = %g", 
				Move::xyToColourAndGoPosValue((*iter->bestLineOfPlay.begin())).c_str(), bestScore);
			log.println(buffer);
		}

		/***************************/
		/* Find the worst score... */
		/***************************/
		if(verbose)
			log.println("Finding worst score...");

		// Reset board copy
		boardCopy = board;
		// The urgent moves must be played as far as we know them
		// otherwise if we just remove stones, without adding the capturing ones...
		if(nextToPlay==BLACK)
			currentNextToPlay = WHITE;
		else
			currentNextToPlay = BLACK;
		message = "Urgent worst line of play: ";
		for(i=0;i<iter->worstLineOfPlay.size();i++) {
			boardCopy.addToBoard(iter->worstLineOfPlay[i]);
			if(currentNextToPlay==BLACK)
				currentNextToPlay = WHITE;
			else
				currentNextToPlay = BLACK;
			if(verbose) {
				message+=Move::xyToColourAndGoPosValue(iter->worstLineOfPlay[i]);
				message+=" ";
			}
		}
		if(verbose)
			log.println(message);

		// Select new or changed strings only and update their status
		if(verbose)
			log.println("Updating status after adding string's urgent worst line of play...");
		changedStrings.clear();
		selectChangedStrings(changedStrings, board, boardCopy);
		updateStringsStatus(boardCopy, currentNextToPlay, NULL, &changedStrings, getMaxStatusDepth(), NULL);

		if(timePerMove!=0 && timePerMoveRanOut)
			return;

		// Find the score for this worst line of play
		float worstScore = estimateScore(boardCopy);
		if(verbose) {
			LogWriter log;
			char buffer[1000];
			sprintf(buffer, "Worst score for urgent move %s = %g", 
				Move::xyToColourAndGoPosValue((*iter->bestLineOfPlay.begin())).c_str(), worstScore);
			log.println(buffer);
		}

		// Worth is best score - worst score
		iter->worth = bestScore - worstScore;
		if(verbose) {
			LogWriter log;
			char buffer[1000];
			sprintf(buffer, "Worth of urgent move %s = %g",
				Move::xyToColourAndGoPosValue((*iter->bestLineOfPlay.begin())).c_str(), iter->worth);
			log.println(buffer);
		}
	}
}

/** @brief This changes the worth value in (BigMoveInfo)bigMoves[n]
  * to a measure of the worth of the move. Move worth is in points,
  * i.e. you're up by 30 points if you play this move, down by 20 if
  * you don't, therefore the worth of this move is 50 points...
  * @param board The board position these big moves apply to.
  * @param nextToPlay The colour to play next.
  * @param bigMoves A vector containing information about the big moves.
  * @see BigMoveInfo */
void Melkor::estimateWorthOfBigMoves(const BoardStruct& board, const int nextToPlay, 
										   vector<BigMoveInfo>& bigMoves)
{
	LogWriter log;
	char buffer[1000];
	string message;
	if(verbose) {
		log.println("\nEstimating worth of big moves...");

		// Print a warning for strings that haven't had their status updated yet, i.e. UNKNOWN
		const BoardStruct::stringTable& strings = board.getStrings();
		BoardStruct::constStringsIterator citer(strings.begin());
		for(;citer!=strings.end();citer++) {
			if(citer->getStatus()==UNKNOWN) {
				sprintf(buffer, "Warning: String at %s status has not been tested...", 
					Move::xyToGoPosValue(citer->getPoints().begin()->x,
						citer->getPoints().begin()->y, board.getSize()).c_str());
				log.println(buffer);
			}
		}
	}

	int notNextToPlay = BLACK;
	if(nextToPlay == BLACK)
		notNextToPlay = WHITE;
	int newNextToPlay;
	float bestScore, worstScore;
	BoardStruct boardCopy(board);
	vector<BigMoveInfo>::iterator iter(bigMoves.begin());

	// Use influence eval and updateStringsStatus() to determine a more accurate
	// value for each big move based on string status updates
	vector<LineOfPlay> linesOfPlay;
	vector<Point<> > stringsToUpdate;
	int lastMoveColour;
	Node root, newRoot;

	float firstGuess = estimateScore(board);

	double lastTime;
#ifdef _WIN32
	LARGE_INTEGER localStart;
	timerStart(&localStart, &timerFreq);
#else
	clock_t localStart = clock();
#endif

#ifdef TIME_SEARCHES
	LARGE_INTEGER freq, tstart;
#endif
	double searchTime = 0.0;

	for(iter=bigMoves.begin();iter!=bigMoves.end();iter++) {
		// Before updating a strings status check the time limit
		if(checkTimePerMove(&localStart, lastTime)) {
			log.println("Time has run out in estimateWorthOfBigMoves()...");
			return;
		}

		if(verbose) {
			message = "\nBig move ";
			message+=Move::xyToColourAndGoPosValue(iter->move);
			log.println(message);
		}
		
		// Worth of a move is the difference between us playing and the enemy playing it
		
		// Find best score...
		if(verbose) {
			if(nextToPlay==BLACK)
				log.println("Finding best score, playing as BLACK...");
			else
				log.println("Finding best score, playing as WHITE...");
		}
		// Reset board copy
		boardCopy = board;
		// Add move to board copy
		boardCopy.addToBoard( Move(iter->move.getX(), iter->move.getY(), nextToPlay, board.getSize()) );

		/**************************/
		/* Find big move sequence */
		/**************************/

		/** Run a minimax search with very low branching factor and Influence goal to find 
		  * the best sequence of exchanged big moves. */

		/* This search must be shallow and narrow because using the Influence goal without updating
         * the status of the strings may give an unrealistic line of play. */

		// Setup the game tree search
		mtdfSearch->saveSettings();
		mtdfSearch->setupMTDFSearch(true, true, true, 0.35f, true, 0.35f, false, true, true);
		mtdfSearch->setMoveSuggester( new GeneralSuggester() );
		mtdfSearch->setGoal(Goal(new InfluenceGoal(iter->move.getX(), iter->move.getY())));
		if(verbose)
			log.println("Big sequence search initialised.");

		mtdfSearch->cleanTranspositionTable();
		if(timePerMove!=0)
			mtdfSearch->setTimeLimit(getTimeLeftPerMove()*0.9);
		else
			mtdfSearch->setTimeLimit(0);

		startSearchTimer(&tstart, &freq);

		// Enemy to play first
		root.reset(boardCopy, notNextToPlay);
		// Maximising colour must always be BLACK
		mtdfSearch->iterativeDeepening(boardCopy, &root, maxBigSequenceDepth-1,
			mtdfSearch->getGoal().getRequiredMaximisingColour(boardCopy, nextToPlay), 1, firstGuess);

		searchTime = endSearchTimer(&tstart, &freq);
		updateSearchTimingStats(searchTime, BIG_SEQUENCE, mtdfSearch->getLastIDDepth());

		lastMoveColour = nextToPlay;

		// Make sure a search actually happened here...
		if(mtdfSearch->getLastIDDepth()>0) {
			// Find the upto-date root in the transposition table
			mtdfSearch->lookup(root, newRoot);
			mtdfSearch->updateBestLineOfPlay(newRoot, boardCopy.getSize());
			const vector<Move>& lineOfPlay = mtdfSearch->getBestLineOfPlay();
			if(lineOfPlay.size()>0)
				lastMoveColour = lineOfPlay[lineOfPlay.size()-1].getColour();
		
			// Add all the moves from the line of play, note the colour of the last move
			string message("Big move sequence: ");
			vector<Move>::const_iterator miter(lineOfPlay.begin());
			for(;miter!=lineOfPlay.end();miter++) {
				if(verbose) {
					message+=Move::xyToColourAndGoPosValue(*miter);
					message+=" ";
				}
				boardCopy.addToBoard( Move(miter->getX(), miter->getY(), miter->getColour(), board.getSize()) );
			}
			if(verbose)
				log.println(message);
		}

		newNextToPlay = BLACK;
		if(lastMoveColour==BLACK) newNextToPlay = WHITE;

		mtdfSearch->restoreSettings();

		/******************************/
		/* Update and score new board */
		/******************************/

		// Changed strings must have their status updated before score estimation...
		if(verbose)
			log.println("Updating status after adding string enemies best line of play...");
		stringsToUpdate.clear();
		selectChangedStrings(stringsToUpdate, board, boardCopy);
		updateStringsStatus(boardCopy, newNextToPlay, NULL, &stringsToUpdate, getMaxStatusDepth()-1, &linesOfPlay);

		exchangeUrgentStrings(boardCopy, newNextToPlay);

		// Estimate score
		bestScore = estimateScore(boardCopy);
		if(verbose) {
			sprintf(buffer, "\nBest score for big move %s = %g", Move::xyToColourAndGoPosValue(iter->move).c_str(),
				bestScore);
			log.println(buffer);
		}

		if(timePerMove!=0 && timePerMoveRanOut) {
			iter->worth = bestScore;
			if(verbose) {
				sprintf(buffer, "Worth of big move %s = %g", Move::xyToColourAndGoPosValue(iter->move).c_str(), iter->worth);
				log.println(buffer);
				log.println("Time has run out...");
			}
			return;
		}

		// Find worst score...
		if(verbose) {
			if(notNextToPlay==BLACK)
				log.println("Finding worst score, playing as BLACK...");
			else
				log.println("Finding worst score, playing as WHITE...");
		}
		// Reset board copy
		boardCopy = board;
		// Now try with the enemy playing the move - check for legality since
		// these moves haven't been checked by a tree search or anything...
		// This is as if nextToPlay has passed...
		worstScore = 0.0f;
		if(Go::isLegal(boardCopy, iter->move.getX(), iter->move.getY(), notNextToPlay, Go::getUseSuperKo())) {
			// Add move to board copy
			boardCopy.addToBoard( Move(iter->move.getX(), iter->move.getY(), notNextToPlay, board.getSize()) );

			/**************************/
			/* Find big move sequence */
			/**************************/

			/* Run a minimax search with very low branching factor and Influence goal to find 
			 * the best sequence of exchanged big moves. */
			
			/* This search must be shallow and narrow because using the Influence goal without updating
			 * the status of the strings may give an unrealistic line of play. */
			
			// Setup the game tree search
			mtdfSearch->saveSettings();
			mtdfSearch->setupMTDFSearch(true, true, true, 0.35f, true, 0.35f, false, true, true);
			mtdfSearch->setMoveSuggester( new GeneralSuggester() );
			mtdfSearch->setGoal(Goal(new InfluenceGoal(iter->move.getX(), iter->move.getY())));
			if(verbose)
				log.println("Big sequence search initialised.");
			
			mtdfSearch->cleanTranspositionTable();
			if(timePerMove!=0)
				mtdfSearch->setTimeLimit(getTimeLeftPerMove()*0.9);
			else
				mtdfSearch->setTimeLimit(0);
			
			startSearchTimer(&tstart, &freq);

			// We play first since enemy has just played at the big move
			root.reset(boardCopy, nextToPlay);
			// Maximising colour must always be BLACK
			// Treat this search as if nextToPlay has passed for their first move meaning
			// after the enemy has played the big move there are maxBigSequenceDepth-2 moves left...
			mtdfSearch->iterativeDeepening(boardCopy, &root, maxBigSequenceDepth-2,
				mtdfSearch->getGoal().getRequiredMaximisingColour(boardCopy, nextToPlay), 1, firstGuess);
			
			searchTime = endSearchTimer(&tstart, &freq);
			updateSearchTimingStats(searchTime, BIG_SEQUENCE, mtdfSearch->getLastIDDepth());

			lastMoveColour = notNextToPlay;

			// Make sure a search actually happened here...
			if(mtdfSearch->getLastIDDepth()>0) {
				// Find the upto-date root in the transposition table
				mtdfSearch->lookup(root, newRoot);
				mtdfSearch->updateBestLineOfPlay(newRoot, boardCopy.getSize());
				const vector<Move>& lineOfPlay = mtdfSearch->getBestLineOfPlay();
				if(lineOfPlay.size()>0)
					lastMoveColour = lineOfPlay[lineOfPlay.size()-1].getColour();
				
				// Add all the moves from the line of play
				// Note the colour of the last move
				string message("Big move sequence: ");
				vector<Move>::const_iterator miter(lineOfPlay.begin());
				for(;miter!=lineOfPlay.end();miter++) {
					if(verbose) {
						message+=Move::xyToColourAndGoPosValue(*miter);
						message+=" ";
					}
					boardCopy.addToBoard( Move(miter->getX(), miter->getY(), miter->getColour(), board.getSize()) );
				}
				if(verbose)
					log.println(message);
			}
			else
				log.println("Big move sequence empty...");

			newNextToPlay = BLACK;
			if(lastMoveColour==BLACK) newNextToPlay = WHITE;

			mtdfSearch->restoreSettings();

			/******************************/
			/* Update and score new board */
			/******************************/

			// Changed strings must have their status updated before score estimation...
			if(verbose)
				log.println("Updating status after adding string enemies best line of play...");
			stringsToUpdate.clear();
			selectChangedStrings(stringsToUpdate, board, boardCopy);
			updateStringsStatus(boardCopy, newNextToPlay, NULL, &stringsToUpdate, getMaxStatusDepth()-1, &linesOfPlay);

			exchangeUrgentStrings(boardCopy, newNextToPlay);
		
			// Estimate score
			worstScore = estimateScore(boardCopy);
			if(verbose) {
				Move m(iter->move);
				m.setColour(notNextToPlay);
				sprintf(buffer, "\nWorst score for big move %s = %g", Move::xyToColourAndGoPosValue(m).c_str(),
					worstScore);
				log.println(buffer);
			}
		}
		else if(verbose) {
			log.println("Enemy big move illegal, skipping...");
			worstScore = bestScore;
		}

		// Worth is best score - worst score
		iter->worth = bestScore - worstScore;
		if(verbose) {
			sprintf(buffer, "Worth of big move %s = %g", Move::xyToColourAndGoPosValue(iter->move).c_str(), iter->worth);
			log.println(buffer);
		}
		if(timePerMove!=0 && timePerMoveRanOut) {
			if(verbose)
				log.println("Time has run out...");
			return;
		}
	}
}

/** @brief This function removes urgent strings from each side in turn until
  * all urgent strings are removed from the board. The order they are removed in 
  * is calculated according to their estimated worth. 
  * @param board The board to remove the urgent strings from. 
  * @param nextToPlay The colour next to play, so we remove the opposite colour first. */
void Melkor::exchangeUrgentStrings(BoardStruct& board, const int nextToPlay) const
{
	int currentColour = BLACK;
	if(nextToPlay==BLACK) currentColour = WHITE;

	/** @todo Make this have a better ordering - perhaps number of stones. */

	// Mark the currentColour string that's urgent as dead
	BoardStruct::stringsIterator siter;
	do {
		for(siter=board.getStrings().begin();siter!=board.getStrings().end();siter++) {
			if(siter->getColour()==currentColour && siter->getStatus()==URGENT) {
				siter->setStatus(DEAD);
				break;
			}
		}
		// Change colour
		if(currentColour==BLACK) currentColour = WHITE;
		else currentColour = BLACK;
	} while(siter!=board.getStrings().end());
}
		
/** @brief This function looks at the oldBoard and newBoard and stores an anchor point for each
  * GoString that has a different number of stones, liberties, or is a new GoString.
  * @param changedStrings A vector to store the anchor points of strings identified as changed.
  * @param oldBoard The old board to compare.
  * @param newBoard The new board to compare.
  */
void Melkor::selectChangedStrings(vector<Point<> >& changedStrings, const BoardStruct& oldBoard,
									 const BoardStruct& newBoard)
{
	const BoardStruct::stringTable& oldStrings = oldBoard.getStrings();
	const BoardStruct::stringTable& newStrings = newBoard.getStrings();

	// Look at each new string
	BoardStruct::constStringsIterator citer(newStrings.begin());
	for(;citer!=newStrings.end();citer++) {
		// Was this string in the old board?
		// Yes, if same number of stones as before skip it, otherwise add an anchor point
		// No, then add an anchor point
		bool changedString = true;
		BoardStruct::constStringsIterator oldCiter(oldStrings.begin());
		for(;oldCiter!=oldStrings.end();oldCiter++) {
			// '==' operator relies on the colour of the string and the key
			// Compare the stones and liberties of the string
			if(citer->getPoints() == oldCiter->getPoints() && citer->getLibertyPoints() == oldCiter->getLibertyPoints()) {
				changedString = false;
				break;
			}
		}
		if(changedString)
			changedStrings.push_back((*citer->getPoints().begin()));
	}
}

/** @brief If not already present in stringsToUpdate, add each enemy string, nsew 
  * of the move played to the list of strings to update
  * Note: These may not be added in selectChangedStrings() since that
  * works only on stone changes. If liberties are affected this won't
  * be noticed by selectChangedStrings().
  * @param stringsToUpdate The vector of string anchor points to add to.
  * @param board The board the function looks at to determine adjacent enemy strings.
  * @param x The x coordinate on the board of which this function finds adjacent enemy strings to.
  * @param y The y coordinate on the board of which this function finds adjacent enemy strings to.
  */
void Melkor::selectAdjacentEnemyStrings(vector<Point<> >& stringsToUpdate, const BoardStruct& board,
	const int x, const int y)
{
	int oppositeColour = BLACK;
	if(board.getPoint(x, y)==BLACK)
		oppositeColour = WHITE;
	// North
	if((y-1)>=0  && board.getPoint(x, y-1)==oppositeColour)
		addStringIfUnique(board, stringsToUpdate, x, y-1);
	// South
	if((y+1)<board.getSize() && board.getPoint(x, y+1)==oppositeColour)
		addStringIfUnique(board, stringsToUpdate, x, y+1);
	// West
	if((x-1)>=0  && board.getPoint(x-1, y)==oppositeColour)
		addStringIfUnique(board, stringsToUpdate, x-1, y);
	// East
	if((x+1)<board.getSize() && board.getPoint(x+1, y)==oppositeColour)
		addStringIfUnique(board, stringsToUpdate, x+1, y);
}

/** @brief This function should return a list of big moves for the current board
  * along with estimates of how much each move is worth from nextToPlay's point
  * of view.
  * @param bigMoves A vector to store the big move information.
  * @param urgentMoves A vector containing information about any identified urgent moves.
  * @param board The board position to find big moves for.
  * @param nextToPlay The colour to play next.
  * @param maxBigMoves The maximum number of big moves to find.
  * @see BigMoveInfo */
void Melkor::getBigMoves(vector<BigMoveInfo>& bigMoves, const vector<UrgentMoveInfo>* urgentMoves,
							const BoardStruct& board, const int nextToPlay,
							const int maxBigMoves)
{
	float bigMovePruneValue = 0.35f;
	BoardStruct boardCopy(board);
	bigMoves.clear();

	/** @todo Somehow filter the NN moves so we know which are actually big moves
	  * and which are for something else (shape, string tactics, etc...).
	  * Do include moves that are contact moves but also _big_ moves, i.e. 
	  * end game moves... */

	// Get big move suggestions from NN
	vector<Move> allMoves;
	getScoredMoves(allMoves, boardCopy, nextToPlay, *getBPN(), false, false, NULL, NULL);

	// Pick n best nn moves for big moves
	sort(allMoves.begin(), allMoves.end());         // This sorts lowest to highest
	int count = 0;
	for(int i=allMoves.size()-1;i>=0;i--) {
		// Use the NN to prune the moves
		Move& m = allMoves[i];
		// Disallow moves that attach to already DEAD strings 
		// These may be valid moves but don't fit our definition of big moves
		// e.g. they may be eye shape destroying moves, but they should
		// be identified as urgent moves in that case...
		// Also disallow moves that play on 2nd order liberties of DEAD strings
		int mx = m.getX();
		int my = m.getY();
		if(Go::isAdjacentToDeadStringOrLiberty(mx, my, boardCopy))
			continue;
		
		// Make sure this big move isn't already an identified urgent move
		// The first move in the bestLineOfPlay is the urgent one for nextToPlay
		bool unique = true;
		if(urgentMoves!=NULL) {
			for(int k=0;k<(*urgentMoves).size();k++) {
				if((*urgentMoves)[k].bestLineOfPlay.size()>0 && 
					mx==(*urgentMoves)[k].bestLineOfPlay[0].getX() &&
					my==(*urgentMoves)[k].bestLineOfPlay[0].getY()) {
					unique = false;
					break;
				}
			}
		}
		if(urgentMoves==NULL || unique) {
			bigMoves.push_back(BigMoveInfo(nextToPlay, m));
			count++;
			if(count >= maxBigMoves)
				break;
		}
	}

	/** @todo We need a method to determine which of our highest worth big
	  * big moves, reduces the effect of the enemies best big moves, the most.
	  * Just including the enemies big moves leads to passive play...
	  * But there will be our big moves that score well and reduce the effect of
	  * nearby enemy moves. The choice of big move should be a reflection of this.
	  * 
	  * Note: Using white's big moves yields defensive moves, this info could be
	  *       used if we're in the lead and want to play safe...
	  */

	// Estimate worth of big moves
	estimateWorthOfBigMoves(board, nextToPlay, bigMoves);
}

/** @brief This function calculates various search depth limits based
  * on how much time we have to calculate the next move, an estimate of
  * the number of searches we'll have to do and a speed constant for this
  * computer. 
  * @param timeAllowed The amount of time in seconds we have to find our next move. */
void Melkor::calculateSearchDepths(int timeAllowed, int numberOfBigMoves)
{
	// If no time limit, use a default of 4 seconds
	if(timeAllowed==0)
		timeAllowed = 4;

	/* How did I measure the time per n-ply search?
	 * See #ifdef TIME_SEARCHES - outputs number of runs and avg time(ms) 
	 * for each search type, at the end of each call to getBestGlobalMove(). */
	// speedConstant on my AMD 1.67GHz is ~0.0042ms
	// Note: Time required may increase exponentially...
	//
	// searchTime = (timeFor1Ply)^maxDepth
	// timeFor1Ply = searchTime^(1/maxDepth)
	// maxDepth = log searchTime / log timeFor1Ply
	const double ladder1Ply = 14;
	const double looseTactical1Ply = 6;
	const double status1Ply = 60;
	const double bigSequence1Ply = 20;

	/* See #ifdef TIME_SEARCHES for our method to find the average number of searches
	 * per move... */
	const int numberOfLadderSearches = 8;
	const int numberOfLooseTacticalSearches = 40;
	const int numberOfLDSearches = 16;
	const int numberOfBigSequenceSearches = 16;

	/** @todo Refine this calculation... Note that it assumes we will always run
	  * numberOfLadderSearches and numberOfLDSearches to the maximum depth in the 
	  * timeAllowed. In reality most of these searches won't reach the max depth. */
	// Calculate the amount of time per estimated search in seconds
	double timePerSearch = (double)timeAllowed / (numberOfLadderSearches + numberOfLooseTacticalSearches + numberOfLDSearches);
	double ladderSearchTime = timePerSearch * numberOfLadderSearches;
	double looseTacticalSearchTime = timePerSearch * numberOfLooseTacticalSearches;
	double statusSearchTime = timePerSearch * numberOfLDSearches;
	double bigSequenceSearchTime = timePerSearch * numberOfBigSequenceSearches;
	// Convert to speedConstant units
	ladderSearchTime = (ladderSearchTime*1000) / speedConstant;
	looseTacticalSearchTime = (looseTacticalSearchTime*1000) / speedConstant;
	statusSearchTime = (statusSearchTime*1000) / speedConstant;
	bigSequenceSearchTime = (bigSequenceSearchTime*1000) / speedConstant;

	maxLadderDepth = log(ladderSearchTime) / log(ladder1Ply);
	maxLooseTacticalDepth = log(looseTacticalSearchTime) / log(looseTactical1Ply);
	maxStatusDepth = log(statusSearchTime) / log(status1Ply);
	maxBigSequenceDepth = log(bigSequenceSearchTime) / log(bigSequence1Ply);

	// Round the depths down to an even value - this seems to give better
	// play due to a balanced exchange of moves (odd gives one move more for the opponent)
	if(maxLadderDepth%2 != 0) maxLadderDepth--;
	if(maxLooseTacticalDepth%2 != 0) maxLooseTacticalDepth--;
	if(maxStatusDepth%2 != 0) maxStatusDepth--;
	if(maxBigSequenceDepth%2 != 0) maxBigSequenceDepth--;

	LogWriter log;
	char buffer[2000];
	sprintf(buffer, "timeAllowed=%ds speedConstant=%gms maxStatusDepth=%d maxLooseTacticalDepth=%d maxLadderDepth=%d maxBigSequenceDepth=%d", 
		timeAllowed, speedConstant, maxStatusDepth, maxLooseTacticalDepth, maxLadderDepth, maxBigSequenceDepth);
	log.println(buffer);
	sprintf(buffer, "Assuming max depth numberOfLadderSearches=%d numberOfLooseTacticalSearches=%d numberOfLDSearches=%d numberOfBigSequenceSearches=%d will occur", 
		numberOfLadderSearches, numberOfLooseTacticalSearches, numberOfLDSearches, numberOfBigSequenceSearches);
	log.println(buffer);
}

/** @brief This function calculates a time per move figure by analysing the
  *  time system in use and how much time is left to us.
  * @board The current board before our move.
  * @return Time per move value in seconds. */
int Melkor::calculateTimePerMove(const int colour, const BoardStruct& board) const
{
	// Estimate the number of moves in a game
	int totalMoveEstimate = 40;
	// Allow 120 moves per side for 19x19
	if(board.getSize()==19)
		totalMoveEstimate = 120;
	else if(board.getSize()==9)
		totalMoveEstimate = 25;

	// Get our time left
	int mainTime, byoyomiTime, byoyomiStones;
	if(colour==BLACK) {
		mainTime = Go::getCurrentMainTimeBlack();
		byoyomiTime = Go::getCurrentByoyomiTimeBlack();
		byoyomiStones = Go::getCurrentByoyomiStonesBlack();
	}
	else {
		mainTime = Go::getCurrentMainTimeWhite();
		byoyomiTime = Go::getCurrentByoyomiTimeWhite();
		byoyomiStones = Go::getCurrentByoyomiStonesWhite();
	}

	// If no byo-yomi just divide the time
	if(Go::getTimeSystem()==Go::CANADIAN_BYO_YOMI) {
		// If the current move number is within 20% of the total move estimate
		// bump the total move estimate up by 20%
		if(Go::getCurrentMoveNumber() > ((float)totalMoveEstimate*0.8))
			totalMoveEstimate+=(float)totalMoveEstimate*0.2;

		// How long do we have per move?
		int totalTimeAvailable = mainTime + byoyomiTime;
		int preciseTimePerMove = totalTimeAvailable / totalMoveEstimate;

		// If we have more than 10*preciseTimePerMove left then 
		// allow 3* the preciseTimePerMove as the timePerMove
		if(10*preciseTimePerMove < totalTimeAvailable) {
			if(verbose) {
				LogWriter log;
				char buffer[1000];
				sprintf(buffer, "totalTimeAvailable=%ds preciseTimePerMove=%ds actualTimePerMove=%ds", 
					totalTimeAvailable, preciseTimePerMove, 3*preciseTimePerMove);
				log.println(buffer);
			}
			return 3*preciseTimePerMove;
		}
		else {
			if(verbose) {
				LogWriter log;
				char buffer[1000];
				sprintf(buffer, "totalTimeAvailable=%ds preciseTimePerMove=%ds actualTimePerMove(low on time)=%ds", 
					totalTimeAvailable, preciseTimePerMove, preciseTimePerMove*0.7);
				log.println(buffer);
			}
			// Allow 30% of the precise time for overheads
			return (float)preciseTimePerMove*0.7;
		}
	}
	else {
		// Failsafe
		return (mainTime + byoyomiTime) / totalMoveEstimate;
	}
}

/** @brief This function updates the continuous timing stats for each type of search.
  * @param searchTime How long the search took in milliseconds.
  * @param searchType What type of search was it, ladder, status, big sequence?
  * @param searchDepth How deep did the iterative part of the search go to? */
void Melkor::updateSearchTimingStats(const double searchTime, const SearchType searchType, const int searchDepth)
{
	// Only update stats on searches of greater than depth 1, otherwise
	// the overheads starting and stopping very quick searches - like ladder search
	// distort the figures quite badly.
	if(searchDepth<=1)
		return;
	// Convert searchTime from milliseconds to speedConstant units
	double newSearchTime = searchTime / speedConstant;
	// timeFor1Ply = searchTime^(1/maxDepth)
	double timeFor1Ply = pow(newSearchTime, (1/(double)searchDepth));
	if(searchType==LADDER) {
		totalLadder1PlyTime+=timeFor1Ply;
		totalLadderSearches++;
	}
	else if(searchType==LOOSE_TACTICAL) {
		totalLooseTactical1PlyTime+=timeFor1Ply;
		totalLooseTacticalSearches++;
	}
	else if(searchType==STATUS) {
		totalStatus1PlyTime+=timeFor1Ply;
		totalStatusSearches++;
	}
	else if(searchType==BIG_SEQUENCE) {
		totalBigSequence1PlyTime+=timeFor1Ply;
		totalBigSequenceSearches++;
	}
}

#ifdef _WIN32 
/** @brief This function checks if the time per move limit has been breached using
  * the member variable startTime. The variable lastTime is used for pre-emptive
  * time checking - it should measure the last local loop, which starts from localStart.
  * In addition the member variable timePerMoveRanOut will be set to true if the time has run out.
  * @param localStart A pointer to a time structure which marks the local time loop start. This
  * will be updated at the end of this function.
  * @param lastTime The duration of the last local loop in seconds. 
  * @return True if the time limit has been breached, false otherwise. */
bool Melkor::checkTimePerMove(LARGE_INTEGER *localStart, double &lastTime) {
	if(timePerMove!=0) {
		LARGE_INTEGER end;
		timerEnd(&end);
		// timerValue returns in seconds
		double t = timerValue(startTime, end, timerFreq);
		lastTime = timerValue(*localStart, end, timerFreq);
		// Do pre-emptive time checking as well as absolute using localStart
		if(t>=timePerMove || (t+lastTime)>=timePerMove) {
			timePerMoveRanOut = true;
			return true;
		}
		timerStart(localStart, &timerFreq);
	}
	return false;
}

void Melkor::startSearchTimer(LARGE_INTEGER* tstart, LARGE_INTEGER* freq)
{
#ifdef TIME_SEARCHES
	timerStart(tstart, freq);
#endif
}

double Melkor::endSearchTimer(LARGE_INTEGER* tstart, LARGE_INTEGER* freq)
{
	double retVal = 0.0;
#ifdef TIME_SEARCHES
	LogWriter log;
	char buffer[500];
	LARGE_INTEGER tend;
	timerEnd(&tend);
	// work out time in milliseconds
	retVal = timerValue(*tstart, tend, *freq)*1000;
	sprintf(buffer, "search time = %gms", retVal);
	log.println(buffer);
#endif
	return retVal;
}

#else
bool Melkor::checkTimePerMove(clock_t *localStart, double &lastTime) {
	if(timePerMove!=0) {
		clock_t end = clock();
		// difftime is in seconds
		double t = (double)(end - startTime) / CLOCKS_PER_SEC;
		lastTime = (double)(end - *localStart) / CLOCKS_PER_SEC;
		// Do pre-emptive time checking as well as absolute using localStart
		if(t>=timePerMove || (t+lastTime)>=timePerMove) {
			timePerMoveRanOut = true;
			return true;
		}
		time(localStart);
	}
	return false;
}
#endif

/** @brief This function adds the string specified at x,y in board to strings
  * if it does not already occur in strings. It uses the GoString == operator
  * for comparisons. 
  * @param board The board to consider.
  * @param strings A vector of anchor points for strings in board.
  * @param x The x coordinate of the string to add to strings.
  * @param y The y coordinate of the string to add to strings. */
void Melkor::addStringIfUnique(const BoardStruct& board, vector<Point<> >& strings, const int x, const int y) const
{
	// Is board.getString(x, y) in strings?
	bool stringFound = false;
	vector<Point<> >::const_iterator citer(strings.begin());
	for(;citer!=strings.end();citer++) {
		if(board.getStringConst(x, y)->containsPoint(citer->x, citer->y)) {
			stringFound = true;
			break;
		}
	}
	if(!stringFound)
		strings.push_back((*board.getStringConst(x, y)->getPoints().begin()));
}

void Melkor::startLog()
{
	useLog = true;
}

void Melkor::stopLog()
{
	useLog = false;
}

void Melkor::getBestMoveWithoutAlphaBeta(Move &m, BoardStruct &b, int nextToPlay)
{
	// best moves list from BPN
	vector<Move> bestMoves;
	getScoredMoves(bestMoves, b, nextToPlay, *getBPN());
	if (bestMoves.size()<=0) {
//		LogWriter::printerr("No suggestions, passing.\n", myName);
		Move::getPassMove(m);
	}
	else {
		// sort the list in order of score
		sort(bestMoves.begin(), bestMoves.end());
		//m = new Move((*bestMoves)[bestMoves->size()-1]);
		m = bestMoves[bestMoves.size()-1];
		// check move score threshold
		if(!noPasses && mtdfSearch->getUseMoveQualityThreshold() && mtdfSearch->getPruneValue()<1 && 
			m.getNNScore()<mtdfSearch->getPruneValue()) {
//			LogWriter::printerr("No good moves, passing.\n", myName);
			Move::getPassMove(m);
		}
	}

	if(debuggo || verbose) {
		LogWriter::printerr("\n");
		string message = "BPN Move: ";
		message+=Move::xyToGoPosValue(m.getX(), m.getY(),
			b.getSize());
		LogWriter::printerr(message+"\n", myName);
		message = "Score   : ";
		char buffer[50];
		sprintf(buffer, "%*g", 7, m.getNNScore());
		message+=buffer;
		LogWriter::printerr(message+"\n", myName);
	}
}

/** Use the MTD(f) algorithm and a specified neural network to find 
and return the best move for the given board postition. 
@param m A move reference to put the answer into.
@param b The current board position.
@param nextToPlay The colour to move next.
@return The best move the routine can find. */
void Melkor::getBestMoveWithAlphaBeta(Move &m, BoardStruct &b, int nextToPlay)
{
	// get move and put in m
	float abScore = mtdfSearch->suggestMove(m, b, nextToPlay);

	// Failsafe to stop us getting passes by mistake when there
	// are still legal moves left to play
	if(noPasses && m.getX()==-1 && m.getY()==-1) {
		// See if there are any legal moves available
		vector<Move> allMoves;
		getScoredMoves(allMoves, b, nextToPlay, *getBPN(), false, false, NULL, NULL);
		if(allMoves.size()==0)
			Move::getPassMove(m);
		else
			m = allMoves[0];
	}

	if(debuggo || verbose) {
		string message = "BPN Move: ";
		char buffer[200];
		if(m.getX()==-1 && m.getY()==-1)
			message+="Pass";
		else
			message+=Move::xyToGoPosValue(m.getX(), m.getY(),
				b.getSize());
		LogWriter::printerr(message+"\n", myName);
		message = "NNScore   : ";
		sprintf(buffer, "%*g", 7, m.getNNScore());
		message+=buffer;
		LogWriter::printerr(message+"\n", myName);
		message = "Alpha-Beta score : ";
		sprintf(buffer, "%*g", 7, abScore);
		message+=buffer;
		LogWriter::printerr(message+"\n", myName);
	}
}

void Melkor::getScoredMoves(vector<Move> &m, BoardStruct &b, int colour, BPNGoAdapterInterface& bpn,
		bool useOldScores /*=false*/, bool useIllegalMoveMemory /*=false*/, vector<Move>* changedPoints /*=NULL*/, 
		const Rect* bounds /*=NULL*/, bool useNeuralNetwork /*=true*/, bool printDebuggingInfo /*=false*/, 
		const int maximisingColour /*=EMPTY*/)
{
	if(printDebuggingInfo) LogWriter::printerr("1");

	vector<Move> legalMoves;


	if(changedPoints!=NULL)
		Go::getLegalMoves(legalMoves, colour, b, Go::getUseSuperKo(), bounds, true, changedPoints);
	else
		Go::getLegalMoves(legalMoves, colour, b, Go::getUseSuperKo(), bounds, useIllegalMoveMemory);

	if(printDebuggingInfo && legalMoves.size()==0)	{
		LogWriter::printerr("getLegalMoves() returned size 0\n", "Go"); 
		LogWriter::printerr("Therefore apparently no legal moves on this board:\n", "Go");
		BoardStruct tempB(b);
		tempB.printAsciiBoard(true);
	}

	Move* move;

	if(printDebuggingInfo) LogWriter::printerr("4");

	if(!useNeuralNetwork) {
		m.clear();
		for(int i=0;i<legalMoves.size();i++) {
			move = &legalMoves[i];
			// set score to 1 so any random bias choice based on NNScore will cause all moves to be equal
			move->setNNScore(1.0f);
			m.push_back(*move);
		}
		return;
	}
	
	vector<Matrix<float> > temp(bpn.getBPN().getNumberOfLayers());
	Matrix<float> p;
	Matrix<float>* answers;
	float s = 0.0;

	// if we're reusing the old scores...
	if(useOldScores) {
		// remove all moves in m that are no longer legal
		// i.e. not in legalMoves
		bool mFound;
		for(int i=0;i<m.size();i++)	{
			// see if m[i] is still legal
			mFound = false;
			for(int j=0;j<legalMoves.size();j++) {
				// move onto next move in m if we have found this one still legal
				if(m[i]==legalMoves[j])  {
					// mark the legal move as being in m already
					legalMoves[j].setNNScore(-1000.0f);
					mFound = true;
					break;
				}
			} // end for j

			// if we did not find m[i] amongst the legal moves remove it from m
			if(!mFound) {
				m.erase(m.begin()+i);
				// take one off i because we do i++ at every loop, so when we erase we might skip one move
				// i.e. i=0, move[i] is illegal so erase it, then the new move[0] doesn't get checked.
				i--;
			}

		} // end for i

		// loop through the legalMoves checking if we have found them in m already
		// if not then add them to m and set NNScore to -1000.0f to indicate it needs to be calculated
		for(i=0;i<legalMoves.size();i++) {
			if(legalMoves[i].getNNScore()!=-1000.0f) {
				m.push_back(legalMoves[i]);
				m[m.size()-1].setNNScore(-1000.0f);
			}
		}

		// find the area we have to recalculate
		// based on size of network and points that have changed on the board
		int width = bpn.getPatternWidth();
		int height = bpn.getPatternHeight();
		int hwidth = width/2;
		int hheight = height/2;

		// initially set the bounds to be around the first changed point
		Rect localBounds(-1, -1, -1, -1);
		if(changedPoints!=NULL) {
			localBounds.top = (*changedPoints)[0].getY()-hheight;
			localBounds.bottom = (*changedPoints)[0].getY()+hheight;
			localBounds.left = (*changedPoints)[0].getX()-hwidth;
			localBounds.right = (*changedPoints)[0].getX()+hwidth;
			
			// if there are more points, expand the bounds
			int newtop, newbottom, newleft, newright;
			for(i=1;i<changedPoints->size();i++) {
				newtop = (*changedPoints)[i].getY()-hheight;
				newbottom = (*changedPoints)[i].getY()+hheight;
				newleft = (*changedPoints)[i].getX()-hwidth;
				newright = (*changedPoints)[i].getX()+hwidth;
				if(newtop<localBounds.top)
					localBounds.top = newtop;
				if(newbottom<localBounds.bottom)
					localBounds.bottom = newbottom;
				if(newleft<localBounds.left)
					localBounds.left = newleft;
				if(newright<localBounds.right)
					localBounds.right = newright;
			}
		}
		// if there are no changed points to make a boundary around
		// use the boundary spec if supplied
		else if(bounds!=NULL)
			localBounds = *bounds;

		// step through m and update moves within the bounds and with NNScore == -1000.0f
		for(i=0;i<m.size();i++) {
			move = &m[i];
			if(move->getNNScore()==-1000.0f ||
			   (move->getX()<=localBounds.right && move->getX()>=localBounds.left &&
			   move->getY()<=localBounds.bottom && move->getY()>=localBounds.top)) {
				bpn.getInput(move->getX(), move->getY(), b, p, colour);
				bpn.getBPN().getAnswer(p);
				answers = &(bpn.getBPN().getOutputs()[bpn.getBPN().getOutputs().size()-1]);
				move->setNNScore(answers->getValue(0, 0));
			}
		}
	}
	// if we're doing all scoring from scratch...
	else {
		if(printDebuggingInfo) LogWriter::printerr("5");
		m.clear();

		for(int i=0;i<legalMoves.size();i++) {
			move = &legalMoves[i];
			bpn.getInput(move->getX(), move->getY(), b, p, colour);
			bpn.getBPN().getAnswer(p);
			answers = &(bpn.getBPN().getOutputs()[bpn.getBPN().getOutputs().size()-1]);
			move->setNNScore(answers->getValue(0, 0));
			m.push_back(*move);
		}
		if(printDebuggingInfo) LogWriter::printerr("6");
	}
}

float Melkor::getScoredMove(int j, int i, const BoardStruct &b, int colour, BPNGoAdapterInterface& bpn,
							vector<Matrix<float> >* outputs /*NULL*/)
{
	if(outputs!=NULL && outputs->size()!=bpn.getBPN().getNumberOfLayers())
		outputs->resize(bpn.getBPN().getNumberOfLayers());

	Matrix<float> p;
	bpn.getInput(j, i, b, p, colour);
	Matrix<float>* answers;
	if(outputs==NULL) {
		bpn.getBPN().getAnswer(p);
		answers = &(bpn.getBPN().getOutputs()[bpn.getBPN().getOutputs().size()-1]);
	}
	else {
		bpn.getBPN().getAnswer(p);
		answers = &(bpn.getBPN().getOutputs()[bpn.getBPN().getOutputs().size()-1]);
		*outputs = bpn.getBPN().getOutputs();
	}
 
	return answers->getValue(0, 0);
}

/** @todo Move this to Go or Misc. */
void Melkor::reverseColours(const BoardStruct &in, BoardStruct &out)
{
	int h = in.getHeight();
	int w = in.getWidth();
	int c;
	for(int i=0;i<h;i++) {
		for(int j=0;j<w;j++) {
			c = in.getPoint(j, i);
			if(c==BLACK)
				out.setPoint(j, i, WHITE);
			else if(c==WHITE)
				out.setPoint(j, i, BLACK);
			else
				out.setPoint(j, i, c); 	
		} // end for j
	} // end for i
}

/** @brief Use the neural network to decide if we should pass.
 *
 * @param board The board to find legal moves for.
 * @param nextToPlay The colour next to play.
 * @param bounds A specification of the area on the board to look at.
 * @param passValue The threshold value to test the neural network scores against.
 * @param maximisingColour The colour that's is trying to maximise their score. Typically
 * this will be the computers colour.
 * @return True if no legal moves within the specified bounds score equal or higher to the passValue,
 *         false otherwise.
 */
bool Melkor::NNPassJudge(const BoardStruct& board, int nextToPlay, const Rect& bounds, float passValue,
							BPNGoAdapterInterface& bpn, const int maximisingColour /*=EMPTY*/)
{
	// Find all legal moves within 'bounds' on 'board'
	vector<Move> legalMoves;
	BoardStruct tempBoard(board);
	Go::getLegalMoves(legalMoves, nextToPlay, tempBoard, Go::getUseSuperKo(), &bounds, false, NULL, maximisingColour);

	// Score each and if the score >= passValue return false
	Matrix<float> p;
	Matrix<float>* answers;
	float s = 0.0;

	vector<Move>::const_iterator citer(legalMoves.begin());
	for(;citer!=legalMoves.end();citer++) {
		bpn.getInput(citer->getX(), citer->getY(), board, p, nextToPlay);
		// NOTE: This line causes a 4k memory use increase in DEBUG
		// when it should not change the memory usage total for before/after
		bpn.getBPN().getAnswer(p);
		answers = &(bpn.getBPN().getOutputs()[bpn.getBPN().getOutputs().size()-1]);
		if(answers->getValue(0, 0) >= passValue)
			return false;
	}

	// If we have no legal moves available then we must pass
	// or if we run out of moves and haven't returned yet, return true
	return true;
}

/***********************************************/
/* Functions that modify the internal SGF tree */
/***********************************************/

void Melkor::initialiseSGFTree(int boardSize, int handicap /*=0*/) 
{
	SGFGameInfo info(sgf.getGameInfo());
	info.size = boardSize;
	info.application = APPLICATION_NAME;
	info.application+=":Version ";
	info.application+=MAJOR_VERSION;
	info.application+=".";
	info.application+=MINOR_VERSION;
	// Set the date to today initially
	char tmpBuf[128];
	time_t ltime;
	struct tm *today;
	// Set the local time zone from environment variable or OS setting
	_tzset();
	time(&ltime);                              // Get UNIX-style time
	today = localtime(&ltime);                 // Convert to a structure with individual fields
	strftime(tmpBuf, 128, "%Y-%m-%d", today);  // Format a string based on today
	info.date = tmpBuf;
	if(handicap!=0)
		info.handicap = handicap;
	sgf.init(info);
}

void Melkor::setBoardSize(int bs)
{
	gtp->gtp_internal_set_boardsize(bs);
	resizeBoard(bs);

	// always reset the MTDF stuff since we must assume a boardsize change indicates a new game here.
	// Anyway the previous contents of the TTable in MTDFSearch will probably be for a
	// different board size...
	mtdfSearch->init();

	// New sgf so reset the whole sgf tree
	sgfModified = true;
	initialiseSGFTree(bs);

	noPasses = false;          // reset this in case we've had a kgs-genmove_cleanup command
}

void Melkor::setBlackName(string name)
{
	// Update the SGF details
	sgfModified = true;
	SGFGameInfo info(sgf.getGameInfo());
	info.blackPlayer = name;
	sgf.setGameInfo(info);
}
void Melkor::setWhiteName(string name)
{
	// Update the SGF details
	sgfModified = true;
	SGFGameInfo info(sgf.getGameInfo());
	info.whitePlayer = name;
	sgf.setGameInfo(info);
}
void Melkor::setSGFResultString(string result)
{
	// Update the SGF details
	sgfModified = true;
	SGFGameInfo info(sgf.getGameInfo());
	info.result = result;
	sgf.setGameInfo(info);
}

void Melkor::setKomi(float komi)
{
	Go::setKomi(komi);

	// Update the SGF details
	sgfModified = true;
	SGFGameInfo info(sgf.getGameInfo());
	info.komi = komi;
	sgf.setGameInfo(info);
}

bool Melkor::setHandicap(int handicap)
{

	getBoard().clear();
	if(!Go::doFixedHandicap(getBoard()))
		return false;
	Go::setHandicap(handicap);

	sgfModified = true;
	initialiseSGFTree(getBoard().getSize(), handicap);
	return true;
}

/** @brief Set the time settings for this game. */
void Melkor::setTimeSettings(int mainTime, int byoyomiTime, int byoyomiStones)
{
	// Update the SGF details
	sgfModified = true;
	SGFGameInfo info(sgf.getGameInfo());
	
	// Set main time
	info.time = mainTime;

	// Set overtime
	if(byoyomiTime>0) {
		char buffer[200];
		buffer[0] = '\0';
		if(Go::getTimeSystem()==Go::CANADIAN_BYO_YOMI)
			sprintf(buffer, "%d/%d Canadian", byoyomiStones, byoyomiTime);
		else
			sprintf(buffer, "%d/%d", byoyomiStones, byoyomiTime);
		info.overtime = buffer;
	}

	sgf.setGameInfo(info);

	Go::setMainTime(mainTime);
	Go::setByoyomiTime(byoyomiTime);
	Go::setByoyomiStones(byoyomiStones);
}

/** @brief Set the time left for the computer in this game. */
void Melkor::setCurrentTimeSettings(int colour, int mainTime, int byoyomiTime, int byoyomiStones)
{
	if(colour==BLACK) {
		Go::setCurrentMainTimeBlack(mainTime);
		Go::setCurrentByoyomiTimeBlack(byoyomiTime);
		Go::setCurrentByoyomiStonesBlack(byoyomiStones);
	}
	else {
		Go::setCurrentMainTimeWhite(mainTime);
		Go::setCurrentByoyomiTimeWhite(byoyomiTime);
		Go::setCurrentByoyomiStonesWhite(byoyomiStones);
	}
}
