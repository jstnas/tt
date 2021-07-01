#include <time.h>
#include "config.h"
#include "tmenu.h"
#include "twindow.h"
#include "window.h"
#include "tdraw.h"
#include "tresult.h"

#define WINDOW_COUNT 3
Window *windows[WINDOW_COUNT];
TResult *test_result;
// TODO: resize the window when switching windows.
size_t target_window = 1;
int running = 1;

void draw();
void destroy_windows();

void m_update(void *menu);
void m_draw(void *menu);
void m_destroy(void *menu);

void t_update(void *twindow);
void t_draw(void *twindow);
void t_destroy(void *twindow);

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
	TMenu *menu = tmenu_init("Menu", menu_options);
	windows[0] = window_init(menu, m_update, m_draw, m_destroy);
	TWindow *twindow = twindow_init(time(NULL), test_result);
	windows[1] = window_init(twindow, t_update, t_draw, t_destroy);
	TMenu *results_menu = tmenu_result_init("Results", menu_options, test_result);
	windows[2] = window_init(results_menu, m_update, m_draw, m_destroy);
	// Main loop.
	while (running) {
		// Draw.
		draw();
		// Update.
		windows[target_window]->update(windows[target_window]->window);
	}
	// Exit.
	endwin();
	destroy_windows();
	return 0;
}

void draw() {
	clear();
	mvprintw(0, 0, "tt");
	refresh();
	windows[target_window]->draw(windows[target_window]->window);
}

void destroy_windows() {
	for (size_t w = 0; w < WINDOW_COUNT; w++) {
		windows[w]->destroy(windows[w]);
	}
}

// Menu window functions.
void m_update(void *menu) {
	const int result = tmenu_update((TMenu *)menu);
	// Restart.
	if (result == 0) {
		// Destroy twindow.
		windows[1]->destroy(windows[1]);
		// Create new twindow.
		TWindow *twindow = twindow_init(time(NULL), test_result);
		windows[1] = window_init(twindow, t_update, t_draw, t_destroy);
		target_window = 1;
	}
	else if (result == 1) {
		const size_t seed = ((TWindow *)windows[1]->window)->seed;
		windows[1]->destroy(windows[1]);
		TWindow *new_window = twindow_init(seed, test_result);
		windows[1] = window_init(new_window, t_update, t_draw, t_destroy);
		target_window = 1;
	}
	// Exit.
	else if (result == 2)
		running = 0;
	// Escape.
	else if (result == -2)
		target_window = 1;
}

void m_draw(void *menu) { tmenu_draw((TMenu *)menu); };
void m_destroy(void *menu) { tmenu_destroy((TMenu *)menu); };

// Typing window functions.
void t_update(void *twindow) {
	const int result = twindow_update((TWindow *)twindow);
	// Completed the test.
	if (result == 0)
		target_window = 2;
	// Escape.
	else if (result == -2)
		target_window = 0;
}

void t_draw(void *twindow) { twindow_draw((TWindow *)twindow); };
void t_destroy(void *twindow) { twindow_destroy((TWindow *)twindow); };
