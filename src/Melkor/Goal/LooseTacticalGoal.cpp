#include "LooseTacticalGoal.h"
#include "Misc.h"
#include "Go.h"

bool LooseTacticalGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
											  const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	// add the genomes moves to the boardstruct
	int targetSize = target.size();
	for(int i=0;i<targetSize;i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, targetSize, factorInSolutionSize, nextToPlay);
}

bool LooseTacticalGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b, const int targetSize,
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
bool LooseTacticalGoal::evaluate(const int maximisingColour, float &score, const BoardStruct &b,
										   const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	if(b.getPoint(arg1, arg2)==EMPTY) {
		score = DEAD;
		return true;
	}
	
	// If we've connected to an unconditionally alive string or we've made 2 eyes...
	BoardStruct::constStringsIterator citer(b.getStringConst(arg1, arg2));
	if(citer->getStatus()==ALIVE || citer->getRealEyeCount()>=2) {
		score = ALIVE;
		return true;
	}
	else
		score = citer->getLibertyCount();

	return false;
}