#include "InfluenceGoal.h"
#include "Misc.h"
#include <math.h>

bool InfluenceGoal::successWithBoardBeforeMoves(const vector<Move>& target, const int ourColour, float& score,
												const BoardStruct& b, bool factorInSolutionSize, const int nextToPlay)
{
	BoardStruct temp(b);

	for(int i=0;i<target.size();i++)
		temp.setPoint(target[i].getX(), target[i].getY(), target[i].getColour());

	return successWithBoardAfterMoves(ourColour, score, temp, target.size(), factorInSolutionSize, nextToPlay);
}

bool InfluenceGoal::successWithBoardAfterMoves(const int ourColour, float& score, const BoardStruct& b,
											   const int targetSize, bool factorInSolutionSize, const int nextToPlay)
{
	bool returnValue= evaluate(ourColour, score, b, nextToPlay);

	if(factorInSolutionSize)
		score = score/targetSize;

	return returnValue;
}

// Game tree search can handle +.v.e. and -.v.e. evaluation scores
bool InfluenceGoal::evaluate(const int ourColour, float &score, const BoardStruct &b,
							 const int nextToPlay, int* bestX /*=NULL*/, int* bestY /*=NULL*/)
{
	if(bestX!=NULL) *bestX = -1;
	if(bestY!=NULL) *bestY = -1;

	score = 0.0f;
	// Influence Goal should always calculate the influence from BLACK's point of view
	return calculateInfluence(b, score, BLACK, "InfluenceGoal");
}