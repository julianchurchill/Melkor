#ifndef PATTERNMATCHER_H
#define PATTERNMATCHER_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "BoardStruct.h"
#include "Rect.h"
#include "PatternTree.h"
#include "Pattern.h"
#include "Orderer.h"
#include <vector>

/** @todo Write a unit test for this class. */

/** @brief This class is solely to provide pattern matching functions. */
class PatternMatcher
{
	private:
		PatternTree patternTree;
		vector<Pattern> patterns;

	public:
		void setPatternTree(const PatternTree& tree) { patternTree = tree; };
		PatternTree& getPatternTree() { return patternTree; };

		void setPatterns(const vector<Pattern>& inputPatterns) { patterns = inputPatterns; };
		const vector<Pattern>& getPatterns() const { return patterns; };

		/** @brief This function will attempt to make as many matches as possible
		  * through the PatternTree supplied, on board within the specified bounds. 
		  * @param tree The pattern tree to read from.
		  * @param board The board to match to restricted to bounds.
		  * @param bounds The area on the board to match patterns in.
		  * @param friendlyColour The colour whose point of view we take when considering the patterns.
		  * @param matches A vector to hold any patterns that match within the bounds.
		  * @return True if any patterns are matched, false otherwise.
		  */
		bool match(const PatternTree& tree, const BoardStruct& board, const Rect& bounds, const int friendlyColour,
			vector<Pattern>& matches) 
		{
			/** @todo Write this function! */
			return false;
		}

		/** @brief This function constructs a pattern tree from the vector of patterns
		  * supplied, using the orderer to dictate the order that coordinates are looked at. 
		  * @param inputPatterns The patterns to make into a pattern tree.
		  * @param tree The tree structure to hold the new pattern tree.
		  * @param orderer A point ordering object to determine the order patterns match against the points.
		  */
		void makePatternTree(const vector<Pattern> inputPatterns, PatternTree& tree, Orderer& orderer)
		{
			int centreX = 0;
			int centreY = 0;

			vector<Pattern>::const_iterator patternIter(inputPatterns.begin());
			for(;patternIter!=inputPatterns.end();patternIter++) {
				/** @todo */
				// Add each symmetry of the pattern to the tree

				/** @todo */
				// Find the centre point of the pattern

				// Restart the orderer
				orderer.initialise(centreX, centreY);

				/** @todo */
				// Restart adding at the root of the tree

				/** @todo */
				// For each point in the pattern
				bool pointsLeft = true;
				while(pointsLeft) {
					/** @todo Check bounds and skip if out of bounds. */
					Pattern::PatternPoint point = patternIter->getPoint(orderer.currentX(), orderer.currentY());
					/** @todo Add point to tree. */
					// Get next point
					orderer.advance();
				}
			}

			/** @todo */
			// After last point in tree update leaf node with pattern info
		}

		/** @todo Write this function! */
		void loadPatterns(const string filename) {};
};
#endif