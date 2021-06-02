#ifndef WINDOW_H
#define WINDOW_H

typedef struct Window Window;

struct Window {
	void *window;
	void (*update)(void*);
	void (*draw)(void*);
	void (*destroy)(void*);
};

#endif
