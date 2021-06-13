#ifndef TWINDOW_H
#define TWINDOW_H

#include <time.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tbackend.h"

typedef struct TWindow TWindow;
struct TWindow {
	WINDOW *window;
	Vector2 *screen_size;
	Vector2 position;
	Vector2 cursor;
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

	win->cursor = vector2_init(0, 1);
	win->word_offset = 0;

	// Create the target sentence.
	srand(time(NULL));
	const Vector2 sentence_size = vector2_init(target_width, target_height - 1);
	win->t_sen = sentence_init_size(sentence_size);

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
	const Vector2 size = {
		.x = min(target_width, win->screen_size->x),
		.y = min(target_height, win->screen_size->y)
	};
	wresize(win->window, size.y, size.x);


	const Vector2 position = {
		.x = (win->screen_size->x - size.x) / 2,
		.y = (win->screen_size->y - size.y) / 2
	};
	mvwin(win->window, position.y, position.x);

	wclear(win->window);
	box(win->window, 0, 0);

	// Draw the sentence.
	size_t line_length = 0;
	size_t row = 1;
	wmove(win->window, row, 0);
	Node *t_word = win->t_sen;
	Node *i_word = win->i_sen;
	// Move up a line if on the last line.
	if (win->cursor.y == size.y - 1) {
		win->word_offset += 5;
	}
	// Offset the sentence.
	for (size_t o = 0; o < win->word_offset; o++) {
		node_advance(&t_word);
		node_advance(&i_word);
	}
	// Continue until the longer sentence is printed.
	while (t_word != NULL || i_word != NULL) {
		Node *t_char = t_word == NULL ? NULL : (Node *)t_word->data;
		Node *i_char = i_word == NULL ? NULL : (Node *)i_word->data;
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
				getyx(win->window, win->cursor.y, win->cursor.x);
				win->cursor.x++;
			}
			// Update cursor position even if you get the wrong character.
			else if (i_char != NULL) {
				getyx(win->window, win->cursor.y, win->cursor.x);
				win->cursor.x++;
			}
			// Draw the character.
			waddch(win->window, character | COLOR_PAIR(pair));
			// Advance to the next character.
			node_advance(&t_char);
			node_advance(&i_char);
			// Draw onto the next line.
			if (line_length == size.x) {
				line_length = 0;
				row++;
			}
			else {
				line_length++;
			}
		}
		// Skip if there are no more words.
		if (t_word != NULL || i_word != NULL) {
			const size_t word_length = get_word_length(node_next(t_word), node_next(i_word));
			// Add a space if there is enough space for the next word.
			if (word_length + line_length < size.x) {
				waddch(win->window, ' ');
				line_length++;
			} else {
				// Otherwise go to the next line.
				row++;
				wmove(win->window, row, 0);
				line_length = 0;
				// Stop drawing if hit last column.
				if (row == target_height)
					break;
			}
			if (i_word != NULL && i_word->next != NULL)
				getyx(win->window, win->cursor.y, win->cursor.x);
		}
		node_advance(&t_word);
		node_advance(&i_word);
	}

	// Draw the stats.
	mvwprintw(win->window, 0, 0, "%u %u", target_width, target_height);

	// Position the cursor.
	wmove(win->window, win->cursor.y, win->cursor.x);

	wrefresh(win->window);
}

unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

#endif
