#ifndef _MINMAX_H_
#define _MINMAX_H_

#include "reversi.h"

namespace minmax{
	void computeMinmax(reversi::State_t * base, reversi::Movement_t * movement, reversi::Occupancy_t myColor, reversi::Occupancy_t oppColor);	
}

#endif //_MINMAX_H_

