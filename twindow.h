#ifndef TWINDOW_H
#define TWINDOW_H

#include <time.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tbackend.h"
#include "tmath.h"
#include "tdraw.h"
#include "tresult.h"
#include "ttime.h"

typedef struct TWindow TWindow;
struct TWindow {
	WINDOW *window;
	Vector2 size;
	Vector2 cursor;
	time_t seed;
	TTime start_time;
	bool start_time_set;
	TResult *result;
	Node *t_sen;
	Node *i_sen;
	Node *mistakes;
};

TWindow *twindow_init(time_t, TResult *);
void twindow_destroy(TWindow *);
int twindow_update(TWindow *);
void twindow_draw(TWindow *);
bool twindow_complete_words(TWindow *);
void twindow_status_wpm(TWindow *);
void twindow_status_time_taken(TWindow *);
void twindow_status_words(TWindow *);

TWindow *twindow_init(time_t seed, TResult *result) {
	TWindow *win = (TWindow *)malloc(sizeof(TWindow));
	win->window = newwin(0, 0, 0, 0);
	win->size = vector2_init(target_width, target_height);
	win->cursor = vector2_init(0, 1);
	win->seed = seed;
	win->start_time_set = false;
	win->result = result;
	tdraw_reposition(win->window, win->size);
	keypad(win->window, TRUE);
	// Create the target sentence.
	srand(seed);
	win->t_sen = sentence_init_words(25);
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
	bool set_start_time = false;
	switch (input) {
		case TKEY_RESIZE:
			tdraw_reposition(win->window, win->size);
			return -1;
		case TKEY_MENU:
			return -2;
		case TKEY_BACK:
			if (!remove_input_key(&win->i_sen))
				mistake = true;
			break;
		case TKEY_SPACE:
			set_start_time = true;
			if (!add_input_word(&win->i_sen))
				mistake = true;
			break;
		default:
			if (is_key_allowed(input)) {
				set_start_time = true;
				if (!add_input_key(&win->i_sen, input))
					mistake = true;
			}
	}
	if (mistake)
		add_mistake(&win->mistakes);
	// Set start time on first keypress.
	if (set_start_time && !win->start_time_set) {
		get_time(&win->start_time);
		win->start_time_set = true;
	}
	// Check if test is complete.
	if (twindow_complete_words(win)) {
		const float time_taken = time_diff(&win->start_time);
		const size_t typed_words = node_length(win->i_sen);
		const float wpm = get_wpm(typed_words, time_taken);
		*(win->result) = tresult_init(wpm, time_taken);
		return 0;
	}
	return -1;
}

void twindow_draw(TWindow *win) {
	wclear(win->window);
	Vector2 win_size = get_window_size(win->window);
	box(win->window, 0, 0);
	// Draw the sentence.
	size_t line_length = 0;
	size_t row = 1;
	wmove(win->window, row, 0);
	Node *t_word = win->t_sen;
	Node *i_word = win->i_sen;
	const size_t offset = get_offset(win_size, t_word, i_word);
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
			if (line_length == win_size.x) {
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
			if (word_length + line_length + 1 < win_size.x) {
				waddch(win->window, ' ');
				line_length++;
			} else {
				// Otherwise go to the next line.
				row++;
				wmove(win->window, row, 0);
				line_length = 0;
				// Stop drawing if hit last column.
				if (row == win_size.y)
					break;
			}
			if (i_word != NULL && i_word->next != NULL)
				getyx(win->window, win->cursor.y, win->cursor.x);
		}
		node_advance(&t_word);
		node_advance(&i_word);
	}
	// Draw the stats.
	if (win_size.y > 1) {
		wmove(win->window, 0, 0);
		wattron(win->window, COLOR_PAIR(4));
		twindow_status_words(win);
//		twindow_status_wpm(win);
//		twindow_status_time_taken(win);
		wattroff(win->window, COLOR_PAIR(4));
	}
	// Position the cursor.
	wmove(win->window, win->cursor.y, win->cursor.x);
	wrefresh(win->window);
	curs_set(1);
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
	if (!win->start_time_set) {
		wprintw(win->window, "  0 ");
		return;
	}
	// TODO: word count should be the amount of words typed correctly.
	const size_t word_count = node_length(win->i_sen);
	const float time_taken = time_diff(&win->start_time);
	const float wpm = get_wpm(word_count, time_taken);
	wprintw(win->window, "%3.0f ", wpm);
}

void twindow_status_time_taken(TWindow *win) {
	// Skip if start time hasn't been set.
	if (!win->start_time_set) {
		wprintw(win->window, "0 ");
		return;
	}
	const float time_taken = time_diff(&win->start_time);
	wprintw(win->window, "%3.0f ", time_taken);
}

void twindow_status_words(TWindow *win) {
	const size_t typed_words = node_length(win->i_sen);
	const size_t total_words = node_length(win->t_sen);
	wprintw(win->window, "%u/%u ", typed_words > 0 ? typed_words - 1 : 0, total_words);
}

#endif
