#include "SGFGameInfo.h"
#include <time.h>
#include <string>
using namespace std;

SGFGameInfo::SGFGameInfo()
{
	blackPlayer = whitePlayer = blackRank = whiteRank = application = rules = result = "";
	overtime = "";
	size = 19;
	handicap = 0;
	komi = 5.5;
	fileFormat = 4;			// the format version we adhere to
	gameType = 1;			// Go
	time = 0;
}

/** @brief Set all game info fields to their defaults. */
void SGFGameInfo::clear() 
{
	blackPlayer = whitePlayer = blackRank = whiteRank = application = rules = result = date = "";
	overtime = "";
	size = 19;
	handicap = 0;
	komi = 5.5;
	fileFormat = 4;			// the format version we adhere to
	gameType = 1;			// Go
	time = 0;
}