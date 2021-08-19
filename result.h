#ifndef RESULT_H
#define RESULT_H

typedef struct {
	double wpm, time;
	time_t seed;
} Result;

void result_init(Result **, time_t);
void result_free(Result *);
void result_new(Result *, time_t);

void
result_init(Result **result, time_t seed) {
	*result = (Result *)malloc(sizeof(Result));
	result_new(*result, seed);
}

void
result_free(Result *result) {
	free(result);
}

void
result_new(Result *result, time_t seed) {
	result->wpm = 0;
	result->time = 0;
	result->seed = seed;
}

#endif
