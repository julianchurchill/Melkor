#ifndef RPROPBPNTRAINER_H
#define RPROPBPNTRAINER_H

#pragma warning( disable : 4786 )

#include "BPNTrainer.h"
#include "Matrix.h"
#include <vector>
using namespace std;

/** @brief This class implements Reidmillers RPROP neural network training algorithm.
See Reidmiller - "Faster Backprop - The RPROP Algorithm" */
class RPROPBPNTrainer : public virtual BPNTrainer
{
	private:
		// the weight update-values must be saved between epochs to ensure RPROP
		// can use them again for the next epoch, they should be updated every epoch
		vector<Matrix<float> > updateValues;
		vector<Matrix<float> > biasUpdateValues;

		vector<Matrix<float> > previousSummedErrorDerivatives;
		vector<Matrix<float> > biasPreviousSummedErrorDerivatives;

		vector<Matrix<float> > lastBatchWeightChanges;
		vector<Matrix<float> > lastBatchBiasWeightChanges;

		// values recommended in Reidmiller to avoid underflow/overflow of the floating point unit
		float updateMax;
		const float updateMin;

		const float updateIncrease;
		const float updateDecrease;

	public:

		static const float updateMinDefault;

		RPROPBPNTrainer(BPN* bpn, int p1, int p2) : BPNTrainer(bpn, p1, p2), updateMax(50.0f), updateMin(updateMinDefault),
			updateIncrease(1.2f), updateDecrease(0.5f)
		{
			// initialise the update values
			init(internalBPN->getWeights(), updateValues, previousSummedErrorDerivatives,
				lastBatchWeightChanges);
			init(internalBPN->getBiasWeights(), biasUpdateValues,
				biasPreviousSummedErrorDerivatives, lastBatchBiasWeightChanges);
		}
		virtual ~RPROPBPNTrainer() {};

		float getUpdateMin() const { return updateMin; };
		float getUpdateMax() const { return updateMax; };
		void setUpdateMax(float m) { updateMax = m; };

		void init(const vector<Matrix<float> >& w, vector<Matrix<float> >& uv, 
			vector<Matrix<float> >& psed, vector<Matrix<float> >& lbwc)
		{
			uv.resize(internalBPN->weightsSize);
			psed.resize(internalBPN->weightsSize);
			lbwc.resize(internalBPN->weightsSize);
			for(int l=0;l<uv.size();l++)
			{
				Matrix<float>& updateValuesl = uv[l];
				Matrix<float>& previousSummedErrorDerivativesl = psed[l];
				Matrix<float>& lastBatchWeightChangesl = lbwc[l];

				int width = w[l].getWidth();
				int height = w[l].getHeight();
				updateValuesl.resize(width, height);
				previousSummedErrorDerivativesl.resize(width, height);
				lastBatchWeightChangesl.resize(width, height);
				for(int i=0;i<height;i++)
				{
					// initial update value of 0.1 is just a guess
					// according to Reidmiller this parameter is arbitrary
					// and will be quickly adapted during training
					for(int j=0;j<width;j++)
					{
						updateValuesl.setValue(j, i, 0.1f);
						// these should be 0, the summedErrorDerivative 
						// doesn't even need to be intialised but this keeps it 
						// tidy and will have no performance impact on actual training
						previousSummedErrorDerivativesl.setValue(j, i, 0.0f);
						lastBatchWeightChangesl.setValue(j, i, 0.0f);
					}
				}
			}
		}

		// The functions below call trainPattern() directly
		virtual int trainAndPrint(vector<Matrix<float> >& input, 
			vector<Matrix<float> >& output, const vector<int>& freezeLayers, int epochs = 0, 
			float tolerance = (float)0.1, int saveEveryNEpochs = 10,
			bool useSymmetry = false, bool doMomentum = true, bool batchUpdate = false,
			bool convergenceCheck = true)
		{
//			LogWriter::printerr("ERROR: Don't use this function, batchUpdate must always be used with RPROP!\n");
//			return 0;
			return internalTrainAndPrint(input, output, freezeLayers, epochs, tolerance, saveEveryNEpochs, useSymmetry, doMomentum);
		}
		virtual int train(vector<Matrix<float> >& input, vector<Matrix<float> >& output,
			const vector<int>& freezeLayers, int epochs = 0, float tolerance = (float)0.1, 
			int saveEveryNEpochs = 10, bool useSymmetry = false, bool doMomentum = true,
			bool batchUpdate = false, bool convergenceCheck = true, bool printProgress = true)
		{
//			LogWriter::printerr("ERROR: Don't use this function, batchUpdate must always be used with RPROP!\n");
//			return 0;
			return internalTrain(input, output, freezeLayers, epochs, tolerance, saveEveryNEpochs, useSymmetry, doMomentum,
				convergenceCheck, printProgress);
		}

		// Only use these functions since we need batch update to be on all the time!
		int internalTrainAndPrint(vector<Matrix<float> >& input, 
			vector<Matrix<float> >& output, const vector<int>& freezeLayers, int epochs = 0, 
			float tolerance = (float)0.1, int saveEveryNEpochs = 10,
			bool useSymmetry = false, bool doMomentum = true, bool convergenceCheck = true);
		int internalTrain(vector<Matrix<float> >& input, vector<Matrix<float> >& output,
			const vector<int>& freezeLayers, int epochs = 0, float tolerance = (float)0.1, 
			int saveEveryNEpochs = 10, bool useSymmetry = false, bool doMomentum = true,
			bool convergenceCheck = true, bool printProgress = true);

		void updateWeights(Matrix<float>& thisWeights, Matrix<float>& thisUpdateValues, 
			Matrix<float>& seDerivatives, const Matrix<float>& pseDerivatives, Matrix<float>& lbwc);

		virtual bool trainPattern(const Matrix<float>& input, const Matrix<float>& output, 
			const vector<int>& freezeLayers, float& previousMSE, bool doMomentum = true, bool batchUpdate = false);
};

#endif