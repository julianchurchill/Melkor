#ifndef TDBPNTRAINER_H
#define TDBPNTRAINER_H

#pragma warning( disable : 4786 )

#include "BPNTrainer.h"
#include "Matrix.h"
#include "ObjectMatrix.h"
#include <vector>
using namespace std;

/** @brief This class defines trainPattern() for the TD method of reinforcement
  * learning. See "Reinforcement Learning", Sutton & Barto, 1998. */
class TDBPNTrainer : public virtual BPNTrainer
{
	public:

		typedef ObjectMatrix<ObjectMatrix<Matrix<float> > > HVECTOR;
		static const float lambdaMinDefault;

	private:
		float lambda;
		
		vector<Matrix<float> > totalWeightChanges;
		vector<Matrix<float> > totalBiasWeightChanges;
		// for each layer, for each weight x,y, there is a Matrix of length outputNeurons
		HVECTOR history;
		HVECTOR biasHistory;

		Matrix<float> previousPrediction, currentPrediction;
		bool resizeMatricesFlag;

		void resizeMatrices();

	public:

		TDBPNTrainer(BPN* bpn, int p1, int p2) : BPNTrainer(bpn, p1, p2), resizeMatricesFlag(false),
												 lambda(0.5f) {};
		virtual ~TDBPNTrainer() {};

		void setLambda(float newLambda) { lambda = newLambda; };
		HVECTOR& getHistory() { return history; };
		HVECTOR& getBiasHistory() { return biasHistory; };

		// This needs to be run to reset the history vectors and temporary variables
		// at the start of each sequence...
		void startEpoch();


		// Overridden from BPNTrainer
		virtual bool trainPattern(const Matrix<float>& input, const Matrix<float>& output, 
			const vector<int>& freezeLayers, float& previousMSE, bool doMomentum = true, bool batchUpdate = false);

		void updateHistoryVectors(HVECTOR& h, float lambda);
};

#endif