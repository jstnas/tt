#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

typedef struct Node Node;
struct Node {
	Node *next;
	void *data;
};

Node *node_tail(Node *);
Node *node_next(Node *);
void node_push(Node **, void *);
void node_pop(Node **);
void node_pop_front(Node **);
bool node_advance(Node **);
size_t node_length(Node *);


Node *node_tail(Node *head) {
	if (head == NULL)
		return NULL;
	// Get the last node in the list.
	Node *current = head;
	while (current->next != NULL)
		node_advance(&current);
	return current;
}

Node *node_next(Node *node) {
	return node == NULL ? NULL : node->next;
}

void node_push(Node **head, void *data) {
	Node *new_node = (Node *)malloc(sizeof(Node));
	if (new_node == NULL)
		return;
	new_node->data = data;
	new_node->next = NULL;
	// Set the new node to the head if list is empty.
	if (*head == NULL) {
		*head = new_node;
		return;
	}
	// Add the new node to the end of the list.
	Node *last = node_tail(*head);
	last->next = new_node;
}

// TODO: return data so it can be freed.
void node_pop(Node **head) {
	// Done if node is empty.
	if (*head == NULL)
		return;
	if ((*head)->next == NULL) {
		free(*head);
		*head = NULL;
		return;
	}
	Node **current = head;
	Node **previous = NULL;
	while ((*current)->next != NULL) {
		previous = current;
		current = &(*current)->next;
	}
	(*previous)->next = NULL;
	free(*current);
}

void node_pop_front(Node **head) {
	// Done if node is empty.
	if (*head == NULL)
		return;
	// Delete the head if it's the only node.
	if ((*head)->next == NULL) {
		free(*head);
		*head = NULL;
		return;
	}
	// Delete the first element and make the second element the head.
	Node *first = *head;
	*head = first->next;
	free(first);
	return;
}

bool node_advance(Node **node) {
	if (*node == NULL)
		return false;
	*node = (*node)->next;
	if (*node == NULL)
		return false;
	return true;
}

size_t node_length(Node *head) {
	size_t length = 0;
	Node *current = head;
	while (current != NULL) {
		node_advance(&current);
		length++;
	}
	return length;
}

#endif
