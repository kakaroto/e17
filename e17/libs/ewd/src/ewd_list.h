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
int ewd_list_append(Ewd_List * list, void *_data);
int ewd_list_prepend(Ewd_List * list, void *_data);
int ewd_list_insert(Ewd_List * list, void *_data);

/* Removing items from the list */
int ewd_list_remove_destroy(Ewd_List *list);
void *ewd_list_remove(Ewd_List * list);
void *ewd_list_remove_first(Ewd_List * list);
void *ewd_list_remove_last(Ewd_List * list);

/* Retrieve the current position in the list */
void *ewd_list_current(Ewd_List * list);
int ewd_list_index(Ewd_List * list);
int ewd_list_nodes(Ewd_List * list);

/* Traversing the list */
int ewd_list_goto_first(Ewd_List * list);
int ewd_list_goto_last(Ewd_List * list);
int ewd_list_goto_index(Ewd_List * list, int index);
int ewd_list_goto(Ewd_List * list, void *_data);

/* Traversing the list and returning data */
void *ewd_list_next(Ewd_List * list);

/* Check to see if there is any data in the list */
int ewd_list_is_empty(Ewd_List * list);

/* Remove every node in the list without free'ing it */
int ewd_list_clear(Ewd_List * list);
void ewd_list_destroy(Ewd_List *list);

/* Creating and initializing list nodes */
Ewd_List_Node *ewd_list_node_new();
int ewd_list_node_init(Ewd_List_Node *new);

/* Destroying nodes */
int ewd_list_node_destroy(Ewd_List_Node * _e_node, Ewd_Free_Cb free_func);

int ewd_list_set_free_cb(Ewd_List * list, Ewd_Free_Cb free_func);

#endif
