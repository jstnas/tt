#ifndef TWINDOW_H
#define TWINDOW_H

#include <time.h>
#include <math.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tbackend.h"
#include "tmath.h"

typedef struct TWindow TWindow;
struct TWindow {
	WINDOW *window;
	Vector2 *screen_size;
	Vector2 position;
	Vector2 cursor;
	time_t seed;
	time_t start_time;

	Node *t_sen;
	Node *i_sen;
	Node *mistakes;
};

TWindow *twindow_init(Vector2 *screen_size, time_t seed);
void twindow_destroy(TWindow *win);
int twindow_update(TWindow *win);
void twindow_draw(TWindow *win);
bool twindow_complete_words(TWindow *win);
void twindow_status_wpm(TWindow *win);
void twindow_status_time_taken(TWindow *win);

TWindow *twindow_init(Vector2 *screen_size, time_t seed) {
	TWindow *win = (TWindow *)malloc(sizeof(TWindow));
	win->screen_size = screen_size;
	win->window = newwin(0, 0, 0, 0);
	keypad(win->window, TRUE);
	win->cursor = vector2_init(0, 1);
	time(&win->seed);
	// Create the target sentence.
	srand(time(NULL));
	win->t_sen = sentence_init_words(50);
	win->i_sen = NULL;
	return win;
}

void twindow_destroy(TWindow *win) {
	delwin(win->window);
	free(win);
}

int twindow_update(TWindow *win) {
	const int input = wgetch(win->window);
	bool mistake = false;
	if (input == key_menu)
		return -2;
	if (win->start_time == 0)
		time(&win->start_time);
	if (input == key_back) {
		if (!remove_input_key(&win->i_sen))
			mistake = true;
	}
	else if (input == key_space) {
		if (!add_input_word(&win->i_sen))
			mistake = true;
	}
	else if (is_key_allowed((char)input)) {
		if (!add_input_key(&win->i_sen, input))
			mistake = true;
	}
	if (mistake)
		add_mistake(&win->mistakes);
	if (twindow_complete_words(win))
		return 0;
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
	const size_t offset = get_offset(size, t_word, i_word);
	// Offset the sentence.
	for (size_t o = 0; o < offset; o++) {
		node_advance(&t_word);
		node_advance(&i_word);
	}
	// Reset cursor position.
	win->cursor = vector2_init(0, 1);
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
			if (word_length + line_length + 1 < size.x) {
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
	wmove(win->window, 0, 0);
	twindow_status_wpm(win);
	twindow_status_time_taken(win);

	// Position the cursor.
	wmove(win->window, win->cursor.y, win->cursor.x);

	wrefresh(win->window);
}

// Complete function for words mode.
bool twindow_complete_words(TWindow *win) {
	const size_t i_sen_length = node_length(win->i_sen);
	const size_t t_sen_length = node_length(win->t_sen);
	if (i_sen_length > t_sen_length)
		return true;
	else if (i_sen_length < t_sen_length)
		return false;
	const size_t i_word_length = node_length((Node *)node_tail(win->i_sen)->data);
	const size_t t_word_length = node_length((Node *)node_tail(win->t_sen)->data);
	if (i_word_length >= t_word_length)
		return true;
	return false;
}

void twindow_status_wpm(TWindow *win) {
	const size_t word_count = node_length(win->i_sen);
	const double time_taken = (time(NULL) - win->start_time) / 60.0;
	const unsigned wpm = (unsigned)round(word_count / time_taken);
	wprintw(win->window, "%u ", wpm);
}

void twindow_status_time_taken(TWindow *win) {
	const time_t time_taken = time(NULL) - win->seed;
	wprintw(win->window, "%u ", time_taken);
}

#endif
