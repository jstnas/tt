#ifndef TMENU_H
#define TMENU_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tdraw.h"
#include "tresult.h"

typedef struct {
	WINDOW *window;
	char *title;
	char **options;
	bool resize;
	TResult *result;
	Vector2 size;
	size_t option_count;
	int current_option;
} TMenu;

TMenu *tmenu_init(char *, char *[]);
TMenu *tmenu_result_init(char *, char *[], TResult *);
void tmenu_destroy(TMenu *);
int tmenu_update(TMenu *);
void tmenu_draw(TMenu *);
void tmenu_draw_options(TMenu *, const unsigned);
void tmenu_draw_result(TMenu *);

TMenu *tmenu_init(char *title, char *options[]) {
	TMenu *menu = (TMenu *)malloc(sizeof(TMenu));
	menu->window = newwin(0, 0, 0, 0);
	wbkgd(menu->window, COLOR_PAIR(1));
	menu->title = title;
	menu->options = options;
	menu->resize = true;
	menu->result = NULL;
	// Initialise menu width to the width of the title.
	menu->size.x = strlen(title);
	// Work out the option count and the longest option.
	menu->option_count = 0;
	while (options[menu->option_count] != NULL) {
		const size_t option_length = strlen(options[menu->option_count]);
		if (option_length > menu->size.x)
			menu->size.x = option_length;
		menu->option_count++;
	}
	menu->size.x += 2;
	// Height is based on the amount of options.
	menu->size.y = menu->option_count + 2;
	keypad(menu->window,  TRUE);
	return menu;
}

TMenu *tmenu_result_init(char *title, char *options[], TResult *result) {
	TMenu *menu = tmenu_init(title, options);
	menu->result = result;
	if (menu->size.x < 9)
		menu->size.x = 9;
	menu->size.y += 2;
	return menu;
}

void tmenu_destroy(TMenu *menu) {
	delwin(menu->window);
	free(menu);
}

int tmenu_update(TMenu *menu) {
	const int input = wgetch(menu->window);
	switch (input) {
		case TKEY_RESIZE:
			menu->resize = true;
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

void tmenu_draw(TMenu *menu) {
	if (menu->resize) {
		menu->resize = false;
		tdraw_reposition(menu->window, menu->size);
	}
	wclear(menu->window);
	// Draw a box around the menu.
	box(menu->window, 0, 0);
	// Draw the title.
	wattron(menu->window, COLOR_PAIR(4));
	mvwprintw(menu->window, 0, 1, menu->title);
	wattroff(menu->window, COLOR_PAIR(4));
	wattron(menu->window, COLOR_PAIR(2));
	// Draw the options.
	if (menu->result == NULL)
		tmenu_draw_options(menu, 1);
	else {
		tmenu_draw_result(menu);
		tmenu_draw_options(menu, 3);
	}
	wattroff(menu->window, COLOR_PAIR(2));
	wrefresh(menu->window);
	curs_set(0);
}

void tmenu_draw_options(TMenu *menu, const unsigned offset) {
	for (size_t o = 0; o < menu->option_count; o++) {
		const int y_pos = offset + o;
		wmove(menu->window, y_pos, 1);
		if (menu->current_option == o) {
			wattron(menu->window, A_REVERSE);
			wprintw(menu->window, menu->options[o]);
			wattroff(menu->window, A_REVERSE);
		}
		else
			wprintw(menu->window, menu->options[o]);
	}
}

void tmenu_draw_result(TMenu *menu) {
	mvwprintw(menu->window, 1, 1,
			"WPM: %3.0f", menu->result->wpm);
	mvwprintw(menu->window, 2, 1,
			"time: %3.0f", menu->result->time_taken);
}

#endif
