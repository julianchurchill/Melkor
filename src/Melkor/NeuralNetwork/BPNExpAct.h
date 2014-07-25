#ifndef BPNEXPACT_H
#define BPNEXPACT_H

#pragma warning( disable : 4786 )

#include "BPN.h"

class BPNExpAct : virtual public BPN
{
	protected:
		int sourceType;

	public:

		BPNExpAct(int input, vector<int>& hidden, int output, float alpha, float m, int type, int st) : 
		  BPN(input, hidden, output, alpha, m, type), sourceType(st)
		{
//#error setup activation and Dactivation functions for hidden and output layers
			  activationFunction.clear();
		}

		BPNExpAct(string f) : BPN(0.25f, 0.7f, BPN::BPNEXPACTTYPE) 
		{
			if(!load(f))
			{
				setErrorCode(BPN_ERROR);
				throw "BPNExpAct constructor failed to load file!";
			}
		}
		virtual ~BPNExpAct() {};

		int getSourceType() { return sourceType; };

		// Getting output from the net
		virtual bool getAnswer(const Matrix<float>& input);

		virtual void save(string f);
		virtual void save() { NeuralNetwork::save(); };
		virtual bool load(string f, bool quiet = false);
		virtual bool load(bool quiet = false) { return NeuralNetwork::load(quiet); };

		virtual void printInfo(bool printWeights = true) const;
};

#endif