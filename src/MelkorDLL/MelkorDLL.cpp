// MelkorDLL.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "MelkorDLL.h"
#include <wtypes.h>      // For BSTR declaration
#include <oleauto.h>

// Seed of 0 lets Melkor take a seed from the current time
unsigned long seed = 0;
const string defaultNetwork("newBPN3-9x9.bpn");
const string defaultGSSetup("");
// Declare Morgoth as a global variable
Melkor *melkor = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

/** @brief This function must be called to initialise the DLL before use.
  * @param errorString A returnable parameter containing an error message if one occurs.
  * @param maxChar The maximum length of errorString.
  * @return Returns 1 on success, 0 otherwise. */
int _stdcall initDll(char* errorString, int maxChar)
{
	if(melkor)
		delete melkor;
	melkor = new Melkor(seed, &defaultNetwork, false);
	
	// Detect errors
	if(melkor->error) {
		string message("Error in Melkor object initialisation:\n");
		message+=melkor->errorText;
		// Must add a null character here since strncpy doesn't!!!
		message+='\0';
		strncpy(errorString, message.c_str(), maxChar);
		return 0;
	}

	return 1;
}

/** @brief This function must be called before the calling program finishes.
  * @return Always returns 1. */
int _stdcall cleanupDll()
{
	if(melkor)
		delete melkor;
	return 1;
}

// Method to convert a Multibyte String to BSTR string
// This is a private method
BSTR getBSTRString(const char *a_lpszString) {
     BSTR     r_bstrString;
     int          v_iLength;
     wchar_t *v_wcString;

//     try {
          v_iLength = strlen(a_lpszString);
          v_wcString = new wchar_t[v_iLength];
          mbstowcs(v_wcString,a_lpszString,v_iLength+1);
          r_bstrString = SysAllocString(v_wcString);
          return r_bstrString;
//     } catch(CException a_GeneralException) {
          //setErrorMsg("Error in converting Bstr to MB : ",&a_GeneralException);
//          r_bstrString = NULL;
//          return r_bstrString;
//     }
}

/***********************************************/
/* My exported DLL Melkor interface functions */
/***********************************************/

// NOTE: Functions that return strings must return BSTR types
//       Use getBSTRString(const char*) to convert from char*

BSTR _stdcall DllGetName()
{
	// BSTR conversion
	return getBSTRString(melkor->getBPN()->getBPN().getFilename().c_str());
}

// Assume the pointer inBoard has enough memory allocated to
// be filled with the current board
int _stdcall DllGetBoard(int *inBoard, int boardSize)
{
	int size = melkor->getBoard().getSize();

	if(boardSize != size)
		return 0;

	for(int y=0;y<size;y++) {
		for(int x=0;x<size;x++)
			inBoard[y*size+x] = melkor->getBoard().getPoint(x, y);
	}

	return 1;
}

void _stdcall DllSetBoardSize(int bs)
{
	melkor->setBoardSize(bs);
}

int _stdcall DllGetBoardSize()
{
	return melkor->getBoard().getSize();
}

/** @brief Plays the move specified on the internal board - if it is legal.
  * @return 0 if the move is illegal, 1 otherwise */
int _stdcall DllPlayMove(int x, int y, int colour)
{
	int size = melkor->getBoard().getSize();
	string move;
	
	// Check for a pass so that gtp function will handle it correctly
	if(x==-1 && y==-1)
		move = "pass";
	else
	{
		move = Move::xyToGoPosValue(x, y, size);
		
		// If the move is illegal return 0
		if(!Go::isLegal(melkor->getBoard(), Move::GoPosToX(move), Move::GoPosToY(move, size), colour, Go::getUseSuperKo()))
			return 0;
	}
	
	// Use the GTP function for consistency
	char buffer[100];
	sprintf(buffer, "%s", move.c_str());
	if(colour == BLACK)
		melkor->gtp->gtp_playblack(buffer, 0);
	else if (colour == WHITE)
		melkor->gtp->gtp_playwhite(buffer, 0);
	else
		return 0;   // invalid colour

	/* Test code */
//	Move m(x, y, colour);
//	m.setBoardSize(size);
//	Go::addToMoveList(m, morgoth->go->getBoard());
	// DLL importers don't like the following line...
//	morgoth->go->getBoard().addToBoard(m);
	/* End test code */

	return 1;
}

/** @brief Get Melkor to generate a move for a given colour and
  * return the coordinates in the arguments x and y 
  * @return Returns 0 on error, 1 otherwise. */
int _stdcall DllGenerateMove(int *x, int *y, int colour)
{
	// No args I can think of for genmove
	string args("");
	char buffer[100];
	sprintf(buffer, "%s", args.c_str());

	if(colour == BLACK)
		melkor->gtp->gtp_genmove_black(buffer, 0);
	else if(colour == WHITE)
		melkor->gtp->gtp_genmove_white(buffer, 0);
	else
		return 0;

	*x = melkor->gtp->lastMove.getX();
	*y = melkor->gtp->lastMove.getY();

	/* Test code */
//	*x = 14;
//	*y = 14;
	/* End test code */

	return 1;
}

/** @brief Undo the last move. Note: This function should only be called when in
  * the middle of a game - editing an SGF or stepping through may invalidate the board.
  * @return 0 if unsuccessful, 1 otherwise */
int _stdcall DllUndo()
{
	melkor->gtp->gtp_undo(NULL, 0);
	return 1;
}

/** @brief Return NN scores for each legal move available.
  * @param inBoard A pointer to store the NN scores, return -1 for illegal moves. 
  * @param boardSize The current board size. 
  * @param colour The point of view to score from.
  * @return Return 0 on failure, 1 otherwise. */
int _stdcall DllGetScoredPoints(float *inBoard, int boardSize, int colour)
{
	int size = melkor->getBoard().getSize();

	if(boardSize != size)
		return 0;

	vector<Move> moves;
	BoardStruct tempBoard(melkor->getBoard());
	melkor->getScoredMoves(moves, tempBoard, colour, *melkor->getBPN(), false, false, NULL, NULL);

//	float* ptr = &inBoard;

	// Clear the inBoard array
	for(int y=0;y<size;y++) {
		for(int x=0;x<size;x++)
			inBoard[y*size+x] = -1.0f;
	}

	// Save NN scores
	vector<Move>::const_iterator citer(moves.begin());
	for(;citer!=moves.end();citer++)
		inBoard[citer->getY()*size+citer->getX()] = citer->getNNScore();

	return 1;
}

/** @brief Save the current SGF internal tree.
  * @param filename The filename to save to.
  * @return Return value is always GTP_OK at the moment. */
int _stdcall DllSaveSGF(const char* filename)
{
	char buffer[1000];
	sprintf(buffer, "%s", filename);
	return melkor->gtp->gtp_save_sgf(buffer, 0);
}

/** @brief Load the specified sgf file.
  * @param filename The filename to load.
  * @return Return value is always GTP_OK at the moment. */
int _stdcall DllLoadSGF(const char* filename)
{
	char buffer[1000];
	// Add a '0' on the end of the filename to get it to load to move 0
	// i.e. the start of the file. Default load is to the end of the file.
	sprintf(buffer, "%s 0", filename);
	return melkor->gtp->gtp_loadsgf(buffer, 0);
}

/** @brief Step backward through the current sgf file.
  * @param step The number of moves to step back.
  * @return Return value is always GTP_OK at the moment. */
int _stdcall DllBackSGF(int step)
{
	char buffer[1000];
	buffer[0] = '\0';
	if(step>0)
		sprintf(buffer, "%d", step);
	return melkor->gtp->gtp_backsgf(buffer, 0);
}

/** @brief Step forward through the current sgf file.
  * @param step The number of moves to step forward.
  * @return Return value is always GTP_OK at the moment. */
int _stdcall DllNextSGF(int step)
{
	char buffer[1000];
	buffer[0] = '\0';
	if(step>0)
		sprintf(buffer, "%d", step);
	return melkor->gtp->gtp_nextsgf(buffer, 0);
}

/** @brief Load the specified neural network file.
  * @param filename The filename to load.
  * @return Return 0 on failure, 1 otherwise. */
int _stdcall DllLoadNeuralNetwork(const char* filename)
{
	string f(filename);
	if(!melkor->loadBPN(f))
		return 0;
	return 1;
}

/** @brief Return the position of the last move played. 
  * @return Always returns 1. */
int _stdcall DllGetLastMove(int* x, int* y)
{
	*x = melkor->getBoard().getLastX();
	*y = melkor->getBoard().getLastY();
	return 1;
}

////////////////////////
// Settings Functions //
////////////////////////

/** @brief Return the current version of Melkor as a string.
  * @param versionString A pointer to a string to fill with the version.
  * @param maxChar The maximum length of versionString.
  * @return Always 1. */
int _stdcall DllGetVersionString(char* versionString, int maxChar)
{
	// Must add a null character here since strncpy doesn't!!!
	string version(Melkor::getVersionString());
	version+='\0';
	strncpy(versionString, version.c_str(), maxChar);
	return 1;
}

/** @brief Set whether DLL output should be verbose or not.
  * @param verbose The new level of verbosity.
  * @return Always returns 1. */
int _stdcall DllSetVerbose(bool verbose)
{
	melkor->verbose = verbose;
	return 1;
}

/** @brief Return the maximum status search depth. 
  * @return The maximum status search depth. */
int _stdcall DllGetStatusDepth()
{
	return melkor->getMaxStatusDepth();
}

/** @brief Set the maximum status search depth.
  * @param depth The new maximum status search depth.
  * @return Always returns 1. */
int _stdcall DllSetStatusDepth(int depth)
{
	melkor->setMaxStatusDepth(depth);
	return 1;
}

/** @brief Find out if we're using a dynamic search depth. 
  * @return Non-zero if we are using a dynamic search depth, zero otherwise. */
int _stdcall DllGetUseDynamicSearchDepth()
{
	return (int)melkor->useDynamicSearchDepth;
}

/** @brief Set whether we should use a dynamic search depth or not.
  * @param useDynamicSearchDepth The new use status.
  * @return Always returns 1. */
int _stdcall DllSetUseDynamicSearchDepth(bool useDynamicSearchDepth)
{
	melkor->useDynamicSearchDepth = useDynamicSearchDepth;
	return 1;
}

/** @brief Find out if we're using the global move function or not.
  * @return True if we are using the global move function, false otherwise. */
int _stdcall DllGetUseGlobalMoveFunction()
{
	return (int)melkor->useGlobalMoveFunction;
}

/** @brief Set whether we should use the global move function or not.
  * @param useGlobalMoveFunction The new global move function use status.
  * @return Always returns 1. */
int _stdcall DllSetUseGlobalMoveFunction(bool useGlobalMoveFunction)
{
	melkor->useGlobalMoveFunction = useGlobalMoveFunction;
	return 1;
}

/** @brief Set the komi.
  * @param komi The new komi value. 
  * @return Always returns 1. */
int _stdcall DllSetKomi(float komi)
{
	melkor->setKomi(komi);
	return 1;
}

/** @brief Set the handicap.
  * @param komi The new handicap value. 
  * @return Always returns 1. */
int _stdcall DllSetHandicap(int handicap)
{
	melkor->setHandicap(handicap);
	return 1;
}

/** @brief Get the time allowed per move in seconds.
  * @return The time allowed per move in seconds. */
int _stdcall DllGetTimePerMove()
{
	return melkor->getTimePerMove();
}

/** @brief Set the time allowed per move, in seconds.
  * @param timePerMove The time allowed per move in seconds.
  * @return Always returns 1. */
int _stdcall DllSetTimePerMove(int timePerMove)
{
	melkor->setTimePerMove((double)timePerMove);
	return 1;
}

/** @brief Set the time settings in seconds.
  * @param mainTime The main time period allowed in seconds.
  * @param byoyomiTime The byo-yomi time period in seconds.
  * @param byoyomiStones The number of stones to play in a byo-yomi period.
  * @return Always returns 1. */
int _stdcall DllSetTimeSettings(int mainTime, int byoyomiTime, int byoyomiStones)
{
	melkor->setTimeSettings(mainTime, byoyomiTime, byoyomiStones);
	return 1;
}

/** @brief Set the current time settings left for black, in seconds.
  * @param mainTime The main time period left in seconds.
  * @param byoyomiTime The byo-yomi time period left in seconds.
  * @param byoyomiStones The number of stones left to play in this byo-yomi period.
  * @return Always returns 1. */
int _stdcall DllSetCurrentTimeSettingsBlack(int mainTime, int byoyomiTime, int byoyomiStones)
{
	melkor->setCurrentTimeSettings(BLACK, mainTime, byoyomiTime, byoyomiStones);
	return 1;
}

/** @brief Set the current time settings left for white, in seconds.
  * @param mainTime The main time period left in seconds.
  * @param byoyomiTime The byo-yomi time period left in seconds.
  * @param byoyomiStones The number of stones left to play in this byo-yomi period.
  * @return Always returns 1. */
int _stdcall DllSetCurrentTimeSettingsWhite(int mainTime, int byoyomiTime, int byoyomiStones)
{
	melkor->setCurrentTimeSettings(WHITE, mainTime, byoyomiTime, byoyomiStones);
	return 1;
}

/** @brief Tell Melkor it's playing a timed game and should use the information
  * in the Go static object to allocate time appropriately.
  * @param playTimedGame An integer to indicate whether to play a timed game or not,
  * a value of 1 indicates play a timed game, any other value means it's a non-timed game.
  * @return Always returns 1. */
int _stdcall DllPlayTimedGame(int playTimedGame)
{
	if(playTimedGame==1)
		melkor->setPlayTimedGame(true);
	else
		melkor->setPlayTimedGame(false);
	return 1;
}

// SGF Information

/** @brief Set the black player's name in the internal SGF tree. 
  * @param name The black player's name.
  * @return Always returns 1. */
int _stdcall DllSetBlackName(char* name)
{
	melkor->setBlackName(name);
	return 1;
}

/** @brief Set the white player's name in the internal SGF tree. 
  * @param name The white player's name.
  * @return Always returns 1. */
int _stdcall DllSetWhiteName(char* name)
{
	melkor->setWhiteName(name);
	return 1;
}

/** @brief Set the result string in the internal SGF tree. 
  * @param result The result string.
  * @return Always returns 1. */
int _stdcall DllSetSGFResultString(char* result)
{
	melkor->setSGFResultString(result);
	return 1;
}

///////////////////////
// Scoring functions //
///////////////////////

/** @brief Get the number of black stones that have been captured for the latest board state. */
int _stdcall DllGetCapturedBlackStones()
{
	// Look at the last board state in the board list
	return Go::getBoardList().back().getCapturedBlackStones();
}

/** @brief Get the number of white stones that have been captured for the latest board state. */
int _stdcall DllGetCapturedWhiteStones()
{
	// Look at the last board state in the board list
	return Go::getBoardList().back().getCapturedWhiteStones();
}

/** @brief Return a board sized integer array with values !=0 marking
  * the position of black territory. 
  * @param inBoard A pointer to a pre-allocated integer array the same
  * size (but single-dimension) as the board being played on.
  * @param boardSize The size of the board, i.e. 19 (19x19). 
  * @return Always 1. */
int _stdcall DllGetTerritoryBlack(int *inBoard, int boardSize)
{
	// Reset inBoard
	for(int i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++)
			inBoard[i*boardSize+j] = 0;
	}

	int influence[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	melkor->estimateScore(melkor->getBoard(), influence);

	for(i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++) {
			// Influence map is [x][y] format compared to board
			if(influence[j][i]>0)
				inBoard[i*boardSize+j] = 1;
		}
	}

	return 1;
}

/** @brief Return a board sized integer array with values !=0 marking
  * the position of white territory. 
  * @param inBoard A pointer to a pre-allocated integer array the same
  * size (but single-dimension) as the board being played on.
  * @param boardSize The size of the board, i.e. 19 (19x19). 
  * @return Always 1. */
int _stdcall DllGetTerritoryWhite(int *inBoard, int boardSize)
{
	// Reset inBoard
	for(int i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++)
			inBoard[i*boardSize+j] = 0;
	}

	int influence[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	melkor->estimateScore(melkor->getBoard(), influence);

	for(i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++) {
			// Influence map is [x][y] format compared to board
			if(influence[j][i]<0)
				inBoard[i*boardSize+j] = 1;
		}
	}

	return 1;
}

/** @brief Return a board sized integer array with values !=0 marking
  * the position of dead black stones. 
  * @param inBoard A pointer to a pre-allocated integer array the same
  * size (but single-dimension) as the board being played on.
  * @param boardSize The size of the board, i.e. 19 (19x19). 
  * @return Always 1. */
int _stdcall DllGetDeadStonesBlack(int *inBoard, int boardSize)
{
	// Reset inBoard
	for(int i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++)
			inBoard[i*boardSize+j] = 0;
	}

	const BoardStruct::stringTable& strings = melkor->getBoard().getStrings();
	BoardStruct::constStringsIterator citer(strings.begin());
	for(;citer!=strings.end();citer++) {
		if(citer->getStatus()!=DEAD && citer->getColour()==BLACK)
			continue;
		const GoString::pointsTable& points = citer->getPoints();
		GoString::constPointsIterator piter(points.begin());
		for(;piter!=points.end();piter++)
			inBoard[piter->y*boardSize+piter->x] = 1;
	}

	return 1;
}

/** @brief Return a board sized integer array with values !=0 marking
  * the position of dead white stones. 
  * @param inBoard A pointer to a pre-allocated integer array the same
  * size (but single-dimension) as the board being played on.
  * @param boardSize The size of the board, i.e. 19 (19x19). 
  * @return Always 1. */
int _stdcall DllGetDeadStonesWhite(int *inBoard, int boardSize)
{
	// Reset inBoard
	for(int i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++)
			inBoard[i*boardSize+j] = 0;
	}

	const BoardStruct::stringTable& strings = melkor->getBoard().getStrings();
	BoardStruct::constStringsIterator citer(strings.begin());
	for(;citer!=strings.end();citer++) {
		if(citer->getStatus()!=DEAD && citer->getColour()==WHITE)
			continue;
		const GoString::pointsTable& points = citer->getPoints();
		GoString::constPointsIterator piter(points.begin());
		for(;piter!=points.end();piter++)
			inBoard[piter->y*boardSize+piter->x] = 1;
	}

	return 1;
}

/** @brief Return a board sized integer array with values !=0 marking
  * the position of all dead stones. 
  * @param inBoard A pointer to a pre-allocated integer array the same
  * size (but single-dimension) as the board being played on.
  * @param boardSize The size of the board, i.e. 19 (19x19). 
  * @return Always 1. */
int _stdcall DllGetDeadStones(int *inBoard, int boardSize)
{
	// Reset inBoard
	for(int i=0;i<boardSize;i++) {
		for(int j=0;j<boardSize;j++)
			inBoard[i*boardSize+j] = 0;
	}

	const BoardStruct::stringTable& strings = melkor->getBoard().getStrings();
	BoardStruct::constStringsIterator citer(strings.begin());
	for(;citer!=strings.end();citer++) {
		if(citer->getStatus()!=DEAD)
			continue;
		const GoString::pointsTable& points = citer->getPoints();
		GoString::constPointsIterator piter(points.begin());
		for(;piter!=points.end();piter++)
			inBoard[piter->y*boardSize+piter->x] = 1;
	}

	return 1;
}

/** @return The score with respect to Black. */
float _stdcall DllScoreBoard()
{
	/** @todo We should run updateStringsStatus() if there is no current string
	  * status info available in Melkor. */
	return melkor->estimateScore(melkor->getBoard());
}

/** @brief Calculate a final score with the board and strings statuses updated.
  * @return The final score with respect to Black. */
float _stdcall DllFinalScore()
{
	return melkor->getFinalScore();
}

/** @brief A function to start logging DLL output to a file. 
  * @param filename The file to send log messages to. 
  * @return Always 1. */
int _stdcall DllLogStart(const char* filename)
{
	LogWriter::startLogFile(string(filename));
	return 1;
}

/** @brief A function to stop logging DLL output to a file.
  * @return Always 1. */
int _stdcall DllLogStop()
{
	LogWriter::stopLogFile();
	return 1;
}
