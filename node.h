#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

typedef struct Node Node;
struct Node
{
	Node *next;
	void *data;
};

Node *node_tail(Node *head);
void node_push(Node **head, void *data);
void node_pop(Node **head);
int node_length(Node *head);


Node *node_tail(Node *head)
{
	if (head == NULL)
	{
		return NULL;
	}
	// Get the last node in the list.
	Node *current = head;
	while (current->next != NULL)
	{
		current = current->next;
	}
	return current;
}

void node_push(Node **head, void *data)
{
	Node *new_node = (Node*)malloc(sizeof(Node));
	if (new_node == NULL)
	{
		return;
	}
	new_node->data = data;
	new_node->next = NULL;
	// Set the new node to the head if list is empty.
	if (*head == NULL)
	{
		*head = new_node;
	}
	// Add the new node to the end of the list.
	else
	{
		Node *last = node_tail(*head);
		last->next = new_node;
	}
}

void node_pop(Node **head)
{
	// Return if list is empty.
	if (*head == NULL)
	{
		return;
	}
	else if ((*head)->next == NULL)
	{
		free(*head);
		*head = NULL;
		return;
	}
	else
	{
		Node **current = head;
		Node **previous = NULL;
		while ((*current)->next != NULL)
		{
			previous = current;
			current = &(*current)->next;
		}
		(*previous)->next = NULL;
		free(*current);
		return;
	}
}

int node_length(Node *head)
{
	if (head == NULL)
	{
		return 0;
	}
	else
	{
		Node *current = head;
		int count = 1;
		while (current != NULL)
		{
			current = current->next;
			count++;
		}
		return count;
	}
}

#endif
