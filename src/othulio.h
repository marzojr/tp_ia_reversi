#ifndef _OTHULIO_H_
#define _OTHULIO_H_

#include "reversi.h"

void runOthulio(
	const reversi::Occupancy_t mycolor,
	const reversi::Occupancy_t (*board)[8][8],
	reversi::Movement_t &movement
);

#endif


