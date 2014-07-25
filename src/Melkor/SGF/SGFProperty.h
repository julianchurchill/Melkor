#ifndef SGFPROPERTY_H
#define SGFPROPERTY_H

#pragma warning( disable : 4786 )

#include <string>
#include <vector>
using namespace std;

class SGFProperty
{
	protected:

		string name;
		vector<string> values;

	public:

		// Tag names to associate with values
		// Game info tags
		static const string blackPlayerTag, whitePlayerTag, blackRankTag, whiteRankTag;
		static const string dateTag, rulesTag;
		static const string sizeTag, handicapTag, fileFormatTag, gameTypeTag, komiTag;
		static const string applicationTag, resultTag;
		static const string timeTag, overtimeTag;

		// Board moves and markup tags
		static const string blackMoveTag, whiteMoveTag, blackSetupTag, whiteSetupTag, emptySetupTag;
		static const string markerTag, circleTag, squareTag, triangleTag;
		static const string blackTerritoryTag, whiteTerritoryTag;
		static const string commentTag, nameTag;

		SGFProperty(string& n, vector<string>& v) : name(n), values(v) { };
		SGFProperty(string& n, string v) : name(n) { values.push_back(v); };
		virtual ~SGFProperty() {};

		vector<string> getValues() const { return values; };
		void setValues(vector<string>& v) { values = v; };
		void addValue(string v) { values.push_back(v); };
		void clearValues() { values.clear(); };

		string getName() const { return name; };
		bool operator<(const SGFProperty& p1) const;
		bool operator==(const SGFProperty& p1) const;
};

#endif