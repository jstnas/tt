#ifndef TTIME_H
#define TTIME_H

#include <sys/time.h>

typedef struct timeval TTime;

int get_time(TTime *);
float time_diff(TTime *);

int get_time(TTime *time) {
	// TODO: stop using gettimeofday.
	return gettimeofday(time, NULL);
}

float time_diff(TTime *start) {
	TTime now;
	get_time(&now);
	return (now.tv_sec - start->tv_sec) + 1e-6 * (now.tv_usec - start->tv_usec);
}

#endif
