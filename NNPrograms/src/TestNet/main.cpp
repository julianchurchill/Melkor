#include "BPN.h"
#include "CN.h"
#include "Urgency3BPN.h"
#include "BPNGoAdapterInterface.h"
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
#include <direct.h>
#include <stdio.h>
#include <vector>
#include <io.h>
#include <time.h>

int main(int argc, char* argv[])
{
	// seed the random number generator
	random_seed() = (unsigned)time(NULL);
	srand(random_seed());
	
	// get list of test files
	vector<string> testsgffiles;
	if(argc<6)
	{
		printf("Usage: TestNet -file nnfilename -test [RANK|LOCALRANK|DISTRIBUTION everyNPercent|LOCALDISTRIBUTION everyNPercent");
		printf("	           |TOPNMOVE topNMoves|LOCALTOPNMOVE topNMoves manhattanSize|TWOMOVECHOICE manhattanSize|GAMESTAGESCORES]\n");
		printf("               -sgfdir dir -[p|printDetails] -[h|hiddenNodeInfo] -[printEveryN <n>]\n");
		printf("       -printEveryN only works for RANK, LOCALRANK, TOPNMOVE and LOCALTOPNMOVE tests.\n");
		return 1;
	}
	

	bool printDetails = false;
	bool hiddenNodeInfo = false;
	string sgfDir = "";
	vector<float> hiddenInfo;

	enum TestType
	{
		NONE,
		RANK,
		LOCALRANK,
		DISTRIBUTION,
		LOCALDISTRIBUTION,
		TOPNMOVE,
		LOCALTOPNMOVE,
		TWOMOVECHOICE,
		URGENCY,
		GAMESTAGESCORES
	};

	TestType testType = NONE;
	int topNMoves = -1;
	int manhattanSize = -1;
	int printEveryN = -1;
	int everyNPercent = -1;

	string nnfilename("");

	// Handle command line arguments
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i], "-file")==0)
		{
			i++;
			if(i>=argc)
			{
				printf("-file option requires an argument...");
				return 1;
			}
			nnfilename = argv[i];
		}
		else if(strcmp(argv[i], "-test")==0)
		{
			i++;
			if(i>=argc)
			{
				printf("-test option requires an argument...");
				return 1;
			}
			if(strcmp(argv[i], "RANK")==0)
				testType = RANK;
			else if(strcmp(argv[i], "LOCALRANK")==0)
				testType = LOCALRANK;
			else if(strcmp(argv[i], "DISTRIBUTION")==0)
			{
				testType = DISTRIBUTION;
				i++;
				if(i>=argc)
				{
					printf("-test DISTRIBUTION requires everyNPercent argument...");
					return 1;
				}
				if(sscanf(argv[i], "%d", &everyNPercent)!=1)
				{
					printf("Bad -test DISTRIBUTION everyNPercent argument: %s", argv[i]);
					return 1;
				}
				if(everyNPercent<=0 || everyNPercent>100 || (100%everyNPercent)!=0)
				{
					cout << "Error: everyNPercent must be between 1 and 100 and exactly divisible into 100." << endl;
					return false;
				}
			}
			else if(strcmp(argv[i], "LOCALDISTRIBUTION")==0)
			{
				testType = LOCALDISTRIBUTION;
				i++;
				if(i>=argc)
				{
					printf("-test LOCALDISTRIBUTION requires everyNPercent argument...");
					return 1;
				}
				if(sscanf(argv[i], "%d", &everyNPercent)!=1)
				{
					printf("Bad -test LOCALDISTRIBUTION everyNPercent argument: %s", argv[i]);
					return 1;
				}
				if(everyNPercent<=0 || everyNPercent>100 || (100%everyNPercent)!=0)
				{
					cout << "Error: everyNPercent must be between 1 and 100 and exactly divisible into 100." << endl;
					return false;
				}
			}
			else if(strcmp(argv[i], "TOPNMOVE")==0)
			{
				testType = TOPNMOVE;
				i++;
				if(i>=argc)
				{
					printf("-test TOPNMOVE requires topNMoves argument...");
					return 1;
				}
				if(sscanf(argv[i], "%d", &topNMoves)!=1)
				{
					printf("Bad -test TOPNMOVE topNMoves argument: %s", argv[i]);
					return 1;
				}
				// range checking
				if(topNMoves<1 || topNMoves>361)
				{
					printf("Out of range -test TOPNMOVE topNMoves argument: %s", argv[i]);
					printf("Should be between 1 and 361 inclusive.");
					return 1;
				}
			}
			else if(strcmp(argv[i], "LOCALTOPNMOVE")==0)
			{
				testType = LOCALTOPNMOVE;
				i++;
				if(i>=argc)
				{
					printf("-test LOCALTOPNMOVE requires topNMoves argument...");
					return 1;
				}
				if(sscanf(argv[i], "%d", &topNMoves)!=1)
				{
					printf("Bad -test LOCALTOPNMOVE topNMoves argument: %s", argv[i]);
					return 1;
				}
				// range checking
				if(topNMoves<1 || topNMoves>361)
				{
					printf("Out of range -test TOPNMOVE topNMoves argument: %s", argv[i]);
					printf("Should be between 1 and 361 inclusive.");
					return 1;
				}
				
				i++;
				if(i>=argc)
				{
					printf("-test LOCALTOPNMOVE requires manhattanSize argument...");
					return 1;
				}
				if(sscanf(argv[i], "%d", &manhattanSize)!=1)
				{
					printf("Bad -test LOCALTOPNMOVE topNMoves manhattanSize argument: %s", argv[i]);
					return 1;
				}
				// range checking
				if(manhattanSize<1)
				{
					printf("Please specify a manhattanSize argument greater than 0...");
					return 1;
				}
			}
			else if(strcmp(argv[i], "TWOMOVECHOICE")==0)
			{
				testType = TWOMOVECHOICE;
				i++;
				if(i>=argc)
				{
					printf("-test TWOMOVECHOICE requires manhattanSize argument...");
					return 1;
				}
				if(sscanf(argv[i], "%d", &manhattanSize)!=1)
				{
					printf("Bad -test TWOMOVECHOICE topNMoves manhattanSize argument: %s", argv[i]);
					return 1;
				}
				// range checking
				if(manhattanSize<1)
				{
					printf("Please specify a manhattanSize argument greater than 0...");
					return 1;
				}
			}
			else if(strcmp(argv[i], "URGENCY")==0)
				testType = URGENCY;
			else if(strcmp(argv[i], "GAMESTAGESCORES")==0)
				testType = GAMESTAGESCORES;
			else
			{
				printf("Invalid test for -test option: %s", argv[i]);
				return 1;
			}
		} // end if -test
		else if(strcmp(argv[i], "-p")==0 || strcmp(argv[i], "-printDetails")==0)
			printDetails = true;
		else if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "-hiddenNodeInfo")==0)
			hiddenNodeInfo = true;
		else if(strcmp(argv[i], "-sgfdir")==0)
		{
			i++;
			if(i>=argc)
			{
				printf("-sgfdir option requires an argument...");
				return 1;
			}
			sgfDir = argv[i];
		}
		else if(strcmp(argv[i], "-printEveryN")==0)
		{
			i++;
			if(i>=argc)
			{
				printf("-printEveryN option requires an integer argument...");
				return 1;
			}
			if(sscanf(argv[i], "%d", &printEveryN)!=1 || printEveryN<=0)
			{
				printf("Bad argument for -printEveryN: %s", argv[i]);
				return 1;
			}
		}
		else
		{
			printf("Unrecognised argument %s\n", argv[i]);
			printf("Usage: TestNet -file nnfilename -test [RANK|LOCALRANK|DISTRIBUTION|LOCALDISTRIBUTION|TOPNMOVE topNMoves|LOCALTOPNMOVE topNMoves manhattanSize|TWOMOVECHOICE manhattanSize]");
			printf("               -sgfdir dir -[p|printDetails] -[h|hiddenNodeInfo] -[printEveryN <n>]");
			printf("       -printEveryN only works for RANK, LOCALRANK, TOPNMOVE and LOCALTOPNMOVE tests.");
			return 1;
		}
	}

	if(nnfilename=="")
	{
		printf("Argument missing: -file");
		return 1;
	}
	if(sgfDir=="")
	{
		printf("Argument missing: -sgfDir");
		return 1;
	}
	if(testType==NONE)
	{
		printf("Argument missing -test");
		return 1;
	}

	int id = BPN::getType(nnfilename);

	BPN* bpn;
	int sourceType = 0;
	if(id==BPN::URGENCY3TYPE)
	{
		Urgency3BPN tempBPN(nnfilename);
		if(tempBPN.getErrorCode()!=BPN::NONE)
		{
			printf("Error in constructing bpn: %s", nnfilename.c_str());
			return 1;
		}
		sourceType = tempBPN.getSourceType();
		bpn = new Urgency3BPN(nnfilename);
	}
	else if(id==BPN::CNTYPE)
	{
		CN tempBPN(nnfilename);
		if(tempBPN.getErrorCode()!=BPN::NONE)
		{
			printf("Error in constructing bpn: %s", nnfilename.c_str());
			return 1;
		}
		sourceType = tempBPN.getSourceType();
		bpn = new CN(nnfilename);
	}
	else
		bpn = new BPN(nnfilename);
	if(bpn->getErrorCode()!=BPN::NONE)
	{
		printf("Error in constructing bpn: %s", nnfilename.c_str());
		delete bpn;
		return 1;
	}

	BPNGoAdapterInterface* goAdapter;

	if(id==BPN::BPNTYPE)
		goAdapter = new BPNGoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE)
		goAdapter = new newBPNGoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE2)
		goAdapter = new newBPN2GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE3 || id==BPN::NEWBPNTYPE3GOPROBLEMS)
		goAdapter = new newBPN3GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE4)
		goAdapter = new newBPN4GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE5)
		goAdapter = new newBPN5GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE6)
		goAdapter = new newBPN6GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE7)
		goAdapter = new newBPN7GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE8)
		goAdapter = new newBPN8GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE9)
		goAdapter = new newBPN9GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE10)
		goAdapter = new newBPN10GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE11)
		goAdapter = new newBPN11GoAdapter(bpn);
	else if(id==BPN::NEWBPNTYPE12)
		goAdapter = new newBPN12GoAdapter(bpn);
	else if(id==BPN::URGENCY1TYPE)
		goAdapter = new Urgency1GoAdapter(bpn);
	else if(id==BPN::URGENCY3TYPE)
		goAdapter = new Urgency3BPNGoAdapter(bpn, sourceType);
	else if(id==BPN::CNTYPE)
		goAdapter = new CNGoAdapter(bpn, sourceType);
	else
	{
		printf("No test available for type: %d", id);
		return 1;
	}

	float result = 0.0f;
	if(id==BPN::TDTYPE || id==BPN::EVALBPNTYPE || 
	   id==BPN::EVALBPN2TYPE || id==BPN::EVALBPN3TYPE)
	{
		if(!goAdapter->evalTest(sgfDir, result, hiddenNodeInfo, &hiddenInfo))
		{
			printf("Test failed aborting...");
			delete goAdapter;
			return 1;
		}
		printf("EvalTest result (average score error): %f\n", result);
	}
	else if(id==BPN::SAFETYTYPE || id==BPN::SAFETY2TYPE)
	{
		_chdir(sgfDir.c_str());
		getFileList(testsgffiles, "*.mgt");
		getFileList(testsgffiles, "*.sgf");
		if(!goAdapter->safetyTest(testsgffiles, result, printDetails))
		{
			printf("Test failed aborting...");
			delete goAdapter;
			return 1;
		}
		printf("SafetyTest result (%%correct): %f\n", result);
	}
	else if(id==BPN::URGENCY1TYPE)
	{
		_chdir(sgfDir.c_str());
		getFileList(testsgffiles, "*.mgt");
		getFileList(testsgffiles, "*.sgf");
		goAdapter->urgencyTest(testsgffiles, result, hiddenNodeInfo, &hiddenInfo);
	}
	else 
	{
		_chdir(sgfDir.c_str());
		getFileList(testsgffiles, "*.mgt");
		getFileList(testsgffiles, "*.sgf");
		if(testType==RANK)
			goAdapter->rankTest(testsgffiles, result, NULL, printDetails, hiddenNodeInfo, &hiddenInfo, printEveryN);
		else if(testType==LOCALRANK)
			goAdapter->localRankTest(testsgffiles, NULL, printDetails, hiddenNodeInfo, &hiddenInfo, printEveryN);
		else if(testType==DISTRIBUTION)
		{
			vector<pair<int,float> > distrib;
			goAdapter->rankTest(testsgffiles, result, &distrib, printDetails, hiddenNodeInfo, &hiddenInfo, printEveryN,
				everyNPercent);

			// print the distribution values
			printf("\nDistribution values: \n\n");
			printf("Percentile    Value      Percentage of moves in this percentile\n");
			for(int i=0;i<distrib.size();i++)
			{
				int lowerBound = i*everyNPercent;
				int upperBound = (i+1)*everyNPercent;
				printf("%d%% - %d%% : %d moves  %g%\n", lowerBound, upperBound, distrib[i].first, distrib[i].second);
			}
		}
		else if(testType==LOCALDISTRIBUTION)
		{
			vector<pair<int,float> > distrib;
			goAdapter->localRankTest(testsgffiles, &distrib, printDetails, hiddenNodeInfo, &hiddenInfo, printEveryN,
				everyNPercent);

			// print the distribution values
			printf("\nDistribution values: \n\n");
			printf("Percentile    Value      Percentage of moves in this percentile\n");
			for(int i=0;i<distrib.size();i++)
			{
				int lowerBound = i*everyNPercent;
				int upperBound = (i+1)*everyNPercent;
				printf("%d%% - %d%% : %d moves  %g%\n", lowerBound, upperBound, distrib[i].first, distrib[i].second);
			}
		}
		else if(testType==TOPNMOVE)
			goAdapter->topNMoveFrequencyTest(testsgffiles, topNMoves, printEveryN);
		else if(testType==LOCALTOPNMOVE)
			goAdapter->localTopNMoveFrequencyTest(testsgffiles, topNMoves,manhattanSize, printEveryN);
		else if(testType==TWOMOVECHOICE)
			goAdapter->twoMoveChoiceTest(testsgffiles, manhattanSize);
		else if(testType==GAMESTAGESCORES)
			goAdapter->gameStageScoresTest(testsgffiles);
		else 
		{
			printf("Invalid test type: %s\n", argv[2]);
			printf("Aborting...");
			delete bpn;
			delete goAdapter;
			return 1;
		}
	}

	// print hidden node activation if requested
	if(hiddenNodeInfo)
	{
		printf("\nAverage hidden node outputs: \n\n");
		for(int i=0;i<hiddenInfo.size();i++)
			printf("Hidden node [%d]: %f\n", i, hiddenInfo.at(i));
	}

	delete bpn;
	delete goAdapter;

	return 0;
}

