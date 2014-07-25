#ifndef BPNTRAINER_H
#define BPNTRAINER_H

#pragma warning( disable : 4786 )

#include "BPN.h"
#include "Matrix.h"
#include <vector>
using namespace std;

class BPNTrainer
{
	protected:
		BPN* internalBPN;
		// lengths of each phase, competitive then output training
		int phase1, phase2;

		vector<Matrix<float> > summedOutput;
		vector<Matrix<float> > averageSummedOutput;
		vector<Matrix<float> > summedError;
		vector<Matrix<float> > averageSummedError;
		vector<Matrix<float> > averageErrorDerivative;

	public:

		BPNTrainer(BPN* bpn, int p1, int p2) : internalBPN(bpn), phase1(p1), phase2(p2)
		{
			if(internalBPN==NULL)
				throw "NULL BPN* for BPNTrainer constructor!!!";

			// init the temporary variables
			summedOutput.resize(internalBPN->weightsSize+1);
			averageSummedOutput.resize(internalBPN->weightsSize+1);

			summedError.resize(internalBPN->weightsSize);
			averageSummedError.resize(internalBPN->weightsSize);
			averageErrorDerivative.resize(internalBPN->weightsSize);

			for(int l=0;l<internalBPN->weightsSize;l++)
			{
				summedOutput[l].resize(internalBPN->outputs[l].width, internalBPN->outputs[l].height);
				averageSummedOutput[l].resize(internalBPN->outputs[l].width, internalBPN->outputs[l].height);

				// remember summedErrors will be post-transpose so 
				// we need to be height x width
				summedError[l].resize(internalBPN->errors[l].height, internalBPN->errors[l].width);
				averageSummedError[l].resize(internalBPN->errors[l].height, internalBPN->errors[l].width);

				// derivative is per weight
				averageErrorDerivative[l].resize(internalBPN->weights[l].width, internalBPN->weights[l].height);

				for(int i=0;i<summedOutput[l].getHeight();i++)
					for(int j=0;j<summedOutput[l].getWidth();j++)
						summedOutput[l].setValue(j, i, 0.0f);

				for(i=0;i<averageSummedOutput[l].getHeight();i++)
					for(int j=0;j<averageSummedOutput[l].getWidth();j++)
						averageSummedOutput[l].setValue(j, i, 0.0f);

				for(i=0;i<summedError[l].getHeight();i++)
					for(int j=0;j<summedError[l].getWidth();j++)
						summedError[l].setValue(j, i, 0.0f);

				for(i=0;i<averageSummedError[l].getHeight();i++)
					for(int j=0;j<averageSummedError[l].getWidth();j++)
						averageSummedError[l].setValue(j, i, 0.0f);

				for(i=0;i<averageErrorDerivative[l].getHeight();i++)
					for(int j=0;j<averageErrorDerivative[l].getWidth();j++)
						averageErrorDerivative[l].setValue(j, i, 0.0f);

/*				summedOutput[l].setAllValues(0.0f);
				averageSummedOutput[l].setAllValues(0.0f);
				summedError[l].setAllValues(0.0f);
				averageSummedError[l].setAllValues(0.0f);
				averageErrorDerivative[l].setAllValues(0.0f);
*/
			}

			// the output matrices are a different shape and size to the others
			// and include one more layer then the number of weight layers
			summedOutput[internalBPN->weightsSize].resize(internalBPN->outputs[internalBPN->weightsSize].width,
				internalBPN->outputs[internalBPN->weightsSize].height);

		};
		virtual ~BPNTrainer() {};

		// Training
		virtual double trainAndTime(vector<Matrix<float> >& input, 
			vector<Matrix<float> >& output, const vector<int>& freezeLayers, bool print = false, 
			int epochs = 0, bool useSymmetry = false, bool doMomentum = true,
			bool batchUpdate = false, bool convergenceCheck = true, bool printProgress = true);
		virtual int trainWithDatabase(NNDatabase& db, const vector<int>& freezeLayers, int epochs = 0, float tolerance = 0.1, 
			int saveEveryNEpochs = 10, bool useSymmetry = false, 
			bool doMomentum = true, bool batchUpdate = false, bool convergenceCheck = true);

		// The functions below call trainPattern() directly
		virtual int trainAndPrint(vector<Matrix<float> >& input, 
			vector<Matrix<float> >& output, const vector<int>& freezeLayers, int epochs = 0, 
			float tolerance = (float)0.1, int saveEveryNEpochs = 10,
			bool useSymmetry = false, bool doMomentum = true, bool batchUpdate = false, bool convergenceCheck = true);
		virtual int train(vector<Matrix<float> >& input, vector<Matrix<float> >& output,
			const vector<int>& freezeLayers, int epochs = 0, float tolerance = (float)0.1, 
			int saveEveryNEpochs = 10, bool useSymmetry = false, bool doMomentum = true,
			bool batchUpdate = false, bool convergenceCheck = true, bool printProgress = true);

		virtual bool trainPattern(const Matrix<float>& input, const Matrix<float>& output, 
			const vector<int>& freezeLayers, float& previousMSE, bool doMomentum = true, bool batchUpdate = false);

};

#endif