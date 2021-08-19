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
	(*rmenu)->menu->win->height += 3;
	(*rmenu)->result = result;
}

void
rmenu_draw(RMenu *rmenu) {
	menu_draw(rmenu->menu);
	WINDOW *win = rmenu->menu->window;
	const double wpm = rmenu->result->wpm;
	const double time = rmenu->result->time;
	const time_t seed = rmenu->result->seed;
	// Draw wpm.
	mvwprintw(win, 1, 1, "WPM:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%3.0f", wpm);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw time taken.
	mvwprintw(win, 2, 1, "Time:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%3.0f", time);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw seed.
	mvwprintw(win, 3, 1, "Seed:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%u", seed);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw options.
	menu_draw_options(rmenu->menu, 4);
}

void
rmenu_free(RMenu *rmenu) {
	menu_free(rmenu->menu);
}

#endif
