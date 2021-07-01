#ifndef TTIME_H
#define TTIME_H

#include <time.h>

typedef struct timespec TTime;

int get_time(TTime *);
float time_diff(TTime *);

int get_time(TTime *time) {
	return clock_gettime(CLOCK_MONOTONIC, time);
}

float time_diff(TTime *start) {
	TTime now;
	get_time(&now);
	return (now.tv_sec - start->tv_sec) + 1e-9 * (now.tv_nsec - start->tv_nsec);
}

#endif
