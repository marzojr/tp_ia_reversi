#include <unordered_map>
#include <string>
#include "opening.h"

namespace reversi{
	typedef std::unordered_map<std::string, Movement_t> Opening_t;
	Opening_t openings{};

	bool getOpening(State_t * state, Movement_t * movement){
		Opening_t::const_iterator it;
		// Unflipped.
		it = openings.find(state->toString(false, false));
		if (it != openings.end()) {
			*movement = it->second;
			return true;
		}
		// Flipped along main diagonal.
		it = openings.find(state->toString(true , false));
		if (it != openings.end()) {
			*movement = it->second;
			return true;
		}
		// Flipped along reverse diagonal
		it = openings.find(state->toString(false, true ));
		if (it != openings.end()) {
			*movement = it->second;
			return true;
		}
		// Flipped along both diagonals (180 degree rotation)
		it = openings.find(state->toString(true , true ));
		if (it != openings.end()) {
			*movement = it->second;
			return true;
		}
		// Initialize to no movement.
		movement->x = -1;
		movement->y = -1;
		return false;
	}
}
