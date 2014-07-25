#ifndef NEWBPN11GOADAPTER_H
#define NEWBPN11GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 4 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR, EMPTY or OFFBOARD in that order.                               **/
/** 2 units indicating distance to two nearest board edges.                    **/
/*********************************************************************************/

class newBPN11GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 4 nodes per board point, 2 extra nodes
		newBPN11GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 4, 2)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN11GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;

};

#endif
