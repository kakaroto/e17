#include "Ewl.h"

static void __ewl_tree_add(Ewl_Container *c, Ewl_Widget *w);
static void __ewl_tree_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o);
static void __ewl_tree_configure(Ewl_Widget *w, void *ev_data, void *user_data);


static void __ewl_tree_node_configure(Ewl_Widget * w, void *ev_data,
		void *user_data);
static void __ewl_tree_node_clicked(Ewl_Widget * w, void *ev_data,
		void *user_data);
static void __ewl_tree_node_theme_update(Ewl_Widget * w, void *ev_data,
		void *user_data);


static void __ewl_tree_node_add(Ewl_Container *c, Ewl_Widget *w);
static void __ewl_tree_node_remove(Ewl_Container *c, Ewl_Widget *w);
static void __ewl_tree_node_resize(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o);

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

	ewl_container_init(EWL_CONTAINER(tree), "tree", __ewl_tree_add,
			__ewl_tree_child_resize, NULL);

	ewl_callback_append(EWL_WIDGET(tree), EWL_CALLBACK_CONFIGURE,
			__ewl_tree_configure, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(tree), EWL_FILL_POLICY_HSHRINK |
			EWL_FILL_POLICY_HFILL);

	tree->ncols = columns;
	tree->colbases = NEW(int, columns);
	tree->colbounds = NEW(int, columns);

	ewl_tree_set_headers(tree, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_tree_set_headers
 */
void ewl_tree_set_headers(Ewl_Tree *tree, Ewl_Widget **headers)
{
	unsigned short i;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	row = ewl_row_new();
	if (headers) {
		for (i = 0; i < tree->ncols; i++) {
			ewl_container_append_child(EWL_CONTAINER(row),
					headers[i]);

			tree->colbases[i] = &CURRENT_X(headers[i]);
			tree->colbounds[i] = &CURRENT_W(headers[i]);
		}
	}
	else {
		Ewl_Widget *button;

		for (i = 0; i < tree->ncols; i++) {
			button = ewl_button_new(NULL);
			ewl_container_append_child(EWL_CONTAINER(row), button);
			ewl_widget_show(button);

			tree->colbases[i] = &CURRENT_X(button);
			tree->colbounds[i] = &CURRENT_W(button);
		}
	}

	ewl_container_append_child(EWL_CONTAINER(tree), row);
	ewl_widget_show(row);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
	Ewl_Widget *w;
	Ewl_Widget *row;
	Ewl_Widget *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_PARAM_PTR_RET("children", children, NULL);

	w = EWL_WIDGET(prow);

	if (prow && w->parent && (EWL_TREE_NODE(w->parent)->tree != tree))
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

	ewl_row_set_column_bounds(EWL_ROW(row), tree->ncols, tree->colbases,
			tree->colbounds);
	ewl_widget_show(row);

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

		ewl_widget_show(cell);
		ewl_container_append_child(EWL_CONTAINER(cell), children[i]);
		ewl_container_append_child(EWL_CONTAINER(row), cell);
	}

	/*
	 * Place the new row in the tree.
	 */
	if (prow && w->parent) {
		if (EWL_TREE_NODE(w->parent)->expanded)
			ewl_widget_show(node);
		ewl_container_append_child(EWL_CONTAINER(w->parent), node);
	}
	else {
		ewl_container_append_child(EWL_CONTAINER(tree), node);
		ewl_widget_show(node);
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
 * ewl_tree_set_row_expanded - set the expanded state of a specific row
 * @row: the row to change the expanded state
 * @expanded: the new expanded state for the row
 *
 * Returns no value. Changes the expanded state of @row to @expanded, which
 * should be TRUE or FALSE.
 */
void ewl_tree_set_row_expanded(Ewl_Row *row, int expanded)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("row", row);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);

	if (node && node->expanded != expanded) {
		if (!expanded)
			ewl_tree_node_collapse(EWL_TREE_NODE(node));
		else
			ewl_tree_node_expand(EWL_TREE_NODE(node));
	}

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

	ZERO(node, Ewl_Tree_Node, 1);
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

	ewl_container_init(EWL_CONTAINER(node), "node", __ewl_tree_node_add,
			__ewl_tree_node_resize, __ewl_tree_node_remove);

	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
			__ewl_tree_node_configure, NULL);
	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_THEME_UPDATE,
			__ewl_tree_node_theme_update, NULL);
	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CLICKED,
			__ewl_tree_node_clicked, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_tree_node_collapse - collapse a node in the tree
 * @node: the node in the tree to collapse
 *
 * Returns no value. Hides the rows below a node @node.
 */
void ewl_tree_node_collapse(Ewl_Tree_Node *node)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);

	if (!node->expanded)
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(EWL_CONTAINER(node)->children);
	ewd_list_next(EWL_CONTAINER(node)->children);

	while ((w = ewd_list_next(EWL_CONTAINER(node)->children))) {
		ewl_widget_hide(w);
	}

	node->expanded = FALSE;

	__ewl_tree_node_theme_update(EWL_WIDGET(node), NULL, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_tree_node_expand - expand a node in the tree
 * @node: the node in the tree to expand
 *
 * Returns no value. Hides the rows below a node @node.
 */
void ewl_tree_node_expand(Ewl_Tree_Node *node)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);

	if (node->expanded)
		DRETURN(DLEVEL_STABLE);

	node->expanded = TRUE;

	ewd_list_goto_first(EWL_CONTAINER(node)->children);
	ewd_list_next(EWL_CONTAINER(node)->children);

	while ((w = ewd_list_next(EWL_CONTAINER(node)->children))) {
		ewl_widget_show(w);
	}

	__ewl_tree_node_theme_update(EWL_WIDGET(node), NULL, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_add(Ewl_Container *c, Ewl_Widget *w)
{
	int cw;

	cw = ewl_object_get_preferred_w(EWL_OBJECT(w));
	if (cw > PREFERRED_W(c))
		ewl_object_set_preferred_w(EWL_OBJECT(c), cw);

	ewl_object_set_preferred_h(EWL_OBJECT(c), PREFERRED_H(c) +
			ewl_object_get_preferred_h(EWL_OBJECT(w)));
}

static void
__ewl_tree_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o)
{
	if (o == EWL_ORIENTATION_HORIZONTAL) {
		if (ewl_object_get_preferred_w(EWL_OBJECT(w)) > PREFERRED_W(c))
			ewl_object_set_preferred_w(EWL_OBJECT(c),
					PREFERRED_W(c) + size);
		else
			ewl_container_prefer_largest(c, o);
	}
	else {
		ewl_object_set_preferred_h(EWL_OBJECT(c),
				PREFERRED_H(c) + size);
	}
}

static void
__ewl_tree_configure(Ewl_Widget *w, void *ev_data, void *user_data)
{
	int y, h;
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
	y = CURRENT_Y(w);
	ewd_list_goto_first(c->children);
	while ((child = ewd_list_next(c->children))) {
		h = ewl_object_get_preferred_h(child);
		ewl_object_request_geometry(child, CURRENT_X(w), y,
				CURRENT_W(w), h);
		y += h;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tree_Node *node;
	Ewl_Container *c;
	Ewl_Object *child;
	int y, width;

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
	ewl_object_request_geometry(child, CURRENT_X(w), CURRENT_Y(w),
			CURRENT_W(w), ewl_object_get_preferred_h(child));
	
	y = CURRENT_Y(w) + ewl_object_get_current_h(child);
	width = CURRENT_W(w) - CURRENT_X(w) + CURRENT_X(w);

	/*
	 * All subsequent children are lower nodes and rows.
	 */
	while ((child = ewd_list_next(c->children))) {
		ewl_object_request_geometry(child, CURRENT_X(w), y, width,
				ewl_object_get_preferred_h(child));
		y += ewl_object_get_current_h(child);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_clicked(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(w);
	if (node->expanded)
		ewl_tree_node_collapse(node);
	else
		ewl_tree_node_expand(node);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(w);
	if (node->expanded)
		ewl_widget_update_appearance(w, "expanded");
	else
		ewl_widget_update_appearance(w, "collapsed");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_add(Ewl_Container *c, Ewl_Widget *w)
{
	int width;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(c);

	if (!ewd_list_nodes(c->children))
		node->row = w;

	if (node->expanded || w == node->row) {
		ewl_object_set_preferred_h(EWL_OBJECT(c),
				PREFERRED_H(c) +
				ewl_object_get_preferred_h(EWL_OBJECT(w)));
	}

	width = ewl_object_get_preferred_w(EWL_OBJECT(w));
	if (PREFERRED_W(c) < width)
		ewl_object_set_preferred_w(EWL_OBJECT(c), width);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_remove(Ewl_Container *c, Ewl_Widget *w)
{
	int width;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(c);

	if (node->expanded || w == node->row) {
		ewl_object_set_preferred_h(EWL_OBJECT(c),
				PREFERRED_H(c) -
				ewl_object_get_preferred_h(EWL_OBJECT(w)));
	}

	width = ewl_object_get_preferred_w(EWL_OBJECT(w));
	if (PREFERRED_W(c) >= width)
		ewl_container_prefer_largest(c, EWL_ORIENTATION_HORIZONTAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_tree_node_resize(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(c);

	if (node->expanded || (w == ewd_list_goto_first(c->children))) {
		if (o == EWL_ORIENTATION_HORIZONTAL)
			ewl_object_set_preferred_w(EWL_OBJECT(c),
					PREFERRED_W(c) + size);
		else
			ewl_object_set_preferred_h(EWL_OBJECT(c),
					PREFERRED_H(c) + size);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
