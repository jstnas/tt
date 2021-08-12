#ifndef TWINDOW_H
#define TWINDOW_H

#include <ncurses.h>
#include "vector2.h"

typedef struct {
	WINDOW *window;
	Vector2 size;
	bool resize;
} TWindow;

unsigned min(unsigned, unsigned);
void get_window_size(Vector2 *, WINDOW *);
TWindow *twindow_init(Vector2);
void twindow_destroy(TWindow *);
void twindow_resize(TWindow *);

unsigned
min(unsigned a, unsigned b) {
	if (a < b)
		return a;
	return b;
}

void
get_window_size(Vector2 *size, WINDOW *window) {
	getmaxyx(window, size->y, size->x);
}

// Initialise a TWindow struct.
TWindow *
twindow_init(Vector2 size) {
	TWindow *twin = (TWindow *)malloc(sizeof(TWindow));
	// Create and setup the window.
	twin->window = newwin(0, 0, 0, 0);
	wbkgd(twin->window, COLOR_PAIR(1));
	// Assign the rest.
	twin->size = size;
	twin->resize = true;
	return twin;
}

void twindow_destroy(TWindow *twin) {
	delwin(twin->window);
	free(twin);
}

// Repositions a TWindow if needed.
void
twindow_resize(TWindow *twin) {
	// Only resize if needed.
	if (!twin->resize)
		return;
	twin->resize = false;
	// Work out the screen size.
	Vector2 screen_size;
	get_window_size(&screen_size, stdscr);
	// Make sure window isn't bigger than the screen.
	const Vector2 new_size = {
		min(twin->size.x, screen_size.x),
		min(twin->size.y, screen_size.y),
	};
	wresize(twin->window, new_size.y, new_size.x);
	const Vector2 position = {
		(screen_size.x - new_size.x) / 2,
		(screen_size.y - new_size.y) / 2
	};
	mvwin(twin->window, position.y, position.x);
}

#endif
