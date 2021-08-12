#ifndef TMENU_H
#define TMENU_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "twindow.h"
#include "tresult.h"

typedef struct {
	int current_option;
	size_t option_count;
	TWindow *twin;
	WINDOW *window;
	char *title;
	char **options;
} TMenu;

TMenu *tmenu_init(char *, char *[]);
void tmenu_destroy(TMenu *);
int tmenu_update(TMenu *);
void tmenu_draw(TMenu *);
void tmenu_draw_options(TMenu *, unsigned);

TMenu *
tmenu_init(char *title, char *options[]) {
	TMenu *menu = (TMenu *)malloc(sizeof(TMenu));
	menu->current_option = 0;
	// Work out the size of the window, and the amount of options.
	Vector2 size;
	// Initialise menu width to the width of the title.
	size.x = strlen(title);
	// Work out the option count and the longest option.
	unsigned option_count = 0;
	while (options[option_count] != NULL) {
		const size_t option_length = strlen(options[option_count]);
		if (option_length > size.x)
			size.x = option_length;
		option_count++;
	}
	size.x += 2;
	// Height is based on the amount of options.
	size.y = option_count + 2;
	menu->option_count = option_count;
	// Create a twindow.
	menu->twin = twindow_init(size);
	menu->window = menu->twin->window;
	keypad(menu->window,  TRUE);
	menu->title = title;
	menu->options = options;
	return menu;
}

void
tmenu_destroy(TMenu *menu) {
	twindow_destroy(menu->twin);
	free(menu);
}

int
tmenu_update(TMenu *menu) {
	const int input = wgetch(menu->window);
	switch (input) {
		case TKEY_RESIZE:
			menu->twin->resize = true;
			return -1;
		// Go back to typing.
		case TKEY_MENU:
			return -2;
		// Return the selected option.
		case TKEY_SUBMIT:
			return menu->current_option;
		// Cycle options.
		case TKEY_UP:
			if (menu->current_option == 0)
				menu->current_option = menu->option_count - 1;
			else
				menu->current_option--;
			break;
		case TKEY_DOWN:
			if (menu->current_option == menu->option_count - 1)
				menu->current_option = 0;
			else
				menu->current_option++;
			break;
	}
	return -1;
}

void
tmenu_draw(TMenu *menu) {
	twindow_resize(menu->twin);
	wclear(menu->window);
	// Draw a box around the menu.
	box(menu->window, 0, 0);
	// Draw the title.
	wattron(menu->window, COLOR_PAIR(4));
	mvwprintw(menu->window, 0, 1, menu->title);
	wattroff(menu->window, COLOR_PAIR(4));
	// Refresh the screen.
	wrefresh(menu->window);
	curs_set(0);
}

void tmenu_draw_options(TMenu *menu, unsigned offset) {
	wattron(menu->window, COLOR_PAIR(2));
	for (size_t o = 0; o < menu->option_count; o++) {
		wmove(menu->window, o + offset, 1);
		if (menu->current_option == o) {
			wattron(menu->window, A_REVERSE);
			wprintw(menu->window, menu->options[o]);
			wattroff(menu->window, A_REVERSE);
		}
		else
			wprintw(menu->window, menu->options[o]);
	}
	wattroff(menu->window, COLOR_PAIR(2));
}

#endif
