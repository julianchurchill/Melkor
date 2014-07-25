#pragma warning( disable : 4786 )

#include "SGFProperty.h"
#include <string>
//using namespace std;

// Game info tags
const string SGFProperty::blackPlayerTag = "PB";
const string SGFProperty::whitePlayerTag = "PW";
const string SGFProperty::blackRankTag = "BR";
const string SGFProperty::whiteRankTag = "WR";
const string SGFProperty::dateTag = "DT";
const string SGFProperty::rulesTag = "RU";
const string SGFProperty::sizeTag = "SZ";
const string SGFProperty::handicapTag = "HA";
const string SGFProperty::komiTag = "KM";
const string SGFProperty::fileFormatTag = "FF";
const string SGFProperty::gameTypeTag = "GM";
const string SGFProperty::applicationTag = "AP";
const string SGFProperty::resultTag = "RE";
const string SGFProperty::timeTag = "TM";
const string SGFProperty::overtimeTag = "OT";

// Board move and markup tags
const string SGFProperty::blackMoveTag = "B";
const string SGFProperty::whiteMoveTag = "W";
const string SGFProperty::blackSetupTag = "AB";
const string SGFProperty::whiteSetupTag = "AW";
const string SGFProperty::emptySetupTag = "AE";
const string SGFProperty::markerTag = "MA";
const string SGFProperty::circleTag = "CR";
const string SGFProperty::squareTag = "SQ";
const string SGFProperty::triangleTag = "TR";
const string SGFProperty::blackTerritoryTag = "TB";
const string SGFProperty::whiteTerritoryTag = "TW";
const string SGFProperty::commentTag = "C";
const string SGFProperty::nameTag = "N";

bool SGFProperty::operator<(const SGFProperty& p1) const
{
	return (name<p1.name) ? true : false;
}

bool SGFProperty::operator==(const SGFProperty& p1) const
{
	return (name==p1.name) ? true : false;
}