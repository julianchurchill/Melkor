// All the static GTP functions that we support

// TODO
// Make sure when reading in a coordinate of the form 'A1'
// that the letter is converted to lower case if necessary.
// This is causing problems with GoGui which passes coords
// upper case...

#include "globals.h"
#include "GTPFunctions.h"
#include "Main.h"
#include "SGFReader.h"
#include "SGFWriter.h"
#include "Misc.h"
#include "CaptureGoal.h"
#include "StoneCountGoal.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

// for gtp_cputime
#ifdef _WIN32
#include <windows.h>
#endif

const char* GTPFunctions::gtpCommandList[] = 
{
	"all_legal",
	"backsgf",
	"black",
	"boardsize",
	"capture_string",
	"clear_board",
	"clear_search_history",
	"cputime",
	"empty",
	"estimate_score",
	"eval",
	"final_score",
	"final_status_list",
	"fixed_handicap",
	"genmove",
	"genmove_black",
	"genmove_white",
	"get_random_seed",
	"help",
	"influence_map",
	"is_legal",
	"kgs-genmove_cleanup",
	"komi",
	"known_command",
	"liberty_influence_map",
	"list_big_moves",
	"list_commands",
	"list_strings_status",
	"loadsgf",
	"loadnetwork",
	"menu",
	"mtdf_search",
	"name",
	"network_suggest_black",
	"network_suggest_white",
	"nextsgf",
	"place_free_handicap",
	"play",
	"print_search_settings",
	"protocol_version",
	"quit",
	"reg_genmove",
	"reset",
	"reset_MTDF",
	"save_sgf",
	"score_point",
	"score_all_points_and_print_board",
	"score_all_points",
	"score_all_points_and_illegal",
	"set_black_name",
	"set_goal",
	"set_free_handicap",
	"set_pruning_settings",
	"set_pass_value",
	"set_play_timed_game",
	"set_search_depth",
	"set_time_per_move",
	"set_useNChildren",
	"set_white_name",
	"showboard",
	"string_data",
	"suggest_black",
	"suggest_white",
	"time_left",
	"time_settings",
	"undo",
	"update_strings_status",
	"version",
	"white",
	NULL
};

// the second argument is a failsafe so we don't get a type mismatch
// when specifying the void*
GTPFunctions::GTPFunctions(const void* m, bool useMelkorType) : melkor(m)
{
	// check melkor type is Melkor*
	if(((Melkor*)melkor)->getName()!="Melkor") {
		LogWriter::printerr("Invalid Melkor* in GTPFunctions()\n", "");
		exit(0);
	}

	// initialise member variables
	gtp_boardsize = -1;
	theyPassed = false;
	((Melkor*)melkor)->setSGFModified(false);

	programRunning = false;
	id = 0;
}

///////////////////
// GTP functions //
///////////////////

void GTPFunctions::play_gtp(string gtpScriptFilename)
{
	gtp_internal_set_boardsize(((Melkor*)melkor)->getBoard().getSize());

	// Reset the sgf tree
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->initialiseSGFTree(gtp_boardsize);

	if(gtpScriptFilename == "")
		gtp_main_loop(NULL);
	else
		gtp_main_loop(gtpScriptFilename.c_str());
}

void GTPFunctions::gtp_pre_command()
{
	lastOutput = "";
//#ifdef _MFC_VER
//	CSingleLock g(&gtpLock, TRUE);
//#endif
}

int GTPFunctions::gtp_all_legal(char *s, int id)
{
	gtp_pre_command();
	vector<Move> m;
	int colour;
//	int x[361];
//	int y[361];
	int* x;
	int* y;

	// get colour we are getting legal moves for
	if(!gtp_decode_colour(s, &colour))
		return gtp_failure(id, "Invalid colour");

	if(Go::dontFillOwnEyes)
		gtp_printf("Warning: Go::dontFillOwnEyes == true, so may affect result.\n");

	Go::getLegalMoves(m, colour, ((Melkor*)melkor)->getBoard(), Go::getUseSuperKo());
	x = new int[m.size()];
	y = new int[m.size()];

	// convert Move objects into coordinate
	// lists for gtp_print_vertices function
//	int n;
//	for(n=0;n<m.size();n++)
//	{
//		x[n] = m[n].getX();
//		y[n] = m[n].getY();
//	}

	vector<Move>::iterator iter;
	int n = 0;
	for(iter = m.begin();iter!=m.end();iter++)
	{
		x[n] = (*iter).getX();
		y[n] = (*iter).getY();
		n++;
	}

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertices(n, x, y, gtp_boardsize);
	delete x;
	delete y;
	return gtp_finish_response();	
}

// step back in the sgf file if it is still valid
int GTPFunctions::gtp_backsgf(char *s, int id)
{
	gtp_pre_command();

	// if no step, just go back one move
	int step, jump;
	if(sscanf(s, "%d", &step)==EOF)
		step = 1;

//	jump = Go::getCurrentMoveNumber()-step;
	jump = 	((Melkor*)melkor)->getInternalSGF().getCurrentDepth()-step;

	if(jump<0)
		jump = 0;

	// reset the board etc...
	// then step forwards through the file
	Go::resetMoveList();
	((Melkor*)melkor)->getBoard().clear();

	// add setup points
	((Melkor*)melkor)->getInternalSGF().rewindToStart();
	setupBoardFromSGF(((Melkor*)melkor)->getInternalSGF(), ((Melkor*)melkor)->getBoard());

	return gtp_sgfstep(s, id, jump, &(((Melkor*)melkor)->getInternalSGF().getRootNode()));
}

int GTPFunctions::gtp_capture_string(char *s, int id)
{
	gtp_pre_command();
	char c, d[4];
	int ourColour, x, y; 
	if(sscanf(s, "%c %s", &c, &d)!=2)
		return gtp_failure(id, "Error - use 'capture_string [b|w (nextToPlay)] [coord(e.g. a1)]'");
	if(c=='w' || c=='W')
		ourColour = WHITE;
	else if(c=='b' || c=='B')
		ourColour = BLACK;
	else 
		return gtp_failure(id, "Error - use 'capture_string [b|w (nextToPlay)] [coord(e.g. a1)]'");
	x = Move::GoPosToX(d);
	y = Move::GoPosToY(d, ((Melkor*)melkor)->getBoard().getSize());
	if(x<0 || y<0 || x>(((Melkor*)melkor)->getBoard().getSize()) || y>(((Melkor*)melkor)->getBoard().getSize()))
	{
		char buffer[200];
		sprintf(buffer, "Error - coords out of range - (%d,%d), use 'capture_string [b|w (nextToPlay)] [coord(e.g. a1)]'",
			x, y);
		return gtp_failure(id, buffer);
	}

	// setup the capture string coords before running the eval function
	((Melkor*)melkor)->saveSearchSettings();
	((Melkor*)melkor)->setSearchGoal(Goal(new CaptureGoal(x, y)));

	Move m;

	// always do the search from the point of view of the side that is trying
	// to capture, that way the search will always end immediately if a capture does occur.
	((Melkor*)melkor)->getMove(m, ((Melkor*)melkor)->getBoard(), ourColour);
	
	// set search to old eval function
	((Melkor*)melkor)->restoreSearchSettings();

	m.setColour(ourColour);

	gtp_printid(id, GTP_SUCCESS);
	
	gtp_print_vertex(m.getX(), m.getY(), gtp_boardsize);

	// always print best line of play
	char buffer[50];
	string message("Best move (AB-score, NNScore): ");
	if(m.getColour()==BLACK)
		message+="B[";
	else
		message+="W[";
	message+=m.xyToGoPosValue(m.getX(), m.getY(), m.getBoardSize());
	message+="] ";
	sprintf(buffer, "%d", m.getAlphaBetaScore());
	message+=buffer;
	message+=" ";
	sprintf(buffer, "%*g", 7, m.getNNScore());
	message+=buffer;
	gtp_printf(message.c_str());
				
	message = "Best line of play: ";
	vector<Move> l;
	((Melkor*)melkor)->getBestLineOfPlay(l);
	for(int i=0;i<l.size();i++)
	{
		if(l[i].getColour()==BLACK)
			message+="B[";
		else
			message+="W[";
		message+=l[i].xyToGoPosValue(l[i].getX(), l[i].getY(), l[i].getBoardSize());
		message+="] ";
	}
	gtp_printf(message.c_str());

	message = "Evaluation score = ";
	sprintf(buffer, "%*g", 5, m.getAlphaBetaScore());
	message+=buffer;
	gtp_printf("%s\n", message.c_str());

	return gtp_finish_response();
}

int GTPFunctions::gtp_clear_board(char *s, int id)
{
	gtp_pre_command();
	((Melkor*)melkor)->clearBoard();

	// always reset the MTDF stuff since we must assume a boardsize change
	// indicates a new game here.
	// Anyway the previous contents of the TTable in MTDFSearch will probably be for a
	// different board size...
	((Melkor*)melkor)->mtdfSearch->init();

	// Update the sgf tree
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addSetupNode(((Melkor*)melkor)->getBoard());

	return gtp_success(id, "");
}

int GTPFunctions::gtp_clear_search_history(char *s, int id)
{
	gtp_pre_command();
	((Melkor*)melkor)->mtdfSearch->cleanTranspositionTable();
	return gtp_success(id, "");
}

// report the kernel+user cpu time elapsed so far
int GTPFunctions::gtp_cputime(char *s, int id)
{
	gtp_pre_command();

	double timeSpent;

	// This bit will only work if we have WIN32
#ifdef _WIN32
	// taken from GNUGo 3.4 gg_utils.c
    FILETIME creationTime, exitTime, kernelTime, userTime;
    ULARGE_INTEGER uKernelTime, uUserTime, uElapsedTime;
    GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime,
                    &kernelTime, &userTime);
    uKernelTime.LowPart = kernelTime.dwLowDateTime;
    uKernelTime.HighPart = kernelTime.dwHighDateTime;
    uUserTime.LowPart = userTime.dwLowDateTime;
    uUserTime.HighPart = userTime.dwHighDateTime;
    uElapsedTime.QuadPart = uKernelTime.QuadPart + uUserTime.QuadPart;
    /* convert from multiples of 100nanosecs to seconds: */
    timeSpent = (signed __int64)(uElapsedTime.QuadPart) * 1.e-7;
#else
	return gtp_failure(id, "CPU timing unavailable, current clock time in seconds: %g", time(NULL));
#endif
	return gtp_success(id, "%g", timeSpent);
}

/** @brief This GTP command estimates the score without doing any status
  * update calculations. gtp_final_score does status updates. */
int GTPFunctions::gtp_estimate_score(char *s, int id)
{
	gtp_pre_command();
	float score = ((Melkor*)melkor)->estimateScore(((Melkor*)melkor)->getBoard());
	gtp_printid(id, GTP_SUCCESS);
	if(score<0.0)
		gtp_printf("Scaling to strength: W+%3.1f\n", -score);
	else if(score>0.0)
		gtp_printf("Scaling to strength: B+%3.1f\n", score);
	else
		gtp_printf("Scaling to strength: 0");
	score = ((Melkor*)melkor)->estimateScore(((Melkor*)melkor)->getBoard(), NULL, 5, 21, false);
	if(score<0.0)
		gtp_printf("Without S2S: W+%3.1f", -score);
	else if(score>0.0)
		gtp_printf("Without S2S: B+%3.1f", score);
	else
		gtp_printf("Without S2S: 0");
	return gtp_finish_response();
}

int GTPFunctions::gtp_eval(char *s, int id)
{
	gtp_pre_command();
	bool printScores = false;
	float score;
	string message = "";

	if(((Melkor*)melkor)->mtdfSearch->getGoal().isGoalType(GoalInterface::GoalType::CAPTUREGOAL))
	{
		char c, x;
		int ourColour, y; 
		if(sscanf(s, "%c %c%d", &c, &x, &y)!=3)
			return gtp_failure(id, "Error - capture goal is currently selected, specify string coords 'eval [b|w] [coord(e.g. a1)]'");
		if(c=='w' || c=='W')
			ourColour = WHITE;
		else if(c=='b' || c=='B')
			ourColour = BLACK;
		else 
			return gtp_failure(id, "Error - capture goal is currently selected, specify string coords 'eval [b|w] [coord(e.g. a1)]'");

		// setup the capture string coords before running the eval function
		if(x<'a' || y<0 || x>('a'+((Melkor*)melkor)->getBoard().getSize()) || y>(((Melkor*)melkor)->getBoard().getSize()))
			return gtp_failure(id, "Error - coords out of range, use 'eval [b|w] [coord(e.g. a1)]'");

		((Melkor*)melkor)->mtdfSearch->setGoal(Goal(new CaptureGoal(x, y)));
		((Melkor*)melkor)->mtdfSearch->getGoal().evaluate(ourColour, score, 
			((Melkor*)melkor)->getBoard(), ourColour);
	}
	else if(((Melkor*)melkor)->mtdfSearch->getGoal().isGoalType(GoalInterface::GoalType::INFLUENCEGOAL)) {
		((Melkor*)melkor)->mtdfSearch->getGoal().evaluate(BLACK, score, 
			((Melkor*)melkor)->getBoard(), BLACK);
		message = "Score from BLACKs point of view, without komi:\n";

	}
	else {
		int ourColour; 
		if(!gtp_decode_colour(s, &ourColour))
			return gtp_failure(id, "Invalid colour - use eval [b|w]");
		((Melkor*)melkor)->mtdfSearch->getGoal().evaluate(ourColour, score, 
			((Melkor*)melkor)->getBoard(), ourColour);
	}

	char buffer[50];
	message+= "Evaluation score = ";
	sprintf(buffer, "%*g", 5, score);
	message+=buffer;
	return gtp_success(id, message.c_str());
}

/** @brief This GTP command updates the status of the strings and then
  * calculates the score. gtp_estimate_score does no status updates. */
int GTPFunctions::gtp_final_score(char *s, int id)
{
	gtp_pre_command();

	// Run final score and then use the status info in the GoStrings
	float score = ((Melkor*)melkor)->getFinalScore();

	gtp_printid(id, GTP_SUCCESS);
	if(score<0.0)
		gtp_printf("W+%3.1f", -score);
	else if(score>0.0)
		gtp_printf("B+%3.1f", score);
	else
		gtp_printf("0");
	return gtp_finish_response();
}

/** @brief Get a list of a specified class of stones, class may be
  * alive, seki or dead. This GTP command should only ever be used at the
  * end of a game. */
int GTPFunctions::gtp_final_status_list(char *s, int id)
{
	gtp_pre_command();

	// Mode - seki mode is ignored until we can recognise seki!
	bool alive = false;
	bool seki = false;
	bool dead = false;
	char mode[200];

	// Read and set mode from command arguments
	if(sscanf(s, "%s", &mode)==1) {
		// Check it's a valid mode...
		if(strcmp("alive", mode)==0) alive = true;
		else if(strcmp("dead", mode)==0) dead = true;
		else if(strcmp("seki", mode)==0) seki = true;
		else
			return gtp_failure(id, "Usage final_status_list [alive|seki|dead]");
	}
	else
		return gtp_failure(id, "Usage final_status_list [alive|seki|dead]");

	// Run final score and then use the status info in the GoStrings
	((Melkor*)melkor)->getFinalScore();

	gtp_printid(id, GTP_SUCCESS);

	int boardSize = ((Melkor*)melkor)->getBoard().getSize();
	BoardStruct::stringTable& strings = ((Melkor*)melkor)->getBoard().getStrings();
	BoardStruct::stringsIterator siter(strings.begin());
	for(;siter!=strings.end();siter++) {
		// If status is other than DEAD or SEKI(not currently implemented) it is alive
		if((siter->getStatus()==DEAD && dead) || (siter->getStatus()!=DEAD && alive)) {
			// Output the strings stones on a single line
			const GoString::pointsTable& points = siter->getPoints();
			GoString::constPointsIterator piter(points.begin());
			for(;piter!=points.end();piter++)
				gtp_printf("%s ", Move::xyToGoPosValue(piter->x, piter->y, boardSize).c_str());
			gtp_printf("\n");
		}
	}

	return gtp_finish_response();
}

// place handicap stones (go.doHandicap)
int GTPFunctions::gtp_fixed_handicap(char *s, int id)
{
	gtp_pre_command();
	int first = 1;
	int h;
	if(sscanf(s, "%d", &h)==EOF)
		return gtp_failure(id, "handicap not an integer");

	if(h<=0)
		return gtp_success(id, "");

	if(!((Melkor*)melkor)->setHandicap(h))
		return gtp_failure(id, "invalid handicap");

	gtp_printid(id, GTP_SUCCESS);

	// add the list of handicap vertices used
	// to our response
	for(int i=0;i<((Melkor*)melkor)->getBoard().getSize();i++) {
		for(int j=0;j<((Melkor*)melkor)->getBoard().getSize();j++) {
			if(((Melkor*)melkor)->getBoard().getPoint(j, i)!=EMPTY) {
				if(!first)
					gtp_printf(" ");
				else
					first = 0;
				gtp_printf("%s", 
					Move::xyToGoPosValue(j, i, ((Melkor*)melkor)->getBoard().getSize()).c_str());
				//gtp_mprintf("%m", j, i);
			}
		} // end for j
	} // end for i

	return gtp_finish_response();
}

// as above except colour is indicated in argument
int GTPFunctions::gtp_genmove(char *s, int id)
{
	gtp_pre_command();
	int colour;
	if(!gtp_decode_colour(s, &colour))
		return gtp_failure(id, "Invalid colour - use genmove [b|w] ['print' (for line of play)]");

	// extract printLineOfPlay argument
	bool printLineOfPlay = false;
	char args[50];
	args[0] = '\0';
	sscanf(s, "%*s %s", &args);
	if(strcmp(args, "print")==0)
		printLineOfPlay = true;

	Move m;
	// do same as genmove_black
	((Melkor*)melkor)->getMove(m, ((Melkor*)melkor)->getBoard(), colour);

	int j = m.getX();
	int i = m.getY();

	// add move to the board
	m.setColour(colour);
	m.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
	((Melkor*)melkor)->getBoard().addToBoard(m);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addMoveNode(m);
	
	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);
	// print best line of play if requested
	if(printLineOfPlay)	{
		vector<Move> moves;
		gtp_printf(" ");
		((Melkor*)melkor)->getBestLineOfPlay(moves);
		for(int i=0;i<moves.size();i++) {
			string moveValue = Move::xyToGoPosValue(
						moves[i].getX(), moves[i].getY(), moves[i].getBoardSize());
			if(moves[i].getColour()==BLACK)
				gtp_printf("B[%s] ", moveValue.c_str());
			else
				gtp_printf("W[%s] ", moveValue.c_str());
		}
		gtp_printf("\n");
	}
	return gtp_finish_response();
}

// generate the best move for black assuming its legal
// add to board and record in game record (go.addToMoveList)
int GTPFunctions::gtp_genmove_black(char *s, int id)
{
	gtp_pre_command();
	// extract printLineOfPlay argument
	bool printLineOfPlay = false;
	char args[50];
	args[0] = '\0';
	sscanf(s, "%s", &args);
	if(strcmp(args, "print")==0)
		printLineOfPlay = true;

	((Melkor*)melkor)->getMove(lastMove, ((Melkor*)melkor)->getBoard(), BLACK);

	int j = lastMove.getX();
	int i = lastMove.getY();

	lastMove.setColour(BLACK);

	// add move to the board
	lastMove.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	Go::addToMoveList(lastMove, ((Melkor*)melkor)->getBoard());
	((Melkor*)melkor)->getBoard().addToBoard(lastMove);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addMoveNode(lastMove);
	
	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);
	// print best line of play if requested
	if(printLineOfPlay)
	{
		vector<Move> moves;
		gtp_printf(" ");
		((Melkor*)melkor)->getBestLineOfPlay(moves);
		for(int i=0;i<moves.size();i++)
		{
			string moveValue = Move::xyToGoPosValue(
						moves[i].getX(), moves[i].getY(), moves[i].getBoardSize());
			if(moves[i].getColour()==BLACK)
				gtp_printf("B[%s] ", moveValue.c_str());
			else
				gtp_printf("W[%s] ", moveValue.c_str());
		}
		gtp_printf("\n");
	}
	return gtp_finish_response();
}

// same for white
int GTPFunctions::gtp_genmove_white(char *s, int id)
{
	gtp_pre_command();
	// extract printLineOfPlay argument
	bool printLineOfPlay = false;
	char args[50];
	args[0] = '\0';
	sscanf(s, "%s", &args);
	if(strcmp(args, "print")==0)
		printLineOfPlay = true;

	((Melkor*)melkor)->getMove(lastMove, ((Melkor*)melkor)->getBoard(), WHITE);

	int j = lastMove.getX();
	int i = lastMove.getY();

	lastMove.setColour(WHITE);

	// add move to the board
	lastMove.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	Go::addToMoveList(lastMove, ((Melkor*)melkor)->getBoard());
	((Melkor*)melkor)->getBoard().addToBoard(lastMove);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addMoveNode(lastMove);
	
	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);
	// print best line of play if requested
	if(printLineOfPlay)
	{
		vector<Move> moves;
		gtp_printf(" ");
		((Melkor*)melkor)->getBestLineOfPlay(moves);
		for(int i=0;i<moves.size();i++)
		{
			string moveValue = Move::xyToGoPosValue(
						moves[i].getX(), moves[i].getY(), moves[i].getBoardSize());
			if(moves[i].getColour()==BLACK)
				gtp_printf("B[%s] ", moveValue.c_str());
			else
				gtp_printf("W[%s] ", moveValue.c_str());
		}
		gtp_printf("\n");
	}
	return gtp_finish_response();
}

int GTPFunctions::gtp_get_random_seed(char* s, int id)
{
	gtp_pre_command();
	return gtp_success(id, "%d", random_seed());
}

// Print help, e.g. what GTP commands we provide space separated
// as opposed to list_commands which is separated by '\n' 
int GTPFunctions::gtp_help(char *s, int id)
{
	gtp_pre_command();
	gtp_printid(id, GTP_SUCCESS);

	int i=0;
	while(gtpCommandList[i]!=NULL)
	{
		gtp_printf(gtpCommandList[i]);
		gtp_printf(" ");
		i++;
	}
	return gtp_finish_response();
}

int GTPFunctions::gtp_influence_map(char *s, int id)
{
	gtp_pre_command();

	int dilations = 5;
	int erosions = 21;
	// Look for erosion and dilation arguments
	if(sscanf(s, "%d %d", &dilations, &erosions)==2) {
		if(dilations<0 || erosions<0)
			return gtp_failure(id, "dilations and erosions must be positive!");
	}
	else
		return gtp_failure(id, "Usage: influence_map [<dilations> <erosions>]");

	float score = 0.0f;
	int influenceMap[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	calculateInfluence(((Melkor*)melkor)->getBoard(), score, BLACK, "GTPFunctions::gtp_influence_map()",
		influenceMap, dilations, erosions);

	// Print the map as a board position, allow spaces to fit 4 digit number (inc. sign) in per point
	printInfluenceMap(influenceMap, ((Melkor*)melkor)->getBoard().getSize());

	gtp_printf("\nWith DEAD strings removed - via estimateScore(no scale to strength):\n");

	// Now generate one with the dead strings removed
	((Melkor*)melkor)->estimateScore(((Melkor*)melkor)->getBoard(), influenceMap, dilations, erosions, false);
	printInfluenceMap(influenceMap, ((Melkor*)melkor)->getBoard().getSize());

	return gtp_success(id, "");
}

int GTPFunctions::gtp_is_legal(char *s, int id)
{
	gtp_pre_command();
	int colour;
	int j;
	int i;

	if(!gtp_decode_move(s, &j, &i, &colour, gtp_boardsize))
		return gtp_failure(id, "Invalid coordinate or colour, use colour followed by vertex");

	if(Go::dontFillOwnEyes)
		gtp_printf("Warning: Go::dontFillOwnEyes == true, so may affect result.\n");

	if(Go::isLegal(((Melkor*)melkor)->getBoard(), j, i, colour, Go::getUseSuperKo()))
		return gtp_success(id, "true");
	else
		return gtp_success(id, "false");
}

/** @brief This GTP command is specifically for communicating with KGS servers. 
  * At the end of a game, in the case of a dispute over dead stones, this command
  * instructs the engine to play moves without passing until no disputed stones remain. */
int GTPFunctions::gtp_kgs_genmove_cleanup(char *s, int id)
{
	gtp_pre_command();
	int colour;
	if(!gtp_decode_colour(s, &colour))
		return gtp_failure(id, "Invalid colour - use kgs-genmove_cleanup [b|w]");

	// If there are no more dead stones on the board according to our
	// status estimates then return a pass
	bool deadStonesPresent = false;
	BoardStruct::stringTable& strings = ((Melkor*)melkor)->getBoard().getStrings();
	BoardStruct::stringsIterator siter(strings.begin());
	for(;siter!=strings.end();siter++) {
		if(siter->getStatus()==DEAD) {
			deadStonesPresent = true;
			break;
		}
	}

	// No dead stones - so pass
	if(!deadStonesPresent) {
		gtp_printid(id, GTP_SUCCESS);
		gtp_print_vertex(-1, -1, gtp_boardsize);
		return gtp_finish_response();
	}

	// Otherwise do genmove but no passes allowed!
	((Melkor*)melkor)->setNoPasses(true);
	
	Move m;
	((Melkor*)melkor)->getMove(m, ((Melkor*)melkor)->getBoard(), colour);

	// add move to the board
	m.setColour(colour);
	m.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
	((Melkor*)melkor)->getBoard().addToBoard(m);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addMoveNode(m);
	
	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(m.getX(), m.getY(), gtp_boardsize);

	return gtp_finish_response();
}

int GTPFunctions::gtp_known_command(char *s, int id)
{
	gtp_pre_command();
	// output true or false depending on whether the
	// command in s is known by this program
	int i=0;
	while(gtpCommandList[i]!=NULL) {
		if(strcmp(s, gtpCommandList[i])==0)
			return gtp_success(id, "true");
		i++;
	}

	return gtp_success(id, "false");
}

int GTPFunctions::gtp_liberty_influence_map(char *s, int id)
{
	gtp_pre_command();

	int dilations = 5;
	int erosions = 21;
	// Look for erosion and dilation arguments
	if(sscanf(s, "%d %d", &dilations, &erosions)==2) {
		if(dilations<0 || erosions<0)
			return gtp_failure(id, "dilations and erosions must be positive!");
	}
	else
		return gtp_failure(id, "Usage: liberty_influence_map [<dilations> <erosions>]");

	float score = 0.0f;
	int influenceMap[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
	calculateLibertyInfluence(((Melkor*)melkor)->getBoard(), score, BLACK, "GTPFunctions::gtp_influence_map()",
		influenceMap, dilations, erosions);

	// Print the map as a board position, allow spaces to fit 4 digit number (inc. sign) in per point
	printInfluenceMap(influenceMap, ((Melkor*)melkor)->getBoard().getSize());

	gtp_printf("\nWith DEAD strings removed:\n");

	// Now generate one with the dead strings removed
	((Melkor*)melkor)->generateLibertyInfluenceMap(((Melkor*)melkor)->getBoard(), influenceMap, dilations, erosions);
	printInfluenceMap(influenceMap, ((Melkor*)melkor)->getBoard().getSize());

	return gtp_success(id, "");
}

/** List the n biggest moves with their values. */
int GTPFunctions::gtp_list_big_moves(char *s, int id)
{
	gtp_pre_command();

	int n;

	// get nextToPlay
	int nextToPlay;
	if(!gtp_decode_colour(s, &nextToPlay))
		return gtp_failure(id, "Invalid colour - use list_big_moves <b|w> <list max n moves>");

	if(sscanf(s, "%*s %d", &n)!=1)
		return gtp_failure(id, "Syntax: list_big_moves <b|w> <list max n moves>");
	if(n<0)
		return gtp_failure(id, "max number of moves to list must be positive!");

	// Save time limit
	double timePerMove = ((Melkor*)melkor)->getTimePerMove();
	((Melkor*)melkor)->setTimePerMove(0.0);

	vector<Melkor::BigMoveInfo> bigMoves;
	((Melkor*)melkor)->getBigMoves(bigMoves, NULL, ((Melkor*)melkor)->getBoard(), nextToPlay, n);
//		return gtp_failure(id, "Go::getBigMoves() failed!");

	// Restore time limit
	((Melkor*)melkor)->setTimePerMove(timePerMove);

	gtp_printid(id, GTP_SUCCESS);

	if(bigMoves.size()==0)
		gtp_printf("No big moves!\n");

	for(int i=0;i<bigMoves.size();i++)
	{
		gtp_print_vertex(bigMoves[i].move.getX(), bigMoves[i].move.getY(), gtp_boardsize);
		gtp_printf(":%g \n", bigMoves[i].worth);
	}

	return gtp_finish_response();
}

int GTPFunctions::gtp_list_commands(char *s, int id)
{
	// print all commands we understand separated by newlines
	gtp_pre_command();
	gtp_printid(id, GTP_SUCCESS);

	int i=0;
	while(gtpCommandList[i]!=NULL)
	{
		gtp_printf("%s\n", gtpCommandList[i]);
		i++;
	}
	gtp_printf("\n");
	return GTP_OK;
}

int GTPFunctions::gtp_list_strings_status(char *s, int id)
{
	gtp_pre_command();

	BoardStruct& board = ((Melkor*)melkor)->getBoard();

	gtp_printid(id, GTP_SUCCESS);

	const BoardStruct::stringTable& strings = board.getStrings();
	BoardStruct::constStringsIterator citer(strings.begin());
	for(;citer!=strings.end();citer++)
	{
		gtp_printf("String at {%s} status = ", 
			Move::xyToGoPosValue(citer->getPoints().begin()->x, citer->getPoints().begin()->y,
				board.getSize()).c_str());
		if(citer->getStatus()==ALIVE)
			gtp_printf("ALIVE\n");
		else if(citer->getStatus()==DEAD)
			gtp_printf("DEAD\n");
		else if(citer->getStatus()==URGENT)
			gtp_printf("URGENT\n");
		else
			gtp_printf("UNKNOWN (%d)\n", citer->getStatus());
	}

	return gtp_finish_response();
}

/* Function:  Load an sgf file
 * Arguments: <filename> [move_number]
 * Fails:     missing filename or failure to open or parse file
 * Returns:   color to play
 *
 * The board, number of captured stones and move history should be updated to the
 * position before move_number if supplied or to the end of the sgf otherwise.
 *
 */
int GTPFunctions::gtp_loadsgf(char *s, int id)
{
	gtp_pre_command();
	char filename[GTP_BUFSIZE];
  
	if(sscanf(s, "%s", filename)==EOF)
		return gtp_failure(id, "missing filename");
  
	if(!((Melkor*)melkor)->getInternalSGF().load(filename))
		return gtp_failure(id, "cannot open or parse '%s'", filename);

	int move_number = -1;
	if(sscanf(s, "%*s %d", &move_number)==1 && move_number<0)
		return gtp_failure(id, "move_number must be positive!");

	string k;
	((Melkor*)melkor)->getInternalSGF().getBoardSize(k);
	int size = atoi(k.c_str());
	if(size==0) size = 19;
	((Melkor*)melkor)->resizeBoard(size);
	gtp_internal_set_boardsize(size);
	float f;
	((Melkor*)melkor)->getInternalSGF().getKomi(f);
	Go::setKomi(f);
	int p;
	((Melkor*)melkor)->getInternalSGF().getHandicap(p);
	Go::setHandicap(p);
	int colour_to_move;
	if(p>0)
		colour_to_move = BLACK;
	else
		colour_to_move = WHITE;

	setupBoardFromSGF(((Melkor*)melkor)->getInternalSGF(), ((Melkor*)melkor)->getBoard());

	Go::resetMoveList();

	SGFNode* nextNode = &(((Melkor*)melkor)->getInternalSGF().getRootNode());
	((Melkor*)melkor)->setSGFModified(false);

	// advance the sgf to the end if move_number == -1
	// otherwise advance to the position before move_number
	int currentMoveNumber = 0;
	string moveString;
	vector<string> stringProps;
	Move move;
	while(((Melkor*)melkor)->getInternalSGF().peekNextNode()!=NULL && (move_number==-1 || currentMoveNumber<move_number)) {
		nextNode = ((Melkor*)melkor)->getInternalSGF().getNextNode();

		// parse any setup tags at this node
		setupBoardFromSGFNode(((Melkor*)melkor)->getInternalSGF(), ((Melkor*)melkor)->getBoard(), nextNode);

		// get the actual move
		// try grab a black move first
		nextNode->getProperty(SGFProperty::blackMoveTag, stringProps);
		if(stringProps.size()>0 && (moveString=stringProps[0])!="") {
			// next to move
			colour_to_move = WHITE;
			move.setColour(BLACK);
		}
		// try grab a white move
		else {
			nextNode->getProperty(SGFProperty::whiteMoveTag, stringProps);
			if(stringProps.size()>0 && (moveString=stringProps[0])!="")	{
				// next to move
				colour_to_move = BLACK;
				move.setColour(WHITE);
			}
		}
		// we found a move of either colour so add it to the board...
		if(moveString!="") {
			move.setX(Move::SGFToX(moveString));
			move.setY(Move::SGFToY(moveString));
			move.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
			Go::addToMoveList(move, ((Melkor*)melkor)->getBoard());
			((Melkor*)melkor)->getBoard().addToBoard(move);
		}
		currentMoveNumber++;
	}

	gtp_printid(id, GTP_SUCCESS);
	if(colour_to_move==BLACK)
		gtp_printf("black");
	else if(colour_to_move==WHITE)
		gtp_printf("white");
	else
		gtp_printf("empty");
	return gtp_finish_response();
}

int GTPFunctions::gtp_loadnetwork(char *s, int id)
{
	gtp_pre_command();
	char filename[GTP_BUFSIZE];

	if(sscanf(s, "%s", filename)==EOF)
		return gtp_failure(id, "missing filename");

	string temp = filename;
	if(!((Melkor*)melkor)->loadBPN(temp))
		return gtp_failure(id, "load network failed");

	return gtp_success(id, "");
}

// Use setupMTDFSearch() string argument to set search parameters
// and run the search.
int GTPFunctions::gtp_mtdf_search(char *s, int id)
{
	gtp_pre_command();

	if(s==NULL) {
		string message("Please specify some parameters:");
		message+=((Melkor*)melkor)->mtdfSearch->getSetupStringSyntax();
		return gtp_failure(id, message.c_str());
	}

	// save MTDFSearch setup for restoration after tests
	((Melkor*)melkor)->mtdfSearch->saveSettings();

	// arguments should be any settings required
	string setupString(s);
	if(!((Melkor*)melkor)->mtdfSearch->setupMTDFSearch(&setupString, ((Melkor*)melkor)->getBoard())) {
		// restore MTDFSearch settings
		((Melkor*)melkor)->mtdfSearch->restoreSettings();
		char buffer[500];
		string message = "Error in gtp_mtdf_search(), bad setup string: ";
		sprintf(buffer, "%s", s);
		message+=buffer;
		return gtp_failure(id, message.c_str());
	}

	// NOTE: ourColour and nextToPlay args can be the same
	// here, it makes no difference
	Move m;
	float result = ((Melkor*)melkor)->mtdfSearch->suggestMove(m, ((Melkor*)melkor)->getBoard(),
		((Melkor*)melkor)->mtdfSearch->getNextToPlay());

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(m.getX(), m.getY(), gtp_boardsize);

	// restore MTDFSearch settings
	((Melkor*)melkor)->mtdfSearch->restoreSettings();

	return gtp_finish_response();
}

int GTPFunctions::gtp_menu(char *s, int id)
{
	gtp_pre_command();
	LogWriter log;
	((Melkor*)melkor)->DoMainMenu(log);
	return gtp_success(id, "Melkor");
}

int GTPFunctions::gtp_name(char *s, int id)
{
	gtp_pre_command();
	return gtp_success(id, "Melkor");
}

int GTPFunctions::gtp_network_suggest_black(char *s, int id)
{
	gtp_pre_command();
	vector<Move> m;
	Move best;
	Melkor::getScoredMoves(m, ((Melkor*)melkor)->getBoard(), BLACK, *((Melkor*)melkor)->getBPN());

	// sort the list in order of score
	sort(m.begin(), m.end());
	best = m[m.size()-1];

	int j = best.getX();
	int i = best.getY();

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);
	return gtp_finish_response();
}

int GTPFunctions::gtp_network_suggest_white(char *s, int id)
{
	gtp_pre_command();
	vector<Move> m;
	Move best;
	Melkor::getScoredMoves(m, ((Melkor*)melkor)->getBoard(), WHITE, *((Melkor*)melkor)->getBPN());

	// sort the list in order of score
	sort(m.begin(), m.end());
	best = m[m.size()-1];

	int j = best.getX();
	int i = best.getY();

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);
	return gtp_finish_response();
}

// step forward in the sgf file if it is still valid
int GTPFunctions::gtp_nextsgf(char *s, int id)
{
	gtp_pre_command();

	int jump;
	if(sscanf(s, "%d", &jump)==EOF)
		jump = 1;
	return gtp_sgfstep(s, id, jump, ((Melkor*)melkor)->getInternalSGF().getCurrentNode());
}

// not a GTP command, used by nextsgf and backsgf
int GTPFunctions::gtp_sgfstep(char *s, int id, int jump, SGFNode* nextNode)
{
	gtp_pre_command();
	string message, c;
	char buffer[50];
	static int oldColour;
	int colour;
	Move move;
	vector<Move> tags;

	// step 'jump' moves forward
	while(jump>0) {
		if(((Melkor*)melkor)->getInternalSGF().peekNextNode()==NULL)
			return gtp_success(id, "End of SGF file");
		nextNode = ((Melkor*)melkor)->getInternalSGF().getNextNode();
		sprintf(buffer, "%i", (Go::getCurrentMoveNumber()+1));
		message+= buffer;
		message+=": ";
		vector<string> props;
		nextNode->getProperty(SGFProperty::blackMoveTag, props);
		if(props.size()>0 && (c=props[0])!="") {
			colour = BLACK;
			message+= "B [";
			message+=c;
			message+="] ";
			theyPassed = false;
			oldColour = colour;
		}
		else {
			nextNode->getProperty(SGFProperty::whiteMoveTag, props);
			if(props.size()>0 && (c=props[0])!="") {
				colour = WHITE;
				message+= "W [";
				message+=c;
				message+="] ";
				theyPassed = false;
				oldColour = colour;
			}
			else {
				// in case of a B[] or W[] pass node...
				if(oldColour==BLACK) {
					message+="W ";
					colour = WHITE;
				}
				else {
					message+="B ";
					colour = BLACK;
				}
				c = "tt";
				message+="[] ";
				theyPassed = false;
			}
		}

		setupBoardFromSGFNode(((Melkor*)melkor)->getInternalSGF(), ((Melkor*)melkor)->getBoard(), nextNode);

		// add move to board
		move.setX(Move::SGFToX(c));
		move.setY(Move::SGFToY(c));
		move.setColour(colour);
		move.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
		if(move.getX()==-1 || move.getY()==-1)
			message+="Pass";
		else
			message+=Move::xyToGoPosValue(move.getX(), move.getY(), move.getBoardSize());
		message+="\n";
		Go::addToMoveList(move, ((Melkor*)melkor)->getBoard());
		((Melkor*)melkor)->getBoard().addToBoard(move);
		
		jump--;
	} // end while(jump)

	return gtp_success(id, message.c_str());
}

/** @brief This gtp command asks the program (Melkor) to place a number
  * of handicap stones anywhere it likes and print out the vertices. */
int GTPFunctions::gtp_place_free_handicap(char *s, int id)
{
	// 1 argument, int number_of_stones - Number of handicap stones to place
	// Placement is at engines discretion
	int numberOfStones = 0;
	if(sscanf(s, "%d", &numberOfStones)==EOF)
		return gtp_failure(id, "bad number of handicap stones");

	vector<Move> stones;
	((Melkor*)melkor)->getBoard().clear();
	Go::setHandicap(numberOfStones);
	Go::doFreeHandicap(stones, ((Melkor*)melkor)->getBoard());

	/** @todo How do we specify a free handicap in the SGF game info? */
	((Melkor*)melkor)->setSGFModified(true);
	SGFGameInfo info(((Melkor*)melkor)->getInternalSGF().getGameInfo());
	info.handicap = numberOfStones;
	((Melkor*)melkor)->getInternalSGF().clear();
	((Melkor*)melkor)->getInternalSGF().setGameInfo(info);
	((Melkor*)melkor)->getInternalSGF().addSetupNode(((Melkor*)melkor)->getBoard());

	// separate stones into movei and movej
	int n = stones.size();
	int* movei = new int[n];
	int* movej = new int[n];
	for(int i=0;i<n;i++) {
		movej[i] = stones[i].getX();
		movei[i] = stones[i].getY();
	}

	// Output: A list of vertices where the handicap stones have been placed
	gtp_print_vertices(n, movej, movei, gtp_boardsize);

	delete movej;
	delete movei;

	return gtp_success(id, "");
}

int GTPFunctions::gtp_play(char *s, int id)
{
	// 1 argument - Colour and vertex of move

	// identify colour, strip it from s and pass move string on to playblack() or playwhite()
	int colour;
	int charsRead;
	char* newS = s;
	if((charsRead=gtp_decode_colour(s, &colour))>0)
	{
		// advance our pointer into s so we skip the colour bit
		for(int i=0;i<charsRead;i++)
			newS++;

		if(colour==BLACK)
			return gtp_playblack(newS, id);
		else
			return gtp_playwhite(newS, id);
	}
	else
		return gtp_failure(id, "bad move format, use 'a1' style");
}

// place a black stone, including legality checks
// and record in game record (go.addToMoveList)
int GTPFunctions::gtp_playblack(char *s, int id)
{
	gtp_pre_command();
	Move m;

	// remove whitespace and make all lower case
	char *c;
	while(*s==' ') s++;
	for(c=s;*c;c++)
		*c = tolower((int)*c);

	if(strncmp(s, "pass", 4)==0) {
		Move::getPassMove(m);
		theyPassed = true;
	}
	else {
		// decode s into j,i coords
		string temp(s);
		if(temp=="")
			return gtp_failure(id, "bad move format, use 'a1' style");
		int i, j;
		if((j=Move::GoPosToX(temp))==-1)
			return gtp_failure(id, "bad move format");
		if((i=Move::GoPosToY(temp, ((Melkor*)melkor)->getBoard().getSize()))==-1)
			return gtp_failure(id, "bad move format");
		// verify coords and legality
		// NOTE: We must specify our colour to be the opposite of the stone being played
		// so we avoid the shortcuts that our colour uses such as Go::dontFillOwnEyes
		if(!Go::isLegal(((Melkor*)melkor)->getBoard(), j, i, BLACK, Go::getUseSuperKo(), false, NULL, false, WHITE))
			return gtp_failure(id, "illegal move");
		m.setX(j);
		m.setY(i);
		theyPassed = false;
	}

	m.setColour(BLACK);

	// add move to the board
	m.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
	((Melkor*)melkor)->getBoard().addToBoard(m);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addMoveNode(m);
		
	return gtp_success(id, "");
}

int GTPFunctions::gtp_playempty(char *s, int id)
{
	gtp_pre_command();
	Move m;

	// remove whitespace and make all lower case
	char *c;
	while(*s==' ') s++;
	for(c=s;*c;c++)
		*c = tolower((int)*c);

	int i, j;
	if(strncmp(s, "pass", 4)==0) {
		i = -1;
		j = -1;
		theyPassed = true;
	}
	else {
		// decode s into j,i coords
		string temp(s);
		if(temp=="")
			return gtp_failure(id, "bad move format, use 'a1' style");
		if((j=Move::GoPosToX(temp))==-1)
			return gtp_failure(id, "bad move format");
		if((i=Move::GoPosToY(temp, ((Melkor*)melkor)->getBoard().getSize()))==-1)
			return gtp_failure(id, "bad move format");
		theyPassed = false;
	}

	((Melkor*)melkor)->getBoard().setPoint(j, i, EMPTY);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addSetupNode(((Melkor*)melkor)->getBoard());
		
	return gtp_success(id, "");
}

// same for white
int GTPFunctions::gtp_playwhite(char *s, int id)
{
	gtp_pre_command();
	Move m;

	// remove whitespace and make all lower case
	char *c;
	while(*s==' ') s++;
	for(c=s;*c;c++)
		*c = tolower((int)*c);

	if(strncmp(s, "pass", 4)==0) {
		Move::getPassMove(m);
		theyPassed = true;
	}
	else {
		// decode s into j,i coords
		string temp(s);
		int i, j;
		if((j=Move::GoPosToX(temp))==-1)
			return gtp_failure(id, "bad move format");
		if((i=Move::GoPosToY(temp, ((Melkor*)melkor)->getBoard().getSize()))==-1)
			return gtp_failure(id, "bad move format");
		// verify coords and legality
		// NOTE: We must specify our colour to be the opposite of the stone being played
		// so we avoid the shortcuts that our colour uses such as Go::dontFillOwnEyes
		if(!Go::isLegal(((Melkor*)melkor)->getBoard(), j, i, WHITE, Go::getUseSuperKo(), false, NULL, false, BLACK))
			return gtp_failure(id, "illegal move");
		m.setX(j);
		m.setY(i);
		theyPassed = false;
	}

	m.setColour(WHITE);

	// add move to the board
	m.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
	((Melkor*)melkor)->getBoard().addToBoard(m);

	// adding a self generated move invalidates the SGF
	((Melkor*)melkor)->setSGFModified(true);
	((Melkor*)melkor)->getInternalSGF().addMoveNode(m);
		
	return gtp_success(id, "");
}

int GTPFunctions::gtp_print_search_settings(char *s, int id)
{
	gtp_pre_command();
	// print all search settings
	string message = ((Melkor*)melkor)->printSearchSettings();
	return gtp_success(id, message.c_str());
}

// Report the GTP version be used
int GTPFunctions::gtp_protocol_version(char *s, int id)
{
	gtp_pre_command();
	return gtp_success(id, "2");
}

// Quit this GTP session
int GTPFunctions::gtp_quit(char *s, int id)
{
	gtp_pre_command();
	gtp_success(id, "");
	return GTP_QUIT;
}

// This version of genmove is used for regression testing when
// you might not want random elements (used to vary actual play)
// to interfere with testing...
// NOTE: reg_genmove should not add the move generated to the board!
int GTPFunctions::gtp_reg_genmove(char *s, int id)
{
	gtp_pre_command();
	int colour;
	if(!gtp_decode_colour(s, &colour))
		return gtp_failure(id, "Invalid colour - use reg_genmove [b|w] ['print' (for line of play)]");

	// extract printLineOfPlay argument
	bool printLineOfPlay = false;
	char args[50];
	args[0] = '\0';
	sscanf(s, "%*s %s", &args);
	if(strcmp(args, "print")==0)
		printLineOfPlay = true;

	Move m;
	// do same as genmove_black
	((Melkor*)melkor)->getMove(m, ((Melkor*)melkor)->getBoard(), colour);

	int j = m.getX();
	int i = m.getY();

	m.setColour(colour);
	m.setBoardSize(((Melkor*)melkor)->getBoard().getSize());
	// NOTE: reg_genmove should not add the move generated to the board!
//	Go::addToMoveList(m, ((Melkor*)melkor)->getBoard());
//	((Melkor*)melkor)->getBoard().addToBoard(m);

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);
	// print best line of play if requested
	if(printLineOfPlay)
	{
		vector<Move> moves;
		gtp_printf(" ");
		((Melkor*)melkor)->getBestLineOfPlay(moves);
		for(int i=0;i<moves.size();i++)
		{
			string moveValue = Move::xyToGoPosValue(
						moves[i].getX(), moves[i].getY(), moves[i].getBoardSize());
			if(moves[i].getColour()==BLACK)
				gtp_printf("B[%s] ", moveValue.c_str());
			else
				gtp_printf("W[%s] ", moveValue.c_str());
		}
		gtp_printf("\n");
	}
	return gtp_finish_response();
}

// reset the board, as though to start another game
int GTPFunctions::gtp_reset(char *s, int id)
{
	gtp_pre_command();
	gtp_internal_set_boardsize(((Melkor*)melkor)->getBoard().getSize());
	Go::resetMoveList();
	((Melkor*)melkor)->getBoard().clear();

	// Reset the sgf tree
	((Melkor*)melkor)->setSGFModified(true);
	SGFGameInfo info(((Melkor*)melkor)->getInternalSGF().getGameInfo());
	((Melkor*)melkor)->getInternalSGF().init(info);

	return gtp_success(id, "");
}

int GTPFunctions::gtp_resetMTDF(char *s, int id)
{
	gtp_pre_command();
	((Melkor*)melkor)->mtdfSearch->init();
	return gtp_success(id, "");
}

/** @brief This function saves the current internal sgf tree to a specified filename. */
int GTPFunctions::gtp_save_sgf(char *s, int id)
{
	gtp_pre_command();

	char filename[200];
	if(s==NULL || strlen(s)>=200 || sscanf(s, "%s", filename)!=1) {
		string message("Usage: save_sgf <filename (max 199 chars)>");
		return gtp_failure(id, message.c_str());
	}
	
	SGFWriter::save(filename, ((Melkor*)melkor)->getInternalSGF().getTree());
	return gtp_success(id, "");
}

int GTPFunctions::gtp_score_point(char *s, int id)
{
	gtp_pre_command();
	int j, i;
	int colour;
	int next = 0;
	
	if(!(next=gtp_decode_move(s, &j, &i, &colour, gtp_boardsize)))
		return gtp_failure(id, "Invalid coordinate or colour, use colour followed by vertex and -o at the end to print hidden node outputs");

	char buffer[50];
	bool printHiddenOutputs = false;
	vector<Matrix<float> >* outputs = NULL;
	if(sscanf(s+next, "%s", buffer)==1)
	{
		if(strcmp(buffer, "-o")==0)
		{
			printHiddenOutputs = true;
			outputs = new vector<Matrix<float> >;
		}
	}

	gtp_printid(id, GTP_SUCCESS);
	float score = Melkor::getScoredMove(j, i, ((Melkor*)melkor)->getBoard(), colour, *((Melkor*)melkor)->getBPN(),
		outputs);

	if(printHiddenOutputs) {
		BPNGoAdapterInterface* bpn = ((Melkor*)melkor)->bpn;
		const vector<Matrix<float> >& weights = bpn->getBPN().getWeights();
		const int hiddenNodes = weights[weights.size()-1].getHeight();
		string message;
		for(int i=0;i<hiddenNodes;i++) {
			float hiddenOutput = (*outputs)[outputs->size()-2].getValue(0, i)*weights[weights.size()-1].getValue(0, i);
			sprintf(buffer, "%d:%g ", i, hiddenOutput);
			message+=buffer;
		}
		message+="\n";
		gtp_printf(message.c_str());
	}

	gtp_printf("Score = %f", score);
	return gtp_finish_response();
}

int GTPFunctions::gtp_internal_score_all_points(char*s, int id, bool printBoard, bool onlyLegalMoves)
{
	int colour;

	if(!gtp_decode_colour(s, &colour))
		return gtp_failure(id, "Invalid colour");

	gtp_printid(id, GTP_SUCCESS);

	// step through 
	float score;
	int t;
	string message = "";
	string out = "";
	char buffer[50];
	if(printBoard)
		message+="\n";
	vector<Move> scores;
	Move c;
	int i;
	for(i=0;i<((Melkor*)melkor)->getBoard().getSize();i++)
	{
		for(int j=0;j<((Melkor*)melkor)->getBoard().getSize();j++)
		{
			if(onlyLegalMoves && !Go::isLegal(((Melkor*)melkor)->getBoard(), j, i, colour, Go::getUseSuperKo()))
			{
				if(printBoard)
				{
					if(((Melkor*)melkor)->getBoard().getPoint(j, i)==BLACK)
						message+="b ";
					else if(((Melkor*)melkor)->getBoard().getPoint(j, i)==WHITE)
						message+="w ";
				}
				continue;
			}
			else
				score = Melkor::getScoredMove(j, i, ((Melkor*)melkor)->getBoard(), colour, *((Melkor*)melkor)->getBPN());
			// add to list
			c.setX(j);
			c.setY(i);
			c.setColour(colour);
			c.setNNScore(score);
			scores.push_back(c);
			if(printBoard)
			{
				t = (int)(score*10);
				sprintf(buffer, "%i", i);
				message+= buffer;
				//if(((Melkor*)melkor)->getBoard().getPoint(j, i)==EMPTY)
				//	LogWriter::printerr(message);
			}
		}
		if(printBoard)
			message+="\n";
	}
	if(printBoard)
		LogWriter::printerr(message);

	// sort the list and then print
	sort(scores.begin(), scores.end());
	// the above sorts it lowest first
	// we need it highest first
	reverse(scores.begin(), scores.end());

	for(i=0;i<scores.size();i++)
	{
		gtp_print_vertex(scores[i].getX(), scores[i].getY(), gtp_boardsize);
		gtp_printf(":%f ", scores[i].getNNScore());
	}

	//gtp_printf("Score = %f", score);
	return gtp_finish_response();
}

int GTPFunctions::gtp_score_all_points_and_print_board(char* s, int id)
{
	gtp_pre_command();
	return gtp_internal_score_all_points(s, id, true, true);
}

int GTPFunctions::gtp_score_all_points(char *s, int id)
{
	gtp_pre_command();
	return gtp_internal_score_all_points(s, id, false, true);
}

int GTPFunctions::gtp_score_all_points_and_illegal(char *s, int id)
{
	gtp_pre_command();
	return gtp_internal_score_all_points(s, id, false, false);
}

// Set the boardsize GTP should report we are using and resize and clear the board too
int GTPFunctions::gtp_set_boardsize(char *s, int id)
{
	gtp_pre_command();
	int boardsize;
	if(sscanf(s, "%d", &boardsize)==EOF)
		return gtp_failure(id, "boardsize not an integer");

	if(boardsize < 9 || boardsize > 19)
		return gtp_failure(id, "unacceptable boardsize");

	((Melkor*)melkor)->setBoardSize(boardsize);

	return gtp_success(id, "");
}

/** @brief Set the black player name for the internal SGF tree. */
int GTPFunctions::gtp_set_black_name(char *s, int id)
{
	gtp_pre_command();
	char playerName[2000];
	sscanf(s, "%s", playerName);

	// Update the SGF details
	((Melkor*)melkor)->setBlackName(playerName);

	return gtp_success(id, "");
}

/** @brief This gtp command specifies a list of vertices where handicap
  * stones must be placed. */
int GTPFunctions::gtp_set_free_handicap(char *s, int id)
{
	// 1 argument - A list of vertices where handicap stones have been placed.

	// fail if board not empty or bad vertex list
	vector<Move> stones;
	int j, i;
	
	char* currentS = s;

	int sLength = strlen(s);
	   
	int charsRead;
	int currentLength = 0;
	while(currentLength<sLength && (charsRead=gtp_decode_coord(currentS, &j, &i, gtp_boardsize))>0)
	{
		stones.push_back(Move(j, i, BLACK, gtp_boardsize));
		for(int p=0;p<charsRead;p++)
			currentS++;
		currentLength+=charsRead;
	}

	Go::setHandicap(stones.size());

	// add vertices to board as BLACK handicap stones 
	// i.e. Go::addHandicapMove(x, y, ((Melkor*)melkor)->getBoard())
	for(int c=0;c<stones.size();c++)
		Go::addHandicapMove(stones[c].getX(), stones[c].getY(), ((Melkor*)melkor)->getBoard());

	/** @todo How do we specify a free handicap in the SGF game info? */
	((Melkor*)melkor)->setSGFModified(true);
	SGFGameInfo info(((Melkor*)melkor)->getInternalSGF().getGameInfo());
	info.handicap = stones.size();
	((Melkor*)melkor)->getInternalSGF().clear();
	((Melkor*)melkor)->getInternalSGF().setGameInfo(info);
	((Melkor*)melkor)->getInternalSGF().addSetupNode(((Melkor*)melkor)->getBoard());

	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_komi(char *s, int id)
{
	gtp_pre_command();
	float komi;
	if (sscanf(s, "%f", &komi)==EOF)
		return gtp_failure(id, "komi not a float");

	((Melkor*)melkor)->setKomi(komi);

	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_goal(char *s, int id)
{
	gtp_pre_command();

	// Must intialise goal with a dummy goal to start with
	// which will get replaced during parseGoalString
	Goal goal(new StoneCountGoal());
	string errorString;
	if(!parseGoalString(s, goal, errorString, ((Melkor*)melkor)->getBoard())) {
		string syntax = " Use set_goal ";
		syntax+=Goal::getGoalStringParameterSyntax();
		syntax+="\n";
		errorString+=syntax;
		return gtp_failure(id, errorString.c_str());
	}

	((Melkor*)melkor)->setSearchGoal(goal);

	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_pass_value(char *s, int id)
{
	gtp_pre_command();
	float p = 0.0f;

	/** @todo make this work for GA when it is the search algorithm of choice */
	if(sscanf(s, "%f", &p)==1)
	{
		// check threshold is between 0.0 and 1.0 inclusive
		if(p<0.0 || p>1.0)
			return gtp_failure(id, "Invalid argument, threshold should be between 0.0 and 1.0");
		((Melkor*)melkor)->mtdfSearch->setPassValue(p);
	}
	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_play_timed_game(char *s, int id)
{
	gtp_pre_command();

	int p;
	if(sscanf(s, "%d", &p)==1) {
		if(p==1)
			((Melkor*)melkor)->setPlayTimedGame(true);
		else
			((Melkor*)melkor)->setPlayTimedGame(false);
	}
	else
		return gtp_failure(id, "Invalid argument, use set_play_timed_game [1=true, other value=false]");

	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_pruning_settings(char *s, int id)
{
	gtp_pre_command();
	int percent = 100;
	float p = 0.0f;

	// read in user arguments
	// e.g. set_pruning_settings 10% 
	// or   set_pruning_settings 0.35

	if(sscanf(s, "%f", &p)==1)
	{
		// check threshold is between 0.0 and 1.0 inclusive
		if(p<0.0 || p>1.0)
			return gtp_failure(id, "Invalid argument, threshold should be between 0.0 and 1.0");
		((Melkor*)melkor)->mtdfSearch->setUseMoveQualityThreshold(true);
		((Melkor*)melkor)->mtdfSearch->setPruneValue(p);
	}
	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_search_depth(char *s, int id)
{
	gtp_pre_command();
	int depth = atoi(s);
	if(depth<1 || depth>100)
		return gtp_failure(id, "Invalid argument, use set_search_depth [1-100]");
	((Melkor*)melkor)->mtdfSearch->setDepth(depth);
	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_time_per_move(char* s, int id)
{
	gtp_pre_command();
	int t;
	if(sscanf(s, "%d", &t)==1) {
		if(t<0)
			return gtp_failure(id, "Invalid argument, use set_time_per_move [x>=0 in seconds]");
		((Melkor*)melkor)->setTimePerMove(t);
	}
	else
		return gtp_failure(id, "Invalid argument, use set_time_per_move [x>=0 in seconds]");
	
	return gtp_success(id, "");
}

int GTPFunctions::gtp_set_useNChildren(char *s, int id)
{
	gtp_pre_command();
	int percent = 100;
	char b;

	// attempt to read in a percentage (a decimal followed by a '%')
	if(sscanf(s, "%d%c", &percent, &b)==2 && b=='%') {
		// check percent is between 0 and 100 inclusive
		if(percent<0 || percent>100)
			return gtp_failure(id, "Invalid argument, % should be between 0 and 100");
		((Melkor*)melkor)->mtdfSearch->setUseNChildren(0);
		((Melkor*)melkor)->mtdfSearch->setUseNChildrenPercent((float)percent/(float)100);
	}
	// read in whole number of moves to be using at each node expansion
	else if(sscanf(s, "%d", &percent)==1) {
		if(percent<1)
			return gtp_failure(id, "set_useNChildren whole number argument should be greater than 0");
		((Melkor*)melkor)->mtdfSearch->setUseNChildren(percent);
		((Melkor*)melkor)->mtdfSearch->setUseNChildrenPercent(0.0f);
	}	
	else 
		return gtp_failure(id, "Invalid arguments, use set_useNChildren [percentage%%|decimal]");
	
	return gtp_success(id, "");
}

/** @brief Set the white player name for the internal SGF tree. */
int GTPFunctions::gtp_set_white_name(char *s, int id)
{
	gtp_pre_command();
	char playerName[2000];
	sscanf(s, "%s", playerName);

	// Update the SGF details
	((Melkor*)melkor)->setWhiteName(playerName);

	return gtp_success(id, "");
}

// print the board to stderr
int GTPFunctions::gtp_showboard(char *s, int id)
{
	gtp_pre_command();
	// with or without margins, and ostream
	((Melkor*)melkor)->getBoard().printAsciiBoard(true);
	return gtp_success(id, "");
}

int GTPFunctions::gtp_string_data(char *s, int id)
{
	gtp_pre_command();
	int j, i;
	if(!gtp_decode_coord(s, &j, &i, gtp_boardsize))
		return gtp_failure(id, "Invalid coordinate");
	if(((Melkor*)melkor)->getBoard().getPoint(j, i)==EMPTY)
		return gtp_failure(id, "No string at that point");
	((Melkor*)melkor)->getBoard().getStringConst(j, i)->printInfo();
	return gtp_success(id, "");
}

// just print a move vertex, don't add it to the board
int GTPFunctions::gtp_suggest_black(char *s, int id)
{
	gtp_pre_command();
	// extract printLineOfPlay argument
	bool printLineOfPlay = false;
	char args[50];
	args[0] = '\0';
	sscanf(s, "%s", &args);
	if(strcmp(args, "print")==0)
		printLineOfPlay = true;

	Move m;
	((Melkor*)melkor)->getMove(m, ((Melkor*)melkor)->getBoard(), BLACK);

	int j = m.getX();
	int i = m.getY();

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);

	// print best line of play if requested
	if(printLineOfPlay)
	{
		vector<Move> moves;
		((Melkor*)melkor)->getBestLineOfPlay(moves);
		gtp_printf("\n");
		for(int i=0;i<moves.size();i++)
		{
			string moveValue = Move::xyToGoPosValue(
						moves[i].getX(), moves[i].getY(), moves[i].getBoardSize());
			if(moves[i].getColour()==BLACK)
				gtp_printf("B[%s] ", moveValue.c_str());
			else
				gtp_printf("W[%s] ", moveValue.c_str());
		}
		gtp_printf("\n");
	}
	return gtp_finish_response();
}

// just print a move vertex, don't add it to the board
int GTPFunctions::gtp_suggest_white(char *s, int id)
{
	gtp_pre_command();
	// extract printLineOfPlay argument
	bool printLineOfPlay = false;
	char args[50];
	args[0] = '\0';
	sscanf(s, "%s", &args);
	if(strcmp(args, "print")==0)
		printLineOfPlay = true;

	Move m;
	((Melkor*)melkor)->getMove(m, ((Melkor*)melkor)->getBoard(), WHITE);

	int j = m.getX();
	int i = m.getY();

	gtp_printid(id, GTP_SUCCESS);
	gtp_print_vertex(j, i, gtp_boardsize);

	// print best line of play if requested
	if(printLineOfPlay)
	{
		vector<Move> moves;
		((Melkor*)melkor)->getBestLineOfPlay(moves);
		for(int i=0;i<moves.size();i++)
		{
			string moveValue = Move::xyToGoPosValue(
						moves[i].getX(), moves[i].getY(), moves[i].getBoardSize());
			if(moves[i].getColour()==BLACK)
				gtp_printf("B[%s] ", moveValue.c_str());
			else
				gtp_printf("W[%s] ", moveValue.c_str());
		}
		gtp_printf("\n");
	}
	return gtp_finish_response();
}

/** @brief Set how much time is left for a particular colour. */
int GTPFunctions::gtp_time_left(char* s, int id)
{
	gtp_pre_command();

	// Extract the colour argument
	int colour, count;
	if(!(count=gtp_decode_colour(s, &colour)))
		return gtp_failure(id, "Invalid colour - use time_left [b|w] time byo_yomi_stones");

	// Eat count characters from the front of s
	char* newS = s+count;

	int mt = 0;
	int bt = 0;
	int bs = 0;
	if(sscanf(newS, "%d %d", &mt, &bs)==2) {
		// While there is still main time left, bs will be 0
		// after that the time represents the byo-yomi period
		if(bs==0) {
			if(colour==BLACK) {
				bt = Go::getCurrentByoyomiTimeBlack();
				bs = Go::getCurrentByoyomiStonesBlack();
			}
			else { 
				bt = Go::getCurrentByoyomiTimeWhite();
				bs = Go::getCurrentByoyomiStonesWhite();
			}
		}
		else {
			bt = mt;
			mt = 0;
		}
		((Melkor*)melkor)->setCurrentTimeSettings(colour, mt, bt, bs);
	}
	else
		return gtp_failure(id, "Invalid or not enough arguments, use time_left colour time byo_yomi_stones");

	return gtp_success(id, "");
}

/** @brief Set the general time limits used by both colours. */
int GTPFunctions::gtp_time_settings(char* s, int id)
{
	gtp_pre_command();
	int mt, bt, bs;
	if(sscanf(s, "%d %d %d", &mt, &bt, &bs)==3)
		((Melkor*)melkor)->setTimeSettings(mt, bt, bs);
	else
		return gtp_failure(id, "Invalid or not enough arguments, use time_settings main_time byo_yomi_time byo_yomi_stones");

	return gtp_success(id, "");
}

// undo last move, remove from board, rewind GoStrings,
// remove from game record etc...
// - perhaps this can be done by wiping last move
// then reset board and stepping through all moves again
// to avoid complex problems with GoString updating?
int GTPFunctions::gtp_undo(char *s, int id)
{
	gtp_pre_command();

	// changing the board invalidates the SGF stepthrough
	((Melkor*)melkor)->setSGFModified(true);
	// Remove the last node added to the sgf
	((Melkor*)melkor)->getInternalSGF().removeNode();
	
	Go::undo(((Melkor*)melkor)->getBoard());
	return gtp_success(id, "");
}

int GTPFunctions::gtp_update_strings_status(char *s, int id)
{
	gtp_pre_command();

	// get nextToPlay
	int nextToPlay;
	if(!gtp_decode_colour(s, &nextToPlay))
		return gtp_failure(id, "Invalid colour - use update_strings_status <nextToPlay>");

	BoardStruct& board = ((Melkor*)melkor)->getBoard();

	// Save time limit
	double timePerMove = ((Melkor*)melkor)->getTimePerMove();
	((Melkor*)melkor)->setTimePerMove(0.0);

	((Melkor*)melkor)->updateStringsStatus(board, nextToPlay, NULL, NULL,
		((Melkor*)melkor)->getMaxStatusDepth(), NULL);

	// Restore time limit
	((Melkor*)melkor)->setTimePerMove(timePerMove);

	gtp_printid(id, GTP_SUCCESS);

	const BoardStruct::stringTable& strings = board.getStrings();
	BoardStruct::constStringsIterator citer(strings.begin());
	for(;citer!=strings.end();citer++)
	{
		gtp_printf("String at {%s} status = ", 
			Move::xyToGoPosValue(citer->getPoints().begin()->x, citer->getPoints().begin()->y,
				board.getSize()).c_str());
		if(citer->getStatus()==ALIVE)
			gtp_printf("ALIVE\n");
		else if(citer->getStatus()==DEAD)
			gtp_printf("DEAD\n");
		else if(citer->getStatus()==URGENT)
			gtp_printf("URGENT\n");
		else
			gtp_printf("UNKNOWN (%d)\n", citer->getStatus());
	}

	return gtp_finish_response();
}

// print the version of melkor that is running
int GTPFunctions::gtp_version(char *s, int id)
{
	gtp_pre_command();
	return gtp_success(id, Melkor::getVersionString().c_str());
}

// functions borrowed from GNUGo 3.0.0 gtp.c and gtp.h
// protocol info taken from gnugo.ps from gnugo 3.0.0

/** Read stdin linewise and interpret as GTP commands. 
  * @param filename An optional parameter. If specified the function
  * will read GTP commands from the file. */
void GTPFunctions::gtp_main_loop(const char* filename /*=NULL*/)
{
  char line[GTP_BUFSIZE];
  char command[GTP_BUFSIZE];
  char *p;
  int id;
  int n;
  int status = GTP_OK;

  bool readScript = false;
  ifstream inScript;

  if(filename != NULL)
  {
	  // Load the file
	inScript.open(filename);

	if(!inScript)
	{
		string message = "GTP: Could not open file for save: ";
		message+=filename;
		LogWriter::printerr(message+"\n");
		return;
	}
	readScript = true;
  }
  
  while (status == GTP_OK) {
    /* Read a line from stdin. */
	if(readScript)
	{
		// Get a line from the file
		inScript.getline(line, GTP_BUFSIZE);

		// If EOF start reading from STDIN
		if(inScript.eof())
		{
			inScript.close();
			readScript = false;
			if (!fgets(line, GTP_BUFSIZE, stdin))
				break; /* EOF or some error */
		}
	}
	else
	{
		if (!fgets(line, GTP_BUFSIZE, stdin))
			break; /* EOF or some error */
	}

    /* Remove comments. */
    if ((p = strchr(line, '#')) != NULL)
      *p = 0;
    
    p = line;

    /* Look for an identification number. */
    if (sscanf(p, "%d%n", &id, &n) == 1)
      p += n;
    else
      id = -1; /* No identification number. */

    /* Look for command name. */
    if (sscanf(p, " %s %n", command, &n) < 1)
      continue; /* Whitespace only on this line, ignore. */
    p += n;

    /* Search the list of commands and call the corresponding function
     * if it's found.
     */
	if(strcmp(command, "all_legal") == 0) status = gtp_all_legal(p, id);
	else if(strcmp(command, "backsgf") == 0) status = gtp_backsgf(p, id);
	else if(strcmp(command, "black") == 0) status = gtp_playblack(p, id);
	else if(strcmp(command, "boardsize") == 0) status = gtp_set_boardsize(p, id);
	else if(strcmp(command, "capture_string") == 0) status = gtp_capture_string(p, id);
	else if(strcmp(command, "clear_board") == 0) status = gtp_clear_board(p, id);
	else if(strcmp(command, "clear_search_history") == 0) status = gtp_clear_search_history(p, id);
	else if(strcmp(command, "cputime") == 0) status = gtp_cputime(p, id);
	else if(strcmp(command, "empty") == 0) status = gtp_playempty(p, id);
	else if(strcmp(command, "estimate_score") == 0) status = gtp_estimate_score(p, id);
	else if(strcmp(command, "eval") == 0) status = gtp_eval(p, id);
	else if(strcmp(command, "final_score") == 0) status = gtp_final_score(p, id);
	else if(strcmp(command, "final_status_list") == 0) status = gtp_final_status_list(p, id);
	else if(strcmp(command, "fixed_handicap") == 0) status = gtp_fixed_handicap(p, id);
	else if(strcmp(command, "genmove") == 0) status = gtp_genmove(p, id);
	else if(strcmp(command, "genmove_black") == 0) status = gtp_genmove_black(p, id);
	else if(strcmp(command, "genmove_white") == 0) status = gtp_genmove_white(p, id);
	else if(strcmp(command, "get_random_seed") == 0) status = gtp_get_random_seed(p, id);
	else if(strcmp(command, "help") == 0) status = gtp_help(p, id);
	else if(strcmp(command, "influence_map") == 0) status = gtp_influence_map(p, id);
	else if(strcmp(command, "is_legal") == 0) status = gtp_is_legal(p, id);
	else if(strcmp(command, "kgs-genmove_cleanup") == 0) status = gtp_kgs_genmove_cleanup(p, id);
	else if(strcmp(command, "komi") == 0) status = gtp_set_komi(p, id);
	else if(strcmp(command, "known_command") == 0) status = gtp_known_command(p, id);
	else if(strcmp(command, "liberty_influence_map") == 0) status = gtp_liberty_influence_map(p, id);
	else if(strcmp(command, "list_big_moves") == 0) status = gtp_list_big_moves(p, id);
	else if(strcmp(command, "list_commands") == 0) status = gtp_list_commands(p, id);
	else if(strcmp(command, "list_strings_status") == 0) status = gtp_list_strings_status(p, id);
	else if(strcmp(command, "loadsgf") == 0) status = gtp_loadsgf(p, id);
	else if(strcmp(command, "loadnetwork") == 0) status = gtp_loadnetwork(p, id);
	else if(strcmp(command, "menu") == 0) status = gtp_menu(p, id);
	else if(strcmp(command, "mtdf_search") == 0) status = gtp_mtdf_search(p, id);
	else if(strcmp(command, "name") == 0) status = gtp_name(p, id);
	else if(strcmp(command, "network_suggest_black") == 0) status = gtp_network_suggest_black(p, id);
	else if(strcmp(command, "network_suggest_white") == 0) status = gtp_network_suggest_white(p, id);
	else if(strcmp(command, "nextsgf") == 0) status = gtp_nextsgf(p, id);
	else if(strcmp(command, "place_free_handicap") == 0) status = gtp_place_free_handicap(p, id);
	else if(strcmp(command, "play") == 0) status = gtp_play(p, id);
	else if(strcmp(command, "print_search_settings") == 0) status = gtp_print_search_settings(p, id);
	else if(strcmp(command, "protocol_version") == 0) status = gtp_protocol_version(p, id);
	else if(strcmp(command, "quit") == 0) status = gtp_quit(p, id);
	else if(strcmp(command, "reg_genmove") == 0) status = gtp_reg_genmove(p, id);
	else if(strcmp(command, "reset") == 0) status = gtp_reset(p, id);
	else if(strcmp(command, "reset_MTDF") == 0) status = gtp_resetMTDF(p, id);
	else if(strcmp(command, "save_sgf") == 0) status = gtp_save_sgf(p, id);
	else if(strcmp(command, "score_point") == 0) status = gtp_score_point(p, id);
	else if(strcmp(command, "score_all_points_and_print_board") == 0) status = gtp_score_all_points_and_print_board(p, id);
	else if(strcmp(command, "score_all_points") == 0) status = gtp_score_all_points(p, id);
	else if(strcmp(command, "score_all_points_and_illegal") == 0) status = gtp_score_all_points_and_illegal(p, id);
	else if(strcmp(command, "set_black_name") == 0) status = gtp_set_black_name(p, id);
	else if(strcmp(command, "set_goal") == 0) status = gtp_set_goal(p, id);
	else if(strcmp(command, "set_free_handicap") == 0) status = gtp_set_free_handicap(p, id);
	else if(strcmp(command, "set_pass_value") == 0) status = gtp_set_pass_value(p, id);
	else if(strcmp(command, "set_play_timed_game") == 0) status = gtp_set_play_timed_game(p, id);
	else if(strcmp(command, "set_pruning_settings") == 0) status = gtp_set_pruning_settings(p, id);
	else if(strcmp(command, "set_search_depth") == 0) status = gtp_set_search_depth(p, id);
	else if(strcmp(command, "set_time_per_move") == 0) status = gtp_set_time_per_move(p, id);
	else if(strcmp(command, "set_useNChildren") == 0) status = gtp_set_useNChildren(p, id);
	else if(strcmp(command, "set_white_name") == 0) status = gtp_set_white_name(p, id);
	else if(strcmp(command, "showboard") == 0) status = gtp_showboard(p, id);
	else if(strcmp(command, "string_data") == 0) status = gtp_string_data(p, id);
	else if(strcmp(command, "suggest_black") == 0) status = gtp_suggest_black(p, id);
	else if(strcmp(command, "suggest_white") == 0) status = gtp_suggest_white(p, id);
	else if(strcmp(command, "time_left") == 0) status = gtp_time_left(p, id);
	else if(strcmp(command, "time_settings") == 0) status = gtp_time_settings(p, id);
	else if(strcmp(command, "undo") == 0) status = gtp_undo(p, id);
	else if(strcmp(command, "update_strings_status") == 0) status = gtp_update_strings_status(p, id);
	else if(strcmp(command, "version") == 0) status = gtp_version(p, id);
	else if(strcmp(command, "white") == 0) status = gtp_playwhite(p, id);
	else gtp_failure(id, "unknown command: '%s'", command);

    if (status == GTP_FATAL)
      gtp_panic();
  }
}

// NOTE: Compiling a native GUI with MFC is deprecated in favour of creating a DLL version
//       of Melkor and running through a VB interface. See MelkorVBGUI project.
/*#ifdef _MFC_VER
#error need to add gtp_threadWriteResponse() (was gtp_write) and gtp_threadReadResponse() (gtp_read())
#error from NeuralGo project, GTPFunctions.cpp
#error should be the same but gtp_threadWriteResponse() should not do a gtp_read() at the
#error end since we're only responding to a command sent to us
#endif
*/
/* This currently works exactly like printf. */
void __cdecl GTPFunctions::gtp_printf(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  char buffer[5000];
  int nChars = vsprintf(buffer, format, ap);
  lastOutput+=buffer;
//  vfprintf(stdout, format, ap);
#ifndef _MFC_VER
  // write to stdout if we're _not_ in MelkorGUI
  fwrite(buffer, 1, nChars, stdout);
#endif
  va_end(ap);
}

/* Write success or failure indication plus identity number if one was
 * given.
 */
void GTPFunctions::gtp_printid(int id, int status)
{
  if (status == GTP_SUCCESS)
    gtp_printf("=");
  else
    gtp_printf("?");
  
  if (id < 0)
    gtp_printf(" ");
  else
    gtp_printf("%d ", id);
}

/* Finish a GTP response by writing a double newline and returning GTP_OK. */
int GTPFunctions::gtp_finish_response()
{
  gtp_printf("\n\n");
  return GTP_OK;
}

/* Write a full success response. Except for the id number, the call
 * is just like one to printf.
 */
int __cdecl GTPFunctions::gtp_success(int id, const char *format, ...)
{
  va_list ap;
  gtp_printid(id, GTP_SUCCESS);
  va_start(ap, format);
  char buffer[5000];
  int nChars = vsprintf(buffer, format, ap);
  lastOutput+=buffer;
//  vfprintf(stdout, format, ap);
//#ifdef _MFC_VER
  // if we're communicating with another program whilst
  // being in MelkorGUI we must use gtp_threadWrite()
  // to tell the other program our response
//  if(programRunning)
//  {
	  // add gtp_finish_response
//	  lastOutput+="\n\n";
//	  gtp_threadWriteResponse(lastOutput);
//  }
//#else
  // write to stdout if we're _not_ in MelkorGUI
  fwrite(buffer, 1, nChars, stdout);
//#endif
  va_end(ap);
  return gtp_finish_response();
}

/* Write a full failure response. The call is identical to gtp_success. */
int __cdecl GTPFunctions::gtp_failure(int id, const char *format, ...)
{
  va_list ap;
  gtp_printid(id, GTP_FAILURE);
  va_start(ap, format);
  char buffer[5000];
  int nChars = vsprintf(buffer, format, ap);
  lastOutput+=buffer;
//  vfprintf(stdout, format, ap);
//#ifdef _MFC_VER
  // if we're communicating with another program whilst
  // being in MelkorGUI we must use gtp_threadWrite()
  // to tell the other program our response
//  if(programRunning)
//  {
	  // add gtp_finish_response
//	  lastOutput+="\n\n";
//	  gtp_threadWriteResponse(lastOutput);
//  }
//#else
  // write to stdout if we're _not_ in MelkorGUI
  fwrite(buffer, 1, nChars, stdout);
//#endif
  va_end(ap);
  return gtp_finish_response();
}

/* Write a panic message. */
void GTPFunctions::gtp_panic()
{
  gtp_printf("! panic\n\n");
}

/* Convert a string describing a color, "b", "black", "w", or "white",
 * to Melkor's integer representation of colors. Return the number of
 * characters read from the string s.
 */
int GTPFunctions::gtp_decode_colour(char *s, int *color)
{
  char color_string[7];
  int i;
  int n;

  if (sscanf(s, "%6s%n", color_string, &n) != 1)
    return 0;

  for (i = 0; i < (int) strlen(color_string); i++)
    color_string[i] = tolower((int) color_string[i]);

  if (strcmp(color_string, "b") == 0
      || strcmp(color_string, "black") == 0)
    *color = BLACK;
  else if (strcmp(color_string, "w") == 0
	   || strcmp(color_string, "white") == 0)
    *color = WHITE;
  else
    return 0;
  
  return n;
}

/* Convert an intersection given by a string to two coordinates
 * according to Melkor's convention. Return the number of characters
 * read from the string s.
 */
int GTPFunctions::gtp_decode_coord(char *s, int *j, int *i, int gtp_boardsize)
{
  char column;
  int row;
  int n;

  if (sscanf(s, " %c%d%n", &column, &row, &n) != 2)
    return 0;
  
  if (tolower((int) column) == 'i')
    return 0;
  *j = tolower((int) column) - 'a';
  if (tolower((int) column) > 'i')
    --*j;

  *i = gtp_boardsize - row;

  if (*i < 0 || *i >= gtp_boardsize || *j < 0 || *j >= gtp_boardsize)
    return 0;

  return n;
}

/* Convert a move, i.e. "b" or "w" followed by a vertex to a color and
 * coordinates. Return the number of characters read from the string
 * s.
 */
int GTPFunctions::gtp_decode_move(char *s, int *j, int *i, int *color, int gtp_boardsize)
{
  int n1, n2;

  n1 = gtp_decode_colour(s, color);
  if (n1 == 0)
    return 0;

  n2 = gtp_decode_coord(s + n1, j, i, gtp_boardsize);
  if (n2 == 0)
    return 0;

  return n1 + n2;
}

/* This a bubble sort. Given the expected size of the sets to
 * sort, it's probably not worth the overhead to set up a call to
 * qsort.
 */
void GTPFunctions::sort_moves(int n, int movei[], int movej[])
{
  int b, a;
  for (b = n-1; b > 0; b--) {
    for (a = 0; a < b; a++) {
      if (movei[a] > movei[b]
	  || (movei[a] == movei[b] && movej[a] > movej[b])) {
	int tmp;
	tmp = movei[b];
	movei[b] = movei[a];
	movei[a] = tmp;
	tmp = movej[b];
	movej[b] = movej[a];
	movej[a] = tmp;
      }
    }
  }
}

/* Write a number of space separated vertices. The moves are sorted
 * before being written.
 */
void GTPFunctions::gtp_print_vertices(int n, int movej[], int movei[], int gtp_boardsize)
{
  int k;
  
 string res;
 sort_moves(n, movei, movej);
  for (k = 0; k < n; k++) {
    if (k > 0)
      gtp_printf(" ");
    if (movei[k] == -1 && movej[k] == -1)
      gtp_printf("PASS");
    else if (movei[k] < 0 || movei[k] >= gtp_boardsize
	     || movej[k] < 0 || movej[k] >= gtp_boardsize)
      gtp_printf("??");
    else
	{
	  gtp_internal_print_vertex(movej[k], movei[k], res, gtp_boardsize);
	  gtp_printf(res.c_str());
      //gtp_printf("%c%d", 'A' + movej[k] + (movej[k] >= 8),
		// gtp_boardsize - movei[k]);
	}
  }
}

void GTPFunctions::gtp_internal_print_vertex(int j, int i, string& res, int gtp_boardsize)
{
	res = "";
	res+=(char)('A'+j+(j>=8));
	char buffer[50];
	int diff = gtp_boardsize-i;
	sprintf(buffer, "%d", diff);
	res+=buffer;
}

/* Write a single move, converted from internal x,y format to 
GTP A19 format. */
void GTPFunctions::gtp_print_vertex(int j, int i, int gtp_boardsize)
{
  gtp_print_vertices(1, &j, &i, gtp_boardsize);
}
