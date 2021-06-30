#ifndef TMATH_H
#define TMATH_H

unsigned min(unsigned, unsigned);
unsigned max(unsigned, unsigned);

unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

unsigned max(unsigned a, unsigned b) {
	return a > b ? a : b;
}

#endif
