#include <string.h>
#include <ncurses.h>
#include "node.h"

struct node *t_sen = NULL;
struct node *i_sen = NULL;
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

void add_target_word(char word[])
{
	// Create a new word node.
	struct node* new_word = NULL;
	int length = strlen(word);
	for (int c = 0; c < length; c++)
	{
		node_push(&new_word, &word[c]);
	}
	node_push(&t_sen, new_word);
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
	// Remove the whole word if the word is empty.
	if (last_word->data == NULL)
	{
		node_pop(&i_sen);
	}
	// Remove the last character otherwise.
	else
	{
		node_pop((struct node**)&last_word->data);
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
	add_target_word("Hello");
	add_target_word("World");
	return 0;
}

void draw_words_new()
{
	struct node *i_word = i_sen, *t_word = t_sen;
	while (i_word != NULL || t_word != NULL)
	{
		struct node *i_char = i_word == NULL ? NULL : i_word->data;
		struct node *t_char  = t_word == NULL ? NULL : t_word->data;
		while (i_char != NULL || t_char != NULL)
		{
			// Normal.
			if (i_char == NULL)
				addch(*(char*)t_char->data | COLOR_PAIR(0));
			// Wrong.
			else if (t_char == NULL)
				addch(*(char*)i_char->data | COLOR_PAIR(2));
			else
			{
				if (*(char*)i_char->data == *(char*)t_char->data)
				{
					addch(*(char*)i_char->data | COLOR_PAIR(1));
				}
				else
				{
					addch(*(char*)t_char->data | COLOR_PAIR(2));
				}
			}
			i_char = i_char == NULL ? NULL : i_char->next;
			t_char = t_char == NULL ? NULL : t_char->next;
		}
		// Add space.
		bool add_space = false;
		if (i_word != NULL)
		{
			if (i_word->next != NULL)
				add_space = true;
		}
		if (t_word != NULL)
		{
			if (t_word->next != NULL)
				add_space = true;
		}
		if (add_space)
			addch(' ');
		i_word = i_word == NULL ? NULL : i_word->next;
		t_word = t_word == NULL ? NULL : t_word->next;
	}
}

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
	draw_words_new();
	// Update screen.
	refresh();
	return 0;
};

int main()
{
	init();
	draw();
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
