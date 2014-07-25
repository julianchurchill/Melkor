#ifndef BPNGOTRAINERINTERFACE_H
#define BPNGOTRAINERINTERFACE_H

#pragma warning( disable : 4786 )

#include "Misc.h"
#include "BPNGoTrainerInterface.h"
#include "BPNGoAdapterInterface.h"
#include "SGFReader.h"

class BPNGoTrainerInterface
{
	protected:
		BPNGoAdapterInterface* goAdapter;
	
	public:

		BPNGoTrainerInterface(BPNGoAdapterInterface* goAdapterTemp)
			: goAdapter(goAdapterTemp)
		{
			if(goAdapter==NULL)
				throw "BPNGoTrainerInterface requires non NULL BPNGoAdapterInterface!";
		};
		virtual ~BPNGoTrainerInterface() {};

		virtual void extractPairsFromDirectory(string path, NNDatabase* db, const GoRankRange* rankRange = NULL) const = 0;
		virtual void extractPairs(vector<string>& files, NNDatabase* db,
			int movesFrom = 0, int movesTo = 0, const GoRankRange* rankRange = NULL) const = 0;
		virtual void extractTrainingPairs(SGFReader* sgf, NNDatabase* database,
			int movesFrom = 0, int movesTo = 0, int lookahead = 0, bool quiet = false) const = 0;
		// for area finder NNs
		virtual void extractAFPairs(string path, vector<string>& files, NNDatabase* db) const = 0;
		virtual void extractAFTrainingPairs(SGFReader* sgf, NNDatabase* database) const = 0;

};

#endif