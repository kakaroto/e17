/* Some internal functions that do not lock the entire list struct. These
 * are given a header file so that objects that inherit from the list
 * structure can make use of them. */

/* Return information about the list */
int _ewd_list_index(Ewd_List * list);
int _ewd_list_nodes(Ewd_List * list);
int _ewd_list_is_empty(Ewd_List * list);
void *_ewd_list_current(Ewd_List * list);

/* Adding functions */
int _ewd_list_insert(Ewd_List * list, void *data);
int _ewd_list_append(Ewd_List * list, void *data);
int _ewd_list_prepend(Ewd_List * list, void *data);

/* Remove functions */
int _ewd_list_remove(Ewd_List * list);
int _ewd_list_remove_first(Ewd_List * list);
int _ewd_list_remove_last(Ewd_List * list);

/* Basic traversal functions */
void *_ewd_list_next(Ewd_List * list);
int _ewd_list_goto_last(Ewd_List * list);
int _ewd_list_goto_first(Ewd_List * list);
int _ewd_list_goto(Ewd_List * list, void *data);
int _ewd_list_goto_index(Ewd_List *list, int index);

/* Iterative function */
int _ewd_list_for_each(Ewd_List *list, Ewd_For_Each function);

/* Private double linked list functions */
void *_ewd_dlist_previous(Ewd_DList * list);
int _ewd_dlist_remove_first(Ewd_DList *list);
int _ewd_dlist_goto_index(Ewd_DList *list, int index);
