#ifndef RESULT_H
#define RESULT_H

#include "mode.h"

typedef struct {
	Mode mode;
	unsigned length;
	double wpm, time;
	time_t seed;
} Result;

void result_init(Result **, Mode, unsigned, time_t);
void result_free(Result *);
void result_new(Result *, Mode, unsigned, time_t);

void
result_init(Result **result, Mode mode, unsigned length, time_t seed) {
	*result = (Result *)malloc(sizeof(Result));
	result_new(*result, mode, length, seed);
}

void
result_free(Result *result) {
	free(result);
}

void
result_new(Result *result, Mode mode, unsigned length, time_t seed) {
	result->mode = mode;
	result->length = length;
	result->wpm = 0;
	result->time = 0;
	result->seed = seed;
}

#endif
