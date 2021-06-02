#ifndef TYPING_H
#define TYPING_H

#include "node.h"
#include "words.h"

static char allowed_keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
static size_t allowed_key_count = sizeof(allowed_keys) / sizeof(char);

bool is_key_allowed(const char key);
Node *init_target_sentence(const size_t target_length);
void add_input_key(Node **input_sentence, const int key);
void remove_input_key(Node **input_sentence);
void add_input_word(Node **input_sentence);

bool is_key_allowed(const char key) {
	for (size_t k = 0; k < allowed_key_count; k++) {
		if (key == allowed_keys[k])
			return true;
	}
	return false;
}

Node *init_target_sentence(const size_t target_length) {
	Node *target_sentence = NULL;
	size_t sentence_length = 0;

	while (sentence_length < target_length) {
		const size_t word_index = rand() % word_count;
		const size_t word_length = strlen(words[word_index]);
		// Stop if the word would be too long.
		const size_t new_length = sentence_length + word_length + 1;
		if (new_length > target_length)
			break;
		sentence_length += word_length;
		Node *new_word = NULL;
		for (size_t c = 0; c < word_length; c++)
			node_push(&new_word, &words[word_index][c]);
		node_push(&target_sentence, new_word);
	}

	return target_sentence;
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
	else
		node_pop((Node **)&last_word->data);

}

void add_input_word(Node **input_sentence) {
	// Skip if the sentence is empty.
	if (*input_sentence == NULL)
		return;
	// Prevent consecutive spaces.
	if (node_tail(*input_sentence)->data == NULL)
		return;
	// Add a new word.
	node_push(input_sentence, NULL);
}

#endif
