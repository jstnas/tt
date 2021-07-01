#include <time.h>
#include "config.h"
#include "tmenu.h"
#include "twindow.h"
#include "tdraw.h"
#include "tresult.h"

static TResult *test_result;
// TODO: resize the window when switching windows.
static size_t current_window = 0;
static bool running = true;
// Windows.
static TWindow *test_window;
static TMenu *main_menu;
static TMenu *results_menu;

void m_update(TMenu *);

int main() {
	// Init.
	setenv("ESCDELAY", escape_delay, 0);
	initscr();
	start_color();
	use_default_colors();
	noecho();
	raw();
	cbreak();
	init_pair(1, color_foreground, color_background);
	init_pair(2, color_correct, color_background);
	init_pair(3, color_incorrect, color_background);
	init_pair(4, color_accent, color_background);
	test_result = (TResult *)malloc(sizeof(TResult));
	*test_result = tresult_init(0, 0);
	// Create windows.
	char *menu_options[] = {"Next test", "Repeat test", "Exit", NULL};
	test_window = twindow_init(time(NULL), test_result);
	main_menu = tmenu_init("Menu", menu_options);
	results_menu = tmenu_result_init("Results", menu_options, test_result);
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
				twindow_draw(test_window);
				const int result = twindow_update(test_window);
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
				break;
			// Main menu.
			case 1:
				tmenu_draw(main_menu);
				m_update(main_menu);
				break;
			// Results menu.
			case 2:
				tmenu_draw(results_menu);
				m_update(results_menu);
				break;
		}
	}
	// Cleanup.
	endwin();
	twindow_destroy(test_window);
	tmenu_destroy(main_menu);
	tmenu_destroy(results_menu);
	return 0;
}

// Menu window functions.
void m_update(TMenu *menu) {
	const int result = tmenu_update(menu);
	switch (result) {
		// Next test.
		case 0:
			twindow_destroy(test_window);
			test_window = twindow_init(time(NULL), test_result);
			current_window = 0;
			break;
		// Repeat test.
		case 1:
			const size_t seed = test_window->seed;
			twindow_destroy(test_window);
			test_window = twindow_init(seed, test_result);
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
