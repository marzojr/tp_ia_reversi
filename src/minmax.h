#ifndef _MINMAX_H_
#define _MINMAX_H_

#include "reversi.h"
#include "timer.h"

class Heuristic_t;
namespace minmax{
	extern Timer_t timer;
	void computeMinmax(reversi::State_t * base, const Heuristic_t & H, reversi::Movement_t * movement, reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor);	
}

#endif //_MINMAX_H_

