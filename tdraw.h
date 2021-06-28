#ifndef TDRAW_H
#define TDRAW_H

#include <ncurses.h>
#include "vector2.h"
#include "tmath.h"

void tdraw_reposition(WINDOW *window, Vector2 *screen_size, Vector2 size);
Vector2 get_window_size(WINDOW *window);

void tdraw_reposition(WINDOW *window, Vector2 *screen_size, Vector2 size) {
	const Vector2 new_size = {
		min(size.x, screen_size->x),
		min(size.y, screen_size->y)
	};
	wresize(window, new_size.y, new_size.x);
	const Vector2 position = {
		(screen_size->x - new_size.x) / 2,
		(screen_size->y - new_size.y) / 2
	};
	mvwin(window, position.y, position.x);
	wclear(window);
}

Vector2 get_window_size(WINDOW *window) {
	Vector2 window_size;
	getmaxyx(window, window_size.y, window_size.x);
	return window_size;
}

#endif
