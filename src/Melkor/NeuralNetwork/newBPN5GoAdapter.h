#ifndef NEWBPN5GOADAPTER_H
#define NEWBPN5GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 4 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR, EMPTY or OFFBOARD in that order.                               **/
/** 18 units indicating distance to two nearest board edges.                    **/
/** 1 unit indicating distance to last move.                                    **/
/*********************************************************************************/

class newBPN5GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 4 nodes per board point, 19 extra nodes
		newBPN5GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 4, 19)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN5GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
