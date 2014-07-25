#pragma warning( disable : 4786 )

#include "NeuralNetwork.h"
#include <string>
#include <vector>
#include <iostream>
//using namespace std;

/** A default save method which will save to the filename
specified in this objects constructor. */
void NeuralNetwork::save()
{
	if(filename!="")
		save(filename);
}

/** A default load method which will load from the filename
specified in this objects constructor. */
bool NeuralNetwork::load(bool quiet /*=false*/)
{
	if(filename!="")
		return load(filename, quiet);
	return false;
}
