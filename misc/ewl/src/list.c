#include "list.h"

EwlListNode *ewl_list_node_new(void *value)
{
	EwlListNode *node = malloc(sizeof(EwlListNode));
	if (node)	{
		node->data = value;
		node->ref_count = 0;
		node->next = NULL;
		node->prev = NULL;
	} else {
		/* FIXME */
	}

	return node;
}

EwlListNode *ewl_list_node_dup(EwlListNode *node)
{
	EwlListNode *rnode = NULL;
	if (!node)	{
		/* FIXME */
	} else {
		rnode = ewl_list_node_new(node->data);
		if (!rnode)	{
			/* FIXME */
		} else {
			rnode->next = node->next;
			rnode->prev = node->prev;
			rnode->ref_count = node->ref_count;
		}
	}

	return rnode;
}

void         ewl_list_node_free(EwlListNode *node)
{
	if (!node)	{
		/* FIXME */
	} else {
		free(node);
		node = NULL;
	}
	return;
}

void         ewl_list_node_ref(EwlListNode *node)
{
	if (!node)	{
		/* FIXME */
	} else {
		node->ref_count++;
	}
	return;
}


EwlIterator *ewl_iterator_start(EwlList *list)
{
	EwlIterator *i = NULL;
	if (!list)	{
		/* FIXME */
	} else {
		i = list->head;
	}
	return i;	
}

EwlIterator *ewl_iterator_next(EwlIterator *iterator)
{
	ewl_list_node_ref(iterator);
	return iterator->next;
}


EwlList     *ewl_list_new()
{
	EwlList *list = malloc(sizeof(EwlList));
	if (!list)	{
		/* FIXME */
	} else {
		list->type = EWL_LIST_ORDERED;
		list->head = NULL;
		list->tail = NULL;
		list->len  = 0;
		list->ref_inc = 0;
		list->ref_max = 256;
		list->blocked = 0;
	}
	return list;
}

void         ewl_list_dup_cb(EwlListNode *node, void *data)
{
	EwlList *list = (EwlList*) data;
	ewl_list_insert(list,node);
	return;
}

EwlList     *ewl_list_dup(EwlList *list)
{
	EwlList *rlist = NULL;
	if (!list)	{
		/* FIXME */
	} else {
		ewl_list_ref(list);
		rlist = ewl_list_new();
		if (!rlist)	{
			/* FIXME */
		} else {
			ewl_list_foreach(list, ewl_list_dup_cb, rlist);
		}
	}
	return rlist;
}

void         ewl_list_free_cb(EwlListNode *node, void *data)
{
	UNUSED(data);
	ewl_list_node_free(node);
	return;
}

void         ewl_list_free(EwlList *list)
{
	if (!list)	{
		/* FIXME */
	} else {
		ewl_list_foreach(list,ewl_list_free_cb,NULL);
		free(list);    
		list = NULL;
	}
	return;
}

int          ewl_list_len(EwlList *list)
{
	return list->len;
}

void         ewl_list_ref(EwlList *list)
{
	if (!list)	{
		/* FIXME */
	} else {
		list->ref_inc++;
		if (list->ref_inc>=list->ref_max)	{
			ewl_list_optimize(list);
		}

	}
	return;
}


void         ewl_list_insert(EwlList *list, EwlListNode *node)
{
	EwlListNode *tn;
	if (!list)	{
		/* FIXME */
	} else if (!node)	{
		/* FIXME */
	} else {
		ewl_list_ref(list);
		if (list->tail)	{
			list->tail->next = node;
			node->prev = list->tail;
		} else if (list->head)	{
			for (tn=list->head; tn->next; tn=tn->next);
			tn->next = node;
			node->prev = tn;
		} else {
			list->head = node;
			node->prev = NULL;
		}
		list->tail = node;
		node->next = NULL;
		node->ref_count = 0;
		list->len++;
	}
	return;
}

void         ewl_list_push(EwlList *list, EwlListNode *node)
{
	if (!list)	{
		/* FIXME */
	} else if (!node) {
		/* FIXME */
	} else {
		ewl_list_ref(list);
		node->prev = NULL;
		node->next = list->head;
		node->ref_count = 0;
		list->head = node;
		list->len++;
	}
	return;
}


void         ewl_list_remove(EwlList *list, EwlListNode *node)
{
	EwlListNode *tn;
	if (!list)	{
		/* FIXME */
	} else if (!node) {
		/* FIXME */
	} else {
		for (tn=list->head; tn; tn=tn->next)	{
			if (tn==node)	{
				if (node->prev) node->prev->next = node->next;
				if (node->next) node->next->prev = node->prev;
				if (list->head==node) list->head = node->next;
				if (list->tail==node) list->tail = node->prev;
				list->len--;
				break;
			}
		}
		ewl_list_ref(list);
	}
	return;
}

EwlListNode *ewl_list_pop(EwlList *list)
{
	EwlListNode *node = NULL;
	if (!list)	{
		/* FIXME */
	} else {
		node = list->head;
		if (list->head)	{
			list->head = list->head->next;
			if (list->head) list->head->prev = NULL;
			if (node==list->tail) list->tail = NULL;
			list->len--;
			ewl_list_ref(list);
		}
		/*fprintf(stderr,"ewl_list_pop(): list->len = %d\n", list->len);*/
	}
	return node;
}

EwlListNode *ewl_list_find(EwlList *list,
                           char   (*cb)(EwlListNode *node,
                                        void        *data),
                           void    *data)
{
	EwlListNode *tn;
	if (!list)	{
		/* FIXME */
	} else {
		ewl_list_ref(list);
		for (tn=list->head; tn; tn=tn->next)	{
			if (cb(tn,data))	{
				ewl_list_node_ref(tn);
				return tn;
			}
		}
	}
	return NULL;
}

char         ewl_list_find_by_value_cb(EwlListNode *node, void *data)
{
	return (node->data==data);
}

EwlListNode *ewl_list_find_by_value(EwlList *list, void *value)
{
	EwlListNode *node = ewl_list_find(list,
	                                  ewl_list_find_by_value_cb,
	                                  value);
	return node;
}

void         ewl_list_foreach(EwlList *list,
                              void   (*cb)(EwlListNode *node,
                                           void        *data),
                              void    *data)
{
	EwlListNode *tn, *next;
	if (!list)	{
		/* FIXME */
	} else {
		ewl_list_ref(list);
		ewl_list_block(list);
		for (tn=list->head; tn; tn=next)	{
			next = tn->next;
			ewl_list_node_ref(tn);
			cb(tn, data);
		}
		ewl_list_unblock(list);
	}
	return;
}


void         ewl_list_sort(EwlList *list, 
                           char   (*cb)(EwlListNode *n0,
                                        EwlListNode *n1,
                                        void        *data),
                           void    *data)
{
	EwlIterator *i = NULL;
	char done = 0, r;
	ewl_list_block(list);
	while (!done)	{
		r = 0;
		for (i=ewl_iterator_start(list); r!=-1 && i; i=ewl_iterator_next(i)) {
			r = cb(i, i->next, data);
			if (r==-1)	{
				ewl_list_swap_nodes(list, i, i->next);
				break;
			}
		}
		fprintf(stderr,"starting over\n");
		if (!i) done++;
	}
	ewl_list_unblock(list);
	return;
}

void         ewl_list_swap_nodes(EwlList *list,
                                 EwlListNode *n1,
                                 EwlListNode *n2)
{
	if (!n1||!n2) return;
	
	fprintf(stderr,"BS n1 = %08x, n1->prev = %08x, n1->next = %08x\n"
	        "BS n2 = %08x, n2->prev = %08x, n2->next = %08x\n",
	        (unsigned int) n1, (unsigned int) n1->prev,
	        (unsigned int) n1->next, (unsigned int) n2,
	        (unsigned int) n2->prev, (unsigned int) n2->next);

	n1->next = n2->next;
	n2->next = n1;
	n2->prev = n1->prev;
	n1->prev = n2;

	if (list->head==n1) {
		list->head = n2;
	} else if (list->head==n2) {
		list->head = n1;
	}

	if (list->tail==n1) {
		list->tail = n2;
	} else if (list->tail==n2) {
		list->tail = n1;
	} 

	fprintf(stderr,"AS n1 = %08x, n1->prev = %08x, n1->next = %08x\n"
	        "AS n2 = %08x, n2->prev = %08x, n2->next = %08x\n",
	        (unsigned int) n1, (unsigned int) n1->prev,
	        (unsigned int) n1->next, (unsigned int) n2,
	        (unsigned int) n2->prev, (unsigned int) n2->next);

	return;
}


void         ewl_list_set_type(EwlList *list, int type)
{
	list->type = type;
	return;
}

int          ewl_list_get_type(EwlList *list)
{
	return list->type;
}

void         ewl_list_set_optimize_count(EwlList *list, int count)
{
	list->ref_max = count;
	return;
}

int          ewl_list_get_optimize_count(EwlList *list)
{
	return list->ref_max;
}

char ewl_list_optimize_cmp_cb(EwlListNode *n0,
                              EwlListNode *n1,
                              void        *data)
{
	char r = 0;
	UNUSED(data);
	if (n0&&n1)	{
		if (n0->ref_count>n1->ref_count) {
			r = 1;
		} else if (n0->ref_count==n1->ref_count) {
			r = 0;
		} else /*if (n0->ref_count==n1->ref_count)*/ {
			r = -1;
		}
	}
	return r;
}

void         ewl_list_optimize(EwlList *list)
{
	/* FIXME -- disabled until i can figure ot why it's not working
	            correctly */
	list->ref_inc = 0;
	return;
	if (ewl_list_blocked(list)) {
		/* FIXME */
	} else if (ewl_list_get_type(list)==EWL_LIST_UNORDERED) {
		list->ref_inc = 0;
		ewl_list_sort(list, ewl_list_optimize_cmp_cb, NULL);
	} else {
		list->ref_inc = 0;
	}
	return;
}

void        ewl_list_block(EwlList *list)
{
	list->blocked = 1;
	return;
}

void        ewl_list_unblock(EwlList *list)
{
	list->blocked = 0;
	return;
}

char        ewl_list_blocked(EwlList *list)
{
	return list->blocked;
}
