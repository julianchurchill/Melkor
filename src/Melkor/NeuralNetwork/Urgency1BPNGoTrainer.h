#ifndef URGENCY1BPNGOTRAINER_H
#define URGENCY1BPNGOTRAINER_H

#pragma warning( disable : 4786 )

#include "BPNGoTrainerInterface.h"
#include "SGFReader.h"
#include "NNDatabase.h"
#include <string>
using namespace std;

class Urgency1BPNGoTrainer : public BPNGoTrainerInterface
{
	public:

		Urgency1BPNGoTrainer(BPNGoAdapterInterface* goAdapterTemp)
			: BPNGoTrainerInterface(goAdapterTemp) {};
		virtual ~Urgency1BPNGoTrainer() {};

		virtual void extractPairsFromDirectory(string path, NNDatabase* db, const GoRankRange* rankRange = NULL) const;
		virtual void extractPairs(vector<string>& files, NNDatabase* db,
			int movesFrom = 0, int movesTo = 0, const GoRankRange* rankRange = NULL) const;
		virtual void extractTrainingPairs(SGFReader* sgf, NNDatabase* database,
			int movesFrom = 0, int movesTo = 0, int lookahead = 0, bool quiet = false) const;
		// for area finder NNs
		virtual void extractAFPairs(string path, vector<string>& files, NNDatabase* db) const;
		virtual void extractAFTrainingPairs(SGFReader* sgf, NNDatabase* database) const;
};

#endif