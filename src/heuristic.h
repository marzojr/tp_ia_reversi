#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include "reversi.h"

class Heuristic_t{
	double weightCount;
	double weightFrontier;
	double weightCloseness;
	double weightBoard;
public:
	Heuristic_t() { set(); }
	double eval(
		const reversi::State_t * state, size_t actionsBCnt, size_t actionsWCnt,
		reversi::Occupancy_t color
	) const;
	void set(
		const double weightCount = 10.0, const double weightFrontier = 75.0, const double weightCloseness = 50.0,
		const double weightBoard = 10.0
	);
};

#endif //_HEURISTIC_H_

