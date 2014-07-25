#include "StoneCountGoal.h"
#include <math.h>

bool StoneCountGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
												 const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	for(int i=0;i<target.size();i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, target.size(), factorInSolutionSize, nextToPlay);
}

bool StoneCountGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b,
												const int targetSize, bool factorInSolutionSize, const int nextToPlay)
{
	bool returnValue = evaluate(ourColour, score, b, nextToPlay);

	if(factorInSolutionSize)
		score = score/targetSize;

	return returnValue;
}

// Game tree search can handle +.v.e. and -.v.e. evaluation scores
bool StoneCountGoal::evaluate(const int ourColour, float &score, const BoardStruct &b,
							  const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	// count our stones and the enemies stones
	int stones = 0;

	const list<GoString>& strings = b.getStrings();
	list<GoString>::const_iterator liter;
	for(liter = strings.begin();liter!=strings.end();liter++) {
		if(liter->getColour()==ourColour)
			stones+=liter->getStoneCount();
		else
			stones-=liter->getStoneCount();
	}
	score = stones;
	return true;
}