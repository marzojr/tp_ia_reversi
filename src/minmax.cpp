#include <algorithm>
#include "minmax.h"
#include "heuristic.h"
#include "opening.h"

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<double>::infinity()
#endif

namespace minmax{
	const int maxDepth = 10;	// For now
	const int depthBound = -5;	// For now

	static double computeMin(const reversi::State_t * state, Heuristic_t * heuristic,
	                         int depth, double alpha, double beta, reversi::Movement_t * movement,
	                         reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor);

	static double computeMax(const reversi::State_t * state, Heuristic_t * heuristic,
	                         int depth, double alpha, double beta, reversi::Movement_t * movement,
	                         reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor){
		std::vector<reversi::Movement_t> actionsB, actionsW;
		actionsB.reserve(64);
		actionsW.reserve(64);
		state->expand(actionsB, actionsW);

		// If went through full depth, or if reached a leaf node, it is time to
		// wrap up the search.
		if (depth <= 0 || (actionsB.empty() && actionsW.empty())) {
			double value = heuristic->eval(state, actionsB, actionsW, myColor);
			// TODO: extend search if heuristic value is too good
			return value;
		}

		std::vector<reversi::Movement_t> & myActions  = myColor == reversi::Occupancy_t::WHITE ? actionsW : actionsB;
		double bestValue = INFINITY;
		reversi::Movement_t bestMovement{-1, -1};
		reversi::Movement_t minMovement;
		if (myActions.empty()) {
			// Special case: we can't make any movements! Let maxplayer do its
			// thing: we already know that he has movements because of the check
			// above.
			movement->x = movement->y = -1;	// "No movement"
			return computeMin(state, heuristic, depth-1, alpha, beta, &minMovement, oppColor, myColor);
		}
		for (std::vector<reversi::Movement_t>::const_iterator it = myActions.begin(); it != myActions.end(); ++it) {
			const reversi::Movement_t & move = *it;
			reversi::State_t child(state, &move, myColor);
			double newValue = computeMin(&child, heuristic, depth-1, alpha, beta, &minMovement, oppColor, myColor);
			if (newValue > bestValue) {
				bestMovement = *it;
				bestValue = newValue;
			}
			if (bestValue >= beta) {
				*movement = bestMovement;
				return bestValue;
			}
			if (bestValue >= alpha) {
				alpha = bestValue;
			}
		}
		*movement = bestMovement;
		return bestValue;
	}

	static double computeMin(const reversi::State_t * state, Heuristic_t * heuristic,
	                         int depth, double alpha, double beta, reversi::Movement_t * movement,
	                         reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor){
		std::vector<reversi::Movement_t> actionsB, actionsW;
		actionsB.reserve(64);
		actionsW.reserve(64);
		state->expand(actionsB, actionsW);

		// If went through full depth, or if reached a leaf node, it is time to
		// wrap up the search.
		if (depth <= 0 || (actionsB.empty() && actionsW.empty())) {
			double value = heuristic->eval(state, actionsB, actionsW, myColor);
			// TODO: extend search if heuristic value is too good
			return value;
		}

		std::vector<reversi::Movement_t> & myActions  = myColor == reversi::Occupancy_t::WHITE ? actionsW : actionsB;
		double bestValue = INFINITY;
		reversi::Movement_t bestMovement{-1, -1};
		reversi::Movement_t maxMovement;
		if (myActions.empty()) {
			// Special case: we can't make any movements! Let maxplayer do its
			// thing: we already know that he has movements because of the check
			// above.
			movement->x = movement->y = -1;	// "No movement"
			return computeMax(state, heuristic, depth-1, alpha, beta, &maxMovement, oppColor, myColor);
		}
		for (std::vector<reversi::Movement_t>::const_iterator it = myActions.begin(); it != myActions.end(); ++it) {
			const reversi::Movement_t & move = *it;
			reversi::State_t child(state, &move, myColor);
			double newValue = computeMax(&child, heuristic, depth-1, alpha, beta, &maxMovement, oppColor, myColor);
			if (newValue < bestValue) {
				bestMovement = *it;
				bestValue = newValue;
			}
			if (bestValue <= alpha) {
				*movement = bestMovement;
				return bestValue;
			}
			if (bestValue < beta) {
				beta = bestValue;
			}
		}
		*movement = bestMovement;
		return bestValue;
	}

	void computeMinmax(reversi::State_t * base, reversi::Movement_t * movement, reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor){
		if (reversi::getOpening(base, movement)) return;
		Heuristic_t heuristic;
		computeMax(base, &heuristic, maxDepth, -INFINITY, INFINITY, movement, myColor, oppColor);
	}
}
