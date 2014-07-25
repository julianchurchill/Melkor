#ifndef SGFWRITER_H
#define SGFWRITER

#pragma warning( disable : 4786 )

#include "Move.h"
#include "SGFReader.h"
#include "SGFTree.h"
#include <string>
#include <vector>
using namespace std;

class SGFWriter 
{
	protected:

		SGFTree* st;

	public:

		SGFWriter() { st = NULL; };
		SGFWriter(vector<Move>& moves, SGFGameInfo& info);
		virtual ~SGFWriter() { if(st!=NULL) delete st; };

		void save(string filename);
		static void save(string filename, SGFTree& tree);
};

#endif