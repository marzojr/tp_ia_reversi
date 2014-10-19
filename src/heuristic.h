#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include "reversi.h"

class Heuristic_t{

public:
	double eval(const reversi::State_t * state, reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor);
};

#endif //_HEURISTIC_H_

