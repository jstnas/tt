#ifndef RESULT_H
#define RESULT_H

typedef struct {
	float wpm, time_taken;
} Result;

void result_init(Result **, float, float);
void result_free(Result *);

void
result_init(Result **result, float wpm, float time_taken) {
	*result = (Result *)malloc(sizeof(Result));
	(*result)->wpm = wpm;
	(*result)->time_taken = time_taken;
}

void
result_free(Result *result) {
	free(result);
}

#endif
