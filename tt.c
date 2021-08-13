#include <time.h>
#include "config.h"
#include "menu.h"
#include "ttest.h"
#include "tresult.h"

TResult *test_result;
// TODO: resize the window when switching windows.
size_t current_window = 0;
bool running = true;
// Windows.
Test *test_window;
Menu *main_menu;
Menu *results_menu;

void t_update(Test *);
void m_update(Menu *);

int main() {
	// Init.
	setenv("ESCDELAY", "0", 0);
	initscr();
	start_color();
	use_default_colors();
	noecho();
	raw();
	cbreak();
	init_pair(1, COLOR_SUB, COLOR_BACKGROUND);
	init_pair(2, COLOR_TEXT, COLOR_BACKGROUND);
	init_pair(3, COLOR_ERROR, COLOR_BACKGROUND);
	init_pair(4, COLOR_ACCENT, COLOR_BACKGROUND);
	wbkgd(stdscr, COLOR_PAIR(1));
	test_result = (TResult *)malloc(sizeof(TResult));
	test_result->wpm = 0;
	test_result->time_taken = 0;
	// Create windows.
	char *menu_options[] = {"Next test", "Repeat test", "Exit", NULL};
	test_init(&test_window, time(NULL), test_result);
	menu_init(&main_menu, "Menu", menu_options);
//	results_menu = menu_result_init("Results", menu_options, test_result);
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
				test_draw(test_window);
				t_update(test_window);
				break;
			// Main menu.
			case 1:
				menu_draw(main_menu);
				menu_draw_options(main_menu, 1);
				m_update(main_menu);
				break;
			// Results menu.
			case 2:
				menu_draw(results_menu);
				m_update(results_menu);
				break;
		}
	}
	// Cleanup.
	endwin();
	test_free(test_window);
	menu_free(main_menu);
	menu_free(results_menu);
	return 0;
}

void t_update(Test *test) {
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
void m_update(Menu *menu) {
	const int result = menu_update(menu);
	switch (result) {
		// Next test.
		case 0:
			test_free(test_window);
			test_init(&test_window, time(NULL), test_result);
			current_window = 0;
			break;
		// Repeat test.
		case 1:
			const size_t seed = test_window->seed;
			test_free(test_window);
			test_init(&test_window, time(NULL), test_result);
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
