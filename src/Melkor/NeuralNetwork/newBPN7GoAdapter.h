#ifndef NEWBPN7GOADAPTER_H
#define NEWBPN7GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 2 units per board point, OURCOLOUR (+1, 0), THEIRCOLOUR (-1, 0),            **/
/** EMPTY (0, +1) and OFFBOARD (0, -1).                                         **/
/*********************************************************************************/

class newBPN7GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 2 nodes per board point, 0 extra nodes
		newBPN7GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 2, 0)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN7GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
