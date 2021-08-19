#ifndef TTIME_H
#define TTIME_H

#include <time.h>

typedef struct timespec TTime;

int time_init(TTime **);
void time_free(TTime *);
double time_diff(TTime *);

int
time_init(TTime **time) {
	*time = (TTime *)malloc(sizeof(TTime));
	return clock_gettime(CLOCK_MONOTONIC, *time);
}

void
time_free(TTime *time) {
	free(time);
}

double
time_diff(TTime *start) {
	TTime *now = NULL;
	time_init(&now);
	return (now->tv_sec - start->tv_sec) + 1e-9 * (now->tv_nsec - start->tv_nsec);
}

#endif
