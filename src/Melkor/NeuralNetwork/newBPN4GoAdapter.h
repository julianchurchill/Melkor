#ifndef NEWBPN4GOADAPTER_H
#define NEWBPN4GOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class is designed to be a network of the form:                         **/
/** 4 units per board point, 1 each indicating whether the point is OURCOLOUR,  **/
/** THEIRCOLOUR, EMPTY or OFFBOARD in that order.                               **/
/** 18 units indicating distance to two nearest board edges.                    **/
/** 16 units indicating liberties of each string north, south, east and west    **/
/** of point being considered. 1 neuron indicating each of 1,2,3,>=4 liberties. **/
/*********************************************************************************/

class newBPN4GoAdapter : public BPNGoAdapterInterface
{
	public:

		// 4 nodes per board point, 18+16 extra nodes
		newBPN4GoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 4, 34)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());;
		};
		virtual ~newBPN4GoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;
		void setLibertyNeurons(int base, const BoardStruct& board, int x, int y, Matrix<float>& input) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
