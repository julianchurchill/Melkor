#ifndef SPIRALORDERER_H
#define SPIRALORDERER_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "Orderer.h"
#include "Point.h"

/** @brief This class will suggest points in a spiral order starting from a central point. 
  *
  * This orderer should increment the y-coord once initially and then step diagonally in 
  * an anti-clockwise direction, until the next point in order has already been seen. In which
  * case the order increments the current y-coord once and continues in diagonally as before.
  *
  * E.g, starting at 0,0:
  *
  *   -3-2-1 0 1 2 3
  *
  * -3 . . . M . . .
  * -2 . . N B L . .
  * -1 . O C 4 A K .
  *  0 P D 5 1 3 9 J
  *  1 Q E 6 2 8 I .
  *  2 . R F 7 H . .
  *  3 . . S G . . .
*/
class SpiralOrderer : public Orderer
{
	protected:

		enum Direction { North, North_East, East, South_East, South, South_West, West, North_West };

		Direction currentDirection;
		int edgeLength;
		int edgeStep;

	public:
		SpiralOrderer() : Orderer(), currentDirection( Direction::South ), edgeLength( 0 ), edgeStep( 0 ) {};

		void initialise(int x, int y) { currentDirection = Direction::South; edgeLength = 0; edgeStep = 0; };

		void advance()
		{
			// if edgeLength == 0 AND edgeStep == 0 go s
			//
			// else if direction == s 
			//
			//    edgeStep = 0
			//
			//    if edgeLength == 0 go ne, edgeLength++
			//    else go se
			//
			// else if edgeLength == edgeStep then change direction
			//
			//    edgeStep = 0
			//    
			//    if direction == sw go s
			//    else if direction == se go ne, edgeLength++
			//    else if direction == ne go nw
			//    else if direction == nw go sw
			//
			
			// advance in the current direction
			// edgeStep++

			if( edgeLength == 0 && edgeStep == 0 )
				currentDirection = Direction::South;
			else if( currentDirection == Direction::South ) {
				edgeStep = 0;
				if( edgeLength == 0 ) {
					currentDirection = Direction::North_East;
					edgeLength++;
				}
				else
					currentDirection = Direction::South_East;
			}
			else if( edgeLength == edgeStep ) {
				edgeStep = 0;
				if( currentDirection == Direction::South_West )
					currentDirection = Direction::South;
				else if( currentDirection == Direction::South_East ) {
					currentDirection = Direction::North_East;
					edgeLength++;
				}
				else if( currentDirection == Direction::North_East )
					currentDirection = Direction::North_West;
				else if( currentDirection == Direction::North_West )
					currentDirection = Direction::South_West;
			}

			// Advance in the current direction
			if( currentDirection == Direction::South )
				y++;
			else if( currentDirection == Direction::South_West ) {
				y++;
				x--;
			}
			else if( currentDirection == Direction::South_East ) {
				y++;
				x++;
			}
			else if( currentDirection == Direction::North_West ) {
				y--;
				x--;
			}
			else if( currentDirection == Direction::North_East ) {
				y--;
				x++;
			}

			edgeStep++;

		}
};
#endif