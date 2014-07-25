#ifndef NEWBPN10GOADAPTER_H
#define NEWBPN10GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 4 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR, EMPTY or OFFBOARD in that order.                               **/
/*********************************************************************************/

class newBPN10GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 4 nodes per board point, 0 extra nodes
		newBPN10GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 4, 0)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN10GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
