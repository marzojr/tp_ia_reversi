#include <algorithm>
#include "minmax.h"
#include "heuristic.h"
#include "opening.h"

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<double>::infinity()
#endif

namespace minmax{
	const int maxDepth = 6;	// For now
	const int depthBound = -5;	// For now

	template<bool max> static double computeMinmax(
		const reversi::State_t * state, Heuristic_t * heuristic,
		int depth, double alpha, double beta, reversi::Movement_t * movement,
		reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor)
	{

		// Expand the current state (neede by heuristic)
		std::vector<reversi::Movement_t> actionsB, actionsW;
		actionsB.reserve(64);
		actionsW.reserve(64);
		state->expand(actionsB, actionsW);

		// If went through full depth, or if reached a leaf node, it is time to wrap up the search.
		if (depth <= 0 || (actionsB.empty() && actionsW.empty())) {
			double value = heuristic->eval(state, actionsB, actionsW, myColor);
			// TODO: extend search if heuristic value is exceptionally good
			return value;
		}


		std::vector<reversi::Movement_t> & myActions = myColor == reversi::Occupancy_t::WHITE ? actionsW : actionsB;
		double bestValue = max ? -INFINITY : INFINITY;
		reversi::Movement_t bestMovement{ -1, -1 };
		reversi::Movement_t movementOpponent;

		// If we have no moves, let the opponent move
		if (myActions.empty()) {
			movement->x = movement->y = -1;	// "No movement"
			return computeMinmax<max ? false : true>(state, heuristic, depth - 1, alpha, beta, &movementOpponent, oppColor, myColor);

		// Otherwise, iterate over movements and choose the best one
		} else for (auto it = myActions.begin(), itend = myActions.end(); it != itend; ++it){

			// Apply the movement and evaluate it
			const reversi::Movement_t & move = *it;
			reversi::State_t child(state, &move, myColor);
			double newValue = computeMinmax<max ? false : true>(&child, heuristic, depth - 1, alpha, beta, &movementOpponent, oppColor, myColor);
				
			// If this is a max-node, keep the largest value and use it as alpha 
			if (max){
				if (alpha < newValue){
					alpha = newValue;
					bestMovement = *it;
					if (beta <= alpha) break;
				}
			// If this is a min-node, keep the smallest value and use it as beta
			} else{
				if (beta > newValue){
					beta = newValue;
					bestMovement = *it;
					if (beta <= alpha) break;
				}
			}
		}

		*movement = bestMovement;
		return max ? alpha : beta;
	}
	
	void computeMinmax(reversi::State_t * base, reversi::Movement_t * movement, reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor){
		if (reversi::getOpening(base, movement)) return;
		Heuristic_t heuristic;
		computeMinmax<true>(base, &heuristic, maxDepth, -INFINITY, INFINITY, movement, myColor, oppColor);
	}
}
