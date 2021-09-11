#ifndef TBACKEND_H
#define TBACKEND_H

#include <string.h>
#include <time.h>
#include "node.h"
#include "vector.h"
#include "words/english.h"
#include "quotes/english.h"

static const size_t words_count = sizeof(words) / sizeof(*words);
static const size_t quotes_count = sizeof(english_quotes) / sizeof(*english_quotes);
static const char allowed_keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-_=+`~[{]}\\|;:'\",<.>/?";
static const size_t allowed_key_count = sizeof(allowed_keys) / sizeof(char);

bool is_key_allowed(const int);
void sentence_init_size(Node **, const Vector);
void sentence_init_words(Node **, const size_t);
void sentence_init_quote(Node **);
bool add_input_key(Node **, const int);
bool remove_input_key(Node **);
bool add_input_word(Node **);
void add_mistake(Node **);
size_t get_word_length(Node *, Node *);
size_t get_typed_chars(Node *);
size_t get_offset(const Vector, Node *, Node *);

bool
is_key_allowed(const int key) {
	for (size_t k = 0; k < allowed_key_count; k++) {
		if (key == allowed_keys[k])
			return true;
	}
	return false;
}

void
sentence_init_size(Node **sentence, const Vector size) {
	*sentence = NULL;
	Vector position = {0, 1};
	while (true) {
		const size_t word_index = rand() % words_count;
		const size_t word_length = strlen(words[word_index]);
		// Stop if the word would be too long.
		const size_t new_length = position.x + word_length + 1;
		if (new_length >= size.x) {
			// Return if on the last line.
			if (position.y == size.y)
				break;
			// Move to the next line.
			position.x = 0;
			position.y++;
		}
		position.x += word_length + 1;
		Node *new_word = NULL;
		for (size_t c = 0; c < word_length; c++)
			node_push(&new_word, &words[word_index][c]);
		node_push(sentence, new_word);
	}
}

void
sentence_init_words(Node **sentence, const size_t word_count) {
	*sentence = NULL;
	for (size_t w = 0; w < word_count; w++) {
		const size_t word_index = rand() % words_count;
		const size_t word_length = strlen(words[word_index]);
		Node *word = NULL;
		for (size_t c = 0; c < word_length; c++)
			node_push(&word, &words[word_index][c]);
		node_push(sentence, word);
	}
}

void sentence_init_quote(Node **sentence) {
	// Pick a random quote.
	const Quote *quote = &english_quotes[rand() % quotes_count];
	// Initialise the sentence and add an empty word.
	*sentence = NULL;
	node_push(sentence, NULL);
	// Go through the quote.
	const char *text = quote->text;
	const size_t quote_length = strlen(text);
	for (size_t c = 0; c < quote_length; c++) {
		switch (text[c]) {
			case ' ':
				node_push(sentence, NULL);
				break;
			default:
				Node *last_word = node_tail(*sentence);
				node_push((Node **)&last_word->data, (void *)&text[c]);
		}
	}
}

// TODO: return bool to indicate if a mistake was made.
bool
add_input_key(Node **input_sentence, const int key) {
	char *character = (char *)malloc(sizeof(char));
	*character = (char)key;
	// Create new word if sentence is empty.
	if (*input_sentence == NULL) {
		Node *new_word = NULL;
		node_push(&new_word, character);
		node_push(input_sentence, new_word);
	}
	else {
		Node *last_word = node_tail(*input_sentence);
		node_push((Node **)&last_word->data, character);
	}
	return true;
}

// TODO: return bool to indicate if a mistake was made.
bool
remove_input_key(Node **input_sentence) {
	// Skip if already empty.
	if (*input_sentence == NULL)
		return false;
	// Otherwise remove from the last word.
	Node *last_word = node_tail(*input_sentence);
	// Remove the word if it is empty.
	if (last_word->data == NULL) {
		node_pop(input_sentence);
		return false;
	}
	// Remove the last character otherwise.
	node_pop((Node **)&last_word->data);
	return true;
}

bool
add_input_word(Node **input_sentence) {
	// Add a space character if the current sentence is empty.
	if (*input_sentence == NULL || node_tail(*input_sentence)->data == NULL) {
		add_input_key(input_sentence, ' ');
		return false;
	}
	// Add a new word.
	// TODO: Check if space was the correct key.
	node_push(input_sentence, NULL);
	return true;
}

void
add_mistake(Node **mistakes) {
	time_t *mistake = (time_t *)malloc(sizeof(time_t));
	*mistake = time(NULL);
	node_push(mistakes, mistake);
}

size_t
get_word_length(Node *t_word, Node *i_word) {
	size_t length = 0;
	if (t_word != NULL)
		length = node_length((Node *)t_word->data);
	if (i_word != NULL) {
		const size_t i_length = node_length((Node *)i_word->data);
		if (i_length > length)
			return i_length;
	}
	return length;
}

size_t
get_typed_chars(Node *i_sen) {
	size_t length = 0;
	while (i_sen != NULL) {
		length += node_length(i_sen->data);
		if (node_advance(&i_sen))
			length++;
	}
	return length;
}

size_t
get_offset(const Vector size, Node *t_word, Node *i_word) {
	size_t offset = 0;
	// Adds a one line buffer, so you can see the previous line.
	size_t buffer = 0;
	size_t line_offset = 0;
	size_t line_length = 0;
	while (i_word != NULL) {
		const size_t word_length = get_word_length(t_word, i_word) + 1;
		// Word is longer than line.
		if (word_length > size.x && i_word->next != NULL) {
			offset += buffer + 1;
			buffer = line_offset;
			line_offset = 0;
			line_length = 0;
		}
		// Word too long for line.
		else if (line_length + word_length > size.x) {
			offset += buffer;
			buffer = line_offset;
			line_offset = 1;
			line_length = word_length;
		}
		// Normal word.
		else {
			line_offset++;
			line_length += word_length;
		}
		node_advance(&t_word);
		node_advance(&i_word);
	}
	return offset;
}

#endif
