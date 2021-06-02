#include "menu.h"
#include "window.h"

int running = 1;

Vector2 get_window_size(WINDOW *window) {
	Vector2 window_size;
	getmaxyx(window, window_size.y, window_size.x);
	return window_size;
}

void m_update(void* menu) {
	int result = menu_update((Menu*)menu);
	if (result == 2)
		running = 0;
}

void m_draw(void* menu) { menu_draw((Menu*)menu); };
void m_destroy(void* menu) { menu_destroy((Menu*)menu); };

int main() {
	Window *target_window;
	// Init.
	initscr();
	refresh();
	noecho();
	cbreak();

	Vector2 *screen_size = (Vector2 *)malloc(sizeof(Vector2));
	*screen_size = get_window_size(stdscr);

	// Create new window.
	char *menu_options[] = {"Restart", "Options", "Exit", NULL};
	Vector2 padding = vector2_init(1, 1);
	Menu *menu = menu_init("Menu", menu_options, padding, screen_size);
	menu_draw(menu);
	Window main_menu_window = {
		.window = menu,
		.update = &m_update,
		.draw = &m_draw,
		.destroy = &m_destroy
	};
	target_window = &main_menu_window;

	while (running)
	{
		target_window->update(target_window->window);
		*screen_size = get_window_size(stdscr);
		clear();
		refresh();
		target_window->draw(target_window->window);
	}

	// Exit.
	endwin();
	target_window->destroy(target_window->window);
	free(screen_size);
	return 0;
}
