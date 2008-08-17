#ifndef _EDATA_HASH_H
#define _EDATA_HASH_H

/**
 * @defgroup Hash_Group Hash
  * Hash Table Implementation:
 * 
 * Traditional hash table implementation. I had tried a list of tables
 * approach to save on the realloc's but it ended up being much slower than
 * the traditional approach.
 * @{
 */

typedef struct _edata_hash_node Edata_Hash_Node;
# define EDATA_HASH_NODE(hash) ((Edata_Hash_Node *)hash)

struct _edata_hash_node {
	Edata_Hash_Node *next; /* Pointer to the next node in the bucket list */
	void *key;	     /* The key for the data node */
	void *value;	     /* The value associated with this node */
};

typedef struct _edata_hash Edata_Hash;
# define EDATA_HASH(hash) ((Edata_Hash *)hash)

struct _edata_hash {
	Edata_Hash_Node **buckets;
	int size;	/* An index into the table of primes to
			 determine size */
	int nodes;	/* The number of nodes currently in the hash */

	int index;    /* The current index into the bucket table */

	Edata_Compare_Cb compare;	/* The function used to compare node values */
	Edata_Hash_Cb hash_func;	/* The callback function to determine hash */

	Edata_Free_Cb free_key;	/* The callback function to free key */
	Edata_Free_Cb free_value;	/* The callback function to free value */
};

/**
 * @defgroup Edata_Data_Hash_ADT_Creation_Group Hash Creation Functions
 *
 * Functions that create hash tables.
 * Create and initialize a hash
 * @{
 */
EAPI Edata_Hash *edata_hash_new(Edata_Hash_Cb hash_func, Edata_Compare_Cb compare);
EAPI int edata_hash_init(Edata_Hash *hash, Edata_Hash_Cb hash_func, Edata_Compare_Cb compare);
/** @} */

/**
 * @defgroup Edata_Data_Hash_ADT_Destruction_Group Hash Destruction Functions
 *
 * Functions that destroy hash tables and their contents.
 * Functions related to freeing the data in the hash table
 * @{
 */
EAPI int edata_hash_free_key_cb_set(Edata_Hash *hash, Edata_Free_Cb function);
EAPI int edata_hash_free_value_cb_set(Edata_Hash *hash, Edata_Free_Cb function);
EAPI void edata_hash_destroy(Edata_Hash *hash);
/** @} */

/**
 * @defgroup Edata_Data_Hash_ADT_Traverse_Group Hash Traverse Functions
 *
 * Functions that iterate through hash tables.
 * @{
 */

EAPI int edata_hash_count(Edata_Hash *hash);
EAPI int edata_hash_for_each_node(Edata_Hash *hash, Edata_For_Each for_each_func,
			     void *user_data);
EAPI Edata_List *edata_hash_keys(Edata_Hash *hash);
/** @} */

/**
 * @defgroup Edata_Data_Hash_ADT_Data_Group Hash Data Functions
 *
 * Functions that set, access and delete values from the hash tables.
 * Retrieve and store data into the hash
 * @{
 */

EAPI void *edata_hash_get(Edata_Hash *hash, const void *key);
EAPI int edata_hash_set(Edata_Hash *hash, void *key, void *value);
EAPI int edata_hash_hash_set(Edata_Hash *hash, Edata_Hash *set);
EAPI void *edata_hash_remove(Edata_Hash *hash, const void *key);
EAPI void *edata_hash_find(Edata_Hash *hash, Edata_Compare_Cb compare, const void *value);
EAPI void edata_hash_dump_graph(Edata_Hash *hash);
EAPI void edata_hash_dump_stats(Edata_Hash *hash);
/** @} */
/** @} */
#endif /* _EDATA_HASH_H */
