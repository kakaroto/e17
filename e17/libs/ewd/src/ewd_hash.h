#ifndef _EWD_HASH_H
#define _EWD_HASH_H

/*
 * Hash Table Implementation:
 * 
 * This hash table implementation uses a combination of techniques for resolving
 * collision issues. It uses a combination of separate chaining and re-hashing.
 * Separate chaining is used for resolving collisions as data is inserted into
 * the table. Re-hashing is used for retrieving data from the table, as the
 * table size may change.
 *
 * The table size is changed by incrementing the size of the table to the next
 * prime number > 2 * current size. Hopefully, this will act as a pre-caching
 * mechanism. Then a table is constructed of size (old size - new size) and
 * added to the list of tables. Data is retrieved from the hash by first using
 * the current size as the modulo operator. If the data is not found in the
 * list located at the calculated position, previous sizes are tried until the
 * data is found or there are no previous sizes available to try. The previous
 * sizes are easily obtained from the ewd_prime_table in ewd_value.c.
 *
 * The main advantage of this scheme is that when the table size needs to be
 * increased, the data does not need to be recopied into the new table. When
 * the table size is decreased, the only data copied is that in the last
 * table.
 *
 * The disadvantage of this scheme is that searching time may be adversely
 * affected because of the multiple possible hash positions. The fact that
 * each table size is a prime number should cut down on this overhead.
 *
 * Once the scheme is filled out a little more, I plan to run some tests
 * comparing this hash table scheme vs. the glib hash table scheme.
 */

#define EWD_HASH_CHAIN_MAX 3

typedef struct _ewd_hash_node Ewd_Hash_Node;
#define EWD_HASH_NODE(hash) ((Ewd_Hash_Node *)hash)

struct _ewd_hash_node {
	void *key;	/* The key for the data node */
	void *value;	/* The value associated with this node */

	EWD_DECLARE_LOCKS;
};

typedef struct _ewd_hash_table Ewd_Hash_Table;
#define EWD_HASH_TABLE(table) ((Ewd_Hash_Table *)table)

struct _ewd_hash_table {
	Ewd_List **table; /* The table of nodes */

	int base; /* The base number of the nodes in this table */
	int size; /* The size of this table */

	EWD_DECLARE_LOCKS;
};

typedef struct _ewd_hash Ewd_Hash;
#define EWD_HASH(hash) ((Ewd_Hash *)hash)

struct _ewd_hash {
	Ewd_Hash_Table *tables[PRIME_TABLE_MAX];
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

#endif
