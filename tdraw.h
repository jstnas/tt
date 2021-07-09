#ifndef TDRAW_H
#define TDRAW_H

#define MIN(a, b)		((a) < (b) ? (a) : (b))

#include <ncurses.h>
#include "vector2.h"

void tdraw_reposition(WINDOW *, Vector2);
void get_window_size(Vector2 *, WINDOW *);

void tdraw_reposition(WINDOW *window, Vector2 size) {
	Vector2 screen_size;
	get_window_size(&screen_size, window);
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

void get_window_size(Vector2 *size, WINDOW *window) {
	getmaxyx(window, size->y, size->x);
}

#endif
