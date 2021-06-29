#ifndef TRESULT_H
#define TRESULT_H

#include "ttime.h"

typedef struct TResult TResult;
struct TResult {
	float wpm;
	float time_taken;
};

TResult tresult_init(float, float);

TResult tresult_init(float wpm, float time_taken) {
	TResult result = {wpm, time_taken};
	return result;
}

#endif
