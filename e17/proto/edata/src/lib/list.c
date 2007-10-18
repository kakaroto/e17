#include "edata_private.h"
#include "Edata.h"

/* Some tests showed that beyond that value heap sort is faster than merge sort
 * (in this implementation). This value has to be changed or at least review
 * if someone is changing the implementation. */
#define EDATA_MERGESORT_LIMIT 40000

/* Return information about the list */
static void *_edata_list_current(Edata_List * list);

/* Adding functions */
static int _edata_list_insert(Edata_List * list, Edata_List_Node *node);
static int _edata_list_append_0(Edata_List * list, Edata_List_Node *node);
static int _edata_list_prepend_0(Edata_List * list, Edata_List_Node *node);

/* Remove functions */
static void *_edata_list_remove_0(Edata_List * list);
static void *_edata_list_first_remove(Edata_List * list);
static void *_edata_list_last_remove(Edata_List * list);

/* Basic traversal functions */
static void *_edata_list_next(Edata_List * list);
static void *_edata_list_last_goto(Edata_List * list);
static void *_edata_list_first_goto(Edata_List * list);
static void *_edata_list_goto(Edata_List * list, const void *data);
static void *_edata_list_index_goto(Edata_List *list, int index);

/* Iterative functions */
static int _edata_list_for_each(Edata_List *list, Edata_For_Each function, void *user_data);
static void *_edata_list_find(Edata_List *list, Edata_Compare_Cb function, const void *user_data);

/* Sorting functions */
static Edata_List_Node *_edata_list_node_mergesort(Edata_List_Node *first, int n, Edata_Compare_Cb compare, int order);
static Edata_List_Node *_edata_list_node_merge(Edata_List_Node *first, Edata_List_Node *second, Edata_Compare_Cb compare, int order);
static Edata_List_Node *_edata_dlist_node_mergesort(Edata_List_Node *first, int n, Edata_Compare_Cb compare, int order);
static Edata_List_Node *_edata_dlist_node_merge(Edata_List_Node *first, Edata_List_Node *second, Edata_Compare_Cb compare, int order);

/* Private double linked list functions */
static void *_edata_dlist_previous(Edata_DList * list);
static void *_edata_dlist_first_remove(Edata_DList *list);
static void *_edata_dlist_index_goto(Edata_DList *list, int index);

/* XXX: Begin deprecated code */
EAPI void * _edata_list2_append(void *in_list, void *in_item)
{
	Edata_List2 *l, *new_l;
	Edata_List2 *list, *item;

	list = in_list;
	item = in_item;
	new_l = item;
	new_l->next = NULL;
	if (!list)
	{
		new_l->prev = NULL;
		new_l->last = new_l;
		return new_l;
	}
	if (list->last)
		l = list->last;
	else
		for (l = list; l; l = l->next);
	l->next = new_l;
	new_l->prev = l;
	list->last = new_l;
	return list;
}

EAPI void * _edata_list2_prepend(void *in_list, void *in_item)
{
	Edata_List2 *new_l;
	Edata_List2 *list, *item;

	list = in_list;
	item = in_item;
	new_l = item;
	new_l->prev = NULL;
	if (!list)
	{
		new_l->next = NULL;
		new_l->last = new_l;
		return new_l;
	}
	new_l->next = list;
	list->prev = new_l;
	new_l->last = list->last;
	list->last = NULL;
	return new_l;
}

EAPI void * _edata_list2_append_relative(void *in_list, void *in_item,
		void *in_relative)
{
	Edata_List2 *l;
	Edata_List2 *list, *item, *relative;

	list = in_list;
	item = in_item;
	relative = in_relative;
	for (l = list; l; l = l->next)
	{
		if (l == relative)
		{
			Edata_List2 *new_l;

			new_l = item;
			if (l->next)
			{
				new_l->next = l->next;
				l->next->prev = new_l;
			}

			else
				new_l->next = NULL;
			l->next = new_l;
			new_l->prev = l;
			if (!new_l->next)
				list->last = new_l;
			return list;
		}
	}
	return _edata_list2_append(list, item);
}

EAPI void * _edata_list2_prepend_relative(void *in_list, void *in_item,
		void *in_relative)
{
	Edata_List2 *l;
	Edata_List2 *list, *item, *relative;

	list = in_list;
	item = in_item;
	relative = in_relative;
	for (l = list; l; l = l->next)
	{
		if (l == relative)
		{
			Edata_List2 *new_l;

			new_l = item;
			new_l->prev = l->prev;
			new_l->next = l;
			l->prev = new_l;
			if (new_l->prev)
			{
				new_l->prev->next = new_l;
				if (!new_l->next)
					list->last = new_l;
				return list;
			}
			else
			{
				if (!new_l->next)
					new_l->last = new_l;
				else
				{
					new_l->last = list->last;
					list->last = NULL;
				}
				return new_l;
			}
		}
	}
	return _edata_list2_prepend(list, item);
}

EAPI void * _edata_list2_remove(void *in_list, void *in_item)
{
	Edata_List2 *return_l;
	Edata_List2 *list, *item;

	/* checkme */
	if (!in_list)
		return in_list;

	list = in_list;
	item = in_item;
	if (!item)
		return list;
	if (item->next)
		item->next->prev = item->prev;
	if (item->prev)
	{
		item->prev->next = item->next;
		return_l = list;
	}
	else
	{
		return_l = item->next;
		if (return_l)
			return_l->last = list->last;
	}
	if (item == list->last)
		list->last = item->prev;
	item->next = NULL;
	item->prev = NULL;
	return return_l;
}

EAPI void * _edata_list2_find(void *in_list, void *in_item)
{
	Edata_List2 *l;
	Edata_List2 *list, *item;

	list = in_list;
	item = in_item;
	for (l = list; l; l = l->next)
	{
		if (l == item)
			return item;
	}
	return NULL;
}
/* XXX: End deprecated code */

/**
 @defgroup Edata_Data_List_Creation_Group List Creation/Destruction Functions

 Functions that create, initialize and destroy Edata_Lists.
 */

/**
 * Create and initialize a new list.
 * @return  A new initialized list on success, @c NULL on failure.
 * @ingroup Edata_Data_List_Creation_Group
 */
EAPI Edata_List *
edata_list_new()
{
	Edata_List *list;

	list = (Edata_List *)malloc(sizeof(Edata_List));
	if (!list)
	return NULL;

	if (!edata_list_init(list))
	{
		FREE(list);
		return NULL;
	}

	return list;
}

/**
 * Initialize a list to some sane starting values.
 * @param   list The list to initialize.
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup Edata_Data_List_Creation_Group
 */
EAPI int edata_list_init(Edata_List *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	memset(list, 0, sizeof(Edata_List));

	return TRUE;
}

/**
 * Free a list and all of it's nodes.
 * @param   list The list to be freed.
 * @ingroup Edata_Data_List_Creation_Group
 */
EAPI void edata_list_destroy(Edata_List *list)
{
	void *data;

	CHECK_PARAM_POINTER("list", list);

	while (list->first)
	{
		data = _edata_list_first_remove(list);
		if (list->free_func)
			list->free_func(data);
	}

	FREE(list);
}

/**
 * Set the function for freeing data.
 * @param  list      The list that will use this function when nodes are
 *                   destroyed.
 * @param  free_func The function that will free the key data.
 * @return @c TRUE on successful set, @c FALSE otherwise.
 */
EAPI int edata_list_free_cb_set(Edata_List *list, Edata_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	list->free_func = free_func;

	return TRUE;
}

/**
 * Checks the list for any nodes.
 * @param  list  The list to check for nodes
 * @return @c TRUE if no nodes in list, @c FALSE if the list contains nodes
 */
EAPI int edata_list_empty_is(Edata_List *list)
{
	int ret = TRUE;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	if (list->nodes)
		ret = FALSE;

	return ret;
}

/**
 * Returns the number of the current node.
 * @param  list The list to return the number of the current node.
 * @return The number of the current node in the list.
 */
EAPI int edata_list_index(Edata_List *list)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	ret = list->index;

	return ret;
}

/**
 * Find the number of nodes in the list.
 * @param  list The list to find the number of nodes
 * @return The number of nodes in the list.
 */
EAPI int edata_list_count(Edata_List *list)
{
	int ret = 0;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	ret = list->nodes;

	return ret;
}

/**
 @defgroup Edata_Data_List_Add_Item_Group List Item Adding Functions

 Functions that are used to add nodes to an Edata_List.
 */

/**
 * Append data to the list.
 * @param   list The list.
 * @param   data The data to append.
 * @return  @c FALSE if an error occurs, @c TRUE if appended successfully
 * @ingroup Edata_Data_List_Add_Item_Group
 */
EAPI inline int edata_list_append(Edata_List *list, void *data)
{
	int ret;
	Edata_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	node = edata_list_node_new();
	node->data = data;

	ret = _edata_list_append_0(list, node);

	return ret;
}

/* For adding items to the end of the list */
static int _edata_list_append_0(Edata_List *list, Edata_List_Node *end)
{
	if (list->last)
		list->last->next = end;

	list->last = end;

	if (list->first == NULL)
	{
		list->first = end;
		list->index = 0;
		list->current = NULL;
	}

	if (list->index >= list->nodes)
		list->index++;

	list->nodes++;

	return TRUE;
}

/**
 * Prepend data to the beginning of the list.
 * @param  list The list.
 * @param  data The data to prepend.
 * @return @c FALSE if an error occurs, @c TRUE if prepended successfully.
 * @ingroup Edata_Data_List_Add_Item_Group
 */
EAPI inline int edata_list_prepend(Edata_List *list, void *data)
{
	int ret;
	Edata_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	node = edata_list_node_new();
	node->data = data;

	ret = _edata_list_prepend_0(list, node);

	return ret;
}

/* For adding items to the beginning of the list */
static int _edata_list_prepend_0(Edata_List *list, Edata_List_Node *start)
{
	/* Put it at the beginning of the list */
	start->next = list->first;

	list->first = start;

	/* If no last node, then the first node is the last node */
	if (list->last == NULL)
		list->last = list->first;

	list->nodes++;
	list->index++;

	return TRUE;
}

/**
 * Insert data in front of the current point in the list.
 * @param   list The list to hold the inserted @p data.
 * @param   data The data to insert into @p list.
 * @return  @c FALSE if there is an error, @c TRUE on success
 * @ingroup Edata_Data_List_Add_Item_Group
 */
EAPI inline int edata_list_insert(Edata_List *list, void *data)
{
	int ret;
	Edata_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	node = edata_list_node_new();
	node->data = data;

	ret = _edata_list_insert(list, node);

	return ret;
}

/* For adding items in front of the current position in the list */
static int _edata_list_insert(Edata_List *list, Edata_List_Node *new_node)
{
	/*
	 * If the current point is at the beginning of the list, then it's the
	 * same as prepending it to the list.
	 */
	if (list->current == list->first)
		return _edata_list_prepend_0(list, new_node);

	if (list->current == NULL)
	{
		int ret_value;

		ret_value = _edata_list_append_0(list, new_node);
		list->current = list->last;

		return ret_value;
	}

	/* Setup the fields of the new node */
	new_node->next = list->current;

	/* And hook the node into the list */
	_edata_list_index_goto(list, edata_list_index(list) - 1);

	list->current->next = new_node;

	/* Now move the current item to the inserted item */
	list->current = new_node;
	list->nodes++;

	return TRUE;
}
/**
 * Append a list to the list.
 * @param   list The list.
 * @param   append The list to append.
 * @return  @c FALSE if an error occurs, @c TRUE if appended successfully
 * @ingroup Edata_Data_List_Add_Item_Group
 */

EAPI int edata_list_append_list(Edata_List *list, Edata_List *append)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);
	CHECK_PARAM_POINTER_RETURN("append", append, FALSE);

	if (edata_list_empty_is(append))
		return TRUE;

	if (edata_list_empty_is(list))
	{
		list->first = append->first;
		list->current = NULL;
		list->last = append->last;
		list->nodes = append->nodes;
	}
	else
	{
		list->last->next = append->first;
		list->last = append->last;
		list->nodes += append->nodes;
	}
	edata_list_init(append);
	return TRUE;
}

/**
 * Prepend a list to the beginning of the list.
 * @param  list The list.
 * @param  prepend The list to prepend.
 * @return @c FALSE if an error occurs, @c TRUE if prepended successfully.
 * @ingroup Edata_Data_List_Add_Item_Group
 */
EAPI int edata_list_prepend_list(Edata_List *list, Edata_List *prepend)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);
	CHECK_PARAM_POINTER_RETURN("prepend", prepend, FALSE);

	if (edata_list_empty_is(prepend))
		return TRUE;

	if (edata_list_empty_is(list))
	{
		list->first = prepend->first;
		list->current = NULL;
		list->last = prepend->last;
		list->nodes = prepend->nodes;
	}
	else
	{
		prepend->last->next = list->first;
		list->first = prepend->first;
		list->nodes += prepend->nodes;
		list->index += prepend->nodes;
	}
	edata_list_init(prepend);
	return TRUE;
}

/**
 @defgroup Edata_Data_List_Remove_Item_Group List Item Removing Functions

 Functions that remove nodes from an Edata_List.
 */

/**
 * Remove the current item from the list.
 * @param   list The list to remove the current item
 * @return  A pointer to the removed data on success, @c NULL on failure.
 * @ingroup Edata_Data_List_Remove_Item_Group
 */
EAPI inline void * edata_list_remove(Edata_List *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_remove_0(list);

	return ret;
}

/* Remove the current item from the list */
static void * _edata_list_remove_0(Edata_List *list)
{
	void *ret = NULL;
	Edata_List_Node *old;

	if (!list)
		return NULL;

	if (edata_list_empty_is(list))
		return NULL;

	if (!list->current)
		return NULL;

	if (list->current == list->first)
		return _edata_list_first_remove(list);

	if (list->current == list->last)
		return _edata_list_last_remove(list);

	old = list->current;

	_edata_list_index_goto(list, list->index - 1);

	list->current->next = old->next;
	old->next = NULL;
	ret = old->data;
	old->data = NULL;

	_edata_list_next(list);

	edata_list_node_destroy(old, NULL);
	list->nodes--;

	return ret;
}

/**
 * Remove and free the data in lists current position.
 * @param   list The list to remove and free the current item.
 * @return  @c TRUE on success, @c FALSE on error
 * @ingroup Edata_Data_List_Remove_Item_Group
 */
EAPI int edata_list_remove_destroy(Edata_List *list)
{
	void *data;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	data = _edata_list_remove_0(list);
	if (list->free_func)
		list->free_func(data);

	return TRUE;
}

/**
 * Remove the first item from the list.
 * @param   list The list to remove the current item
 * @return  Returns a pointer to the removed data on success, @c NULL on
 *          failure.
 * @ingroup Edata_Data_List_Remove_Item_Group
 */
EAPI inline void * edata_list_first_remove(Edata_List *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_first_remove(list);

	return ret;
}

/* Remove the first item from the list */
static void * _edata_list_first_remove(Edata_List *list)
{
	void *ret = NULL;
	Edata_List_Node *old;

	if (!list)
		return NULL;

	if (edata_list_empty_is(list))
		return NULL;

	old = list->first;

	list->first = list->first->next;

	if (list->current == old)
		list->current = list->first;
	else
		(list->index ? list->index-- : 0);

	if (list->last == old)
		list->last = list->first;

	ret = old->data;
	old->data = NULL;

	edata_list_node_destroy(old, NULL);
	list->nodes--;

	return ret;
}

/**
 * Remove the last item from the list.
 * @param   list The list to remove the last node from
 * @return  A pointer to the removed data on success, @c NULL on failure.
 * @ingroup Edata_Data_List_Remove_Item_Group
 */
EAPI inline void * edata_list_last_remove(Edata_List *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_last_remove(list);

	return ret;
}

/* Remove the last item from the list */
static void * _edata_list_last_remove(Edata_List *list)
{
	void *ret = NULL;
	Edata_List_Node *old, *prev;

	if (!list)
		return NULL;

	if (edata_list_empty_is(list))
		return NULL;

	old = list->last;
	if (list->current == old)
		list->current = NULL;

	if (list->first == old)
		list->first = NULL;
	for (prev = list->first; prev && prev->next != old; prev = prev->next);
	list->last = prev;
	if (prev)
		prev->next = NULL;

	old->next = NULL;
	ret = old->data;
	old->data = NULL;

	edata_list_node_destroy(old, NULL);
	list->nodes--;

	return ret;
}

/**
 @defgroup Edata_Data_List_Traverse_Group List Traversal Functions

 Functions that can be used to traverse an Edata_List.
 */

/**
 * Make the current item the item with the given index number.
 * @param   list  The list.
 * @param   index The position to move the current item.
 * @return  A pointer to new current item on success, @c NULL on failure.
 * @ingroup Edata_Data_List_Traverse_Group
 */
EAPI inline void * edata_list_index_goto(Edata_List *list, int index)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_index_goto(list, index);

	return ret;
}

/* This is the non-threadsafe version, use this inside internal functions that
 * already lock the list */
static void * _edata_list_index_goto(Edata_List *list, int index)
{
	int i;

	if (!list)
		return NULL;

	if (edata_list_empty_is(list))
		return NULL;

	if (index > edata_list_count(list) || index < 0)
		return NULL;

	if (index < list->index)
	{
		_edata_list_first_goto(list);
		i = 0;
	}
	else
		i = list->index;

	for (; i < index && _edata_list_next(list); i++);

	if (i >= list->nodes)
		return NULL;

	list->index = i;

	return list->current->data;
}

/**
 * Make the current item the node that contains @p data.
 * @param   list The list.
 * @param   data The data to find.
 * @return  A pointer to @p data on success, @c NULL on failure.
 * @ingroup Edata_Data_List_Traverse_Group
 */
EAPI inline void * edata_list_goto(Edata_List *list, const void *data)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_goto(list, data);

	return ret;
}

/* Set the current position to the node containing data */
static void * _edata_list_goto(Edata_List *list, const void *data)
{
	int index;
	Edata_List_Node *node;

	if (!list)
		return NULL;

	index = 0;

	node = list->first;
	while (node && node->data)
	{
		Edata_List_Node *next;

		if (node->data == data)
			break;

		next = node->next;

		node = next;

		index++;
	}

	if (!node)
		return NULL;

	list->current = node;
	list->index = index;

	return list->current->data;
}

/**
 * Make the current item the first item in the list
 * @param   list The list.
 * @return  A pointer to the first item on success, @c NULL on failure
 * @ingroup Edata_Data_List_Traverse_Group
 */
EAPI inline void * edata_list_first_goto(Edata_List *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_first_goto(list);

	return ret;
}

/* Set the current position to the start of the list */
static void * _edata_list_first_goto(Edata_List *list)
{
	if (!list || !list->first)
		return NULL;

	list->current = list->first;
	list->index = 0;

	return list->current->data;
}

/**
 * Make the current item the last item in the list.
 * @param   list The list.
 * @return  A pointer to the last item on success, @c NULL on failure.
 * @ingroup Edata_Data_List_Traverse_Group
 */
EAPI inline void * edata_list_last_goto(Edata_List *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_last_goto(list);

	return ret;
}

/* Set the current position to the end of the list */
static void * _edata_list_last_goto(Edata_List *list)
{
	if (!list || !list->last)
		return NULL;

	list->current = list->last;
	list->index = (list->nodes - 1);

	return list->current->data;
}

/**
 * Retrieve the data pointed to by the current item in @p list.
 * @param  list The list.
 * @return Returns the data at current position, can be @c NULL.
 */
EAPI inline void * edata_list_current(Edata_List *list)
{
	void *ret;

	ret = _edata_list_current(list);

	return ret;
}

/**
 * Retrieve the data pointed to by the first item in @p list.
 * @param  list The list.
 * @return Returns the data at current position, can be @c NULL.
 */
EAPI inline void * edata_list_first(Edata_List *list)
{
	void *ret;

	if (!list->first)
		return NULL;
	ret = list->first->data;

	return ret;
}

/**
 * Retrieve the data pointed to by the last item in @p list.
 * @param  list The list.
 * @return Returns the data at current position, can be @c NULL.
 */
EAPI inline void * edata_list_last(Edata_List *list)
{
	void *ret;

	if (!list->last)
		return NULL;
	ret = list->last->data;

	return ret;
}

/* Return the data of the current node without incrementing */
static void * _edata_list_current(Edata_List *list)
{
	void *ret;

	if (!list->current)
		return NULL;

	ret = list->current->data;

	return ret;
}

/**
 * Retrieve the data pointed to by the current item, and make the next item
 * the current item.
 * @param   list The list to retrieve data from.
 * @return  The current item in the list on success, @c NULL on failure.
 */
EAPI inline void * edata_list_next(Edata_List *list)
{
	void *data;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	data = _edata_list_next(list);

	return data;
}

/* Return the data contained in the current node and go to the next node */
static void * _edata_list_next(Edata_List *list)
{
	void *data;
	Edata_List_Node *ret;
	Edata_List_Node *next;

	if (!list->current)
		return NULL;

	ret = list->current;
	next = list->current->next;

	list->current = next;
	list->index++;

	data = ret->data;

	return data;
}

/**
 * Remove all nodes from @p list.
 * @param  list The list.
 * @return Returns @c TRUE on success, @c FALSE on error.
 * @note The data for each item on the list is not freed by
 *       @c edata_list_clear().
 */
EAPI int edata_list_clear(Edata_List *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	while (!edata_list_empty_is(list))
		_edata_list_first_remove(list);

	return TRUE;
}

/**
 * Execute function for each node in @p list.
 * @param   list     The list.
 * @param   function The function to pass each node from @p list to.
 * @return  Returns @c TRUE on success, @c FALSE on failure.
 * @ingroup Edata_Data_List_Traverse_Group
 */
EAPI int edata_list_for_each(Edata_List *list, Edata_For_Each function,
		void *user_data)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	ret = _edata_list_for_each(list, function, user_data);

	return ret;
}

/* The real meat of executing the function for each data node */
static int _edata_list_for_each(Edata_List *list, Edata_For_Each function,
		void *user_data)
{
	void *value;

	if (!list || !function)
		return FALSE;

	_edata_list_first_goto(list);
	while ((value = _edata_list_next(list)) != NULL)
		function(value, user_data);

	return TRUE;
}

/**
 * Find data in @p list using the compare function @p func
 * @param list      The list.
 * @param function  The function to test each node of @p list with
 * @param user_data Data to match against (used by @p function)
 * @return the first matching data node, or NULL if none match
 */
EAPI void * edata_list_find(Edata_List *list, Edata_Compare_Cb function,
		const void *user_data)
{
	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	return _edata_list_find(list, function, user_data);
}

/* The real meat of finding a node via a compare cb */
static void * _edata_list_find(Edata_List *list, Edata_Compare_Cb function,
		const void *user_data)
{
	void *value;
	if (!list || !function)
		return NULL;

	_edata_list_first_goto(list);
	while ((value = _edata_list_current(list)) != NULL)
	{
		if (!function(value, user_data))
			return value;
		edata_list_next(list);
	}

	return NULL;
}

/**
 * Sort data in @p list using the compare function @p compare
 * @param list      The list.
 * @param compare   The function to compare the data of @p list
 * @param order     The sort direction, possible values are EDATA_SORT_MIN and
 *                  EDATA_SORT_MAX
 * @return          true on success
 *
 * This is a wrapper function for mergesort and heapsort. It
 * tries to choose the fastest algorithm depending on the
 * number of notes. Note: The sort may be unstable.
 */
EAPI int edata_list_sort(Edata_List *list, Edata_Compare_Cb compare, char order)
{
	CHECK_PARAM_POINTER_RETURN("list", list, 0);

	if (list->nodes < 2)
		return 1;
	if (list->nodes < EDATA_MERGESORT_LIMIT)
		return edata_list_mergesort(list, compare, order);
	if (!edata_list_heapsort(list, compare, order))
		return edata_list_mergesort(list, compare, order);

	return 1;
}

/**
 * Sort data in @p list using the compare function @p compare
 * @param list      The list.
 * @param compare   The function to compare the data of @p list
 * @param order     The sort direction, possible values are EDATA_SORT_MIN and
 *                  EDATA_SORT_MAX
 * @return          true on success
 *
 * Mergesort is a stable, in-place sorting algorithm 
 */
EAPI int edata_list_mergesort(Edata_List *list, Edata_Compare_Cb compare,
		char order)
{
	Edata_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, 0);
	if (list->nodes < 2)
		return 1;

	if (order == EDATA_SORT_MIN)
		order = 1;
	else
		order = -1;

	node = _edata_list_node_mergesort(list->first, list->nodes, compare, order);
	list->first = node;

	/* maybe there is a better way to do that but our last node has changed */
	while (node->next)
		node = node->next;
	list->last = node;

	_edata_list_first_goto(list);

	return 1;
}

/* this is the internal recrusive function for the merge sort */
static Edata_List_Node * _edata_list_node_mergesort(Edata_List_Node *first,
		int n, Edata_Compare_Cb compare, int order)
{
	Edata_List_Node *middle;
	Edata_List_Node *premid;
	int mid;
	int i;

	mid = n / 2;

	if (n < 2)
		return first;
	else if (n == 2)
	{
		if (compare(first->data, first->next->data) * order > 0)
		{
			/* swap the data */
			void *data;
			data = first->next->data;
			first->next->data = first->data;
			first->data = data;
		}
		return first;
	}

	/* first find the premiddle node*/
	for (premid = first, i = 0; i < mid - 1; i++)
		premid = premid->next;

	/* split the list */
	middle = premid->next;
	premid->next = NULL;

	/* sort the the partial lists */
	first = _edata_list_node_mergesort(first, mid, compare, order);
	middle = _edata_list_node_mergesort(middle, n - mid, compare, order);

	return _edata_list_node_merge(first, middle, compare, order);
}

/* this function is used to merge the partial sorted lists */
static Edata_List_Node * _edata_list_node_merge(Edata_List_Node *first,
		Edata_List_Node *second, Edata_Compare_Cb compare, int order)
{
	Edata_List_Node *list;
	Edata_List_Node *l;

	/* select the first node outside the loop, because we need to keep
	 * a pointer to it */
	if (compare(first->data, second->data) * order > 0)
	{
		list = l = second;
		second = second->next;
	}
	else
	{
		list = l = first;
		first = first->next;
	}

	/* and now start the merging */
	while (first && second)
	{
		if (compare(first->data, second->data) * order > 0)
		{
			l = l->next = second;
			second = second->next;
		}
		else
		{
			l = l->next = first;
			first = first->next;
		}
	}

	/* append the rest or set it to NULL */
	if (first)
		l->next = first;
	else if (second)
		l->next = second;
	else
		l->next = NULL;

	return list;
}

/**
 * Sort data in @p list using the compare function @p compare
 * @param list      The list.
 * @param compare   The function to compare the data of @p list
 * @param order     The sort direction, possible values are EDATA_SORT_MIN and
 *                  EDATA_SORT_MAX
 * @return          true on success
 *
 * Heapsort is a unstable sorting algorithm, it needs to allocate extra memomry,
 * but there for it is for a great number of nodes faster than mergesort
 */
EAPI int edata_list_heapsort(Edata_List *list, Edata_Compare_Cb compare,
		char order)
{
	Edata_Sheap *heap;
	Edata_List_Node *node;
	void *data;

	CHECK_PARAM_POINTER_RETURN("list", list, 0);
	/*
	 * Push the data into a heap.
	 */
	heap = edata_sheap_new(compare, list->nodes);
	if (!heap)
		return 0;

	edata_sheap_order_set(heap, order);
	_edata_list_first_goto(list);
	while ((data = _edata_list_next(list)))
	{
		edata_sheap_insert(heap, data);
	}

	/*
	 * Extract in sorted order.
	 */
	node = list->first;
	while (node)
	{
		node->data = edata_sheap_extract(heap);
		node = node->next;
	}

	edata_sheap_destroy(heap);

	_edata_list_first_goto(list);
	return 1;
}

/* Initialize a node to starting values */
EAPI int edata_list_node_init(Edata_List_Node *node)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	node->next = NULL;
	node->data = NULL;

	return TRUE;
}

/**
 @defgroup Edata_Data_List_Node_Group List Node Functions

 Functions that are used in the creation, maintenance and destruction of
 Edata_List nodes.
 */

/**
 * Allocates and initializes a new list node.
 * @return  A new Edata_List_Node on success, @c NULL otherwise.
 * @ingroup Edata_Data_List_Node_Group
 */
EAPI Edata_List_Node *
edata_list_node_new()
{
	Edata_List_Node *new_node;

	new_node = malloc(sizeof(Edata_List_Node));

	if (!edata_list_node_init(new_node))
	{
		FREE(new_node);
		return NULL;
	}

	return new_node;
}

/**
 * Calls the function to free the data and the node.
 * @param   node      Node to destroy.
 * @param   free_func Function to call if @p node points to data to free.
 * @return  @c TRUE.
 * @ingroup Edata_Data_List_Node_Group
 */
EAPI int edata_list_node_destroy(Edata_List_Node *node, Edata_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	if (free_func && node->data)
		free_func(node->data);

	FREE(node);

	return TRUE;
}

/**
 * @defgroup Edata_Data_DList_Creation_Group Doubly Linked List Creation/Destruction Functions
 *
 * Functions used to create, initialize and destroy @c Edata_DLists.
 */

/**
 * Creates and initialises a new doubly linked list.
 * @return  A new initialised doubly linked list on success, @c NULL
 *          on failure.
 * @ingroup Edata_Data_DList_Creation_Group
 */
EAPI Edata_DList *
edata_dlist_new()
{
	Edata_DList *list = NULL;

	list = (Edata_DList *)malloc(sizeof(Edata_DList));
	if (!list)
	return NULL;

	if (!edata_dlist_init(list))
	{
		IF_FREE(list);
		return NULL;
	}

	return list;
}

/**
 * Initialises a list to some sane starting values.
 * @param   list The doubly linked list to initialise.
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup Edata_Data_DList_Creation_Group
 */
EAPI int edata_dlist_init(Edata_DList *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	memset(list, 0, sizeof(Edata_DList));

	return TRUE;
}

/**
 * Frees a doubly linked list and all of its nodes.
 * @param   list The doubly linked list to be freed.
 * @ingroup Edata_Data_DList_Creation_Group
 */
EAPI void edata_dlist_destroy(Edata_DList *list)
{
	void *data;
	CHECK_PARAM_POINTER("list", list);

	while (list->first)
	{
		data = _edata_dlist_first_remove(list);
		if (list->free_func)
			list->free_func(data);
	}

	FREE(list);
}

/**
 * Sets the function used for freeing data stored in a doubly linked list.
 * @param   list      The doubly linked list that will use this function when
 *                    nodes are destroyed.
 * @param   free_func The function that will free the key data
 * @return  @c TRUE on success, @c FALSE on failure.
 * @ingroup Edata_Data_DList_Creation_Group
 */
EAPI int edata_dlist_free_cb_set(Edata_DList *list, Edata_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return edata_list_free_cb_set(EDATA_LIST(list), free_func);
}

/**
 * Returns whether there is anything in the given doubly linked list.
 * @param  list The given doubly linked list.
 * @return @c TRUE if there are nodes, @c FALSE otherwise.
 */
EAPI int edata_dlist_empty_is(Edata_DList *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return edata_list_empty_is(EDATA_LIST(list));
}

/**
 * Retrieves the index of the current node of the given doubly linked list.
 * @param  list The given doubly linked list.
 * @return The index of the current node.
 */
EAPI inline int edata_dlist_index(Edata_DList *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	return edata_list_index(EDATA_LIST(list));
}

/**
 * @defgroup Edata_Data_DList_Add_Item_Group Doubly Linked List Adding Functions
 *
 * Functions that are used to add nodes to an Edata_DList.
 */

/**
 * Appends data to the given doubly linked list.
 * @param   list The given doubly linked list.
 * @param   data The data to append.
 * @return  @c TRUE if the data is successfully appended, @c FALSE otherwise.
 * @ingroup Edata_Data_DList_Add_Item_Group
 */
EAPI int edata_dlist_append(Edata_DList *list, void *data)
{
	int ret;
	Edata_DList_Node *prev;
	Edata_DList_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	node = edata_dlist_node_new();
	EDATA_LIST_NODE(node)->data = data;

	prev = EDATA_DLIST_NODE(EDATA_LIST(list)->last);
	ret = _edata_list_append_0(EDATA_LIST(list), EDATA_LIST_NODE(node));
	if (ret)
		node->previous = prev;

	return ret;
}

/**
 * Adds data to the very beginning of the given doubly linked list.
 * @param   list The given doubly linked list.
 * @param   data The data to prepend.
 * @return  @c TRUE if the data is successfully prepended, @c FALSE otherwise.
 * @ingroup Edata_Data_DList_Add_Item_Group
 */
EAPI int edata_dlist_prepend(Edata_DList *list, void *data)
{
	int ret;
	Edata_DList_Node *prev;
	Edata_DList_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	node = edata_dlist_node_new();
	EDATA_LIST_NODE(node)->data = data;

	prev = EDATA_DLIST_NODE(EDATA_LIST(list)->first);
	ret = _edata_list_prepend_0(EDATA_LIST(list), EDATA_LIST_NODE(node));
	if (ret && prev)
		prev->previous = node;

	return ret;
}

/**
 * Inserts data at the current point in the given doubly linked list.
 * @param   list The given doubly linked list.
 * @param   data The data to be inserted.
 * @return  @c TRUE on success, @c FALSE otherwise.
 * @ingroup Edata_Data_DList_Add_Item_Group
 */
EAPI int edata_dlist_insert(Edata_DList *list, void *data)
{
	int ret = TRUE;
	Edata_DList_Node *prev;
	Edata_DList_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	/*
	 * Identify and shortcut the end cases.
	 */
	if (!EDATA_LIST(list)->current)
		return edata_dlist_append(list, data);
	if (EDATA_LIST(list)->current == EDATA_LIST(list)->first)
		return edata_dlist_prepend(list, data);

	node = edata_dlist_node_new();
	EDATA_LIST_NODE(node)->data = data;

	/* Setup the fields of the new node */
	EDATA_LIST_NODE(node)->next = EDATA_LIST(list)->current;

	/* And hook the node into the list */
	prev = EDATA_DLIST_NODE(EDATA_LIST(list)->current)->previous;
	EDATA_LIST_NODE(prev)->next = EDATA_LIST_NODE(node);
	EDATA_DLIST_NODE(EDATA_LIST(list)->current)->previous = node;
	node->previous = prev;

	/* Now move the current item to the inserted item */
	EDATA_LIST(list)->current = EDATA_LIST_NODE(node);
	EDATA_LIST(list)->nodes++;

	return ret;
}

/**
 * Appends a list to the given doubly linked list.
 * @param   list The given doubly linked list.
 * @param   append The list to append.
 * @return  @c TRUE if the data is successfully appended, @c FALSE otherwise.
 * @ingroup Edata_Data_DList_Add_Item_Group
 */
EAPI int edata_dlist_append_list(Edata_DList *list, Edata_DList *append)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);
	CHECK_PARAM_POINTER_RETURN("append", append, FALSE);

	if (edata_dlist_empty_is(append))
		return TRUE;

	if (edata_dlist_empty_is(list))
	{
		list->first = append->first;
		list->current = NULL;
		list->last = append->last;
		list->nodes = append->nodes;
	}
	else
	{
		list->last->next = append->first;
		EDATA_DLIST_NODE(append->first)->previous = EDATA_DLIST_NODE(list->last);
		list->last = append->last;
		list->nodes += append->nodes;
	}
	edata_dlist_init(append);
	return TRUE;
}

/**
 * Adds a list to the very beginning of the given doubly linked list.
 * @param   list The given doubly linked list.
 * @param   prepend The list to prepend.
 * @return  @c TRUE if the data is successfully prepended, @c FALSE otherwise.
 * @ingroup Edata_Data_DList_Add_Item_Group
 */
EAPI int edata_dlist_prepend_list(Edata_DList *list, Edata_DList *prepend)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);
	CHECK_PARAM_POINTER_RETURN("prepend", prepend, FALSE);

	if (edata_dlist_empty_is(prepend))
		return TRUE;

	if (edata_dlist_empty_is(list))
	{
		list->first = prepend->first;
		list->current = NULL;
		list->last = prepend->last;
		list->nodes = prepend->nodes;
	}
	else
	{
		prepend->last->next = list->first;
		EDATA_DLIST_NODE(list->first)->previous = EDATA_DLIST_NODE(prepend->last);
		list->first = prepend->first;
		list->nodes += prepend->nodes;
		list->index += prepend->nodes;
	}
	edata_dlist_init(prepend);
	return TRUE;
}

/**
 * @defgroup Edata_Data_DList_Remove_Item_Group Doubly Linked List Removing Functions
 *
 * Functions that remove nodes from an @c Edata_DList.
 */

/**
 * Removes the current item from the given doubly linked list.
 * @param   list The given doubly linked list.
 * @return  A pointer to the removed data on success, @c NULL otherwise.
 * @ingroup Edata_Data_DList_Remove_Item_Group
 */
EAPI void * edata_dlist_remove(Edata_DList *list)
{
	void *ret;
	Edata_List *l2 = EDATA_LIST(list);
	Edata_DList_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	if (l2->current)
	{
		node = EDATA_DLIST_NODE(list->current->next);
		if (node)
			node->previous = EDATA_DLIST_NODE(l2->current)->previous;
	}
	ret = _edata_list_remove_0(list);

	return ret;
}

/**
 * Removes the first item from the given doubly linked list.
 * @param   list The given doubly linked list.
 * @return  A pointer to the removed data on success, @c NULL on failure.
 * @ingroup Edata_Data_DList_Remove_Item_Group
 */
EAPI void * edata_dlist_first_remove(Edata_DList *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_dlist_first_remove(list);

	return ret;
}

/**
 * Removes and frees the data at the current position in the given doubly
 * linked list.
 * @param   list The given doubly linked list.
 * @return  @c TRUE on success, @c FALSE otherwise.
 * @ingroup Edata_Data_DList_Remove_Item_Group
 */
EAPI int edata_dlist_remove_destroy(Edata_DList *list)
{
	void *data;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	data = edata_dlist_remove(list);
	if (!data)
		return FALSE;

	if (list->free_func)
		list->free_func(data);

	return TRUE;
}

static void * _edata_dlist_first_remove(Edata_DList *list)
{
	void *ret;

	if (!list)
		return NULL;

	ret = _edata_list_first_remove(list);
	if (ret && EDATA_LIST(list)->first)
		EDATA_DLIST_NODE(EDATA_LIST(list)->first)->previous = NULL;

	return ret;
}

/**
 * Removes the last item from the given doubly linked list.
 * @param   list The given doubly linked list.
 * @return  A pointer to the removed data on success, @c NULL otherwise.
 * @ingroup Edata_Data_DList_Remove_Item_Group
 */
EAPI void * edata_dlist_last_remove(Edata_DList *list)
{
	void *ret;
	Edata_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	if (edata_list_empty_is(list))
		return NULL;

	node = list->last;
	list->last = EDATA_LIST_NODE(EDATA_DLIST_NODE(node)->previous);
	if (list->last)
		list->last->next = NULL;
	if (list->first == node)
		list->first = NULL;
	if (list->current == node)
		list->current = NULL;

	ret = node->data;
	edata_list_node_destroy(node, NULL);

	list->nodes--;
	if (list->index >= list->nodes)
		list->index--;

	return ret;
}

/**
 * Moves the current item to the index number in the given doubly linked list.
 * @param  list  The given doubly linked list.
 * @param  index The position to move the current item
 * @return The node at specified index on success, @c NULL on error.
 */
EAPI void * edata_dlist_index_goto(Edata_DList *list, int index)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_dlist_index_goto(list, index);

	return ret;
}

/* This is the non-threadsafe version, use this inside internal functions that
 * already lock the list */
static void * _edata_dlist_index_goto(Edata_DList *list, int index)
{
	int i, increment;

	if (!list)
		return NULL;

	if (edata_list_empty_is(EDATA_LIST(list)))
		return NULL;

	if (index > edata_list_count(EDATA_LIST(list)) || index < 0)
		return NULL;

	if (EDATA_LIST(list)->index >= EDATA_LIST(list)->nodes)
		_edata_list_last_goto(EDATA_LIST(list));

	if (index < EDATA_LIST(list)->index)
		increment = -1;
	else
		increment = 1;

	for (i = EDATA_LIST(list)->index; i != index; i += increment)
	{
		if (increment > 0)
			_edata_list_next(list);
		else
			_edata_dlist_previous(list);
	}

	return _edata_list_current(list);
}

/**
 * @brief Move the current item to the node that contains data
 * @param list: the list to move the current item in
 * @param data: the data to find and set the current item to
 *
 * @return Returns specified data on success, NULL on error
 */
EAPI void * edata_dlist_goto(Edata_DList *list, void *data)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_goto(EDATA_LIST(list), data);

	return ret;
}

/**
 * @brief Move the current pointer to the first item in the list
 * @param list: the list to change the current to the first item
 *
 * @return Returns a pointer to the first item on success, NULL on failure.
 */
EAPI void * edata_dlist_first_goto(Edata_DList *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_first_goto(list);

	return ret;
}

/**
 * @brief Move the pointer to the current item to the last item
 * @param list: the list to move the current item pointer to the last
 * @return Returns a pointer to the last item in the list , NULL if empty.
 */
EAPI void * edata_dlist_last_goto(Edata_DList *list)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("list", list, NULL);

	ret = _edata_list_last_goto(EDATA_LIST(list));

	return ret;
}

/**
 * @brief Return the data in the current list item
 * @param list: the list to the return the current data
 * @return Returns value of the current data item, NULL if no current item
 */
EAPI void * edata_dlist_current(Edata_DList *list)
{
	void *ret;

	ret = _edata_list_current(EDATA_LIST(list));

	return ret;
}

/**
 * @brief Move to the next item in the list and return current item
 * @param list: the list to move to the next item in.
 * @return Returns data in the current list node, or NULL on error
 */
EAPI void * edata_dlist_next(Edata_DList *list)
{
	void *data;

	data = _edata_list_next(list);

	return data;
}

/**
 * @brief Move to the previous item and return current item
 * @param list: the list to move to the previous item in.
 * @return Returns data in the current list node, or NULL on error
 */
EAPI void * edata_dlist_previous(Edata_DList *list)
{
	void *data;

	data = _edata_dlist_previous(list);

	return data;
}

static void * _edata_dlist_previous(Edata_DList *list)
{
	void *data = NULL;

	if (!list)
		return NULL;

	if (EDATA_LIST(list)->current)
	{
		data = EDATA_LIST(list)->current->data;
		EDATA_LIST(list)->current = EDATA_LIST_NODE(EDATA_DLIST_NODE(
				EDATA_LIST(list)->current)->previous);
		EDATA_LIST(list)->index--;
	}
	else
		_edata_list_last_goto(EDATA_LIST(list));

	return data;
}

/**
 * @brief Remove all nodes from the list.
 * @param list: the list to remove all nodes from
 *
 * @return Returns TRUE on success, FALSE on errors
 */
EAPI int edata_dlist_clear(Edata_DList *list)
{
	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	edata_list_clear(EDATA_LIST(list));

	return TRUE;
}

/**
 * Sort data in @p list using the compare function @p compare
 * @param list      The list.
 * @param compare   The function to compare the data of @p list
 * @param order     The sort direction, possible values are EDATA_SORT_MIN and
 *                  EDATA_SORT_MAX
 * @return          true on success
 *
 * This is a wrapper function for mergesort and heapsort. It
 * tries to choose the fastest algorithm depending on the
 * number of notes. Note: The sort may be unstable.
 */
EAPI int edata_dlist_sort(Edata_List *list, Edata_Compare_Cb compare, char order)
{
	CHECK_PARAM_POINTER_RETURN("list", list, 0);

	if (list->nodes < 2)
		return 1;
	if (list->nodes < EDATA_MERGESORT_LIMIT)
		return edata_dlist_mergesort(list, compare, order);
	if (!edata_dlist_heapsort(list, compare, order))
		return edata_dlist_mergesort(list, compare, order);

	return 1;
}

/**
 * Sort data in @p list using the compare function @p compare
 * @param list      The list.
 * @param compare   The function to compare the data of @p list
 * @param order     The sort direction, possible values are EDATA_SORT_MIN and
 *                  EDATA_SORT_MAX
 * @return          true on success
 *
 * Mergesort is a stable, in-place sorting algorithm 
 */
EAPI int edata_dlist_mergesort(Edata_DList *list, Edata_Compare_Cb compare,
		char order)
{
	Edata_List_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, 0);
	if (list->nodes < 2)
		return 1;

	if (order == EDATA_SORT_MIN)
		order = 1;
	else
		order = -1;

	node
			= _edata_dlist_node_mergesort(list->first, list->nodes, compare,
					order);
	list->first = node;

	/* maybe there is a better way to do that but our last node has changed */
	while (node->next)
		node = node->next;
	list->last = node;

	_edata_list_first_goto(list);

	return 1;
}

/* this is the internal recrusive function for the merge sort */
static Edata_List_Node * _edata_dlist_node_mergesort(Edata_List_Node *first,
		int n, Edata_Compare_Cb compare, int order)
{
	Edata_List_Node *middle;
	Edata_List_Node *premid;
	int mid;
	int i;

	mid = n/2;

	if (n < 2)
		return first;
	else if (n == 2)
	{
		if (compare(first->data, first->next->data) * order > 0)
		{
			/* swap the data */
			void *data;
			data = first->next->data;
			first->next->data = first->data;
			first->data = data;
		}
		return first;
	}

	/* first find the premiddle node*/
	for (premid = first, i = 0; i < mid - 1; i++)
		premid = premid->next;

	/* split the list */
	middle = premid->next;
	premid->next = NULL;
	EDATA_DLIST_NODE(middle)->previous = NULL;

	/* sort the the partial lists */
	first = _edata_dlist_node_mergesort(first, mid, compare, order);
	middle = _edata_dlist_node_mergesort(middle, n - mid, compare, order);

	return _edata_dlist_node_merge(first, middle, compare, order);
}

/* this function is used to merge the partial sorted lists */
static Edata_List_Node * _edata_dlist_node_merge(Edata_List_Node *first,
		Edata_List_Node *second, Edata_Compare_Cb compare, int order)
{
	Edata_List_Node *list;
	Edata_List_Node *l;

	/* select the first node outside the loop, because we need to keep
	 * a pointer to it */
	if (compare(first->data, second->data) * order > 0)
	{
		list = l = second;
		second = second->next;
	}
	else
	{
		list = l = first;
		first = first->next;
	}

	/* and now start the merging */
	while (first && second)
	{
		if (compare(first->data, second->data) * order > 0)
		{
			EDATA_DLIST_NODE(second)->previous = EDATA_DLIST_NODE(l);
			l = l->next = second;
			second = second->next;
		}
		else
		{
			EDATA_DLIST_NODE(first)->previous = EDATA_DLIST_NODE(l);
			l = l->next = first;
			first = first->next;
		}
	}

	/* append the rest or set it to NULL */
	if (first)
	{
		EDATA_DLIST_NODE(first)->previous = EDATA_DLIST_NODE(l);
		l->next = first;
	}
	else if (second)
	{
		EDATA_DLIST_NODE(second)->previous = EDATA_DLIST_NODE(l);
		l->next = second;
	}
	else
		l->next = NULL;

	return list;
}

/*
 * @brief Initialize a node to sane starting values
 * @param node: the node to initialize
 * @return Returns TRUE on success, FALSE on errors
 */
EAPI int edata_dlist_node_init(Edata_DList_Node *node)
{
	int ret;

	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	ret = edata_list_node_init(EDATA_LIST_NODE(node));
	if (ret)
		node->previous = NULL;

	return ret;
}

/*
 * @brief Allocate and initialize a new list node
 * @return Returns NULL on error, new list node on success
 */
EAPI Edata_DList_Node *
edata_dlist_node_new()
{
	Edata_DList_Node *new_node;

	new_node = malloc(sizeof(Edata_DList_Node));

	if (!new_node)
	return NULL;

	if (!edata_dlist_node_init(new_node))
	{
		FREE(new_node);
		return NULL;
	}

	return new_node;
}

/*
 * @brief Call the data's free callback function, then free the node
 * @param node: the node to be freed
 * @param free_func: the callback function to execute on the data
 * @return Returns TRUE on success, FALSE on error
 */
EAPI int edata_dlist_node_destroy(Edata_DList_Node * node,
		Edata_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	return edata_list_node_destroy(EDATA_LIST_NODE(node), free_func);
}
