#ifndef NEWBPN12GOADAPTER_H
#define NEWBPN12GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 3 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR, or EMPTY in that order.                                        **/
/** 18 units indicating distance to two nearest board edges.                    **/
/*********************************************************************************/

class newBPN12GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 3 nodes per board point, 18 extra nodes
		newBPN12GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 3, 18)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN12GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
