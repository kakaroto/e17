/* ewd_tree.c

Copyright (C) 2001 Nathan Ingersoll         <ningerso@d.umn.edu>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <Ewd.h>

/* A macro for determining the highest node at given branch */
#define MAX_HEIGHT(node) (node ? MAX(node->max_left, node->max_right) : 0)

/* Utility functions for searching the tree and returning a node, or its
 * parent */
Ewd_Tree_Node *tree_node_find(Ewd_Tree * tree, void *key);
Ewd_Tree_Node *tree_node_find_parent(Ewd_Tree * tree, void *key);

/* Balancing functions, keep the tree balanced within a one node height
 * difference */
int tree_node_balance(Ewd_Tree * Tree, Ewd_Tree_Node * top_node);
int tree_node_rotate_right(Ewd_Tree * tree, Ewd_Tree_Node * top_node);
int tree_node_rotate_left(Ewd_Tree * tree, Ewd_Tree_Node * top_node);

/* Fucntions for executing a specified function on each node of a tree */
int tree_for_each_node(Ewd_Tree_Node * node, Ewd_For_Each for_each_func);
int tree_for_each_node_value(Ewd_Tree_Node * node,
			     Ewd_For_Each for_each_func);

/*
 * Description: Allocate a new tree structure.
 * Parameters:  1. compare_func - function used to compare the two values, most
 *                                likely NULL is what you want.
 * Returns: NULL if the operation fails, otherwise a pointer to the new tree
 */
Ewd_Tree *ewd_tree_new(Ewd_Compare_Cb compare_func)
{
	Ewd_Tree *new_tree;

	new_tree = EWD_TREE(malloc(sizeof(Ewd_Tree)));
	if (!new_tree)
		return NULL;

	if (!ewd_tree_init(new_tree, compare_func)) {
		IF_FREE(new_tree);
		return NULL;
	}

	return new_tree;
}

/*
 * Description: Initialize a tree structure to some sane initial values
 * Parameters: 1. new_tree - the new tree structure to be initialized
 *             2. compare_func - the function used to compare node keys, this is
 *                               usually NULL since a direct comparison is
 *                               most common
 * Returns: TRUE on successful initialization, FALSE on an error
 */
int ewd_tree_init(Ewd_Tree * new_tree, Ewd_Compare_Cb compare_func)
{
	CHECK_PARAM_POINTER_RETURN("new_tree", new_tree, FALSE);

	memset(new_tree, 0, sizeof(Ewd_Tree));

	if (!compare_func)
		new_tree->compare_func = ewd_direct_compare;
	else
		new_tree->compare_func = compare_func;

	EWD_INIT_LOCKS(new_tree);

	return TRUE;
}

/*
 * Description: Add a function that will be called at node destroy time, the
 * 		function will be passed the value of the node to be destroyed.
 * Parameters: 1. tree - the tree that will use this function when nodes are
 *                       destroyed.
 *             2. free_func - the function that will be passed the value of
 *                            the node being freed.
 * Returns: TRUE on successful set, FALSE otherwise.
 */
int ewd_tree_set_free_cb(Ewd_Tree * tree, Ewd_Free_Cb free_func)
{
	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_WRITE_LOCK(tree);
	tree->free_func = free_func;
	EWD_WRITE_UNLOCK(tree);

	return TRUE;
}

/*
 * Description: Initialize a new tree node
 * Parameters: None.
 * Returns: FALSE if the operation fails, otherwise TRUE
 */
int ewd_tree_node_init(Ewd_Tree_Node * new_node)
{
	CHECK_PARAM_POINTER_RETURN("new_node", new_node, FALSE);

	new_node->key = NULL;
	new_node->value = NULL;

	new_node->parent = NULL;
	new_node->right_child = NULL;
	new_node->left_child = NULL;

	new_node->max_left = new_node->max_right = 0;

	EWD_INIT_LOCKS(new_node);

	return TRUE;
}

/*
 * Description: Allocate a new tree node
 * Parameters: None.
 * Returns: NULL if the operation fails, otherwise a pointer to the new node.
 */
Ewd_Tree_Node *ewd_tree_node_new()
{
	Ewd_Tree_Node *new_node;

	new_node = EWD_TREE_NODE(malloc(sizeof(Ewd_Tree_Node)));
	if (!new_node)
		return NULL;

	if (!ewd_tree_node_init(new_node)) {
		IF_FREE(new_node);
		return NULL;
	}

	return new_node;
}

/*
 * Description: Free a tree node and it's children. If you don't want the
 * 		children free'd then you need to remove the node first.
 * Parameters: 1. node - tree node to be free()'d
 * 	       2. data_free - callback for destroying the data held in node
 * Returns: TRUE if the node is destroyed successfully, FALSE if not.
 */
int ewd_tree_node_destroy(Ewd_Tree_Node * node, Ewd_Free_Cb data_free)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	EWD_WRITE_LOCK(node);
	if (data_free)
		data_free(node->value);
	EWD_WRITE_UNLOCK(node);

	EWD_DESTROY_LOCKS(node);

	FREE(node);

	return TRUE;
}

/*
 * Description: Set the value of the node to value
 * Parameters: 1. node - the node to be set
 *             2. value - the value to set the node to.
 * Returns: TRUE if the node is set successfully, FALSE if not.
 */
int ewd_tree_node_value_set(Ewd_Tree_Node * node, void *value)
{
	CHECK_PARAM_POINTER_RETURN("node", node,
				   FALSE);

	EWD_WRITE_LOCK(node);
	node->value = value;
	EWD_WRITE_UNLOCK(node);

	return TRUE;
}

/*
 * Description: Get the value of the node
 * Parameters: 1. node - the node that contains the desired value
 * Returns: NULL if an error, otherwise the value associated with node
 */
void *ewd_tree_node_value_get(Ewd_Tree_Node * node)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);
	EWD_READ_LOCK(node);
	ret = node->value;
	EWD_READ_UNLOCK(node);

	return ret;
}

/*
 * Description: Set the value of the node's key  to key
 * Parameters: 1. node - the node to be set
 *             2. key - the value to set it's key to.
 * Returns: TRUE if the node is set successfully, FALSE if not.
 */
int ewd_tree_node_key_set(Ewd_Tree_Node * node, void *key)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	EWD_WRITE_LOCK(node);
	node->key = key;
	EWD_WRITE_UNLOCK(node);

	return TRUE;
}

/*
 * Description: Get the value of the node's key 
 * Parameters: 1. node - the node that contains the desired key
 * Returns: NULL if an error occurs, otherwise the key is returned
 */
void *ewd_tree_node_key_get(Ewd_Tree_Node * node)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);
	EWD_READ_LOCK(node);
	ret = node->key;
	EWD_READ_UNLOCK(node);

	return ret;
}

/*
 * Description: Free a tree
 * Parameters: 1. tree - the tree to destroy
 * Returns: TRUE if tree destroyed successfully, FALSE if not.
 */
int ewd_tree_destroy(Ewd_Tree * tree)
{
	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_WRITE_LOCK(tree);
	while (tree->tree)
		ewd_tree_remove_node(tree, tree->tree);
	EWD_WRITE_UNLOCK(tree);
	EWD_DESTROY_LOCKS(tree);

	FREE(tree);

	return TRUE;
}

/* Description: Return the node corresponding to key
 * Parameters: 1. tree - the tree to search
 *             2. key - the key to search for in the tree
 * Returns: The node corresponding to the key if found, otherwise NULL. */
Ewd_Tree_Node *ewd_tree_get_node(Ewd_Tree * tree, void *key)
{
	Ewd_Tree_Node *ret;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_READ_LOCK(tree);
	ret = tree_node_find(tree, key);
	EWD_READ_UNLOCK(tree);

	return ret;
}

/* Description: Return the value corresponding to key
 * Parameters: 1. tree - the tree to search
 *             2. key - the key to search for in tree
 * Returns: The value corresponding to the key if found, otherwise NULL. */
void *ewd_tree_get(Ewd_Tree * tree, void *key)
{
	void *ret;
	Ewd_Tree_Node *node;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_READ_LOCK(tree);
	node = tree_node_find(tree, key);
	EWD_READ_UNLOCK(tree);

	EWD_READ_LOCK(node);
	ret = (node ? node->value : NULL);
	EWD_READ_UNLOCK(node);

	return ret;
}

/* Description: Find the closest value greater than or equal to key
 * Parameters: 1. tree - the tree to search 
 *             2. key - the key to search for in tree
 * Returns: NULL if no valid nodes, otherwise the node >= key */
void *ewd_tree_get_closest_larger(Ewd_Tree * tree, void *key)
{
	Ewd_Tree_Node *node;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_READ_LOCK(tree);
	node = tree_node_find(tree, key);
	EWD_READ_UNLOCK(tree);

	if (node)
		return node;

	EWD_READ_LOCK(tree);
	node = tree_node_find_parent(tree, key);

	if (!node) {
		EWD_READ_UNLOCK(tree);
		return NULL;
	}

	EWD_READ_LOCK(node);
	if (tree->compare_func(node->key, key) < 0)
		return NULL;
	EWD_READ_UNLOCK(node);
	EWD_READ_UNLOCK(tree);

	return node;
}

/* Description: Find the closest value less than or equal to key
 * Parameters: 1. tree - the tree to search
 *             2. key - the key to search for in tree
 * Returns: NULL if no valid nodes, otherwise the node <= key */
void *ewd_tree_get_closest_smaller(Ewd_Tree * tree, void *key)
{
	Ewd_Tree_Node *node;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_READ_LOCK(tree);
	node = tree_node_find(tree, key);
	EWD_READ_UNLOCK(tree);

	if (node)
		return node;

	EWD_READ_LOCK(tree);
	node = tree_node_find_parent(tree, key);
	EWD_READ_LOCK(tree);

	if (node)
		node = node->right_child;

	return node;
}

/* Description: Set key to value
 * Parameters: 1. tree - the tree that contains the key/value pair
 *             2. key - the key to identify which node to set a value
 *             3. value - value to set the found node
 * Returns: TRUE if successful, FALSE if not. */
int ewd_tree_set(Ewd_Tree * tree, void *key, void *value)
{
	Ewd_Tree_Node *node = NULL;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	EWD_READ_LOCK(tree);
	node = tree_node_find(tree, key);
	EWD_READ_UNLOCK(tree);

	if (!node) {
		node = ewd_tree_node_new();
		ewd_tree_node_key_set(node, key);
		if (!ewd_tree_add_node(tree, node))
			return FALSE;
	}
	ewd_tree_node_value_set(node, value);

	EWD_WRITE_LOCK(tree);
	for (; node; node = node->parent)
		tree_node_balance(tree, node);
	EWD_WRITE_UNLOCK(tree);

	return TRUE;
}

/* Description: Place a node in the tree
 * Parameters: 1. tree - the tree to add the node
 *             2. node - the node to add
 * Returns: TRUE on a successful add, FALSE otherwise. */
int ewd_tree_add_node(Ewd_Tree * tree, Ewd_Tree_Node * node)
{
	Ewd_Tree_Node *travel = NULL;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	/* Find where to put this new node. */
	if (!tree->tree) {
		tree->tree = node;
	} else {
		travel = tree_node_find_parent(tree, node->key);
		node->parent = travel;

		/* The node is less than travel */
		if (tree->compare_func(node->key, travel->key) < 0) {
			travel->right_child = node;
			travel->max_left = 1;
			/* The node is greater than travel */
		} else {
			travel->left_child = node;
			travel->max_right = 1;
		}
	}

	return TRUE;
}


/* Description: Remove the node from the tree
 * Parameters: 1. tree - the tree to remove from
 *             2. node - the node to remove.
 * Returns: TRUE on a successful remove, FALSE otherwise. */
int ewd_tree_remove_node(Ewd_Tree * tree, Ewd_Tree_Node * node)
{
	Ewd_Tree_Node *traverse;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	traverse = node;

	/* If there's a right node we need to start at that point, otherwise
	 * there's only a single left node. */
	if (node->left_child)
		traverse = node->left_child;

	/* Now work our way down left side of the traverse node.
	 * This will give us the node with the next closest value to the
	 * current node. If node had no right node, then this will stop at
	 * node's left node. */
	while (traverse->right_child)
		traverse = traverse->right_child;

	if (traverse == node) {
		if (traverse->parent) {
			if (traverse->parent->left_child == traverse)
				traverse->parent->left_child = NULL;
			else
				traverse->parent->right_child = NULL;
		} else
			tree->tree = NULL;
		traverse = NULL;
	} else {
		/* This if ensures that we won't point traverse->right_child back
		 * to traverse */
		if (node->right_child != traverse)
			traverse->right_child = node->right_child;

		/* This if ensures that we won't point traverse->left_child
		 * back to traverse */
		if (node->left_child != traverse)
			traverse->left_child = node->left_child;

		if (traverse->parent) {
			if (traverse->parent->left_child == traverse)
				traverse->parent->left_child = NULL;
			else
				traverse->parent->right_child = NULL;
		}
		traverse->parent = node->parent;
	}

	node->parent = node->left_child = node->right_child = NULL;

	for (; traverse; traverse = traverse->parent)
		tree_node_balance(tree, traverse);

	return TRUE;
}

/* Description: Remove the key from the tree
 * Parameters: 1. tree - the tree to remove from
 *             2. key - the key to search for and remove the found node.
 * Returns: TRUE on a successful remove, FALSE otherwise. */
int ewd_tree_remove(Ewd_Tree * tree, void *key)
{
	Ewd_Tree_Node *node;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);
	if (!tree->tree)
		return FALSE;

	/* Find the node we need to remove */
	node = tree_node_find(tree, key);
	if (!node)
		return FALSE;

	if (!ewd_tree_remove_node(tree, node))
		return FALSE;

	node->left_child = node->right_child = NULL;
	ewd_tree_node_destroy(node, tree->free_func);

	return TRUE;
}

/*
 * Description: Test to see if the tree has any nodes
 * Parameters: 1. tree - the tree to check
 * Returns: TRUE if no nodes exist, FALSE otherwise
 */
int ewd_tree_is_empty(Ewd_Tree * tree)
{
	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	if (!tree->tree)
		return TRUE;

	return FALSE;
}

/*
 * Description: Execute the function for each node in the tree, passing in
 *              the value of each node.
 * Parameters: 1. tree - the tree to traverse
 *             2. for_each_func - the function to execute for each node of the
 *                                tree
 * Returns: No return value.
 */
int ewd_tree_for_each_node_value(Ewd_Tree * tree,
				 Ewd_For_Each for_each_func)
{
	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);
	CHECK_PARAM_POINTER_RETURN("for_each_func", for_each_func, FALSE);

	if (!tree->tree)
		return FALSE;

	return tree_for_each_node_value(tree->tree, for_each_func);
}

/*
 * Description: Execute the function for each node in the tree.
 * Parameters: 1. tree - the tree to traverse
 *             2. for_each_func - the function to execute for each node
 * Returns: The value returned by tree_for_each_node
 */
int ewd_tree_for_each_node(Ewd_Tree * tree, Ewd_For_Each for_each_func)
{
	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);
	CHECK_PARAM_POINTER_RETURN("for_each_func", for_each_func, FALSE);

	if (!tree->tree)
		return FALSE;

	return tree_for_each_node(tree->tree, for_each_func);
}

/* Find the parent for the key */
Ewd_Tree_Node *tree_node_find_parent(Ewd_Tree * tree, void *key)
{
	Ewd_Tree_Node *parent, *travel;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	parent = tree_node_find(tree, key);
	if (parent)
		parent = parent->parent;

	travel = tree->tree;
	if (!travel)
		return NULL;

	while (!parent) {
		int compare;

		if ((compare = tree->compare_func(key, travel->key)) < 0) {
			if (!travel->right_child)
				parent = travel;
			travel = travel->right_child;
		} else {
			if (!travel->left_child)
				parent = travel;
			travel = travel->left_child;
		}
	}

	return parent;
}

/* Search for the node with a specified key */
Ewd_Tree_Node *tree_node_find(Ewd_Tree * tree, void *key)
{
	int compare;
	Ewd_Tree_Node *node;

	CHECK_PARAM_POINTER_RETURN("tree", tree, FALSE);

	node = tree->tree;
	while (node && (compare = tree->compare_func(key, node->key)) != 0) {

		if (compare < 0) {
			if (!node->right_child) {
				return NULL;
			}

			node = node->right_child;
		} else {
			if (!node->left_child) {
				return NULL;
			}

			node = node->left_child;
		}
	}

	return node;
}

/* Balance the tree with respect to node */
int tree_node_balance(Ewd_Tree * tree, Ewd_Tree_Node * top_node)
{
	int balance;

	CHECK_PARAM_POINTER_RETURN("top_node", top_node, FALSE);

	/* Get the height of the left branch. */
	if (top_node->right_child) {
		top_node->max_left = MAX_HEIGHT(top_node->right_child) + 1;
	} else
		top_node->max_left = 0;

	/* Get the height of the right branch. */
	if (top_node->left_child) {
		top_node->max_right = MAX_HEIGHT(top_node->left_child) + 1;
	} else
		top_node->max_right = 0;

	/* Determine which side has a larger height. */
	balance = top_node->max_right - top_node->max_left;

	/* if the left side has a height advantage >1 rotate right */
	if (balance < -1)
		tree_node_rotate_right(tree, top_node);
	/* else if the left side has a height advantage >1 rotate left */
	else if (balance > 1)
		tree_node_rotate_left(tree, top_node);

	return TRUE;
}

/* Tree is overbalanced to the left, so rotate nodes to the right. */
int tree_node_rotate_right(Ewd_Tree * tree, Ewd_Tree_Node * top_node)
{
	Ewd_Tree_Node *temp;

	CHECK_PARAM_POINTER_RETURN("top_node", top_node, FALSE);

	/* The left branch's right branch becomes the nodes left branch,
	 * the left branch becomes the top node, and the node becomes the
	 * right branch. */
	temp = top_node->right_child;
	top_node->right_child = temp->left_child;
	temp->left_child = top_node;

	/* Make sure the nodes know who their new parents are and the tree
	 * structure knows the start of the tree. */
	temp->parent = top_node->parent;
	if (temp->parent == NULL)
		tree->tree = temp;
	else {
		if (temp->parent->left_child == top_node)
			temp->parent->left_child = temp;
		else
			temp->parent->right_child = temp;
	}
	top_node->parent = temp;

	/* And recalculate node heights */
	tree_node_balance(tree, top_node);
	tree_node_balance(tree, temp);

	return TRUE;
}

/* The tree is overbalanced to the right, so we rotate nodes to the left */
int tree_node_rotate_left(Ewd_Tree * tree, Ewd_Tree_Node * top_node)
{
	Ewd_Tree_Node *temp;

	CHECK_PARAM_POINTER_RETURN("top_node", top_node, FALSE);

	/*
	 * The right branch's left branch becomes the nodes right branch,
	 * the right branch becomes the top node, and the node becomes the
	 * left branch.
	 */
	temp = top_node->left_child;
	top_node->left_child = temp->right_child;
	temp->right_child = top_node;

	/* Make sure the nodes know who their new parents are. */
	temp->parent = top_node->parent;
	if (temp->parent == NULL)
		tree->tree = temp;
	else {
		if (temp->parent->left_child == top_node)
			temp->parent->left_child = temp;
		else
			temp->parent->right_child = temp;
	}
	top_node->parent = temp;

	/* And recalculate node heights */
	tree_node_balance(tree, top_node);
	tree_node_balance(tree, temp);

	return TRUE;
}

/*
 * Description: Execute a function for each node below this point in the tree.
 * Parameters: 1. node - the highest node in the tree the function will be
 *                       executed for
 *             2. for_each_func - the function to pass the nodes as data into
 * Returns: FALSE if an error condition occurs, otherwise TRUE
 */
int tree_for_each_node(Ewd_Tree_Node * node, Ewd_For_Each for_each_func)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	if (node->right_child)
		tree_for_each_node(node->right_child, for_each_func);

	if (node->left_child)
		tree_for_each_node(node->left_child, for_each_func);

	for_each_func(node);

	return TRUE;
}


/*
 * Description: Execute a function for each node below this point in the tree.
 * Parameters: 1. node - the highest node in the tree the function will be
 *                    executed for
 *             2. for_each_func - the function to pass the nodes values as data
 * Returns: FALSE if an error condition occurs, otherwise TRUE
 */
int tree_for_each_node_value(Ewd_Tree_Node * node,
			     Ewd_For_Each for_each_func)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	if (node->right_child)
		tree_for_each_node_value(node->right_child, for_each_func);

	if (node->left_child)
		tree_for_each_node_value(node->left_child, for_each_func);

	for_each_func(node->value);

	return TRUE;
}
