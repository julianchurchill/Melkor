#include "BPN.h"
#include "Urgency3BPN.h"
#include "CN.h"
#include "BPNGoAdapter.h"
#include "newBPNGoAdapter.h"
#include "newBPN2GoAdapter.h"
#include "newBPN3GoAdapter.h"
#include "newBPN4GoAdapter.h"
#include "newBPN5GoAdapter.h"
#include "newBPN6GoAdapter.h"
#include "newBPN7GoAdapter.h"
#include "newBPN8GoAdapter.h"
#include "newBPN9GoAdapter.h"
#include "newBPN10GoAdapter.h"
#include "newBPN11GoAdapter.h"
#include "newBPN12GoAdapter.h"
#include "Urgency1GoAdapter.h"
#include "Urgency3BPNGoAdapter.h"
#include "CNGoAdapter.h"
#include "Misc.h"
#include <iostream>
#include <time.h>

BPNGoAdapterInterface* createGoAdapter(int useType, BPN* bpn, int sourceType = -1);

int main(int argc, char* argv[])
{
	// seed the random number generator
	random_seed() = (unsigned)time(NULL);
	srand(random_seed());
	
	if(argc<8) {
		cout << "Usage: createnet <filename> <inputnodes> <hiddennodes e.g. 20*20*10* > <outputnodes>" << endl;
		cout << "                 <learning rate> <momentum> <type [BPNTYPE|NEWBPNTYPE|" << endl;
		cout << "                 NEWBPNTYPE2|NEWBPNTYPE3|NEWBPNTYPE4|NEWBPNTYPE5|NEWBPNTYPE6|NEWBPNTYPE7|" << endl;
		cout << "                 NEWBPNTYPE8|NEWBPNTYPE9|NEWBPNTYPE10|NEWBPNTYPE11|NEWBPNTYPE12|NEWBPNTYPE3GOPROBLEMS|" << endl;
		cout << "				  |URGENCY1TYPE|URGENCY3TYPE <source.bpn>|" << endl;
		cout << "                 CNTYPE <sourceType> <hiddenActivationConstant>] >" << endl;
		cout << "                 [-dynamicLearningRate] [-dynamicMomentum]" << endl;
		cout << "                 [-inputFieldShape SQUARE(default)|DIAMOND]" << endl;
		return 1;
	}

	int input, output;
	if(sscanf(argv[2], "%d", &input)!=1) {
		cout << "Bad argument: " << argv[2] << endl;
		return 1;
	}
	if(sscanf(argv[4], "%d", &output)!=1) {
		cout << "Bad argument: " << argv[4] << endl;
		return 1;
	}

	float lr, m;
	if(sscanf(argv[5], "%f", &lr)!=1) {
		cout << "Bad argument: " << argv[5] << endl;
		return 1;
	}
	if(sscanf(argv[6], "%f", &m)!=1) {
		cout << "Bad argument: " << argv[6] << endl;
		return 1;
	}

	INPUTFIELDSHAPE inputFieldShape = IFS_SQUARE;

	// parse hidden nodes/layers string
	// format: n*n*n* etc... 
	vector<int> hidden;
	string h = argv[3];
	int i = 0;
	int t, hn;
	while((t=h.find_first_of("*", i))>i && t<h.size()) {
		// find substring between i and t
		// include i, exclude t
		int length = t-i;
		if(length<=0)
			break;
		if((hn=atoi(h.substr(i, length).c_str()))==0) {
			cout << "Bad argument: " << argv[3] << endl;
			return 1;
		}
		hidden.push_back(hn);
		// continue searching after t
		i = t+1;
	}

	// if the format didn't hold to the end of the argument
	// there must be something wrong...
	if(i!=h.size()) {
		cout << "Probable missing '*' in hidden nodes argument: " << argv[3] << endl;
		cout << "Make sure each hidden layer size is followed by a '*'. " << endl;
		return 1;
	}

	// failsafe check
	int hCount = 0;
	for(i=0;i<h.size();i++) {
		if(h.at(i)=='*')
			hCount++;
	}
	if(hidden.size()!=hCount || hCount==0) {
		cout << "Probable missing '*' in hidden nodes argument: " << argv[3] << endl;
		cout << "Make sure each hidden layer size is followed by a '*'. " << endl;
		return 1;
	}

	int type = BPN::stringToType(argv[7]);
	if(type==-1) {
		cout << "Bad type: " << argv[7] << endl;
		return 1;
	}
	else if(type==BPN::URGENCY3TYPE) {
		if(argc<9) {
			cout << "Missing source filename for URGENCY3TYPE..." << endl;
			return 1;
		}

		for(int i=9;i<argc;i++) {
			if(strcmp(argv[i], "-dynamicLearningRate")==0) {
				cout << "Dynamic learning and momentum not implemented for URGENCY3TYPE..." << endl;
				return 1;
			}
			else if(strcmp(argv[i], "-dynamicMomentum")==0) {
				cout << "Dynamic learning and momentum not implemented for URGENCY3TYPE..." << endl;
				return 1;
			}
			else if(strcmp(argv[i], "-inputFieldShape")==0) {
				i++;
				if(i>=argc) {
					cout << "-inputFieldShape requires a parameter <SQUARE(default)|DIAMOND>" << endl;
					return 1;
				}
				if(strcmp(argv[i], "SQUARE")==0)
					inputFieldShape = IFS_SQUARE;
				else if(strcmp(argv[i], "DIAMOND")==0)
					inputFieldShape = IFS_DIAMOND;
				else {
					cout << "-inputFieldShape requires a parameter <SQUARE(default)|DIAMOND>" << endl;
					return 1;
				}
			}
			else {
				cout << "Unrecognised option: " << argv[i] << endl;
				return 1;
			}
		}

		Urgency3BPN bpn(argv[8], input, hidden, output, lr, m);
		if(bpn.getErrorCode()==BPN::BPN_ERROR)
			return 1;

		// validate the bpn
		string error;
		BPNGoAdapterInterface* goAdapter = createGoAdapter(type, &bpn, bpn.getSourceType());
		if(goAdapter==NULL || !goAdapter->validateParameters(input, hidden, output, error)) {
			cout << "BPN parameter validation error: " << error << endl;
			delete goAdapter;
			return 1;
		}

		bpn.setFilename(argv[1]);
		bpn.save();
		bpn.printInfo(false);
	}
	else if(type==BPN::CNTYPE) {
		if(argc<10) {
			cout << "Missing source type or hidden activation constant for CNTYPE..." << endl;
			return 1;
		}

		if(BPN::stringToType(argv[8])<BPN::FIRSTBPNTYPE || BPN::stringToType(argv[8])>BPN::LASTBPNTYPE) {
			cout << "Bad source type: " << argv[8] << endl;
			return 1;
		}
		float hiddenActivationConstant;
		if(sscanf(argv[9], "%g", &hiddenActivationConstant)!=1) {
			cout << "Bad hidden activation constant argument: " << argv[9] << endl;
			return 1;
		}

		for(int i=10;i<argc;i++) {
			if(strcmp(argv[i], "-dynamicLearningRate")==0) {
				cout << "Dynamic learning and momentum not implemented for CNTYPE..." << endl;
				return 1;
			}
			else if(strcmp(argv[i], "-dynamicMomentum")==0) {
				cout << "Dynamic learning and momentum not implemented for CNTYPE..." << endl;
				return 1;
			}
			else if(strcmp(argv[i], "-inputFieldShape")==0) {
				i++;
				if(i>=argc) {
					cout << "-inputFieldShape requires a parameter <SQUARE(default)|DIAMOND>" << endl;
					return 1;
				}
				if(strcmp(argv[i], "SQUARE")==0)
					inputFieldShape = IFS_SQUARE;
				else if(strcmp(argv[i], "DIAMOND")==0)
					inputFieldShape = IFS_DIAMOND;
				else {
					cout << "-inputFieldShape requires a parameter <SQUARE(default)|DIAMOND>" << endl;
					return 1;
				}
			}
			else {
				cout << "Unrecognised option: " << argv[i] << endl;
				return 1;
			}
		}

		CN bpn(input, hidden, output, lr, m, BPN::CNTYPE, BPN::stringToType(argv[8]), hiddenActivationConstant, inputFieldShape);

		// validate the bpn
		string error;
		BPNGoAdapterInterface* goAdapter = createGoAdapter(type, &bpn, bpn.getSourceType());
		if(goAdapter==NULL || !goAdapter->validateParameters(input, hidden, output, error)) {
			cout << "BPN parameter validation error: " << error << endl;
			delete goAdapter;
			return 1;
		}

		bpn.setFilename(argv[1]);
		bpn.save();
		bpn.printInfo(false);
	}
	else {
		// check for dynamic args
		bool dl = false;
		bool dm = false;
		for(int i=8;i<argc;i++) {
			if(strcmp(argv[i], "-dynamicLearningRate")==0)
				dl = true;
			else if(strcmp(argv[i], "-dynamicMomentum")==0)
				dm = true;
			else if(strcmp(argv[i], "-inputFieldShape")==0) {
				i++;
				if(i>=argc) {
					cout << "-inputFieldShape requires a parameter <SQUARE(default)|DIAMOND>" << endl;
					return 1;
				}
				if(strcmp(argv[i], "SQUARE")==0)
					inputFieldShape = IFS_SQUARE;
				else if(strcmp(argv[i], "DIAMOND")==0)
					inputFieldShape = IFS_DIAMOND;
				else {
					cout << "-inputFieldShape requires a parameter <SQUARE(default)|DIAMOND>" << endl;
					return 1;
				}
			}
			else {
				cout << "Unrecognised option: " << argv[i] << endl;
				return 1;
			}
		}

		BPN bpn(input, hidden, output, lr, m, type, inputFieldShape);

		// validate the bpn
		string error;
		BPNGoAdapterInterface* goAdapter = createGoAdapter(type, &bpn);
		if(goAdapter==NULL || !goAdapter->validateParameters(input, hidden, output, error)) {
			cout << "BPN parameter validation error: " << error << endl;
			delete goAdapter;
			return 1;
		}

		bpn.setFilename(argv[1]);
		bpn.setDynamicLearningRate(dl);
		bpn.setDynamicMomentum(dm);
		bpn.save();
		bpn.printInfo(false);
	}

	return 0;
}

BPNGoAdapterInterface* createGoAdapter(int useType, BPN* bpn, int sourceType /*=-1*/)
{
	if(useType==BPN::BPNTYPE)
		return new BPNGoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE)
		return new newBPNGoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE2)
		return new newBPN2GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE3)
		return new newBPN3GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE4)
		return new newBPN4GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE5)
		return new newBPN5GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE6)
		return new newBPN6GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE7)
		return new newBPN7GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE8)
		return new newBPN8GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE9)
		return new newBPN9GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE10)
		return new newBPN10GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE11)
		return new newBPN11GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE12)
		return new newBPN12GoAdapter(bpn);
	else if(useType==BPN::NEWBPNTYPE3GOPROBLEMS)
		return new newBPN3GoAdapter(bpn);
	else if(useType==BPN::URGENCY1TYPE)
		return new Urgency1GoAdapter(bpn);
	else if(useType==BPN::URGENCY3TYPE)
		return new Urgency3BPNGoAdapter(bpn, sourceType);
	else if(useType==BPN::CNTYPE)
		return new CNGoAdapter(bpn, sourceType);
	else {
		cout << "Bad type value: " << useType << endl;
		return NULL;
	}
}