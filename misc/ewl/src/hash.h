#ifndef _HASH_H_
#define _HASH_H_

#include "includes.h"
#include "error.h"
#include "util.h"
#include "list.h"

#define EWL_HASH_OPTIMIZE_REF_COUNT 1024

#define EWL_HASH(a) ((EwlHash*)a)
#define EWL_HASH_NODE(a) ((EwlHashNode*)a)

typedef struct _EwlHashNode EwlHashNode;
typedef EwlList             EwlHash;

struct _EwlHashNode	{
	EwlListNode  node;
	char        *key;
	void        *data;
};

/* HASH FUNCTIONS */
EwlHash     *ewl_hash_new();
void         ewl_hash_free(EwlHash *hash);

/* HASH GET/SET FUNCTIONS */
void        *ewl_hash_get(EwlHash *hash, char *key);
void         ewl_hash_set(EwlHash *hash, char *key, void *data);
void         ewl_hash_remove(EwlHash *hash, char *key);

/* HASH NODE FUNCTIONS */
EwlHashNode *ewl_hash_node_new(char *key, void *data);
void         ewl_hash_node_free(EwlHashNode *node);

/* HASH DEBUGGING FUNCTIONS */
void         ewl_hash_dump(EwlHash *hash);

#endif /* _HASH_H_ */
