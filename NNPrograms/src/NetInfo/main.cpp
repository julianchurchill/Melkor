#include "BPN.h"
#include "CN.h"
#include "Misc.h"
#include <iostream>
#include <time.h>

int main(int argc, char* argv[])
{
	// seed the random number generator
	random_seed() = (unsigned)time(NULL);
	srand(random_seed());
	
	if(argc<2)
	{
		cout << "Usage: netinfo <filename> -w [layer number] -a -p" << endl;
		cout << "-w = print weights" << endl;
		cout << "-a = analyse weights" << endl;
		cout << "-p = when analysing weights print each node type in it's own panel" << endl;
		cout << "Or netinfo -spliceWeightImages <directory of numbered .bmps to splice> <output filename>" << endl;
		return 1;
	}

	if(strcmp(argv[1], "-spliceWeightImages")==0)
	{
		if(argc<=3)
		{
			cout << "-spliceWeightImages requires two arguments" << endl;
			return 1;
		}
		// do splicing
		spliceWeightImages(argv[2], argv[3]);
		return 0;
	}

	bool w = false;
	bool a = false;
	bool onePanelPerNodeType = false;
	int layer;
	int sourceType = -1;

	if(BPN::getType(argv[1])==BPN::CNTYPE)
	{
		CN cn(argv[1]);
		sourceType = cn.getSourceType();
	}

	BPN bpn(argv[1]);
	if(bpn.getErrorCode()!=BPN::NONE)
		return 1;

	for(int i=2;i<argc;i++)
	{
		if(strcmp(argv[i], "-w")==0)
		{
			w = true;
			if(i+1>=argc)
			{
				cout << "-w requires an integer argument" << endl;
				return 1;
			}
			if(sscanf(argv[i+1], "%d", &layer)!=1)
			{
				cout << "Bad argument for -w : " << argv[i+1] << endl;
				return 1;
			}
			if(layer<0 || layer>=bpn.getWeights().size())
			{
				cout << "Out of range argument for -w : " << argv[i+1] << endl;
				cout << "Should be between 0 and " << bpn.getWeights().size()-1 << " inclusive." << endl;
				return 1;
			}
			i++;
		}
		else if(strcmp(argv[i], "-a")==0)
			a = true;
		else if(strcmp(argv[i], "-p")==0)
			onePanelPerNodeType = true;
		else 
		{
			cout << "Unknown option: " << argv[i] << endl;
			cout << "Usage: netinfo <filename> -w [layer number] -a" << endl;
			cout << "-w = print weights" << endl;
			cout << "-a = analyse weights" << endl;
			cout << "-p = when analysing weights print each node type in it's own panel" << endl;
			cout << "Or netinfo -spliceWeightImages <directory of numbered .bmps to splice> <output filename>" << endl;
			return 1;
		}
	}

	if(!a)
	{
		if(w)
			bpn.getWeights()[layer].printInfo();
		else
			bpn.printInfo(false);
	}
	else
	{
		//createRawLayerImages(bpn);
		string bitmapPrefix("");
		createLayerImages(bpn, sourceType, bitmapPrefix, onePanelPerNodeType);
	}

	//string filename = bpn.getFilename();
	//filename+="layer0.bmp";
	//checkBMP(filename);

	return 0;
}