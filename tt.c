#include <time.h>
#include "config.h"
#include "menu.h"
#include "rmenu.h"
#include "test.h"
#include "result.h"

Result *tresult;
// TODO: resize the window when switching windows.
size_t current_window = 0;
bool running = true;
// Windows.
Test *test;
Menu *main_menu;
RMenu *results_menu;

void t_update(Test *);
void m_update(Menu *);

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
	result_init(&tresult, 0, 0);
	// Create windows.
	char *menu_options[] = {"Next test", "Repeat test", "Exit", NULL};
	test_init(&test, time(NULL), tresult);
	menu_init(&main_menu, "Menu", menu_options);
	rmenu_init(&results_menu, tresult, "Results", menu_options);
	// Main loop.
	while (running) {
		// Draw.
		clear();
		mvprintw(0, 0, "tt");
		refresh();
		// Draw and update current window.
		switch (current_window) {
			// Test window.
			case 0:
				test_draw(test);
				t_update(test);
				break;
			// Main menu.
			case 1:
				menu_draw(main_menu);
				menu_draw_options(main_menu, 1);
				m_update(main_menu);
				break;
			// Results menu.
			case 2:
				rmenu_draw(results_menu);
				m_update(results_menu->menu);
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

void
t_update(Test *test) {
	const int result = test_update(test);
	switch (result) {
		// Completed the test.
		case 0:
			current_window = 2;
			break;
		// Escape key pressed.
		case -2:
			current_window = 1;
			break;
	}
}

// Menu window functions.
void
m_update(Menu *menu) {
	const int result = menu_update(menu);
	switch (result) {
		// Next test.
		case 0:
			test_free(test);
			test_init(&test, time(NULL), tresult);
			current_window = 0;
			break;
		// Repeat test.
		case 1:
			const size_t seed = test->seed;
			test_free(test);
			test_init(&test, time(NULL), tresult);
			current_window = 0;
			break;
		// Exit.
		case 2:
			running = false;
			break;
		// Escape key pressed.
		case -2:
			current_window = 0;
			break;
	}
}
