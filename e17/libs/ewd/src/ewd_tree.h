#ifndef _EWD_TREE_H
#define _EWD_TREE_H

typedef struct _Ewd_Tree_Node Ewd_Tree_Node;
#define EWD_TREE_NODE(object) ((Ewd_Tree_Node *)object)
struct _Ewd_Tree_Node {

	/* The actual data for each node */
	void *key;
	void *value;

	/* Pointers to surrounding nodes */
	Ewd_Tree_Node *parent;
	Ewd_Tree_Node *left_child;
	Ewd_Tree_Node *right_child;

	/* Book keeping information for quicker balancing of the tree */
	int max_right;
	int max_left;

	EWD_DECLARE_LOCKS;
};

typedef struct _Ewd_Tree Ewd_Tree;
#define EWD_TREE(object) ((Ewd_Tree *)object)
struct _Ewd_Tree {
	/* Nodes of the tree */
	Ewd_Tree_Node *tree;

	/* Callback for comparing node values, default is direct comparison */
	Ewd_Compare_Cb compare_func;

	/* Callback for freeing node data, default is NULL */
	Ewd_Free_Cb free_func;

	EWD_DECLARE_LOCKS;
};

/* Some basic tree functions */
/* Allocate and initialize a new tree */
Ewd_Tree *ewd_tree_new(Ewd_Compare_Cb compare_func);
/* Initialize a new tree */
int ewd_tree_init(Ewd_Tree * tree, Ewd_Compare_Cb compare_func);

/* Free the tree */
int ewd_tree_destroy(Ewd_Tree * tree);
/* Check to see if the tree has any nodes in it */
int ewd_tree_is_empty(Ewd_Tree * tree);

/* Retrieve the value associated with key */
void *ewd_tree_get(Ewd_Tree * tree, void *key);
Ewd_Tree_Node *ewd_tree_get_node(Ewd_Tree * tree, void *key);
/* Retrieve the value of node with key greater than or equal to key */
void *ewd_tree_get_closest_larger(Ewd_Tree * tree, void *key);
/* Retrieve the value of node with key less than or equal to key */
void *ewd_tree_get_closest_smaller(Ewd_Tree * tree, void *key);

/* Set the value associated with key to value */
int ewd_tree_set(Ewd_Tree * tree, void *key, void *value);
/* Remove the key from the tree */
int ewd_tree_remove(Ewd_Tree * tree, void *key);

/* Add a node to the tree */
int ewd_tree_add_node(Ewd_Tree * tree, Ewd_Tree_Node * node);
/* Remove a node from the tree */
int ewd_tree_remove_node(Ewd_Tree * tree, Ewd_Tree_Node * node);

/* For each node in the tree perform the for_each_func function */
/* For this one pass in the node */
int ewd_tree_for_each_node(Ewd_Tree * tree, Ewd_For_Each for_each_func);
/* And here pass in the node's value */
int ewd_tree_for_each_node_value(Ewd_Tree * tree,
				 Ewd_For_Each for_each_func);

/* Some basic node functions */
/* Initialize a node */
int ewd_tree_node_init(Ewd_Tree_Node * new_node);
/* Allocate and initialize a new node */
Ewd_Tree_Node *ewd_tree_node_new();
/* Free the desired node */
int ewd_tree_node_destroy(Ewd_Tree_Node * node, Ewd_Free_Cb free_data);

/* Set the node's key to key */
int ewd_tree_node_key_set(Ewd_Tree_Node * node, void *key);
/* Retrieve the key in node */
void *ewd_tree_node_key_get(Ewd_Tree_Node * node);

/* Set the node's value to value */
int ewd_tree_node_value_set(Ewd_Tree_Node * node, void *value);
/* Retrieve the value in node */
void *ewd_tree_node_value_get(Ewd_Tree_Node * node);

/* Add a function to free the data stored in nodes */
int ewd_tree_set_free_cb(Ewd_Tree * tree, Ewd_Free_Cb free_func);

#endif
