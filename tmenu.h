#ifndef TMENU_H
#define TMENU_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tmath.h"

typedef struct TMenu TMenu;
struct TMenu {
	WINDOW *window;
	char *title;
	char **options;
	Vector2 *screen_size;
	Vector2 padding;
	Vector2 size;
	size_t option_count;
	int current_option;
};

TMenu *tmenu_init(char *title, char *options[], Vector2 *screen_size);
void tmenu_destroy(TMenu *menu);
int tmenu_update(TMenu *menu);
void tmenu_draw(TMenu *menu);

TMenu *tmenu_init(char *title, char *options[], Vector2 *screen_size) {
	TMenu *menu = (TMenu *)malloc(sizeof(TMenu));
	menu->title = title;
	menu->options = options;
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
	// Update menu size and position.
	const Vector2 size = {
		.x = min(menu->size.x, menu->screen_size->x),
		.y = min(menu->size.y, menu->screen_size->y)
	};
	const Vector2 position = {
		.x = (menu->screen_size->x - size.x) / 2,
		.y = (menu->screen_size->y - size.y) / 2
	};
	wresize(menu->window, size.y, size.x);
	mvwin(menu->window, position.y, position.x);
	// Draw a box around the menu.
	box(menu->window, 0, 0);
	// Draw the title.
	wattron(menu->window, COLOR_PAIR(4));
	mvwprintw(menu->window, 0, 1, menu->title);
	wattroff(menu->window, COLOR_PAIR(4));
	// Draw the options.
	for (size_t o = 0; o < menu->option_count; o++) {
		const int y_pos = menu->padding.y + o;
		wmove(menu->window, y_pos, menu->padding.x);
		if (menu->current_option == o) {
			wattron(menu->window, A_REVERSE);
			wprintw(menu->window, menu->options[o]);
			wattroff(menu->window, A_REVERSE);
		}
		else
			wprintw(menu->window, menu->options[o]);
	}
	wrefresh(menu->window);
	curs_set(0);
}

#endif
