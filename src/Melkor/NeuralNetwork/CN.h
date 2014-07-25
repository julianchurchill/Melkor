#ifndef CN_H
#define CN_H

#pragma warning( disable : 4786 )

#include "BPN.h"

class CN : virtual public BPN
{
	protected:
		int sourceType;

	public:

		CN(int input, vector<int>& hidden, int output, float alpha, float m, int type, int st, float hac, INPUTFIELDSHAPE ifs = IFS_SQUARE) : 
		  BPN(input, hidden, output, alpha, m, type, ifs), sourceType(st)
		{
			  hiddenActivationConstant = hac;
			  if(hidden.size()>1)
			  {
				  setErrorCode(BPN_ERROR);
				  throw "CN nets can only have 1 hidden layer!";
			  }
		}

		CN(string f) : BPN(0.25f, 0.7f, BPN::CNTYPE) 
		{
			if(!load(f))
			{
				setErrorCode(BPN_ERROR);
				throw "CN constructor failed to load file!";
			}
		}
		virtual ~CN() {};

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