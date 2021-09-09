#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "window.h"

typedef struct {
	short current_option;
	short option_offset;
	size_t option_count;
	Window *win;
	WINDOW *window;
	char *title;
	char **options;
} Menu;

void menu_init(Menu **, char *, char *[]);
void menu_free(Menu *);
int menu_update(Menu *);
void menu_draw(Menu *);
void menu_draw_options(Menu *);

void
menu_init(Menu **menu, char *title, char *options[]) {
	*menu = (Menu *)malloc(sizeof(Menu));
	(*menu)->current_option = 0;
	(*menu)->option_offset = 1;
	// Work out the size of the window, and the amount of options.
	// Initialise menu width to the width of the title.
	unsigned width = strlen(title);
	// Work out the option count and the longest option.
	unsigned option_count = 0;
	while (options[option_count] != NULL) {
		const size_t option_length = strlen(options[option_count]);
		if (option_length > width)
			width = option_length;
		option_count++;
	}
	width += 2;
	// Height is based on the amount of options.
	const unsigned height = option_count + 2;
	(*menu)->option_count = option_count;
	// Create a window.
	window_init(&(*menu)->win, width, height);
	(*menu)->window = (*menu)->win->window;
	keypad((*menu)->window,  TRUE);
	(*menu)->title = title;
	(*menu)->options = options;
}

void
menu_free(Menu *menu) {
	window_free(menu->win);
	free(menu);
}

int
menu_update(Menu *menu) {
	const int input = wgetch(menu->window);
	switch (input) {
		case KEY_RESIZE:
			menu->win->resize = true;
			return -1;
		// Go back to typing.
		case TKEY_MENU:
			return -2;
		// Return the selected option.
		case TKEY_SUBMIT:
			const int result = menu->current_option;
			menu->current_option = 0;
			return result;
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
menu_draw(Menu *menu) {
	window_resize(menu->win);
	wclear(menu->window);
	// Draw a box around the menu.
	box(menu->window, 0, 0);
	// Draw the title.
	wattron(menu->window, COLOR_PAIR(ACCENT));
	mvwprintw(menu->window, 0, 1, menu->title);
	wattroff(menu->window, COLOR_PAIR(ACCENT));
	// Refresh the screen.
	wrefresh(menu->window);
}

void
menu_draw_options(Menu *menu) {
	wattron(menu->window, COLOR_PAIR(TEXT));
	for (size_t o = 0; o < menu->option_count; o++) {
		wmove(menu->window, o + menu->option_offset, 1);
		if (menu->current_option == o) {
			wattron(menu->window, A_REVERSE);
			wprintw(menu->window, menu->options[o]);
			wattroff(menu->window, A_REVERSE);
		}
		else
			wprintw(menu->window, menu->options[o]);
	}
	wattroff(menu->window, COLOR_PAIR(TEXT));
}

#endif
