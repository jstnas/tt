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
	(*rmenu)->menu->win->width += 3;
	(*rmenu)->menu->win->height += 2;
	(*rmenu)->result = result;
}

void
rmenu_draw(RMenu *rmenu) {
	menu_draw(rmenu->menu);
	WINDOW *win = rmenu->menu->window;
	const float wpm = rmenu->result->wpm;
	const float time_taken = rmenu->result->time_taken;
	// Draw wpm.
	mvwprintw(win, 1, 1, "WPM:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%3.0f", wpm);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw time taken.
	mvwprintw(win, 2, 1, "Time Taken:");
	wattron(win, COLOR_PAIR(PAIR_TEXT));
	wprintw(win, "%3.0f", time_taken);
	wattroff(win, COLOR_PAIR(PAIR_TEXT));
	// Draw options.
	menu_draw_options(rmenu->menu, 3);
}

void
rmenu_free(RMenu *rmenu) {
	menu_free(rmenu->menu);
}

#endif
