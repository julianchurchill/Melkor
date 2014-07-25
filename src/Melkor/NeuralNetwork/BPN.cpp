#pragma warning( disable : 4786 )

#include "globals.h"
#include "BPN.h"
#include "Matrix.h"
#include "NNDatabase.h"
#include "LogWriter.h"
#include "Misc.h"
#include <strstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
//using namespace std;

const int BPN::BPNTYPE = 0;
const int BPN::NEWBPNTYPE = 1;
const int BPN::TDTYPE = 2;
const int BPN::NEWBPNTYPE2 = 3;
const int BPN::NEWBPNTYPE3 = 4;
const int BPN::EVALBPNTYPE = 5;
const int BPN::SAFETYTYPE = 6;
const int BPN::SAFETY2TYPE = 7;
const int BPN::NEWBPNTYPE4 = 8;
const int BPN::NEWBPNTYPE5 = 9;
const int BPN::NEWBPNTYPE6 = 10;
const int BPN::NEWBPNTYPE7 = 11;
const int BPN::EVALBPN2TYPE = 12;
const int BPN::EVALBPN3TYPE = 13;
const int BPN::NEWBPNTYPE8 = 14;
const int BPN::NEWBPNTYPE9 = 15;
const int BPN::NEWBPNTYPE10 = 16;
const int BPN::NEWBPNTYPE11 = 17;
const int BPN::NEWBPNTYPE3GOPROBLEMS = 18;
const int BPN::URGENCY1TYPE = 19;
const int BPN::NEWBPNTYPE12 = 20;
const int BPN::URGENCY3TYPE = 21;
const int BPN::CNTYPE = 22;
const int BPN::BPNEXPACTTYPE = 23;
const int BPN::FIRSTBPNTYPE = 0;
const int BPN::LASTBPNTYPE = 23;

// returns -1 on failure to match the type string
int BPN::stringToType(const char* t)
{
	if(strcmp(t, "BPNTYPE")==0)
		return BPNTYPE;
	else if(strcmp(t, "NEWBPNTYPE")==0)
		return NEWBPNTYPE;
	else if(strcmp(t, "NEWBPNTYPE2")==0)
		return NEWBPNTYPE2;
	else if(strcmp(t, "NEWBPNTYPE3")==0)
		return NEWBPNTYPE3;
	else if(strcmp(t, "NEWBPNTYPE4")==0)
		return NEWBPNTYPE4;
	else if(strcmp(t, "NEWBPNTYPE5")==0)
		return NEWBPNTYPE5;
	else if(strcmp(t, "NEWBPNTYPE6")==0)
		return NEWBPNTYPE6;
	else if(strcmp(t, "NEWBPNTYPE7")==0)
		return NEWBPNTYPE7;
	else if(strcmp(t, "NEWBPNTYPE8")==0)
		return NEWBPNTYPE8;
	else if(strcmp(t, "NEWBPNTYPE9")==0)
		return NEWBPNTYPE9;
	else if(strcmp(t, "NEWBPNTYPE10")==0)
		return NEWBPNTYPE10;
	else if(strcmp(t, "NEWBPNTYPE11")==0)
		return NEWBPNTYPE11;
	else if(strcmp(t, "NEWBPNTYPE12")==0)
		return NEWBPNTYPE12;
	else if(strcmp(t, "NEWBPNTYPE3GOPROBLEMS")==0)
		return NEWBPNTYPE3GOPROBLEMS;
	else if(strcmp(t, "TDTYPE")==0)
		return TDTYPE;
	else if(strcmp(t, "EVALBPNTYPE")==0)
		return EVALBPNTYPE;
	else if(strcmp(t, "EVALBPN2TYPE")==0)
		return EVALBPN2TYPE;
	else if(strcmp(t, "EVALBPN3TYPE")==0)
		return EVALBPN3TYPE;
	else if(strcmp(t, "SAFETYTYPE")==0)
		return SAFETYTYPE;
	else if(strcmp(t, "SAFETY2TYPE")==0)
		return SAFETY2TYPE;
	else if(strcmp(t, "URGENCY1TYPE")==0)
		return URGENCY1TYPE;
	else if(strcmp(t, "URGENCY3TYPE")==0)
		return URGENCY3TYPE;
	else if(strcmp(t, "CNTYPE")==0)
		return CNTYPE;
	else if(strcmp(t, "BPNEXPACTTYPE")==0)
		return BPNEXPACTTYPE;
	else
		return -1;
}

const char* BPN::typeToString(int t)
{
	switch(t)
	{
		case BPNTYPE:
			return "BPNTYPE";
		case NEWBPNTYPE:
			return "NEWBPNTYPE";
		case NEWBPNTYPE2:
			return "NEWBPNTYPE2";
		case NEWBPNTYPE3:
			return "NEWBPNTYPE3";
		case NEWBPNTYPE4:
			return "NEWBPNTYPE4";
		case NEWBPNTYPE5:
			return "NEWBPNTYPE5";
		case NEWBPNTYPE6:
			return "NEWBPNTYPE6";
		case NEWBPNTYPE7:
			return "NEWBPNTYPE7";
		case NEWBPNTYPE8:
			return "NEWBPNTYPE8";
		case NEWBPNTYPE9:
			return "NEWBPNTYPE9";
		case NEWBPNTYPE10:
			return "NEWBPNTYPE10";
		case NEWBPNTYPE11:
			return "NEWBPNTYPE11";
		case NEWBPNTYPE12:
			return "NEWBPNTYPE12";
		case NEWBPNTYPE3GOPROBLEMS:
			return "NEWBPNTYPE3GOPROBLEMS";
		case TDTYPE:
			return "TDTYPE";
		case EVALBPNTYPE:
			return "EVALBPNTYPE";
		case EVALBPN2TYPE:
			return "EVALBPN2TYPE";
		case EVALBPN3TYPE:
			return "EVALBPN3TYPE";
		case SAFETYTYPE:
			return "SAFETYTYPE";
		case SAFETY2TYPE:
			return "SAFETY2TYPE";
		case URGENCY1TYPE:
			return "URGENCY1TYPE";
		case URGENCY3TYPE:
			return "URGENCY3TYPE";
		case CNTYPE:
			return "CNTYPE";
		case BPNEXPACTTYPE:
			return "BPNEXPACTTYPE";
		default:
			return "UNKNOWN TYPE";
	}
}

bool BPN::hasSourceType(int type)
{
	if(type==BPN::CNTYPE || type==BPN::URGENCY3TYPE || type==BPN::BPNEXPACTTYPE)
		return true;
	return false;
}

bool BPN::hasOffboardNodes(int type)
{
	if(type==NEWBPNTYPE3 || type==NEWBPNTYPE4 || type==NEWBPNTYPE5 || type==NEWBPNTYPE6 ||
	   type==NEWBPNTYPE10 || type==NEWBPNTYPE11)
		return true;
	return false;
}

bool BPN::has18DistanceNodes(int type)
{
	if(type==NEWBPNTYPE2 || type==NEWBPNTYPE3 || type==NEWBPNTYPE4 || type==NEWBPNTYPE5 ||
	   type==NEWBPNTYPE6 || type==NEWBPNTYPE12)
		return true;
	return false;
}

bool BPN::has2DistanceNodes(int type)
{
	if(type==NEWBPNTYPE || type==NEWBPNTYPE11)
		return true;
	return false;
}

int BPN::getNeuronsPerPoint(int type)
{
	switch(type)
	{
		case BPNTYPE:
			return 1;
		case NEWBPNTYPE:
			return 3;
		case NEWBPNTYPE2:
			return 3;
		case NEWBPNTYPE3:
			return 4;
		case NEWBPNTYPE4:
			return 4;
		case NEWBPNTYPE5:
			return 4;
		case NEWBPNTYPE6:
			return 4;
		case NEWBPNTYPE7:
			return 2;
		case NEWBPNTYPE8:
			return 2;
		case NEWBPNTYPE9:
			return 3;
		case NEWBPNTYPE10:
			return 4;
		case NEWBPNTYPE11:
			return 4;
		case NEWBPNTYPE12:
			return 3;
		case NEWBPNTYPE3GOPROBLEMS:
			return getNeuronsPerPoint(BPN::NEWBPNTYPE3);
		case TDTYPE:
			return 3;
		case EVALBPNTYPE:
			return 3;
		case EVALBPN2TYPE:
			return 3;
		case EVALBPN3TYPE:
			return 3;
		case SAFETYTYPE:
			return 4;
		case SAFETY2TYPE:
			return 4;
		case URGENCY1TYPE:
			return 6;
		// these three below depend on their sourceType
		case URGENCY3TYPE:
			return 0;
		case CNTYPE:
			return 0;
		case BPNEXPACTTYPE:
			return 0;
		default:
			return 0;
	}
}

int BPN::getType(string filename)
{
	ifstream in(filename.c_str(), ios::binary);
	if(!in)
		return -1;
	int tempid;
	// read saveVersion and discard
	in.read(reinterpret_cast<char*>(&tempid), sizeof(int));
	// now read NN type
	in.read(reinterpret_cast<char*>(&tempid), sizeof(int));
	in.close();
	return tempid;
}

const int BPN::SIGMOID = 0;
const int BPN::BIPOLARSIGMOID = 1;
const int BPN::EXPONENTIAL_ACTIVATION = 2;

const int BPN::NONE = 0;
const int BPN::USER_INTERRUPT = 1;
const int BPN::BPN_ERROR = 2;

BPN::BPN(float alpha, float m, int type, INPUTFIELDSHAPE ifs) : NeuralNetwork(), id(type), learningRate(alpha), momentum(m),
	epochsCompleted(0), errorCode(NONE), lastPatternTest(0), patternsCompleted(0),
	saveVersion(2), dynamicLearningRate(false), dynamicMomentum(false), inputFieldShape(ifs)
{
	activationFunction.push_back(SIGMOID);
}

/** Create a BPN object from filename. 
@param filename The name of the file to load this object from. */
BPN::BPN(string filename) : NeuralNetwork(), id(BPNTYPE), learningRate(0.25f), momentum(0.7f), epochsCompleted(0), errorCode(NONE),
   lastPatternTest(0), patternsCompleted(0), saveVersion(2), dynamicLearningRate(false), dynamicMomentum(false),
   inputFieldShape(IFS_SQUARE)
{
	activationFunction.push_back(SIGMOID);
	if(!load(filename))
	{
		setErrorCode(BPN_ERROR);
		throw "BPN constructor failed to load file!";
	}
}
	
/** Create a new BPN object with the specified number of input, hidden and output neurons.
@param input The number of neurons in the input layer. 
@param hidden A vector indicating the number of neurons in each hidden layer.
@param output The number of neurons in the output layer.
@param alpha The learning rate.
@param m The momentum rate.
@param type The neural network type.
@param ifs Optional parameter to define the input field shape, default is IFS_SQUARE. */
BPN::BPN(int input, vector<int>& hidden, int output, float alpha, float m, int type, INPUTFIELDSHAPE ifs /*=IFS_SQUARE*/)
 : NeuralNetwork(), id(type), learningRate(alpha), momentum(m), epochsCompleted(0), errorCode(NONE),
   lastPatternTest(0), patternsCompleted(0), saveVersion(2), dynamicLearningRate(false), dynamicMomentum(false),
   inputFieldShape(ifs)
{
	if(type<FIRSTBPNTYPE || type>LASTBPNTYPE)
	{
		setErrorCode(BPN_ERROR);
		throw "Unknown BPN type specified for BPN constructor!";
	}
	
	// height of matrix is number of neurons in layer
	// width is number of neurons in next layer
	// +1 to each height for the bias neuron weight

	int h = hidden.size();
	for(int i=0;i<h;i++)
	{
		if(i==0) 
			weights.push_back(Matrix<float>(hidden[i], input));
		else 
			weights.push_back(Matrix<float>(hidden[i], hidden[i-1]));
		biasWeights.push_back(Matrix<float>(hidden[i], 1));
		activationFunction.push_back(SIGMOID);
	}

	// weights to output layer
	if(h==0) 
		weights.push_back(Matrix<float>(output, input));
	else 
		weights.push_back(Matrix<float>(output, hidden[i-1]));
	biasWeights.push_back(Matrix<float>(output, 1));
	activationFunction.push_back(SIGMOID);

	// resize to number of layers
	weightsSize = weights.size();
//	net.resize(weightsSize);
	initWeights();
}
	
/** Initialise all weights in the network with random values
and reset all weight change values. */
void BPN::initWeights()
{
	// -0.3 <= x < 0.3
	for(int i=0;i<weightsSize;i++)
	{
		randomizeLayerWeights(weights[i], WEIGHTMIN, WEIGHTMAX);
		randomizeLayerWeights(biasWeights[i], WEIGHTMIN, WEIGHTMAX);
	}
	
	resetWeightChanges();
}
	
/** Set all the weight change values, which remember the last
change in each weight, back to 0. */
void BPN::resetWeightChanges()
{
	errors.resize(weightsSize);
	outputs.resize(weightsSize+1);
	outputsTransposed.resize(weightsSize+1);

	// set all weightChanges values to 0

	getAnswerTemp.clear();
	errorDerivatives.clear();
	biasErrorDerivatives.clear();
	lastWeightChanges.clear();
	lastBiasWeightChanges.clear();
	batchWeightChanges.clear();
	batchBiasWeightChanges.clear();
	summedErrorDerivatives.clear();
	biasSummedErrorDerivatives.clear();
	
	for(int l=0;l<weightsSize;l++)
	{
		// need to do this here since things below rely on it
		outputs[l].resize(1, weights[l].height);
		outputsTransposed[l].resize(weights[l].height, 1);
	}
	outputs[weightsSize].resize(1, weights[weightsSize-1].width);
	outputsTransposed[weightsSize].resize(weights[weightsSize-1].width, 1);

	for(l=0;l<weightsSize;l++)
	{
		// setup getAnswer temporary - these will always be the same size
		// as the transpose of the outputs[l+1]
		getAnswerTemp.push_back(Matrix<float>(outputs[l+1].height, outputs[l+1].width));

		errorDerivatives.push_back(Matrix<float>(weights[l].width, weights[l].height));
		biasErrorDerivatives.push_back(Matrix<float>(weights[l].width, 1));
		lastWeightChanges.push_back(Matrix<float>(weights[l].width, weights[l].height));
		lastBiasWeightChanges.push_back(Matrix<float>(weights[l].width, 1));
		batchWeightChanges.push_back(Matrix<float>(weights[l].width, weights[l].height));
		batchBiasWeightChanges.push_back(Matrix<float>(weights[l].width, 1));
		summedErrorDerivatives.push_back(Matrix<float>(weights[l].width, weights[l].height));
		biasSummedErrorDerivatives.push_back(Matrix<float>(weights[l].width, 1));
	}

	for(l=0;l<weightsSize;l++)
		errors[l].resize(outputs[l+1].width, outputs[l+1].height);


	int h, w;
	for(l=0;l<lastWeightChanges.size();l++)
	{
		h = lastWeightChanges[l].getHeight();
		w = lastWeightChanges[l].getWidth();
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				errorDerivatives[l].setValue(j, i, 0);
				lastWeightChanges[l].setValue(j, i, 0);
				batchWeightChanges[l].setValue(j, i, 0);
				summedErrorDerivatives[l].setValue(j, i, 0);
			}
		} // end for i
		for(i=0;i<lastBiasWeightChanges[l].getWidth();i++)
		{
			biasErrorDerivatives[l].setValue(i, 0, 0);
			lastBiasWeightChanges[l].setValue(i, 0, 0);
			batchBiasWeightChanges[l].setValue(i, 0, 0);
			biasSummedErrorDerivatives[l].setValue(i, 0, 0);
		}
	} // end for l
}

/** Set all weights in a layer to a random value between specified
bounds.
@param layer The layer of weights to set.
@param min The lower bound of allowed random numbers. 
@param max The upper bound of allowed random numbers. */
void BPN::randomizeLayerWeights(Matrix<float>& layer, float min, float max)
{
	int h = layer.getHeight();
	int w = layer.getWidth();
	float range = max-min;
	
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			// generate random value
			layer.setValue(j, i, getRandom(min, max));
		}
	}
}

/** Apply a matrix of input values to the neural network and
return the output values from all neurons.
@param input The matrix of input values.
@return Return true if successful or false if an error occured. */
bool BPN::unoptimisedGetAnswer(const Matrix<float>& input)
{
#ifdef _DEBUG
	// check input dimensions
	if(input.height!=weights[0].height) return false;
	if(input.width!=1) return false;
#endif

	outputs[0] = input;

	Matrix<float> temp;
	for(int l=0;l<weightsSize;l++)
	{
		// use references for speed
		const Matrix<float>& thisWeights = weights[l];
		const Matrix<float>& thisOutputs = outputs[l];

		temp.resize(thisWeights.width, thisOutputs.width);
		// previous layer output transposed * next layer weights
		thisOutputs.transpose().multiply(thisWeights, temp);
		// net + bias weights
		temp.sum(biasWeights[l], temp);
		// temp = f(net)
		activationFN(temp, temp, l);
		outputs[l+1] = temp.transpose();
	}
	return true;
}

/** A high speed version of BPN::getAnswer().
@param input The matrix of input values.
@return Return true if successful or false if an error occured.
@see getAnswer */
bool BPN::highPerformanceGetAnswer(const Matrix<float>& input)
{
	// Note: This function assumes:
	// 1. All matrices are the correct size for the calculations
	// 2. weightsSize > 0 (therefore we have 1 or move layers of weights)
	// 3. getAnswerTemp[0..weightsSize].size > 0

	register int l;
	register int i;
	register int size;
	register float *fPtr, *bPtr;

	// size of temporary matrices will always be the same for particular network...
	// so no resizing required here
	
	Matrix<float> outputsT;
	outputs[0] = input;

	l = 0;
	do {
		register Matrix<float>& templ = getAnswerTemp[l];
		size = templ.size;

		// previous layer output transposed * next layer weights
		outputs[l].transposeWithPointers(outputsT);

		// NOTE: This matrix multiplyWithPointers is 4x faster than the standard multiply
		outputsT.multiply(weights[l], templ);

		// net + bias weights
//		templ.sum(biasWeights[l], templ);
		// try pointer access for summing
		fPtr = templ.values;
		bPtr = biasWeights[l].values;
		
		// Loop flipping optimisation - reduces asm instructions compared to for loop
		i = 0;
		do {
			(*fPtr++)+=(*bPtr++);
		} while(++i<size);

		// temp = f(net)
//		activationFN(templ, templ, l);
		fPtr = templ.values;
		if(activationFunction[l]==SIGMOID){
			i = 0;
			do {
				(*fPtr++) = 1/(1+exp(-(*fPtr)));
			} while(++i<size);
		}
		else if(activationFunction[l]==BIPOLARSIGMOID){
			i = 0;
			do {
				(*fPtr++) = (2/(1+exp(-(*fPtr))))-1;
			} while(++i<size);
		}
		else if(activationFunction[l]==EXPONENTIAL_ACTIVATION){
			i = 0;
			do {
				(*fPtr++) = exp(-((*fPtr)*(*fPtr)));
			} while(++i<size);
		}

		templ.transposeWithPointers(outputs[l+1]);
	} while(++l<weightsSize);

	return true;
}
	
/** A high speed network architecture specific version of BPN::getAnswer()
  * for a newBPN3 type network of the form 342-40-1
@param input The matrix of input values.
@return Return true if successful or false if an error occured.
@see getAnswer */
bool BPN::highPerformanceGetAnswer342_40_1_SIGMOID(const Matrix<float>& input)
{
	// Note: This function assumes:
	// 1. All matrices are the correct size for the calculations
	// 2. weightsSize == 2 (therefore we have 2 layers of weights, 3 layers of neurons)
	// 3. getAnswerTemp[0].size == 40 and getAnswerTemp[1].size == 1
	// 4. That all activation functions throughout the network are SIGMOID
	// 5. outputs[0].height == 342 and outputs[0].width == 1
	// 6. outputs[1].height == 40 and outputs[1].width == 1
	// 7. weights[0].width == 40 and weights[0].height == 342
	// 8. weights[1].width == 1 and weights[0].height == 40

	// size of temporary matrices will always be the same for particular network...
	// so no resizing required here

	register Matrix<float>& temp0 = getAnswerTemp[0];
	register Matrix<float>& temp1 = getAnswerTemp[1];
	
	outputs[0] = input;

	/***************/
	/* First layer */
	/***************/

	register int i = 0;

	// previous layer output transposed * next layer weights
	//outputs[0].transposeWithPointers(outputsT0);

	// Because we're using 1-dim arrays for matrices, and outputs[0]
	// and outputsT0 are 1 x n or n x 1 we don't have to do the tranposing step!

	// Multiply the weights by the transposed output and put in temp0
	//outputsT0.multiply(weights[0], temp0);

	register float* fPtr = outputs[0].values;
	register float* bPtr = weights[0].values;
	register float* rPtr = temp0.values;
	register float* bwPtr = biasWeights[0].values;
	register float* o1Ptr = outputs[1].values;

	register int c = 0;
	register int j = 0;
	register int cj40 = 0;

	do {
		// Do the multiplication here
		(*rPtr) = 0.0f;
		c = 0;
		cj40 = j;
		fPtr = outputs[0].values;
		do {
			// This loop has been unrolled to 18 statements for efficiency
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			(*rPtr) += (*(fPtr++)) * (*(bPtr+cj40));
			cj40+=40;
			c+=18;
		} while(c<342);
		// Add on the bias weights here
		(*rPtr) += (*bwPtr++);
		// Do the activation function and copy to outputs[1] at the same time
		(*rPtr++) = (*o1Ptr++) = 1/(1+exp(-(*rPtr)));
	} while(++j<40);

	/****************/
	/* Second layer */
	/****************/

	// previous layer output transposed * next layer weights
	//outputs[1].transposeWithPointers(outputsT1);

	// Because we're using 1-dim arrays for matrices, and outputs[1]
	// and outputsT1 are 1 x n or n x 1 we don't have to do this tranposing step!

	// Multiply the weights by the transposed output and put in temp1
	//outputsT1.multiply(weights[1], temp1);

	fPtr = outputs[1].values;
	bPtr = weights[1].values;
	rPtr = temp1.values;

	(*rPtr) = 0.0f;
	c = 0;
	do {
		// This loop has been unrolled to 20 statements
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		(*rPtr) += (*(fPtr++)) * (*(bPtr++));
		c+=20;
	} while(c<40);
	
	// net + bias weights
	// Since size == 1 we only need this statement, not a loop, or increments
	(*rPtr) += (*biasWeights[1].values);

	// Since size == 1 we only need this statement, not a loop, or increments
	(*rPtr) = 1/(1+exp(-(*rPtr)));
	
	//temp1.transposeWithPointers(outputs[2]);
	// Only 1 value cos it's an 1x1 matrix :)
	(*outputs[2].values) = (*rPtr);

	return true;
}
	
float BPN::activationFN(float i, int layer) const
{ 
	if(activationFunction[layer]==SIGMOID) return sigmoidActivationFN(i); 
	else if(activationFunction[layer]==BIPOLARSIGMOID) return bipolarSigmoidActivationFN(i);
	else if(activationFunction[layer]==EXPONENTIAL_ACTIVATION) return exponentialActivationFN(i);
	else
	{
		string message("Unknown activation function type: ");
		message+=activationFunction[layer];
		throw message;
	}
	return 0.0f;
}

float BPN::DActivationFN(float i, int layer) const
{ 
	if(activationFunction[layer]==SIGMOID) return sigmoidDActivationFN(i); 
	else if(activationFunction[layer]==BIPOLARSIGMOID) return bipolarSigmoidDActivationFN(i); 
	else if(activationFunction[layer]==EXPONENTIAL_ACTIVATION) return exponentialDActivationFN(i);
	else
	{
		string message("Unknown derivative activation function type: ");
		message+=activationFunction[layer];
		throw message;
	}
	return 0.0f;
}

bool BPN::activationFN(Matrix<float>& in, Matrix<float>& out, int layer) const
{
#ifdef _DEBUG
	if(in.width!=out.width || in.height!=out.height) return false;
#endif
	for(int y=0;y<in.height;y++)
	{
		for(int x=0;x<in.width;x++)
			out.setValue(x, y, activationFN(in.getValue(x, y), layer));
	}
	return true;
}

bool BPN::DActivationFN(Matrix<float>& in, Matrix<float>& out, int layer) const
{
#ifdef _DEBUG
	if(in.width!=out.width || in.height!=out.height) return false;
#endif
	for(int y=0;y<in.height;y++)
	{
		for(int x=0;x<in.width;x++)
			out.setValue(x, y, DActivationFN(in.getValue(x, y), layer));
	}
	return true;
}

/** Save this BPN network.
Save version differences:
Version 0: Includes saveVersion, type, learningRate, momentum, epochsCompleted and weights.
Version 1: Adds lastPatternTest, patternsCompleted.
Version 2: Adds dynamicLearningRate, dynamicMomentum.
Version 3: Adds inputFieldShape.
@param f The filename to save this object to. 
@see BPN::load() */
void BPN::save(string f)
{
	LogWriter log;
	string message = "Saving ";
	message+=f;
	LogWriter::printerr(message+"\n", typeToString(id));

	ofstream fos(f.c_str(), ios::binary);

	if(!fos)
	{
		message = "BPN: Could not open file for save: ";
		message+=f;
		LogWriter::printerr(message+"\n", typeToString(id));
		return;
	}

	// save version first
//	fos.write(reinterpret_cast<char*>(&saveVersion), sizeof(int));
	int tempInt = saveVersion;
	if(saveVersion==0 || saveVersion==1 || saveVersion==2) tempInt = 3;
	fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));

	// save type
	tempInt = id;
	fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));

	// save learningRate
	float tempFloat = learningRate;
	fos.write(reinterpret_cast<char*>(&tempFloat), sizeof(float));
	// save momentum value
	tempFloat = momentum;
	fos.write(reinterpret_cast<char*>(&tempFloat), sizeof(float));
	// save the number of epochs this net has completed
	tempInt = epochsCompleted;
	fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	// save number of layers

	// save weights as matrices including top row of each for bias weights
	vector<Matrix<float> > tempWeights;
	tempWeights.resize(weights.size());
	for(int i=0;i<tempWeights.size();i++)
	{
		tempWeights[i].resize(weights[i].width, weights[i].height+1);
		for(int x=0;x<tempWeights[i].width;x++)
			tempWeights[i].setValue(x, 0, biasWeights[i].getValue(x, 0));
		for(int y=0;y<weights[i].height;y++)
		{
			for(x=0;x<weights[i].width;x++)
				tempWeights[i].setValue(x, y+1, weights[i].getValue(x, y));
		}
	}

	float t = 0.0;
	int it = tempWeights.size();
	tempInt = it;
	fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	// save layer sizes and layers
	for(i=0;i<it;i++)
		Matrix<float>::save(fos, &tempWeights[i]);

	// Handle old save versions, putting in default values were they weren't previously saved
	if(saveVersion==0)
	{
		double tempDouble = 0;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		tempDouble = 0;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		bool tempBool = false;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
		tempBool = false;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));

		// save input field shape
		tempInt = inputFieldShape;
		fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	}
	else if(saveVersion==1)
	{
		double tempDouble = lastPatternTest;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		tempDouble = patternsCompleted;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		bool tempBool = false;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
		tempBool = false;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));

		// save input field shape
		tempInt = inputFieldShape;
		fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	}
	else if(saveVersion==2)
	{
		double tempDouble = lastPatternTest;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		tempDouble = patternsCompleted;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		bool tempBool = dynamicLearningRate;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
		tempBool = dynamicMomentum;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));

		// save input field shape
		tempInt = inputFieldShape;
		fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	}
	else if(saveVersion==3)
	{
		double tempDouble = lastPatternTest;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		tempDouble = patternsCompleted;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		bool tempBool = dynamicLearningRate;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
		tempBool = dynamicMomentum;
		fos.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));

		// save input field shape
		tempInt = inputFieldShape;
		fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	}

	// update save version to latest version
	if(saveVersion==0 || saveVersion==1 || saveVersion==2) saveVersion = 3;

	fos.close();
}

/** Load a BPN network from the specified file.
Save version differences:
Version 0: Includes saveVersion, type, learningRate, momentum, epochsCompleted and weights.
Version 1: Adds lastPatternTest, patternsCompleted.
Version 2: Adds dynamicLearningRate, dynamicMomentum.
Version 3: Adds inputFieldShape.
@param f The filename to read this BPN network from. 
@param quiet An optional parameter to select level of text output, default is false.
@see BPN::save() */
bool BPN::load(string f, bool quiet /*=false*/)
{
	LogWriter log;
	string message = "Loading ";
	message+=f;
	if(!quiet)
		log.println(message);

	ifstream in(f.c_str(), ios::binary);

	if(!in)
	{
		message = "BPN: Could not load file: ";
		message+=f;
		LogWriter::printerr(message+"\n");
		return false;
	}

	this->filename = f;

	// read save version
	in.read(reinterpret_cast<char*>(&saveVersion), sizeof(int));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}

	// read type
	in.read(reinterpret_cast<char*>(&id), sizeof(int));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}


	// load learning rate
	in.read(reinterpret_cast<char*>(&learningRate), sizeof(float));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}

	// load momentum
	in.read(reinterpret_cast<char*>(&momentum), sizeof(float));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}

	// load the number of epochs this net has completed
	in.read(reinterpret_cast<char*>(&epochsCompleted), sizeof(int));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}

	// load number of layers

	// load weights as matrices including top row of each for bias weights
	int it = 0;
	in.read(reinterpret_cast<char*>(&it), sizeof(int));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}

	activationFunction.clear();
	weights.resize(it);
	biasWeights.resize(it);
	errors.resize(it);
	outputs.resize(it+1);
//	net.resize(it);
	// load layer sizes - width, height
	Matrix<float>* temp;
	for(int l=0;l<it;l++)
	{
		temp = Matrix<float>::load(in);
		if(temp==NULL)
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}

		//weights[l] = *temp;
		biasWeights[l].resize(temp->getWidth(), 1);
		weights[l].resize(temp->getWidth(), temp->getHeight()-1);
		for(int y=0;y<temp->getHeight();y++)
		{
			for(int x=0;x<temp->getWidth();x++)
			{
				// if this is the bias weight row
				if(y==0)
					biasWeights[l].setValue(x, 0, temp->getValue(x, 0));
				else
					weights[l].setValue(x, y-1, temp->getValue(x, y));
			}
		}
		delete temp;
		temp = NULL;
		activationFunction.push_back(SIGMOID);
	}

	weightsSize = weights.size();

	// load lastPatternTest and patternsCompleted
	if(saveVersion>0)
	{
		in.read(reinterpret_cast<char*>(&lastPatternTest), sizeof(double));
		if(in.fail()) 
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}
		in.read(reinterpret_cast<char*>(&patternsCompleted), sizeof(double));
		if(in.fail()) 
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}
		dynamicLearningRate = false;
		dynamicMomentum = false;
	}

	if(saveVersion>1)
	{
		in.read(reinterpret_cast<char*>(&dynamicLearningRate), sizeof(bool));
		if(in.fail()) 
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}
		in.read(reinterpret_cast<char*>(&dynamicMomentum), sizeof(bool));
		if(in.fail()) 
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}
	}

	if(saveVersion>2)
	{
		in.read(reinterpret_cast<char*>(&inputFieldShape), sizeof(int));
		if(in.fail()) 
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}
	}

	in.close();

	resetWeightChanges();
	
	return true;
}

/** Print useful information about this object. In this case,
output all the weights for each layer. */
void BPN::printInfo(bool printWeights /* = true */) const
{
	LogWriter log;
	char buffer[50];
	sprintf(buffer, "Filename: %s", filename.c_str());
	log.println(buffer);
	sprintf(buffer, "Save version: %d", saveVersion);
	log.println(buffer);
	sprintf(buffer, "Type: %s", typeToString(id));
	log.println(buffer);
	sprintf(buffer, "Learning rate: %*g", 7, learningRate);
	log.println(buffer);
	sprintf(buffer, "Momentum: %*g", 7, momentum);
	log.println(buffer);
	sprintf(buffer, "Epochs completed: %d", epochsCompleted);
	log.println(buffer);
	sprintf(buffer, "Patterns completed: %g", patternsCompleted);
	log.println(buffer);
	sprintf(buffer, "Last pattern test: %g", lastPatternTest);
	log.println(buffer);
	if(dynamicLearningRate)
		sprintf(buffer, "Dynamic learning rate: True");
	else
		sprintf(buffer, "Dynamic learning rate: False");
	log.println(buffer);
	if(dynamicMomentum)
		sprintf(buffer, "Dynamic momentum: True");
	else
		sprintf(buffer, "Dynamic momentum: False");
	log.println(buffer);

	if(inputFieldShape==IFS_SQUARE)
		log.println("Input field shape: IFS_SQUARE");
	else if(inputFieldShape==IFS_DIAMOND)
		log.println("Input field shape: IFS_DIAMOND");
	else
		log.println("Input field shape: UNKNOWN");

	// output neurons info
	sprintf(buffer, "Input neurons: %d", weights[0].getHeight());
	log.println(buffer);
	for(int i=1;i<weights.size();i++)
	{
		sprintf(buffer, "Hidden neurons(%d): %d", i-1, weights[i].getHeight());
		log.println(buffer);
	}
	sprintf(buffer, "Output neurons: %d", weights[weights.size()-1].getWidth());
	log.println(buffer);

	if(printWeights)
	{
		for(int i=0;i<weights.size();i++)
		{
			sprintf(buffer, "Weights for layer %d:", i);
			log.println(buffer);
			getWeights()[i].printInfo();
		}
		for(i=0;i<biasWeights.size();i++)
		{
			sprintf(buffer, "Bias weights for layer %d:", i);
			log.println(buffer);
			biasWeights[i].printInfo();
		}
	}

	// print rank test values
//	if(printTestResults)
//	{
//		log.print("Rank test values: ");
//		for(i=0;i<rankTestValue.size();i++)
//		{
//			sprintf(buffer, "%d", rankTestEpoch[i]);
//			message+=buffer;
//			message+="[";
//			sprintf(buffer, "%*g", 9, rankTestValue[i]);
//			message+=buffer;
//			message+="] ";
//			log.print(message);
//		}
//	}
	log.print("\n");
}

void BPN::completenessTest(NNDatabase& db)
{
	LogWriter log;
	string message;
	char buffer[50];

	log.println("Running training completeness test.");

	vector<Matrix<float> >& input = db.getTrainingInput();
	vector<Matrix<float> >& output = db.getTrainingOutput();
	int correct = 0;
	for(int i=0;i<input.size();i++)
	{
		getAnswer(input[i]);
		if(outputs[outputs.size()-1]==output[i])
			correct++;
		//delete bpnOutput;
		if(i%100==0)
		{
			log.print(".");
		}
	}

	log.print("\n");
	sprintf(buffer, "%d", correct);
	message+=buffer;
	message+= " correct out of ";
	sprintf(buffer, "%d", input.size());
	message+=buffer;
	log.println(message);
	int percent = ((float)correct/(float)input.size())*(float)100;
	sprintf(buffer, "%d", percent);
	message+=buffer;
	message+="% correct";
	log.println(message);
	log.println("Completeness test finished.");
}

// This function simply adds the specified number of neurons to the 
// hidden layer.
void BPN::growHiddenLayer(int extraNodes)
{
	// must have a hidden layer and +.v.e number of nodes to add
	if(weights.size()<2 || extraNodes<1) return;

	// weights between input layer and hidden layer
	// increase the width by extraNodes
	int oldWidth = weights[0].getWidth();
	Matrix<float> temp = weights[0];
	weights[0].resize(weights[0].getWidth()+extraNodes, weights[0].getHeight());
	// and set random values to new weights
	for(int i=0;i<weights[0].getHeight();i++)
	{
		for(int j=0;j<weights[0].getWidth();j++)
		{
			if(j<oldWidth)
				weights[0].setValue(j, i, temp.getValue(j, i));
			else
				weights[0].setValue(j, i, getRandom(WEIGHTMIN, WEIGHTMAX));
		}
	}

	// do bias weights too
	oldWidth = biasWeights[0].getWidth();
	temp = biasWeights[0];
	biasWeights[0].resize(biasWeights[0].getWidth()+extraNodes, biasWeights[0].getHeight());
	// and set random values to new weights
	for(i=0;i<biasWeights[0].getHeight();i++)
	{
		for(int j=0;j<biasWeights[0].getWidth();j++)
		{
			if(j<oldWidth)
				biasWeights[0].setValue(j, i, temp.getValue(j, i));
			else
				biasWeights[0].setValue(j, i, getRandom(WEIGHTMIN, WEIGHTMAX));
		}
	}

	// weights between 1st hidden layer and next layer
	// increase the height by extraNodes
	int oldHeight = weights[1].getHeight();
	temp = weights[1];
	weights[1].resize(weights[1].getWidth(), weights[1].getHeight()+extraNodes);
	// and set random values to new weights
	for(i=0;i<weights[1].getHeight();i++)
	{
		for(int j=0;j<weights[1].getWidth();j++)
		{
			if(i<oldHeight)
				weights[1].setValue(j, i, temp.getValue(j, i));
			else
				weights[1].setValue(j, i, getRandom(WEIGHTMIN, WEIGHTMAX));
		}
	}

	// 1st hidden layer doesn't connect to second bias neuron
	// so skip that one now
}
