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

	unsigned int **colbases;
	unsigned int **colbounds;
	Ewl_Widget *header;
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
	Ewl_Widget *row;
	int expanded;
};

Ewl_Widget *ewl_tree_node_new();
int ewl_tree_node_init(Ewl_Tree_Node *tree_node);
void ewl_tree_node_collapse(Ewl_Tree_Node *tree);
void ewl_tree_node_expand(Ewl_Tree_Node *tree);

Ewl_Widget *ewl_tree_new(unsigned short columns);
int ewl_tree_init(Ewl_Tree *tree, unsigned short columns);
void ewl_tree_set_headers(Ewl_Tree *tree, char **headers);
void ewl_tree_set_columns(Ewl_Tree *tree, unsigned short columns);
Ewl_Widget *ewl_tree_add(Ewl_Tree *tree, Ewl_Row *prow, Ewl_Widget **children);
void ewl_tree_set_row_expand(Ewl_Row *row, Ewl_Tree_Node_Flags expanded);
Ewl_Row *ewl_tree_find_row(Ewl_Tree *tree, int row);

#endif
