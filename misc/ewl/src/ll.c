#include "ll.h"

EwlLL *ewl_ll_node_new(EwlData *data)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_node_new");

	if (!data)	{
		ewl_debug("ewl_ll_node_new", EWL_NULL_ERROR, "data");
		/* some people might _want_ NULL nodes.. :) */
		/* FUNC_END("ewl_ll_new");
		return NULL; */ 
	}

	l = malloc(sizeof(EwlLL));
	if (!l)	{
		ewl_debug("ewl_ll_new", EWL_NULL_ERROR, "l");
	} else {
		l->data = data;
		l->next = NULL;
	}

	FUNC_END("ewl_ll_node_new");
	return l;
}

EwlLL *ewl_ll_node_dup(EwlLL *node)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_node_dup");

	if (!node)	{
		ewl_debug("ewl_ll_node_dup", EWL_NULL_ERROR, "node");
		FUNC_END("ewl_ll_node_dup");
		return NULL;
	}

	l = ewl_ll_node_new(node->data);
	
	if (!l)	{
		ewl_debug("ewl_ll_node_dup", EWL_NULL_ERROR, "l");
	} else {	
		l->next = node->next;
	}
	FUNC_END("ewl_ll_node_dup");
	return l;
}

EwlLL *ewl_ll_node_free(EwlLL *node)
{
	FUNC_BGN("ewl_ll_node_free");
	if (!node)	{
		ewl_debug("ewl_ll_node_free", EWL_NULL_ERROR, "node");
	} else {
		free(node);
		node = NULL;
	}
	FUNC_END("ewl_ll_node_free");
	return NULL;
}

EwlLL *ewl_ll_new(EwlData *data)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_new");
	l = ewl_ll_node_new(data);
	if (!l)	{
		ewl_debug("ewl_ll_new", EWL_NULL_ERROR, "l");
	}
	FUNC_END("ewl_ll_new");
	return l;
}

static EwlBool _cb_ewl_ll_dup(EwlLL *node, EwlData *data)
{
	EwlLL *l = NULL,
	      *d = (EwlLL*) data;
	if (!node)	{
		ewl_debug("for_cb_ewl_ll_dup", EWL_NULL_ERROR, "node");
		return 1;
	} else {
		l = ewl_ll_node_dup(node);
		if (!l)	{
			ewl_debug("for_cb_ewl_ll_dup", EWL_NULL_ERROR, "l");
		} else {
			l->next = NULL;
			d = ewl_ll_insert(d,l);
		}
	}
	return 1;
}

EwlLL *ewl_ll_dup(EwlLL *node)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_dup");

	if (!node)	{
		ewl_debug("ewl_ll_dup", EWL_NULL_ERROR, "node");
		FUNC_END("ewl_ll_dup");
		return NULL;
	}

	l = NULL;
	ewl_ll_foreach(node, _cb_ewl_ll_dup, (EwlData*) l);
	FUNC_END("ewl_ll_dup");
	return l;
}

static EwlBool _cb_ewl_ll_free(EwlLL *node, EwlData *data)
{
	if (node)
		ewl_ll_node_free(node);
	return 1;
}

EwlLL *ewl_ll_free(EwlLL *node)
{
	FUNC_BGN("ewl_ll_free");
	if (!node)	{
		ewl_debug("ewl_ll_free", EWL_NULL_ERROR, "node");
	} else {
		/* BROKEN SHOULD CALL EWL_LL_FOREACH(node,free_node_cb,bleh) */
		node = ewl_ll_foreach(node, _cb_ewl_ll_free, NULL);
		node = NULL;
	}
	FUNC_END("ewl_ll_free");
	return NULL;
}


EwlLL *ewl_ll_insert(EwlLL *base, EwlLL *node)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_insert");

	if (!base)	{
		ewl_debug("ewl_ll_insert", EWL_NULL_ERROR, "base");
		FUNC_END("ewl_ll_insert");
		return node;
	}

	if (!node)	{
		ewl_debug("ewl_ll_insert", EWL_NULL_ERROR, "node");
		FUNC_END("ewl_ll_insert");
		return base;
	}

	/*for (l=base; l->next; l=l->next);*/
	l = ewl_ll_tail(base);
	l->next = node;

	FUNC_END("ewl_ll_insert");
	return base;
}

EwlLL *ewl_ll_insert_with_data(EwlLL *base, EwlData *data)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_insert_with_data");

	l = ewl_ll_new(data);
	if (!l)	{
		ewl_debug("ewl_ll_insert_with_data", EWL_NULL_ERROR, "l");
	} else {
		base = ewl_ll_insert(base,l);
	}
	
	FUNC_END("ewl_ll_insert_with_data");
	return base;
}

EwlLL *ewl_ll_remove(EwlLL *base, EwlLL *node)
{
	EwlBool    found = 0;
	EwlLL     *l = NULL;
	FUNC_BGN("ewl_ll_remove");
	
	if (!base)	{
		ewl_debug("ewl_ll_remove", EWL_NULL_ERROR, "base");
		FUNC_END("ewl_ll_remove");
		return base;
	}

	if (!node)	{
		ewl_debug("ewl_ll_remove", EWL_NULL_ERROR, "node");
		FUNC_END("ewl_ll_remove");
		return base;
	}


	if (base==node)	{
		FUNC_END("ewl_ll_remove");
		return base->next;
	} else {
		for (l=base;l->next; l=l->next)	{
			if (l->next==node)	{
				found++;
				break;
			}
		}
	}

	if (found)	{
		l->next = node->next;
	}

	FUNC_END("ewl_ll_remove");
	return base;
}

EwlLL *ewl_ll_remove_by_data(EwlLL *base, EwlData *data)
{
	EwlLL       *l = NULL;
	FUNC_BGN("ewl_ll_remove_by_data");
	
	if (!base)	{
		ewl_debug("ewl_ll_remove_by_data", EWL_NULL_ERROR, "base");
		FUNC_END("ewl_ll_remove_by_data");
		return base;
	}

	if (!data)	{
		ewl_debug("ewl_ll_remove_by_data", EWL_NULL_ERROR, "data");
	}


	l = ewl_ll_find(base,data);
	if (!l)	{
		ewl_debug("ewl_ll_remove_by_data", EWL_NULL_ERROR, "l");
	} else {
		base = ewl_ll_remove(base,l);
	}

	FUNC_END("ewl_ll_remove_by_data");
	return base;

}

EwlLL   *ewl_ll_push(EwlLL *base, EwlLL *node)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_push");
	if (!node)	{
		ewl_debug("ewl_ll_push", EWL_NULL_ERROR, "node");
	} else {
		l = ewl_ll_tail(node);
		l->next = base;
	}
	FUNC_END("ewl_ll_push");
	return node;
}

EwlLL   *ewl_ll_push_with_data(EwlLL *base, EwlData *data)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_push_with_data");
	l = ewl_ll_node_new(data);
	if (!l)	{
		ewl_debug("ewl_ll_push_with_data", EWL_NULL_ERROR, "node");
	} else {
		l->next = base;
	}
	FUNC_END("ewl_ll_push_with_data");
	return l;
}

EwlLL   *ewl_ll_pop(EwlLL *base)
{
	EwlLL *l = base;
	FUNC_BGN("ewl_ll_push_with_data");
	if (!base)	{
		ewl_debug("ewl_ll_pop", EWL_NULL_ERROR, "base");
	} else {
		base = base->next;
		l->next = NULL;
	}
	FUNC_END("ewl_ll_push_with_data");
	return l;
}

EwlData *ewl_ll_pop_data(EwlLL *base)
{
	EwlLL   *l = ewl_ll_pop(base);
	EwlData *d = NULL;
	FUNC_BGN("ewl_ll_pop_data");
	if (!l)	{
		ewl_debug("ewl_ll_pop_data", EWL_NULL_ERROR, "l");
	} else {
		d = l->data;
		ewl_ll_node_free(l);
	}
	FUNC_END("ewl_ll_pop_data");
	return d;
}



EwlLL *ewl_ll_find(EwlLL *base, EwlData *data)
{
	EwlBool     found = 0;
	EwlLL      *l = NULL;
	FUNC_BGN("ewl_ll_find");

	if (!base)	{
		ewl_debug("ewl_ll_find", EWL_NULL_ERROR, "base");
		FUNC_END("ewl_ll_find");
		return NULL;
	}

	
	for (l=base;l; l=l->next)	{
		if (l->data==data)	{
			found++;
			break;
		}
	}

	if (!found)
		l = NULL;

	FUNC_END("ewl_ll_find");
	return l;
}

EwlLL *ewl_ll_callback_find(EwlLL    *base,
                            EwlBool (*cmp_cb)(EwlLL   *node,
                                              EwlData *search_data),
                            EwlData  *data)
{
	EwlBool      found = 0;
	EwlLL       *l = NULL;
	FUNC_BGN("ewl_ll_callback_find");

	if (!base)	{
		ewl_debug("ewl_ll_callback_find", EWL_NULL_ERROR, "base");
		FUNC_END("ewl_ll_callback_find");
		return NULL;
	}

	if (!cmp_cb)	{
		ewl_debug("ewl_ll_callback_find", EWL_NULL_ERROR, "find_cb");
		l = ewl_ll_find(base, data);
		FUNC_END("ewl_ll_callback_find");
		return l;
	}

	
	for (l=base;l; l=l->next)	{
		if (cmp_cb(l,data))	{
			found++;
			break;
		}
	}

	if (!found)
		l = NULL;

	FUNC_END("ewl_ll_callback_find");
	return l;
}

EwlLL *ewl_ll_tail(EwlLL *base)
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_tail");
	if (!base)	{
		ewl_debug("ewl_ll_tail", EWL_NULL_ERROR, "base");
	} else {
		for (l=base; l->next; l=l->next);
	}
	FUNC_END("ewl_ll_tail");
	return l;
}

int ewl_ll_sizeof(EwlLL *base)
{
	int     i = 0;
	EwlLL *l = NULL;
	FUNC_BGN("ewl_ll_sizeof");
	if (!base)	{
		ewl_debug("ewl_ll_sizeof", EWL_NULL_ERROR, "base");
	} else {
		for (l=base;l; l=l->next) i++;
	}
	FUNC_END("ewl_ll_sizeof");
	return i;
}

EwlLL *ewl_ll_foreach(EwlLL    *base, 
                      EwlBool (*for_cb)(EwlLL *node,
                                        EwlData *data),
                      EwlData  *data)
{
	EwlLL *tn = NULL,
	       *nn = NULL;
	FUNC_BGN("ewl_ll_foreach");

	if (!base)	{
		ewl_debug("ewl_ll_foreach", EWL_NULL_ERROR, "base");
		FUNC_END("ewl_ll_foreach");
		return NULL;
	} else if (!for_cb)	{
		ewl_debug("ewl_ll_foreach", EWL_NULL_ERROR, "for_cb");
		FUNC_END("ewl_ll_foreach");
		return base;
	}

	for(tn=base;tn;tn=nn)	{
		nn = tn->next;
		if (!for_cb(tn,data))	{
			break;
		}
	}

	FUNC_END("ewl_ll_foreach");
	return base;
}



