#include "hash.h"

EwlHash     *ewl_hash_new()
{
	EwlHash *hash = ewl_list_new();
	ewl_list_set_type(hash, EWL_LIST_UNORDERED);
	ewl_list_set_optimize_count(hash, EWL_HASH_OPTIMIZE_REF_COUNT);
	return hash;
}

void         ewl_hash_free_cb(EwlListNode *node, void *data)
{
	UNUSED(data);
	ewl_hash_node_free((EwlHashNode*) node);
	return;
}

void         ewl_hash_free(EwlHash *hash)
{
	if (!hash)	{
		/* FIXME */
	} else {
		ewl_list_foreach(hash,ewl_hash_free_cb,NULL);
		free(hash);
	}
	return;
}


char         ewl_hash_get_cb(EwlListNode *node, void *data)
{
	EwlHashNode *hn = (EwlHashNode*) node;
	char        *key = (char*) data;
	return ewl_string_cmp(hn->key,key,0);
}

void        *ewl_hash_get(EwlHash *hash, char *key)
{
	EwlListNode *ln;
	void        *value = NULL;
	if (!hash)	{
		/* FIXME */
	} else {
		ln = ewl_list_find(hash, ewl_hash_get_cb, key);
		if (ln)	{
			value = EWL_HASH_NODE(ln)->data;
		}
	}
	return value;
}

void         ewl_hash_set(EwlHash *hash, char *key, void *data)
{
	EwlListNode *ln;
	EwlHashNode *node;
	if (!hash)	{
		/* FIXME */
	} else {
		ln = ewl_list_find(hash, ewl_hash_get_cb, key);
		if (ln)	{
			node = EWL_HASH_NODE(ln);
			node->data = data;
		} else {
			node = ewl_hash_node_new(key,data);
			node->key = ewl_string_dup(key);
			ln = (EwlListNode*) node;
			ewl_list_insert(hash,ln);
		}
	}
	return;
}

void         ewl_hash_remove(EwlHash *hash, char *key)
{
	EwlListNode *ln;
	EwlHashNode *node;
	if (!hash)	{
		/* FIXME */
	} else {
		ln = ewl_list_find(hash, ewl_hash_get_cb, key);
		if (ln)	{
			node = (EwlHashNode*) ln;
			ewl_list_remove(hash, ln);
			ewl_hash_node_free(node);
		}
	}
	return;
}


EwlHashNode *ewl_hash_node_new(char *key, void *data)
{
	EwlHashNode *node = malloc(sizeof(EwlHashNode));
	if (node)	{
		node->key = ewl_string_dup(key);
		node->data = data;
	}
	return node;
}

void         ewl_hash_node_free(EwlHashNode *node)
{
	if (node->key) free(node->key);
	free(node);
	return;
}


void         ewl_hash_dump_cb(EwlListNode *node, void *data)
{
	EwlHashNode *hn = EWL_HASH_NODE(node);
	UNUSED(data);
	fprintf(stderr,"  %8p: \"%s\" => %8p\n", hn, hn->key, hn->data);
	return;
}

void         ewl_hash_dump(EwlHash *hash)
{
	fprintf(stderr,"ewl_hash_dump(%8p) {\n", hash);
	ewl_list_foreach(hash, ewl_hash_dump_cb, NULL);
	fprintf(stderr,"}\n");
	return;
}
