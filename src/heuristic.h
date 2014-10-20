#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include "reversi.h"

class Heuristic_t{

public:
	double eval(
		const reversi::State_t * state,
		const reversi::Movement_t * actionsB, size_t actionsBCnt,
		const reversi::Movement_t * actionsW, size_t actionsWCnt,
		reversi::Occupancy_t color
	);
};

#endif //_HEURISTIC_H_

