#ifndef _LIST_H_
#define _LIST_H_ 

#include "includes.h"
#include "error.h"
#include "util.h"

#define EWL_LIST(a) ((EwlList*)a)
#define EWL_LIST_NODE(a) ((EwlListNode*)a)

typedef struct _EwlList     EwlList;
typedef struct _EwlListNode EwlListNode;
typedef EwlListNode         EwlIterator;

enum _EwlListTypes	{
	EWL_LIST_ORDERED,
	EWL_LIST_UNORDERED
};

struct _EwlListNode	{
	void        *data;
	EwlListNode *next;
	EwlListNode *prev;
	int          ref_count;
};

struct _EwlList	{
	int          type;
	EwlListNode *head;
	EwlListNode *tail;
	int          len;
	
	/* reference incrementers -- used for reordering the list */
	int          ref_inc;
	int          ref_max;
	char         blocked;
};

/* NODE FUNCTIONS */
EwlListNode *ewl_list_node_new(void *value);
EwlListNode *ewl_list_node_dup(EwlListNode *node);
void         ewl_list_node_free(EwlListNode *node);

void         ewl_list_node_ref(EwlListNode *node);

/* ITERATOR FUNCTIONS (for inline ewl_list_foreach() */
EwlIterator *ewl_iterator_start(EwlList *hash);
EwlIterator *ewl_iterator_next(EwlIterator *iterator);

/* LIST FUNCTIONS */
EwlList     *ewl_list_new();
EwlList     *ewl_list_dup(EwlList *list);
void         ewl_list_free(EwlList *list);

int          ewl_list_len(EwlList *list);
void         ewl_list_ref(EwlList *list);

/* LIST INSERT/REMOVE FUNCTIONS */
void         ewl_list_insert(EwlList *list, EwlListNode *node);
void         ewl_list_push(EwlList *list, EwlListNode *node);

void         ewl_list_remove(EwlList *list, EwlListNode *node);
EwlListNode *ewl_list_pop(EwlList *list);

/* LIST SEARCH FUNCTIONS */
EwlListNode *ewl_list_find(EwlList *list,
                           char   (*cb)(EwlListNode *node,
                                        void        *data),
                           void    *data);
EwlListNode *ewl_list_find_by_value(EwlList *list, void *value);

void         ewl_list_foreach(EwlList *list,
                              void   (*cb)(EwlListNode *node,
                                           void        *data),
                              void    *data);

/* LIST SORT FUNCTIONS */
void         ewl_list_sort(EwlList *list, 
                           char   (*cb)(EwlListNode *node0,
                                        EwlListNode *node1,
                                        void        *data),
                           void    *data);
void         ewl_list_swap_nodes(EwlList *list,
                                 EwlListNode *n1,
                                 EwlListNode *n2);

void         ewl_list_block(EwlList *list);
void         ewl_list_unblock(EwlList *list);
char         ewl_list_blocked(EwlList *list);

/* LIST OPTIMIZATION FUNCTIONS */
void         ewl_list_set_type(EwlList *list, int type);
int          ewl_list_get_type(EwlList *list);

void         ewl_list_set_optimize_count(EwlList *list, int count);
int          ewl_list_get_optimize_count(EwlList *list);

void         ewl_list_optimize(EwlList *list);

#endif /* _LIST_H_ */
