#ifndef BPNGOADAPTER_H
#define BPNGOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPNGoAdapterInterface.h"
#include "BPN.h"
#include "Matrix.h"
#include "NNDatabase.h"
#include "BoardStruct.h"
#include "SGFReader.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class BPNGoAdapter : public BPNGoAdapterInterface
{
	public:

		// 1 nodes per board point, 0 extra nodes
		BPNGoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 1, 0)
		{
			patternWidth = calculatePatternWidth(getBPN());
			patternHeight = calculatePatternHeight(getBPN());
		};
		virtual ~BPNGoAdapter() {};

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;

		virtual int calculateInputHeight(const BPN& bpn, int pWidth, int pHeight) const
		{ return bpn.getWeights()[0].getHeight(); };
};

#endif