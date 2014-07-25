#ifndef NNDATABASE_H
#define NNDATABASE_H

#pragma warning( disable : 4786 )

#include "Matrix.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class NNDatabase 
{
	private:

		static const string myName;
		/** Array to hold all the input training matrices. */
		vector<Matrix<float> > input;
		/** Array to hold all the output training matrices. */
		vector<Matrix<float> > output;
		/** The filename this object will be saved to and loaded from. */
		string filename;
		/** An array of SGF games signatures that made up this
		database. */
		vector<string> signatures;

	public:
	
		NNDatabase();
		virtual ~NNDatabase() {};
		bool equals(NNDatabase* obj);
		void addTrainingPair(Matrix<float>* in, Matrix<float>* out);
		void save();
		void save(string f);
		bool load();
		bool load(string f);

		// List the contents of the file specified in f
		// This does not require all the patterns to be loaded
		// so will be much faster than load() for large databases
		bool listContents(string f);

		vector<Matrix<float> >& getTrainingInput();
		vector<Matrix<float> >& getTrainingOutput();
		void setFilename(string f);
		bool addSignature(string sig);
		bool isContained(string sig);
		void printInfo();
		int getPatternSize();
		void clear();
};

#endif