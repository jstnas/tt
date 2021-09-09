#include <time.h>
#include "config.h"
#include "menu.h"
#include "rmenu.h"
#include "test.h"
#include "result.h"

typedef enum {
	WINDOW_TEST,
	WINDOW_MENU,
	WINDOW_RESULT,
	WINDOW_MODE,
} current_window;

current_window cur_win = WINDOW_TEST;
bool running = true;
Result *tresult;
// Windows.
Test *test;
Menu *main_menu;
Menu *mode_menu;
RMenu *results_menu;

void t_update(Test *);
void m_update(Menu *);
void mode_update(Menu *);
void switch_window(current_window);

void
t_update(Test *test) {
	const int result = test_update(test);
	switch (result) {
		// Completed the test.
		case 0:
			switch_window(WINDOW_RESULT);
			break;
		// Escape key pressed.
		case -2:
			switch_window(WINDOW_MODE);
			break;
	}
}

// Menu window functions.
void
m_update(Menu *menu) {
	const int result = menu_update(menu);
	const Mode mode = tresult->mode;
	const unsigned length = tresult->length;
	switch (result) {
		// Next test.
		case 0:
			result_new(tresult, mode, length, time(NULL));
			test_reset(test);
			switch_window(WINDOW_TEST);
			break;
		// Repeat test.
		case 1:
			const size_t seed = tresult->seed;
			result_new(tresult, mode, length, seed);
			test_reset(test);
			switch_window(WINDOW_TEST);
			break;
		case 2:
			switch_window(WINDOW_MODE);
			break;
		// Exit.
		case 3:
			running = false;
			break;
		// Escape key pressed.
		case -2:
			switch_window(WINDOW_TEST);
			break;
	}
}

// Mode window functions.
void mode_update(Menu *menu) {
	const int result = menu_update(menu);
	switch (result) {
		case -1:
			break;
		case -2:
		case 5:
			switch_window(WINDOW_MENU);
			break;
		default:
			tresult->mode = result;
			switch_window(WINDOW_MENU);
	}
}

void
switch_window(current_window new_win) {
	cur_win = new_win;
	curs_set(0);
	switch(cur_win) {
		case WINDOW_TEST:
			test->win->resize = true;
			curs_set(1);
			break;
		case WINDOW_MENU:
			main_menu->win->resize = true;
			break;
		case WINDOW_RESULT:
			results_menu->menu->win->resize = true;
			break;
	}
}

int
main() {
	// Init.
	setenv("ESCDELAY", "0", 0);
	initscr();
	start_color();
	use_default_colors();
	noecho();
	raw();
	cbreak();
	init_pair(PAIR_SUB, COLOR_SUB, COLOR_BACKGROUND);
	init_pair(PAIR_TEXT, COLOR_TEXT, COLOR_BACKGROUND);
	init_pair(PAIR_ERROR, COLOR_ERROR, COLOR_BACKGROUND);
	init_pair(PAIR_ACCENT, COLOR_ACCENT, COLOR_BACKGROUND);
	wbkgd(stdscr, COLOR_PAIR(PAIR_SUB));
	result_init(&tresult, QUOTE, 30, time(NULL));
	// Create windows.
	char *menu_options[] = {"Next test", "Repeat test", "Mode", "Exit", NULL};
	menu_init(&main_menu, "Menu", menu_options);
	char *mode_options[] = {"Time", "Words", "Quote", "Zen", "Custom", "Back", NULL};
	menu_init(&mode_menu, "Mode", mode_options);
	rmenu_init(&results_menu, tresult, "Results", menu_options);
	test_init(&test, tresult);
	// Main loop.
	while (running) {
		// Draw.
		clear();
		mvprintw(0, 0, "tt");
		refresh();
		// Draw and update current window.
		switch (cur_win) {
			case WINDOW_TEST:
				test_draw(test);
				t_update(test);
				break;
			case WINDOW_MENU:
				menu_draw(main_menu);
				menu_draw_options(main_menu);
				m_update(main_menu);
				break;
			case WINDOW_RESULT:
				rmenu_draw(results_menu);
				m_update(results_menu->menu);
				break;
			case WINDOW_MODE:
				menu_draw(mode_menu);
				menu_draw_options(mode_menu);
				mode_update(mode_menu);
				break;
		}
	}
	// Cleanup.
	endwin();
	result_free(tresult);
	test_free(test);
	menu_free(main_menu);
	rmenu_free(results_menu);
	return 0;
}
