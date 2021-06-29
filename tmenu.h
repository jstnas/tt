#ifndef TMENU_H
#define TMENU_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tmath.h"
#include "tdraw.h"
#include "tresult.h"

typedef struct TMenu TMenu;
struct TMenu {
	WINDOW *window;
	char *title;
	char **options;
	TResult *result;
	Vector2 *screen_size;
	Vector2 padding;
	Vector2 size;
	size_t option_count;
	int current_option;
};

TMenu *tmenu_init(char *, char *[], Vector2 *);
TMenu *tmenu_result_init(char *, char *[], Vector2 *, TResult *);
void tmenu_destroy(TMenu *);
int tmenu_update(TMenu *);
void tmenu_draw(TMenu *);
void tmenu_draw_options(TMenu *, unsigned);
void tmenu_draw_result(TMenu *);

TMenu *tmenu_init(char *title, char *options[], Vector2 *screen_size) {
	TMenu *menu = (TMenu *)malloc(sizeof(TMenu));
	menu->title = title;
	menu->options = options;
	menu->result = NULL;
	menu->padding = vector2_init(menu_padding_x, menu_padding_y);
	menu->screen_size = screen_size;
	// Initialise width to the width of the tile.
	menu->size.x = strlen(title) + (menu->padding.x * 2);
	// Work out the option count.
	menu->option_count = 0;
	while (options[menu->option_count] != NULL) {
		// Get the longest option.
		const size_t option_length = strlen(options[menu->option_count]) + (menu->padding.x * 2);
		if (option_length > menu->size.x)
			menu->size.x = option_length;
		menu->option_count++;
	}
	// Height is based on the amount of options.
	menu->size.y = menu->option_count + (menu->padding.y * 2);
	// Create the window.
	menu->window = newwin(0, 0, 0, 0);
	keypad(menu->window,  TRUE);
	return menu;
}

TMenu *tmenu_result_init(char *title, char *options[], Vector2 *screen_size, TResult *result) {
	TMenu *menu = tmenu_init(title, options, screen_size);
	menu->result = result;
	menu->size.x = max(menu->size.x, 9);
	menu->size.y += 2;
	return menu;
}

void tmenu_destroy(TMenu *menu) {
	delwin(menu->window);
	free(menu);
}

int tmenu_update(TMenu *menu) {
	int input = wgetch(menu->window);
	// Cycle options.
	if (input == key_up) {
		if (menu->current_option == 0)
			menu->current_option = menu->option_count - 1;
		else
			menu->current_option--;
	}
	else if (input == key_down) {
		if (menu->current_option == menu->option_count - 1)
			menu->current_option = 0;
		else
			menu->current_option++;
	}
	// Return the selected option.
	else if (input == key_submit)
		return menu->current_option;
	// Go back to typing.
	else if (input == key_menu)
		return -2;
	return -1;
}

void tmenu_draw(TMenu *menu) {
	tdraw_reposition(menu->window, menu->screen_size, menu->size);
	// Draw a box around the menu.
	box(menu->window, 0, 0);
	// Draw the title.
	wattron(menu->window, COLOR_PAIR(4));
	mvwprintw(menu->window, 0, 1, menu->title);
	wattroff(menu->window, COLOR_PAIR(4));
	// Draw the options.
	if (menu->result == NULL)
		tmenu_draw_options(menu, 0);
	else {
		tmenu_draw_result(menu);
		tmenu_draw_options(menu, 2);
	}
	wrefresh(menu->window);
	curs_set(0);
}

void tmenu_draw_options(TMenu *menu, unsigned offset) {
	for (size_t o = 0; o < menu->option_count; o++) {
		const int y_pos = menu->padding.y + offset + o;
		wmove(menu->window, y_pos, menu->padding.x);
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
	mvwprintw(menu->window, menu->padding.y, menu->padding.x,
			"WPM: %3.0f", menu->result->wpm);
	mvwprintw(menu->window, menu->padding.y + 1, menu->padding.x,
			"time: %3.0f", menu->result->time_taken);
}

#endif
