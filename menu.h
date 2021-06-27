#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tmath.h"

typedef struct Menu Menu;
struct Menu {
	WINDOW *window;
	char *title;
	char **content;
	char **options;
	Vector2 *screen_size;
	Vector2 padding;
	Vector2 size;
	size_t content_count;
	size_t option_count;
	int current_option;
};

Menu *menu_init(char *title, char *content[], char *options[], Vector2 padding,
		Vector2 *screen_size);
void menu_destroy(Menu *menu);
int menu_update(Menu *menu);
void menu_draw(Menu *menu);

Menu *menu_init(char *title, char *content[], char *options[], Vector2 padding,
		Vector2 *screen_size) {
	Menu *menu = (Menu *)malloc(sizeof(Menu));
	menu->title = title;
	menu->content = content;
	menu->options = options;
	menu->padding = padding;
	menu->screen_size = screen_size;
	// Initialise width to the width of the tile.
	menu->size.x = strlen(title) + (padding.x * 2);
	// Work out the content count.
	menu->content_count = 0;
	if (content != NULL) {
		while (content[menu->content_count] != NULL) {
			// Get the longest content.
			const size_t content_length = strlen(content[menu->content_count])+ (padding.x * 2);
			if (content_length > menu->size.x)
				menu->size.x = content_length;
			menu->content_count++;
		}
	}
	// Work out the option count.
	menu->option_count = 0;
	while (options[menu->option_count] != NULL) {
		// Get the longest option.
		const size_t option_length = strlen(options[menu->option_count]) + (padding.x * 2);
		if (option_length > menu->size.x)
			menu->size.x = option_length;
		menu->option_count++;
	}
	// Height is based on the amount of options.
	menu->size.y = menu->content_count + menu->option_count + (padding.y * 2);
	// Create the window.
	menu->window = newwin(0, 0, 0, 0);
	keypad(menu->window,  TRUE);
	return menu;
}

void menu_destroy(Menu *menu) {
	delwin(menu->window);
	free(menu);
}

int menu_update(Menu *menu) {
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

void menu_draw(Menu *menu) {
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
	// Draw the content.
	if (menu->content != NULL) {
		for (size_t c = 0; c < menu->content_count; c++) {
			mvwprintw(menu->window, c + menu->padding.y, menu->padding.x,
					menu->content[c]);
		}
	}
	// Draw the options.
	for (size_t o = 0; o < menu->option_count; o++) {
		const int y_pos = menu->padding.y + menu->content_count + o;
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
}
