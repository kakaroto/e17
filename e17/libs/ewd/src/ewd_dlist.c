#include <Ewd.h>
#include <ewd_list_private.h>

/* 
 * Description: Create and initialize a new list.
 * Parameters: None
 * Returns: Returns a new initialized list.
 */
Ewd_DList *ewd_dlist_new()
{
	Ewd_DList *list = NULL;

	list = (Ewd_DList *)malloc(sizeof(Ewd_DList));
	if (!list)
		return NULL;

	if (!ewd_dlist_init(list)) {
		IF_FREE(list);
		return NULL;
	}

	return list;
}

/* 
 * Description: Initialize a list to some sane starting values.
 * Parameters: 1. list - the list to initialize
 * Returns: FALSE if an error occurs, TRUE if successful
 */
int ewd_dlist_init(Ewd_DList *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	memset(list, 0, sizeof(Ewd_DList));

	EWD_INIT_LOCKS(list);

	return TRUE;
}

/* 
 * Description: Free a list and all of it's nodes.
 * Parameters: 1. list - the list to be free'd
 * Returns: None
 */
void ewd_dlist_destroy(Ewd_DList * list)
{
	void *data;
	CHECK_PARAM_POINTER("list", list);

	EWD_WRITE_LOCK(list);

	while (list->first) {
		data = _ewd_dlist_remove_first(list);
		if (list->free_func)
			list->free_func(data);
	}

	EWD_WRITE_UNLOCK(list);
	EWD_DESTROY_LOCKS(list);

	FREE(list);
}

/*
 * Description: Add a function that will be called at node destroy time, the
 * 		function will be passed the value of the node to be destroyed.
 * Parameters: 1. list - the list that will use this function when nodes are
 *                       destroyed.
 *             2. free_func - the function that will be passed the value of
 *                            the node being freed.
 * Returns: TRUE on successful set, FALSE otherwise.
 */
int ewd_dlist_set_free_cb(Ewd_DList * list, Ewd_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return ewd_list_set_free_cb(EWD_LIST(list), free_func);
}

/*
 * Description: Checks the list for any nodes.
 * Parameters: 1. list - the list to check
 * Returns: TRUE if no nodes in list, FALSE if the list contains nodes
 */
int ewd_dlist_is_empty(Ewd_DList * list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return ewd_list_is_empty(EWD_LIST(list));
}

/*
 * Description: Returns the number of the current node
 * Parameters: 1. list - the list to return the number of the current node
 * Returns: The number of the current node in the list.
 */
int ewd_dlist_index(Ewd_DList * list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return ewd_list_index(EWD_LIST(list));
}

/*
 * Description: Find the number of nodes in the list.
 * Parameters: 1. list - the list to find the number of nodes
 * Returns: The number of nodes in the list.
 */
int ewd_dlist_nodes(Ewd_DList * list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return ewd_list_nodes(EWD_LIST(list));
}

/*
 * Description: Append data to the list.
 * Parameters: 1. list - the list to append the data
 *             2. data - the data to append to the list.
 * Returns: FALSE if an error occurs, TRUE if the data is appended successfully
 */
int ewd_dlist_append(Ewd_DList * list, void *data)
{
	int ret;
	Ewd_List_Node *prev;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);

	prev = EWD_LIST(list)->last;
	ret = _ewd_list_append(EWD_LIST(list), data);
	if (ret) {
		EWD_DLIST_NODE(EWD_LIST(list)->last)->previous =
			EWD_DLIST_NODE(prev);
	}

	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Prepend data to the beginning of the list
 * Parameters: 1. list - the list to prepend the data
 *             2. data - the data to prepend to the list
 * Returns: FALSE if an error occurs, TRUE if data prepended successfully
 */
int ewd_dlist_prepend(Ewd_DList * list, void *data)
{
	int ret;
	Ewd_List_Node *prev;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);

	prev = EWD_LIST(list)->first;
	ret = _ewd_list_prepend(EWD_LIST(list), data);
	if (ret)
		EWD_DLIST_NODE(prev)->previous =
			EWD_DLIST_NODE(EWD_LIST(list)->first);

	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Insert data at the current point in the list
 * Parameters: 1. list - the list to hold the inserted data
 *             2. data - the data to insert into the list
 * Returns: FALSE on an error, TRUE on success
 */
int ewd_dlist_insert(Ewd_DList * list, void *data)
{
	int ret;
	int index;
	Ewd_List_Node *current;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);

	index = ewd_list_index(EWD_LIST(list));
	current = EWD_LIST(list)->current;

	ret = _ewd_list_insert(list, data);
	if (ret && current) {
		_ewd_list_goto_index(EWD_LIST(list), index);
		EWD_DLIST_NODE(current)->previous =
			EWD_DLIST_NODE(EWD_LIST(list)->current);
		_ewd_list_goto_index(EWD_LIST(list), index + 1);
	}

	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Remove the current item from the list.
 * Parameters: 1. list - the list to remove the current item
 * Returns: TRUE on success, FALSE on error
 */
void *ewd_dlist_remove(Ewd_DList * list)
{
	void *ret;
	Ewd_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);

	if (list->current) {
		node = list->current->next;
		EWD_DLIST_NODE(node)->previous =
			EWD_DLIST_NODE(EWD_LIST(list)->current)->previous;
	}
	ret = _ewd_list_remove(list);

	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Remove the first item from the list.
 * Parameters: 1. list - the list to remove the current item
 * Returns: TRUE on success, FALSE on error
 */
void *ewd_dlist_remove_first(Ewd_DList * list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);
	ret = _ewd_dlist_remove_first(list);
	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Remove and destroy the data in the list at current position
 * Parameters: 1. list - the list to remove the data from
 * Returns: TRUE on success, FALSE on error
 */
int ewd_dlist_remove_destroy(Ewd_DList *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return ewd_list_remove_destroy(list);
}

void *_ewd_dlist_remove_first(Ewd_DList *list)
{
	void *ret;

	if (!list)
		return FALSE;

	ret = _ewd_list_remove_first(list);
	if (ret && EWD_LIST(list)->first)
		EWD_DLIST_NODE(EWD_LIST(list)->first)->previous = NULL;

	return ret;
}

/*
 * Description: Remove the last item from the list.
 * Parameters: 1. list - the list to remove the last node from
 * Returns: TRUE on success, FALSE on error
 */
void *ewd_dlist_remove_last(Ewd_DList * list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);
	ret = _ewd_list_remove_last(list);
	EWD_WRITE_UNLOCK(list);

	return ret;
}

/* 
 * Description: Move the current item to the index number
 * Parameters: 1. list - the list to move the current item
 *             2. index - the position to move the current item
 * Returns: TRUE on success, FALSE on error
 */
int ewd_dlist_goto_index(Ewd_DList * list, int index)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);
	ret = _ewd_dlist_goto_index(list, index);
	EWD_WRITE_UNLOCK(list);

	return ret;
}

/* This is the non-threadsafe version, use this inside internal functions that
 * already lock the list */
int _ewd_dlist_goto_index(Ewd_DList *list, int index)
{
	int i, increment;

	if (!list)
		return FALSE;

	if (ewd_list_is_empty(EWD_LIST(list)))
		return FALSE;

	if (index > ewd_list_nodes(EWD_LIST(list)) || index < 1)
		return FALSE;

	if (EWD_LIST(list)->index > EWD_LIST(list)->nodes)
		_ewd_list_goto_last(EWD_LIST(list));

	if (index < EWD_LIST(list)->index)
		increment = -1;
	else
		increment = 1;

	for (i = EWD_LIST(list)->index; i != index; i += increment) {
		if (increment > 0)
			_ewd_list_next(list);
		else
			_ewd_dlist_previous(list);
	}

	return TRUE;
}

/*
 * Description: Move the current item to the node that contains data
 * Parameters: 1. list - the list to move the current item in
 *             2. data - the data to find and set the current item to
 * Returns: TRUE on success, FALSE on error
 */
int ewd_dlist_goto(Ewd_DList * list, void *data)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);
	ret = _ewd_list_goto(EWD_LIST(list), data);
	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Move the current pointer to the first item in the list
 * Parameters: 1. list - the list to change the current to the first item
 * Returns: TRUE if successful, FALSE if error occurs
 */
int ewd_dlist_goto_first(Ewd_DList *list)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);
	ret = _ewd_list_goto_first(list);
	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Move the pointer to the current list item to the last item
 * Parameters: 1. list - the list to move the current item pointer to the last
 * Returns: TRUE on success, FALSE on an error
 */
int ewd_dlist_goto_last(Ewd_DList * list)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_WRITE_LOCK(list);
	ret = _ewd_list_goto_last(EWD_LIST(list));
	EWD_WRITE_UNLOCK(list);

	return ret;
}

/*
 * Description: Return the data in the current list item
 * Parameters: 1. list - the list to the return the current data
 * Returns: value of the current data item, NULL if no current item
 */
void *ewd_dlist_current(Ewd_DList * list)
{
	void *ret;

	EWD_READ_LOCK(list);
	ret = _ewd_list_current(EWD_LIST(list));
	EWD_READ_UNLOCK(list);

	return ret;
}

/*
 * Description: Move to the next item in the list and return current item
 * Parameters: 1. list - the list to move to the next item in.
 * Returns: data in the current list node, or NULL on error
 */
void *ewd_dlist_next(Ewd_DList * list)
{
	void *data;

	EWD_WRITE_LOCK(list);
	data = _ewd_list_next(list);
	EWD_WRITE_UNLOCK(list);

	return data;
}

/*
 * Description: Move to the previous item in the list and return current item
 * Parameters: 1. list - the list to move to the previous item in.
 * Returns: data in the current list node, or NULL on error
 */
void *ewd_dlist_previous(Ewd_DList * list)
{
	void *data;

	EWD_WRITE_LOCK(list);
	data = _ewd_dlist_previous(list);
	EWD_WRITE_UNLOCK(list);

	return data;
}

void *_ewd_dlist_previous(Ewd_DList * list)
{
	void *data = NULL;

	if (!list)
		return NULL;

	if (EWD_LIST(list)->current) {
		data = EWD_LIST(list)->current->data;
		EWD_LIST(list)->current = EWD_LIST_NODE(EWD_DLIST_NODE(
				EWD_LIST(list)->current)->previous);
		EWD_LIST(list)->index--;
	}
	else
		_ewd_list_goto_last(EWD_LIST(list));

	return data;
}

/*
 * Description: Execute function for each node in the list.
 * Parameters: 1. list - The list to retrieve nodes from.
 *             2. function - The function to pass each node from the list to.
 * Returns: FALSE if there is an error, TRUE otherwise.
 */
int ewd_dlist_for_each(Ewd_DList *list, Ewd_For_Each function)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	EWD_READ_LOCK(list);

	ret = _ewd_list_for_each(list, function);

	EWD_READ_UNLOCK(list);

	return ret;
}

/*
 * Description: Initialize a node to sane starting values
 * Parameters: 1. node - the node to initialize
 * Returns: TRUE on success, FALSE on errors
 */
int ewd_dlist_node_init(Ewd_DList_Node * node)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	ret = ewd_list_node_init(EWD_LIST_NODE(node));
	if (ret)
		node->previous = NULL;

	return ret;
}

/*
 * Description: Remove all nodes from the list.
 * Parameters: 1. list - the list to remove all nodes from
 * Returns: TRUE on success, FALSE on errors
 */
int ewd_dlist_clear(Ewd_DList * list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	ewd_list_clear(EWD_LIST(list));

	return TRUE;
}

/*
 * Description: Allocate and initialize a new list node
 * Parameters: None
 * Returns: NULL on error, new list node on success
 */
Ewd_DList_Node *ewd_dlist_node_new()
{
	Ewd_DList_Node *new;

	new = malloc(sizeof(Ewd_DList_Node));

	if (!new)
		return NULL;

	if (!ewd_dlist_node_init(new)) {
		FREE(new);
		return NULL;
	}

	return new;
}

/*
 * Description: Call the data's free callback function, then free the node
 * Parameters: 1. node - the node to be freed
 *             2. free_func - the callback function to execute on the data
 * Returns: TRUE on success, FALSE on error
 */
int ewd_dlist_node_destroy(Ewd_DList_Node * node, Ewd_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("node", node,
			FALSE);

	return ewd_list_node_destroy(EWD_LIST_NODE(node), free_func);
}
