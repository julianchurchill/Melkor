#pragma warning( disable : 4786 )

#include "MTDFSearch.h"
#include "Go.h"
#include "Node.h"
#include "Melkor.h"
#include "Misc.h"
#include "MoveSuggester.h"
#include "InfluenceGoal.h"
#include <ctime>
#include <string>
#include <LIMITS.H>
#include <stack>
#include <algorithm>
//using namespace std;

/** @todo
  * Add null move pruning as a setting like etc/bestMoveFirst and
  * check performance on GMFTest1.sgf.
  *
  * Note: Killer moves is effectively a subset of the history heuristic
  *  so we don't need both... Prefer HH.
  *
  * Add killer moves. Keep 'n' best moves from each depth (i.e. 1-9)
  * Promote the killer moves during ordering. Keep vector<Move> killerMoves( maxDepth )
  * Legality of killer moves must be checked before use.
  *
  *
  * Add history heuristic. Keep a table of moves. When leaving a node update the history
  * score of the best move. When entering a node sort moves based on their
  * history heuristic score. Score should reflect the search depth.
  *  HH[m] += (1<<depth)    // 2^depth
  * Legality of HH moves must be checked before use.
  * For a move to qualify for entry into the HH table it must be:
  *  1. A move that causes a cutoff.
  *  2. Or the best scoring minimax move at that node.
  */


/** @todo Profile the MTD(f) search to find bottlenecks. 
  *
  * Perhaps these search algorithms could use a simplified version of BoardStruct
  * which would save time when copying?
  */

// See Go::IsLegal for details of illegal move memory
// Basically keeps track of moves that are illegal to save rechecking them everytime
// See use in MTDF::defaultExpand()
// NOTE: This optimisation is useful for game tree when the list of
// illegal moves is stored within the BoardStruct object - as it is atm.
#define USE_ILLEGAL_MOVE_MEMORY

#define MTDFSEARCH_STATS
#ifdef MTDFSEARCH_STATS
//#define CALCULATE_TABLE_SIZE
#endif

const string MTDFSearch::name = "MTDFSearch";
const float MTDFSearch::POSINFINITY = (float)INT_MAX;
const float MTDFSearch::NEGINFINITY = (float)INT_MIN;
const float MTDFSearch::defaultPruneValue = 0.35f;
const float MTDFSearch::defaultPassValue = 0.35f;
const bool MTDFSearch::defaultUseMoveQualityThreshold = true;
const float MTDFSearch::defaultUseNChildrenPercent = 0.0f;
const int MTDFSearch::defaultUseNChildren = 0;
const int MTDFSearch::defaultMaxDepth = 1;
const int MTDFSearch::defaultNextToPlay = BLACK;
const int MTDFSearch::defaultMaxStatusDepth = 5;

// the second argument is a failsafe so we don't get a type mismatch when specifying the void*
MTDFSearch::MTDFSearch(bool debugFlag, void* m, bool useMelkorType) : settings(new InfluenceGoal()), debugFlag(debugFlag)
{
	melkor = m;
	// check morgoth type is Melkor*
	if(((Melkor*)melkor)->getName()!="Melkor") {
		LogWriter::printerr("Invalid Melkor* in MTDFSearch()\n", "");
		exit(0);
	}

	// create transposition table
	transpositionTable = new TRANSPOSITION_TABLE();

	// for statistics
	nodesTraversed = 0;
	nodesEvaluated = 0;

	settings.setUseBestMoveFirst(true);
	settings.setUseEtc(true);
	settings.setUseTranspositionTable(true);
	settings.setUseNullMovePruning(false);
	settings.setUseMoveQualityThreshold(MTDFSearch::defaultUseMoveQualityThreshold);
	settings.setPruneValue(MTDFSearch::defaultPruneValue);
	settings.setPassValue(MTDFSearch::defaultPassValue);
	settings.setUseNChildrenPercent(MTDFSearch::defaultUseNChildrenPercent);
	settings.setUseNChildren(MTDFSearch::defaultUseNChildren);
	settings.setDepth(MTDFSearch::defaultMaxDepth);
	settings.setNextToPlay(MTDFSearch::defaultNextToPlay);

	settings.setIterativeStep(1);
	settings.setMaximisingColour(BLACK);
	settings.setUseIterativeDeepening(true);
	settings.setUseMTDF(true);

	// default = 0, i.e. no time limit
	settings.setTimeLimit(0);
	timeRanOut = false;
}

void MTDFSearch::init()
{
	nodesTraversed = 0;
	nodesEvaluated = 0;
	if(settings.getUseTranspositionTable())
		transpositionTable->clear();
}

/** @brief Given a board position conduct a search and return the best scoring move found.
  *  Note that this function automatically resets the transposition table.
  * @param m A reference to a Move object to put the function result into.
  * @param b The current board position.
  * @param nextToPlay The colour of the player to move next.
  * @param area An optional argument to specify an area of the board to restrict the search to.
  * @return The best move found for this position. */
float MTDFSearch::suggestMove(Move &m, const BoardStruct &board, const int nextToPlay)
{
	// calculate maximising colour according to goal
	int maximisingColour = getGoal().getRequiredMaximisingColour(board, nextToPlay);
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag) {
		LogWriter::printerr("suggestMove called, running for ", getName());
		if(maximisingColour==BLACK)
			LogWriter::printerr("BLACK\n",getName());
		else if(maximisingColour==WHITE)
			LogWriter::printerr("WHITE\n",getName());
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN

	// if this is a captureStringEval make sure that:
	// maximisingColour should always be the captureStringColour because ALIVE is positive (children of maxNodes)
	// and DEAD is -.v.e. (children of minNodes)

	Goal::GoalListType::const_iterator citer(getGoal().getGoals().begin());
	for(;citer!=getGoal().getGoals().end();citer++) {
		if(citer->goal->testFlags(GoalInterface::TARGETED) && 
			 maximisingColour!=board.getPoint(citer->goal->getArg1(), citer->goal->getArg2()))
			throw "MTDFSearch exception: maximisingColour should be == to captureStringColour for capture goals";
	}

	string message;

	timeRanOut = false;

	// work out move given this boardState
	// partition board
	// find important points on board
	// do a local game tree search on the most urgent

	bool inTranspositionTable = false;
	TRANSPOSITION_TABLE::iterator iter;

	Node* root;
	root = new Node(board, nextToPlay);

#ifdef MTDFSEARCH_STATS
	time_t start;
	time_t finish;
	double t;
	time(&start);
#endif
	
	nodesTraversed = 0;
	nodesEvaluated = 0;

	Node lookupNode(*root);

	nodesTraversed = 0;
	nodesEvaluated = 0;
	// reset the tranposition table
	if(getUseTranspositionTable())
		transpositionTable->clear();

	// now do the tree search
	float abScore = doLocalSearch(board, m, root, getDepth(), maximisingColour);

	// now get the new updated version of root from the transposition table
	Node* tempRoot;
	if(getUseTranspositionTable()) {
		if((iter=transpositionTable->find(lookupNode))!=transpositionTable->end())
			tempRoot = &(*iter);
	}

	updateBestLineOfPlay(*tempRoot, board.getSize());

	if(debugFlag) {
#ifdef MTDFSEARCH_STATS
		char buffer[200];
		time(&finish);
#ifdef CALCULATE_TABLE_SIZE
		if(useTranspositionTable) {
			sprintf(buffer, "Final Transposition table size : %d, memory usage = %d bytes\n", transpositionTable->size(), 
				transpositionTable->getMemoryUsage(true));
			LogWriter::printerr(buffer, getName());
		}
#endif
		message = "Total nodes traversed: ";
		sprintf(buffer, "%d", nodesTraversed);
		message+=buffer;
		LogWriter::printerr(message+"\n", getName());
		message = "Total nodes evaluated: ";
		sprintf(buffer, "%d", nodesEvaluated);
		message+=buffer;
		LogWriter::printerr(message+"\n", getName());
		t = difftime(finish, start);
		message = "Time taken: ";
		sprintf(buffer, "%*g", 7, t);
		message+=buffer;
		message+="s";
		LogWriter::printerr(message+"\n", getName());
#endif
	}
	
	// if root was not in table then it will have
	// been copied into it, leaving the root
	// object here redundant, so delete it
	if(getUseTranspositionTable() && !inTranspositionTable) {
		delete root;
		root = NULL;
	}
	else if(!getUseTranspositionTable()) {
		delete root;
		root = NULL;
	}
	return abScore;
}

/** @brief Set all the flags on nodes in the transposition table so that
  * if the nodes are terminal ones their evaluation scores will
  * be recalculated at every search. The nodes themselves are retained
  * between searchs so only the score need be recalculated. */
void MTDFSearch::cleanTranspositionTable()
{
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag)
		LogWriter::printerr("cleanTranspositionTable called \n", getName());
#endif // MTDFSEARCH_LEAN_AND_MEAN
	TRANSPOSITION_TABLE::iterator nodes(transpositionTable->begin());
	while(nodes!=transpositionTable->end())	{
		// This should be called to guarantee a fresh start, for instance if changing goal,
		// in which case none of the scores or bounds will be relevant, nor will terminal 
		// nodes necessarily be terminal for this new goal...
		nodes->setTerminal(false);
		nodes->setScoreCalculated(false);
		nodes->setUpperboundCalculated(false);
		nodes->setLowerboundCalculated(false);
		nodes->setBestNode(NULL);                  // Reset best child memory
		nodes++;
	}
}

/** Start a search from the root node given to the depth specified.
@param max A return parameter to store the best move found.
@param root The root node of the search tree.
@param maxDepth The maximum tree depth to search to.
@param maximisingColour The colour who's point of view is being taken in this search.
@return The best move found in the search tree. */
float MTDFSearch::doLocalSearch(const BoardStruct& board, Move &max, Node* root, const int maxDepth, const int maximisingColour)
{
	float t;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag)
		LogWriter::printerr("doLocalSearch called \n", getName());
#endif // MTDFSEARCH_LEAN_AND_MEAN

	Node copy;
	if(debugFlag)
		copy = *root;

	Node lookupNode(*root);

	t = iterativeDeepening(board, root, maxDepth, maximisingColour);

	// compare root node before and after to make sure
	// no changes have been made
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag)
		if(!(copy==*root))
			LogWriter::printerr("Root node has been changed!", getName());
#endif

	// now get the new updated version of root from the transposition table
	Node* temp = root;
	//set<Node, less<Node> >::iterator iter;
	TRANSPOSITION_TABLE::iterator iter;
	if(getUseTranspositionTable()) {
		if((iter=transpositionTable->find(lookupNode))!=transpositionTable->end())
			temp = &(*iter);
	}

	// failsafe
	if(getBestNode(*temp)==NULL) {
		if(debugFlag)
			LogWriter::printerr("No best node for MTDF tree, returning pass.\n", getName());
		Move::getPassMove(max);
		return t;
	}
	Node* bestNode = getBestNode(*temp);
	max = bestNode->getMove();
	max.setNNScore(bestNode->getNNScore());
	max.setAlphaBetaScore(t);

#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag) {
		string message;
		char buffer[200];

		if(timeRanOut)
			LogWriter::printerr("Time ran out.\n");

		// scan transposition
		LogWriter::printerr("Depth 1 nodes:\n");
		TRANSPOSITION_TABLE::iterator nodes;
		nodes = transpositionTable->begin();
		while(nodes!=transpositionTable->end())
		{
			// select nodes with depth == 1
			if((*nodes).getDepth()==1)
			{
				LogWriter::printerr(" ");
				(*nodes).getMove().printInfo(false);
				sprintf(buffer, "= %f\n", (*nodes).getAlphaBetaScore());
				LogWriter::printerr(buffer);
			}
			nodes++;
		}

		message = "Tree minimax score: ";
		sprintf(buffer, "%*g", 7, t);
		message+=buffer;
		LogWriter::printerr(message+"\n", getName());
		message = "Best move: ";
		sprintf(buffer, "%d", max.getX());
		message+=buffer;
		message+=",";
		sprintf(buffer, "%d", max.getY());
		message+=buffer;
		LogWriter::printerr(message+"\n", getName());
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN
	return t;
}

/** @brief Run a search based on the internal settings of this SearchAlgorithm derived class. 
  * The internal settings can be accessed through the SearchSettings member object called settings. */
float MTDFSearch::runSearchAsSettings(Node* root, const BoardStruct& board, const float firstGuess /*=NOSTATUS*/,
									  time_t* start /*=NULL*/, const int parentDepth /*=0*/)
{
	// Get the terminal goals if there are any
//	Goal::ConstGoalList terminalGoals;
//	getGoal().getGoals(terminalGoals, GoalInterface::TERMINAL);
	bool testForTerminalNodes = false;
//	if(terminalGoals.size()>0)
	if(getGoal().getGoals(GoalInterface::TERMINAL).size()>0)
		testForTerminalNodes = true;

	if(settings.getUseIterativeDeepening())
		return iterativeDeepening(board, root, settings.getDepth(), settings.getMaximisingColour(), 
			settings.getIterativeStep(), firstGuess, parentDepth);
	else if(settings.getUseMTDF())
		return MTDF(board, root, firstGuess, settings.getDepth(), settings.getMaximisingColour(), start,
		  testForTerminalNodes, parentDepth);
	else {
		BoardStruct changeableBoard(board);
		return AlphaBetaWithMemory(changeableBoard, root, (float)NEGINFINITY, (float)POSINFINITY, settings.getDepth(), 
			parentDepth, settings.getMaximisingColour(), true, start, testForTerminalNodes);
	}
}

/** @brief Perform a tree search from the root node increasing the depth
limit after each iteration until the maximum depth is reached.
@param root A preconstructed Node pointer to the intial search position.
@param maxDepth The maximum depth to iterate to.
@param maximisingColour The colour that is trying to maximise its score this search.
@return The minimax score for the root node after searching. */
float MTDFSearch::iterativeDeepening(const BoardStruct& board, Node* root, const int maxDepth, const int maximisingColour,
									 const int step/*=1*/, const float initialFirstGuess /*=NOSTATUS*/,
									 const int parentDepth /*=0*/)
{	
	/* The point of iterative deepening is that it allows us to establish best nodes
	 * at each depth. We use the best nodes from a previous iteration with best node first
	 * technique in alpha-beta to check those nodes first. 
	 * 
	 * In addition iterative deepening allows us to try increasing depths until our
	 * time runs out, making good usage of the time we have available. 
	 * 
	 * @todo Try a step of 2. It should give us a more stable evaluation value, 
	 *  which should improve MTD(f) and speed things up generally.
	 */

	// start the timer
	time_t start, end;
	double t = 0;
	double lastTime = 0;
	bool maximumDepthSearched = false;

#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag)
		LogWriter::printerr("iterativeDeepening called \n", getName());
#endif // MTDFSEARCH_LEAN_AND_MEAN

	// good first guess might be simply running the evaluation function on root
	// and using that score as the first guess??
	float firstguess;
	if(settings.getUseMTDF()) {
		if(initialFirstGuess==NOSTATUS)
			getGoal().evaluate(maximisingColour, firstguess, board, root->getNextToPlay());
		else
			firstguess = initialFirstGuess;
	}
	
	Node lookupNode(*root);

	// Get the terminal goals if there are any
//	Goal::ConstGoalList terminalGoals;
//	getGoal().getGoals(terminalGoals, GoalInterface::TERMINAL);
//	if(terminalGoals.size()>0)

	Goal::GoalListType terminalGoals;
	bool testForTerminalNodes = false;
	if( (terminalGoals = getGoal().getGoals(GoalInterface::TERMINAL)).size()>0)
		testForTerminalNodes = true;

	int currentIDDepth;
	lastIDDepth = 0;
	currentIDDepth = 1;
	while(currentIDDepth <= maxDepth) {
#ifdef _DEBUG
		if(currentIDDepth==10)
			currentIDDepth = 10;
#endif
		if(getTimeLimit()!=0)
			time(&start);

#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			sprintf(buffer, "Iteration depth: %d\n", currentIDDepth);
			LogWriter::printerr(buffer, getName());
			sprintf(buffer, "Firstguess this iteration : %*g\n", 7, firstguess);
			LogWriter::printerr(buffer, getName());
		}
#endif //MTDFSEARCH_LEAN_AND_MEAN

		// Run the MTD(f) search
		if(settings.getUseMTDF())
			firstguess = MTDF(board, root, firstguess, currentIDDepth, maximisingColour, &start, 
							  testForTerminalNodes, parentDepth);
		else {
			BoardStruct newBoard(board);
			firstguess = AlphaBetaWithMemory(newBoard, root, (float)NEGINFINITY, (float)POSINFINITY, currentIDDepth,
											 parentDepth, maximisingColour, true, &start, testForTerminalNodes);
		}
		lastIDDepth = currentIDDepth;

		if(getUseTranspositionTable()) {
			TRANSPOSITION_TABLE::iterator iter;
			if((iter=transpositionTable->find(lookupNode))!=transpositionTable->end())
				root = &(*iter);
		}

#ifdef _DEBUG
		Node* bestNodePtr = getBestNode(*root);
#endif

#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			// now get the new updated version of root from the transposition table
			if(getBestNode(*root)!=NULL)
				LogWriter::printerr("Iterative deepening has root.bestNode != NULL\n", getName());
			sprintf(buffer, "IterativeDeepening: TranspositionTable size: %d\n", transpositionTable->size());
			LogWriter::printerr(buffer, getName());
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN

		if(((Melkor*)melkor)->verbose) {
#ifdef MTDFSEARCH_STATS
			char buffer[200];
			sprintf(buffer, "Depth %d:\n", currentIDDepth);
			LogWriter::printerr(buffer, getName());
#ifdef CALCULATE_TABLE_SIZE
			if(useTranspositionTable) {
				sprintf(buffer, "Transposition table size : %d, memory usage = %d bytes\n", transpositionTable->size(),
					transpositionTable->getMemoryUsage(true));
				LogWriter::printerr(buffer, getName());
			}
#endif
			sprintf(buffer, "Total nodes traversed: %d\n", nodesTraversed);
			LogWriter::printerr(buffer, getName());
			sprintf(buffer, "Total nodes evaluated: %d\n", nodesEvaluated);
			LogWriter::printerr(buffer, getName());
#endif
		}

		// Check if the search should finish now - dependent on whether we have any BINARY goals to achieve
		Goal::GoalListType::const_iterator citer(terminalGoals.begin());

		// To finish completely we must have completed _all_ binary goals
		bool terminalGoalsCompleted = true;
		for(;citer!=terminalGoals.end();citer++) {
			// Deprecated - use getMaxScore() and getMinScore() instead
			// if(firstguess < (*citer)->getSuccessScore())

			// Has this goal _avoided_ reaching a terminal state?
			if(firstguess > citer->goal->getMinScore() && firstguess < citer->goal->getMaxScore()) {
				terminalGoalsCompleted = false;
				break;
			}
		}
		
		// Make sure we actually have binary goals to check!
		if(terminalGoals.size()>0 && terminalGoalsCompleted)
			break;
		
		// time check here, if (time!=0)
		if(getTimeLimit()!=0) {
			if(timeRanOut)
				break;
			time(&end);
			// get time elapsed this round in seconds
			lastTime = difftime(end, start);
			t+=lastTime;
			// also do pre-emptive check using the last generations duration
			if(t>=getTimeLimit() || (t+lastTime)>=getTimeLimit()) {
				timeRanOut = true;
				break;
			}
		}

		if(currentIDDepth==maxDepth)
			maximumDepthSearched = true;

		currentIDDepth+=step;

		// if the maxDepth is even then a step greater than 1 could mean we
		// skip the maximum depth - always search the maximum depth
		if(!maximumDepthSearched && step>1 && currentIDDepth>maxDepth) {
			currentIDDepth = maxDepth;
			maximumDepthSearched = true;
		}

	}

#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag)
	{
		char buffer[200];
		sprintf(buffer, "Firstguess: %*g\n", 7, firstguess);
		LogWriter::printerr(buffer, getName());
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN

	return firstguess;
}

/** The acutal MTD(f) implementation. A minimax search is performed
from the root node given upto a given depth using a 'guessed'
minimax score to estimate upper and lower bounds for the alpha
beta cutoffs.
@param root The root node to start the tree search from.
@param f The guessed minimax score for this node.
@param d The maximum depth to seach to within the tree.
@param maximisingColour The colour who's point of view is being taken in this search.
@param start The time at which the search began.
@return The minimax score for the root node after searching. */
float MTDFSearch::MTDF(const BoardStruct& board, Node* root, const float f, const int d, const int maximisingColour, 
					   time_t* start, const bool testForTerminalNodes, const int parentDepth /*=0*/)
{
	//g := f;
	float g = f;				
	//upperbound := +INFINITY; 
	float upperbound = (float)POSINFINITY;
	//lowerbound := -INFINITY;
	float lowerbound = (float)NEGINFINITY;
	float beta;
	
	BoardStruct newBoard;
	Node lookupNode(*root);
	time_t end;

#ifdef _DEBUG
	int loopCount = 0;
#endif

	//repeat
	do {
		//if g == lowerbound then beta := g + 1 else beta := g;
		if(g==lowerbound) beta = g+1;
		else beta = g;
		//g := AlphaBetaWithMemory(root, beta - 1, beta, d);
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			LogWriter::printerr("Note: First node will be last move played on this board\n", getName());
			sprintf(buffer, "MTDF lowerbound = %g, upperbound = %g\n", lowerbound, upperbound);
			LogWriter::printerr(buffer, getName());
			sprintf(buffer, "MTDF feeding alphaBeta with alpha = %g, beta = %g\n", beta-1, beta);
			LogWriter::printerr(buffer, getName());
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN

		newBoard = board;
		g = AlphaBetaWithMemory(newBoard, root, beta-1, beta, d, parentDepth, maximisingColour, true, start,
								testForTerminalNodes);

		// Update root pointer from ttable
		if(getUseTranspositionTable()) {
			TRANSPOSITION_TABLE::iterator iter;
			if((iter=transpositionTable->find(lookupNode))!=transpositionTable->end())
				root = &(*iter);
		}
		//if g < beta then upperbound := g else lowerbound := g;
		if(g<beta) {
			upperbound = g;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
			if(debugFlag) {
				char buffer[200];
				sprintf(buffer, "MTDF updating upperbound = %g\n", upperbound);
				LogWriter::printerr(buffer, getName());
			}
#endif // MTDFSEARCH_LEAN_AND_MEAN
		}
		else {
			lowerbound = g;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
			if(debugFlag) {
				char buffer[200];
				sprintf(buffer, "MTDF updating lowerbound = %g\n", lowerbound);
				LogWriter::printerr(buffer, getName());
			}
#endif // MTDFSEARCH_LEAN_AND_MEAN
		}

		// time check here, if (time!=0)
		if(getTimeLimit()!=0 && start!=NULL) {
			if(timeRanOut)
				break;
			time(&end);
			double t = difftime(end, *start);
			if(t>=getTimeLimit()) {
				timeRanOut = true;
				break;
			}
		}
#ifdef _DEBUG
		loopCount++;
#endif
		//until lowerbound >= upperbound;
	}while(lowerbound<upperbound);

#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag) {
		string message;
		char buffer[50];
		message = "MTDF returns g: ";
		sprintf(buffer, "%g", g);
		message+=buffer;
		LogWriter::printerr(message+"\n", getName());
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN

	return g;
}

/** An alpha beta minimax search algorithm with memory. The memory
recalls what the previous alpha beta bounds were for nodes that
caused a cutoff. NOTE: This is a non NegaMax version, for our eval functions
i.e. LifeDeathGoal which has a range of -1 (DEAD) to +1000 (ALIVE) and values
in between as the aliveness of the string (liberties), a NegaMax version of 
AlphaBeta would be confusing and complicated.
@param n The root node of the search tree.
@param alpha The alpha bound of the alpha beta cutoff window, the lower bound of the minimax score.
@param beta The beta bound of the alpha beta cutoff window, the upper bound of the minimax score.
@param depthToSearch The maximum depth to search within this subtree.
@param currentDepth The current depth of node n.
@param maximisingColour The colour who's point of view is being taken in this search.
@param rootLevel True if n is a node at the very root of the search tree, i.e. this function
call is not a recursive child.
@param start An optional parameter, the time at which the search began. Use NULL.to ignore.
@return The minimax score of this tree. */
float MTDFSearch::AlphaBetaWithMemory(BoardStruct& board, Node* n, float alpha, float beta, const int depthToSearch,
									  const int currentDepth, const int maximisingColour, const bool rootLevel,
									  time_t* start, const bool testForTerminalNodes)
{
	// Resize our persistent board change saver - only if this is the start of the search though...
	if(rootLevel && oldBoard.size()<(depthToSearch+currentDepth))
		oldBoard.resize(depthToSearch + currentDepth);

	nodesTraversed++;

	n->setDepth(currentDepth);

	float g, a, b;

//	_ASSERTE( _CrtCheckMemory() );

	bool inTranspositionTable = false;
	if(getUseTranspositionTable()) {
		float boundValue;
		BoundType boundType = BoundType::UNSET;
		if( lookupTranspositionTableBounds(boundValue, boundType, n, currentDepth, depthToSearch, alpha, beta) )
		{
			inTranspositionTable = true;
			if( boundType == EXACT_VALUE )
				return boundValue;
			else if( boundType == LOWER_BOUND )
				return boundValue;
			else if( boundType == UPPER_BOUND )
				return boundValue;
			else if( boundType == BoundType::UNSET )
				throw "boundType not set in MTDFSearch::AlphaBetaWithMemory()! ";
			// else if( boundType == BoundType::NO_CUTOFF )  // No cutoff so no returnable bound
		}
		// node n not in the transposition table so add it and re-reference
		else {
			// copy n into ttable
			n = &(transpositionTable->insert(*n));
			inTranspositionTable = true;
		}
	} // if useTranspositionTable
	
//	_ASSERTE( _CrtCheckMemory() );

	time_t end;
	Node lookupNode(*n);

	// Check the selective search extensions
	bool extensionUsed = false;
	if(settings.getSearchExtensions().size()>0) {
		// Find first extensions[i].validExtension()
		SearchSettings::SearchExtensionsList::iterator siter(settings.getSearchExtensions().begin());
		for(;siter!=settings.getSearchExtensions().end();siter++) {
			if((*siter)->validExtension(board, getGoal())) {
				// Set settings to extensions[i].getSettings()
				saveSettings();
				// Copy the settings to a new object first because the nested extensions
				// in 'settings', including this one, will be deleted by =operator
				SearchSettings extensionSettings((*siter)->getSettings());
				settings = extensionSettings;
				// Set next to play correctly since when setting up the extension earlier
				// we won't have known who would be next to play when it's actually used!
				settings.setNextToPlay(n->getNextToPlay());
				// Same goes for maximising colour
				settings.setMaximisingColour(maximisingColour);
				// Run a new internal search
				g = runSearchAsSettings(n, board, alpha, NULL, currentDepth);
				restoreSettings();
				// n may have been moved in the ttable in the search above
				if( (n=lookupPtr(lookupNode)) == NULL)
					throw "Broken transposition table...\n";
				// Update score for this node... (and g, alpha/beta)
				n->setScoreCalculated(true);
				n->setAlphaBetaScore(g);
				// Does this score mean this a terminal node? I.e. the capture string has been captured?
				// Or DEAD or ALIVE has been achieved...
				if(testForTerminalNodes && getGoal().isTerminalScore(g))
					n->setTerminal(true);
				else
					n->setTerminal(false);
				nodesEvaluated++;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, ", evaluation=%g\n", g);
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
				extensionUsed = true;
				break;
			}
		}
		// No valid extensions so carry on with normal running
	}

	/** @todo This doesn't seem very efficient, is there a better way to do this? */
	
	// We need a test for terminal nodes. When we don't delete the ttable between
	// goals we need this housekeeping when changing goals so we don't miss
	// terminal nodes (which are only tested for terminality upon creation) in
	// subsequent searches.

	/** @todo Won't a maximum terminal score only be terminal if it's the maximising
	  * colour's turn to play and vice versa (minimum terminal score only terminal
	  * if it's the minimising colours turn)? 
	  * 
	  * Without this we may be running a less-than-complete search here... */

	// If the new goal is terminal determine if the nodes are terminal
	if(!extensionUsed && testForTerminalNodes) {
		int bestX, bestY;
		getGoal().evaluate(maximisingColour, g, board, n->getNextToPlay(), &bestX, &bestY);
		//g = getEvaluateFn()(board, maximisingColour, getGoal(), n->getNextToPlay());
		// Does this score mean this a terminal node? I.e. the capture string has been captured?
		// Or DEAD or ALIVE has been achieved...
		if(getGoal().isTerminalScore(g)) {
			n->setScoreCalculated(true);
			n->setAlphaBetaScore(g);
			n->setTerminal(true);
			// This counts as a leaf node evaluation
			nodesEvaluated++;
		}
		else
			n->setTerminal(false);

		n->setBestX(bestX);
		n->setBestY(bestY);
		n->setBestNextToPlay(n->getNextToPlay());
	}

	// time check, if we've run out of time just run evaluate
	if(getTimeLimit()!=0 && !timeRanOut && start!=NULL) {
		time(&end);
		double t = difftime(end, *start);
		if(t>=getTimeLimit())
			timeRanOut = true;
	}

	// early cutoff, if we have already achieved the goal here or if the time is up
	//if d == 0 then g := evaluate(n); /* leaf node */
	if(!extensionUsed && !n->isTerminal() && (timeRanOut || depthToSearch==0)) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			string leafMove(Move::xyToColourAndGoPosValue(n->getMove()));
			if(timeRanOut)
				sprintf(buffer, "Time run out, forcing leaf node %s", leafMove.c_str());
			else
				sprintf(buffer, "Leaf node %s", leafMove.c_str());
			printDebugMessage(buffer, currentDepth);
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN
		int bestX, bestY;
		getGoal().evaluate(maximisingColour, g, board, n->getNextToPlay(), &bestX, &bestY);
		n->setScoreCalculated(true);
		n->setAlphaBetaScore(g);
		// Does this score mean this a terminal node? I.e. the capture string has been captured?
		// Or DEAD or ALIVE has been achieved...
		if(getGoal().isTerminalScore(g))
			n->setTerminal(true);
		nodesEvaluated++;

		n->setBestX(bestX);
		n->setBestY(bestY);
		n->setBestNextToPlay(n->getNextToPlay());

#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			sprintf(buffer, ", evaluation=%g\n", g);
			printDebugMessage(buffer, currentDepth);
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN
	}
	//else if n == MAXNODE then
	// i.e. our moves are children of this node
	else if(!extensionUsed && !n->isTerminal() && n->getNextToPlay()==maximisingColour)
	{
		if(getUseNullMovePruning()) {
			/** @todo Implement null move pruning */
		}
		int bestKey, bestKey2, bestPreviousKey, bestPreviousKey2, bestX, bestY;
		float bestNodeAbwm;
		bool bestNodeExamined = false;
		bool earlyCutoff = false;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag)
			outputNodeDetails(*n, maximisingColour, currentDepth, board.getSize(), alpha, beta);
#endif // MTDFSEARCH_LEAN_AND_MEAN
		//g := -INFINITY; a := alpha; /* save original alpha value */
		g = NEGINFINITY;
		Node* bestNodePtr;
		// Do an alpha-beta search on the previous best node to see if we get a cutoff
		if(getUseBestMoveFirst() && inTranspositionTable && (bestNodePtr=getBestNode(*n))!=NULL) {
			// Make the move
#ifdef _DEBUG
			BoardStruct tempBoard(board);
#endif
			assert( oldBoard.size() > currentDepth );
			oldBoard[currentDepth] = board;
			makeMove(board, *bestNodePtr);
			// Save the best node keys for checking against expanded nodes later
			bestKey = bestNodePtr->getKey();
			bestKey2 = bestNodePtr->getKey2();
			bestPreviousKey = bestNodePtr->getPreviousKey();
			bestPreviousKey2 = bestNodePtr->getPreviousKey2();
			bestX = bestNodePtr->getX();
			bestY = bestNodePtr->getY();
			// Do the alpha-beta search
			bestNodeAbwm = maximum(g, AlphaBetaWithMemory(board, bestNodePtr,
				alpha, beta, depthToSearch-1, currentDepth+1, maximisingColour, false, start, testForTerminalNodes));
			// Unmake the move
//			_ASSERTE( _CrtCheckMemory() );
			assert( tempBoard == oldBoard[currentDepth] );
			board = oldBoard[currentDepth];
			if(bestNodeAbwm>=beta) {
				g = bestNodeAbwm;
				earlyCutoff = true;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag)
					printDebugMessage("Max node BestMoveFirst cutoff\n", currentDepth);
#endif // MTDFSEARCH_LEAN_AND_MEAN
			}
			/** @todo This call could be removed if we didn't 
			  * have a hash_set based on a volatile vector<> implementation. */
			// the pointer n may have been changed during the AB call above so refind it now
			if( (n=lookupPtr(lookupNode)) == NULL)
				throw "Max node Broken transposition table from lookupPtr(), lookupNode not found...\n";
			bestNodeExamined = true;
		} // end bestMoveFirst
		if(!earlyCutoff) {
			a = alpha;
			float bestabwm = g;
			float abwm = 0;
			int bestNode = 0;
			int bestNodeIndex = 0;
			vector<Node> nodes;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
			if(debugFlag)
				printDebugMessage("Expanding to: ", currentDepth);
#endif // MTDFSEARCH_LEAN_AND_MEAN
			expand(nodes, *n, board, maximisingColour);
			if(getUseEtc() && getUseTranspositionTable()) {
				// test the child nodes quickly to see if they can
				// cause an immediate cutoff
				TRANSPOSITION_TABLE::iterator iter;
				vector<Node>::const_iterator citer(nodes.begin());
				for(;citer!=nodes.end();citer++) {
					if((iter=transpositionTable->find((*citer))) != transpositionTable->end()) {
						if(iter->getDepthSearchedTo()>=(depthToSearch-1)) {
							// If we are deep enough and the score for this has 
							// been calculated before return the score rather than a bound
							if(n->scoreCalculated()) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, score found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								return n->getAlphaBetaScore();
							}

							float lb;
							if(iter->lowerboundCalculated() && (lb=iter->getLowerbound())>=beta) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, lowerbound found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								return lb;
							} // end lower bound cutoff
						} // end are we deep enough to rely on these scores?
					} // end find node in ttable
				} // end for j
			} // end ETC

			int	i = 0;
			// Note: So the searches return the same result we must make sure if the best
			// node is examined it starts as the best move available in the while() loop
			// below. Otherwise equal moves that occur before bestNodePtr may be chosen instead.
			// Do a quick search of the expanded nodes to find which one was the best node from above
			if(bestNodeExamined) {
				vector<Node>::const_iterator citer(nodes.begin());
				for(;citer!=nodes.end();citer++) {
					if(citer->getX()==bestX && citer->getY()==bestY) {
						// Save the index
						bestNode = bestNodeIndex = (citer - nodes.begin());
						break;
					}
				}
				// Set our best score to the best node we looked at
				bestabwm = bestNodeAbwm;
			}
			//while (g < beta) and (c != NOCHILD) do
			while(g<beta && i<nodes.size()) {
				Node& c = nodes[i];
				//g := max(g, AlphaBetaWithMemory(c, a, beta, d - 1));
#ifdef _DEBUG
				BoardStruct tempBoard(board);
#endif
				assert( oldBoard.size() > currentDepth );
				oldBoard[currentDepth] = board;
				makeMove(board, c);                      // Make the move
				// check if this child node is the best node we have already alpha-beta'd
				// Note: bestNodePtr may change due to ttable insertions so we can't use it here
				/** @todo What's wrong with if(bestNodeExamined && i==bestNodeIndex) ? */
				if(bestNodeExamined && c.getKey()==bestKey && c.getKey2()==bestKey2 &&
					c.getPreviousKey()==bestPreviousKey && c.getPreviousKey2()==bestPreviousKey2 &&
					c.getX()==bestX && c.getY()==bestY)
					abwm = bestNodeAbwm;
				else {
					abwm = AlphaBetaWithMemory(board, &c, a, beta, depthToSearch-1, currentDepth+1, maximisingColour,
						false, start, testForTerminalNodes);
				}
//				_ASSERTE( _CrtCheckMemory() );
				assert( tempBoard == oldBoard[currentDepth] );
				board = oldBoard[currentDepth];          // Undo the move
				// save the best child node
				if(abwm>bestabwm) {
					bestabwm = abwm;
					bestNode = i;
				}
				g = maximum(g, abwm);
				a = maximum(a, g);
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "Parent g updated to = %g, ", g);
					printDebugMessage(buffer, currentDepth);
					sprintf(buffer, "alpha updated to = %g\n", a);
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN

				if(!timeRanOut && getTimeLimit()!=0 && start!=NULL) {
					time(&end);
					double t = difftime(end, *start);
					if(t>=getTimeLimit())
						timeRanOut = true;
				}
				if(timeRanOut)
					break;

				i++;
			} // end while nodes
			/** @todo This call could be removed if we didn't 
			  * have a hash_set based on a volatile vector<> implementation. */
			if(nodes.size()>0) {
				if(getUseTranspositionTable())
					n = updateTTable(lookupNode, nodes[bestNode]);
				else
					n->setBestNode(&(nodes[bestNode]));
			}
		} // if !earlyCutoff
	}
	//else /* n is a MINNODE */
	else if(!extensionUsed && !n->isTerminal()) {
		if(getUseNullMovePruning()) {
			/** @todo Implement null move pruning */
		}
		int bestKey, bestKey2, bestPreviousKey, bestPreviousKey2, bestX, bestY;
		float bestNodeAbwm;
		bool bestNodeExamined = false;
		bool earlyCutoff = false;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag)
			outputNodeDetails(*n, maximisingColour, currentDepth, board.getSize(), alpha, beta);
#endif // MTDFSEARCH_LEAN_AND_MEAN
		//g := +INFINITY; b := beta; /* save original beta value */
		g = POSINFINITY;
		Node* bestNodePtr;
		// Do an alpha-beta search on the previous best node to see if we get a cutoff
		if(getUseBestMoveFirst() && inTranspositionTable && (bestNodePtr=getBestNode(*n))!=NULL) {
			// Make the move
#ifdef _DEBUG
			BoardStruct tempBoard(board);
#endif
			assert( oldBoard.size() > currentDepth );
			oldBoard[currentDepth] = board;
			makeMove(board, *bestNodePtr);
			// Save the best node keys for checking against expanded nodes later
			bestKey = bestNodePtr->getKey();
			bestKey2 = bestNodePtr->getKey2();
			bestPreviousKey = bestNodePtr->getPreviousKey();
			bestPreviousKey2 = bestNodePtr->getPreviousKey2();
			bestX = bestNodePtr->getX();
			bestY = bestNodePtr->getY();
			// Do the alpha-beta search
			bestNodeAbwm = minimum(g, AlphaBetaWithMemory(board, bestNodePtr, 
				alpha, beta, depthToSearch-1, currentDepth+1, maximisingColour, false, start, testForTerminalNodes));
			// Unmake the move
//			_ASSERTE( _CrtCheckMemory() );
			assert( tempBoard == oldBoard[currentDepth] );
			board = oldBoard[currentDepth];
			if(bestNodeAbwm<=alpha) {
				g = bestNodeAbwm;
				earlyCutoff = true;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag)
					printDebugMessage("Min node BestMoveFirst cutoff\n", currentDepth);
#endif // MTDFSEARCH_LEAN_AND_MEAN
			}
			/** @todo This call could be removed if we didn't 
			  * have a hash_set based on a volatile vector<> implementation. */
			// the pointer n may have been changed during the AB call above so refind it now
			if( (n=lookupPtr(lookupNode)) == NULL)
				throw "Min node Broken transposition table from lookupPtr(), lookupNode not found...\n";
			bestNodeExamined = true;
		} // end bestMoveFirst
		if(!earlyCutoff) {
			b = beta;
			float bestabwm = g;
			float abwm = 0;
			int bestNode = 0;
			int bestNodeIndex = 0;
			vector<Node> nodes;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
			if(debugFlag)
				printDebugMessage("Expanding to: ", currentDepth);
#endif // MTDFSEARCH_LEAN_AND_MEAN
			expand(nodes, *n, board, maximisingColour);
			if(getUseEtc() && getUseTranspositionTable()) {
				// test the child nodes quickly to see if they can
				// cause an immediate cutoff
				vector<Node>::const_iterator citer(nodes.begin());
				TRANSPOSITION_TABLE::iterator iter;
				for(;citer!=nodes.end();citer++) {
					if((iter=transpositionTable->find((*citer))) !=transpositionTable->end()) {
						if(iter->getDepthSearchedTo()>=(depthToSearch-1)) {
							// If we are deep enough and the score for this has 
							// been calculated before return the score rather than a bound
							if(n->scoreCalculated()) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, score found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								return n->getAlphaBetaScore();
							}

							float ub;
							if(iter->upperboundCalculated() && (ub=iter->getUpperbound())<=alpha) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, upperbound found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								return ub;
							} // end upper bound cutoff
						} // end are we deep enough to rely on these scores?
					} // end find node in ttable
				} // end for j
			} // end ETC

			int i = 0;
			// Note: So the searches return the same result we must make sure if the best
			// node is examined it starts as the best move available in the while() loop
			// below. Otherwise equal moves that occur before bestNodePtr may be chosen instead.
			// Do a quick search of the expanded nodes to find which one was the best node from above
			if(bestNodeExamined) {
				vector<Node>::const_iterator citer(nodes.begin());
				for(;citer!=nodes.end();citer++) {
					if(citer->getX()==bestX && citer->getY()==bestY) {
						// Save the index
						bestNode = bestNodeIndex = (citer - nodes.begin());
						break;
					}
				}
				// Set our best score to the best node we looked at
				bestabwm = bestNodeAbwm;
			}
			//while (g > alpha) and (c != NOCHILD) do
			while(g>alpha && i<nodes.size()) {
				Node& c = nodes[i];
				//g := max(g, AlphaBetaWithMemory(c, a, beta, d - 1));
#ifdef _DEBUG
				BoardStruct tempBoard(board);
#endif
				assert( oldBoard.size() > currentDepth );
				oldBoard[currentDepth] = board;
				makeMove(board, c);                          // Make the move
				// check if this child node is the best node we have already alpha-beta'd
				// Note: bestNodePtr may change due to ttable insertions so we can't use it here
				/** @todo What's wrong with if(bestNodeExamined && i==bestNodeIndex) ? */
				if(bestNodeExamined && c.getKey()==bestKey && c.getKey2()==bestKey2 &&
					c.getPreviousKey()==bestPreviousKey && c.getPreviousKey2()==bestPreviousKey2 &&
					c.getX()==bestX && c.getY()==bestY)
					abwm = bestNodeAbwm;
				else
					abwm = AlphaBetaWithMemory(board, &c, alpha, b, depthToSearch-1, currentDepth+1, maximisingColour,
						false, start, testForTerminalNodes);
//				_ASSERTE( _CrtCheckMemory() );
				assert( tempBoard == oldBoard[currentDepth] );
				board = oldBoard[currentDepth];              // Undo the move
				// save the best child node
				if(abwm<bestabwm) {
					bestabwm = abwm;
					bestNode = i;
				}
				g = minimum(g, abwm);
				b = minimum(b, g);
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "Parent g updated to = %g, ", g);
					printDebugMessage(buffer, currentDepth);
					sprintf(buffer, "beta updated to = %g\n", b);
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
				if(!timeRanOut && getTimeLimit()!=0 && start!=NULL) {
					time(&end);
					double t = difftime(end, *start);
					if(t>=getTimeLimit())
						timeRanOut = true;
				}
				if(timeRanOut)
					break;
				i++;
			}
			/** @todo This call could be removed if we didn't 
			  * have a hash_set based on a volatile vector<> implementation. */
			if(nodes.size()>0) {
				if(getUseTranspositionTable())
					n = updateTTable(lookupNode, nodes[bestNode]);
				else
					n->setBestNode(&(nodes[bestNode]));
			}
		}
	}

//	_ASSERTE( _CrtCheckMemory() );
	/* Traditional transposition table storing of bounds */ 
	/* Fail low result implies an upper bound */ 
	if(g<=alpha) {
		n->setUpperbound(g);
		n->setUpperboundCalculated(true);
	}

	/* Found an accurate minimax value - will not occur if called with zero window */ 
	if(g>alpha && g<beta) {
		//n.lowerbound := g; n.upperbound := g; store n.lowerbound, n.upperbound;
//		_ASSERTE( _CrtCheckMemory() );
		n->setLowerbound(g);
		n->setUpperbound(g);
		n->setUpperboundCalculated(true);
		n->setLowerboundCalculated(true);
		// since accurate score, store it as such
		n->setAlphaBetaScore(g);
		n->setScoreCalculated(true);
//		_ASSERTE( _CrtCheckMemory() );
	}

	/* Fail high result implies a lower bound */ 
	if(g>=beta)	{
		n->setLowerbound(g);
		n->setLowerboundCalculated(true);
	}

	n->setDepthSearchedTo(depthToSearch);
	
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag)
	{
		outputNodeDetails(*n, maximisingColour, currentDepth, board.getSize(), alpha, beta);
		char buffer[50];
		sprintf(buffer, "AlphaBetaWithMemory (depth=%d) returns g = %*g\n", currentDepth, 4, g);
		printDebugMessage(buffer, currentDepth);
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN
	return g;
}

/** An alpha beta minimax search algorithm with memory. The memory
recalls what the previous alpha beta bounds were for nodes that
caused a cutoff. 
NOTE: This is a NegaMax version - this version is inappropriate for
using with LifeDeathGoal evals.
WARNING: This function is unstable and untested.
@param n The root node of the search tree.
@param alpha The alpha bound of the alpha beta cutoff window, the lower bound of the minimax score.
@param beta The beta bound of the alpha beta cutoff window, the upper bound of the minimax score.
@param depthToSearch The maximum depth to search within this subtree.
@param currentDepth The current depth of node n.
@param maximisingColour The colour who's point of view is being taken in this search.
@param rootLevel True if n is a node at the very root of the search tree, i.e. this function
call is not a recursive child.
@param start An optional parameter, the time at which the search began. Use NULL.to ignore.
@return The minimax score of this tree. */
float MTDFSearch::NegaMaxAlphaBetaWithMemory(BoardStruct& board, Node* n, float alpha, float beta, const int depthToSearch,
									  const int currentDepth, const int maximisingColour, const bool rootLevel,
									  time_t* start, const bool testForTerminalNodes)
{
	// Resize our persistent board change saver - only if this is the start of the search though...
	if(rootLevel && oldBoard.size()!=depthToSearch)
		oldBoard.resize(depthToSearch);

	nodesTraversed++;

	n->setDepth(currentDepth);

	float g, a;
	TRANSPOSITION_TABLE::iterator iter;

	bool inTranspositionTable = false;
	if(getUseTranspositionTable()) {
		float boundValue;
		BoundType boundType = BoundType::UNSET;
		if( lookupTranspositionTableBounds(boundValue, boundType, n, currentDepth, depthToSearch, alpha, beta) )
		{
			inTranspositionTable = true;
			if( boundType == EXACT_VALUE ) {
				if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
					return -boundValue;
				return boundValue;
			}
			else if( boundType == LOWER_BOUND ) {
				if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
					return -boundValue;
				return boundValue;
			}
			else if( boundType == UPPER_BOUND ) {
				if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
					return -boundValue;
				return boundValue;
			}
			else if( boundType == BoundType::UNSET )
				throw "boundType not set in MTDFSearch::AlphaBetaWithMemory()! ";
			// else if( boundType == BoundType::NO_CUTOFF )  // No cutoff so no returnable bound
		}
		// node n not in the transposition table so add it and re-reference
		else {
			// copy n into ttable
			n = &(transpositionTable->insert(*n));
			inTranspositionTable = true;
		}
	} // if useTranspositionTable

	time_t end;

	/** @todo This doesn't seem very efficient, is there a better way to do this? */
	
	// We need a test for terminal nodes. When we don't delete the ttable between
	// goals we need this housekeeping when changing goals so we don't miss
	// terminal nodes (which are only tested for terminality upon creation) in
	// subsequent searches.
	
	// If the new goal is terminal determine if the nodes are terminal
	if(testForTerminalNodes) {
		getGoal().evaluate(maximisingColour, g, board, n->getNextToPlay());
		// Does this score mean this a terminal node? I.e. the capture string has been captured?
		// Or DEAD or ALIVE has been achieved...
		float terminalTest = g;
		if(n->getNextToPlay()!=maximisingColour) // For NegaMax
			terminalTest = -g;
		if(getGoal().isTerminalScore(terminalTest)) {
			n->setScoreCalculated(true);
			n->setAlphaBetaScore(terminalTest);
			n->setTerminal(true);
			// This counts as a leaf node evaluation
			nodesEvaluated++;
		}
		else
			n->setTerminal(false);
	}

	Node lookupNode(*n);

	// time check, if we've run out of time just run evaluate
	if(getTimeLimit()!=0 && !timeRanOut && start!=NULL) {
		time(&end);
		double t = difftime(end, *start);
		if(t>=getTimeLimit())
			timeRanOut = true;
	}

	/** @todo Ladder handling here. 
	  * Only do for capture search, where target has <=2 liberties.
	  * Use ladderExpand().
	  * Should this use LadderGoal (DEAD-UNKNOWN(5)) or LifeDeathGoal? 
	  * Probably LifeDeathGoal */
	// early cutoff, if we have already achieved the goal here or if the time is up
	//if d == 0 then g := evaluate(n); /* leaf node */
	if(!n->isTerminal() && (timeRanOut || depthToSearch==0)) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			string leafMove(Move::xyToColourAndGoPosValue(n->getMove()));
			if(timeRanOut)
				sprintf(buffer, "Time run out, forcing leaf node %s", leafMove.c_str());
			else
				sprintf(buffer, "Leaf node %s", leafMove.c_str());
			printDebugMessage(buffer, currentDepth);
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN
		getGoal().evaluate(maximisingColour, g, board, n->getNextToPlay());
		// Does this score mean this a terminal node? I.e. the capture string has been captured?
		// Or DEAD or ALIVE has been achieved...
		float terminalTest = g;
		if(n->getNextToPlay()!=maximisingColour) // For NegaMax
			terminalTest = -g;
		if(getGoal().isTerminalScore(terminalTest))
			n->setTerminal(true);
		n->setScoreCalculated(true);
		n->setAlphaBetaScore(terminalTest);
		nodesEvaluated++;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			sprintf(buffer, ", evaluation=%g\n", g);
			printDebugMessage(buffer, currentDepth);
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN
	}
	// All nodes attempt to maximise in NegaMax
	else if(!n->isTerminal())
	{
		if(getUseNullMovePruning()) { /** @todo Implement null move pruning */
		}
		int bestKey, bestKey2, bestPreviousKey, bestPreviousKey2, bestX, bestY;
		float bestNodeAbwm;
		bool bestNodeExamined = false;
		bool earlyCutoff = false;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag)
			outputNodeDetails(*n, maximisingColour, currentDepth, board.getSize(), alpha, beta);
#endif // MTDFSEARCH_LEAN_AND_MEAN
		//g := -INFINITY; a := alpha; /* save original alpha value */
		g = NEGINFINITY;
		Node* bestNodePtr;
		// Do an alpha-beta search on the previous best node to see if we get a cutoff
		if(getUseBestMoveFirst() && inTranspositionTable && (bestNodePtr=getBestNode(*n))!=NULL) {
			// Make the move
			oldBoard[currentDepth] = board;
			makeMove(board, *bestNodePtr);
			// Save the best node keys for checking against expanded nodes later
			bestKey = bestNodePtr->getKey();
			bestKey2 = bestNodePtr->getKey2();
			bestPreviousKey = bestNodePtr->getPreviousKey();
			bestPreviousKey2 = bestNodePtr->getPreviousKey2();
			bestX = bestNodePtr->getX();
			bestY = bestNodePtr->getY();
			// Do the alpha-beta search
			bestNodeAbwm = maximum(g, -AlphaBetaWithMemory(board, bestNodePtr,
				-beta, -alpha, depthToSearch-1, currentDepth+1, maximisingColour, false, start, testForTerminalNodes));
			// Unmake the move
			board = oldBoard[currentDepth];
			if(bestNodeAbwm>=beta) {
				g = bestNodeAbwm;
				earlyCutoff = true;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag)
					printDebugMessage("Max node BestMoveFirst cutoff\n", currentDepth);
#endif // MTDFSEARCH_LEAN_AND_MEAN
			}
			/** @todo Is this necessary or does the insertion bit in the AB call update the
			  *  n pointer??? If the pointer address is passed by value then it may not... */
			// the pointer n may have been changed during the AB call above so refind it now
			if((iter=transpositionTable->find(lookupNode))==transpositionTable->end())
				LogWriter::printerr("Max node broken transposition table 1...\n", getName());
			n = &(*iter);
			bestNodeExamined = true;
		} // end bestMoveFirst
		if(!earlyCutoff) {
			a = alpha;
			float bestabwm = g;
			float abwm = 0;
			int bestNode = 0;
			int bestNodeIndex = 0;
			vector<Node> nodes;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
			if(debugFlag)
				printDebugMessage("Expanding to: ", currentDepth);
#endif // MTDFSEARCH_LEAN_AND_MEAN
			expand(nodes, *n, board, maximisingColour);
			if(getUseEtc() && getUseTranspositionTable()) {
				// test the child nodes quickly to see if they can
				// cause an immediate cutoff
				vector<Node>::const_iterator citer(nodes.begin());
				for(;citer!=nodes.end();citer++) {
					if((iter=transpositionTable->find((*citer))) != transpositionTable->end()) {
						if(iter->getDepthSearchedTo()>=(depthToSearch-1)) {
							// If we are deep enough and the score for this has 
							// been calculated before return the score rather than a bound
							if(n->scoreCalculated()) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, score found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
									return -n->getAlphaBetaScore();
								return n->getAlphaBetaScore();
							}

							if(n->getNextToPlay()==maximisingColour && iter->lowerboundCalculated() 
								&& iter->getLowerbound()>=beta) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, lowerbound found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
									return -iter->getLowerbound();
								return iter->getLowerbound();
							} // end lower bound cutoff
							if(n->getNextToPlay()!=maximisingColour && iter->upperboundCalculated() 
								&& iter->getUpperbound()<=alpha) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
								if(debugFlag) {
									printDebugMessage("ETC cutoff, upperbound found, caused by child node: ", currentDepth);
									outputNodeDetails((*iter), maximisingColour, currentDepth+1, board.getSize(), alpha, beta);
								}
#endif // MTDFSEARCH_LEAN_AND_MEAN
								if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
									return -iter->getUpperbound();
								return iter->getUpperbound();
							} // end upper bound cutoff
						} // end are we deep enough to rely on these scores?
					} // end find node in ttable
				} // end for j
			} // end ETC

			int	i = 0;
			// Note: So the searches return the same result we must make sure if the best
			// node is examined it starts as the best move available in the while() loop
			// below. Otherwise equal moves that occur before bestNodePtr may be chosen instead.
			// Do a quick search of the expanded nodes to find which one was the best node from above
			if(bestNodeExamined) {
				vector<Node>::const_iterator citer(nodes.begin());
				for(;citer!=nodes.end();citer++) {
					if(citer->getX()==bestX && citer->getY()==bestY) {
						// Save the index
						bestNode = bestNodeIndex = (citer - nodes.begin());
						break;
					}
				}
				// Set our best score to the best node we looked at
				bestabwm = bestNodeAbwm;
			}
			//while (g < beta) and (c != NOCHILD) do
			while(g<beta && i<nodes.size())	{
				Node& c = nodes[i];
				//g := max(g, AlphaBetaWithMemory(c, a, beta, d - 1));
				oldBoard[currentDepth] = board;
				makeMove(board, c);                      // Make the move
				// check if this child node is the best node we have already alpha-beta'd
				// Note: bestNodePtr may change due to ttable insertions so we can't use it here
				/** @todo What's wrong with if(bestNodeExamined && i==bestNodeIndex) ? */
				if(bestNodeExamined && c.getKey()==bestKey && c.getKey2()==bestKey2 &&
					c.getPreviousKey()==bestPreviousKey && c.getPreviousKey2()==bestPreviousKey2 &&
					c.getX()==bestX && c.getY()==bestY)
					abwm = bestNodeAbwm;
				else {
					abwm = -AlphaBetaWithMemory(board, &c, -beta, -a, depthToSearch-1, currentDepth+1, maximisingColour,
						false, start, testForTerminalNodes);
				}
				board = oldBoard[currentDepth];          // Undo the move
				// save the best child node
				if(abwm>bestabwm) {
					bestabwm = abwm;
					bestNode = i;
					/** @todo If (!getUseTranspositionTable() && rootLevel) store the best move */
				}
				g = maximum(g, abwm);
				a = maximum(a, g);
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "Parent g updated to = %g, ", g);
					printDebugMessage(buffer, currentDepth);
					sprintf(buffer, "alpha updated to = %g\n", a);
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN

				if(!timeRanOut && getTimeLimit()!=0 && start!=NULL) {
					time(&end);
					double t = difftime(end, *start);
					if(t>=getTimeLimit())
						timeRanOut = true;
				}
				if(timeRanOut)
					break;

				i++;
			}
			if(nodes.size()>0) {
				if(getUseTranspositionTable())
					n = updateTTable(lookupNode, nodes[bestNode]);
				else
					n->setBestNode(&(nodes[bestNode]));
			}
		}
	}

	/* Traditional transposition table storing of bounds */ 
	/* Fail low result implies an upper bound */ 
	if(g<=alpha) {
		n->setUpperbound(g);
		n->setUpperboundCalculated(true);
	}

	/* Found an accurate minimax value - will not occur if called with zero window */ 
	if(g>alpha && g<beta) {
		//n.lowerbound := g; n.upperbound := g; store n.lowerbound, n.upperbound;
		n->setLowerbound(g);
		n->setUpperbound(g);
		n->setUpperboundCalculated(true);
		n->setLowerboundCalculated(true);
		// since accurate score, store it as such
		n->setAlphaBetaScore(g);
		n->setScoreCalculated(true);
	}

	/* Fail high result implies a lower bound */ 
	if(g>=beta)	{
		n->setLowerbound(g);
		n->setLowerboundCalculated(true);
	}

	n->setDepthSearchedTo(depthToSearch);
	
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if(debugFlag) {
		outputNodeDetails(*n, maximisingColour, currentDepth, board.getSize(), alpha, beta);
		char buffer[50];
		sprintf(buffer, "AlphaBetaWithMemory (depth=%d) returns g = %*g\n", currentDepth, 4, g);
		printDebugMessage(buffer, currentDepth);
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN
	if(rootLevel && n->getNextToPlay()!=maximisingColour) // For NegaMax
		return -g;
	return g;
}

///////////////////////////////////////
// Utility functions for tree search //
///////////////////////////////////////

/** @brief This function calls the MoveSuggester associated with this search instance
  * to obtain moves for the given board state. These moves are converted into nodes and
  * returned to the caller. 
  * @param nodes A list reference to return the child nodes in.
  * @param n The parent node to expand. 
  * @param board The board state at node n.
  * @param maximisingColour The colour trying to maximise their score. */
void MTDFSearch::expand(vector<Node>& nodes, const Node& n, const BoardStruct& board, const int maximisingColour)
{
	vector<Move> moves;
	settings.getMoveSuggester().getMoves(moves, board, settings, n.getNextToPlay(), maximisingColour,
		*((Melkor*)melkor)->getBPN());

	// Convert moves to nodes
	nodes.clear();
	vector<Move>::const_iterator miter(moves.begin());
	for(;miter!=moves.end();miter++)
		nodes.push_back(getChildNode(n, *miter));

#ifndef MTDFSEARCH_LEAN_AND_MEAN
	// print out the nodes it's expanded to if required
	if(debugFlag) {
		if(nodes.size()==0)
			LogWriter::printerr("Expand returns nodes.size()==0\n", getName());
		for(int i=0;i<nodes.size();i++)	{
			LogWriter::printerr(Move::xyToColourAndGoPosValue(nodes[i].getMove()));
			LogWriter::printerr(" ");
		}
		LogWriter::printerr("\n");
	}
#endif // MTDFSEARCH_LEAN_AND_MEAN
}

Node* MTDFSearch::updateTTable(const Node& lookupNode, const Node& bestNode)
{
	if(getUseTranspositionTable()) {
//		_ASSERTE( _CrtCheckMemory() );
		// the pointer n may have been changed during the AB call above so refind it now
		Node* tempNode;
		if( (tempNode=lookupPtr(lookupNode)) == NULL)
			throw "Broken transposition table in updateTTable(), Node n not found...\n";
		// find nodes[bestNode] in trans table and copy that upto date version
		Node ttBestNode;
		if(!lookup(bestNode, ttBestNode))
			LogWriter::printerr("Broken transposition table in updateTTable(), bestNode not found...\n", getName());
		tempNode->setBestNode(&ttBestNode);
//		_ASSERTE( _CrtCheckMemory() );
		return tempNode;
	}
	return NULL;
}

/** @brief Lookup the given node in the transposition table and return any 
  * bound that can cause an immediate cutoff. If the node has not been searched 
  * deep enough in the ttable then reset all the 'bound calculated' flags in n.
  * Additionally update alpha or beta if the bounds aren't high/low enough to cause a cutoff.
  * @return True if the node is in the transposition table. */
bool MTDFSearch::lookupTranspositionTableBounds(float& boundValue, BoundType& boundType, Node* n,
			const int currentDepth, const int depthToSearch, float& alpha, float& beta)
{
#ifndef MTDFSEARCH_LEAN_AND_MEAN
	if( !getUseTranspositionTable() )
		throw "No transposition table in use in MTDFSearch::transpositionTableLookup()! ";
#endif // MTDFSEARCH_LEAN_AND_MEAN

	/* Transposition table lookup */
	Node* tempNode;
	if( (tempNode=lookupPtr(*n)) != NULL ) {
		n = tempNode;
#ifndef MTDFSEARCH_LEAN_AND_MEAN
		if(debugFlag) {
			char buffer[200];
			sprintf(buffer, "Node %s found in transposition table", Move::xyToColourAndGoPosValue(n->getMove()).c_str());
			printDebugMessage(buffer, currentDepth);
			if(n->scoreCalculated()) {
				for(int v=0;v<currentDepth;v++)
					LogWriter::printerr(" ");
				sprintf(buffer, " (alphaBeta score=%g)", n->getAlphaBetaScore());
				LogWriter::printerr(buffer);
			}
			else
				LogWriter::printerr(" (alphaBeta score unknown)");
			Node* bestNode;
			if((bestNode=getBestNode(*n))!=NULL) {
				sprintf(buffer, " has bestNode = %s\n", Move::xyToColourAndGoPosValue(bestNode->getMove()).c_str());
				LogWriter::printerr(buffer);
			}
			else
				LogWriter::printerr("\n");	
		}
#endif // MTDFSEARCH_LEAN_AND_MEAN
		// check if we have analysed this node deeply enough to use the values it has stored
		if(n->getDepthSearchedTo()>=depthToSearch || n->isTerminal()) {
			// If we are deep enough and the score for this has 
			// been calculated before return the score rather than a bound
			if(n->scoreCalculated()) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "AlphaBetaWithMemory returns alphaBeta score from transposition table: %g\n", n->getAlphaBetaScore());
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
				boundValue = n->getAlphaBetaScore();
				boundType = BoundType::EXACT_VALUE;
				return true;
			}
			
			//if n.lowerbound >= beta then return n.lowerbound;
			if(n->lowerboundCalculated() && n->getLowerbound()>=beta) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "AlphaBetaWithMemory returns lowerbound from transposition table: %g\n", n->getLowerbound());
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
				boundValue = n->getLowerbound();
				boundType = BoundType::LOWER_BOUND;
				return true;
			}
			//if n.upperbound <= alpha then return n.upperbound;
			if(n->upperboundCalculated() && n->getUpperbound()<=alpha) {
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "AlphaBetaWithMemory returns upperbound from transposition table: %g\n", n->getUpperbound());
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
				boundValue = n->getUpperbound();
				boundType = BoundType::UPPER_BOUND;
				return true;
			}
			boundType = BoundType::NO_CUTOFF;
			//alpha := max(alpha, n.lowerbound);
			if(n->lowerboundCalculated() && n->getLowerbound()>alpha) {
				alpha = n->getLowerbound();
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "alpha updated from lowerbound in transposition table, alpha = %g\n", alpha);
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
			}
			//beta := min(beta, n.upperbound);
			if(n->upperboundCalculated() && n->getUpperbound()<beta) {
				beta = n->getUpperbound();
#ifndef MTDFSEARCH_LEAN_AND_MEAN
				if(debugFlag) {
					char buffer[200];
					sprintf(buffer, "beta updated from lowerbound in transposition table, beta = %g\n", beta);
					printDebugMessage(buffer, currentDepth);
				}
#endif // MTDFSEARCH_LEAN_AND_MEAN
			}
		}
		// if not searched deep enough, make sure values are recalculated
		else {
			boundType = BoundType::NO_CUTOFF;
			n->setLowerboundCalculated(false);
			n->setUpperboundCalculated(false);
			n->setScoreCalculated(false);
		}
	} // end if n in ttable
	else
		return false;
	return true;
}

Node MTDFSearch::getChildNode(const Node& parent, const Move& move)
{
	static Node t;
	t = parent;
	t.setBestNode(NULL);
	t.setScoreCalculated(false);
	t.setUpperboundCalculated(false);
	t.setLowerboundCalculated(false);
	t.setTerminal(false);
	
	if(parent.getNextToPlay()==BLACK) t.setNextToPlay(WHITE);
	else t.setNextToPlay(BLACK);
	
	t.setMove(move);
	return t;
}

void MTDFSearch::outputNodeDetails(const Node& n, const int ourColour, const int depth, const int boardSize, const float alpha, const float beta)
{
	// output details of this node
	// Name Max/Min LowerBound UpperBound Alpha Beta Score
	LogWriter log;
	char buffer[200];
	string message = "";
	// ourColour moves are MAX, WHITE are MIN
	if(n.getNextToPlay()==ourColour)
		message+="Max node: ";
	else
		message+="Min node: ";
	message+=Move::xyToColourAndGoPosValue(n.getMove());
	message+=" ";
	sprintf(buffer, "Depth=%d ", depth);
	message+=buffer;
	sprintf(buffer, "DepthSearchedTo=%d ", n.getDepthSearchedTo());
	message+=buffer;
	sprintf(buffer, "NNScore=%g ", n.getNNScore());
	message+=buffer;
	if(n.lowerboundCalculated())
		sprintf(buffer, "LB=%g ", n.getLowerbound());
	else
		sprintf(buffer, "LB=? ");
	message+=buffer;
	if(n.upperboundCalculated())
		sprintf(buffer, "UB=%g ", n.getUpperbound());
	else
		sprintf(buffer, "UB=? ");
	message+=buffer;
	sprintf(buffer, "A=%g ", alpha);
	message+=buffer;
	sprintf(buffer, "B=%g ", beta);
	message+=buffer;
	if(n.scoreCalculated())
		sprintf(buffer, "AB-Score=%g \n", n.getAlphaBetaScore());
	else
		sprintf(buffer, "AB-Score=? \n");
	message+=buffer;
	printDebugMessage(message.c_str(), depth);
}

/** @brief A function to setup batches of variables since there's 
  * potentially a lot to configure for any single search. */
void MTDFSearch::setupMTDFSearch(const bool bestMoveFirst, const bool etc, const bool moveQualityThreshold, 
	const float prune, const bool transpositionTable, const float pass, const bool nullMovePruning,
	const bool useIterativeDeepening, const bool useMTDF)
{
	setUseBestMoveFirst(bestMoveFirst);
	setUseEtc(etc);
	setUseNullMovePruning(nullMovePruning);
	setUseTranspositionTable(transpositionTable);
	setUseMoveQualityThreshold(moveQualityThreshold);
	setPruneValue(prune);
	setPassValue(pass);
	setUseIterativeDeepening(useIterativeDeepening);
	setUseMTDF(useMTDF);
}

bool MTDFSearch::setupMTDFSearch(const string* args, const BoardStruct& b)
{
	// string must be of the form:
	// "variable=value"
	// variable/value pair may be repeated for as many different
	// variables as necessary, unrecognised variables are ignored.
	// variable/value pairs must be separated by spaces.
	// e.g. "time=5 depth=0"
	if(args==NULL)
		return true;

	// valid arguments are:
	// time
	// maxDepth
	// prune
	// useNChildren
	// useNChildrenPercent
	// goal
	// nextToPlay
	// pass

	int end = -1;
	string pair, variable, value, message;
	int start = 0;
	int n, t;
	float f;
	bool useNChildrenSet = false;
	bool useNChildrenPercentSet = false;
	while(start<args->size()) {
		// find space or end of string
		end = args->find(' ', end+1);
		if(end<0)
			end = args->size();
		// read from start to end and put in pair
		// substr reads from start, (end-start) elements along
		pair = args->substr(start, (end-start));
		// update start
		start=end+1;

		// interpret pair
		// separate variable name and value
		if((n = pair.find('='))<1 || n>=(pair.size()-1)) {
			// no '=' found or variable name of length 0
			// or value of length 0
			// so return error
			string message = "Bad argument pair: ";
			message+=pair;
			message+="\n";
			LogWriter::printerr(message, "");
			return false;
		}
		variable = pair.substr(0, n);
		value = pair.substr(n+1, (pair.size()-(n+1)));
		// see if we recognise this pair
		if(variable=="time") {
			// interpret value as an int
			// limits: n>0
			t = atoi(value.c_str());
			if(t<0) {
				message = "Out of range error: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			setTimeLimit(t);
		}
		else if(variable=="maxDepth") {
			// interpret value as an int
			// limits: n>0, n<100
			if((t = atoi(value.c_str()))<=0 || t>=100) {
				message = "Out of range error: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			setDepth(t);
		}
		else if(variable=="prune") {
			// interpret value as an float
			// limits: 0.0<=n<=1.0,
			if((f = atof(value.c_str()))<0 || f>1) {
				message = "Out of range error: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			// because atof returns 0 on failure
			// we need to check it's not an actual failure
			if(f==0 && value!="0.0") {
				message = "Value not a float: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			setUseMoveQualityThreshold(true);
			setPruneValue(f);
		}
		else if(variable=="pass") {
			// interpret value as an float
			// limits: 0.0<=n<=1.0,
			if((f = atof(value.c_str()))<0 || f>1) {
				message = "Out of range error: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			// because atof returns 0 on failure
			// we need to check it's not an actual failure
			if(f==0 && value!="0.0") {
				message = "Value not a float: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			setPassValue(f);
		}
		else if(variable=="useNChildren") {
			// read in whole number of moves to be using at each node expansion
			if(useNChildrenPercentSet) {
				message = "Syntax error, cannot use useNChildren and useNChildrenPercent together: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			else if((t = atoi(value.c_str()))<0) {
				message = "Out of range error: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			setUseNChildren(t);
			setUseNChildrenPercent(0.0f);
			useNChildrenSet = true;
		}
		else if(variable=="useNChildrenPercent") {
			if(useNChildrenSet) {
				message = "Syntax error, cannot use useNChildren and useNChildrenPercent together: ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			else if(sscanf(value.c_str(), "%d%%", &t)==1) {
				// check percent is between 0 and 100 inclusive
				if(t<0 || t>100) {
					message = "Out of range error: ";
					message+=pair;
					message+="\n";
					LogWriter::printerr(message, "");
					return false;
				}
				setUseNChildren(0);
				setUseNChildrenPercent((float)t/(float)100);
				useNChildrenPercentSet = true;
			}
			else {
				message = "Syntax error, (perhaps missing %?): ";
				message+=pair;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
		}
		else if(variable=="goal") {
			string errorString;
			if(!parseGoalString(value, getGoal(), errorString, b)) {
				LogWriter::printerr(errorString);
				return false;
			}
		}
		else if(variable=="nextToPlay") {
			if(value!="BLACK" && value!="WHITE") {
				message = "Bad 'nextToPlay' colour: ";
				message+=value;
				message+="\n";
				LogWriter::printerr(message, "");
				return false;
			}
			if(value=="BLACK")
				setNextToPlay(BLACK);
			else if(value=="WHITE")
				setNextToPlay(WHITE);
		}
		else {
			message = "Unrecognised argument: ";
			message+=variable;
			message+="\n";
			LogWriter::printerr(message, "");
			LogWriter::printerr(getSetupStringSyntax(), "");
			return false;
		}
	} // end for start
	return true;
}

string MTDFSearch::getSetupStringSyntax()
{
	string message = "Syntax: 'arguments=value', valid arguments are: \n";
	message+=" time (n>=0), maxDepth (0<n<100), pass (0.0<=n<=1.0), prune (0.0<=n<=1.0), maxStatusDepth (0<n<100),\n";
	message+=" [useNChildren|useNChildrenPercent 0%<=n<=100%]\n";
	message+=" goal ";
	message+=Goal::getGoalStringParameterSyntax();
	message+="\n";
	message+=" nextToPlay (BLACK|WHITE)\n";
	return message;
}

void MTDFSearch::saveSettings()
{
	savedSettings.push(settings);
}

void MTDFSearch::restoreSettings()
{
	if(savedSettings.empty())
		return;

	settings = savedSettings.top();
	savedSettings.pop();
}

void MTDFSearch::printSearchSettings(ostream& out) const
{
	out << printSearchSettings();
}

string MTDFSearch::printSearchSettings() const
{
	string message;
	char buffer[200];
	sprintf(buffer, "maxDepth = %d\nuseNChildren = %d\nuseNChildrenPercent = %g\n", getDepth(),
		getUseNChildren(), getUseNChildrenPercent());
	message+=buffer;
	if(getUseMoveQualityThreshold())
		message+="useMoveQualityThreshold = true\n";
	else
		message+="useMoveQualityThreshold = false\n";
	if(getUseBestMoveFirst())
		message+="bestMoveFirst = true\n";
	else
		message+="bestMoveFirst = false\n";
	if(getUseEtc())
		message+="etc = true\n";
	else
		message+="etc = false\n";
	if(getUseNullMovePruning())
		message+="useNullMovePruning = true\n";
	else
		message+="useNullMovePruning = false\n";
	if(getUseTranspositionTable())
		message+="useTranspositionTable = true\n";
	else
		message+="useTranspositionTable = false\n";

	sprintf(buffer, "pruneValue = %g\n", getPruneValue());
	message+=buffer;
	
	sprintf(buffer, "passValue = %g\n", getPassValue());
	message+=buffer;

	if(getNextToPlay()==BLACK)
		message+="nextToPlay = BLACK\n";
	else
		message+="nextToPlay = WHITE\n";

	string s;
	getGoal().printInfo(&s);
	message+=s;

	sprintf(buffer, "timeLimit = %d\n", getTimeLimit());
	message+=buffer;
	return message;
}

void MTDFSearch::printDebugMessage(const char* buffer, const int currentDepth)
{
	// print an indent relative to the depth
	for(int i=0;i<currentDepth;i++)
		LogWriter::printerr(" ");
	LogWriter::printerr(buffer);
}

Node* MTDFSearch::lookupPtr(const Node& findMe)
{
	// the pointer n may have been changed during the AB call above so refind it now
	TRANSPOSITION_TABLE::iterator iter;
	if((iter=transpositionTable->find(findMe))==transpositionTable->end())
		return NULL;
	// This returnMe pointer doesn't seem to be returned to the calling function...!!!
	Node* returnMe = &(*iter);
	return returnMe;
}


bool MTDFSearch::lookup(const Node& findMe, Node& returnMe)
{
	if(!getUseTranspositionTable()) return false;
	TRANSPOSITION_TABLE::iterator iter;
	if((iter=transpositionTable->find(findMe))==transpositionTable->end())
		return false;
	returnMe = *iter;
	return true;
}

Node* MTDFSearch::getBestNode(const Node& parent) const
{
	if(getUseTranspositionTable()) {
		if(!parent.getHaveBestNode())
			return NULL;

		// setup the node to search for
		static Node bestNode;
		bestNode.setKey(parent.getBestKey());
		bestNode.setKey2(parent.getBestKey2());
		bestNode.setPreviousKey(parent.getBestPreviousKey());
		bestNode.setPreviousKey2(parent.getBestPreviousKey2());
		bestNode.setNextToPlay(parent.getBestNextToPlay());
		bestNode.setX(parent.getBestX());
		bestNode.setY(parent.getBestY());

		TRANSPOSITION_TABLE::iterator iter;
		if((iter=transpositionTable->find(bestNode))==transpositionTable->end())
			return NULL;
		else
			return &(*iter);
	}
	return NULL;
}

/** @brief Construct the best line of play from the last search
  * using the transposition table.
  * @param root The node in the ttable to start the line of play from. */
void MTDFSearch::updateBestLineOfPlay(Node& root, const int boardSize)
{
	// update the bestLineOfPlay
	bestLineOfPlay.clear();
	Node* current = &root;
	Node* peek;
	vector<Node*> loopCatch;

	while((peek=getBestNode(*current))!=NULL) {
		current = peek;
		// check we're not looping forever e.g. only pass moves in t-table
		// then could say bestmove is pass white next, then bestmove is pass
		// black next, then bestmove is pass white next etc...
		if(find(loopCatch.begin(), loopCatch.end(), current)!=loopCatch.end())
			break;
		loopCatch.push_back(current);
		bestLineOfPlay.push_back(current->getMove());
	}

	// Some goal functions will emulate a lookahead of n moves such as the ladder
	// search which traces the path of the ladder. In which case the next move
	// down that line of play won't be stored in the Transposition table but marked
	// in the node simply as the bestX, bestY coords.
	// Check if that's the case and add the extra move to the end of the line of play.
	if(current->getBestX()>-1 && current->getBestY()>-1) {
		bestLineOfPlay.push_back(
			Move(current->getBestX(), current->getBestY(), current->getBestNextToPlay(), boardSize));
	}
}