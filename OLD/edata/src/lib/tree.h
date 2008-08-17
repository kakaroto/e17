#ifndef _EDATA_TREE_H
#define _EDATA_TREE_H

typedef struct _Edata_Tree_Node Edata_Tree_Node;
# define EDATA_TREE_NODE(object) ((Edata_Tree_Node *)object)
struct _Edata_Tree_Node {

/* The actual data for each node */
void *key;
void *value;

/* Pointers to surrounding nodes */
Edata_Tree_Node *parent;
Edata_Tree_Node *left_child;
Edata_Tree_Node *right_child;

/* Book keeping information for quicker balancing of the tree */
int max_right;
int max_left;
};

typedef struct _Edata_Tree Edata_Tree;
# define EDATA_TREE(object) ((Edata_Tree *)object)
struct _Edata_Tree {
/* Nodes of the tree */
Edata_Tree_Node *tree;

/* Callback for comparing node values, default is direct comparison */
Edata_Compare_Cb compare_func;

/* Callback for freeing node data, default is NULL */
Edata_Free_Cb free_value;
/* Callback for freeing node key, default is NULL */
Edata_Free_Cb free_key;
};

/* Some basic tree functions */
/* Allocate and initialize a new tree */
EAPI Edata_Tree *edata_tree_new(Edata_Compare_Cb compare_func);
/* Initialize a new tree */
EAPI int edata_tree_init(Edata_Tree * tree, Edata_Compare_Cb compare_func);

/* Free the tree */
EAPI int edata_tree_destroy(Edata_Tree * tree);
/* Check to see if the tree has any nodes in it */
EAPI int edata_tree_empty_is(Edata_Tree * tree);

/* Retrieve the value associated with key */
EAPI void *edata_tree_get(Edata_Tree * tree, const void *key);
EAPI Edata_Tree_Node *edata_tree_get_node(Edata_Tree * tree, const void *key);
/* Retrieve the value of node with key greater than or equal to key */
EAPI void *edata_tree_closest_larger_get(Edata_Tree * tree, const void *key);
/* Retrieve the value of node with key less than or equal to key */
EAPI void *edata_tree_closest_smaller_get(Edata_Tree * tree, const void *key);

/* Set the value associated with key to value */
EAPI int edata_tree_set(Edata_Tree * tree, void *key, void *value);
/* Remove the key from the tree */
EAPI int edata_tree_remove(Edata_Tree * tree, const void *key);

/* Add a node to the tree */
EAPI int edata_tree_node_add(Edata_Tree * tree, Edata_Tree_Node * node);
/* Remove a node from the tree */
EAPI int edata_tree_node_remove(Edata_Tree * tree, Edata_Tree_Node * node);

/* For each node in the tree perform the for_each_func function */
/* For this one pass in the node */
EAPI int edata_tree_for_each_node(Edata_Tree * tree, Edata_For_Each for_each_func,
			     void *user_data);
/* And here pass in the node's value */
EAPI int edata_tree_for_each_node_value(Edata_Tree * tree,
				   Edata_For_Each for_each_func,
				   void *user_data);

/* Some basic node functions */
/* Initialize a node */
EAPI int edata_tree_node_init(Edata_Tree_Node * new_node);
/* Allocate and initialize a new node */
EAPI Edata_Tree_Node *edata_tree_node_new(void);
/* Free the desired node */
EAPI int edata_tree_node_destroy(Edata_Tree_Node * node, 
	   Edata_Free_Cb free_value, Edata_Free_Cb free_key);

/* Set the node's key to key */
EAPI int edata_tree_node_key_set(Edata_Tree_Node * node, void *key);
/* Retrieve the key in node */
EAPI void *edata_tree_node_key_get(Edata_Tree_Node * node);

/* Set the node's value to value */
EAPI int edata_tree_node_value_set(Edata_Tree_Node * node, void *value);
/* Retrieve the value in node */
EAPI void *edata_tree_node_value_get(Edata_Tree_Node * node);

/* Add a function to free the data stored in nodes */
EAPI int edata_tree_free_value_cb_set(Edata_Tree * tree, Edata_Free_Cb free_value);
/* Add a function to free the keys stored in nodes */
EAPI int edata_tree_free_key_cb_set(Edata_Tree * tree, Edata_Free_Cb free_key);

#endif /* _EDATA_TREE_H */
