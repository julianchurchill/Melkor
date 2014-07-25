#pragma warning( disable : 4786 )

#include "NNDatabase.h"
#include "Matrix.h"
#include "LogWriter.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
//using namespace std;

const string NNDatabase::myName = "NNDatabase";

/** This constructor automatically loads a training database
from the specified file. */
NNDatabase::NNDatabase()
{
	// create an empty database
	input.clear();
	output.clear();
	signatures.clear();
	filename = "";
}

/** This method is used to compare two NNDatabase objects. 
@return true if the two NNDatabase objects contain the same
input and output training pairs in the same order, 
false otherwise. */
bool NNDatabase::equals(NNDatabase* obj)
{
	//NNDatabase* other = (NNDatabase*)obj;
	vector<Matrix<float> >& thisInput = getTrainingInput();
	vector<Matrix<float> >& thisOutput = getTrainingOutput();
	vector<Matrix<float> >& otherInput = obj->getTrainingInput();
	vector<Matrix<float> >& otherOutput = obj->getTrainingOutput();
	
	if(thisInput.size()!=otherInput.size()) return false;
	if(thisOutput.size()!=otherOutput.size()) return false;
	
	for(int i=0;i<thisInput.size();i++)
	{
		if(!thisInput[i].equals(&otherInput[i])) return false;
	}
	
	for(i=0;i<thisOutput.size();i++)
	{
		if(!thisOutput[i].equals(&otherOutput[i])) return false;
	}
	
	return true;
}

/** Add the specified training pair to the database.
@param in The input matrix of the training pair.
@param out The expected output matrix of the training pair. */
void NNDatabase::addTrainingPair(Matrix<float>* in, Matrix<float>* out)
{	
	// first check this input is not already in the database
	input.push_back(*in);
	output.push_back(*out);
}

/** A default method that will use filename specified in the
constructor as file to save to. */
void NNDatabase::save()
{
	if(filename!="")
		save(filename);
}

/** Save the database to the specified filename. 
@param f The filename to save the database to. */
void NNDatabase::save(string f)
{
	LogWriter log;
	string message;
	char buffer[50];

	ofstream fos(f.c_str(), ios::binary);

	if(!fos)
	{
		message = "NNDatabase: Could not open file for save: ";
		message+=f;
		log.println(f, myName);
		return;
	}
	
	// save signature list size and contents
	int size = signatures.size();
	message = "Saving signatures: ";
	sprintf(buffer, "%d", size);
	message+=buffer;
	log.println(message, myName);
	fos.write(reinterpret_cast<char*>(&size), sizeof(int));
	int sizeOfString;
	char* sigTemp;
	for(int i=0;i<size;i++)
	{
		sizeOfString = signatures[i].size()+1;
		try {
			sigTemp = new char[sizeOfString];
		} catch(exception& e) {
			cerr << e.what();
			return;
		}
		strcpy(sigTemp, signatures[i].c_str());
		fos.write(reinterpret_cast<char*>(&sizeOfString), sizeof(int));
		fos.write(reinterpret_cast<char*>(sigTemp), sizeOfString);
		delete[] sigTemp;
		//fos.write(reinterpret_cast<char*>(&signatures[i]), sizeof(string));
	}

	// save size of input array list
	message  = "Saving input size: ";
	sprintf(buffer, "%d", input.size());
	message+=buffer;
	log.println(message, myName);
	//dos.writeInt(input.size());
	size = input.size();
	fos.write(reinterpret_cast<char*>(&size), sizeof(int));
	// save input array list
	for(i=0;i<size;i++)
	{
		if(i%100==0)
		{
			log.print(".");
		}
		Matrix<float>::save(fos, &input[i]);
	}
	
	log.print("\n");

	// save size of output array list
	message = "Saving output size: ";
	sprintf(buffer, "%d", output.size());
	message+=buffer;
	log.println(message, myName);
	//dos.writeInt(output.size());
	size = output.size();
	fos.write(reinterpret_cast<char*>(&size), sizeof(int));
	// save output array list
	for(i=0;i<size;i++)
	{
		if(i%100==0)
		{
			log.print(".");
		}
		Matrix<float>::save(fos, &output[i]);
	}
	log.print("\n");
	
	fos.close();

	filename = f;
}

/** A default method that will use filename specified in the
constructor as file to load from. */
bool NNDatabase::load()
{
	if(filename!="")
		return load(filename);
	return false;
}

/** Load the database from the file specified.
@param f The filename to load the database from. */
bool NNDatabase::load(string f)
{
	int size=0;
	
	LogWriter log;
	string message;
	char buffer[50];

	input.clear();
	output.clear();
	
	ifstream in(f.c_str(), ios::binary);

	if(!in)
	{
		message = "Could not load file: ";
		message+=f;
		log.println(message, myName);
		return false;
	}

	filename = f;

	// load signature list size and contents
	in.read(reinterpret_cast<char*>(&size), sizeof(int));
	message = "Loading signatures: ";
	sprintf(buffer, "%d", size);
	message+=buffer;
	log.println(message, myName);
	signatures.reserve(size);
	char* sigTemp;
	int sizeOfString;
	for(int i=0;i<size;i++)
	{
		in.read(reinterpret_cast<char*>(&sizeOfString), sizeof(int));
		try {
			sigTemp = new char[sizeOfString];
		} catch(exception& e) {
			cerr << e.what();
			return false;
		}
		in.read(reinterpret_cast<char*>(sigTemp), sizeOfString);
		signatures.push_back(sigTemp);
		delete[] sigTemp;
		//in.read(reinterpret_cast<char*>(&signatures[i]), sizeof(string));
	}

	// load size of input array list
	in.read(reinterpret_cast<char*>(&size), sizeof(int));
	message  = "Loading input size: ";
	sprintf(buffer, "%d", size);
	message+=buffer;
	log.println(message, myName);
	// load input list objects
	// resize vector to stop it readjusting a lot
	input.reserve(size);
	Matrix<float>* temp;
	for(i=0;i<size;i++)
	{
		if(i%100==0)
		{
			log.print(".");
		}
		temp = Matrix<float>::load(in);
		input.push_back(*temp);
		delete temp;
		temp = NULL;
	}
	
	log.print("\n");
	
	// load size of output array list
	in.read(reinterpret_cast<char*>(&size), sizeof(int));
	message = "Loading output size: ";
	sprintf(buffer, "%d", size);
	message+=buffer;
	log.println(message, myName);
	// load output list objects
	// resize vector to stop it readjusting a lot
	output.reserve(size);
	for(i=0;i<size;i++)
	{
		if(i%100==0)
		{
			log.print(".");
		}
		//output.push_back(Matrix::load(in));
		temp = Matrix<float>::load(in);
		output.push_back(*temp);
		delete temp;
		temp = NULL;
	}
	log.print("\n");

	in.close();
	return true;
}

/**  List the details of the file specified in f.
Note: This will be much quicker than load() to get the
information from large databases.
@param f The filename to examine.
@return A boolean indicating success.
*/
bool NNDatabase::listContents(string f)
{
	int size=0;
	
	LogWriter log;
	string message;
	char buffer[500];

	input.clear();
	output.clear();
	
	ifstream in(f.c_str(), ios::binary);

	if(!in)
	{
		message = "Could not load file: ";
		message+=f;
		log.println(message, myName);
		return false;
	}

	message = "Filename: ";
	message+=f;
	log.println(message);

	// load signature list size and contents
	in.read(reinterpret_cast<char*>(&size), sizeof(int));

	vector<string> thisSignatures;
	thisSignatures.reserve(size);
	char* sigTemp;
	int sizeOfString;
	for(int i=0;i<size;i++)
	{
		in.read(reinterpret_cast<char*>(&sizeOfString), sizeof(int));
		try {
			sigTemp = new char[sizeOfString];
		} catch(exception& e) {
			cerr << e.what();
			return false;
		}
		in.read(reinterpret_cast<char*>(sigTemp), sizeOfString);
		thisSignatures.push_back(sigTemp);
		delete[] sigTemp;
	}

	// load size of input array list
	in.read(reinterpret_cast<char*>(&size), sizeof(int));
	sprintf(buffer, "Number of input matrices: %d", size);
	log.println(buffer);

	// read in the first matrix to get the size of them
	// and measure how many bytes are read for a Matrix
	Matrix<float>* temp;
	int bytesRead = 0;
	temp = Matrix<float>::load(in, &bytesRead);
	int patternWidth = temp->getWidth();
	int patternHeight = temp->getHeight();
	delete temp;
	
	// skip loading the input list objects
	// and skip forwards through the file by (size-1)*bytesRead
	in.seekg((size-1)*bytesRead, ios::cur);

	// check not at the EOF or error occurred
	if(!in.good())
	{
		log.println("Error occurred while skipping input matrices, aborting...");
		return false;
	}

	// load size of output array list
	in.read(reinterpret_cast<char*>(&size), sizeof(int));
	sprintf(buffer, "Number of output matrices: %d", size);
	log.println(buffer);

	// skip loading output list objects
	in.close();

	sprintf(buffer, "Pattern size: %d x %d", patternWidth, patternHeight);
	log.println(buffer);

	// print sgf signatures
	sprintf(buffer, "SGF game signatures: %d", thisSignatures.size());
	log.println(buffer);

	for(i=0;i<thisSignatures.size();i++)
	{
		message = "[";
		sprintf(buffer, "%d", i);
		message+=buffer;
		message+="] = ";
		message+=thisSignatures[i];
		log.println(message);
	}

	return true;
}

/** Get all of this databases' input training matrices. 
@return An array of all the input matrices stored in this database. 
*/
vector<Matrix<float> >& NNDatabase::getTrainingInput()
{
	return input;
}

/** Get all of this databases' output training matrices. 
@return An array of all the output matrices stored in this database. 
*/
vector<Matrix<float> >& NNDatabase::getTrainingOutput()
{
	return output;
}

void NNDatabase::setFilename(string f)
{
	filename = f;
}

bool NNDatabase::addSignature(string sig)
{
	if(isContained(sig))
		return false;
	signatures.push_back(sig);
	return true;
}

bool NNDatabase::isContained(string sig)
{
	for(int i=0;i<signatures.size();i++)
	{
		if(signatures[i]==sig)
			return true;
	}
	return false;
}

void NNDatabase::printInfo()
{
	LogWriter log;
	log.println("NNDatabase Info");
	string message = "Filename: ";
	message+=filename;
	log.println(message);
	message = "Number of input matrices: ";
	char buffer[50];
	sprintf(buffer, "%d", input.size());
	message+=buffer;
	log.println(message);
	message = "Number of output matrices: ";
	sprintf(buffer, "%d", output.size());
	message+=buffer;
	log.println(message);
	if(input.size()>0)
	{
		message = "Pattern size: ";
		sprintf(buffer, "%d", input[0].getWidth());
		message+=buffer;
		message+=" x ";
		sprintf(buffer, "%d", input[0].getHeight());
		message+=buffer;
		log.println(message);
	}
	// print signatures
	message = "SGF game signatures: ";
	sprintf(buffer, "%d", signatures.size());
	message+=buffer;
	log.println(message);
	for(int i=0;i<signatures.size();i++)
	{
		message = "[";
		sprintf(buffer, "%d", i);
		message+=buffer;
		message+="] = ";
		message+=signatures[i];
		log.println(message);
	}
}

int NNDatabase::getPatternSize()
{
	if(input.size()<=0)
		return 0;
	return input[0].getHeight();
}

void NNDatabase::clear()
{
	input.clear();
	output.clear();
	signatures.clear();
	filename = "";
}