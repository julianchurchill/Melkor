#ifndef NEWBPN6GOADAPTER_H
#define NEWBPN6GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 4 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR, EMPTY or OFFBOARD in that order.                               **/
/** 18 units indicating distance to two nearest board edges.                    **/
/** The point that the last move was played is emphasised, i.e. has 2 as input  **/
/** instead of 1 or 0.                                                          **/
/*********************************************************************************/

class newBPN6GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 4 nodes per board point, 18 extra nodes
		newBPN6GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 4, 18)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN6GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;
		void setPointLastMove(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
