#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include "reversi.h"

class Heuristic_t{

public:
	double eval(reversi::State_t * state);
};

#endif //_HEURISTIC_H_

