#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

struct node;
struct node *node_tail(struct node *head);
void node_push(struct node **head, void *data);
void node_pop(struct node **head);
int node_length(struct node *head);

struct node
{
	struct node *next;
	void *data;
};

struct node *node_tail(struct node *head)
{
	if (head == NULL)
	{
		return NULL;
	}
	// Get the last node in the list.
	struct node *current = head;
	while (current->next != NULL)
	{
		current = current->next;
	}
	return current;
}

void node_push(struct node **head, void *data)
{
	struct node *new_node = (struct node*)malloc(sizeof(struct node));
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
		struct node *last = node_tail(*head);
		last->next = new_node;
	}
}

void node_pop(struct node **head)
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
		struct node **current = head;
		struct node **previous = NULL;
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

int node_length(struct node *head)
{
	if (head == NULL)
	{
		return 0;
	}
	else
	{
		struct node *current = head;
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
