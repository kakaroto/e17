#include <Ewd.h>

#define EWD_HASH_INCREASE(hash) ((hash && hash->size < PRIME_MAX) ? \
		((double)hash->nodes / (double)ewd_prime_table[hash->size]) > \
		EWD_HASH_CHAIN_MAX : FALSE)
#define EWD_HASH_REDUCE(hash) ((hash && hash->size > PRIME_MIN) ? \
		(double)hash->nodes / (double)ewd_prime_table[hash->size-1] \
		< ((double)EWD_HASH_CHAIN_MAX * 3.0 / 8.0) : FALSE)

#define EWD_HASH_KEY_IN_TABLE(table, key) (table == NULL ? FALSE : \
		((key < (table->base + table->size)) && (key >= table->base)))

#define FIND_TABLE(index) (index > PRIME_MIN ? \
                ((log((double)index) / log(2.0)) - 4) : 0)


/* Private hash manipulation functions */
static int ewd_hash_add_node(Ewd_Hash *hash, Ewd_Hash_Node *node);
static Ewd_Hash_Node * ewd_hash_get_node(Ewd_Hash *hash, void *key);
static int ewd_hash_increase(Ewd_Hash *hash);
static int ewd_hash_decrease(Ewd_Hash *hash);
static Ewd_Hash_Node *ewd_hash_get_with_size(Ewd_Hash *hash, void *key,
		int size);
static int ewd_hash_find_table(Ewd_Hash *hash, unsigned int index);
static Ewd_Hash_Node *ewd_hash_get_bucket(Ewd_Hash *hash, Ewd_List *bucket,
		void *key);
static int ewd_hash_table_rehash(Ewd_Hash *hash, Ewd_Hash_Table *table);


/* Private hash table manipulation functions */
static Ewd_Hash_Table *ewd_hash_table_new(int size);
static int ewd_hash_table_init(Ewd_Hash_Table *table, int size);
static int ewd_hash_table_destroy(Ewd_Hash_Table *table, Ewd_Free_Cb keyd,
		Ewd_Free_Cb valued);
static Ewd_Hash_Node *ewd_hash_node_new(void *key, void *value);
static int ewd_hash_node_init(Ewd_Hash_Node *node, void *key, void *value);
static int ewd_hash_node_destroy(Ewd_Hash_Node *node, Ewd_Free_Cb keyd,
		Ewd_Free_Cb valued);


/*
 * Description: Create and initialize a new hash
 * Parameters: 1. hash_func - the function for determining hash position
 *             2. compare - the function for comparing node keys
 * Returns: NULL on error, a new hash on success
 */
Ewd_Hash *ewd_hash_new(Ewd_Hash_Cb hash_func, Ewd_Compare_Cb compare)
{
	Ewd_Hash *new = (Ewd_Hash *)malloc(sizeof(Ewd_Hash));

	if (!ewd_hash_init(new, hash_func, compare)) {
		FREE(new);
		return NULL;
	}

	return new;
}

/*
 * Description: Initialize a hash to some sane starting values
 * Parameters: 1. hash - the hash table to initialize
 *             2. compare - the function for comparing node keys
 * Returns: TRUE on success, FALSE on an error.
 */
int ewd_hash_init(Ewd_Hash *hash, Ewd_Hash_Cb hash_func, Ewd_Compare_Cb compare)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	memset(hash, 0, sizeof(Ewd_Hash));

	if (hash_func)
		hash->hash_func = hash_func;
	else
		hash->hash_func = ewd_direct_hash;

	if (compare)
		hash->compare = compare;

	hash->tables[0] = ewd_hash_table_new(ewd_prime_table[0]);

	EWD_INIT_LOCKS(hash);

	return TRUE;
}

/*
 * Description: Set the function called when node's are free'd to free keys
 * Parameters: 1. hash - the hash that this will affect
 *             2. function - the function that will free the node keys
 * Returns: TRUE on success, FALSE on error
 */
int ewd_hash_set_free_key(Ewd_Hash *hash, Ewd_Free_Cb function)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("function", function, FALSE);

	hash->free_key = function;

	return TRUE;
}

/*
 * Description: Set the function called when node's are free'd to free values
 * Parameters: 1. hash - the hash that this will affect
 *             2. function - the function that will free the node values
 * Returns: TRUE on success, FALSE on error
 */
int ewd_hash_set_free_value(Ewd_Hash *hash, Ewd_Free_Cb function)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("function", function, FALSE);

	hash->free_value = function;

	return TRUE;
}

/*
 * Description: Set the key/value pair in the hash table, if node doesn't exist
 *              then create it.
 * Parameters: 1. hash - the hash table to set the the value in
 *             2. key - the key for this value pair
 *             3. value - the value corresponding with the key
 * Returns: TRUE if successful, FALSE if not
 */
int ewd_hash_set(Ewd_Hash *hash, void *key, void *value)
{
	int ret = FALSE;
	Ewd_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	node = ewd_hash_get_node(hash, key);
	if (node)
		node->value = value;
	else {
		node = ewd_hash_node_new(key, value);
		if (node)
			ret = ewd_hash_add_node(hash, node);
	}

	return ret;
}

/*
 * Description: Free the hash table and the data contained inside it
 * Parameters: 1. hash - the hash table to destroy
 * Returns: TRUE on success, FALSE on error
 */
void ewd_hash_destroy(Ewd_Hash *hash)
{
	int i = 0;

	CHECK_PARAM_POINTER("hash", hash);

	EWD_WRITE_LOCK_STRUCT(hash);

	while (i <= hash->size) {
		ewd_hash_table_destroy(hash->tables[i], hash->free_key,
				hash->free_value);
		i++;
	}

	EWD_WRITE_UNLOCK_STRUCT(hash);
	EWD_DESTROY_LOCKS(hash);

	FREE(hash);

	return;
}

/*
 * Description: Free the hash table and the data contained inside it
 * Parameters: 1. table - the table to destroy
 * Returns: TRUE on success, FALSE on error
 */
static int
ewd_hash_table_destroy(Ewd_Hash_Table *table, Ewd_Free_Cb keyd,
		Ewd_Free_Cb valued)
{
	int i;
	Ewd_List *list;

	CHECK_PARAM_POINTER_RETURN("table", table, FALSE);

	for (i = 0; i < table->size; i++) {
		Ewd_Hash_Node *node;
		list = table->table[i];

		if (list) {
			ewd_list_goto_first(list);
			while ((node = EWD_HASH_NODE(ewd_list_next(list)))
					!= NULL)
				ewd_hash_node_destroy(node, keyd, valued);
		}
	}

	return TRUE;
}

/*
 * Description: Add the node to the hash table
 * Parameters: 1. hash - the hash table to add the key
 *             2. node - the node to add to the hash table
 * Returns: FALSE on error, TRUE on success
 */
static int
ewd_hash_add_node(Ewd_Hash *hash, Ewd_Hash_Node *node)
{
	int i = 0;
	Ewd_Hash_Table *table;
	unsigned int hash_val;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	/* Check to see if the hash needs to be resized */
	if (EWD_HASH_INCREASE(hash))
		ewd_hash_increase(hash);
	else if (EWD_HASH_REDUCE(hash))
		ewd_hash_decrease(hash);

	/* Compute the position in the table */
	hash_val = hash->hash_func(node->key) % ewd_prime_table[hash->size];

	/* Traverse the list of tables until the one containing calculated
	 * index is reached */
	/*
	while (i <= hash->size && !EWD_HASH_KEY_IN_TABLE(hash->tables[i],
				hash_val))
		i++;
	*/
	i = FIND_TABLE(hash_val);
	table = hash->tables[i];
	if (!table)
		return FALSE;

	while (table->base + table->size < hash_val) {
		i++;
		table = hash->tables[i];
		if (!table)
			return FALSE;
	}

	/* Find the index into the table and create a list if none exists */
	hash_val -= table->base;
	if (!table->table[hash_val])
		table->table[hash_val] = ewd_list_new();

	/* Append the node to the list at the index position */
	if (!ewd_list_append(table->table[hash_val], node))
		return FALSE;

	hash->nodes++;

	return TRUE;
}

/*
 * Description: Retrieve the value associated with key
 * Parameters: 1. hash - the hash table to search for the key
 *             2. key - the key to search for in the hash table
 * Returns: NULL on error, value corresponding to key on success
 */
void *ewd_hash_get(Ewd_Hash *hash, void *key)
{
	void *data = NULL;
	Ewd_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	node = ewd_hash_get_node(hash, key);

	EWD_READ_LOCK_STRUCT(node);
	data = (node ? node->value : NULL);
	EWD_READ_UNLOCK_STRUCT(node);

	return data;
}

/*
 * Description: Retrieve the node associated with key
 * Parameters: 1. hash - the hash table to search for the key
 *             2. key - the key to search for in the hash table
 * Returns: NULL on error, node corresponding to key on success
 */
static Ewd_Hash_Node *
ewd_hash_get_node(Ewd_Hash *hash, void *key)
{
	int size;
	Ewd_Hash_Node *node = NULL;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	size = hash->size;
	do {
		node = ewd_hash_get_with_size(hash, key, ewd_prime_table[size]);
		size--;
	} while (size >= 0 && !node);

	return node;
}

/*
 * Description: Search for a node using the given table size
 * Parameters: 1. hash - the hash table to search for the key
 *             2. key - the key to search the hash table
 *             3. size - the size of the hash table for this search
 * Returns: The found node on success, NULL on error or not found
 */
static Ewd_Hash_Node *
ewd_hash_get_with_size(Ewd_Hash *hash, void *key, int size)
{
	unsigned int hash_val;
	Ewd_Hash_Table *table = NULL;
	Ewd_List *list = NULL;
	Ewd_Hash_Node *node = NULL;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	hash_val = hash->hash_func(key) % size;
	table = hash->tables[ewd_hash_find_table(hash, hash_val)];
/*	printf("%d: %d\n", hash_val, ewd_hash_find_table(hash, hash_val)); */
	if (table) {
		hash_val -= table->base;
		list = table->table[hash_val];
	}

	if (list)
		node = ewd_hash_get_bucket(hash, list, key);

	return node;
}

/*
 * Description: Determine the table that contains the given index
 * Parameters: 1. hash - the hash table to search
 *             2. index - the index to determine the needed table
 * Returns: -1 on error, a hash table index number on success
 */
static int
ewd_hash_find_table(Ewd_Hash *hash, unsigned int index)
{
	int i;

	CHECK_PARAM_POINTER_RETURN("hash", hash, -1);

	i = FIND_TABLE(index);
	while (index >= ewd_prime_table[i] && i <= hash->size)
		i++;

	return i;
}

/*
 * Description: Search the hash bucket for a specified key
 * Parameters: 1. hash - the hash table to retrieve the comparison function
 *             2. bucket - the list to search for the key
 *             3. key - the key to search for in the list
 * Returns: NULL on error or not found, the found node on success
 */
static Ewd_Hash_Node *
ewd_hash_get_bucket(Ewd_Hash *hash, Ewd_List *bucket, void *key)
{
	Ewd_Hash_Node *node = NULL;

	if (hash->compare) {
		while ((node = ewd_list_next(bucket)) != NULL) {
			if (hash->compare(node->key, key) == 0)
				return node;
		}
	}
	else {
		while ((node = ewd_list_next(bucket)) != NULL) {
			if (node->key == key)
				return node;
		}
	}

	return NULL;
}

/*
 * Description: Increase the size of the hash table by > 2 * current size
 * Parameters: 1. hash - the hash table to increase the size of
 * Returns: TRUE on success, FALSE on error
 */
static int
ewd_hash_increase(Ewd_Hash *hash)
{
	Ewd_Hash_Table *table, *last;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	if (hash->size == PRIME_TABLE_MAX)
		return FALSE;

	table = ewd_hash_table_new(ewd_prime_table[hash->size + 1] -
			ewd_prime_table[hash->size]);
	if (!table)
		return FALSE;

	last = hash->tables[hash->size];
	if (last)
		table->base = last->base + last->size;

	hash->size++;
	hash->tables[hash->size] = table;

	return TRUE;
}

/*
 * Description: Decrease the size of the hash table by < 1/2 * current size
 * Parameters: 1. hash - the hash table to decrease the size of
 * Returns: TRUE on success, FALSE on error
 */
static int
ewd_hash_decrease(Ewd_Hash *hash)
{
	Ewd_Hash_Table *table;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	if (ewd_prime_table[hash->size] == PRIME_MIN)
		return FALSE;

	table = hash->tables[hash->size];
	hash->size--;
	ewd_hash_table_rehash(hash, table);
	ewd_hash_table_destroy(table, hash->free_key, hash->free_value);

	return TRUE;
}

/*
 * Description: Rehash the nodes of a table into the hash table
 * Parameters: 1. hash - the hash to place the nodes of the table
 *             2. table - the table to remove the nodes from and place in hash
 * Returns: TRUE on success, FALSE on success
 */
static int
ewd_hash_table_rehash(Ewd_Hash *hash, Ewd_Hash_Table *table)
{
	int i;
	Ewd_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("table", table, FALSE);

	for (i = 0; i < table->size; i++) {
		while ((node = EWD_HASH_NODE(ewd_list_remove(table->table[i])))
				!= NULL)
			ewd_hash_add_node(hash, node);
		ewd_list_destroy(table->table[i]);
		table->table[i] = NULL;
	}

	return TRUE;
}

/*
 * Description: Create and initialize a new table
 * Parameters: 1. size - the size of the table to be created
 * Returns: NULL on error, a new table on success
 */
static Ewd_Hash_Table *
ewd_hash_table_new(int size)
{
	Ewd_Hash_Table *table;

	table = (Ewd_Hash_Table *)malloc(sizeof(Ewd_Hash_Table));
	if (!table)
		return NULL;

	if (!ewd_hash_table_init(table, size)) {
		FREE(table);
		return NULL;
	}

	return table;
}

/*
 * Description: Initialize a table to some sane starting value
 * Parameters: 1. table - the table to initialize
 * Returns: TRUE on success, FALSE on error
 */
static int
ewd_hash_table_init(Ewd_Hash_Table *table, int size)
{
	CHECK_PARAM_POINTER_RETURN("table", table, FALSE);

	memset(table, 0, sizeof(Ewd_Hash_Table));

	table->table = (Ewd_List **)malloc(size * sizeof(Ewd_List *));
	if (!table->table)
		return FALSE;

	table->size = size;

	return TRUE;
}

/*
 * Description: Create a new hash node for key and value storage
 * Parameters: 1. key - the key for this node
 *             2. value - the value that the key references
 * Returns: NULL on error, a new hash node on success
 */
static Ewd_Hash_Node *
ewd_hash_node_new(void *key, void *value)
{
	Ewd_Hash_Node *node;

	node = (Ewd_Hash_Node *)malloc(sizeof(Ewd_Hash_Node));
	if (!node)
		return NULL;

	if (!ewd_hash_node_init(node, key, value)) {
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
ewd_hash_node_init(Ewd_Hash_Node *node, void *key, void *value)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

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
ewd_hash_node_destroy(Ewd_Hash_Node *node, Ewd_Free_Cb keyd, Ewd_Free_Cb valued)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	if (keyd)
		keyd(node->key);

	if (valued)
		valued(node->value);

	FREE(node);

	return TRUE;
}
