#include <Ewd.h>

static int				ewd_list_node_init();

/* Creating and initializing new list structures */
Ewd_List *
ewd_list_new()
{
	Ewd_List *list = NULL;

	list = (Ewd_List *) malloc(sizeof(Ewd_List));

	if (!list)
		return NULL;

	memset(list, 0, sizeof(Ewd_List));

	return list;
}

/* Free the list and it's nodes */
void
ewd_list_destroy(Ewd_List * _ewd_list)
{
	Ewd_List_Node *node;

	if (!_ewd_list)
		return;

	node = _ewd_list->first;
	while (node) {
		Ewd_List_Node *old;

		old = node;
		node = node->next;
		ewd_list_node_destroy(old);
	}

	free(_ewd_list);
}

/* Check to see if the list has any data in it */
int
ewd_list_is_empty(Ewd_List * _ewd_list)
{
	if (!_ewd_list)
		return TRUE;

	if (_ewd_list->nodes)
		return FALSE;

	return TRUE;
}

/* Find the current position in the list */
int
ewd_list_index(Ewd_List * _ewd_list)
{
	if (!_ewd_list)
		return FALSE;

	return _ewd_list->index;
}

/* For adding items to the end of the list */
void
ewd_list_append(Ewd_List * _ewd_list, void *_data)
{
	Ewd_List_Node *end = NULL;

	if (!_ewd_list)
		return;

	end = ewd_list_node_new();
	if (!end)
		return;

	end->prev = _ewd_list->last;
	if (_ewd_list->last)
		_ewd_list->last->next = end;

	end->data = _data;
	_ewd_list->last = end;

	if (_ewd_list->first == NULL) {
		_ewd_list->first = _ewd_list->current = end;
		_ewd_list->index = 1;
	}

	_ewd_list->nodes++;
}

/* For adding items to the beginning of the list */
void
ewd_list_prepend(Ewd_List * _ewd_list, void *_data)
{
	Ewd_List_Node *start = NULL;

	if (!_ewd_list || !_data)
		return;

	/* Create a new node to add to the list */
	start = ewd_list_node_new();
	if (!start)
		return;

	/* Put it at the beginning of the list */
	start->next = _ewd_list->first;
	if (_ewd_list->first)
		_ewd_list->first->prev = start;

	start->data = _data;
	_ewd_list->first = start;

	/* If there's no current item selected, select the first one
	 * otherwise increment the index */
	if (!_ewd_list->current) {
		_ewd_list->current = _ewd_list->first;
		_ewd_list->index = 1;
	} else
		_ewd_list->index++;

	/* If no last node, then the first node is the last node */
	if (_ewd_list->last == NULL)
		_ewd_list->last = _ewd_list->first;

	_ewd_list->nodes++;
}

/* For adding items in front of the current position in the list */
void
ewd_list_insert(Ewd_List * _ewd_list, void *_data)
{
	Ewd_List_Node *new = NULL;

	if (!_ewd_list || !_data)
		return;

	/*
	 * If the current point is at the beginning of the list, then it's the
	 * same as prepending it to the list.
	 */
	if (_ewd_list->current == _ewd_list->first) {
		ewd_list_prepend(_ewd_list, _data);
		return;
	} else if (!_ewd_list->current) {
		ewd_list_append(_ewd_list, _data);
		return;
	}

	/* Create the node to insert into the list */
	new = ewd_list_node_new();
	if (!new)
		return;

	/* Setup the fields of the new node */
	new->data = _data;
	new->next = _ewd_list->current;
	new->prev = _ewd_list->current->prev;
	if (_ewd_list->current->prev)
		_ewd_list->current->prev->next = new;
	_ewd_list->current->prev = new;

	_ewd_list->index++;
	_ewd_list->nodes++;
}

/* Remove the current item from the list */
void
ewd_list_remove(Ewd_List * _ewd_list)
{
	Ewd_List_Node *old;

	if (!_ewd_list)
		return;

	if (ewd_list_is_empty(_ewd_list))
		return;

	if (!_ewd_list->current)
		return;

	if (_ewd_list->current == _ewd_list->first) {
		ewd_list_remove_first(_ewd_list);
		return;
	}

	if (_ewd_list->current == _ewd_list->last) {
		ewd_list_remove_last(_ewd_list);
		return;
	}

	_ewd_list->current->prev->next = _ewd_list->current->next;
	_ewd_list->current->next->prev = _ewd_list->current->prev;

	old = _ewd_list->current;
	_ewd_list->current = _ewd_list->current->next;

	ewd_list_node_destroy(old);
	_ewd_list->nodes--;
}

/* Remove the first item from the list */
void
ewd_list_remove_first(Ewd_List * list)
{
	Ewd_List_Node *old;

	if (!list)
		return;

	if (ewd_list_is_empty(list))
		return;

	if (!list->first)
		return;

	old = list->first;

	list->first = list->first->next;
	if (list->first)
		list->first->prev = NULL;

	if (list->current == old)
		list->current = list->first;
	else
		list->index--;

	if (list->last == old)
		list->last = list->first;

	ewd_list_node_destroy(old);
	list->nodes--;
}

/* Remove the last item from the list */
void
ewd_list_remove_last(Ewd_List * _ewd_list)
{
	Ewd_List_Node *old;

	if (!_ewd_list)
		return;

	if (ewd_list_is_empty(_ewd_list))
		return;

	if (!_ewd_list->last)
		return;

	old = _ewd_list->last;
	if (_ewd_list->last->prev)
		_ewd_list->last->prev = NULL;
	_ewd_list->last = _ewd_list->first->prev;

	ewd_list_node_destroy(old);
	_ewd_list->nodes--;
}

/* Set the current position to the node at index */
void
ewd_list_goto_index(Ewd_List * _ewd_list, int _index)
{
	int i;
	Ewd_List_Node *node;

	if (!_ewd_list)
		return;

	node = _ewd_list->first;
	for (i = 0; i < _index && node; i++, node = node->next);

	if (!node)
		return;

	_ewd_list->current = node;
	_ewd_list->index = _index;
}

/* Set the current position to the node containing data */
void
ewd_list_goto(Ewd_List * _ewd_list, void *_data)
{
	int index;
	Ewd_List_Node *node;

	if (!_ewd_list)
		return;

	index = 1;
	node = _ewd_list->first;
	while (node && node->data) {
		if (node->data == _data)
			break;
		node = node->next;
		index++;
	}

	if (!node)
		return;

	_ewd_list->current = node;
	_ewd_list->index = index;
}

/* Set the current position to the start of the list */
void
ewd_list_goto_first(Ewd_List * _ewd_list)
{
	if (!_ewd_list)
		return;

	_ewd_list->current = _ewd_list->first;
	_ewd_list->index = 1;
}

/* Set the current position to the end of the list */
void
ewd_list_goto_last(Ewd_List * _ewd_list)
{
	if (!_ewd_list)
		return;

	_ewd_list->current = _ewd_list->last;
	_ewd_list->index = _ewd_list->nodes;
}

/* Return the data of the current node without incrementing */
void *
ewd_list_current(Ewd_List * _ewd_list)
{
	if (!_ewd_list) return FALSE;

	if (!_ewd_list->current)
		return NULL;

	return _ewd_list->current->data;
}

/* Return the data contained in the current node and go to the next node */
void *
ewd_list_next(Ewd_List * _ewd_list)
{
	Ewd_List_Node *ret;

	if (!_ewd_list) return FALSE;

	if (!_ewd_list->current)
		return NULL;

	if (_ewd_list->current == _ewd_list->current->next)
		return NULL;

	ret = _ewd_list->current;
	_ewd_list->current = _ewd_list->current->next;
	_ewd_list->index++;

	return ret->data;
}

/* Return the data contained in the current node and go to the previous node */
void *
ewd_list_previous(Ewd_List * _ewd_list)
{
	Ewd_List_Node *ret;

	if (!_ewd_list) return FALSE;

	if (!_ewd_list->current)
		return NULL;

	ret = _ewd_list->current;
	_ewd_list->current = _ewd_list->current->prev;
	_ewd_list->index--;

	return ret->data;
}

/* Initialize a node to starting values */
static int
ewd_list_node_init(Ewd_List_Node * _ewd_node)
{
	if (!_ewd_node) return FALSE;
	_ewd_node->next = NULL;
	_ewd_node->prev = NULL;
	_ewd_node->data = NULL;

	return TRUE;
}

/* Remove all nodes from the list */
void
ewd_list_clear(Ewd_List * _ewd_list)
{
	if (!_ewd_list)
		return;

	if (ewd_list_is_empty(_ewd_list))
		return;

	ewd_list_goto_first(_ewd_list);
	while (_ewd_list->current)
		ewd_list_remove(_ewd_list);

	memset(_ewd_list, 0, sizeof(Ewd_List));
}

/* Allocate and initialize a new list node */
Ewd_List_Node *
ewd_list_node_new()
{
	Ewd_List_Node *new;

	new = malloc(sizeof(Ewd_List_Node));

	if (!new)
		return NULL;

	if (!ewd_list_node_init(new)) {
		free(new);
		return NULL;
	}

	return new;
}

void
ewd_list_node_destroy(Ewd_List_Node * _ewd_node)
{
	if (!_ewd_node) return;

	IF_FREE(_ewd_node);

	return;
}
