#ifndef QUOTE_H
#define QUOTE_H

typedef enum {
	SHORT,
	MEDIUM,
	LONG,
	THICC,
} QuoteLength;

typedef struct {
	char *text;
	char *source;
	unsigned length;
} Quote;

#endif
