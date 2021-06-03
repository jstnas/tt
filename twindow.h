#ifndef TWINDOW_H
#define TWINDOW_H

#include <ncurses.h>
#include "vector2.h"
#include "tbackend.h"

#define TWINDOW_WIDTH 80
#define TWINDOW_HEIGHT 4

typedef struct TWindow TWindow;
struct TWindow {
	WINDOW *window;
	Vector2 *screen_size;
	Vector2 size;
	Vector2 position;

	Vector2 cursor;

	Node *t_sen;
	Node *i_sen;
};

TWindow *twindow_init(Vector2 *screen_size);
void twindow_destroy(TWindow *win);
int twindow_update(TWindow *win);
void twindow_draw(TWindow *win);

unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

TWindow *twindow_init(Vector2 *screen_size) {
	TWindow *win = (TWindow *)malloc(sizeof(TWindow));

	win->screen_size = screen_size;

	win->window = newwin(0, 0, 0, 0);
	keypad(win->window, TRUE);

	win->cursor = vector2_init(0, 1);

	// Create the target sentence.
	srand(time(NULL));
	const size_t target_length = TWINDOW_WIDTH * (TWINDOW_HEIGHT - 1);
	win->t_sen = init_target_sentence(target_length);
	win->i_sen = NULL;

	return win;
}

void twindow_destroy(TWindow *win) {
	return;
}

int twindow_update(TWindow *win) {
	int input = wgetch(win->window);
	if (input == KEY_BACKSPACE)
		remove_input_key(&win->i_sen);
	else if (input == ' ') {
		add_input_word(&win->i_sen);
	}
	else if (is_key_allowed((char)input))
		add_input_key(&win->i_sen, input);
	return -1;
}

void twindow_draw(TWindow *win) {
	Vector2 size = {
		.x = min(TWINDOW_WIDTH, win->screen_size->x),
		.y = min(TWINDOW_HEIGHT, win->screen_size->y)
	};
	wresize(win->window, size.y, size.x);


	Vector2 position = {
		.x = (win->screen_size->x - size.x) / 2,
		.y = (win->screen_size->y - size.y) / 2
	};
	mvwin(win->window, position.y, position.x);

	wclear(win->window);
//	box(win->window, 0, 0);

	// Draw the stats.
	mvwprintw(win->window, 0, 0, "30");

	// Draw the sentence.

	size_t line_length = 0;
	size_t row = 1;
	wmove(win->window, row, 0);
	Node *t_word = win->t_sen;
	while (t_word != NULL) {
		Node *t_char = t_word == NULL ? NULL : t_word->data;
		while (t_char != NULL) {
			waddch(win->window, *(char*)t_char->data);
			line_length++;
			t_char = t_char == NULL ? NULL : t_char->next;
		}
		// Add a space or wrap the word.
		if (t_word != NULL && t_word->next != NULL)
		{
			if (node_length(t_word->next->data) + line_length <= size.x) {
				waddch(win->window, ' ');
				line_length++;
			}
			else {
				row++;
				wmove(win->window, row, 0);
				line_length = 0;
				// Stop drawing if hit last column.
				if (row == TWINDOW_HEIGHT)
					break;
			}
		}
		t_word = t_word == NULL ? NULL : t_word->next;
	}

	/*
	size_t row = 1;
	size_t line_length = 0;
	wmove(win->window, row, 0);
	Node *i_word = win->i_sen;
	while (i_word != NULL) {
		Node *i_char = i_word == NULL ? NULL : i_word->data;
		while(i_char != NULL) {
			waddch(win->window, *(char *)i_char->data);
			line_length++;
			i_char = i_char == NULL ? NULL : i_char->next;
		}
		if (i_word != NULL && i_word->next != NULL)
		{
			if (node_length(i_word->next->data) + line_length <= size.x) {
				waddch(win->window, ' ');
				line_length++;
			}
			else {
				row++;
				wmove(win->window, row, 0);
				line_length = 0;
				// Stop drawing if hit last column.
				if (row == TWINDOW_HEIGHT)
					break;
			}
		}
		i_word = i_word == NULL ? NULL : i_word->next;
	}
	*/

	wmove(win->window, win->cursor.y, win->cursor.x);

	wrefresh(win->window);
}

#endif
