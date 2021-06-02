#ifndef WINDOW_H
#define WINDOW_H

typedef struct Window Window;

struct Window {
	void *window;
	void (*update)(void*);
	void (*draw)(void*);
	void (*destroy)(void*);
};

Window *window_init(void *win, void (*update)(void*), void (*draw)(void*),
		void (*destroy)(void*)) {
	Window *window = (Window *)malloc(sizeof(Window));
	window->window = win;
	window->update = update;
	window->draw = draw;
	window->destroy = destroy;
	return window;
}

#endif
