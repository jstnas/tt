#ifndef TYPING_H
#define TYPING_H

#include "node.h"
#include "words.h"

Node *init_target_sentence(const size_t target_length);

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

#endif
