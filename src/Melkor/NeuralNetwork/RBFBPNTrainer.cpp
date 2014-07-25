#include "RBFBPNTrainer.h"
#include "CN.h"
#include <ctime>

// This training method is based on Adrian Cook's PhD thesis work
// This version uses the first two phases:
// 1. Competitive training of hidden nodes (unsupervised).
// 2. Output node training (supervised).

/** Train the neural network against a single input, output
training pair through one epoch.
@params input The input matrix.
@params output The required output matrix. 
@params momentum If true then take the momentum value into
consideration when recalculating weight values. Some amount of the 
previous epochs weight change will be added to each weight. */
bool RBFBPNTrainer::trainPattern(const Matrix<float>& input, const Matrix<float>& output, const vector<int>& freezeLayers,
					   bool doMomentum /*=true*/, bool batchUpdate /*=false*/)
{
	// First check which phase we should be in
	int patternsCompleted = internalBPN->getPatternsCompleted();
	if(patternsCompleted>=(phase1+phase2))
	{
		// phase 3 combined hidden and output training
/* J is hidden node learning strength, K is output node learning strength
procedure TrainCombined(float J, float K)
  float E	{output error value}
  int   j	{hidden node index}
  int   k	{output node index}

  for 1 £ j £ NJ	{for each hidden node}
    sj = 0	{set hidden node's learning strength to zero}
  end for
  CalcActivations	{precalculate all hidden activations}
  for 1 £ k £ NK	{for each output node}
    E = ActualOutput(k) - DesiredOutput(k) {determine output error}
    for 1 £ j £ NJ	{for each hidden node}
      sj -= JEajWkj	{modify hidden node's learning strength according to hidden learning
      Wkj -= KEaj	  strength, output error, hidden activation, and output weight}
    end for	{modify output weight according to output learning strength, output error,
  end for	  and hidden activation}
  for 1 £ j £ NJ	{for each hidden node}
    TrainHiddenNode(j, sj) {train hidden node using hidden node's learning strength}
  end for
end procedure*/
		Matrix<float>& errors0 = internalBPN->errors[0];
		Matrix<float>& weights0 = internalBPN->weights[0];
		Matrix<float>& outputWeights = internalBPN->weights[1];
		int inputNodes = weights0.getHeight();
		int hiddenNodes = weights0.getWidth();
		int outputNodes = outputWeights.getWidth();
		float lr = internalBPN->learningRate;
		float hiddenActivationConstant = internalBPN->getHiddenActivationConstant();
// can't use a linearly degrading learning rate in phase 3 because we have an indefinite number 
// of patterns to train from...
//#ifdef LINEARLY_DEGRADING_LEARNING_RATE
//		lr = internalBPN->learningRate*(((phase1+phase2+1)-patternsCompleted)/(float)(phase1+phase2+1));
//#endif

// ADRIAN COOK'S VERSION OF PHASE3
		vector<float> hiddenErrors(hiddenNodes, 0.0f);

		// input to an RBF hidden unit is the Euclidean norm:
		//    sqrt[sum((Xi-Wij)^2)]
		// where x is each input value, and w is each weight into 
		// the hidden unit

		// calculate hidden errors
		float error, diff;
		float totalErrors = 0.0f;
		int i, j;
		if(errors0.getWidth()!=1 || errors0.getHeight()!=hiddenNodes)
			errors0.resize(1, hiddenNodes);
		for(i=0;i<hiddenNodes;i++)
		{
			error = 0.0f;
			for(j=0;j<inputNodes;j++)
			{
				diff = input.getValue(0, j)-weights0.getValue(i, j);
				error+=diff*diff;
			}
			errors0.setValue(0, i, error);
		} // end for i
		
		// NOTE: We should sqrt the error for each hidden unit
		// but our activation function is exp(-net^2) (Gaussian form)
		// so we square it straight away anyway...

		// convert hidden errors to hidden activations
		for(i=0;i<hiddenNodes;i++)
			errors0.setValue(0, i, exp(-errors0.getValue(0, i)));

		// determine the output errors and train output node weights
		float outputError, actualOutput, outputChange, hiddenActivation, outputWeight;
		for(i=0;i<outputNodes;i++)
		{
			actualOutput = 0.0f;
			for(j=0;j<hiddenNodes;j++)
				actualOutput+=errors0.getValue(0, j)*outputWeights.getValue(i, j);
#ifdef USE_CN_OUTPUT_SIGMOID_ACTIVATION_FUNCTION
			actualOutput = 1/(1+exp(-actualOutput));
#endif
			outputError = actualOutput - output.getValue(i, 0);
#ifdef USE_CN_OUTPUT_SIGMOID_ACTIVATION_FUNCTION
			// multiply the output error by the output activation function derivative
			// which for sigmoid is luckily actValue(1-actValue)
//			outputError*=output.getValue(i, 0)*(1-output.getValue(i, 0));
			// Work on the network output not the target value!!!
			outputError*=actualOutput*(1-actualOutput);
#endif
			for(j=0;j<hiddenNodes;j++)
			{
				// calculate the hidden node error
				hiddenActivation = errors0.getValue(0, j);
				outputWeight = outputWeights.getValue(i, j);
#ifdef RBF_ERRORS
#error how do you calculate the hidden node error for RBF nets?
#endif
				hiddenErrors[j]-=lr*outputError*hiddenActivation*outputWeight;
				// correct the hidden to output weights
				outputChange = outputWeight-(lr*outputError*hiddenActivation);
				outputWeights.setValue(i, j, outputChange);
			}
		} // end for i

		// train hidden node weights
		float w, hiddenLr;
		for(i=0;i<hiddenNodes;i++)
		{
			hiddenLr = hiddenErrors[i];
			for(int j=0;j<inputNodes;j++)
			{
				w = weights0.getValue(i, j);
				weights0.setValue(i, j, w+(hiddenLr*(input.getValue(0, j)-w)));
			}
		}
// END ADRIAN COOK'S VERSION OF PHASE3
	}
	// run phase 2 output training
	else if(patternsCompleted>phase1)
	{
#ifdef RBF_ERRORS
#error do this
#endif
		Matrix<float>& outputWeights = internalBPN->weights[1];
		int hiddenNodes = internalBPN->weights[0].getWidth();
//		int outputNodes = outputWeights.getWidth();
		float lr = internalBPN->learningRate;
#ifdef LINEARLY_DEGRADING_LEARNING_RATE
		lr = internalBPN->learningRate*(((phase1+phase2+1)-patternsCompleted)/(float)(phase1+phase2+1));
#endif

/*procedure CalcActivations
  float s	{average hidden error value}
  int   j	{hidden node index}

  s = AverageHiddenError
  for 1 £ j £ NJ	{for each hidden node}
    aj = e^(-(ej)/s)	{convert hidden error to hidden activation}
  end for
end procedure

float AverageHiddenError
  float e	{cumulative hidden error value}
  int   j	{hidden node index}

  CalcHiddenErrors	{precalculate all hidden errors}
  e = 0
  for 1 £ j £ NJ	{for each hidden node}
    e += ej	{accumulate hidden error}
  end for
return e/NJ	{return average hidden error}

float ActualOutput(int k)
  float A	{cumulative actual output value}
  int   j	{hidden node index}

  A = 0
  for 1 £ j £ NJ	{for each hidden node}
    A += ajWkj	{accumulate actual output using hidden activation and output weight}
  end for
	  return A/NJ	{return actual output} */

/*procedure TrainOutputs(float L)
  float E	{output error value}
  int   j	{hidden node index}
  int   k	{output node index}

  CalcActivations	{precalculate all hidden activations}
  for 1 £ k £ NK	{for each output node}
    E = ActualOutput(k) - DesiredOutput(k) {determine output error}
    for 1 £ j £ NJ	{for each hidden node}
      Wkj -= LEaj	{modify output weight according to learning strength, output error, and
    end for	  hidden activation}
  end for
end procedure*/

		// calculate hidden errors
/*		float error, diff;
		float totalErrors = 0.0f;
		errors.resize(1, hiddenNodes);
		for(int i=0;i<hiddenNodes;i++)
		{
			error = 0.0f;
			for(int j=0;j<inputNodes;j++)
			{
				diff = input.getValue(0, j)-weights0.getValue(i, j);
				error+=diff*diff;
//				error+=pow((input.getValue(0, j)-weights0.getValue(i, j)), 2);
			}
			error = error/(float)inputNodes;
			errors.setValue(0, i, error);
			totalErrors+=error;
		} // end for i

		// find the average hidden error
		float averageHiddenError = totalErrors/hiddenNodes;

		// convert hidden errors to hidden activations
		for(i=0;i<hiddenNodes;i++)
			errors.setValue(0, i, exp(-errors.getValue(0, i)/averageHiddenError));

		// determine the output errors and adapt weights
		float outputError, actualOutput, outputChange;
		for(i=0;i<outputNodes;i++)
		{
			actualOutput = 0.0f;
			for(int j=0;j<hiddenNodes;j++)
				actualOutput+=errors.getValue(0, j)*outputWeights.getValue(i, j);
			actualOutput = actualOutput/(float)hiddenNodes;
		}*/


/*		for(i=0;i<outputNodes;i++)
		{
			outputError = actualOutput - output.getValue(i, 0);
			for(j=0;j<hiddenNodes;j++)
			{
				outputChange = outputWeights.getValue(i, j)-(lr*outputError*errors.getValue(0, j));
				outputWeights.setValue(i, j, outputChange);
			}
		} // end for i*/

		if(!internalBPN->getAnswer(input)) return false;
		
		int outputLayer = internalBPN->getNumberOfLayers()-1;
		Matrix<float>& outputLayerMatrix = internalBPN->outputs[outputLayer];
		Matrix<float>& hiddenOutputs = internalBPN->outputs[1];
		int numberOfOutputs = outputLayerMatrix.getWidth();
		float outputError, outputChange;
		for(int i=0;i<numberOfOutputs;i++)
		{
			outputError = outputLayerMatrix.getValue(i, 0) - output.getValue(i, 0);
#ifdef USE_CN_OUTPUT_SIGMOID_ACTIVATION_FUNCTION
			// multiply the output error by the output activation function derivative
			// which for sigmoid is luckily actValue(1-actValue)
//			outputError*=output.getValue(i, 0)*(1-output.getValue(i, 0));
			// work on network output not target value!!!
			outputError*=outputLayerMatrix.getValue(i, 0)*(1-outputLayerMatrix.getValue(i, 0));
#endif
			for(int j=0;j<hiddenNodes;j++)
			{
				outputChange = outputWeights.getValue(i, j)-(lr*outputError*hiddenOutputs.getValue(0, j));
				outputWeights.setValue(i, j, outputChange);
			}
		}
	}
	// run phase 1 unsupervised hidden node training
	else
	{
#ifdef RBF_ERRORS
#error do this
#endif
		// check this pattern is not one of the bad examples
		// skip it if it is
		if(output.getValue(0, 0)<0.9f)
			return true;

		// current hidden layer width and height is sqrt of hidden node number
		Matrix<float>& errors0 = internalBPN->errors[0];
		Matrix<float>& weights0 = internalBPN->weights[0];
		int inputNodes = weights0.getHeight();
		int hiddenNodes = weights0.getWidth();
		int hiddenSize = sqrt(hiddenNodes);
		float neighbourhoodSize = hiddenSize;
		float lr = internalBPN->learningRate;
#ifdef LINEARLY_DEGRADING_NEIGHBOURHOOD_SIZE
		// set neighbourhood size
		neighbourhoodSize = hiddenSize*(((phase1+1)-patternsCompleted)/(float)(phase1+1));
#endif
#ifdef NO_NEIGHBOURHOOD
		neighbourhoodSize = 1;
#endif
#ifdef LINEARLY_DEGRADING_LEARNING_RATE
		lr = internalBPN->learningRate*(((phase1+1)-patternsCompleted)/(float)(phase1+1));
#endif
/* int HiddenWinner
float m	{minimum hidden error value}
int   j	{hidden node index}
int   w	{winning hidden node index}

CalcHiddenErrors	{precalculate all hidden errors}
m = MAXFLOAT
for 1 £ j £ NJ	{for each hidden node}
	if ej < m	{if hidden error is less than current minimum}
		m = ej	{record hidden error as current minimum}
		w = j	{record hidden node as current winner}
	end if
end for
return w	{return winning hidden node} */

/* procedure CalcHiddenErrors
  int   j	{hidden node index}

  for 1 £ j £ NJ	{for each hidden node}
    ej = HiddenError(j)	{get hidden error}
  end for
end procedure

float HiddenError(int j)
  float e	{cumulative hidden error value}
  int   i	{input node index}

  e = 0
  for 1 £ i £ NI	{for each input node}
    e += (Si - wji)2	{accumulate hidden error between input node and hidden weight}
  end for
return e/NI	{return hidden error} */

		// calculate hidden errors
		float error, diff;
		errors0.resize(1, hiddenNodes);
		for(int i=0;i<hiddenNodes;i++)
		{
			error = 0.0f;
			for(int j=0;j<inputNodes;j++)
			{
				diff = input.getValue(0, j) - weights0.getValue(i, j);
				error+=diff*diff;
//				error+=pow((input.getValue(0, j)-weights0.getValue(i, j)), 2);
			}
			errors0.setValue(0, i, error/(float)inputNodes);
		} // end for i

		// find the best hidden node
		int bestHiddenNode = 0;
		float bestHiddenError = errors0.getValue(0, 0);
		for(i=1;i<hiddenNodes;i++)
		{
			if(errors0.getValue(0, i)<bestHiddenError)
			{
				bestHiddenError = errors0.getValue(0, i);
				bestHiddenNode = i;
			}
		}

/* w = hidden winner, L = learning rate, a and b = width
   and height of neighbourhood
procedure TrainNeighbourhood(int w, float L, float a, float b)
  int   c	{horizontal position of winning hidden node in 2D hidden layer}
  int   d	{vertical position of winning hidden node in 2D hidden layer}
  int   x	{horizontal distance from winning hidden node}
  int   y	{vertical distance from winning hidden node}
  float g	{Gaussian-derived learning strength for each hidden node}
  int   j	{hidden node index}

  c = w mod WJ	{determine horizontal position of winning hidden node}
  d = w/WJ	{determine vertical position of winning hidden node}
  for 1 £ j £ NJ	{for each hidden node}
    x = |j mod WJ - c|	{determine horizontal distance from winning hidden node}
    x = min(x, WJ - x)	{wrap around to opposite side of hidden layer if distance is less}
    y = |j/WJ - d|	{determine vertical distance from winning hidden node}
    y = min(y, HJ - y)	{wrap around to opposite side of hidden layer if distance is less}
    g = e-10(x2/a2 + y2/b2)	{determine learning strength depending on position in neighbourhood}
    TrainHiddenNode(j, Lg) {train hidden node using learning strength}
  end for
end procedure

procedure TrainHiddenNode(int j, float L)
  int   i	{input node index}

  for 1 £ i £ NI	{for each input node}
    wji += L(Si - wji)	{make hidden weight more like input node according to learning strength}
  end for
end procedure*/
	
		// train other hidden nodes to be more like the best one

		// determine horizontal and vertical position of winning hidden node
		int hPos = bestHiddenNode%hiddenSize;
		int vPos = bestHiddenNode/hiddenSize;
		int hDist, vDist;
		float g;
		float w;
		for(i=0;i<hiddenNodes;i++)
		{
			// determine horizontal distance of this hidden node from the winner
			hDist = abs((i%hiddenSize)-hPos);
			// wrap to opposite side of hidden layer if distance is less
			if((hiddenSize-hDist)<hDist) hDist = hiddenSize-hDist;
			// determine vertical distance of this hidden node from the winner
			vDist = abs((i/hiddenSize)-vPos);
			// wrap to opposite side of hidden layer if distance is less
			if((hiddenSize-vDist)<vDist) vDist = hiddenSize-vDist;

			// calculate gaussian learning strength
			g = exp(-10*((hDist*hDist)/(float)(neighbourhoodSize*neighbourhoodSize) + (vDist*vDist)/(float)(neighbourhoodSize*neighbourhoodSize)));
			// multiply by the actual learning rate
			g*=lr;
			for(int j=0;j<inputNodes;j++)
			{
				w = weights0.getValue(i, j);
				weights0.setValue(i, j, w+(g*(input.getValue(0, j)-w)));
			}
		} // end for hiddenNodes
	} // end if phase 1

	internalBPN->patternsCompleted++;
	return true;
} // end trainPattern(Matrix input, Matrix output)
