#ifndef WINDOW_H
#define WINDOW_H

#include <ncurses.h>

typedef struct {
	WINDOW *window;
	unsigned width, height;
	bool resize;
} Window;

unsigned min(unsigned, unsigned);
void window_init(Window **, unsigned, unsigned);
void window_free(Window *);
void window_resize(Window *);

unsigned
min(unsigned a, unsigned b) {
	if (a < b)
		return a;
	return b;
}

// Initialise a Window struct.
void
window_init(Window **win, unsigned width, unsigned height) {
	*win = (Window *)malloc(sizeof(Window));
	// Create and setup the window.
	(*win)->window = newwin(0, 0, 0, 0);
	wbkgd((*win)->window, COLOR_PAIR(1));
	// Assign the rest.
	(*win)->width = width;
	(*win)->height = height;
	(*win)->resize = true;
}

void window_free(Window *win) {
	delwin(win->window);
	free(win);
}

// Repositions a Window if needed.
void
window_resize(Window *win) {
	// Only resize if needed.
	if (!win->resize)
		return;
	win->resize = false;
	// Work out the screen size.
	unsigned screen_width, screen_height;
	getmaxyx(stdscr, screen_height, screen_width);
	// Make sure window isn't bigger than the screen.
	const unsigned new_width = min(win->width, screen_width);
	const unsigned new_height = min(win->height, screen_height);
	wresize(win->window, new_height, new_width);
	const unsigned x_pos = (screen_width - new_width) / 2;
	const unsigned y_pos = (screen_height - new_height) / 2;
	mvwin(win->window, y_pos, x_pos);
}

#endif
