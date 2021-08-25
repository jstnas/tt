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

void
rmenu_init(RMenu **rmenu, Result *result, char *title, char *options[]) {
	*rmenu = (RMenu *)malloc(sizeof(RMenu));
	menu_init(&(*rmenu)->menu, title, options);
	(*rmenu)->menu->win->width += 4;
	(*rmenu)->menu->win->height += 5;
	(*rmenu)->result = result;
}

void
rmenu_draw(RMenu *rmenu) {
	menu_draw(rmenu->menu);
	WINDOW *win = rmenu->menu->window;
	const unsigned length = rmenu->result->length;
	const double wpm = rmenu->result->wpm;
	const double time = rmenu->result->time;
	const time_t seed = rmenu->result->seed;
	// Draw mode.
	mvwprintw(win, 1, 1, "Mode:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
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
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw length.
	mvwprintw(win, 2, 1, "Length:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%d", length);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw wpm.
	mvwprintw(win, 3, 1, "WPM:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%3.0f", wpm);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw time taken.
	mvwprintw(win, 4, 1, "Time:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%3.0f", time);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw seed.
	mvwprintw(win, 5, 1, "Seed:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%u", seed);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw options.
	menu_draw_options(rmenu->menu, 6);
}

void
rmenu_free(RMenu *rmenu) {
	menu_free(rmenu->menu);
}

#endif
