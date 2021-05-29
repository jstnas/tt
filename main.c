#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "config.h"
#include "node.h"
#include "words.h"

static unsigned word_count = (sizeof words / sizeof words[0]);
static char allowed_keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
static unsigned seed = 0;
static int rng_word_count = 50;
struct node *t_sen = NULL;
struct node *i_sen = NULL;
bool allowed = false;
int x = 0;
int y = 0;

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

void add_target_word(char *word)
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

void add_random_word()
{
	// Get the word index.
	const int word_index = rand() % word_count;
	add_target_word(words[word_index]);
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
	// Initialise rng.
	if (seed == 0)
	{
		srand(time(NULL));
	}
	else
		srand(seed);
	initscr();
	use_default_colors();
	noecho();
	keypad(stdscr, TRUE);
	if (!has_colors())
	{
		printw("No colour support!");
		return 1;
	}
	start_color();
	init_pair(1, foreground_color, background_color); // Normal.
	init_pair(2, correct_color, background_color); // Correct.
	init_pair(3, incorrect_color, background_color); // Incorrect.
	for (int i = 0; i < rng_word_count; i++)
		add_random_word();
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
				addch(*(char*)t_char->data | COLOR_PAIR(1));
			// Wrong.
			else if (t_char == NULL)
				addch(*(char*)i_char->data | COLOR_PAIR(3));
			else
			{
				if (*(char*)i_char->data == *(char*)t_char->data)
				{
					addch(*(char*)i_char->data | COLOR_PAIR(2));
				}
				else
				{
					addch(*(char*)t_char->data | COLOR_PAIR(3));
				}
			}
			i_char = i_char == NULL ? NULL : i_char->next;
			t_char = t_char == NULL ? NULL : t_char->next;
		}
		// Add space.
		bool add_space = false;

		if (i_word != NULL && i_word->next != NULL)
		{
			add_space = true;
		}
		else if (t_word != NULL && t_word->next != NULL)
		{
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
	move(5, 0);
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
