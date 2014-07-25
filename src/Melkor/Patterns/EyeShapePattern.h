#ifndef EYESHAPEPATTERN_H
#define EYESHAPEPATTERN_H

#pragma warning( disable : 4786 )

#include "globals.h"

/** @brief This class is designed to hold information about eye shape patterns. */
class EyeShapePattern : public Pattern
{
	private:
		// How many eyes can 'O' get if the attacker goes first?
		int attackerEyeValue;
		// How many eyes can the friendly side 'O' get if they go first?
		int defenderEyeValue;

	public:
		static const string PatternTypeID;

		EyeShapePattern();
		EyeShapePattern(const EyeShapePattern& p);

		EyeShapePattern& p operator=(const EyeShapePattern& p);

		int getAttackerEyeValue() const { return attackerEyeValue; };
		int getDefenderEyeValue() const { return defenderEyeValue; };

		// Overload these from Pattern to load up the eye values
		void load(instream& in);
		void save(ostream& out);
		PatternPoint getPoint(const int x, const int y);
};
#endif