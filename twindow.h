#ifndef TWINDOW_H
#define TWINDOW_H

#include <ncurses.h>
#include "vector2.h"
#include "node.h"
#include "words.h"

#define TWINDOW_WIDTH 80
#define TWINDOW_HEIGHT 4

static size_t word_count = sizeof(words) / sizeof(words[0]);

typedef struct TWindow TWindow;
struct TWindow {
	WINDOW *window;
	Vector2 *screen_size;
	Vector2 padding;
	Vector2 size;
	Vector2 position;

	Vector2 cursor;

	Node *t_sen;
};

TWindow *twindow_init(Vector2 padding, Vector2 *screen_size);
void twindow_destroy(TWindow *win);
int twindow_update(TWindow *win);
void twindow_draw(TWindow *win);

unsigned min(unsigned a, unsigned b) {
	return a < b ? a : b;
}

TWindow *twindow_init(Vector2 padding, Vector2 *screen_size) {
	TWindow *win = (TWindow *)malloc(sizeof(TWindow));

	win->padding = padding;
	win->screen_size = screen_size;

	win->window = newwin(0, 0, 0, 0);
	keypad(win->window, TRUE);

	win->cursor = vector2_init(win->padding.y, win->padding.x);

	// Create the target sentence.
	win->t_sen = NULL;
	size_t char_count = 0;
	// Continue until enough characters are generated.
	while (char_count < TWINDOW_WIDTH * TWINDOW_HEIGHT) {
		const int word_index = rand() % word_count;
		const size_t word_length = strlen(words[word_index]);
		char_count += word_length + 1;
		Node* new_word = NULL;
		for (size_t c = 0; c < word_length; c++)
		{
			node_push(&new_word, &words[word_index][c]);
		}
		node_push(&win->t_sen, new_word);
	}


	return win;
}

void twindow_destroy(TWindow *win) {
	return;
}

int twindow_update(TWindow *win) {
	int c = wgetch(win->window);
	return -1;
}

void twindow_draw(TWindow *win) {
	Vector2 size = {
		.x = min(TWINDOW_WIDTH + win->padding.x * 2, win->screen_size->x),
		.y = min(TWINDOW_HEIGHT + win->padding.y * 2, win->screen_size->y)
	};
	wresize(win->window, size.y, size.x);


	Vector2 position = {
		.x = (win->screen_size->x - size.x) / 2,
		.y = (win->screen_size->y - size.y) / 2
	};
	mvwin(win->window, position.y, position.x);

	wclear(win->window);
	box(win->window, 0, 0);

	// Draw the sentence.
	

	wmove(win->window, win->cursor.y, win->cursor.x);

	wrefresh(win->window);
}

#endif
