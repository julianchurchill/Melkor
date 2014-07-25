#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#pragma warning( disable : 4786 )

#include "Matrix.h"
//#include "NNDatabase.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class NeuralNetwork
{
	protected:
	
		string filename;

	public:

		NeuralNetwork() : filename("") {};
		virtual ~NeuralNetwork() {};
		void setFilename(string f) { filename = f; };
		string getFilename() const { return filename; };
		virtual void save();
		virtual void save(string f) = 0;
		virtual bool load(bool quiet = false);
		virtual bool load(string f, bool quiet = false) = 0;
		//virtual int train(NNDatabase* db, int epochs = 0,
		//		float tolerance = 0.1, 
		//		int saveEveryNEpochs = 10,
		//		bool useSymmetry = false) = 0;
		virtual bool getAnswer(const Matrix<float>& input) = 0;
		virtual void printInfo(bool printWeights = true) const = 0;
};

#endif

