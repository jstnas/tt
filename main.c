#include <string.h>
#include <ncurses.h>
#include "node.h"

struct node* t_sen = NULL;
struct node* i_sen = NULL;
char allowed_keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
bool allowed = false;

bool is_key_allowed(const int key)
{
	for (int c = 0; c < strlen(allowed_keys); c++)
	{
		const int char_num = (int)allowed_keys[c];
		if (key == char_num)
			return true;
	}
	return false;
}

void add_char(char* character)
{
	// Create a new word.
	if (i_sen == NULL)
	{
		struct node* new_word = NULL;
		node_push(&new_word, character);
		node_push(&i_sen, new_word);
		return;
	}
	// Add the character to the last word.
	struct node* last_word = node_tail(i_sen);
	node_push((struct node**)&last_word->data, character);
}

void remove_char()
{
	// Skip if sentence is empty.
	if (i_sen == NULL)
		return;
	// Get the last word.
	struct node* last_word = node_tail(i_sen);
	node_pop((struct node**)&last_word->data);
	if (last_word->data == NULL)
	{
		node_pop(&i_sen);
	}
}

void add_word()
{
	// Skip if the sentence is empty.
	if (i_sen == NULL)
		return;
	if (node_tail(i_sen)->data == NULL)
		return;
	node_push(&i_sen, NULL);
}

int init()
{
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	if (!has_colors())
	{
		printw("No colour support!");
		return 1;
	}
	start_color();
	init_pair(0, COLOR_WHITE, COLOR_BLACK); // Normal.
	init_pair(1, COLOR_GREEN, COLOR_BLACK); // Correct.
	init_pair(2, COLOR_RED, COLOR_BLACK); // Incorrect.
	return 0;
}

void draw_words()
{
	struct node* c_word = i_sen;
	if (c_word == NULL)
		return;
	// Go through each word.
	do
	{
		struct node* c_char = c_word->data;
		if (c_char == NULL)
			return;
		// Go through each character.
		do
		{
			addch(*(char*)c_char->data);
			c_char = c_char->next;
		}
		while (c_char != NULL);
		// Add a space between words.
		if (c_word->next != NULL)
		{
			addch(' ');
		}
		c_word = c_word->next;
	}
	while (c_word != NULL);
};

int input()
{
	// Get a keypress.
	int input = getch();
	// Normal keypress.
	if (is_key_allowed(input))
	{
		char* character = (char*)malloc(sizeof(char));
		*character = (char)input;
		add_char(character);
	}
	// Space.
	else if (input == ' ')
	{
		add_word();
	}
	// Backspace.
	else if (input == KEY_BACKSPACE)
	{
		remove_char();
	}
	return 0;
};

int draw()
{
	// Clear screen.
	erase();
	// Draw each character.
	draw_words();
	// Update screen.
	refresh();
	return 0;
};

int main()
{
	init();
	struct node *word = NULL;
	// Update.
	while (TRUE)
	{
		// Get input.
		input();
		draw();
	}
	// End.
	endwin();
	return 0;
}
