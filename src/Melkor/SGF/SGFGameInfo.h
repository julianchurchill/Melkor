#ifndef SGFGAMEINFO_H
#define SGFGAMEINFO

#pragma warning( disable : 4786 )

#include <string>
using namespace std;

class SGFGameInfo
{
	public:

		SGFGameInfo();
		virtual ~SGFGameInfo() {};

		void clear();

		// value holders
		string blackPlayer, whitePlayer, blackRank, whiteRank, date, rules;
		int size, handicap, fileFormat, gameType;
		float komi;
		string application, result;
		int time;
		string overtime;
};

#endif