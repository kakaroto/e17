#include <Ewd.h>

#define EWD_HASH_CHAIN_MAX 3

#define EWD_COMPUTE_HASH(hash, key) hash->hash_func(key) % \
					ewd_prime_table[hash->size];

#define EWD_HASH_INCREASE(hash) ((hash && hash->size < PRIME_MAX) ? \
		(hash->nodes / ewd_prime_table[hash->size]) > \
		EWD_HASH_CHAIN_MAX : FALSE)
#define EWD_HASH_REDUCE(hash) ((hash && hash->size > PRIME_MIN) ? \
		(double)hash->nodes / (double)ewd_prime_table[hash->size-1] \
		< ((double)EWD_HASH_CHAIN_MAX * 0.375) : FALSE)


/* Private hash manipulation functions */
static int _ewd_hash_add_node(Ewd_Hash *hash, Ewd_Hash_Node *node);
static Ewd_Hash_Node * _ewd_hash_get_node(Ewd_Hash *hash, void *key);
static int _ewd_hash_increase(Ewd_Hash *hash);
static int _ewd_hash_decrease(Ewd_Hash *hash);
inline int _ewd_hash_rehash(Ewd_Hash *hash, Ewd_List **old_table, int old_size);
static int _ewd_hash_bucket_destroy(Ewd_List *list, Ewd_Free_Cb keyd,
		Ewd_Free_Cb valued);
inline Ewd_Hash_Node * _ewd_hash_get_bucket(Ewd_Hash *hash, Ewd_List *bucket,
		void *key);

static Ewd_Hash_Node *_ewd_hash_node_new(void *key, void *value);
static int _ewd_hash_node_init(Ewd_Hash_Node *node, void *key, void *value);
static int _ewd_hash_node_destroy(Ewd_Hash_Node *node, Ewd_Free_Cb keyd,
		Ewd_Free_Cb valued);


/**
 * ewd_hash_new - create and initialize a new hash
 * @hash_func: the function for determining hash position
 * @compare: the function for comparing node keys
 *
 * Returns NULL on error, a new hash on success
 */
Ewd_Hash *ewd_hash_new(Ewd_Hash_Cb hash_func, Ewd_Compare_Cb compare)
{
	Ewd_Hash *new_hash = (Ewd_Hash *)malloc(sizeof(Ewd_Hash));
	if (!new_hash)
		return NULL;

	if (!ewd_hash_init(new_hash, hash_func, compare)) {
		FREE(new_hash);
		return NULL;
	}

	return new_hash;
}

/**
 * ewd_hash_init - initialize a hash to some sane starting values
 * @hash: the hash table to initialize
 * @hash_func: the function for hashing node keys
 * @compare: the function for comparing node keys
 *
 * Returns TRUE on success, FALSE on an error.
 */
int ewd_hash_init(Ewd_Hash *hash, Ewd_Hash_Cb hash_func, Ewd_Compare_Cb compare)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	memset(hash, 0, sizeof(Ewd_Hash));

	hash->hash_func = hash_func;
	hash->compare = compare;

	hash->buckets = (Ewd_List **)malloc(ewd_prime_table[0] *
			sizeof(Ewd_List *));
	memset(hash->buckets, 0, ewd_prime_table[0] * sizeof(Ewd_List *));

	EWD_INIT_LOCKS(hash);

	return TRUE;
}

/**
 * ewd_hash_set_free_key - set the function to destroy the keys of entries
 * @hash: the hash that this will affect
 * @function: the function that will free the node keys
 *
 * Returns TRUE on success, FALSE on error
 */
int ewd_hash_set_free_key(Ewd_Hash *hash, Ewd_Free_Cb function)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("function", function, FALSE);

	EWD_WRITE_LOCK(hash);
	hash->free_key = function;
	EWD_WRITE_UNLOCK(hash);

	return TRUE;
}

/**
 * ewd_hash_set_free_value - set the function to destroy the value
 * @hash: the hash that this will affect
 * @function: the function that will free the node values
 *
 * Returns TRUE on success, FALSE on error
 */
int ewd_hash_set_free_value(Ewd_Hash *hash, Ewd_Free_Cb function)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("function", function, FALSE);

	EWD_WRITE_LOCK(hash);
	hash->free_value = function;
	EWD_WRITE_UNLOCK(hash);

	return TRUE;
}

/**
 * ewd_hash_set - set the key/value pair in the hash table
 * @hash: the hash table to set the the value in
 * @key: the key for this value pair
 * @value: the value corresponding with the key
 *
 * Returns TRUE if successful, FALSE if not
 */
int ewd_hash_set(Ewd_Hash *hash, void *key, void *value)
{
	int ret = FALSE;
	Ewd_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	EWD_WRITE_LOCK(hash);
	node = _ewd_hash_get_node(hash, key);
	if (node)
		node->value = value;
	else {
		node = _ewd_hash_node_new(key, value);
		if (node)
			ret = _ewd_hash_add_node(hash, node);
	}
	EWD_WRITE_UNLOCK(hash);

	return ret;
}

/**
 * ewd_hash_destroy - free the hash table and the data contained inside it
 * @hash: the hash table to destroy
 *
 * Returns TRUE on success, FALSE on error
 */
void ewd_hash_destroy(Ewd_Hash *hash)
{
	int i = 0;

	CHECK_PARAM_POINTER("hash", hash);

	EWD_WRITE_LOCK(hash);

	while (i < ewd_prime_table[hash->size]) {
		if (hash->buckets[i])
			_ewd_hash_bucket_destroy(hash->buckets[i],
					hash->free_key, hash->free_value);
		i++;
	}

	FREE(hash->buckets);

	EWD_WRITE_UNLOCK(hash);
	EWD_DESTROY_LOCKS(hash);

	FREE(hash);

	return;
}

/**
 * ewd_hash_dump_graph - print the distribution of the hash table for graphing
 * @hash: the hash table to print
 *
 * Returns no value.
 */
void
ewd_hash_dump_graph(Ewd_Hash *hash)
{
	int i;

	for (i = 0; i < ewd_prime_table[hash->size]; i++)
		if (hash->buckets[i])
			printf("%d\t%u\n", i, ewd_list_nodes(hash->buckets[i]));
		else
			printf("%d\t0\n", i);
}

static int
_ewd_hash_bucket_destroy(Ewd_List *list, Ewd_Free_Cb keyd, Ewd_Free_Cb valued)
{
	Ewd_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	while ((node = ewd_list_remove_first(list)) != NULL)
		_ewd_hash_node_destroy(node, keyd, valued);

	ewd_list_destroy(list);

	return TRUE;
}

/*
 * Description: Add the node to the hash table
 * Parameters: 1. hash - the hash table to add the key
 *             2. node - the node to add to the hash table
 * Returns: FALSE on error, TRUE on success
 */
static int
_ewd_hash_add_node(Ewd_Hash *hash, Ewd_Hash_Node *node)
{
	unsigned int hash_val;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	/* Check to see if the hash needs to be resized */
	if (EWD_HASH_INCREASE(hash))
		_ewd_hash_increase(hash);

	/* Compute the position in the table */
	if (!hash->hash_func)
		hash_val = (unsigned int)node->key %
			ewd_prime_table[hash->size];
	else
		hash_val = EWD_COMPUTE_HASH(hash, node->key);

	/* Create the list if it's not already present */
	if (!hash->buckets[hash_val])
		hash->buckets[hash_val] = ewd_list_new();

	/* Append the node to the list at the index position */
	if (!ewd_list_prepend(hash->buckets[hash_val], node))
		return FALSE;
	hash->nodes++;

	return TRUE;
}

/**
 * ewd_hash_get - retrieve the value associated with key
 * @hash: the hash table to search for the key
 * @key: the key to search for in the hash table
 *
 * Returns NULL on error, value corresponding to key on success
 */
void *ewd_hash_get(Ewd_Hash *hash, void *key)
{
	void *data = NULL;
	Ewd_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	node = _ewd_hash_get_node(hash, key);
	if (!node)
		return NULL;

	EWD_READ_LOCK(node);
	data = node->value;
	EWD_READ_UNLOCK(node);

	return data;
}


/**
 * ewd_hash_remove - remove the value associated with key
 * @hash: the hash table to remove the key from
 * @key: the key to search for in the hash table
 *
 * Returns NULL on error, value corresponding to key on success
 */
void *ewd_hash_remove(Ewd_Hash *hash, void *key)
{
	Ewd_Hash_Node *node = NULL;
	Ewd_List *list;
	unsigned int hash_val;
	void *ret = NULL;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	EWD_WRITE_LOCK(hash);

	/* Compute the position in the table */
	if (!hash->hash_func)
		hash_val = (unsigned int)key % ewd_prime_table[hash->size];
	else
		hash_val = EWD_COMPUTE_HASH(hash, key);

	/*
	 * If their is a list that could possibly hold the key/value pair
	 * traverse it and remove the hash node.
	 */
	if (hash->buckets[hash_val]) {
		list = hash->buckets[hash_val];
		ewd_list_goto_first(list);

		/*
		 * Traverse the list to find the specified key
		 */
		if (hash->compare) {
			while ((node = ewd_list_current(list)) &&
					hash->compare(node->key, key) != 0)
				ewd_list_next(list);
		}
		else {
			while ((node = ewd_list_current(list)) &&
					node->key != key)
				ewd_list_next(list);
		}

		if (node) {
			ewd_list_remove(list);

			ret = node->value;
			FREE(node);
		}
	}

	if (EWD_HASH_REDUCE(hash))
		_ewd_hash_decrease(hash);

	EWD_WRITE_UNLOCK(hash);

	return ret;
}

/*
 * Description: Retrieve the node associated with key
 * Parameters: 1. hash - the hash table to search for the key
 *             2. key - the key to search for in the hash table
 * Returns: NULL on error, node corresponding to key on success
 */
static Ewd_Hash_Node *
_ewd_hash_get_node(Ewd_Hash *hash, void *key)
{
	unsigned int hash_val;
	Ewd_Hash_Node *node = NULL;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	EWD_READ_LOCK(hash);

	/* Compute the position in the table */
	if (!hash->hash_func)
		hash_val = (unsigned int)key % ewd_prime_table[hash->size];
	else
		hash_val = EWD_COMPUTE_HASH(hash, key);

	/* Grab the bucket at the specified position */
	if (hash->buckets[hash_val])
		node = _ewd_hash_get_bucket(hash, hash->buckets[hash_val], key);

	EWD_READ_UNLOCK(hash);

	return node;
}

/*
 * Description: Search the hash bucket for a specified key
 * Parameters: 1. hash - the hash table to retrieve the comparison function
 *             2. bucket - the list to search for the key
 *             3. key - the key to search for in the list
 * Returns: NULL on error or not found, the found node on success
 */
inline Ewd_Hash_Node *
_ewd_hash_get_bucket(Ewd_Hash *hash, Ewd_List *bucket, void *key)
{
	Ewd_Hash_Node *node = NULL;

	EWD_READ_LOCK(hash);
	ewd_list_goto_first(bucket);

	/*
	 * Traverse the list to find the desired node, if the node is in the
	 * list, then return the node.
	 */
	if (hash->compare) {
		while ((node = ewd_list_next(bucket)) != NULL) {
			EWD_READ_LOCK(node);
			if (hash->compare(node->key, key) == 0) {
				EWD_READ_UNLOCK(node);
				EWD_READ_UNLOCK(hash);
				return node;
			}
			EWD_READ_UNLOCK(node);
		}
	}
	else {
		while ((node = ewd_list_next(bucket)) != NULL) {
			EWD_READ_LOCK(node);
			if (node->key == key) {
				EWD_READ_UNLOCK(node);
				EWD_READ_UNLOCK(hash);
				return node;
			}
			EWD_READ_UNLOCK(node);
		}
	}
	EWD_READ_UNLOCK(hash);

	return NULL;
}

/*
 * Description: Increase the size of the hash table by approx.  2 * current size
 * Parameters: 1. hash - the hash table to increase the size of
 * Returns: TRUE on success, FALSE on error
 */
static int
_ewd_hash_increase(Ewd_Hash *hash)
{
	void *old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	/* Max size reached so return FALSE */
	if (hash->size == PRIME_TABLE_MAX)
		return FALSE;

	/*
	 * Increase the size of the hash and save a pointer to the old data
	 */
	hash->size++;
	old = hash->buckets;

	/*
	 * Allocate a new bucket area, of the new larger size
	 */
	hash->buckets = (Ewd_List **)calloc(ewd_prime_table[hash->size],
			sizeof(Ewd_List *));

	/*
	 * Make sure the allocation succeeded, if not replace the old data and
	 * return a failure.
	 */
	if (!hash->buckets) {
		hash->buckets = old;
		hash->size--;
		return FALSE;
	}
	hash->nodes = 0;

	/*
	 * Now move all of the old data into the new bucket area
	 */
	if (_ewd_hash_rehash(hash, old, hash->size - 1)) {
		FREE(old);
		return TRUE;
	}

	/*
	 * Free the old buckets regardless of success.
	 */
	FREE(old);

	return FALSE;
}

/*
 * Description: Decrease the size of the hash table by < 1/2 * current size
 * Parameters: 1. hash - the hash table to decrease the size of
 * Returns: TRUE on success, FALSE on error
 */
static int
_ewd_hash_decrease(Ewd_Hash *hash)
{
	Ewd_List **old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	if (ewd_prime_table[hash->size] == PRIME_MIN)
		return FALSE;

	/*
	 * Decrease the hash size and store a pointer to the old data
	 */
	hash->size--;
	old = hash->buckets;

	/*
	 * Allocate a new area to store the data
	 */
	hash->buckets = (Ewd_List **)malloc(ewd_prime_table[hash->size] *
			sizeof(Ewd_List *));

	/*
	 * Make sure allocation succeeded otherwise rreturn to the previous
	 * state
	 */
	if (!hash->buckets) {
		hash->buckets = old;
		hash->size++;
		return FALSE;
	}

	/*
	 * Zero out the new area
	 */
	memset(hash->buckets, 0, ewd_prime_table[hash->size]
			* sizeof(Ewd_List *));
	hash->nodes = 0;

	if (_ewd_hash_rehash(hash, old, hash->size - 1)) {
		FREE(old);
		return TRUE;
	}

	return FALSE;
}

/*
 * Description: Rehash the nodes of a table into the hash table
 * Parameters: 1. hash - the hash to place the nodes of the table
 *             2. table - the table to remove the nodes from and place in hash
 * Returns: TRUE on success, FALSE on success
 */
inline int
_ewd_hash_rehash(Ewd_Hash *hash, Ewd_List **old_table, int old_size)
{
	int i;
	Ewd_Hash_Node *node;
	Ewd_List *old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("old_table", old_table, FALSE);

	for (i = 0; i < ewd_prime_table[old_size]; i++) {
		/* Hash into a new list to avoid loops of rehashing the same
		 * nodes */
		old = old_table[i];
		old_table[i] = NULL;

		/* Loop through re-adding each node to the hash table */
		while (old && (node = ewd_list_remove_last(old))) {
			_ewd_hash_add_node(hash, node);
		}

		/* Now free up the old list space */
		if (old)
			ewd_list_destroy(old);
	}

	return TRUE;
}

/*
 * Description: Create a new hash node for key and value storage
 * Parameters: 1. key - the key for this node
 *             2. value - the value that the key references
 * Returns: NULL on error, a new hash node on success
 */
static Ewd_Hash_Node *
_ewd_hash_node_new(void *key, void *value)
{
	Ewd_Hash_Node *node;

	node = (Ewd_Hash_Node *)malloc(sizeof(Ewd_Hash_Node));
	if (!node)
		return NULL;

	if (!_ewd_hash_node_init(node, key, value)) {
		FREE(node);
		return NULL;
	}

	return node;
}

/*
 * Description: Initialize a hash node to some sane default values
 * Parameters: 1. node - the node to set the values
 *             2. key - the key to reference this node
 *             3. value - the value that key refers to
 * Returns: TRUE on success, FALSE on error
 */
static int
_ewd_hash_node_init(Ewd_Hash_Node *node, void *key, void *value)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	EWD_INIT_LOCKS(node);
	node->key = key;
	node->value = value;

	return TRUE;
}

/*
 * Description: Destroy a node and call the specified callbacks to free data
 * Parameters: 1. node - the node to be destroyed
 *             2. keyd - the function to free the key
 *             3. valued - the function  to free the value
 * Returns: TRUE on success, FALSE on error
 */
static int
_ewd_hash_node_destroy(Ewd_Hash_Node *node, Ewd_Free_Cb keyd,
		Ewd_Free_Cb valued)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	if (keyd)
		keyd(node->key);

	if (valued)
		valued(node->value);

	FREE(node);

	return TRUE;
}
