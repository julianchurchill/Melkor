#include "LibertyCountGoal.h"
#include <math.h>

bool LibertyCountGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
												   const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	for(int i=0;i<target.size();i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, target.size(), factorInSolutionSize, nextToPlay);
}

bool LibertyCountGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b,
												  const int targetSize, bool factorInSolutionSize, const int nextToPlay)
{
	bool returnValue = evaluate(ourColour, score, b, nextToPlay);

	if(factorInSolutionSize)
		score = score/targetSize;

	return returnValue;
}

// Game tree search can handle +.v.e. and -.v.e. evaluation scores
bool LibertyCountGoal::evaluate(const int ourColour, float &score, const BoardStruct &b,
												 const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	score = 0.0f;
	// count our liberties and the enemies liberties
	int libs = 0;

	const list<GoString>& strings = b.getStrings();
	list<GoString>::const_iterator liter = strings.begin();
	for(;liter!=strings.end();liter++) {
		if(liter->getColour()==ourColour)
			libs+=liter->getLibertyCount();
		else
			libs-=liter->getLibertyCount();
	}
	score = libs;
	return true;
}