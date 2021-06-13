#ifndef TBACKEND_H
#define TBACKEND_H

#include <string.h>
#include "node.h"
#include "words.h"
#include "vector2.h"

static size_t words_count = sizeof(words) / sizeof(*words);
static char allowed_keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
static size_t allowed_key_count = sizeof(allowed_keys) / sizeof(char);

bool is_key_allowed(const char key);
Node *sentence_init_size(const Vector2 size);
Node *sentence_init_words(const size_t word_count);
void add_input_key(Node **input_sentence, const int key);
void remove_input_key(Node **input_sentence);
void add_input_word(Node **input_sentence);
size_t get_word_length(Node *t_word, Node *i_word);

bool is_key_allowed(const char key) {
	for (size_t k = 0; k < allowed_key_count; k++) {
		if (key == allowed_keys[k])
			return true;
	}
	return false;
}

Node *sentence_init_size(const Vector2 size) {
	Node *sentence = NULL;

	Vector2 position = vector2_init(0, 1);

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
		node_push(&sentence, new_word);
	}
	return sentence;
}

Node *sentence_init_words(const size_t word_count) {
	Node *sentence = NULL;
	for (size_t w = 0; w < word_count; w++) {
		const size_t word_index = rand() % words_count;
		const size_t word_length = strlen(words[word_index]);
		Node *word = NULL;
		for (size_t c = 0; c < word_length; c++)
			node_push(&word, &words[word_index][c]);
		node_push(&sentence, word);
	}
	return sentence;
}

void add_input_key(Node **input_sentence, const int key) {
	char *character = (char *)malloc(sizeof(char));
	*character = (char)key;
	// Create new word if sentence is empty.
	if (*input_sentence == NULL) {
		Node *new_word = NULL;
		node_push(&new_word, character);
		node_push(input_sentence, new_word);
		return;
	}
	Node *last_word = node_tail(*input_sentence);
	node_push((Node **)&last_word->data, character);
}

void remove_input_key(Node **input_sentence) {
	// Skip if already empty.
	if (*input_sentence == NULL)
		return;
	// Otherwise remove from the last word.
	Node *last_word = node_tail(*input_sentence);
	// Remove the word if it is empty.
	if (last_word->data == NULL)
		node_pop(input_sentence);
	// Remove the last character otherwise.
	else {
		node_pop((Node **)&last_word->data);
	}

}

void add_input_word(Node **input_sentence) {
	// Add a space character if the current sentence is empty.
	if (*input_sentence == NULL || node_tail(*input_sentence)->data == NULL) {
		add_input_key(input_sentence, ' ');
		return;
	}
	// Add a new word.
	node_push(input_sentence, NULL);
}

size_t get_word_length(Node *t_word, Node *i_word) {
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

#endif
