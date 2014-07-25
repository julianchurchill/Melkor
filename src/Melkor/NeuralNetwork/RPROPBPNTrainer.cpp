#include "RPROPBPNTrainer.h"

const float RPROPBPNTrainer::updateMinDefault = 0.000001f;

int RPROPBPNTrainer::internalTrainAndPrint(vector<Matrix<float> >& input, vector<Matrix<float> >& output,
					   const vector<int>& freezeLayers, int epochs /*=0*/, float tolerance /*=0.1*/,
					   int saveEveryNEpochs /*=10*/, bool useSymmetry /*=false*/,
					   bool doMomentum /*=true*/, bool convergenceCheck /*=true*/)
{
	// batch update must be on all the time for RPROP
	const bool batchUpdate = true;

	// input matrix must be 1 dimensional
	// and the width of the height of the input layer
	// i.e. a value for each neuron in the input layer
	// and a constant 1.0 at the front for the bias neuron
	// e.g. [ 1.0 9.0 2.0 ]

	// check size of training patterns matches size of network
	LogWriter log;
	string message;
	char buffer[200];
	if(input.size()<=0)
	{
		log.println("Input size = 0");
		log.println("Aborting training");
		return 0;
	}
	// add 1 to allow for bias node in layer[0]
	int size = input[0].getHeight()*input[0].getWidth()+1;
	if(size!=internalBPN->weights[0].getHeight())
	{
		log.println("Training pattern size does not match network");
		message = "Training pattern size: ";
		sprintf(buffer, "%d", size);
		message+=buffer;
		log.println(message);
		message = "Network input layer size: ";
		sprintf(buffer, "%d", internalBPN->weights[0].getHeight());
		message+=buffer;
		log.println(message);
		log.println("Aborting training");
		return 0;
	}

	Matrix<float> t;
	bool converged = false;
	int h, i, l;
	float tval, actval;
//	vector<Matrix<float> > temp(internalBPN->weights.size()+1);
	int epochsRun = 0;
	string in;

	internalBPN->errorCode = BPN::NONE;

	//MEMORYSTATUS memBuffer;

	// index of the transform to apply
	int height = input[0].getHeight();
	int width = input[0].getWidth();
	int root = sqrt(height);
	Matrix<float> converted(root, root);
	Matrix<float> transformed(root, root);
	Matrix<float> newInput(width, height);
	int widthOut = output[0].getWidth();
	int heightOut = output[0].getHeight();
	int rootOut = sqrt(heightOut);
	Matrix<float> convertedOutput(rootOut, rootOut);
	Matrix<float> transformedOutput(rootOut, rootOut);
	Matrix<float> newOutput(widthOut, heightOut);
	float previousMSE = 0.0f;

	// for *-1 the errorDerivatives before summing
	vector<Matrix<float> > negatedED, negatedBiasED;
	negatedED.resize(internalBPN->weightsSize);
	negatedBiasED.resize(internalBPN->weightsSize);
	for(i=0;i<internalBPN->weightsSize;i++)
	{
		negatedED[i].resize(internalBPN->errorDerivatives[i].getWidth(), internalBPN->errorDerivatives[i].getHeight());
		negatedBiasED[i].resize(internalBPN->biasErrorDerivatives[i].getWidth(), 
			internalBPN->biasErrorDerivatives[i].getHeight());
	}

	// repeatedly do trainPattern()
	// until target output within tolerance
	while (!converged)
	{
		converged = true;
		// reset summed error derivatives before each epoch
		for(l=0;l<internalBPN->weightsSize;l++)
		{
			for(i=0;i<internalBPN->summedErrorDerivatives[l].getHeight();i++)
				for(int j=0;j<internalBPN->summedErrorDerivatives[l].getWidth();j++)
					internalBPN->summedErrorDerivatives[l].setValue(j, i, 0.0f);

			for(i=0;i<internalBPN->summedErrorDerivatives[l].getHeight();i++)
				for(int j=0;j<internalBPN->summedErrorDerivatives[l].getWidth();j++)
					internalBPN->biasSummedErrorDerivatives[l].setValue(j, i, 0.0f);

//			internalBPN->summedErrorDerivatives[i].setAllValues(0.0f);
//			internalBPN->biasSummedErrorDerivatives[i].setAllValues(0.0f);
		}

		for(l=0;l<input.size();l++)
		{
			if(useSymmetry)
			{
				for(int symmetry=0;symmetry<8;symmetry++)
				{
					// first symmetry is NONE
					if(symmetry!=0)
					{
						// make sure input is converted to 5x5 for example
						// instead of 1x25 so we can rotate etc...
						input[l].convert(converted, root, root);
						// adjust input[l] before running trainPattern
						converted.doTransform(transformed, symmetry);
						transformed.convert(newInput, width, height);

						output[l].convert(convertedOutput, rootOut, rootOut);
						convertedOutput.doTransform(transformedOutput, symmetry);
						transformedOutput.convert(newOutput, widthOut, heightOut);
					}
					// first symmetry is NONE
					if(symmetry!=0)
						if(!trainPattern(newInput, newOutput, freezeLayers, previousMSE, doMomentum, batchUpdate))
						{
							internalBPN->errorCode = BPN::BPN_ERROR;
							return epochsRun;
						}
					else 
						if(!trainPattern(input[l], output[l], freezeLayers, previousMSE, doMomentum, batchUpdate))
						{
							internalBPN->errorCode = BPN::BPN_ERROR;
							return epochsRun;
						}
					if(convergenceCheck)
					{
						// get output layer
						if(symmetry!=0)
						{
							if(!internalBPN->getAnswer(newInput))
							{
								internalBPN->errorCode = BPN::BPN_ERROR;
								return epochsRun;
							}
						}
						else
						{
							if(!internalBPN->getAnswer(input[l]))
							{
								internalBPN->errorCode = BPN::BPN_ERROR;
								return epochsRun;
							}
						}
					}
				} // end for symmetry
			}
			else 
			{
				if(!trainPattern(input[l], output[l], freezeLayers, previousMSE, doMomentum, batchUpdate))
				{
					internalBPN->errorCode = BPN::BPN_ERROR;
					return epochsRun;
				}
				// get output layer
				if(convergenceCheck && !internalBPN->getAnswer(input[l]))
				{
					internalBPN->errorCode = BPN::BPN_ERROR;
					return epochsRun;
				}
			}
// all the convergence checking can be skipped since
// we know that our NNs will not converge
			if(convergenceCheck && !batchUpdate)
			{
				t = internalBPN->getOutputs()[internalBPN->weights.size()];
				//delete temp;
				h = output[l].getHeight();
				for(int i=0;i<h;i++)
				{
					tval = t.getValue(0, i);
					actval = output[l].getValue(0, i);
					if (converged && actval-tolerance > tval ||
					    actval+tolerance < tval)
						converged = false;
				} // end for i
			}

			// update summed error derivatives
			for(int i=0;i<internalBPN->weightsSize;i++)
			{
				internalBPN->errorDerivatives[i].multiply(-1.0f, negatedED[i]);
				internalBPN->summedErrorDerivatives[i]+=negatedED[i];
				internalBPN->biasErrorDerivatives[i].multiply(-1.0f, negatedBiasED[i]);
				internalBPN->biasSummedErrorDerivatives[i]+=negatedBiasED[i];
			}

			if(l%100==0)
			{
				log.print(".");
			}
		} // end for l
		log.print("\n");
		if(batchUpdate)
		{
			// update weights for this epoch
			for(int i=0;i<internalBPN->weightsSize;i++)
			{
				updateWeights(internalBPN->weights[i], updateValues[i], internalBPN->summedErrorDerivatives[i],
					previousSummedErrorDerivatives[i], lastBatchWeightChanges[i]);
				previousSummedErrorDerivatives[i] = internalBPN->summedErrorDerivatives[i];

				updateWeights(internalBPN->biasWeights[i], biasUpdateValues[i], internalBPN->biasSummedErrorDerivatives[i], 
					biasPreviousSummedErrorDerivatives[i], lastBatchBiasWeightChanges[i]);
				biasPreviousSummedErrorDerivatives[i] = internalBPN->biasSummedErrorDerivatives[i];
			}
			if(convergenceCheck)
			{
				// run convergence check
				for(l=0;l<input.size();l++)
				{
					t = internalBPN->getOutputs()[internalBPN->weights.size()];
					//delete temp;
					h = output[l].getHeight();
					for(i=0;i<h;i++)
					{
						tval = t.getValue(0, i);
						actval = output[l].getValue(0, i);
						if (converged && actval-tolerance > tval ||
							actval+tolerance < tval)
						{
							converged = false;
							break;
						}
					} // end for i
					if(!converged)
						break;
				} // end for l
			}
		}
//#ifdef _DEBUG
		// calculate the summed squared error and output it
		float SSE = 0.0f;
		float val;
		h = output[0].getHeight();
		for(l=0;l<input.size();l++)
		{
			// get the nets answer to input[l]
			if(!internalBPN->getAnswer(input[l]))
			{
				internalBPN->errorCode = BPN::BPN_ERROR;
				return epochsRun;
			}
			// SSE = (sum[(tj - oj)^2])  (summed for all patterns in test/training set)
			//	where tj is the target value for output node j
			//  and oj is the actual output value for output node j
			for(i=0;i<h;i++)
			{
				val = (internalBPN->getOutputs()[internalBPN->weights.size()].getValue(0, i) - output[l].getValue(0, i));
				SSE+=(val*val);
			}
		}
		sprintf(buffer, "SSE = %*g\n", 4, SSE);
		log.printerr(buffer);
//#endif
		epochsRun++;
		internalBPN->epochsCompleted++;
		// save every number of epochs, e.g. 10 epochs
		if(epochsRun%saveEveryNEpochs==0)
			internalBPN->NeuralNetwork::save();
		// print weights after each epoch is run
		internalBPN->printInfo();
		message = "Epoch ";
		sprintf(buffer, "%d", epochsRun);
		message+=buffer;
		message+=" completed.";
		log.println(message, internalBPN->getTypeString());
		// print how much free memory there is now
		/*GlobalMemoryStatus(&memBuffer);
		message = "dwMemoryLoad: ";
		sprintf(buffer, "%d", memBuffer.dwMemoryLoad);
		message+=buffer;
		log.println(message);
		message = "dwAvailVirtual: ";
		sprintf(buffer, "%d", memBuffer.dwAvailVirtual);
		message+=buffer;
		log.println(message);
		message = "dwAvailPhys: ";
		sprintf(buffer, "%d", memBuffer.dwAvailPhys);
		message+=buffer;
		log.println(message);*/
		// check if number of epochs run has been sufficient
		// for the function call
		if(epochsRun==epochs)
			return epochsRun;
		// check if stdin has interrupted training
	} // end while
	return epochsRun;
}
	
/** Train a neural network with the supplied input and output
training pairs using the specified tolerance and until the
specified number of epochs have been run. If epochs==0 then the
training will continue until user interruption.
@param input An array of input matrices.
@param output An array of output matrices that are the correct
reponse to the input matrices.
@param freezeLayers A vector of layer indices to specifically NOT train.
@param epochs The number of epochs to train for.
@param tolerance The allowed amount of error between the actual
output of the neural network and the required output.
@param saveEveryNEpochs Save the neural network every specified number of epochs.
@param useSymmetry If true rotate and reflect the training patterns to use all possible
symmetrical configurations.
@param doMomentum If true use the momentum term to add some of every previous training
generations weight change to the current weight change.
@param convergenceCheck An optional parameter, if true then check for network convergence every epoch, default is true.
@param printProgress An optional parameter to select level of text output, default is true.
@return The number of epochs completed. */
int RPROPBPNTrainer::internalTrain(vector<Matrix<float> >& input, vector<Matrix<float> >& output, 
			   const vector<int>& freezeLayers, int epochs /*=0*/, float tolerance /*=0.1*/, 
			   int saveEveryNEpochs /*=10*/,
			   bool useSymmetry /*=false*/,
			   bool doMomentum /*=true*/, bool convergenceCheck /*=true*/, bool printProgress /*=true*/)
{
	// batch update must be on all the time for RPROP
	const bool batchUpdate = true;

	// input matrix must be a 1 dimensional column vector
	// and the same height as the input layer
	// i.e. a value for each neuron in the input layer
	// and a constant 1.0 at the front for the bias neuron
	// e.g. [ 1.0 9.0 2.0 ]

	// check size of training patterns matches size of network
	LogWriter log;
	char buffer[200];
	string message;
	if(input.size()<=0)
	{
		log.println("Input size = 0");
		log.println("Aborting training");
		return 0;
	}

	int size = input[0].getHeight()*input[0].getWidth();
	if(size!=internalBPN->weights[0].getHeight())
	{
		log.println("Training pattern size does not match network");
		message = "Training pattern size: ";
		sprintf(buffer, "%d", size);
		message+=buffer;
		log.println(message);
		message = "Network input layer size: ";
		sprintf(buffer, "%d", internalBPN->weights[0].getHeight());
		message+=buffer;
		log.println(message);
		log.println("Aborting training");
		return 0;
	}

	Matrix<float> t;
	bool converged = false;
	int h, i, l;
	float tval, actval;
//	vector<Matrix<float> > temp(internalBPN->weights.size()+1);
	int epochsRun = 0;

	internalBPN->errorCode = BPN::NONE;
	// index of the transform to apply
	int height = input[0].getHeight();
	int width = input[0].getWidth();
	int root = sqrt(height);
	Matrix<float> converted(root, root);
	Matrix<float> transformed(root, root);
	Matrix<float> newInput(width, height);
	int widthOut = output[0].getWidth();
	int heightOut = output[0].getHeight();
	int rootOut = sqrt(heightOut);
	Matrix<float> convertedOutput(rootOut, rootOut);
	Matrix<float> transformedOutput(rootOut, rootOut);
	Matrix<float> newOutput(widthOut, heightOut);
	float previousMSE = 0.0f;

	// for *-1 the errorDerivatives before summing
	vector<Matrix<float> > negatedED, negatedBiasED;
	negatedED.resize(internalBPN->weightsSize);
	negatedBiasED.resize(internalBPN->weightsSize);
	for(i=0;i<internalBPN->weightsSize;i++)
	{
		negatedED[i].resize(internalBPN->errorDerivatives[i].getWidth(), internalBPN->errorDerivatives[i].getHeight());
		negatedBiasED[i].resize(internalBPN->biasErrorDerivatives[i].getWidth(), 
			internalBPN->biasErrorDerivatives[i].getHeight());
	}

	// repeatedly do trainPattern()
	// until target output within tolerance
	while (!converged)
	{
		converged = true;
		// reset summed error derivatives before each epoch
		for(l=0;l<internalBPN->weightsSize;l++)
		{
			for(i=0;i<internalBPN->summedErrorDerivatives[l].getHeight();i++)
				for(int j=0;j<internalBPN->summedErrorDerivatives[l].getWidth();j++)
					internalBPN->summedErrorDerivatives[l].setValue(j, i, 0.0f);

			for(i=0;i<internalBPN->biasSummedErrorDerivatives[l].getHeight();i++)
				for(int j=0;j<internalBPN->biasSummedErrorDerivatives[l].getWidth();j++)
					internalBPN->biasSummedErrorDerivatives[l].setValue(j, i, 0.0f);

//			internalBPN->summedErrorDerivatives[i].setAllValues(0.0f);
//			internalBPN->biasSummedErrorDerivatives[i].setAllValues(0.0f);
		}

		// print initial percentage
		if(input.size()>0 && printProgress)
			log.printerr("\nEpoch progress: 00.00%");

		for(l=0;l<input.size();l++)
		{
			if(useSymmetry)
			{
				for(int symmetry=0;symmetry<8;symmetry++)
				{
					// first symmetry is NONE
					if(symmetry!=0)
					{
						// make sure input is converted to 5x5 for example
						// instead of 1x25 so we can rotate etc...
						input[l].convert(converted, root, root);
						// adjust input[l] before running trainPattern
						converted.doTransform(transformed, symmetry);
						transformed.convert(newInput, width, height);

						output[l].convert(convertedOutput, rootOut, rootOut);
						convertedOutput.doTransform(transformedOutput, symmetry);
						transformedOutput.convert(newOutput, widthOut, heightOut);
					}
					// first symmetry is NONE
					if(symmetry!=0)
					{
						if(!trainPattern(newInput, newOutput, freezeLayers, previousMSE, doMomentum, batchUpdate))
						{
							internalBPN->errorCode = BPN::BPN_ERROR;
							return epochsRun;
						}
					}
					else 
					{
						if(!trainPattern(input[l], output[l], freezeLayers, previousMSE, doMomentum, batchUpdate))
						{
							internalBPN->errorCode = BPN::BPN_ERROR;
							return epochsRun;
						}
					}
					if(convergenceCheck && !batchUpdate)
					{
						// get output layer
						if(symmetry!=0)
						{
							if(!internalBPN->getAnswer(newInput))
							{
								internalBPN->errorCode = BPN::BPN_ERROR;
								return epochsRun;
							}
						}
						else
						{
							if(!internalBPN->getAnswer(input[l]))
							{
								internalBPN->errorCode = BPN::BPN_ERROR;
								return epochsRun;
							}
						}
					}
				} // end for symmetry
			}
			else 
			{
				if(!trainPattern(input[l], output[l], freezeLayers, previousMSE, doMomentum, batchUpdate)) 
				{
					internalBPN->errorCode = BPN::BPN_ERROR;
					return epochsRun;
				}
				// get output layer
				if(convergenceCheck && !internalBPN->getAnswer(input[l]))
				{
					internalBPN->errorCode = BPN::BPN_ERROR;
					return epochsRun;
				}
			}
// all the convergence checking can be skipped since
// we know that our NNs will not converge
			if(convergenceCheck && !batchUpdate)
			{
				t = internalBPN->getOutputs()[internalBPN->weights.size()];
				//delete temp;
				h = output[l].getHeight();
				for(i=0;i<h;i++)
				{
					tval = t.getValue(0, i);
					actval = output[l].getValue(0, i);
					if (converged && actval-tolerance > tval ||
					    actval+tolerance < tval)
						converged = false;
				} // end for i
			}
			// update summed error derivatives
			for(i=0;i<internalBPN->weightsSize;i++)
			{
				internalBPN->errorDerivatives[i].multiply(-1.0f, negatedED[i]);
				internalBPN->summedErrorDerivatives[i]+=negatedED[i];
				internalBPN->biasErrorDerivatives[i].multiply(-1.0f, negatedBiasED[i]);
				internalBPN->biasSummedErrorDerivatives[i]+=negatedBiasED[i];
			}
			if(printProgress)
			{
				// output a percentage of how far through the input set we are
				float percentage = ((float)l/(float)input.size())*100;
				// 5 chars width minimum (pad with zeros) and maximum 4 significant digits
				sprintf(buffer, "%05.2f%%", percentage);
				// erase the old one... starts as - "00.00%"
				log.printerr("\b\b\b\b\b\b");
				log.printerr(buffer);
			}
		} // end for l
		if(printProgress)
		{
			log.printerr("\b\b\b\b\b\b");
			log.printerr("100.00%\n");
		}
		if(batchUpdate)
		{
			// update weights for this epoch
			for(i=0;i<internalBPN->weightsSize;i++)
			{

				// normalise the error derivatives over the pattern set
				// by dividing it by the pattern set size
				internalBPN->summedErrorDerivatives[i]*=((float)1/(float)input.size());

				// this is the RPROP bit...
				updateWeights(internalBPN->weights[i], updateValues[i], internalBPN->summedErrorDerivatives[i],
					previousSummedErrorDerivatives[i], lastBatchWeightChanges[i]);
				previousSummedErrorDerivatives[i] = internalBPN->summedErrorDerivatives[i];

				// normalise the error derivatives over the pattern set
				// by dividing it by the pattern set size
				internalBPN->biasSummedErrorDerivatives[i]*=((float)1/(float)input.size());

				updateWeights(internalBPN->biasWeights[i], biasUpdateValues[i], internalBPN->biasSummedErrorDerivatives[i], 
					biasPreviousSummedErrorDerivatives[i], lastBatchBiasWeightChanges[i]);
				biasPreviousSummedErrorDerivatives[i] = internalBPN->biasSummedErrorDerivatives[i];
				// end RPROP bit...
			}
			if(convergenceCheck)
			{
				// run convergence check
				for(l=0;l<input.size();l++)
				{
					// get the nets answer to input[l]
					if(!internalBPN->getAnswer(input[l]))
					{
						internalBPN->errorCode = BPN::BPN_ERROR;
						return epochsRun;
					}
					t = internalBPN->getOutputs()[internalBPN->weights.size()];
					//delete temp;
					h = output[l].getHeight();
					for(i=0;i<h;i++)
					{
						tval = t.getValue(0, i);
						actval = output[l].getValue(0, i);
						if (converged && actval-tolerance > tval ||
							actval+tolerance < tval)
						{
							converged = false;
							break;
						}
					} // end for i
					if(!converged)
						break;
				} // end for l
			}
		}
		// calculate the summed squared error and output it
		float SSE = 0.0f;
		float val;
		h = output[0].getHeight();
		if(printProgress)
			log.printerr("Calculating SSE over training set... 00.00%");
		for(l=0;l<input.size();l++)
		{
			// get the nets answer to input[l]
			if(!internalBPN->getAnswer(input[l]))
			{
				internalBPN->errorCode = BPN::BPN_ERROR;
				return epochsRun;
			}
			// SSE = (sum[(tj - oj)^2])  (summed for all patterns in test/training set)
			//	where tj is the target value for output node j
			//  and oj is the actual output value for output node j
			for(i=0;i<h;i++)
			{
				val = (internalBPN->getOutputs()[internalBPN->weights.size()].getValue(0, i) - output[l].getValue(0, i));
				SSE+=(val*val);
			}
			if(printProgress)
			{
				// output a percentage of how far through the input set we are
				float percentage = ((float)l/(float)input.size())*100;
				// 5 chars width minimum (pad with zeros) and maximum 4 significant digits
				sprintf(buffer, "%05.2f%%", percentage);
				// erase the old one... starts as - "00.00%"
				log.printerr("\b\b\b\b\b\b");
				log.printerr(buffer);
			}
		}
		if(printProgress)
		{
			log.printerr("\b\b\b\b\b\b");
			log.printerr("100.00%\n");
		}
		sprintf(buffer, "SSE = %*g\n", 4, SSE);
		log.printerr(buffer);
		epochsRun++;
		internalBPN->epochsCompleted++;
		// save every number of epochs, e.g. 10 epochs
		if(saveEveryNEpochs!=0 && epochsRun%saveEveryNEpochs==0)
			internalBPN->NeuralNetwork::save();
		// check if number of epochs run has been sufficient
		// for the function call
		if(epochsRun==epochs)
			return epochsRun;
	} // end while
	return epochsRun;
}

void RPROPBPNTrainer::updateWeights(Matrix<float>& thisWeights, Matrix<float>& thisUpdateValues,
									Matrix<float>& seDerivatives, 
									const Matrix<float>& pseDerivatives, Matrix<float>& lbwc)
{			
	float sign;
	float newWeightChange;
	// update all the weight changes according to RPROP
	for(int h=0;h<thisWeights.getHeight();h++)
	{
		for(int w=0;w<thisWeights.getWidth();w++)
		{
			// has the error derivative sign been retained between time steps (hence a +.v.e. multiply)?
			// NOTE: We compare the new error derivative with the previous weight change here
			if((sign=(lbwc.getValue(w, h)*seDerivatives.getValue(w, h)))<0.0f)
			{
				// kept sign so going in the right direction, give it a bit of encouragement
				thisUpdateValues.setValue(w, h, thisUpdateValues.getValue(w, h)*updateIncrease);
				if(thisUpdateValues.getValue(w, h)>updateMax)
					thisUpdateValues.setValue(w, h, updateMax);
				
				// if the derivative is +v.e. the weight change should be -v.e
				if(seDerivatives.getValue(w, h)>0)
					newWeightChange = -thisUpdateValues.getValue(w, h);
				// if the derivative is -v.e. the weight change shoudl be +v.e.
				else
					newWeightChange = thisUpdateValues.getValue(w, h);
			}
			// has the error derivative sign changed between time steps (hence a -.v.e. multiply)?
			else if(sign>0.0f)
			{
				// sign changed
				thisUpdateValues.setValue(w, h, thisUpdateValues.getValue(w, h)*updateDecrease);
				if(thisUpdateValues.getValue(w, h)<updateMin)
					thisUpdateValues.setValue(w, h, updateMin);
				
				// back track the previous weight change since we've gone too far
				// indicated by the error derivative sign change
//				thisWeights.setValue(w, h, thisWeights.getValue(w, h)-lbwc.getValue(w, h));
//				lbwc.setValue(w, h, -lbwc.getValue(w, h));
				// set the weight change value to 0 so we don't punish the sign change again next epoch
				newWeightChange = 0.0f;
			}
			// otherwise one of the time steps error derivatives must be 0
			else
			{
				// if the derivative is +v.e. the weight change should be -v.e
				if(seDerivatives.getValue(w, h)>0)
					newWeightChange = -thisUpdateValues.getValue(w, h);
				// if the derivative is -v.e. the weight change shoudl be +v.e.
				else
					newWeightChange = thisUpdateValues.getValue(w, h);
			}
			lbwc.setValue(w, h, newWeightChange);
			thisWeights.setValue(w, h, thisWeights.getValue(w, h)+newWeightChange);
		}
	}
}

/** Train the neural network against a single input, output
training pair through one epoch.
@param input The input matrix.
@param output The required output matrix. 
@param freezeLayers A vector of layer indices to specifically NOT train.
@param previousMSE The mean squared error of the network from the last pattern trained.
@param doMomentum If true use the momentum term to add some of every previous training
generations weight change to the current weight change.
@param batchUpdate If true update the network weights every iteration of the training set,
if false update after every weight change calculation. */
bool RPROPBPNTrainer::trainPattern(const Matrix<float>& input, const Matrix<float>& output, 
			const vector<int>& freezeLayers, float& previousMSE, bool doMomentum /*=true*/, bool batchUpdate /*=false*/)
{
	// if we have specified a separate unsupervised phase do it now
	if(internalBPN->getPatternsCompleted()<phase1)
	{
		// find hidden node that has weights that are most similar to the input pattern
		// adjust weights to be more similar to input pattern
//#error do this
	}
	else
	{
		// only adjust weights after the singlyConnectedHiddenLayer
		// all weight layers before that should be frozen (i.e. they're
		// from the source net, so hold important pattern information)
		
		if(!internalBPN->getAnswer(input)) return false;
		
		/** calculate errors **/
		
		// output layer errors = (Target-Output).f'(Output)
		Matrix<float> outputDiff(output.width, output.height);
		output.subtract(internalBPN->outputs[internalBPN->weightsSize], outputDiff);
		Matrix<float> temp2(output.width, output.height);
		internalBPN->DActivationFN(internalBPN->outputs[internalBPN->weightsSize], temp2, internalBPN->weightsSize-1);
		Matrix<float>& outputErrors = internalBPN->errors[internalBPN->weightsSize-1];
		outputErrors.resize(1, outputDiff.height);
		for(int y=0;y<outputDiff.height;y++)
			outputErrors.setValue(0, y, outputDiff.getValue(0, y)*temp2.getValue(0, y));

		Matrix<float> temp(output.width, output.height);
		
		// hidden layer errors = f'(layer output)@((layer weights)x(next layer errors))
		// where a@b = multiply each element i in a to each element in b row i
		
		bool checkFreezeLayers = false;
		if(freezeLayers.size()>0)
			checkFreezeLayers = true;
		
		// only calculate errors for layers after the singlyConnectedHiddenLayer
		for(int l=(internalBPN->weightsSize-1);l>0;l--)
		{
			// use references for easier reading
			Matrix<float>& thisErrors = internalBPN->errors[l-1];
			Matrix<float>& nextErrors = internalBPN->errors[l];
			Matrix<float>& thisOutputs = internalBPN->outputs[l];
			Matrix<float>& thisWeights = internalBPN->weights[l];
			
			// temp = f'(output)
			temp.resize(thisOutputs.width, thisOutputs.height);
			internalBPN->DActivationFN(thisOutputs, temp, l);
			// weights*errors
			temp2.resize(nextErrors.width, thisWeights.height);
			thisWeights.multiply(nextErrors, temp2);
			// errors = temp@temp2
			thisErrors.resize(temp2.width, temp2.height);
			for(int y=0;y<temp2.height;y++)
				thisErrors.setValue(0, y, temp.getValue(0, y)*temp2.getValue(0, y));
		}
		
		// calculate weight changes
		
		// weight change = alpha(Output*Tranpose(error))+momentum(oldWeightChange)
		// start doing weight changes after the singlyConnectedHiddenLayer
		for(l=0;l<internalBPN->weightsSize;l++)
		{
			// set references
			Matrix<float>& thisBiasErrorDerivatives = internalBPN->biasErrorDerivatives[l];
			Matrix<float>& thisErrorDerivatives = internalBPN->errorDerivatives[l];
			Matrix<float>& thisErrors = internalBPN->errors[l];
			Matrix<float>& thisOutputs = internalBPN->outputs[l];
			Matrix<float>& thisLastWeightChanges = internalBPN->lastWeightChanges[l];
			Matrix<float>& thisLastBiasWeightChanges = internalBPN->lastBiasWeightChanges[l];
			
			//			Matrix<float> thisErrorsTranspose;
			// the bias error derivative is errors
			// because bias outputs = 1 (therefore errors*1)
			thisErrors.transpose(thisBiasErrorDerivatives);
			
			// error derivative = output*Transpose(error)
			thisOutputs.multiply(thisBiasErrorDerivatives, thisErrorDerivatives);
			
			// check if we're freezing this layers weights or not
			if(checkFreezeLayers)
			{
				bool skipLayer = false;
				for(int i=0;i<freezeLayers.size();i++)
				{
					if(freezeLayers[i]==l)
						skipLayer = true;
				}
				if(skipLayer)
					continue;
			}
			
			// Only calculate these things if not doing batchUpdate
			// For batchUpdate an averaged derivative over the whole epoch
			// will be calculated in the parent function
			if(!batchUpdate)
			{
				// alpha(Output*Tranpose(error))
				temp.resize(thisBiasErrorDerivatives.width, thisOutputs.height);
				// weight change = error derivative * learning rate
				thisErrorDerivatives.multiply(internalBPN->learningRate, temp);
				// bias weights use temp2
				// NOTE: outputs always 1 for biases
				temp2.resize(thisBiasErrorDerivatives.width, thisBiasErrorDerivatives.height);
				// bias weight change = bias error derivative * learning rate
				thisBiasErrorDerivatives.multiply(internalBPN->learningRate, temp2);
				// momentum term
				if(doMomentum)
				{
					thisLastWeightChanges*=internalBPN->momentum;
					temp+=thisLastWeightChanges;
					// bias weights
					thisLastBiasWeightChanges*=internalBPN->momentum;
					temp2+=thisLastBiasWeightChanges;
				}
				thisLastWeightChanges = temp;
				thisLastBiasWeightChanges = temp2;
				internalBPN->weights[l]+=temp;
				internalBPN->biasWeights[l]+=temp2;
			}
		}

		// update learning rate if required
		if(internalBPN->dynamicLearningRate || internalBPN->dynamicMomentum)
		{
			// update the previous mean squared error
			// MSE = (sum[(tj - oj)^2])/2
			//	where tj is the target value for output node j
			//  and oj is the actual output value for output node j
			float totalError = 0.0f;
			float val;
			for(int i=0;i<outputDiff.height;i++)
			{
				val = outputDiff.getValue(0, i);
				totalError+=val*val;
			}
			float newMSE = totalError/2;
			// only update learning rate and momentum if we have a valid value
			// for the previousMSE
			if(previousMSE!=0)
			{
				float r;
				if(newMSE!=0.0f)
					r = (newMSE - previousMSE)/newMSE;
				else
					r = 0.0f;

				if(internalBPN->dynamicLearningRate)
				{
					// **DYNAMIC_UPDATES** //
					// r is the relative change in error
					// r = [E(n) - E(n-1)] / E(n)  (where E is the mean squared error at time n)
					// if r<0
					//	learningRate = learningRate*[1+u*e^(-r)]
					// if r>=0
					//	learningRate = learningRate*[1-u*e^(-r)]
					// where u is between (0, 1) but not including
					float newLearningRate = 0.0f;
//					float u = 0.01f;
					// if the MSE has increased from last iteration r will be < 0
					// therefore the learning rate will be decreased...
					// if the error has increased over previous error, decrease learning rate
//					if(r<0)
//						newLearningRate = internalBPN->learningRate * (1+u*exp(-r));
					// otherwise the MSE has decreased so r will be between 0 and 1
					// exp(-r) will be between ~0.36 and 1
					// so the learning rate will be increased (exp(-r) = positive factor)
//					else
//						newLearningRate = internalBPN->learningRate * increaseFactor;
//						newLearningRate = internalBPN->learningRate * (1-u*exp(-r));

					// check for crazy learning rate increases
//					if((newLearningRate/internalBPN->learningRate)>maxGrowthFactor)
//						internalBPN->learningRate*=maxGrowthFactor;
//					else
//						internalBPN->learningRate = newLearningRate;

/*					float decreaseFactor = 0.7f;
					float increaseFactor = 1.05f;
					if(newMSE<previousMSE)
						newLearningRate = internalBPN->learningRate * decreaseFactor;
					else
						newLearningRate = internalBPN->learningRate * increaseFactor;
					internalBPN->learningRate = newLearningRate;*/

					// ALAP method
					// l(n) = l(n-1) + p * < dE(n-1), dE(n) >
					// where l is the learning rate, p is a constant typically 1
					// dE(n) is the error matrix for all weights, at time step n
					// <,> means the inner product

					cout << "New learning rate: " << internalBPN->learningRate << endl;
				}
				// update momentum if required
				if(internalBPN->dynamicMomentum)
				{
					// **DYNAMIC_UPDATES** //
					// r is the relative change in error
					// r = [E(n) - E(n-1)] / E(n)  (where E is the error at time n)
					// if r<0
					//	momentum = momentum*[1+v*e^(-r)]
					// if r>=0
					//	momentum = momentum*[1-v*e^(-r)]
					// where v is between (0, 1) but not including
					float v = 0.5f;
					if(r<0)
						internalBPN->momentum = internalBPN->momentum * (1+v*exp(-r));
					else
						internalBPN->momentum = internalBPN->momentum * (1-v*exp(-r));
				}
			} // end if (previousMSE!=0)
			previousMSE = newMSE;
		}


	} // end else (not unsupervised phase)
	
	internalBPN->patternsCompleted++;

	return true;
} // end trainPattern(Matrix input, Matrix output)
