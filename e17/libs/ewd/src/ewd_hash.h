#ifndef _EWD_HASH_H
#define _EWD_HASH_H

/*
 * Hash Table Implementation:
 * 
 * Traditional hash table implementation. I had tried a list of tables
 * approach to save on the realloc's but it ended up being much slower than
 * the traditional approach.
 */

typedef struct _ewd_hash_node Ewd_Hash_Node;
#define EWD_HASH_NODE(hash) ((Ewd_Hash_Node *)hash)

struct _ewd_hash_node {
	void *key;	/* The key for the data node */
	void *value;	/* The value associated with this node */

	EWD_DECLARE_LOCKS;
};

typedef struct _ewd_hash Ewd_Hash;
#define EWD_HASH(hash) ((Ewd_Hash *)hash)

struct _ewd_hash {
	Ewd_List **buckets;
	int size;		/* An index into the table of primes to
				   determine size */
	int nodes;		/* The number of nodes currently in the hash */

	Ewd_Compare_Cb compare;	/* The function used to compare node values */
	Ewd_Hash_Cb hash_func;	/* The function used to compare node values */

	Ewd_Free_Cb free_key;	/* The callback function to free key */
	Ewd_Free_Cb free_value;	/* The callback function to determine hash */

	EWD_DECLARE_LOCKS;
};

/* Create and initialize a hash */
Ewd_Hash *ewd_hash_new(Ewd_Hash_Cb hash_func, Ewd_Compare_Cb compare);
int ewd_hash_init(Ewd_Hash *hash, Ewd_Hash_Cb hash_func,
		Ewd_Compare_Cb compare);
void ewd_hash_destroy(Ewd_Hash *hash);

/* Retrieve and store data into the hash */
void *ewd_hash_get(Ewd_Hash *hash, void *key);
int ewd_hash_set(Ewd_Hash *hash, void *key, void *value);
void *ewd_hash_remove(Ewd_Hash *hash, void *key);

#endif
