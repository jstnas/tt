#ifndef TRESULT_H
#define TRESULT_H

#include <time.h>

typedef struct TResult TResult;
struct TResult {
	unsigned wpm;
	time_t time_taken;
};

TResult tresult_init(unsigned wpm, time_t time_taken);
char **tresult_content(TResult result);

TResult tresult_init(unsigned wpm, time_t time_taken) {
	TResult result = {wpm, time_taken};
	return result;
}

#endif
