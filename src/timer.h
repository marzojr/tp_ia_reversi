#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>

class Timer_t {
	timeval startTime;
	double elapsedTime;

public:
	Timer_t() : elapsedTime(0.0) {
	}
	void start() {
		gettimeofday(&startTime, 0);
	}
	double getTime() {
		timeval curr;
		gettimeofday(&curr, 0);
		elapsedTime = (curr.tv_sec - startTime.tv_sec) + (curr.tv_usec - startTime.tv_usec) / 1000000.0;
		return elapsedTime;
	}
};

#endif

