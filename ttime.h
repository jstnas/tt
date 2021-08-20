#ifndef TTIME_H
#define TTIME_H

#include <time.h>

typedef struct timespec TTime;

int get_time(TTime **);
double time_diff(TTime *);

int get_time(TTime **time) {
	*time = (TTime *)malloc(sizeof(TTime));
	return clock_gettime(CLOCK_MONOTONIC, *time);
}

double time_diff(TTime *start) {
	TTime *now = NULL;
	get_time(&now);
	return (now->tv_sec - start->tv_sec) + 1e-9 * (now->tv_nsec - start->tv_nsec);
}

#endif
