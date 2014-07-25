#pragma warning( disable : 4786 )

#include "globals.h"
#include "BPNExpAct.h"

/** Apply a matrix of input values to the neural network and
return the output values from all neurons.
@params input The matrix of input values.
@return An array of matrices containing the output of each
neuron separated into layers. */
bool BPNExpAct::getAnswer(const Matrix<float>& input)
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
//		const Matrix<float>& thisOutputs = results[l];
		const Matrix<float>& thisOutputs = outputs[l];

		temp.resize(thisWeights.width, thisOutputs.width);
		// previous layer output transposed * next layer weights
		thisOutputs.transpose().multiply(thisWeights, temp);
		// net + bias weights
		temp.sum(biasWeights[l], temp);
		// temp = f(net)
		activationFN(temp, temp, l);
//		results[l+1] = temp.transpose();
		outputs[l+1] = temp.transpose();
	}
	return true;
}
	
/** Save this BPN network.
@params filename The filename to save this object to. */
void BPNExpAct::save(string f)
{
	LogWriter log;
	string message = "Saving ";
	message+=f;
	LogWriter::printerr(message+"\n", typeToString(id));

	ofstream fos(f.c_str(), ios::binary);

	if(!fos)
	{
		message = "BPNExpAct: Could not open file for save: ";
		message+=f;
		LogWriter::printerr(message+"\n", typeToString(id));
		return;
	}

	// save version first
//	fos.write(reinterpret_cast<char*>(&saveVersion), sizeof(int));
	int tempInt = saveVersion;
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

	// save lastPatternTest and patternsCompleted
	if(saveVersion>0)
	{
		double tempDouble = lastPatternTest;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		tempDouble = patternsCompleted;
		fos.write(reinterpret_cast<char*>(&tempDouble), sizeof(double));
		tempInt = sourceType;
		fos.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	}

	fos.close();
}

/** Load a BPN network from the specified file.
@params filename The filename to read this BPN network from. */
bool BPNExpAct::load(string f, bool quiet /*=false*/)
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
	//weights.clear();
	//biasWeights.clear();
	//errors.clear();
	//outputs.clear();
	int it = 0;
	in.read(reinterpret_cast<char*>(&it), sizeof(int));
	if(in.fail()) 
	{
		LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
		return false;
	}

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
		in.read(reinterpret_cast<char*>(&sourceType), sizeof(int));
		if(in.fail()) 
		{
			LogWriter::printerr("Corrupt or incorrect version of .bpn file, aborting...\n", typeToString(id));
			return false;
		}
	}

	in.close();

	lastWeightChanges.push_back(Matrix<float>(weights[0].getWidth(), weights[0].getHeight()));
	lastWeightChanges.push_back(Matrix<float>(weights[1].getWidth(), weights[1].getHeight()));
	resetWeightChanges();
	
	return true;
}

/** Print useful information about this object. In this case,
output all the weights for each layer. */
void BPNExpAct::printInfo(bool printWeights /* = true */) const
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
	sprintf(buffer, "Source network type: %s", typeToString(sourceType));
	log.println(buffer);

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

	log.print("\n");
}