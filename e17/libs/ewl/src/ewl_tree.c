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


static void __ewl_tree_row_select(Ewl_Widget *w, void *ev_data,
				  void *user_data);

/**
 * @param columns: the number of columns to display
 * @return Returns NULL on failure, a new tree widget on success.
 * @brief Allocate and initialize a new tree widget
 *
 * The paramater @a columns can be modified at a later time to display
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
 * @param tree: the tree widget to be initialized
 * @param columns: the number of columns in the tree
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the contents of a tree widget
 *
 * The contents of the tree widget
 * @a tree are initialized to their defaults, and the number of columns to
 * display is set to @a columns.
 */
int ewl_tree_init(Ewl_Tree *tree, unsigned short columns)
{
	int i;
	Ewl_Widget *row;
	Ewl_Widget *button;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_PARAM_PTR_RET("columns", columns, FALSE);

	ewl_container_init(EWL_CONTAINER(tree), "tree", __ewl_tree_add,
			__ewl_tree_child_resize, NULL);

	ewl_callback_append(EWL_WIDGET(tree), EWL_CALLBACK_CONFIGURE,
			__ewl_tree_configure, NULL);

	tree->ncols = columns;
	tree->colbases = NEW(int, columns);
	tree->colbounds = NEW(int, columns);

	row = ewl_row_new();
	for (i = 0; i < tree->ncols; i++) {
		button = ewl_button_new(NULL);
		ewl_box_set_orientation(EWL_BOX(button),
				EWL_ORIENTATION_VERTICAL);
		ewl_container_append_child(EWL_CONTAINER(row), button);
		ewl_widget_show(button);

		tree->colbases[i] = &CURRENT_X(button);
		tree->colbounds[i] = &CURRENT_W(button);
	}

	ewl_callback_append(row, EWL_CALLBACK_SELECT, __ewl_tree_row_select,
			NULL);
	ewl_container_append_child(EWL_CONTAINER(tree), row);
	ewl_widget_show(row);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to change column headers
 * @param headers: the array of widget pointers containing the new headers
 * @return Returns no value.
 * @brief Change the widgets in a trees column headers
 *
 * Stores the widgets in @a headers to header row of @a tree.
 */
void ewl_tree_set_headers(Ewl_Tree *tree, char **headers)
{
	unsigned short i;
	Ewl_Widget *button;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	row = ewd_list_goto_first(EWL_CONTAINER(tree)->children);
	ewd_list_goto_first(EWL_CONTAINER(row)->children);

	button = ewd_list_next(EWL_CONTAINER(row)->children);
	for (i = 0; i < tree->ncols && button; i++) {
		ewl_button_set_label(EWL_BUTTON(button), headers[i]);

		tree->colbases[i] = &CURRENT_X(button);
		tree->colbounds[i] = &CURRENT_W(button);
		button = ewd_list_next(EWL_CONTAINER(row)->children);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: the tree to hold the widgets
 * @param prow: the parent row of the new row for the added widgets
 * @param children: a NULL terminated array of widgets to add to the tree
 * @return Returns a pointer to a new row on success, NULL on failure.
 * @brief Add a group of widgets to a row in the tree
 */
Ewl_Widget *
ewl_tree_add_row(Ewl_Tree *tree, Ewl_Row *prow, Ewl_Widget **children)
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
	for (i = 0; i < tree->ncols; i++) {
		Ewl_Widget *cell;

		cell = ewl_cell_new();
		if (!cell) {
			ewl_widget_destroy(node);
			row = NULL;
			break;
		}

		ewl_widget_show(cell);

		if (children[i]) {
			ewl_container_append_child(EWL_CONTAINER(cell),
					children[i]);
			ewl_container_append_child(EWL_CONTAINER(row), cell);
		}
	}

	/*
	 * Place the new row in the tree.
	 */
	if (prow && w->parent) {
		if (EWL_TREE_NODE(w->parent)->expanded == EWL_TREE_NODE_EXPANDED)
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
 * @param tree: the tree to hold the new text row
 * @param prow: the parent row of the new text row
 * @param text: the array of strings that hold the text to be added
 * @brief Add a row of text to a tree
 *
 * @return Returns a pointer to a new row on success, NULL on failure.
 */
Ewl_Widget *ewl_tree_add_text_row(Ewl_Tree *tree, Ewl_Row *prow, char **text)
{
	int i;
	Ewl_Widget **entries;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_PARAM_PTR_RET("text", text, NULL);

	entries = NEW(Ewl_Widget *, tree->ncols);
	ZERO(entries, Ewl_Widget *, tree->ncols);

	for (i = 0; i < tree->ncols; i++) {
		if (text[i]) {
			entries[i] = ewl_entry_new(text[i]);
			ewl_widget_show(entries[i]);
		}
	}

	row = ewl_tree_add_row(tree, prow, entries);

	FREE(entries);

	DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to remove a row from
 * @param row: the row to be removed from the tree
 * @return Returns no value.
 * @brief Remove a specified row from the tree
 *
 * Removes @a row from @a tree if it is present in @a tree. The
 * widgets in the row will not be destroyed, so they can be accessed at a
 * later time.
 */
void ewl_tree_remove_row(Ewl_Tree *tree, Ewl_Row *row)
{
	Ewl_Widget *w;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("row", row);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);

	while ((w = ewd_list_goto_first(EWL_CONTAINER(row)->children)))
		ewl_container_remove_child(EWL_CONTAINER(row), w);

	ewl_widget_destroy(EWL_WIDGET(node));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: the tree to destroy a row from
 * @param row: the row to be destroyed from the tree
 * @return Returns no value.
 * @brief Destroy a specified row from the tree
 *
 * Removes @a row from @a tree if it is present in @a tree. The
 * widgets in the row will be destroyed, so they should not be accessed at a
 * later time.
 */
void ewl_tree_destroy_row(Ewl_Tree *tree, Ewl_Row *row)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("row", row);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);

	ewl_widget_destroy(EWL_WIDGET(node));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: the tree to change the number of columns
 * @param columns: the new number of columns to be displayed
 * @return Returns no value.
 * @brief Change the number of columns displayed in a tree
 *
 * The number of columns displayed in @a tree is changed to
 * @a columns. When rows are added, pre-existing rows have empty contents in the
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
 * @param row: the row to change the expanded state
 * @param expanded: the new expanded state for the row
 * @return Returns no value.
 * @brief Set the expand state of a specific row
 *
 * Changes the expanded state of @a row to @a expanded, which
 * should be TRUE or FALSE.
 */
void ewl_tree_set_row_expand(Ewl_Row *row, Ewl_Tree_Node_Flags expanded)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("row", row);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);

	if (node && node->expanded != expanded) {
		if (!expanded || expanded == EWL_TREE_NODE_COLLAPSED)
			ewl_tree_node_collapse(EWL_TREE_NODE(node));
		else
			ewl_tree_node_expand(EWL_TREE_NODE(node));

		node->expanded = expanded;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a newly allocated node on success, NULL on failure.
 * @brief Allocate and initialize a new node
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
 * @param node: the node object to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the node fields of an inheriting object
 *
 * The fields of the @a node object are initialized to their defaults.
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

	node->expanded = EWL_TREE_NODE_COLLAPSED;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param node: the node in the tree to collapse
 * @return Returns no value. Hides the rows below @a node.
 * @brief Collapse a node in the tree
 */
void ewl_tree_node_collapse(Ewl_Tree_Node *node)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);

	if (node->expanded == EWL_TREE_NODE_COLLAPSED)
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(EWL_CONTAINER(node)->children);
	ewd_list_next(EWL_CONTAINER(node)->children);

	while ((w = ewd_list_next(EWL_CONTAINER(node)->children))) {
		ewl_widget_hide(w);
	}

	node->expanded = EWL_TREE_NODE_COLLAPSED;

	__ewl_tree_node_theme_update(EWL_WIDGET(node), NULL, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param node: the node in the tree to expand
 * @return Returns no value. Hides the rows below @a node.
 * @brief Expand a node in the tree
 */
void ewl_tree_node_expand(Ewl_Tree_Node *node)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);

	if (node->expanded == EWL_TREE_NODE_EXPANDED)
		DRETURN(DLEVEL_STABLE);

	node->expanded = EWL_TREE_NODE_EXPANDED;

	ewd_list_goto_first(EWL_CONTAINER(node)->children);
	ewd_list_next(EWL_CONTAINER(node)->children);

	while ((w = ewd_list_next(EWL_CONTAINER(node)->children)))
		ewl_widget_show(w);

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
		/* FIXME: Should we only grow this in order to reduce list
		 * traversal?
		else
			ewl_container_prefer_largest(c, o);
			*/
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

	if (node->expanded == EWL_TREE_NODE_NOEXPAND)
		DRETURN(DLEVEL_STABLE);

	if (node->expanded == EWL_TREE_NODE_EXPANDED)
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
		ewl_widget_set_state(w, "expanded");
	else
		ewl_widget_set_state(w, "collapsed");

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
		ewl_object_set_minimum_w(EWL_OBJECT(c), width);

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

static void
__ewl_tree_row_select(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Tree *tree;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(w->parent);
	tree = node->tree;

	tree->selected = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
