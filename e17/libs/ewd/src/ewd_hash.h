#ifndef _EWD_HASH_H
#define _EWD_HASH_H

typedef struct _ewd_hash_node Ewd_Hash_Node;
#define EWD_HASH_NODE(hash) ((Ewd_Hash_Node *)hash)

struct _ewd_hash_node {
	Ewd_List *bucket;
	EWD_DECLARE_LOCK;
}

typedef struct _ewd_hash Ewd_Hash;
#define EWD_HASH(hash) ((Ewd_Hash *)hash)

struct _ewd_hash {
	Ewd_List_Node *table;
	EWD_DECLARE_LOCK;
}

#endif
