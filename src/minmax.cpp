#include <algorithm>
#include "minmax.h"
#include "heuristic.h"
#include "opening.h"

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<double>::infinity()
#endif

namespace minmax{
	const int maxDepth = 7;	// For now
	const int depthBound = -5;	// For now

	template<bool max> static double computeMinmax(
		const reversi::State_t * state, Heuristic_t * heuristic,
		int depth, double alpha, double beta, reversi::Movement_t * movement,
		reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor)
	{

		// Expand the current state (neede by heuristic)
		reversi::Movement_t actionsB[64];
		reversi::Movement_t actionsW[64];
		size_t actionsBCnt, actionsWCnt;
		state->expand(actionsB, actionsBCnt, actionsW, actionsWCnt);

		// If went through full depth, or if reached a leaf node, it is time to wrap up the search.
		if (depth <= 0 || (actionsB == 0 && actionsW == 0)) {
			double value = heuristic->eval(state, actionsB, actionsBCnt, actionsW, actionsWCnt, myColor);
			// TODO: extend search if heuristic value is exceptionally good
			return value;
		}

		// Make an alias for the actions of the current player
		const reversi::Movement_t * myActions = myColor == reversi::Occupancy_t::WHITE ? actionsW : actionsB;
		const size_t myActionsCnt = myColor == reversi::Occupancy_t::WHITE ? actionsWCnt : actionsBCnt;
		size_t bestMovement = (size_t)-1;
		reversi::Movement_t movementOpponent;

		// If we have no moves, let the opponent move
		if (myActionsCnt == 0) {
			movement->x = movement->y = -1;	// "No movement"
			return computeMinmax<!max>(state, heuristic, depth - 1, alpha, beta, &movementOpponent, oppColor, myColor);

		// Otherwise, iterate over movements and choose the best one
		} else for (size_t i = 0; i < myActionsCnt; i++){

			// Apply the movement and evaluate it
			const reversi::Movement_t & move = myActions[i];
			reversi::State_t child(state, &move, myColor);
			double newValue = computeMinmax<!max>(&child, heuristic, depth - 1, alpha, beta, &movementOpponent, oppColor, myColor);
				
			// If this is a max-node, keep the largest value and use it as alpha 
			if (max){
				if (alpha < newValue){
					alpha = newValue;
					bestMovement = i;
					if (beta <= alpha) break;
				}
			// If this is a min-node, keep the smallest value and use it as beta
			} else{
				if (beta > newValue){
					beta = newValue;
					bestMovement = i;
					if (beta <= alpha) break;
				}
			}
		}

		if (bestMovement == (size_t)-1) *movement = reversi::Movement_t{ -1, -1 };
		else *movement = myActions[bestMovement];
		return max ? alpha : beta;
	}
	
	void computeMinmax(reversi::State_t * base, reversi::Movement_t * movement, reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor){
		if (reversi::getOpening(base, movement)) return;
		Heuristic_t heuristic;
		computeMinmax<true>(base, &heuristic, maxDepth, -INFINITY, INFINITY, movement, myColor, oppColor);
	}
}
