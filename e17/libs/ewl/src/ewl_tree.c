#include "Ewl.h"

/*
 * FIXME: See fixme below.
 */
#define DEFAULT_INDENT 30

static void __ewl_tree_configure(Ewl_Widget *w, void *ev_data, void *user_data);
static void __ewl_tree_node_configure(Ewl_Widget * w, void *ev_data,
		void *user_data);

/**
 * ewl_tree_new - allocate and initialize a new tree widget
 * @columns: the number of columns to display
 *
 * Returns NULL on failure, a newly allocated and initialized widget on
 * success. The paramater @columns can be modified at a later time to display
 * a different number of columns.
 */
Ewl_Widget *ewl_tree_new(unsigned short columns)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("columns", columns, NULL);

	w = NEW(Ewl_Tree, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(w, Ewl_Tree, 1);
	if (!ewl_tree_init(EWL_TREE(w), columns)) {
		FREE(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * ewl_tree_init - initialize the contents of a tree widget
 * @tree: the tree widget to be initialized
 * @columns: the number of columns in the tree
 *
 * Returns TRUE on success, FALSE on failure. The contents of the tree widget
 * @tree are initialized to their defaults, and the number of columns to
 * display is set to @columns.
 */
int ewl_tree_init(Ewl_Tree *tree, unsigned short columns)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_PARAM_PTR_RET("columns", columns, FALSE);

	ewl_container_init(EWL_CONTAINER(tree), "tree", NULL, NULL);

	ewl_callback_append(EWL_WIDGET(tree), EWL_CALLBACK_CONFIGURE,
			__ewl_tree_configure, NULL);

	tree->ncols = columns;
	tree->colw = NEW(int, columns);

	/*
	 * FIXME: This should be configurable in the theme, possibly character
	 * width based.
	 */
	tree->indent = DEFAULT_INDENT;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_tree_add - add a group of widgets to a row in the tree
 * @tree: the tree to hold the widgets
 * @prow: the parent row of the new row for the added widgets
 * @children: a NULL terminated array of widgets to add to the tree
 *
 * Returns a pointer to the newly created row on success, NULL on failure.
 */
Ewl_Widget *ewl_tree_add(Ewl_Tree *tree, Ewl_Row *prow, Ewl_Widget **children)
{
	int i;
	Ewl_Widget *row;
	Ewl_Widget *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_PARAM_PTR_RET("children", children, NULL);

	if (prow && EWL_WIDGET(prow)->parent &&
			(EWL_TREE_NODE(EWL_WIDGET(prow)->parent)->tree != tree))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	/*
	 * Every row in the tree is a node, this allows easily adding rows
	 * nested below others.
	 */
	node = ewl_tree_node_new();
	if (!node)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	row = ewl_row_new();
	if (!row) {
		ewl_widget_destroy(node);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	EWL_TREE_NODE(node)->tree = tree;
	ewl_container_append_child(EWL_CONTAINER(node), row);

	/*
	 * Pretty basic here, build up the rows and add the widgets to them.
	 */
	for (i = 0; i < tree->ncols && children[i]; i++) {
		Ewl_Widget *cell;

		cell = ewl_cell_new();
		if (!cell) {
			ewl_widget_destroy(node);
			row = NULL;
			break;
		}

		ewl_container_append_child(EWL_CONTAINER(row), cell);
		ewl_container_append_child(EWL_CONTAINER(cell), children[i]);
	}

	/*
	 * Place the new row in the tree.
	 */
	if (row) {
		if (prow && EWL_WIDGET(prow)->parent)
			ewl_container_append_child(EWL_CONTAINER(
						EWL_WIDGET(prow)->parent),
					node);
		else
			ewl_container_append_child(EWL_CONTAINER(tree), node);
	}

	DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * ewl_tree_add_text - add a row of text to a tree
 * @tree: the tree to hold the new text row
 * @prow: the parent row of the new text row
 * @text: the array of strings that hold the text to be added
 *
 * Returns a pointer to the newly created row on success, NULL on failure.
 */
Ewl_Widget *ewl_tree_add_text(Ewl_Tree *tree, Ewl_Row *prow, char **text)
{
	Ewl_Widget *row = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_PARAM_PTR_RET("text", text, NULL);

	DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * ewl_tree_set_columns - change the number of columns displayed in a tree
 * @tree: the tree to change the number of columns
 * @columns: the new number of columns to be displayed
 *
 * Returns no value. The number of columns displayed in @tree is changed to
 * @columns. When rows are added, pre-existing rows have empty contents in the
 * additional columns which are appended. When rows are removed, previously
 * existing rows destroy column contents that are removed from the end. If you
 * need finer grain control over where columns are added or removed, see
 * ewl_tree_add_column and ewl_tree_del_column.
 */
void ewl_tree_set_columns(Ewl_Tree *tree, unsigned short columns)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("columns", columns);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_tree_node_new - allocate and initialize a new node
 *
 * Returns a newly allocated node on success, NULL on failure.
 */
Ewl_Widget *ewl_tree_node_new()
{
	Ewl_Widget *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = NEW(Ewl_Tree_Node, 1);
	if (!node)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree_node_init(EWL_TREE_NODE(node))) {
		FREE(node);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(node, DLEVEL_STABLE);
}

/**
 * ewl_tree_node_init - initialize the node fields of an inheriting object
 * @node: the node object to initialize
 *
 * Returns TRUE on success, FALSE on failure. The fields of the @node object
 * are initialized to their defaults.
 */
int ewl_tree_node_init(Ewl_Tree_Node *node)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("node", node, FALSE);

	ewl_container_init(EWL_CONTAINER(node), "node", NULL, NULL);

	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
			__ewl_tree_node_configure, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
__ewl_tree_configure(Ewl_Widget *w, void *ev_data, void *user_data)
{
	int i, y, h;
	Ewl_Object *child;
	Ewl_Container *c;
	Ewl_Tree *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = EWL_CONTAINER(w);
	tree = EWL_TREE(w);

	/*
	 * Align each top level node at the current x coordinate, and simply
	 * lay them out in a vertical fashion.
	 */
	i = 0;
	y = CURRENT_Y(w);
	h = CURRENT_H(w) / ewd_list_nodes(c->children);
	ewd_list_goto_first(c->children);
	while (i < tree->nrows && (child = ewd_list_next(c->children))) {
		ewl_object_request_geometry(child, CURRENT_X(w), y,
				CURRENT_W(w), h);
		y += h;
		i++;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tree_Node *node;
	Ewl_Container *c;
	Ewl_Object *child;
	int x;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(w);
	if (!node->tree)
		DRETURN(DLEVEL_STABLE);

	c = EWL_CONTAINER(w);

	ewd_list_goto_first(c->children);
	child = ewd_list_next(c->children);

	/*
	 * The first child is the current level row
	 * are lower nodes and rows.
	 */
	x = CURRENT_X(w);
	ewl_object_request_geometry(child, x, CURRENT_Y(w),
			CURRENT_W(w), CURRENT_H(w));
	x += node->tree->indent;

	/*
	 * All subsequent children are lower nodes and rows.
	 */
	while ((child = ewd_list_next(c->children))) {
		ewl_object_request_geometry(child, x, CURRENT_Y(w),
				CURRENT_W(w), CURRENT_H(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
