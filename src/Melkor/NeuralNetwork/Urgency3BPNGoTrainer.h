#ifndef URGENCY3BPNGOTRAINER_H
#define URGENCY3BPNGOTRAINER_H

#pragma warning( disable : 4786 )

#include "BPNGoTrainerInterface.h"
#include "SGFReader.h"
#include "NNDatabase.h"
#include <string>
using namespace std;

class Urgency3BPNGoTrainer : public BPNGoTrainerInterface
{
	public:

		Urgency3BPNGoTrainer(BPNGoAdapterInterface* goAdapterTemp)
			: BPNGoTrainerInterface(goAdapterTemp) {};
		virtual ~Urgency3BPNGoTrainer() {};

		virtual void extractPairsFromDirectory(string path, NNDatabase* db, const GoRankRange* rankRange = NULL) const;
		virtual void extractPairs(vector<string>& files, NNDatabase* db,
			int movesFrom = 0, int movesTo = 0, const GoRankRange* rankRange = NULL) const;
		virtual void extractTrainingPairs(SGFReader* sgf, NNDatabase* database,
			int movesFrom = 0, int movesTo = 0, int lookahead = 0, bool quiet = false) const;
};

#endif