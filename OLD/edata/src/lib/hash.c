#include "edata_private.h"
#include "Edata.h"

#define PRIME_TABLE_MAX 21
#define PRIME_MIN 17
#define PRIME_MAX 16777213

#define EDATA_HASH_CHAIN_MAX 3

#define EDATA_COMPUTE_HASH(hash, key) hash->hash_func(key) % \
					edata_prime_table[hash->size];

#define EDATA_HASH_INCREASE(hash) ((hash && edata_prime_table[hash->size] < PRIME_MAX) ? \
		(hash->nodes / edata_prime_table[hash->size]) > \
		EDATA_HASH_CHAIN_MAX : FALSE)
#define EDATA_HASH_REDUCE(hash) ((hash && edata_prime_table[hash->size] > PRIME_MIN) ? \
		(double)hash->nodes / (double)edata_prime_table[hash->size-1] \
		< ((double)EDATA_HASH_CHAIN_MAX * 0.375) : FALSE)

/* Private hash manipulation functions */
static int _edata_hash_add_node(Edata_Hash *hash, Edata_Hash_Node *node);
static Edata_Hash_Node * _edata_hash_get_node(Edata_Hash *hash, const void *key);
static int _edata_hash_increase(Edata_Hash *hash);
static int _edata_hash_decrease(Edata_Hash *hash);
inline int _edata_hash_rehash(Edata_Hash *hash, Edata_Hash_Node **old_table, int old_size);
static int _edata_hash_bucket_destroy(Edata_Hash_Node *list, Edata_Free_Cb keyd, Edata_Free_Cb valued);
inline Edata_Hash_Node * _edata_hash_get_bucket(Edata_Hash *hash, Edata_Hash_Node *bucket, const void *key);
static Edata_Hash_Node *_edata_hash_node_new(void *key, void *value);
static int _edata_hash_node_init(Edata_Hash_Node *node, void *key, void *value);
static int _edata_hash_node_destroy(Edata_Hash_Node *node, Edata_Free_Cb keyd, Edata_Free_Cb valued);

/**
 * Creates and initializes a new hash
 * @param hash_func The function for determining hash position.
 * @param compare   The function for comparing node keys.
 * @return @c NULL on error, a new hash on success.
 */
EAPI Edata_Hash *
edata_hash_new(Edata_Hash_Cb hash_func, Edata_Compare_Cb compare)
{
	Edata_Hash *new_hash = (Edata_Hash *)malloc(sizeof(Edata_Hash));
	if (!new_hash)
	return NULL;

	if (!edata_hash_init(new_hash, hash_func, compare))
	{
		FREE(new_hash);
		return NULL;
	}

	return new_hash;
}

/**
 * Initializes the given hash.
 * @param   hash       The given hash.
 * @param   hash_func  The function used for hashing node keys.
 * @param   compare    The function used for comparing node keys.
 * @return  @c TRUE on success, @c FALSE on an error.
 */
EAPI int edata_hash_init(Edata_Hash *hash, Edata_Hash_Cb hash_func,
		Edata_Compare_Cb compare)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	memset(hash, 0, sizeof(Edata_Hash));

	hash->hash_func = hash_func;
	hash->compare = compare;

	hash->buckets = (Edata_Hash_Node **)calloc(edata_prime_table[0],
			sizeof(Edata_Hash_Node *));

	return TRUE;
}

/**
 * Sets the function to destroy the keys of the given hash.
 * @param   hash     The given hash.
 * @param   function The function used to free the node keys. NULL is a
 *          valid value and means that no function will be called.
 * @return  @c TRUE on success, @c FALSE on error.
 */
EAPI int edata_hash_free_key_cb_set(Edata_Hash *hash, Edata_Free_Cb function)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	hash->free_key = function;

	return TRUE;
}

/**
 * Sets the function to destroy the values in the given hash.
 * @param   hash     The given hash.
 * @param   function The function that will free the node values. NULL is a
 *          valid value and means that no function will be called.
 * @return  @c TRUE on success, @c FALSE on error
 */
EAPI int edata_hash_free_value_cb_set(Edata_Hash *hash, Edata_Free_Cb function)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	hash->free_value = function;

	return TRUE;
}

/**
 * Sets a key-value pair in the given hash table.
 * @param   hash    The given hash table.
 * @param   key     The key.
 * @param   value   The value.
 * @return  @c TRUE if successful, @c FALSE if not.
 */
EAPI int edata_hash_set(Edata_Hash *hash, void *key, void *value)
{
	int ret = FALSE;
	Edata_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	node = _edata_hash_get_node(hash, key);
	if (node)
	{
		if (hash->free_key)
			hash->free_key(key);
		if (node->value && hash->free_value)
			hash->free_value(node->value);
		node->value = value;
		ret = TRUE;
	}
	else
	{
		node = _edata_hash_node_new(key, value);
		if (node)
			ret = _edata_hash_add_node(hash, node);
	}

	return ret;
}

/**
 * Sets all key-value pairs from set in the given hash table.
 * @param   hash    The given hash table.
 * @param   set     The hash table to import.
 * @return  @c TRUE if successful, @c FALSE if not.
 */
EAPI int edata_hash_hash_set(Edata_Hash *hash, Edata_Hash *set)
{
	unsigned int i;
	Edata_Hash_Node *node, *old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("set", set, FALSE);

	for (i = 0; i < edata_prime_table[set->size]; i++)
	{
		/* Hash into a new list to avoid loops of rehashing the same nodes */
		while ((old = set->buckets[i]))
		{
			set->buckets[i] = old->next;
			old->next = NULL;
			node = _edata_hash_get_node(hash, old->key);
			if (node)
			{
				/* This key already exists. Delete the old and add the new
				 * value */
				if (hash->free_key)
					hash->free_key(node->key);
				if (hash->free_value)
					hash->free_key(node->value);
				node->key = old->key;
				node->value = old->value;
				free(old);
			}
			else
				_edata_hash_add_node(hash, old);
		}
	}
	FREE(set->buckets);
	edata_hash_init(set, set->hash_func, set->compare);
	return TRUE;
}

/**
 * Frees the hash table and the data contained inside it.
 * @param   hash The hash table to destroy.
 * @return  @c TRUE on success, @c FALSE on error.
 */
EAPI void edata_hash_destroy(Edata_Hash *hash)
{
	unsigned int i = 0;

	CHECK_PARAM_POINTER("hash", hash);

	if (hash->buckets)
	{
		while (i < edata_prime_table[hash->size])
		{
			if (hash->buckets[i])
			{
				Edata_Hash_Node *bucket;

				/*
				 * Remove the bucket list to avoid possible recursion
				 * on the free callbacks.
				 */
				bucket = hash->buckets[i];
				hash->buckets[i] = NULL;
				_edata_hash_bucket_destroy(bucket, hash->free_key,
						hash->free_value);
			}
			i++;
		}

		FREE(hash->buckets);
	}
	FREE(hash);

	return;
}

/**
 * Counts the number of nodes in a hash table.
 * @param   hash The hash table to count current nodes.
 * @return  The number of nodes in the hash.
 */
EAPI int edata_hash_count(Edata_Hash *hash)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, 0);

	return hash->nodes;
}

/**
 * Runs the @p for_each_func function on each entry in the given hash.
 * @param   hash          The given hash.
 * @param   for_each_func The function that each entry is passed to.
 * @param		user_data			a pointer passed to calls of for_each_func
 * @return  TRUE on success, FALSE otherwise.
 */
EAPI int edata_hash_for_each_node(Edata_Hash *hash,
		Edata_For_Each for_each_func, void *user_data)
{
	unsigned int i = 0;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("for_each_func", for_each_func, FALSE);

	while (i < edata_prime_table[hash->size])
	{
		if (hash->buckets[i])
		{
			Edata_Hash_Node *node;

			for (node = hash->buckets[i]; node; node = node->next)
			{
				for_each_func(node, user_data);
			}
		}
		i++;
	}

	return TRUE;
}

/**
 * Retrieves an edata_list of all keys in the given hash.
 * @param   hash          The given hash.
 * @return  new edata_list on success, NULL otherwise
 */
EAPI Edata_List *
edata_hash_keys(Edata_Hash *hash)
{
	unsigned int i = 0;
	Edata_List *keys;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	keys = edata_list_new();
	while (i < edata_prime_table[hash->size])
	{
		if (hash->buckets[i])
		{
			Edata_Hash_Node *node;

			for (node = hash->buckets[i]; node; node = node->next)
			{
				edata_list_append(keys, node->key);
			}
		}
		i++;
	}
	edata_list_first_goto(keys);

	return keys;
}

/**
 * Prints the distribution of the given hash table for graphing.
 * @param hash The given hash table.
 */
EAPI void edata_hash_dump_graph(Edata_Hash *hash)
{
	unsigned int i;

	for (i = 0; i < edata_prime_table[hash->size]; i++)
		if (hash->buckets[i])
		{
			int n = 0;
			Edata_Hash_Node *node;
			for (node = hash->buckets[i]; node; node = node->next)
				n++;
			printf("%d\t%u\n", i, n);
		}
		else
			printf("%d\t0\n", i);
}

/**
 * Prints the distribution of the given hash table for graphing.
 * @param hash The given hash table.
 */
EAPI void edata_hash_dump_stats(Edata_Hash *hash)
{
	unsigned int i;
	double variance, sum_n_2 = 0, sum_n = 0;

	for (i = 0; i < edata_prime_table[hash->size]; i++)
	{
		if (hash->buckets[i])
		{
			int n = 0;
			Edata_Hash_Node *node;
			for (node = hash->buckets[i]; node; node = node->next)
				n++;
			sum_n_2 += ((double)n * (double)n);
			sum_n += (double)n;
		}
	}
	variance = (sum_n_2 - ((sum_n * sum_n) / (double)i)) / (double)i;
	printf("Average length: %f\n\tvariance^2: %f\n", (sum_n / (double)i),
			variance);
}

static int _edata_hash_bucket_destroy(Edata_Hash_Node *list,
		Edata_Free_Cb keyd, Edata_Free_Cb valued)
{
	Edata_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("list", list, FALSE);

	for (node = list; node; node = list)
	{
		list = list->next;
		_edata_hash_node_destroy(node, keyd, valued);
	}

	return TRUE;
}

/*
 * @brief Add the node to the hash table
 * @param hash: the hash table to add the key
 * @param node: the node to add to the hash table
 * @return Returns FALSE on error, TRUE on success
 */
static int _edata_hash_add_node(Edata_Hash *hash, Edata_Hash_Node *node)
{
	unsigned int hash_val;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	/* Check to see if the hash needs to be resized */
	if (EDATA_HASH_INCREASE(hash))
		_edata_hash_increase(hash);

	/* Compute the position in the table */
	if (!hash->hash_func)
		hash_val = (unsigned int)node->key % edata_prime_table[hash->size];
	else
		hash_val = EDATA_COMPUTE_HASH(hash, node->key);

	/* Prepend the node to the list at the index position */
	node->next = hash->buckets[hash_val];
	hash->buckets[hash_val] = node;
	hash->nodes++;

	return TRUE;
}

/**
 * Retrieves the value associated with the given key from the given hash
 * table.
 * @param   hash The given hash table.
 * @param   key  The key to search for.
 * @return  The value corresponding to key on success, @c NULL otherwise.
 */
EAPI void * edata_hash_get(Edata_Hash *hash, const void *key)
{
	void *data;
	Edata_Hash_Node *node;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	node = _edata_hash_get_node(hash, key);
	if (!node)
		return NULL;

	data = node->value;

	return data;
}

/**
 * Removes the value associated with the given key in the given hash
 * table.
 * @param   hash The given hash table.
 * @param   key  The key to search for.
 * @return  The value corresponding to the key on success.  @c NULL is
 *          returned if there is an error.
 */
EAPI void * edata_hash_remove(Edata_Hash *hash, const void *key)
{
	Edata_Hash_Node *node = NULL;
	Edata_Hash_Node *list;
	unsigned int hash_val;
	void *ret = NULL;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	/* Compute the position in the table */
	if (!hash->hash_func)
		hash_val = (unsigned int )key % edata_prime_table[hash->size];
	else
		hash_val = EDATA_COMPUTE_HASH(hash, key);

	/*
	 * If their is a list that could possibly hold the key/value pair
	 * traverse it and remove the hash node.
	 */
	if (hash->buckets[hash_val])
	{
		list = hash->buckets[hash_val];

		/*
		 * Traverse the list to find the specified key
		 */
		node = list;
		if (hash->compare)
		{
			while ((node) && (hash->compare(node->key, key) != 0))
			{
				list = node;
				node = node->next;
			}
		}
		else
		{
			while ((node) && (node->key != key))
			{
				list = node;
				node = node->next;
			}
		}

		/*
		 * Remove the node with the matching key and free it's memory
		 */
		if (node)
		{
			if (list == node)
				hash->buckets[hash_val] = node->next;
			else
				list->next = node->next;
			ret = node->value;
			node->value = NULL;
			_edata_hash_node_destroy(node, hash->free_key, NULL);
			hash->nodes--;
		}
	}

	if (EDATA_HASH_REDUCE(hash))
		_edata_hash_decrease(hash);

	return ret;
}

/**
 * Retrieves the first value that matches
 * table.
 * @param   hash The given hash table.
 * @param   key  The key to search for.
 * @return  The value corresponding to key on success, @c NULL otherwise.
 */
EAPI void * edata_hash_find(Edata_Hash *hash, Edata_Compare_Cb compare,
		const void *value)
{
	unsigned int i = 0;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);
	CHECK_PARAM_POINTER_RETURN("compare", compare, NULL);
	CHECK_PARAM_POINTER_RETURN("value", value, NULL);

	while (i < edata_prime_table[hash->size])
	{
		if (hash->buckets[i])
		{
			Edata_Hash_Node *node;

			for (node = hash->buckets[i]; node; node = node->next)
			{
				if (!compare(node->value, value))
					return node->value;
			}
		}
		i++;
	}

	return NULL;
}

/*
 * @brief Retrieve the node associated with key
 * @param hash: the hash table to search for the key
 * @param key: the key to search for in the hash table
 * @return Returns NULL on error, node corresponding to key on success
 */
static Edata_Hash_Node * _edata_hash_get_node(Edata_Hash *hash, const void *key)
{
	unsigned int hash_val;
	Edata_Hash_Node *node = NULL;

	CHECK_PARAM_POINTER_RETURN("hash", hash, NULL);

	if (!hash->buckets)
	{
		return NULL;
	}

	/* Compute the position in the table */
	if (!hash->hash_func)
		hash_val = (unsigned int )key % edata_prime_table[hash->size];
	else
		hash_val = EDATA_COMPUTE_HASH(hash, key);

	/* Grab the bucket at the specified position */
	if (hash->buckets[hash_val])
	{
		node = _edata_hash_get_bucket(hash, hash->buckets[hash_val], key);
		/*
		 * Move matched node to the front of the list as it's likely
		 * to be searched for again soon.
		 */
		if (node && node != hash->buckets[hash_val])
		{
			node->next = hash->buckets[hash_val];
			hash->buckets[hash_val] = node;
		}
	}

	return node;
}

/*
 * @brief Search the hash bucket for a specified key
 * @param hash: the hash table to retrieve the comparison function
 * @param bucket: the list to search for the key
 * @param key: the key to search for in the list
 * @return Returns NULL on error or not found, the found node on success
 */
inline Edata_Hash_Node * _edata_hash_get_bucket(Edata_Hash *hash,
		Edata_Hash_Node *bucket, const void *key)
{
	Edata_Hash_Node *prev = NULL;
	Edata_Hash_Node *node = NULL;

	/*
	 * Traverse the list to find the desired node, if the node is in the
	 * list, then return the node.
	 */
	if (hash->compare)
	{
		for (node = bucket; node; node = node->next)
		{
			if (hash->compare(node->key, key) == 0)
				break;
			prev = node;
		}
	}
	else
	{
		for (node = bucket; node; node = node->next)
		{
			if (node->key == key)
				break;
			prev = node;
		}
	}

	/*
	 * Remove node from the list to replace it at the beginning.
	 */
	if (node && prev)
	{
		prev->next = node->next;
		node->next = NULL;
	}

	return node;
}

/*
 * @brief Increase the size of the hash table by approx.  2 * current size
 * @param hash: the hash table to increase the size of
 * @return Returns TRUE on success, FALSE on error
 */
static int _edata_hash_increase(Edata_Hash *hash)
{
	void *old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	/* Max size reached so return FALSE */
	if ((edata_prime_table[hash->size] == PRIME_MAX) || (hash->size
			== PRIME_TABLE_MAX))
		return FALSE;

	/*
	 * Increase the size of the hash and save a pointer to the old data
	 */
	hash->size++;
	old = hash->buckets;

	/*
	 * Allocate a new bucket area, of the new larger size
	 */
	hash->buckets = calloc(edata_prime_table[hash->size],
			sizeof(Edata_Hash_Node *));

	/*
	 * Make sure the allocation succeeded, if not replace the old data and
	 * return a failure.
	 */
	if (!hash->buckets)
	{
		hash->buckets = old;
		hash->size--;
		return FALSE;
	}
	hash->nodes = 0;

	/*
	 * Now move all of the old data into the new bucket area
	 */
	if (_edata_hash_rehash(hash, old, hash->size - 1))
	{
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
 * @brief Decrease the size of the hash table by < 1/2 * current size
 * @param hash: the hash table to decrease the size of
 * @return Returns TRUE on success, FALSE on error
 */
static int _edata_hash_decrease(Edata_Hash *hash)
{
	Edata_Hash_Node **old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	if (edata_prime_table[hash->size] == PRIME_MIN)
		return FALSE;

	/*
	 * Decrease the hash size and store a pointer to the old data
	 */
	hash->size--;
	old = hash->buckets;

	/*
	 * Allocate a new area to store the data
	 */
	hash->buckets = (Edata_Hash_Node **)calloc(edata_prime_table[hash->size],
			sizeof(Edata_Hash_Node *));

	/*
	 * Make sure allocation succeeded otherwise rreturn to the previous
	 * state
	 */
	if (!hash->buckets)
	{
		hash->buckets = old;
		hash->size++;
		return FALSE;
	}

	hash->nodes = 0;

	if (_edata_hash_rehash(hash, old, hash->size + 1))
	{
		FREE(old);
		return TRUE;
	}

	return FALSE;
}

/*
 * @brief Rehash the nodes of a table into the hash table
 * @param hash: the hash to place the nodes of the table
 * @param table: the table to remove the nodes from and place in hash
 * @return Returns TRUE on success, FALSE on error
 */
inline int _edata_hash_rehash(Edata_Hash *hash, Edata_Hash_Node **old_table,
		int old_size)
{
	unsigned int i;
	Edata_Hash_Node *old;

	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);
	CHECK_PARAM_POINTER_RETURN("old_table", old_table, FALSE);

	for (i = 0; i < edata_prime_table[old_size]; i++)
	{
		/* Hash into a new list to avoid loops of rehashing the same nodes */
		while ((old = old_table[i]))
		{
			old_table[i] = old->next;
			old->next = NULL;
			_edata_hash_add_node(hash, old);
		}
	}

	return TRUE;
}

/*
 * @brief Create a new hash node for key and value storage
 * @param key: the key for this node
 * @param value: the value that the key references
 * @return Returns NULL on error, a new hash node on success
 */
static Edata_Hash_Node * _edata_hash_node_new(void *key, void *value)
{
	Edata_Hash_Node *node;

	node = (Edata_Hash_Node *)malloc(sizeof(Edata_Hash_Node));
	if (!node)
		return NULL;

	if (!_edata_hash_node_init(node, key, value))
	{
		FREE(node);
		return NULL;
	}

	return node;
}

/*
 * @brief Initialize a hash node to some sane default values
 * @param node: the node to set the values
 * @param key: the key to reference this node
 * @param value: the value that key refers to
 * @return Returns TRUE on success, FALSE on error
 */
static int _edata_hash_node_init(Edata_Hash_Node *node, void *key, void *value)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	node->key = key;
	node->value = value;

	return TRUE;
}

/*
 * @brief Destroy a node and call the specified callbacks to free data
 * @param node: the node to be destroyed
 * @param keyd: the function to free the key
 * @param valued: the function  to free the value
 * @return Returns TRUE on success, FALSE on error
 */
static int _edata_hash_node_destroy(Edata_Hash_Node *node, Edata_Free_Cb keyd,
		Edata_Free_Cb valued)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	if (keyd)
		keyd(node->key);

	if (valued)
		valued(node->value);

	FREE(node);

	return TRUE;
}
