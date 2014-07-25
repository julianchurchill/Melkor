#ifndef BPN_H
#define BPN_H

#pragma warning( disable : 4786 )

#include "NeuralNetwork.h"
#include "NNDatabase.h"
#include "Matrix.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

/** Define this to use BPN::highPerformanceGetAnswer() instead of the
 * unoptimised version - BPN::getAnswer(). */
#define USE_HIGH_PERFORMANCE_GET_ANSWER

class BPN : public NeuralNetwork
{
	protected:
		// the layers, represented by a matrix each
		// weights[0] = input-hidden
		// weights[1] = hidden-output
		vector<Matrix<float> > weights;
		vector<Matrix<float> > biasWeights;
		vector<Matrix<float> > lastWeightChanges;
		vector<Matrix<float> > lastBiasWeightChanges;
		vector<Matrix<float> > batchWeightChanges;
		vector<Matrix<float> > batchBiasWeightChanges;

//		vector<Matrix<float> > net;

		float learningRate;
		bool dynamicLearningRate;
		float momentum;
		bool dynamicMomentum;

		int weightsSize;

		int epochsCompleted;
		double lastPatternTest;
		double patternsCompleted;
		int saveVersion;

		// test values to monitor progress
//		vector<float> rankTestValue;
//		vector<int> rankTestEpoch;

		// persistent temps to speed up training
		vector<Matrix<float> > errors;
		vector<Matrix<float> > outputs;
		vector<Matrix<float> > outputsTransposed;
		vector<Matrix<float> > biasErrorDerivatives;
		vector<Matrix<float> > errorDerivatives;
		vector<Matrix<float> > summedErrorDerivatives;
		vector<Matrix<float> > biasSummedErrorDerivatives;

		// number to indicate type of BPN network
		int id;

		// last error result
		int errorCode;

		// What activation function should we use?
		// Can have a different one for each layer...
		vector<int> activationFunction;

//		float averageHiddenError;
		float hiddenActivationConstant;
	
		/* Temporary to speed up getAnswer()
		@see highPerformanceGetAnswer() */ 
		vector<Matrix<float> > getAnswerTemp;

		INPUTFIELDSHAPE inputFieldShape;

	public:

		friend class BPNTrainer;
		friend class CNBPNTrainer;
		friend class RBFBPNTrainer;
		friend class RPROPBPNTrainer;
		friend class TDBPNTrainer;

//		BPN();
		BPN(float alpha, float m, int type, INPUTFIELDSHAPE ifs = IFS_SQUARE);
		BPN(string f);
		BPN(int input, vector<int>& hidden, int output, float alpha, float m, int type, INPUTFIELDSHAPE ifs = IFS_SQUARE);
		virtual ~BPN() {};

		// define for different types of net to train
		static const int BPNTYPE;
		static const int NEWBPNTYPE;
		static const int NEWBPNTYPE2;
		static const int NEWBPNTYPE3;
		static const int NEWBPNTYPE4;
		static const int NEWBPNTYPE5;
		static const int NEWBPNTYPE6;
		static const int NEWBPNTYPE7;
		static const int NEWBPNTYPE8;
		static const int NEWBPNTYPE9;
		static const int NEWBPNTYPE10;
		static const int NEWBPNTYPE11;
		static const int NEWBPNTYPE12;
		static const int NEWBPNTYPE3GOPROBLEMS;
		static const int TDTYPE;
		static const int EVALBPNTYPE;
		static const int EVALBPN2TYPE;
		static const int EVALBPN3TYPE;
		static const int SAFETYTYPE;
		static const int SAFETY2TYPE;
		static const int URGENCY1TYPE;
		static const int URGENCY3TYPE;
		static const int CNTYPE;
		static const int BPNEXPACTTYPE;
		static const int FIRSTBPNTYPE;
		static const int LASTBPNTYPE;

		static int stringToType(const char* t);
		static const char* typeToString(int t);
		static bool hasSourceType(int type);
		static bool hasOffboardNodes(int type);
		static bool has18DistanceNodes(int type);
		static bool has2DistanceNodes(int type);
		static int getNeuronsPerPoint(int type);
		static int getType(string filename);

		int getType() const { return id; };
		string getTypeString() const { return typeToString(id); };

		INPUTFIELDSHAPE getInputFieldShape() const { return inputFieldShape; };

		// define values for different activation functions
		// The BIPOLARSIGMOID function allows the output to
		// in the range -1 to +1.
		// The SIGMOID function allows the output to be in the
		// range 0 to +1.
		static const int SIGMOID;
		static const int BIPOLARSIGMOID;
		static const int EXPONENTIAL_ACTIVATION;

		// error codes if train returns false
		static const int NONE;
		static const int USER_INTERRUPT;
		static const int BPN_ERROR;

		int getErrorCode() const { return errorCode; };
		void setErrorCode(int t) { errorCode = t; };

		double getPatternsCompleted() const { return patternsCompleted; };
		void setPatternsCompleted(double d) { patternsCompleted = d; };
		double getLastPatternTest() const { return lastPatternTest; };
		void setLastPatternTest(double t) { lastPatternTest = t; };

		float getHiddenActivationConstant() { return hiddenActivationConstant; };

		// initialising weights
		vector<Matrix<float> >& getWeights() { return weights; };
		const vector<Matrix<float> >& getWeights() const { return weights; };
		void getWeights(vector<Matrix<float> >& w) const { w = weights; };
		vector<Matrix<float> >& getBiasWeights() { return biasWeights; };
		const vector<Matrix<float> >& getBiasWeights() const { return biasWeights; };
		void getBiasWeights(vector<Matrix<float> >& w) const { w = biasWeights; };

		void initWeights();
		void resetWeightChanges();
		void randomizeLayerWeights(Matrix<float>& layer, float min, 
			float max);
		int getNumberOfLayers() const { return weights.size()+1; };
		vector<Matrix<float> >& getOutputs() { return outputs; };
		const vector<Matrix<float> >& getOutputs() const { return outputs; };
		vector<Matrix<float> >& getErrors() { return errors; };
		const vector<Matrix<float> >& getErrors() const { return errors; };
		vector<Matrix<float> >& getWeightChanges() { return lastWeightChanges; };
		const vector<Matrix<float> >& getWeightChanges() const { return lastWeightChanges; };
		vector<Matrix<float> >& getBiasWeightChanges() { return lastBiasWeightChanges; };
		const vector<Matrix<float> >& getBiasWeightChanges() const { return lastBiasWeightChanges; };

		// setting and getting variables
		void setLearningRate(float l) { learningRate = l; };
		void setDynamicLearningRate(bool l) { dynamicLearningRate = l; };
		void setMomentum(float l) { momentum = l; };
		void setDynamicMomentum(bool l) { dynamicMomentum = l; };
		void setWeights(int layer, Matrix<float>& m) { weights[layer] = m; };
		void setBiasWeights(int layer, Matrix<float>& m) { biasWeights[layer] = m; };
		int getEpochsCompleted() const { return epochsCompleted; };
		void setEpochsCompleted(int d) { epochsCompleted = d; };

#ifdef USE_HIGH_PERFORMANCE_GET_ANSWER
		inline bool getAnswer(const Matrix<float>& input) { return highPerformanceGetAnswer(input); };
#else
		inline bool getAnswer(const Matrix<float>& input) { return unoptimisedGetAnswer(input); };
#endif
		virtual bool unoptimisedGetAnswer(const Matrix<float>& input);
		virtual bool highPerformanceGetAnswer(const Matrix<float>& input);
		virtual bool highPerformanceGetAnswer342_40_1_SIGMOID(const Matrix<float>& input);

		// Activation function
		float activationFN(float i, int layer) const;
		bool activationFN(Matrix<float>& in, Matrix<float>& out, int layer) const;
		float DActivationFN(float i, int layer) const;
		bool DActivationFN(Matrix<float>& in, Matrix<float>& out, int layer) const;
 
		// Sigmoid function (a.k.a. logistic function)
		inline float sigmoidActivationFN(float i) const
			{ return 1/(1+(float)exp(-i)); };
		// Derivative of the activation function
		// assuming that i has already been put through activationFN
		// i = f(net)
		inline float sigmoidDActivationFN(float i) const
			{ return i*(1-i); };
		// Bipolar sigmoid function (a.k.a. logistic function)
		// this allows the output to be between +1 and -1
		inline float bipolarSigmoidActivationFN(float i) const
			{ return (2/(1+(float)exp(-i)))-1; };
		// Derivative of the activation function
		// assuming that i has already been put through activationFN
		// i = f(net)
		// f'(net) = (1/2)*((1+f(net))*(1-f(net)))
		inline float bipolarSigmoidDActivationFN(float i) const
			{ return ((1+i)*(1-i))/2; };
		inline float exponentialActivationFN(float i) const
			{ return exp(-(i*i)); }
		// assume i has already been through exp(-(i*i))
		// so i = f(x)
		// and f'(x) = -2x (exp(-(x*x))
		// and f'(x) = -2x * i
		inline float exponentialDActivationFN(float i) const
		{
			/** @todo This function needs to be implemented */
			return 0.0f;
		}

		virtual void save(string f);
		virtual void save() { NeuralNetwork::save(); };
		virtual bool load(string f, bool quiet = false);
		virtual bool load(bool quiet = false) { return NeuralNetwork::load(quiet); };

		virtual void printInfo(bool printWeights = true) const;
		void completenessTest(NNDatabase& db);

		virtual void growHiddenLayer(int extraNodes);
};

#endif