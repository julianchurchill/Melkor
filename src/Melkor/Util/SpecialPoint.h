#ifndef SPECIALPOINT_H
#define SPECIALPOINT_H

#pragma warning( disable : 4786 )

#include "Point.h"

class SpecialPoint : public Point<>
{
	public:

		char type;
		float extra;

		SpecialPoint() : Point<>(), type(EMPTY), extra(0.0f) {};
		SpecialPoint(int tx, int ty, int ttype, float textra) : Point<>(tx, ty), type(ttype), extra(textra) {};

		inline void reset() { x = y = 0; type = EMPTY; extra = 0.0f; };
		inline bool operator<(const SpecialPoint& p1) const { return (extra<p1.extra) ? true: false; };
		inline bool operator>(const SpecialPoint& p1) const { return (extra>p1.extra) ? true: false; };
};

#endif