#ifndef TWINDOW_H
#define TWINDOW_H

#include <ncurses.h>
#include "vector2.h"
#include "tbackend.h"

typedef struct TWindow TWindow;
struct TWindow {
	WINDOW *window;
	Vector2 *screen_size;
	Vector2 size;
	Vector2 position;
	size_t word_offset;

	Node *t_sen;
	Node *i_sen;
};

TWindow *twindow_init(Vector2 *screen_size);
void twindow_destroy(TWindow *win);
int twindow_update(TWindow *win);
void twindow_draw(TWindow *win);
unsigned min(unsigned a, unsigned b);

TWindow *twindow_init(Vector2 *screen_size) {
	TWindow *win = (TWindow *)malloc(sizeof(TWindow));

	win->screen_size = screen_size;

	win->window = newwin(0, 0, 0, 0);
	keypad(win->window, TRUE);

	win->word_offset = 0;

	// Create the target sentence.
	srand(time(NULL));
	const size_t target_length = window_width * (window_height - 1);
	win->t_sen = sentence_init_words(10);

	win->i_sen = NULL;

	return win;
}

void twindow_destroy(TWindow *win) {
	delwin(win->window);
	free(win);
}

int twindow_update(TWindow *win) {
	int input = wgetch(win->window);

	if (input == key_back)
		remove_input_key(&win->i_sen);
	else if (input == key_space)
		add_input_word(&win->i_sen);
	else if (input == key_menu)
		return -2;
	else if (is_key_allowed((char)input))
		add_input_key(&win->i_sen, input);

	return -1;
}

void twindow_draw(TWindow *win) {
	Vector2 size = {
		.x = min(window_width, win->screen_size->x),
		.y = min(window_height, win->screen_size->y)
	};
	wresize(win->window, size.y, size.x);


	Vector2 position = {
		.x = (win->screen_size->x - size.x) / 2,
		.y = (win->screen_size->y - size.y) / 2
	};
	mvwin(win->window, position.y, position.x);

	wclear(win->window);
	box(win->window, 0, 0);

	// Draw the stats.
	mvwprintw(win->window, 0, 0, "30");

	// Draw the sentence.
	Vector2 cursor = vector2_init(0, 1);
	size_t line_length = 0;
	size_t row = 1;
	wmove(win->window, row, 0);
	Node *t_word = win->t_sen;
	Node *i_word = win->i_sen;
	// Offset the sentence.
	for (size_t o = 0; o < win->word_offset; o++) {
		t_word = t_word == NULL ? NULL : t_word->next;
		i_word = i_word == NULL ? NULL : i_word->next;
	}
	// Continue until the longer sentence is printed.
	while (t_word != NULL || i_word != NULL) {
		Node *t_char = t_word == NULL ? NULL : t_word->data;
		Node *i_char = i_word == NULL ? NULL : i_word->data;
		// Continue until the longer word is printed.
		while (t_char != NULL || i_char != NULL) {
			// Choose the character to display.
			const char character = t_char == NULL ? *(char *)i_char->data :
				*(char *)t_char->data;
			// Choose the color pair.
			short pair = 3;
			// Normal if haven't reached this point.
			if (i_char == NULL && i_word != NULL && i_word->next == NULL)
				pair = 1;
			else if (i_word == NULL)
				pair = 1;
			// Correct if characters match.
			else if (t_char != NULL && i_char != NULL &&
					*(char *)t_char->data == *(char *)i_char->data) {
				pair = 2;
				getyx(win->window, cursor.y, cursor.x);
				cursor.x++;
			}
			// Update cursor position even if you get the wrong character.
			else if (i_char != NULL) {
				getyx(win->window, cursor.y, cursor.x);
				cursor.x++;
			}
			// Draw the character.
			waddch(win->window, character | COLOR_PAIR(pair));
			// Advance to the next character.
			t_char = t_char == NULL ? NULL : t_char->next;
			i_char = i_char == NULL ? NULL : i_char->next;
			line_length++;
		}
		// Work out the next longest word.
		size_t word_length = next_word_length(t_word, i_word);
		// Skip if there are no more words.
		if (word_length > 0) {
			// Add a space if there is enough space for the next word.
			if (word_length + line_length <= size.x) {
				waddch(win->window, ' ');
				line_length++;
			} else {
				// Otherwise go to the next line.
				row++;
				wmove(win->window, row, 0);
				line_length = 0;
				// Stop drawing if hit last column.
				if (row == window_height)
					break;
			}
			if (i_word != NULL && i_word->next != NULL)
				getyx(win->window, cursor.y, cursor.x);
		}
		// Advance to the next word.
		t_word = t_word == NULL ? NULL : t_word->next;
		i_word = i_word == NULL ? NULL : i_word->next;
	}

	// Position the cursor.
	wmove(win->window, cursor.y, cursor.x);

	wrefresh(win->window);
}

unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

#endif
