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
		// TODO: detect leaf node (states where neither player can move)
		if (depth <= 0) {
			double value = heuristic->eval(state, myColor, oppColor);
			// TODO: extend search if heuristic value is too good
			return value;
		}

		double bestValue = -INFINITY;
		reversi::Movement_t bestMovement{-1, -1};
		std::vector<reversi::Movement_t> actions;
		actions.reserve(64);
		state->expand(actions, myColor, oppColor);
		reversi::Movement_t minMovement;

		if (actions.empty()) {
			// Special case: we can't make any movements! Let minplayer do its
			// thing.
			movement->x = movement->y = -1;	// "No movement"
			return computeMin(state, heuristic, depth-1, alpha, beta, &minMovement, oppColor, myColor);
		}
		for (std::vector<reversi::Movement_t>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
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
		// TODO: detect leaf node (states where neither player can move)
		if (depth <= 0) {
			double value = heuristic->eval(state, myColor, oppColor);
			// TODO: extend search if heuristic value is too good
			return value;
		}

		double bestValue = INFINITY;
		reversi::Movement_t bestMovement{-1, -1};
		std::vector<reversi::Movement_t> actions;
		actions.reserve(64);
		state->expand(actions, myColor, oppColor);
		reversi::Movement_t minMovement;

		if (actions.empty()) {
			// Special case: we can't make any movements! Let maxplayer do its
			// thing.
			movement->x = movement->y = -1;	// "No movement"
			return computeMax(state, heuristic, depth-1, alpha, beta, &minMovement, oppColor, myColor);
		}
		for (std::vector<reversi::Movement_t>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
			const reversi::Movement_t & move = *it;
			reversi::State_t child(state, &move, myColor);
			double newValue = computeMax(&child, heuristic, depth-1, alpha, beta, &minMovement, oppColor, myColor);
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
