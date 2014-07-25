#ifndef ORDERER_H
#define ORDERER_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "Orderer.h"
#include "Point.h"

/** @brief This class is an interface for classes that suggest an ordered sequence of coordinates. */
class Orderer
{
	protected:
		int x, y;

	public:
		Orderer() : x(0), y(0) {};

		virtual void initialise(int x, int y) = 0;
		virtual void advance() = 0;
		int currentX() const { return x; };
		int currentY() const { return y; };
};
#endif