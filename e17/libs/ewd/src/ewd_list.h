#ifndef __EWD_LIST_H__
#define __EWD_LIST_H__

typedef struct _ewd_list Ewd_List;
#define EWD_LIST(list) ((Ewd_List *)list)

typedef struct _ewd_list_node Ewd_List_Node;
#define EWD_LIST_NODE(node) ((Ewd_List_Node *)node)

struct _ewd_list_node {
	void *data;
	struct _ewd_list_node *next;

	EWD_DECLARE_LOCKS;
};

struct _ewd_list {
	Ewd_List_Node *first;	/* The first node in the list */
	Ewd_List_Node *last;	/* The last node in the list */
	Ewd_List_Node *current;	/* The current node in the list */

	Ewd_Free_Cb free_func;  /* The callback to free data in nodes */

	int nodes;		/* The number of nodes in the list */
	int index;		/* The position from the front of the
				   list of current node */
	EWD_DECLARE_LOCKS;
};


/* Creating and initializing new list structures */
Ewd_List *ewd_list_new();
int ewd_list_init(Ewd_List *list);

/* Adding items to the list */
inline int ewd_list_append(Ewd_List * list, void *_data);
inline int ewd_list_prepend(Ewd_List * list, void *_data);
inline int ewd_list_insert(Ewd_List * list, void *_data);

/* Removing items from the list */
inline int ewd_list_remove_destroy(Ewd_List *list);
inline void *ewd_list_remove(Ewd_List * list);
inline void *ewd_list_remove_first(Ewd_List * list);
inline void *ewd_list_remove_last(Ewd_List * list);

/* Retrieve the current position in the list */
inline void *ewd_list_current(Ewd_List * list);
int ewd_list_index(Ewd_List * list);
int ewd_list_nodes(Ewd_List * list);

/* Traversing the list */
int ewd_list_for_each(Ewd_List *list, Ewd_For_Each function);
inline void *ewd_list_goto_first(Ewd_List * list);
inline void *ewd_list_goto_last(Ewd_List * list);
inline void *ewd_list_goto_index(Ewd_List * list, int index);
inline void *ewd_list_goto(Ewd_List * list, void *_data);

/* Traversing the list and returning data */
inline void *ewd_list_next(Ewd_List * list);

/* Check to see if there is any data in the list */
int ewd_list_is_empty(Ewd_List * list);

/* Remove every node in the list without freeing the list itself */
int ewd_list_clear(Ewd_List * list);
/* Free the list and it's contents */
void ewd_list_destroy(Ewd_List *list);

/* Creating and initializing list nodes */
Ewd_List_Node *ewd_list_node_new();
int ewd_list_node_init(Ewd_List_Node *newNode);

/* Destroying nodes */
int ewd_list_node_destroy(Ewd_List_Node * _e_node, Ewd_Free_Cb free_func);

int ewd_list_set_free_cb(Ewd_List * list, Ewd_Free_Cb free_func);

typedef Ewd_List Ewd_DList;
#define EWD_DLIST(dlist) ((Ewd_DList *)dlist)

typedef struct _ewd_dlist_node Ewd_DList_Node;
#define EWD_DLIST_NODE(dlist) ((Ewd_DList_Node *)dlist)

struct _ewd_dlist_node {
	Ewd_List_Node single;
	Ewd_DList_Node *previous;
};

/* Creating and initializing new list structures */
Ewd_DList *ewd_dlist_new();
int ewd_dlist_init(Ewd_DList *list);
void ewd_dlist_destroy(Ewd_DList *list);

/* Adding items to the list */
int ewd_dlist_append(Ewd_DList * _e_dlist, void *_data);
int ewd_dlist_prepend(Ewd_DList * _e_dlist, void *_data);
int ewd_dlist_insert(Ewd_DList * _e_dlist, void *_data);

/* Info about list's state */
void *ewd_dlist_current(Ewd_DList *list);
int ewd_dlist_index(Ewd_DList *list);
#define ewd_dlist_nodes(list) ewd_list_nodes(EWD_LIST(list))

/* Removing items from the list */
void *ewd_dlist_remove(Ewd_DList * _e_dlist);
void *ewd_dlist_remove_first(Ewd_DList * _e_dlist);
void *ewd_dlist_remove_last(Ewd_DList * _e_dlist);

/* Traversing the list */
#define ewd_dlist_for_each(list, function) \
				ewd_list_for_each(EWD_LIST(list), function)
inline void *ewd_dlist_goto_first(Ewd_DList * _e_dlist);
inline void *ewd_dlist_goto_last(Ewd_DList * _e_dlist);
inline void *ewd_dlist_goto_index(Ewd_DList * _e_dlist, int index);
inline void *ewd_dlist_goto(Ewd_DList * _e_dlist, void *_data);

/* Traversing the list and returning data */
inline void *ewd_dlist_next(Ewd_DList * list);
inline void *ewd_dlist_previous(Ewd_DList * list);

/* Check to see if there is any data in the list */
int ewd_dlist_is_empty(Ewd_DList * _e_dlist);

/* Remove every node in the list without free'ing it */
int ewd_dlist_clear(Ewd_DList * _e_dlist);

/* Creating and initializing list nodes */
Ewd_DList_Node *ewd_dlist_node_new();

/* Destroying nodes */
int ewd_dlist_node_destroy(Ewd_DList_Node * node, Ewd_Free_Cb free_func);

int ewd_dlist_set_free_cb(Ewd_DList * dlist, Ewd_Free_Cb free_func);

#endif
