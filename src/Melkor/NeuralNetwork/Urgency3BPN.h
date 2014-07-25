#ifndef URGENCY3BPN_H
#define URGENCY3BPN_H

#pragma warning( disable : 4786 )

#include "BPN.h"

class Urgency3BPN : virtual public BPN
{
	protected:
		int sourceType;
		int singlyConnectedHiddenLayer;

	public:

		Urgency3BPN(string f) : BPN(0.25f, 0.7f, BPN::URGENCY3TYPE) 
		{
			if(!load(f))
			{
				setErrorCode(BPN_ERROR);
				throw "Urgency3BPN constructor failed to load file!";
			}
		}
		Urgency3BPN(string createFromBPN, int input, vector<int>& hidden, int output, float alpha, float m, 
			INPUTFIELDSHAPE ifs = IFS_SQUARE);
		virtual ~Urgency3BPN() {};
		
		int getSourceType() { return sourceType; };
		int getSinglyConnectedHiddenLayer() { return singlyConnectedHiddenLayer; };

		// Getting output from the net
		virtual bool getAnswer(const Matrix<float>& input);

		virtual void save(string f);
		virtual void save() { NeuralNetwork::save(); };
		virtual bool load(string f, bool quiet = false);
		virtual bool load(bool quiet = false) { return NeuralNetwork::load(quiet); };

		virtual void printInfo(bool printWeights = true) const;
};

#endif