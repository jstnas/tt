#include <time.h>
#include "config.h"
#include "menu.h"
#include "twindow.h"
#include "window.h"

#define WINDOW_COUNT 2
Window *windows[WINDOW_COUNT];
Vector2 *screen_size;
size_t target_window = 1;
int running = 1;

void twin_update();
void twin_draw();
void destroy_windows();
Vector2 get_window_size(WINDOW *window);

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

	screen_size = (Vector2 *)malloc(sizeof(Vector2));
	*screen_size = get_window_size(stdscr);

	init_pair(1, color_foreground, color_background);
	init_pair(2, color_correct, color_background);
	init_pair(3, color_incorrect, color_background);

	// Create windows.
	char *menu_options[] = {"Restart", "Options", "Exit", NULL};
	Vector2 padding = vector2_init(1, 1);
	Menu *menu = menu_init("Menu", menu_options, padding, screen_size);
	windows[0] = window_init(menu, m_update, m_draw, m_destroy);

	TWindow *twindow = twindow_init(screen_size);
	windows[1] = window_init(twindow, t_update, t_draw, t_destroy);

	// Main loop.
	mvprintw(0, 0, "tt");
	refresh();
	twin_draw();
	while (running)
	{
		twin_update();
		*screen_size = get_window_size(stdscr);
		clear();
		mvprintw(0, 0, "tt");
		refresh();
		twin_draw();
	}

	// Exit.
	endwin();
	destroy_windows();
	free(screen_size);
	return 0;
}

void twin_update() {
	windows[target_window]->update(windows[target_window]->window);
}

void twin_draw() {
	windows[target_window]->draw(windows[target_window]->window);
}

void destroy_windows() {
	for (size_t w = 0; w < WINDOW_COUNT; w++) {
		windows[w]->destroy(windows[w]);
	}
}

Vector2 get_window_size(WINDOW *window) {
	Vector2 window_size;
	getmaxyx(window, window_size.y, window_size.x);
	return window_size;
}

// Menu window functions.
void m_update(void *menu) {
	int result = menu_update((Menu *)menu);
	if (result == 2)
		running = 0;
	else if (result == -2)
		target_window = 1;
}

void m_draw(void *menu) { menu_draw((Menu *)menu); };
void m_destroy(void *menu) { menu_destroy((Menu *)menu); };

// Typing window functions.
void t_update(void *twindow) {
	int result = twindow_update((TWindow *)twindow);
	if (result == 1)
		target_window = 0;
}

void t_draw(void *twindow) { twindow_draw((TWindow *)twindow); };
void t_destroy(void *twindow) { twindow_destroy((TWindow *)twindow); };