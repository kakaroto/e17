#include "Ewl.h"

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

	ewl_box_init(EWL_BOX(tree), EWL_ORIENTATION_VERTICAL);
	ewl_widget_set_appearance(EWL_WIDGET(tree), "tree");
	ewl_object_set_fill_policy(EWL_OBJECT(tree), EWL_FLAG_FILL_SHRINK |
			EWL_FLAG_FILL_FILL);

	tree->ncols = columns;

	row = ewl_row_new();
	for (i = 0; i < tree->ncols; i++) {
		button = ewl_button_new(NULL);
		ewl_box_set_orientation(EWL_BOX(button),
				EWL_ORIENTATION_VERTICAL);
		ewl_object_set_fill_policy(EWL_OBJECT(button),
				EWL_FLAG_FILL_HSHRINK |
				EWL_FLAG_FILL_HFILL);
		ewl_container_append_child(EWL_CONTAINER(row), button);
		ewl_widget_show(button);
	}

	ewl_callback_append(row, EWL_CALLBACK_SELECT, ewl_tree_row_select_cb,
			    NULL);
	tree->header = row;
	ewl_container_append_child(EWL_CONTAINER(tree), row);
	ewl_widget_show(row);

	tree->scrollarea = ewl_scrollpane_new();
	ewl_container_append_child(EWL_CONTAINER(tree), tree->scrollarea);
	ewl_widget_show(tree->scrollarea);

	ewl_container_set_redirect(EWL_CONTAINER(tree),
				   EWL_CONTAINER(tree->scrollarea));

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

	if (!EWL_CONTAINER(tree)->children)
		DRETURN(DLEVEL_STABLE);

	row = ewd_list_goto_first(EWL_CONTAINER(tree)->children);
	ewd_list_goto_first(EWL_CONTAINER(row)->children);

	button = ewd_list_next(EWL_CONTAINER(row)->children);
	for (i = 0; i < tree->ncols && button; i++) {
		ewl_button_set_label(EWL_BUTTON(button), headers[i]);
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

	ewl_row_set_header(EWL_ROW(row), EWL_ROW(tree->header));
	ewl_widget_show(row);

	EWL_TREE_NODE(node)->tree = tree;
	EWL_TREE_NODE(node)->row = row;
	ewl_container_append_child(EWL_CONTAINER(node), row);

	/*
	 * Pretty basic here, build up the rows and add the widgets to them.
	 */
	for (i = 0; i < tree->ncols && children; i++) {
		Ewl_Widget *cell;

		cell = ewl_cell_new();
		if (!cell) {
			ewl_widget_destroy(node);
			row = NULL;
			break;
		}

		ewl_widget_set_internal(cell, TRUE);
		ewl_container_append_child(EWL_CONTAINER(row), cell);
		ewl_object_set_fill_policy(EWL_OBJECT(cell),
				EWL_FLAG_FILL_HFILL |
				EWL_FLAG_FILL_HSHRINK);
		ewl_widget_show(cell);

		if (children[i]) {
			ewl_container_append_child(EWL_CONTAINER(cell),
						   children[i]);
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
	Ewl_Widget **texts;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, NULL);

	texts = NEW(Ewl_Widget *, tree->ncols);
	if (!texts)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	for (i = 0; i < tree->ncols; i++) {
		if (text)
			texts[i] = ewl_text_new(text[i]);
		else
			texts[i] = ewl_text_new(NULL);
		ewl_widget_show(texts[i]);
	}

	row = ewl_tree_add_row(tree, prow, texts);

	FREE(texts);

	DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to hold the new entry row
 * @param prow: the parent row of the new entry row
 * @param text: the array of strings that hold the entry text to be added
 * @brief Add a row of text entries to a tree
 *
 * @return Returns a pointer to a new row on success, NULL on failure.
 */
Ewl_Widget *ewl_tree_add_entry_row(Ewl_Tree *tree, Ewl_Row *prow, char **text)
{
	int i;
	Ewl_Widget **entries;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("tree", tree, NULL);

	entries = NEW(Ewl_Widget *, tree->ncols);
	if (!entries)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	for (i = 0; i < tree->ncols; i++) {
		if (text)
			entries[i] = ewl_entry_new(text[i]);
		else
			entries[i] = ewl_entry_new(NULL);
		ewl_widget_show(entries[i]);
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
	Ewl_Container *c;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("row", row);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);
	c = EWL_CONTAINER(row);

	if (c->children) {
		while ((w = ewd_list_goto_first(c->children)))
			ewl_container_remove_child(c, w);
	}

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

	if (!ewl_container_init(EWL_CONTAINER(node), "node"))
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	
	ewl_container_show_notify(EWL_CONTAINER(node),
				  ewl_tree_node_child_show_cb);
	ewl_container_resize_notify(EWL_CONTAINER(node),
				    ewl_tree_node_resize_cb);
	ewl_container_remove_notify(EWL_CONTAINER(node),
				    ewl_tree_node_child_hide_cb);

	ewl_object_set_fill_policy(EWL_OBJECT(node), EWL_FLAG_FILL_HFILL |
			EWL_FLAG_FILL_HSHRINK);

	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
			ewl_tree_node_configure_cb, NULL);
	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CLICKED,
			ewl_tree_node_clicked_cb, NULL);

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

	if (!EWL_CONTAINER(node)->children)
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(EWL_CONTAINER(node)->children);
	while ((w = ewd_list_next(EWL_CONTAINER(node)->children))) {
		if (w != node->row)
			ewl_widget_hide(w);
	}

	node->expanded = EWL_TREE_NODE_COLLAPSED;

	ewl_widget_set_state(EWL_WIDGET(node), "collapsed");

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

	if (!EWL_CONTAINER(node)->children)
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(EWL_CONTAINER(node)->children);
	while ((w = ewd_list_next(EWL_CONTAINER(node)->children))) {
		if (w != node->row)
			ewl_widget_show(w);
	}

	ewl_widget_set_state(EWL_WIDGET(node), "expanded");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
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

	if (!c->children)
		DRETURN(DLEVEL_STABLE);

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

void
ewl_tree_node_clicked_cb(Ewl_Widget * w, void *ev_data, void *user_data)
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

void
ewl_tree_node_child_show_cb(Ewl_Container *c, Ewl_Widget *w)
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

void
ewl_tree_node_child_hide_cb(Ewl_Container *c, Ewl_Widget *w)
{
	int width;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(c);

	ewl_object_set_preferred_h(EWL_OBJECT(c), PREFERRED_H(c) -
				   ewl_object_get_preferred_h(EWL_OBJECT(w)));

	width = ewl_object_get_preferred_w(EWL_OBJECT(w));
	if (PREFERRED_W(c) >= width)
		ewl_container_prefer_largest(c, EWL_ORIENTATION_HORIZONTAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(c);

	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_object_set_preferred_w(EWL_OBJECT(c),
					   PREFERRED_W(c) + size);
	else
		ewl_object_set_preferred_h(EWL_OBJECT(c),
					   PREFERRED_H(c) + size);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_row_select_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Tree *tree;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = EWL_TREE_NODE(w->parent);
	tree = node->tree;

	tree->selected = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
