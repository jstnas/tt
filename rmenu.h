#ifndef R_MENU_H
#define R_MENU_H

#include "menu.h"
#include "result.h"

// Results menu
typedef struct {
	Menu *menu;
	Result *result;
} RMenu;

void rmenu_init(RMenu **, Result *, char *, char *[]);
void rmenu_draw(RMenu *);
void rmenu_free(RMenu *);
void rmenu_draw_stat(WINDOW *, unsigned, char *, int);

void
rmenu_init(RMenu **rmenu, Result *result, char *title, char *options[]) {
	*rmenu = (RMenu *)malloc(sizeof(RMenu));
	menu_init(&(*rmenu)->menu, title, options);
	(*rmenu)->menu->win->width += 4;
	(*rmenu)->menu->win->height += 5;
	(*rmenu)->menu->option_offset += 5;
	(*rmenu)->result = result;
}

void
rmenu_draw(RMenu *rmenu) {
	menu_draw(rmenu->menu);
	WINDOW *win = rmenu->menu->window;
	unsigned row = 1;
	// Draw mode.
	mvwprintw(win, row++, 1, "Mode:");
	wattron(win, COLOR_PAIR(TEXT));
	switch (rmenu->result->mode) {
		case TIME:
			wprintw(win, "Time");
			break;
		case WORDS:
			wprintw(win, "Words");
			break;
		case QUOTE:
			wprintw(win, "Quote");
			break;
		case ZEN:
			wprintw(win, "Zen");
			break;
		case CUSTOM:
			wprintw(win, "Custom");
			break;
	}
	wattroff(win, COLOR_PAIR(TEXT));
	// Draw length.
	const int length = (int)rmenu->result->length;
	const int wpm = (int)rmenu->result->wpm;
	const int time = (int)rmenu->result->time;
	const int seed = (int)rmenu->result->seed;
	rmenu_draw_stat(win, row++, "Length:", length);
	rmenu_draw_stat(win, row++, "WPM:", wpm);
	rmenu_draw_stat(win, row++, "Time:", time);
	rmenu_draw_stat(win, row++, "Seed:", seed);
	// Draw options.
	menu_draw_options(rmenu->menu);
}

void
rmenu_free(RMenu *rmenu) {
	menu_free(rmenu->menu);
}

void rmenu_draw_stat(WINDOW *win, unsigned row, char *title, int value) {
	mvwprintw(win, row, 1, title);
	wattron(win, COLOR_PAIR(TEXT));
	wprintw(win, "%d", value);
	wattroff(win, COLOR_PAIR(TEXT));
}

#endif
