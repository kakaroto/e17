#ifndef _EWD_DLIST_H
#define _EWD_DLIST_H

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

/* Adding items to the list */
int ewd_dlist_append(Ewd_DList * _e_dlist, void *_data);
int ewd_dlist_prepend(Ewd_DList * _e_dlist, void *_data);
int ewd_dlist_insert(Ewd_DList * _e_dlist, void *_data);

/* Removing items from the list */
int ewd_dlist_remove(Ewd_DList * _e_dlist);
int ewd_dlist_remove_first(Ewd_DList * _e_dlist);
int ewd_dlist_remove_last(Ewd_DList * _e_dlist);

/* Traversing the list */
int ewd_dlist_goto_first(Ewd_DList * _e_dlist);
int ewd_dlist_goto_last(Ewd_DList * _e_dlist);
int ewd_dlist_goto_index(Ewd_DList * _e_dlist, int index);
int ewd_dlist_goto(Ewd_DList * _e_dlist, void *_data);

/* Traversing the list and returning data */
void *ewd_dlist_next(Ewd_DList * _e_dlist);
void *ewd_dlist_previous(Ewd_DList * _e_dlist);

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
