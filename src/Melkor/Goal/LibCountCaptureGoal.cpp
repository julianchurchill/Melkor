#include "LibCountCaptureGoal.h"
#include "Misc.h"

bool LibCountCaptureGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
											  const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	int targetSize = target.size();
	for(int i=0;i<targetSize;i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, targetSize, factorInSolutionSize, nextToPlay);
}

bool LibCountCaptureGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
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
bool LibCountCaptureGoal::evaluate(const int maximisingColour, float &score, const BoardStruct &b,
													const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	/** @todo Create a new goal called LifeDeathGoal(). Include in it this eval
	  * enhanced with an eye-space pattern matcher to detect life/death several moves ahead. 
	  * Also include ladder detection and evaluation. 
	  * Isolated single stones, or 2 stone strings should be regarded as DEAD.
	  * Deal with eye spaces better - 2 separate eye spaces is ALIVE rather 
	  * than checking for 1-space real eyes. */
	
	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	if(b.getPoint(arg1, arg2)==EMPTY) {
		score = DEAD;
		return true;
	}
	
	if(b.getStringConst(arg1, arg2)->getRealEyeCount()>=2)
		score = ALIVE;
	else {
		// Include a liberty count in the score
		score = b.getStringConst(arg1, arg2)->getLibertyCount();
	}

	return false;
}