/* quick linked list api for ewl */
#ifndef _LL_H_
#define _LL_H_ 1

#include "includes.h"
#include "debug.h"

typedef struct _EwlLL EwlLL;
struct _EwlLL
{
	EwlData    *data;
	EwlLL      *next;
};

/* NODE ALLOC FUNCTIONS */
EwlLL *ewl_ll_node_new(EwlData *data);
EwlLL *ewl_ll_node_dup(EwlLL *node);
EwlLL *ewl_ll_node_free(EwlLL *node);

/* ALLOC FUNCTIONS */
EwlLL *ewl_ll_new(EwlData *data);
EwlLL *ewl_ll_dup(EwlLL *node);
EwlLL *ewl_ll_free(EwlLL *node);

/* INSERT/REMOVE FUNCTIONS */
EwlLL *ewl_ll_insert(EwlLL *base, EwlLL *node);
EwlLL *ewl_ll_insert_with_data(EwlLL *base, EwlData *data);
EwlLL *ewl_ll_remove(EwlLL *base, EwlLL *node);
EwlLL *ewl_ll_remove_by_data(EwlLL *base, EwlData *data);

/* PUSH/POP (FRONT OF LIST) FUNCTIONS */
EwlLL   *ewl_ll_push(EwlLL *base, EwlLL *node);
EwlLL   *ewl_ll_push_with_data(EwlLL *base, EwlData *data);
EwlLL   *ewl_ll_pop(EwlLL *base);
EwlData *ewl_ll_pop_data(EwlLL *base);

/* FIND/BATCH FUNCTIONS */
EwlLL *ewl_ll_find(EwlLL *base, EwlData *data);
EwlLL *ewl_ll_callback_find(EwlLL    *base,
                            EwlBool (*cmp_cb)(EwlLL   *node,
                                              EwlData *search_data),
                            EwlData *data);
EwlLL *ewl_ll_foreach(EwlLL    *base, 
                      EwlBool (*for_cb)(EwlLL *node,
                                        EwlData *data),
                      EwlData *data);

/* MISC FUNCTIONS */
EwlLL *ewl_ll_tail(EwlLL *base);
int ewl_ll_sizeof(EwlLL *base);

#endif /* _EWL_H_ */
