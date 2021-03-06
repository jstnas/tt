#ifndef TTEST_H
#define TTEST_H

#include <time.h>
#include <ncurses.h>
#include "config.h"
#include "vector2.h"
#include "tbackend.h"
#include "tdraw.h"
#include "tresult.h"
#include "ttime.h"

typedef struct {
	WINDOW *window;
	Vector2 size;
	Vector2 cursor;
	time_t seed;
	TTime start_time;
	bool start_time_set;
	bool resize;
	TResult *result;
	Node *t_sen;
	Node *i_sen;
	Node *mistakes;
} TTest;

TTest *ttest_init(time_t, TResult *);
void ttest_destroy(TTest *);
int ttest_update(TTest *);
void ttest_draw(TTest *);
bool ttest_complete_words(TTest *);
void ttest_status_wpm(TTest *);
void ttest_status_time_taken(TTest *);
void ttest_status_words(TTest *);
void ttest_status_chars(TTest *);
float get_wpm(TTest *);

TTest *ttest_init(time_t seed, TResult *result) {
	TTest *test = (TTest *)malloc(sizeof(TTest));
	test->window = newwin(0, 0, 0, 0);
	wbkgd(test->window, COLOR_PAIR(1));
	test->size.x = TWINDOW_WIDTH;
	test->size.y = TWINDOW_HEIGHT;
	test->seed = seed;
	test->start_time_set = false;
	test->resize = true;
	test->result = result;
	keypad(test->window, TRUE);
	// Create the target sentence.
	srand(seed);
	test->t_sen = sentence_init_words(25);
	test->i_sen = NULL;
	return test;
}

void ttest_destroy(TTest *test) {
	delwin(test->window);
	free(test);
}

int ttest_update(TTest *test) {
	const int input = wgetch(test->window);
	bool mistake = false;
	bool set_start_time = false;
	switch (input) {
		case TKEY_RESIZE:
			test->resize = true;
			return -1;
		case TKEY_MENU:
			return -2;
		case TKEY_BACK:
			if (!remove_input_key(&test->i_sen))
				mistake = true;
			break;
		case TKEY_SPACE:
			set_start_time = true;
			if (!add_input_word(&test->i_sen))
				mistake = true;
			break;
		default:
			if (is_key_allowed(input)) {
				set_start_time = true;
				if (!add_input_key(&test->i_sen, input))
					mistake = true;
			}
	}
	if (mistake)
		add_mistake(&test->mistakes);
	// Set start time on first keypress.
	if (set_start_time && !test->start_time_set) {
		get_time(&test->start_time);
		test->start_time_set = true;
	}
	// Check if test is complete.
	if (ttest_complete_words(test)) {
		test->result->wpm = get_wpm(test);
		test->result->time_taken = time_diff(&test->start_time);
		return 0;
	}
	return -1;
}

void ttest_draw(TTest *test) {
	if (test->resize) {
		test->resize = false;
		tdraw_reposition(test->window, test->size);
	}
	wclear(test->window);
	Vector2 win_size;
	get_window_size(&win_size, test->window);
	box(test->window, 0, 0);
	// Draw the sentence.
	size_t line_length = 0;
	size_t row = 1;
	wmove(test->window, row, 0);
	Node *t_word = test->t_sen;
	Node *i_word = test->i_sen;
	const size_t offset = get_offset(win_size, t_word, i_word);
	// Offset the sentence.
	for (size_t o = 0; o < offset; o++) {
		node_advance(&t_word);
		node_advance(&i_word);
	}
	// Reset cursor position.
	test->cursor.x = 0;
	test->cursor.y = 1;
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
				getyx(test->window, test->cursor.y, test->cursor.x);
				test->cursor.x++;
			}
			// Update cursor position even if you get the wrong character.
			else if (i_char != NULL) {
				getyx(test->window, test->cursor.y, test->cursor.x);
				test->cursor.x++;
			}
			// Draw the character.
			waddch(test->window, character | COLOR_PAIR(pair));
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
				waddch(test->window, ' ');
				line_length++;
			} else {
				// Otherwise go to the next line.
				row++;
				wmove(test->window, row, 0);
				line_length = 0;
				// Stop drawing if hit last column.
				if (row == win_size.y)
					break;
			}
			if (i_word != NULL && i_word->next != NULL)
				getyx(test->window, test->cursor.y, test->cursor.x);
		}
		node_advance(&t_word);
		node_advance(&i_word);
	}
	// Draw the stats.
	if (win_size.y > 1) {
		wmove(test->window, 0, 0);
		wattron(test->window, COLOR_PAIR(4));
		ttest_status_words(test);
		ttest_status_wpm(test);
		ttest_status_time_taken(test);
//		ttest_status_chars(test);
		wattroff(test->window, COLOR_PAIR(4));
	}
	// Position the cursor.
	wmove(test->window, test->cursor.y, test->cursor.x);
	wrefresh(test->window);
	curs_set(1);
}

// Complete function for words mode.
bool ttest_complete_words(TTest *test) {
	const size_t i_sen_length = node_length(test->i_sen);
	const size_t t_sen_length = node_length(test->t_sen);
	if (i_sen_length > t_sen_length)
		return true;
	else if (i_sen_length < t_sen_length)
		return false;
	const size_t i_word_length = node_length((Node *)node_tail(test->i_sen)->data);
	const size_t t_word_length = node_length((Node *)node_tail(test->t_sen)->data);
	if (i_word_length >= t_word_length)
		return true;
	return false;
}

void ttest_status_wpm(TTest *test) {
	float wpm = get_wpm(test);
	if (!test->start_time_set) {
		wpm = 0;
	}
	// TODO: word count should be the amount of words typed correctly.
	wprintw(test->window, "%3.0f ", wpm);
}

void ttest_status_time_taken(TTest *test) {
	float time_taken = time_diff(&test->start_time);
	// Skip if start time hasn't been set.
	if (!test->start_time_set) {
		time_taken = 0;
	}
	wprintw(test->window, "%3.0f ", time_taken);
}

void ttest_status_words(TTest *test) {
	const size_t typed_words = node_length(test->i_sen);
	const size_t total_words = node_length(test->t_sen);
	wprintw(test->window, "%u/%u ", typed_words > 0 ? typed_words - 1 : 0, total_words);
}

void ttest_status_chars(TTest *test) {
	const size_t chars = get_typed_chars(test->i_sen);
	wprintw(test->window, "%u", chars);
}

float get_wpm(TTest *test) {
	const size_t typed_chars = get_typed_chars(test->i_sen);
	const float time_taken = time_diff(&test->start_time);
	return typed_chars / 5.0 / (time_taken / 60.0);
}

#endif
