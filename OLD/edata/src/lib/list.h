#ifndef _EDATA_LIST_H
#define _EDATA_LIST_H

/**
 * @defgroup List_Group List
 * @{
 */
typedef struct _edata_list Edata_List;
#define EDATA_LIST(list) ((Edata_List *)list)

typedef struct _edata_list_node Edata_List_Node;
#define EDATA_LIST_NODE(node) ((Edata_List_Node *)node)

typedef struct _edata_strbuf Edata_Strbuf;
#define EDATA_STRBUF(buf) ((Edata_Strbuf *)buf)

struct _edata_list_node {
	void *data;
	struct _edata_list_node *next;
};

struct _edata_list {
	Edata_List_Node *first; /* The first node in the list */
	Edata_List_Node *last; /* The last node in the list */
	Edata_List_Node *current; /* The current node in the list */

	Edata_Free_Cb free_func; /* The callback to free data in nodes */

	int nodes; /* The number of nodes in the list */
	int index; /* The position from the front of the
	 list of current node */
};

/* Creating and initializing new list structures */
EAPI Edata_List *edata_list_new(void);
EAPI int edata_list_init(Edata_List *list);

/* Adding items to the list */
EAPI int edata_list_append(Edata_List * list, void *_data);
EAPI int edata_list_prepend(Edata_List * list, void *_data);
EAPI int edata_list_insert(Edata_List * list, void *_data);
EAPI int edata_list_append_list(Edata_List * list, Edata_List * append);
EAPI int edata_list_prepend_list(Edata_List * list, Edata_List * prepend);

/* Removing items from the list */
EAPI int edata_list_remove_destroy(Edata_List *list);
EAPI void *edata_list_remove(Edata_List * list);
EAPI void *edata_list_first_remove(Edata_List * list);
EAPI void *edata_list_last_remove(Edata_List * list);

/* Retrieve the current position in the list */
EAPI void *edata_list_current(Edata_List * list);
EAPI void *edata_list_first(Edata_List * list);
EAPI void *edata_list_last(Edata_List * list);
EAPI int edata_list_index(Edata_List * list);
EAPI int edata_list_count(Edata_List * list);

/** 
 * @defgroup Edata_List_Traverse_Group Traversal Functions
 * @{
 */
EAPI int edata_list_for_each(Edata_List *list, Edata_For_Each function,
	void *user_data);
EAPI void *edata_list_first_goto(Edata_List * list);
EAPI void *edata_list_last_goto(Edata_List * list);
EAPI void *edata_list_index_goto(Edata_List * list, int index);
EAPI void *edata_list_goto(Edata_List * list, const void *_data);
/** @} */

/* Traversing the list and returning data */
EAPI void *edata_list_next(Edata_List * list);
EAPI void *edata_list_find(Edata_List *list, Edata_Compare_Cb function,
	const void *user_data);

/* Sorting the list */
EAPI int edata_list_sort(Edata_List *list, Edata_Compare_Cb compare, char order);
EAPI int edata_list_mergesort(Edata_List *list, Edata_Compare_Cb compare,
	char order);
EAPI int edata_list_heapsort(Edata_List *list, Edata_Compare_Cb compare,
	char order);

/* Check to see if there is any data in the list */
EAPI int edata_list_empty_is(Edata_List * list);

/* Remove every node in the list without freeing the list itself */
EAPI int edata_list_clear(Edata_List * list);
/* Free the list and it's contents */
EAPI void edata_list_destroy(Edata_List *list);

/* Creating and initializing list nodes */
EAPI Edata_List_Node *edata_list_node_new(void);
EAPI int edata_list_node_init(Edata_List_Node *newNode);

/* Destroying nodes */
EAPI int edata_list_node_destroy(Edata_List_Node * _e_node,
		Edata_Free_Cb free_func);

EAPI int edata_list_free_cb_set(Edata_List * list, Edata_Free_Cb free_func);
/** @} */

/**
 * @defgroup DList_Group Double Linked List
 * @{
 */

typedef Edata_List Edata_DList;
#define EDATA_DLIST(dlist) ((Edata_DList *)dlist)

typedef struct _edata_dlist_node Edata_DList_Node;
#define EDATA_DLIST_NODE(dlist) ((Edata_DList_Node *)dlist)

struct _edata_dlist_node {
	Edata_List_Node single;
	Edata_DList_Node *previous;
};

/* Creating and initializing new list structures */
EAPI Edata_DList *edata_dlist_new(void);
EAPI int edata_dlist_init(Edata_DList *list);
EAPI void edata_dlist_destroy(Edata_DList *list);

/* Adding items to the list */
EAPI int edata_dlist_append(Edata_DList * _e_dlist, void *_data);
EAPI int edata_dlist_prepend(Edata_DList * _e_dlist, void *_data);
EAPI int edata_dlist_insert(Edata_DList * _e_dlist, void *_data);
EAPI int edata_dlist_append_list(Edata_DList * _e_dlist, Edata_DList * append);
EAPI int edata_dlist_prepend_list(Edata_DList * _e_dlist, Edata_DList * prepend);

/* Info about list's state */
EAPI void *edata_dlist_current(Edata_DList *list);
EAPI int edata_dlist_index(Edata_DList *list);
#define edata_dlist_count(list) edata_list_count(EDATA_LIST(list))

/* Removing items from the list */
EAPI void *edata_dlist_remove(Edata_DList * _e_dlist);
EAPI void *edata_dlist_first_remove(Edata_DList * _e_dlist);
EAPI int edata_dlist_remove_destroy(Edata_DList *list);
EAPI void *edata_dlist_last_remove(Edata_DList * _e_dlist);

/* Traversing the list */
#define edata_dlist_for_each(list, function, user_data) \
edata_list_for_each(EDATA_LIST(list), function, user_data)
EAPI void *edata_dlist_first_goto(Edata_DList * _e_dlist);
EAPI void *edata_dlist_last_goto(Edata_DList * _e_dlist);
EAPI void *edata_dlist_index_goto(Edata_DList * _e_dlist, int index);
EAPI void *edata_dlist_goto(Edata_DList * _e_dlist, void *_data);

/* Traversing the list and returning data */
EAPI void *edata_dlist_next(Edata_DList * list);
EAPI void *edata_dlist_previous(Edata_DList * list);

/* Sorting the list */
EAPI int edata_dlist_sort(Edata_DList *list, Edata_Compare_Cb compare,
	char order);
EAPI int edata_dlist_mergesort(Edata_DList *list, Edata_Compare_Cb compare,
	char order);
#define edata_dlist_heapsort(list, compare, order) \
edata_list_heapsort(list, compare, order)

/* Check to see if there is any data in the list */
EAPI int edata_dlist_empty_is(Edata_DList * _e_dlist);

/* Remove every node in the list without free'ing it */
EAPI int edata_dlist_clear(Edata_DList * _e_dlist);

/* Creating and initializing list nodes */
EAPI int edata_dlist_node_init(Edata_DList_Node * node);
EAPI Edata_DList_Node *edata_dlist_node_new(void);

/* Destroying nodes */
EAPI int edata_dlist_node_destroy(Edata_DList_Node * node,
	Edata_Free_Cb free_func);

EAPI int edata_dlist_free_cb_set(Edata_DList * dlist, Edata_Free_Cb free_func);

/** @} */
#endif /* _EDATA_LIST_H */
