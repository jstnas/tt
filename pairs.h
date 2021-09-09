#ifndef PAIRS_H
#define PAIRS_H

#include "config.h"

typedef enum {
	SUB = 1,
	TEXT,
	ERROR,
	ACCENT
} Pair;

void init_pairs();

void init_pairs() {
	init_pair(SUB, COLOR_SUB, COLOR_BACKGROUND);
	init_pair(TEXT, COLOR_TEXT, COLOR_BACKGROUND);
	init_pair(ERROR, COLOR_ERROR, COLOR_BACKGROUND);
	init_pair(ACCENT, COLOR_ACCENT, COLOR_BACKGROUND);
};

#endif
