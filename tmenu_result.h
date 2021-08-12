#ifndef TMENU_RESULT_H
#define TMENU_RESULT_H

#include "tmenu.h"
#include "tresult.h"

typedef struct {
	TMenu *menu;
	TResult *result;
} TMenuResult;

TMenuResult *tmenu_result_init(char *[]);
void tmenu_result_destroy(TMenuResult *);

#endif
