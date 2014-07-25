#ifndef URGENCY1GOADAPTER_H
#define URGENCY1GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 5 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR and EMPTY in that order. The last 2 units per point             **/
/** are to mark where the moves to compare are, MOVE A and MOVE B.              **/
/** Note: No OFFBOARD unit is needed since we're feeding the whole board as     **/
/** input.                                                                      **/
/*********************************************************************************/

class Urgency1GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 5 nodes per board point, 0 extra nodes
		Urgency1GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 5, 0)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~Urgency1GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const 
		{ return false; };

		// for comparing two moves
		virtual bool getInput(int x1, int y1, int x2, int y2, const BoardStruct& b, Matrix<float>& input,
			int colour) const;
};

#endif
