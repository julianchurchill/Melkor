#ifndef ALL_LEGAL_SUGGESTER_H
#define ALL_LEGAL_SUGGESTER_H

#include "MoveSuggester.h"

class AllLegalSuggester : public MoveSuggester
{
	public:

		virtual ~AllLegalSuggester() {};

		/** @brief This function should return the top 'max' number of moves
		  * it can suggest, sorted from high to low, with associated scores in 
		  * the Move objects if available.
		  * @param moves A vector to return the moves in.
		  * @param board The current board position.
		  * @param settings An object containing relevant search settings the suggester might require.
		  * @param nextToPlay The colour next to move.
		  * @param maximisingColour The colour trying to maximise their score in a minimax search.
		  * @param max An optional parameter to state the maximum number of moves required. */
		virtual void getMoves(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings,
			const int nextToPlay, const int maximisingColour, BPNGoAdapterInterface& bpn, int maxMoves = -1) const;

		/** @brief This function should return the top 'max' number of moves
		  * it can suggest. The moves will not be sorted to save processing time.
		  * @param moves A vector to return the moves in.
		  * @param board The current board position.
		  * @param settings An object containing relevant search settings the suggester might require.
		  * @param nextToPlay The colour next to move.
		  * @param maximisingColour The colour trying to maximise their score in a minimax search.
		  * @param max An optional parameter to state the maximum number of moves required. */
		virtual void getMovesUnsorted(vector<Move>& moves, const BoardStruct& board, const SearchSettings& settings,
			const int nextToPlay, const int maximisingColour, BPNGoAdapterInterface& bpn, int maxMoves = -1) const
		{ 
			getMoves(moves, board, settings, nextToPlay, maximisingColour, bpn, maxMoves); 
		};
};


#endif