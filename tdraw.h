#ifndef TDRAW_H
#define TDRAW_H

#define MIN(a, b)		((a) < (b) ? (a) : (b))

#include <ncurses.h>
#include "vector2.h"

void tdraw_reposition(WINDOW *, Vector2);
Vector2 get_window_size(WINDOW *);

void tdraw_reposition(WINDOW *window, Vector2 size) {
	const Vector2 screen_size = get_window_size(stdscr);
	const Vector2 new_size = {
		MIN(size.x, screen_size.x),
		MIN(size.y, screen_size.y)
	};
	wresize(window, new_size.y, new_size.x);
	const Vector2 position = {
		(screen_size.x - new_size.x) / 2,
		(screen_size.y - new_size.y) / 2
	};
	mvwin(window, position.y, position.x);
}

Vector2 get_window_size(WINDOW *window) {
	Vector2 window_size;
	getmaxyx(window, window_size.y, window_size.x);
	return window_size;
}

#endif
