#include "TDBPNTrainer.h"
#include <ctime>

const float TDBPNTrainer::lambdaMinDefault = 0.0f;

/** @brief Set up the matrices so that they are the correct size for
  * the network this TdBPNTrainer class is going to train on. */
void TDBPNTrainer::resizeMatrices()
{
	int output = internalBPN->weights.at(internalBPN->weights.size()-1).getWidth();
	history.resize(internalBPN->weights.size(), 1);
	biasHistory.resize(internalBPN->weights.size(), 1);
	totalWeightChanges.resize(0);
	totalBiasWeightChanges.resize(0);

	for(int i=0;i<internalBPN->weights.size();i++) {
		Matrix<float>& w = internalBPN->weights.at(i);
		int ww = w.width;
		int wh = w.height;
		Matrix<float>& bw = internalBPN->biasWeights.at(i);
		int bww = bw.width;
		int bwh = bw.height;

		totalWeightChanges.push_back(Matrix<float>(ww, wh));
		totalBiasWeightChanges.push_back(Matrix<float>(bww, bwh));

		ObjectMatrix<Matrix<float> >& h = history.getValue(i, 0);
		h.resize(ww, wh);
		ObjectMatrix<Matrix<float> >& bh = biasHistory.getValue(i, 0);
		bh.resize(bww, bwh);
		// don't allocate memory here if profiling or testing
		for(int x=0;x<ww;x++) {
			for(int y=0;y<wh;y++)
				h.getValue(x, y).resize(output, 1);
		}
		for(x=0;x<bww;x++) {
			for(int y=0;y<bwh;y++)
				bh.getValue(x, y).resize(output, 1);
		}
	}

	// column vectors (matrices) to remember output of network
	previousPrediction.resize(1, output);
	currentPrediction.resize(1, output);
	
	resizeMatricesFlag = true;
}

/** @brief This must be run at the start of each epoch or sequence of
  * training data to ensure the history weight change vectors are reset. */
void TDBPNTrainer::startEpoch()
{
	if(!resizeMatricesFlag)
		resizeMatrices();
	int hiw, hih, bhiw, bhih;
	for(int i=0;i<totalWeightChanges.size();i++) {
		totalWeightChanges.at(i).initMatrix();
		internalBPN->lastWeightChanges.at(i).initMatrix();
		totalBiasWeightChanges.at(i).initMatrix();
		internalBPN->lastBiasWeightChanges.at(i).initMatrix();
		ObjectMatrix<Matrix<float> >& hi = history.getValue(i, 0);
		hiw = hi.getWidth();
		hih = hi.getHeight();
		for(int x=0;x<hiw;x++) {
			for(int y=0;y<hih;y++)
				hi.getValue(x, y).initMatrix();
		}
		ObjectMatrix<Matrix<float> >& bhi = biasHistory.getValue(i, 0);
		bhiw = bhi.getWidth();
		bhih = bhi.getHeight();
		for(x=0;x<bhiw;x++) {
			for(int y=0;y<bhih;y++)
				bhi.getValue(x, y).initMatrix();
		}
	}
	previousPrediction.initMatrix();
	currentPrediction.initMatrix();
}

/** Train the neural network against a single input, output training pair through one epoch.
@param input The input matrix.
@param output The required output matrix. 
@param freezeLayers A vector of layer indices to specifically NOT train.
@param previousMSE The mean squared error of the network from the last pattern trained.
@param doMomentum If true use the momentum term to add some of every previous training
generations weight change to the current weight change.
@param batchUpdate If true update the network weights every iteration of the training set,
if false update after every weight change calculation. */
bool TDBPNTrainer::trainPattern(const Matrix<float>& input, const Matrix<float>& output, 
			const vector<int>& freezeLayers, float& previousMSE, bool doMomentum /*=true*/, bool batchUpdate /*=false*/)
{

	/** @todo Write the TD algorithm here!!! This is the old, possibly wrong, TD training code... 
	  * Should we be training on a sequence forwards or backwards? 
	  * Don't allow momentum terms... */

	// This function should carry out the formula:
	// weightChanges = a(Yt+1-Yt)*sum[k=1,t]((lambda^(t-k))*dwYk)
	// + momentum factor is required
	// where a is the learning rate
	// Yt is the previous state prediction 
	// Yt+1 is the current state prediction
	// t is the time step
	// lambda is some real value [0,1]
	// dwYk is the gradient (vector of partial derivatives of Yk
	//                       with respect to w)
	
	// to enable incremental updating:
	//	 - Yt+1 must be saved at the end of each call
	//   - if et = sum[k=1,t]((lambda^(t-k))*dwYk)
	//     then et+1 = (dwYt+1)+lambda*et
	//     so we must remember also the last value of et at the end of each call
	// **see Chan-Temporal Difference Learning in the game of Go
	//   for derivation of TD formula for multi-layer network

	// special case for last training pattern in an epoch when output!=NULL
	if(output!=NULL) {
		// update the weights and bias units but don't calculate errors etc...
		currentPrediction = *output;

		// if this is the final state and we are going backwards
		// then we need a previous prediction to calculate the error with
		if(backwards) {
			if(!getAnswer(input))
				return false;
			previousPrediction = outputs.at(weightsSize);
		}
	}
	else {
		if(!getAnswer(input))
			return false;
		currentPrediction = outputs.at(weightsSize);
	}

	/***************************/
	/** Update weight changes **/
	/***************************/

	// weight change = alpha(T(CurrentPrediction-PreviousPrediction)).History
	//					+momentum(oldWeightChange)

	// temp = alpha(T(CurrentPrediction-PreviousPrediction))
	Matrix<float> temp2;
	if(backwards) {
		if(output!=NULL)
			currentPrediction.subtract(previousPrediction, temp2);
		else
			previousPrediction.subtract(currentPrediction, temp2);
	}
	else
		currentPrediction.subtract(previousPrediction, temp2);

	float p;
	Matrix<float> temp, h;
	temp2*=learningRate;
	temp2.transpose(temp);

	// now do history vector and momenutum terms
	for(int l=0;l<weightsSize;l++) {
		Matrix<float>& weightsl = weights.at(l);
		Matrix<float>& lastWeightChangesl = lastWeightChanges.at(l);
		// for each weight, dot product by the related history vector
		for(int y=0;y<weightsl.height;y++) {
			ObjectMatrix<Matrix<float> >& historyl = history.getValue(l, 0);
			for(int x=0;x<weightsl.width;x++) {
				// p = temp.history  (vector dot product)
				h = historyl.getValue(x, y);
				p = 0;
				for(int s=0;s<h.width;s++)
					p+=temp.getValue(s, 0)*h.getValue(s, 0);
				if(doMomentum)
					p+=lastWeightChangesl.getValue(x, y)*momentum;
				lastWeightChangesl.setValue(x, y, p);
			} // end for x
		} // end for y
		// store weight changes for batch updating when endEpoch is called
		totalWeightChanges.at(l)+=lastWeightChangesl;

		// do weight changes for bias weights too
		Matrix<float>& biasWeightsl = biasWeights.at(l);
		Matrix<float>& lastBiasWeightChangesl = lastBiasWeightChanges.at(l);
		// for each weight, dot product by the related history vector
		for(y=0;y<biasWeightsl.height;y++) {
			ObjectMatrix<Matrix<float> >& biasHistoryl = biasHistory.getValue(l, 0);
			for(int x=0;x<biasWeightsl.width;x++) {
				// p = temp.history  (vector dot product)
				h = biasHistoryl.getValue(x, y);
				p = 0;
				for(int s=0;s<h.width;s++)
					p+=temp.getValue(s, 0)*h.getValue(s, 0);
				if(doMomentum)
					p+=lastBiasWeightChangesl.getValue(x, y)*momentum;
				lastBiasWeightChangesl.setValue(x, y, p);
			} // end for x
		} // end for y
		totalBiasWeightChanges.at(l)+=lastBiasWeightChangesl;
	} // end for l


	// is this the last pattern in the sequence i.e. the pattern with the real scores?
	// if so then we don't need to calculate errors etc...
	if(output!=NULL)
		return true;

	/**********************/
	/** Calculate errors **/
	/**********************/

	// output error = diag(f'(output))
	Matrix<float>& outputsWeightsSize = outputs.at(weightsSize);
	temp.resize(outputsWeightsSize.width, outputsWeightsSize.height);
	BPN::DActivationFN(outputsWeightsSize, temp);
	Matrix<float>& errorswS1 = errors.at(weightsSize-1);
	errorswS1.resize(temp.height, temp.height);
	// diag(temp)
	for(int y=0;y<temp.height;y++)
		errorswS1.setValue(y, y, temp.getValue(0, y));

	// hidden layer errors = f'(layer outputs)@((layer weights)x(T((next layer errors))))
	// where a@b = multiply each element i in a to each element in b row i
	Matrix<float> n, result;
	for(l=(weightsSize-1);l>0;l--) {
		// temp = f'(layer outputs)
		Matrix<float>& outputsl = outputs.at(l);
		Matrix<float>& weightsl = weights.at(l);
		temp.resize(outputsl.width, outputsl.height);
		BPN::DActivationFN(outputsl, temp);
		// result = ((layer weights)x(T(next layer errors)))
		errors.at(l).transpose(n);
		result.resize(n.width, weightsl.height);
		weightsl.multiply(n, result);
		// errors.at(l-1) = temp@result
		Matrix<float>& errorsl1 = errors.at(l-1);
		errorsl1.resize(result.width, result.height);
		for(int y=0;y<result.height;y++) {
			float tempy = temp.getValue(0, y);
			for(int x=0;x<result.width;x++)
				errorsl1.setValue(x, y, result.getValue(x, y)*tempy);
		} // end for y
	} // end for l

	/************************************/
	/* Update derivative history matrix */
	/************************************/

	// Update ordinary history vectors first
	updateHistoryVectors(history, lambda);
	// do history matrices for bias weights too
	updateHistoryVectors(biasHistory, lambda);

	// save previous prediction
	previousPrediction = currentPrediction;

	internalBPN->patternsCompleted++;

	return true;
} // end trainPattern(Matrix input, Matrix output)

void TDBPNTrainer::updateHistoryVectors(HVECTOR& h, float lambda)
{
	Matrix<float>& htemp = h.getValue(0, 0).getValue(0, 0);
	Matrix<float> temp(htemp.width, htemp.height);
	for(int l=0;l<h.getWidth();l++) {
		ObjectMatrix<Matrix<float> >& hl = h.getValue(l, 0);
		Matrix<float>& errorsl = errors.at(l);
		Matrix<float>& outputsl = outputs.at(l);
		for(int x=0;x<hl.getWidth();x++) {
			for(int y=0;y<hl.getHeight();y++) {
				Matrix<float>& hw = hl.getValue(x, y);
				float outputsly = outputsl.getValue(0, y);
				for(int q=0;q<errorsl.width;q++)
					temp.setValue(q, 0, errorsl.getValue(q, x)*outputsly);
				// now temp = error*outputForLayer				
				hw*=lambda;     // lambda*historyVector
				hw+=temp;       // add temp to history vector to finish update
			} // end for y
		} // end for x
	} // end for l
}