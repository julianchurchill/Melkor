#ifndef CNBPNTRAINER_H
#define CNBPNTRAINER_H

#pragma warning( disable : 4786 )

#include "BPNTrainer.h"
#include "Matrix.h"
#include <vector>
using namespace std;

// This training method is based on Adrian Cook's PhD thesis work

class CNBPNTrainer : public virtual BPNTrainer
{
	public:

		CNBPNTrainer(BPN* bpn, int p1, int p2) : BPNTrainer(bpn, p1, p2) {};
		virtual ~CNBPNTrainer() {};

		virtual bool trainPattern(const Matrix<float>& input, const Matrix<float>& output, 
			const vector<int>& freezeLayers, bool doMomentum = true, bool batchUpdate = false);

};

#endif