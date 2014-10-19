#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include "reversi.h"

class Heuristic_t{

public:
	double eval(
		const reversi::State_t * state, 
		const std::vector<reversi::Movement_t> & actionsB, 
		const std::vector<reversi::Movement_t> & actionsW, 
		reversi::Occupancy_t color
	);
};

#endif //_HEURISTIC_H_

