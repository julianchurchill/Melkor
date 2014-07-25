#ifndef SAFETY1BPNGOADAPTER_H
#define SAFETY1BPNGOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
using namespace std;

/** @brief This class is designed to assign a score to a 
  * group or string of stones according to its safety. */
class Safety1BPNGoAdapter : public BPNGoAdapterInterface
{
	public:

		// 3 nodes per board point
		Safety1BPNGoAdapter (const string& filename) : BPNGoAdapterInterface(filename, 3, 0)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());
		};
		virtual ~Safety1BPNGoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;
};

#endif
