#ifndef TMATH_H
#define TMATH_H

unsigned min(unsigned a, unsigned b);
unsigned max(unsigned a, unsigned b);

unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

unsigned max(unsigned a, unsigned b) {
	return a > b ? a : b;
}

#endif
