#include "LifeDeathGoal.h"
#include "Misc.h"
#include "Go.h"

bool LifeDeathGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
											  const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	int targetSize = target.size();
	for(int i=0;i<targetSize;i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, targetSize, factorInSolutionSize, nextToPlay);
}

bool LifeDeathGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
											 bool factorInSolutionSize, const int nextToPlay)
{
	bool returnValue = evaluate(ourColour, score, b, nextToPlay);

	if(factorInSolutionSize)
		score = score/targetSize;

	return returnValue;
}

// game tree search can handle +.v.e. and -.v.e. evaluations
// NOTE: Game tree searches with capture goals should _always_ be
// from the point of view of the string being attacked,
// i.e. maximisingColour = b.getString(arg1, arg2)
// ALIVE is positive and DEAD is negative.
// Note: ALIVE is guaranteed to be greater than the maximum possible
// liberties for a string on a board size == to b.size.
bool LifeDeathGoal::evaluate(const int maximisingColour, float &score, const BoardStruct &b, 
											  const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	/** @todo
	  * Enhance with an eye-space pattern matcher to detect life/death several moves ahead. 
	  * Possibly include ladder detection and evaluation. 
	  * Analyse eye spaces. 2 seperate eye spaces means ALIVE. Potential for 2 eyes means ALIVE. 
	  *
	  * A given string is alive if it's parent 'group/dragon' has a large enough eye space.
	  * This should be a minimum value of 6 (the smallest completely enclosed eye space 
	  * that always gives 2 eyes). Strings (or parent groups) that have smaller eye spaces
	  * must be searched until 2 eyes are made (or equivalent patterns are matched giving >=2 eyes).
	  * An upper limit on the eye space size should also be included, since the enemy may be
	  * able to live within the eye space and kill the string/group from within...
	  *
	  * This goal doesn't need to be broken down into creating eyes, connecting, escaping etc...
	  * since eyes are checked for here, and connecting and escaping should be implicit in the search
	  * when trying to achieve a 'large' enough eye space.
	  */

	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	if(b.getPoint(arg1, arg2)==EMPTY) {
		score = DEAD;
		return true;
	}
	
	// If we've connected to an unconditionally alive string or we've made 2 eyes...
	BoardStruct::constStringsIterator target(b.getStringConst(arg1, arg2));
	if(target->getStatus()==ALIVE || target->getRealEyeCount()>=2) {
		score = ALIVE;
		return true;
	}
	else {
		/** @todo Check size of available eye space here */

		/** @todo Do eye pattern matching here.
		  * If we have a total of 2 eyes return ALIVE, otherwise liberty count. 
		  *
		  * Use a 3 step minimax-type exchange of eye killing/creating moves to determine
		  * our best possible result. We need to know who is to play next here.
		  * This may result in an urgent situation, where to get 2 eyes we _must_ play
		  * first. In which case return what ???
		  */

		// Include a liberty count in the score
		score = target->getLibertyCount();
	}

	return false;
}