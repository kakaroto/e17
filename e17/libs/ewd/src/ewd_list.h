
#ifndef __EWD_LIST_H__
#define __EWD_LIST_H__

typedef struct _ewd_list Ewd_List;
typedef struct _ewd_list_node Ewd_List_Node;

struct _ewd_list_node {
	void  * data;
	struct _ewd_list_node *prev;
	struct _ewd_list_node *next;
};

struct _ewd_list {
	Ewd_List_Node * first;	/* The first node in the list */
	Ewd_List_Node * last;		/* The last node in the list */
	Ewd_List_Node * current;	/* The current node in the list */

	int nodes;				/* The number of nodes in the list */
	int index;				/* The position from the front of the list of current node */
};


/* Creating and initializing new list structures */
Ewd_List		* ewd_list_new();

/* Adding items to the list */
void 			ewd_list_append(Ewd_List * _e_list, void *_data);
void			ewd_list_prepend(Ewd_List * _e_list, void *_data);
void			ewd_list_insert(Ewd_List * _e_list, void *_data);

/* Removing items from the list */
void			ewd_list_remove(Ewd_List * _e_list);
void			ewd_list_remove_first(Ewd_List * _e_list);
void			ewd_list_remove_last(Ewd_List * _e_list);

/* Traversing the list */
void			ewd_list_goto_first(Ewd_List * _e_list);
void			ewd_list_goto_last(Ewd_List * _e_list);
void			ewd_list_goto_index(Ewd_List * _e_list, int index);
void			ewd_list_goto(Ewd_List * _e_list, void * _data);

/* Traversing the list and returning data */
void			* ewd_list_next(Ewd_List * _e_list);
void			* ewd_list_previous(Ewd_List * _e_list);

/* Check to see if there is any data in the list */
int				ewd_list_is_empty(Ewd_List * _e_list);

/* Remove every node in the list without free'ing it */
void			ewd_list_clear(Ewd_List * _e_list);

/* Creating and initializing list nodes */
Ewd_List_Node	* ewd_list_node_new();

/* Destroying nodes */
void 			ewd_list_node_destroy(Ewd_List_Node * _e_node);

#endif
