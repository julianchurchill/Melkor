#include "SGFReader.h"
#include "BPN.h"
#include "CN.H"
#include "Urgency3BPN.h"
#include "BPNTrainer.h"
#include "CNBPNTrainer.h"
#include "RPROPBPNTrainer.h"
#include "TDBPNTrainer.h"
#include "BPNGoTrainer.h"
#include "Urgency1BPNGoTrainer.h"
#include "Urgency3BPNGoTrainer.h"
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
#include "NNDatabase.h"
#include "LogWriter.h"
#include "Misc.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <io.h>
#include <direct.h>
#include <time.h>

#ifdef __AFX_H__
#define new DEBUG_NEW
#endif

void printOptions();
bool testNetwork(BPN& bpn, BPNGoAdapterInterface& goAdapter, const bool useTestEveryNEpoch, const int testEveryNEpoch, const int testEveryNPattern,
				 const vector<string>& testsgffiles, const string& testDir, const int sourceType,
				 const bool printWeightBitmaps, const string& imageDir, int& bitmapNumber, const char* cwd);
bool createAdapterAndTrainer(int useType, int sourceType, BPN* bpn, BPNGoAdapterInterface* goAdapter, BPNGoTrainerInterface* goTrainer);
void deallocateNetworkObjects(BPN* bpn, BPNTrainer* bpnTrainer, CNBPNTrainer* cnbpnTrainer, RPROPBPNTrainer* rpropbpnTrainer,
							  TDBPNTrainer* tdbpnTrainer, BPNGoAdapterInterface* goAdapter, BPNGoTrainerInterface* goTrainer);

int main(int argc, char* argv[])
{
	// seed the random number generator
	random_seed() = (unsigned)time(NULL);
	srand(random_seed());
	
	// output all logs to stdout
	cout << "Trainnet started" << endl;

	string tempDir("");
	string network;

	int lookahead = 0;
	int rangelow = -100; // -100 means ignore ranges
	int rangehigh = -100; // i.e. 25k-15k

	// how often should we test the network
	bool useTestEveryNEpoch = true;
	bool useTestEveryNSpecified = false;
	int testEveryNEpoch = 100;
	int testEveryNPattern = 20000;

	int trainForNEpochs = -1;

	bool momentum = true;
	bool batchUpdate = false;

	// constructor parameters for CNTRAINER nets
	int phase1 = 0;
	int phase2 = 0;

	// parameters for RPROPBPNTrainer
	float updateMax;

	// Parameters for TDBPNTrainer
	float lambda;

	bool notests = false;
	bool filesSupplied = false;
	string fileList;

	string testDir("test");
	string imageDir("");

	string databaseFilename("");
	bool createDB = false;
	bool addToDB = false;
	bool useSpecifiedDatabase = false;
	bool listDatabase = false;

	bool printWeightBitmaps = false;

	TRAINERTYPE trainerType = BPNTRAINER;

	// argv[0] is program name
	for(int i=1;i<argc;i++) {
		// strcmp returns no. characters different
		if(strcmp(argv[i], "-dir")==0) {
			i++;
			if(i>=argc) {
				cout << "-dir option needs directory argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			tempDir = argv[i];
		}
		else if(strcmp(argv[i], "-listDB") ==0) {
			i++;
			if(i>=argc) {
				cout << "-listDB option needs a filename argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			cout << "-listDatabase option has been specified, all other options will be ignored..." << endl;
			listDatabase = true;
			databaseFilename = argv[i];
			break;
		}
		else if(strcmp(argv[i], "-createDB")==0) {
			i++;
			if(i>=argc) {
				cout << "-createDB option needs filename argument and -network argument and -source argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			databaseFilename = argv[i];
			createDB = true;
		}
		else if(strcmp(argv[i], "-database")==0) {
			i++;
			if(i>=argc) {
				cout << "-database option needs filename argument and -network argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			databaseFilename = argv[i];
			useSpecifiedDatabase = true;
		}
		else if(strcmp(argv[i], "-addToDB")==0) {
			i++;
			if(i>=argc) {
				cout << "-addFiles option needs database filename argument and -network argument and -source argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			databaseFilename = argv[i];
			addToDB = true;
		}
		else if(strcmp(argv[i], "-trainForNEpochs")==0) {
			i++;
			if(i>=argc || sscanf(argv[i], "%d", &trainForNEpochs)!=1) {
				cout << "-trainForNEpochs option needs an integer argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
		}
		else if(strcmp(argv[i], "-network")==0) {
			i++;
			if(i>=argc) {
				cout << "-network option needs filename argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			network = argv[i];
		}
		else if(strcmp(argv[i], "-range")==0) {
			i++;
			if(i>=argc) {
				cout << "-range option needs argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			char gradelow = 'k';
			char gradehigh = 'k';  // k or d
			char c[50];
			sscanf(argv[i], "%s", &c);
			if(strcmp(c, "all")==0) {
				rangelow = -100;
				rangehigh = -100;
				gradelow = 'k';
				gradehigh = 'k';
			}
			else {
				if(sscanf(argv[i], "%d%c-%d%c", &rangelow, &gradelow, &rangehigh, &gradehigh)!=4
					|| (gradelow!='k'&&gradelow!='d') || (gradehigh!='k'&&gradehigh!='d')) {
					cout << "Bad -range argument: " << argv[i] << endl;
					return 1;
				}
				if(gradelow=='k')
					rangelow=-rangelow;
				if(gradehigh=='k')
					rangehigh=-rangehigh;
				// range is now from -30 to +10
				// -.v.e. is kyu, +.v.e. is dan
				if(rangelow>rangehigh) {
					cout << "Bad range: Low range is higher than high range: " << argv[i] << endl;
					return 1;
				}
				//cout << "rangelow=" << rangelow << " gradelow=" << gradelow << endl;
				//cout << "rangehigh=" << rangehigh << " gradehigh=" << gradehigh << endl;
			}
		}
		else if(strcmp(argv[i], "-lookahead")==0) {
			i++;
			if(i>=argc) {
				cout << "-lookahead option needs argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			if(sscanf(argv[i], "%d", &lookahead)!=1 || lookahead<0) {
				cout << "Bad -lookahead argument: " << argv[i] << endl;
				return 1;
			}
		}
		else if(strcmp(argv[i], "-trainingType")==0) {
			i++;
			if(i>=argc) {
				cout << "-trainingType option needs argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			if(strcmp(argv[i], "BPN")==0)
				trainerType = BPNTRAINER;
			else if(strcmp(argv[i], "CN")==0) {
				trainerType = CNTRAINER;
				// read in extra parameters
				// phase1 and phase2
				i++;
				if((i+1)>=argc) {
					cout << "-trainingType CN option needs arguments <phase1Length> <phase2Length>" << endl;
					cout << "Trainnet finished" << endl;
					return 1;
				}
				if(sscanf(argv[i], "%d", &phase1)!=1 || phase1<0) {
					cout << "Bad -trainingType CN argument: " << argv[i] << endl;
					return 1;
				}
				i++;
				if(sscanf(argv[i], "%d", &phase2)!=1 || phase2<0) {
					cout << "Bad -trainingType CN argument: " << argv[i] << endl;
					return 1;
				}
			}
			else if(strcmp(argv[i], "RPROP")==0) {
				trainerType = RPROPTRAINER;
				// read in extra parameters
				i++;
				if((i+1)>=argc) {
					cout << "-trainingType RPROP option needs arguments <updateMax>" << endl;
					cout << "Trainnet finished" << endl;
					return 1;
				}
				if(sscanf(argv[i], "%g", &updateMax)!=1 || updateMax<RPROPBPNTrainer::updateMinDefault) {
					cout << "Bad -trainingType RPROP argument: " << argv[i] << endl;
					return 1;
				}
			}
			else if(strcmp(argv[i], "TD")==0) {
				trainerType = TDTRAINER;
				// read in extra parameters
				i++;
				if((i+1)>=argc) {
					cout << "-trainingType TD option needs arguments <lambda>" << endl;
					cout << "Trainnet finished" << endl;
					return 1;
				}
				if(sscanf(argv[i], "%g", &lambda)!=1 || lambda<TDBPNTrainer::lambdaMinDefault) {
					cout << "Bad -trainingType TD argument: " << argv[i] << endl;
					return 1;
				}
			}
			else {
				cout << "Unrecognised -trainingType option: " << argv[i] << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
		}
		else if(strcmp(argv[i], "-nomomentum")==0)
			momentum = false;
		else if(strcmp(argv[i], "-testEveryNEpoch")==0) {
			if(useTestEveryNSpecified) {
				cout << "Only specify one of -testEveryNEpoch and -testEveryNPattern" << endl;
				return 1;
			}
			useTestEveryNSpecified = true;
			useTestEveryNEpoch = true;
			i++;
			if(i>=argc) {
				cout << "-testEveryNEpoch option needs argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			if(sscanf(argv[i], "%d", &testEveryNEpoch)!=1 || testEveryNEpoch<0) {
				cout << "Bad -testEveryNEpoch argument: " << argv[i] << endl;
				return 1;
			}
		}
		else if(strcmp(argv[i], "-testEveryNPattern")==0) {
			if(useTestEveryNSpecified) {
				cout << "Only specify one of -testEveryNEpoch and -testEveryNPattern" << endl;
				return 1;
			}
			useTestEveryNSpecified = true;
			useTestEveryNEpoch = false;
			i++;
			if(i>=argc) {
				cout << "-testEveryNPattern option needs argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			if(sscanf(argv[i], "%d", &testEveryNPattern)!=1 || testEveryNPattern<0) {
				cout << "Bad -testEveryNPattern argument: " << argv[i] << endl;
				return 1;
			}
		}
		else if(strcmp(argv[i], "-printWeightBitmaps")==0) {
			printWeightBitmaps = true;
			if(!useTestEveryNSpecified) {
				cout << "-printWeightBitmaps option needs -testEveryNPattern or -testEveryNEpoch to be specified to know when to save a bitmap" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			i++;
			if(i>=argc) {
				cout << "-printWeightBitmaps requires a directory arugment to put the image files into." << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			imageDir = argv[i];
		}
		else if(strcmp(argv[i], "-batchUpdate")==0)
			batchUpdate = true;
		else if(strcmp(argv[i], "-notests")==0)
			notests = true;
		else if(strcmp(argv[i], "-files")==0) {
			i++;
			if(i>=argc) {
				cout << "-fileList option needs an argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			fileList = argv[i];
			filesSupplied = true;
		}
		else if(strcmp(argv[i], "-testDir")==0) {
			i++;
			if(i>=argc) {
				cout << "-testDir option needs directory argument" << endl;
				cout << "Trainnet finished" << endl;
				return 1;
			}
			testDir = argv[i];
		}
		else {
			printOptions();
			cout << "Trainnet finished" << endl;
			return 1;
		}
	}

	if(listDatabase) {
		NNDatabase db;
		if(!db.listContents(databaseFilename)) {
			cout << "Load database failed, aborting..." << endl;
			return 1;
		}
		return 0;
	}

	// check command line arguments
	if(createDB || addToDB) {
		// we need -network and -dir arguments
		if(tempDir=="" || network=="") {
			cout << "When specifying -createDB or -addToDB you must also give -network and -dir parameters" << endl;
			printOptions();
			cout << "Trainnet finished" << endl;
			return 1;
		}
	}

	if(useSpecifiedDatabase && trainForNEpochs==-1) {
		cout << "When specifying a particular database to train on you must specify -trainForNEpochs to train for" << endl;
		printOptions();
		cout << "Trainnet finished" << endl;
		return 1;
	}
	else if(!useSpecifiedDatabase && trainForNEpochs!=-1) {
		cout << "-trainForNEpochs option can only be used when specifying a database to train from" << endl;
		printOptions();
		cout << "Trainnet finished" << endl;
		return 1;
	}

	// store the current working directory for future use
	char cwd[200];
	if(_getcwd(cwd, 200)==NULL) {
		cout << "Error with _getcwd" << endl;
		cout << "Trainnet finished" << endl;
		return 1;
	}
	
	// if no database is supplied we must take training patterns direct
	// from sgf files in a specified directory
	vector<string> trainingFiles;
	if(!useSpecifiedDatabase) {
		if(addToDB && tempDir=="") {
			cout << "To add files to a database you must specify a source directory with -dir, containing sgf or mgt files" << endl;
			cout << "Trainnet finished" << endl;
			return 1;
		}

		// get the file list		
		if(tempDir!="" && _chdir(tempDir.c_str())==-1) {
			cout << "Error changing directory" << endl;
			cout << "Trainnet finished" << endl;
			return 1;
		}
		
		if(filesSupplied) {
			// scan fileList make sure all files end with 
			// .sgf or .mgt and add to files
			string file;
			int pos = 0;
			int endPos = 0;
			while(pos<fileList.size()) {
				endPos = fileList.find(" ", pos);
				// if not found, should be last file string
				if(endPos==-1)
					endPos = fileList.size();
				file = fileList.substr(pos, (endPos-pos));
				pos = endPos+1;
				
				// check file ends with .sgf or .mgt
				// throw an error otherwise
				string ext = file.substr(file.size()-4, 4);
				if(ext.compare(".sgf")!=0 && ext.compare(".mgt")!=0) {
					cout << "Error in fileList for -files argument, bad filename: " << file << endl;
					cout << "Trainnet finished" << endl;
					return 1;
				}
				trainingFiles.push_back(file);
			}
		}
		else {
			getFileList(trainingFiles, "*.mgt");
			getFileList(trainingFiles, "*.sgf");
		}
		
		// change back to original directory
		// otherwise extractPairs can't find the file
		// because if prepends the path string anyway
		_chdir(cwd);
		
		if(trainingFiles.size()<=0) {
			cout << "No appropriate files found in " << tempDir << endl;
			cout << "Trainnet finished" << endl;
			return 1;
		}
	}

	/************************************/
	/* Select here what trainer we want */
	/* i.e. BPNTrainer or CNBPNTrainer  */
	/************************************/

	// used later to construct goAdapter
	int sourceType;

	// get the type quick
	int useType = BPN::getType(network);

	BPN* bpn = NULL;
	BPNTrainer* bpnTrainer = NULL;
	CNBPNTrainer* cnbpnTrainer = NULL;
	RPROPBPNTrainer* rpropbpnTrainer = NULL;
	TDBPNTrainer* tdbpnTrainer = NULL;

	vector<int> freezeLayers;

	// If the net is URGENCY3TYPE then we _must_ use Urgency3BPN()
	if(useType==BPN::URGENCY3TYPE) {
		// make sure it loads alright
		bpn = new Urgency3BPN(network);
		if(bpn->getErrorCode()!=BPN::NONE) {
			delete bpn;
			return 1;
		}
		// this is just created to get the source type
		Urgency3BPN tempUrgency3BPN(network);
		sourceType = tempUrgency3BPN.getSourceType();
		int schl = tempUrgency3BPN.getSinglyConnectedHiddenLayer();

		for(int i=0;i<(schl+1);i++)
			freezeLayers.push_back(i);

		// create the trainer
		bpnTrainer = new BPNTrainer(bpn, 0, 0);
	}
	// We must use CNTrainer for CN networks
	else if(useType==BPN::CNTYPE) {
		if(trainerType!=CNTRAINER) {
			cout << "-trainerType argument must be CN when using a CN network." << endl;
			return 1;
		}
		bpn = new CN(network);
		if(bpn->getErrorCode()!=BPN::NONE) {
			delete bpn;
			return 1;
		}
		CN tempCN(network);
		sourceType = tempCN.getSourceType();
		// create the trainer object
		cnbpnTrainer = new CNBPNTrainer(bpn, phase1, phase2);
	}
	// Otherwise the network is BPN based so we have a choice of training mechanisms...
	else {
		// make sure it loads alright
		bpn = new BPN(network);
		if(bpn->getErrorCode()!=BPN::NONE) {
			delete bpn;
			return 1;
		}

		// create the trainer
		if(trainerType==BPNTRAINER)
			bpnTrainer = new BPNTrainer(bpn, 0, 0);
		else if(trainerType==CNTRAINER) {
			if(bpn->getNumberOfLayers()>3 || bpn->getNumberOfLayers()<3) {
				cout << "CNTRAINER option can only be used with 1 hidden layer neural networks." << endl;
				delete bpn;
				return 1;
			}
			// cos the CN training only works with 1 hidden layer nets
			cnbpnTrainer = new CNBPNTrainer(bpn, phase1, phase2);
		}
		else if(trainerType==RPROPTRAINER) {
			rpropbpnTrainer = new RPROPBPNTrainer(bpn, 0, 0);
			rpropbpnTrainer->setUpdateMax(updateMax);
		}
		else if(trainerType==TDTRAINER) {
			tdbpnTrainer = new TDBPNTrainer(bpn, 0, 0);
			tdbpnTrainer->setLambda(lambda);
		}
		else {
			cout << "Bad trainer type specified, cannot create trainer object: ";
			cout << trainerType << endl;
			delete bpn;
			return 1;
		}
	}

	/***********************************/
	/* Allocate appropriate Go trainer */
	/* and adapter objects for BPNs.   */
	/***********************************/

	BPNGoAdapterInterface* goAdapter = NULL;
	BPNGoTrainerInterface* goTrainer = NULL;
	if(!createAdapterAndTrainer(useType, sourceType, bpn, goAdapter, goTrainer)) {
		deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
		return 1;
	}

	/*********************************/
	/* Create or add to a NNDatabase */
	/*********************************/

	int count;
	SGFReader sgf;
	GoRankRange rankRange(rangelow, rangehigh);
	// Add files to a database
	if(addToDB) {
		// add files to a database
		NNDatabase db;
		if(!db.load(databaseFilename)) {
			deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
			return 1;
		}

		// add the files in databaseSource one at a time
		// applying the range filter if any
		// for each file in temp dir
		if(tempDir!="")
			_chdir(tempDir.c_str());
		count = 1;
		for(i=0;i<trainingFiles.size();i++) {
			cout << "Processing - " << count++ << ": " << trainingFiles.at(i) << "...";	
			// check game is within -range specification
			if(!sgf.load(trainingFiles.at(i)))
				continue;

			if(rankRangeFilterSGF(sgf, rankRange)) {
				cout << "Adding - " << count++ << ": " << trainingFiles.at(i) << endl;
				goTrainer->extractTrainingPairs(&sgf, &db, 0, 0, lookahead, true);
			}
			else
				cout << "* Skipping - : " << trainingFiles.at(i) << endl;	
		}
		
		_chdir(cwd);

		// save the database
		db.save();

		// quit
		deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
		cout << "Trainnet finished" << endl;	
		return 0;
	}
	// Create a new database
	else if(createDB) {
		if(tempDir=="") {
			cout << "To create a database you must specify a source directory with -dir, containing sgf or mgt files" << endl;
			cout << "Trainnet finished" << endl;
			deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
			return 1;
		}

		// create a new database using rankRange criteria to filter the files
		NNDatabase db;
		goTrainer->extractPairsFromDirectory(tempDir, &db, &rankRange);
		db.save(databaseFilename);

		// quit
		deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
		cout << "Trainnet finished" << endl;
		return 0;
	}

	/*************************************/
	/* End create or add to a NNDatabase */
	/*************************************/

	string blackRank = "";
	string whiteRank = "";
//	int bRank, wRank;
//	char bGrade, wGrade;

	count = 1;
	int bitmapNumber = 0;

	// get list of test files
	vector<string> testsgffiles;
	if(!notests) {
		_chdir(testDir.c_str());
		getFileList(testsgffiles, "*.mgt");
		getFileList(testsgffiles, "*.sgf");
		_chdir(cwd);
		if(testsgffiles.size()==0) {
			cout << "Warning: No test files found in directory: " << testDir << endl;
			return 1;
		}
	}

	if(printWeightBitmaps) {
		// find the next available bitmap number for this net
		_chdir(imageDir.c_str());
		vector<string> bitmapFiles;
		string filespec("*.bmp");
		getFileList(bitmapFiles, filespec.c_str());
		int highestBitmapNumber = 0;
		int tempInt = 0;
		int layerNumber = 0;
		for(i=0;i<bitmapFiles.size();i++) {
			if(sscanf(bitmapFiles[i].c_str(), "%dlayer%d.bmp", &tempInt, &layerNumber)==2) {
				if(tempInt>highestBitmapNumber)
					highestBitmapNumber = tempInt;
			}
		}
		bitmapNumber = highestBitmapNumber+1;
		_chdir(cwd);
	}

	/**********************************************/
	/* Do the actual training and testing here... */
	/**********************************************/

	int epochsRun;
	if(useSpecifiedDatabase) {
		NNDatabase db;
		if(!db.load(databaseFilename)) {
			cout << "Trainnet finished" << endl;
			deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
			return 1;
		}
		for(i=0;i<trainForNEpochs;i++) {
			// train net for 1 epoch with same database and then check if we need to test
			if(trainerType==CNTRAINER)
				epochsRun = cnbpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, batchUpdate, false);
			else if(trainerType==RPROPTRAINER)
				epochsRun = rpropbpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, true, false);
			else if(trainerType==TDTRAINER)
				epochsRun = tdbpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, true, false);
			else
				epochsRun = bpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, batchUpdate, false);
			// save net
			bpn->save();

			// now test
			if(!notests && epochsRun>0 && !testNetwork(*bpn, *goAdapter, useTestEveryNEpoch, testEveryNEpoch, testEveryNPattern,
				testsgffiles, testDir, sourceType, printWeightBitmaps, imageDir, bitmapNumber, cwd)) 
			{
				cout << "Trainnet finished" << endl;
				deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
				return 1;
			}
		} // end for (i<trainForNEpochs)
	}
	else {
		NNDatabase db;
		// for each file in temp dir
		for(i=0;i<trainingFiles.size();i++) {
			cout << "Processing - " << count++ << ": " << trainingFiles.at(i) << endl;	
			// check game is within -range specification
			if(tempDir!="")
				_chdir(tempDir.c_str());
			if(!sgf.load(trainingFiles.at(i))) {
				_chdir(cwd);
				continue;
			}
			_chdir(cwd);

			if(!rankRangeFilterSGF(sgf, rankRange))
				continue;

			// Create NN database in memory	
			db.clear();
			goTrainer->extractTrainingPairs(&sgf, &db, 0, 0, lookahead, true);

			// train net for 1 epoch
			if(trainerType==CNTRAINER)
				epochsRun = cnbpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, batchUpdate, false);
			else if(trainerType==RPROPTRAINER)
				epochsRun = rpropbpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, true, false);
			else if(trainerType==TDTRAINER)
				epochsRun = tdbpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, true, false);
			else
				epochsRun = bpnTrainer->trainWithDatabase(db, freezeLayers, 1, 0.1f, 0, false, momentum, batchUpdate, false);
			// save net
			bpn->save();
			
			if(!notests && epochsRun>0 && !testNetwork(*bpn, *goAdapter, useTestEveryNEpoch, testEveryNEpoch, testEveryNPattern,
				testsgffiles, testDir, sourceType, printWeightBitmaps, imageDir, bitmapNumber, cwd))
			{
				cout << "Trainnet finished" << endl;
				deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);
				return 1;
			}
		} // end for each file in trainingFiles
	} // end else (if(useSpecifiedDatabase))

	bpn->save();

	deallocateNetworkObjects(bpn, bpnTrainer, cnbpnTrainer, rpropbpnTrainer, tdbpnTrainer, goAdapter, goTrainer);

	cout << "Trainnet finished" << endl;	
	return 0;
}

void printOptions()
{
	cout << "Use 'trainnet -network nnfilename -range level -lookahead n" << endl;
	cout << "              [-trainingType BPN|CN <phase1Epochs> <phase2Epochs>|RPROP <updateMax(default=50.0)>|TD <lambda>]" << endl;
	cout << "              [-dir directory] [-nomomentum]" << endl;
	cout << "              [-files \"list\"] [-batchUpdate] [-notests]" << endl;
	cout << "			   [-testEveryNPattern n | -testEveryNEpoch n]" << endl;
	cout << "              [-printWeightBitmaps] [-testDir directory (default is 'test')]" << endl;
	cout << endl << "Where level is Go level range, e.g. 15k-1k or 1d-5d or all." << endl;
	cout << endl << "-trainingType indicates whether to use standard BPN (default) or another type of training." << endl;
	cout << endl;
	cout << "  or 'trainnet -createDB <filename> -dir <directory> -network <filename>" << endl;
	cout << "  or 'trainnet -database <filename> -addFiles <directory> -network <filename>" << endl;
	cout << "  or 'trainnet -listDB <filename>" << endl;
	cout << "  or 'trainnet -database <filename> -trainForNEpochs <number>' (and the above options to train from a database)" << endl;
}

bool testNetwork(BPN& bpn, BPNGoAdapterInterface& goAdapter, const bool useTestEveryNEpoch, const int testEveryNEpoch, const int testEveryNPattern,
				 const vector<string>& testsgffiles, const string& testDir, const int sourceType,
				 const bool printWeightBitmaps, const string& imageDir, int& bitmapNumber, const char* cwd)
{
	// if total epochs for net is multiple of testEveryNEpoch
	bool doTestNow = false;
	if(useTestEveryNEpoch && (bpn.getEpochsCompleted()%testEveryNEpoch)==0)
		doTestNow = true;
	
	if(!useTestEveryNEpoch && 
		(int)(bpn.getPatternsCompleted()/testEveryNPattern) > (int)(bpn.getLastPatternTest()/testEveryNPattern))
		doTestNow = true;
	
	if(bpn.getEpochsCompleted()!=0 && doTestNow) {
		// create weight bitmaps when we test every N pattern
		if(printWeightBitmaps) {
			_chdir(imageDir.c_str());
			char buffer[50];
			sprintf(buffer, "%.4d", bitmapNumber);
			bitmapNumber++;
			string bitmapFilename(buffer);
			createLayerImages(bpn, sourceType, bitmapFilename);
			_chdir(cwd);
		}
		
		_chdir(testDir.c_str());
		// run degradation test
		float result;
		
		if(bpn.getType()==BPN::SAFETYTYPE || bpn.getType()==BPN::SAFETY2TYPE) {
			if(!goAdapter.safetyTest(testsgffiles, result)) {
				// error
				_chdir(cwd);
				cout << "SafetyTest failed in Trainnet.exe, aborting..." << endl;
				return false;
			}
			else  {
				_chdir(cwd);
				// append test result to appropriate results file
				string filename = bpn.getFilename();
				filename+="-safetyTestResults.txt";
				ofstream fos(filename.c_str(), ios::out|ios::app);
				if(!fos) {
					cout << "Trainnet: Could not open file for appending: ";
					cout << filename << endl;
				}
				else {
					fos << result << endl;
					fos.close();
				}
				bpn.setLastPatternTest(bpn.getPatternsCompleted());
			}
		}
		else if(bpn.getType()==BPN::URGENCY1TYPE) {
			goAdapter.urgencyTest(testsgffiles, result);
			_chdir(cwd);
			string filename = bpn.getFilename();
			filename+="-urgencyTestResults.txt";
			ofstream fos(filename.c_str(), ios::out|ios::app);
			if(!fos) {
				cout << "Trainnet: Could not open file for appending: ";
				cout << filename << endl;
			}
			else {
				fos << result << endl;
				fos.close();
			}
			bpn.setLastPatternTest(bpn.getPatternsCompleted());
		}
		else {
			if(!goAdapter.rankTest(testsgffiles, result, NULL, false, false, NULL, 10)) {
				// test failed
				/*					_chdir(cwd);
				bpn.save();
				return 1;*/
			}
			// run local ranking test
			float result2 = goAdapter.localRankTest(testsgffiles, NULL, false, false, NULL, 10);
			_chdir(cwd);
			// append test result to appropriate results file
			string filename = bpn.getFilename();
			filename+="-rankTestResults.txt";
			ofstream fos(filename.c_str(), ios::out|ios::app);
			if(!fos) {
				cout << "Trainnet: Could not open file for appending: ";
				cout << filename << endl;
			}
			else {
				fos << result << endl;
				fos.close();
			}
			
			filename = bpn.getFilename();
			filename+="-localRankTestResults.txt";
			ofstream fos2(filename.c_str(), ios::out|ios::app);
			if(!fos2) {
				cout << "Trainnet: Could not open file for appending: ";
				cout << filename << endl;
			}
			else {
				fos2 << result2 << endl;
				fos2.close();
			}
			bpn.setLastPatternTest(bpn.getPatternsCompleted());
		} // end else
		_chdir(cwd);
	} // end if (should we test now?)
	return true;
}

/** @brief Allocate appropriate Go adapter and trainer objects for the given bpn type. */
bool createAdapterAndTrainer(int bpnType, int sourceType, BPN* bpn, BPNGoAdapterInterface* goAdapter, BPNGoTrainerInterface* goTrainer)
{
	if(bpnType==BPN::BPNTYPE) {
		goAdapter = new BPNGoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE) {
		goAdapter = new newBPNGoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE2) {
		goAdapter = new newBPN2GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE3) {
		goAdapter = new newBPN3GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE4) {
		goAdapter = new newBPN4GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE5) {
		goAdapter = new newBPN5GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE6) {
		goAdapter = new newBPN6GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE7) {
		goAdapter = new newBPN7GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE8) {
		goAdapter = new newBPN8GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE9) {
		goAdapter = new newBPN9GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE10) {
		goAdapter = new newBPN10GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE11) {
		goAdapter = new newBPN11GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::NEWBPNTYPE12) {
		goAdapter = new newBPN12GoAdapter(bpn);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::URGENCY1TYPE) {
		goAdapter = new Urgency1GoAdapter(bpn);
		goTrainer = new Urgency1BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::URGENCY3TYPE) {
		goAdapter = new Urgency3BPNGoAdapter(bpn, sourceType);
		goTrainer = new Urgency3BPNGoTrainer(goAdapter);
	}
	else if(bpnType==BPN::CNTYPE) {
		goAdapter = new CNGoAdapter(bpn, sourceType);
		goTrainer = new BPNGoTrainer(goAdapter);
	}
	else {
		cout << "Bad type value: " << bpnType << endl;
		return false;
	}
	return true;
}

/** @brief Deallocate all the memory associated with these pointers. */
void deallocateNetworkObjects(BPN* bpn, BPNTrainer* bpnTrainer, CNBPNTrainer* cnbpnTrainer, 
							  RPROPBPNTrainer* rpropbpnTrainer, TDBPNTrainer* tdbpnTrainer, 
							  BPNGoAdapterInterface* goAdapter, BPNGoTrainerInterface* goTrainer)
{
	if(bpn!=NULL) delete bpn;
	if(bpnTrainer!=NULL) delete bpnTrainer;
	if(cnbpnTrainer!=NULL) delete cnbpnTrainer;
	if(rpropbpnTrainer!=NULL) delete rpropbpnTrainer;
	if(tdbpnTrainer!=NULL) delete tdbpnTrainer;
	if(goAdapter!=NULL) delete goAdapter;
	if(goTrainer!=NULL) delete goTrainer;
}