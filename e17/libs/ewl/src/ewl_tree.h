#ifndef _EWL_TREE_H
#define _EWL_TREE_H


typedef struct _ewl_tree Ewl_Tree;
#define EWL_TREE(t) ((Ewl_Tree *)t)

/*
 * The tree is a columnar listing, where items in the list may be nested below
 * other items.
 */
struct _ewl_tree
{
	Ewl_Container container;

	unsigned short ncols;
	unsigned short nrows;

	unsigned int *colw;
	unsigned int *rowh;

	unsigned int **colbases;
	unsigned int **colbounds;

	int indent;
};

typedef struct _ewl_tree_node Ewl_Tree_Node;
#define EWL_TREE_NODE(t) ((Ewl_Tree_Node *)t)

/*
 * The tree_node exists for each row, at this level the tree_node contains rows
 * that contain cells which display the data.
 */
struct _ewl_tree_node
{
	Ewl_Container container;
	Ewl_Tree *tree;
	int expanded;
};

Ewl_Widget *ewl_tree_node_new();
int ewl_tree_node_init(Ewl_Tree_Node *tree_node);

Ewl_Widget *ewl_tree_new(unsigned short columns);
int ewl_tree_init(Ewl_Tree *tree, unsigned short columns);
void ewl_tree_set_columns(Ewl_Tree *tree, unsigned short columns);

#endif
