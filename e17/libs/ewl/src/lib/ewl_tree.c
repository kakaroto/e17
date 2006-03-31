#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static int ewl_tree_row_pos = 0;

/**
 * @param columns: the number of columns to display
 * @return Returns NULL on failure, a new tree widget on success.
 * @brief Allocate and initialize a new tree widget
 *
 * The paramater @a columns can be modified at a later time to display
 * a different number of columns.
 */
Ewl_Widget *
ewl_tree_new(unsigned short columns)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("columns", columns, NULL);

	w = NEW(Ewl_Tree, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree_init(EWL_TREE(w), columns)) {
		ewl_widget_destroy(w);
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
int
ewl_tree_init(Ewl_Tree *tree, unsigned short columns)
{
	int i;
	Ewl_Widget *header;
	Ewl_Widget *button;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_PARAM_PTR_RET("columns", columns, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(tree)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(tree), EWL_TREE_TYPE);
	ewl_widget_inherit(EWL_WIDGET(tree), EWL_TREE_TYPE);

	ewl_container_show_notify_set(EWL_CONTAINER(tree),
				      (Ewl_Child_Show)ewl_tree_child_resize_cb);
	ewl_container_hide_notify_set(EWL_CONTAINER(tree),
				      (Ewl_Child_Hide)ewl_tree_child_resize_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(tree),
				    (Ewl_Child_Resize)ewl_tree_child_resize_cb);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_SHRINK |
				   EWL_FLAG_FILL_FILL);
	tree->selected = ecore_list_new();

	ewl_callback_append(EWL_WIDGET(tree), EWL_CALLBACK_CONFIGURE,
			    ewl_tree_configure_cb, NULL);

	ewl_callback_prepend(EWL_WIDGET(tree), EWL_CALLBACK_DESTROY,
			    ewl_tree_destroy_cb, NULL);

	tree->ncols = columns;

	header = ewl_paned_new();
	for (i = 0; i < tree->ncols; i++) {
		button = ewl_button_new();
		ewl_box_orientation_set(EWL_BOX(button),
				EWL_ORIENTATION_VERTICAL);
		ewl_object_fill_policy_set(EWL_OBJECT(button),
				EWL_FLAG_FILL_HSHRINK |
				EWL_FLAG_FILL_HFILL);
		ewl_container_child_append(EWL_CONTAINER(header), button);

    		ewl_widget_show(button);
	}

	ewl_container_child_append(EWL_CONTAINER(tree), header);
	ewl_widget_show(header);
	tree->header = header;

	tree->scrollarea = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(tree), tree->scrollarea);
	ewl_callback_append(tree->scrollarea, EWL_CALLBACK_VALUE_CHANGED,
			    ewl_tree_hscroll_cb, tree);
	ewl_widget_show(tree->scrollarea);

	ewl_container_redirect_set(EWL_CONTAINER(tree),
				   EWL_CONTAINER(tree->scrollarea));

	ewl_callback_append(EWL_WIDGET(tree->header), EWL_CALLBACK_CONFIGURE,
				ewl_tree_header_configure_cb, tree->scrollarea);
	ewl_tree_headers_visible_set(tree, 1);
	ewl_tree_expandable_rows_set(tree, 1);

	ewl_widget_focusable_set(EWL_WIDGET(tree), FALSE);

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
void
ewl_tree_headers_set(Ewl_Tree *tree, char **headers)
{
	unsigned short i;
	Ewl_Widget *button;
	Ewl_Widget *header;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);

	if (!EWL_CONTAINER(tree)->children)
		DRETURN(DLEVEL_STABLE);

	header = tree->header;

	ewl_container_child_iterate_begin(EWL_CONTAINER(header));
	for (i = 0; i < tree->ncols; i++) {
		button = ewl_container_child_next(EWL_CONTAINER(header));
		if (!button)
			break;

		ewl_button_label_set(EWL_BUTTON(button), headers[i]);

		if (!tree->headers_visible && VISIBLE(button))
			ewl_widget_hide(button);
		else if (tree->headers_visible && HIDDEN(button))
			ewl_widget_show(button);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the header visiblity
 * @param visible: The visiblity to set
 * @return Returns no value.
 * @brief Sets the visiblity of the headers in the tree
 */
void
ewl_tree_headers_visible_set(Ewl_Tree *tree, unsigned int visible)
{
	Ewl_Widget *button;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);
 
	tree->headers_visible = visible;
 
	row = tree->header;

	ewl_container_child_iterate_begin(EWL_CONTAINER(row));
	while ((button = ewl_container_child_next(EWL_CONTAINER(row)))) {
		if ((visible) && (HIDDEN(button)))
			ewl_widget_show(button);
		else if ((!visible) && (VISIBLE(button)))
			ewl_widget_hide(button);

	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree the get the header visiblity
 * @return Returns the header visibliity of the tree
 * @brief Gets the header visibility of the tree
 */
unsigned int
ewl_tree_headers_visible_get(Ewl_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, 0);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, 0);

	DRETURN_INT(tree->headers_visible, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the row expandability
 * @param expand: The expandability to set
 * @return Returns no value.
 * @brief Sets the expandability of the rows in the tree
 */
void
ewl_tree_expandable_rows_set(Ewl_Tree *tree, unsigned int expand)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);
 
	tree->expands_visible = expand;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree the get the header visiblity
 * @return Returns the header visibliity of the tree
 * @brief Gets the header visibility of the tree
 */
unsigned int
ewl_tree_expandable_rows_get(Ewl_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, 0);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, 0);

	DRETURN_INT(tree->expands_visible, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to hold the widgets
 * @param prow: the parent row of the new row for the added widgets
 * @param children: a NULL terminated array of widgets to add to the tree
 * @return Returns a pointer to a new row on success, NULL on failure.
 * @brief Add a group of widgets to a row in the tree
 *
 * Adds a row to a specified @a tree with a parent row of @a prow and built
 * from the widgets in the array @a children. The created row is nested below
 * @a prow, and if @a prow is NULL the row is appended to the end of the list
 * at the top level. The array @a children must be equal in size to the number
 * of columns in @a tree. It is valid for the entries in @a children to be
 * NULL, this creates an empty cell.
 */
Ewl_Widget *
ewl_tree_row_add(Ewl_Tree *tree, Ewl_Row *prow, Ewl_Widget **children)
{
	int i;
	Ewl_Widget *w;
	Ewl_Widget *row;
	Ewl_Widget *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, NULL);

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

	ewl_tree_node_expandable_set(EWL_TREE_NODE(node), tree->expands_visible);

	row = ewl_row_new();
	if (!row) {
		ewl_widget_destroy(node);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	ewl_row_header_set(EWL_ROW(row), EWL_ROW(tree->header));

	ewl_widget_show(row);

	EWL_TREE_NODE(node)->tree = tree;
	EWL_TREE_NODE(node)->row = row;
	ewl_container_child_append(EWL_CONTAINER(node), row);
	ewl_callback_append(row, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_tree_row_select_cb, NULL);
	ewl_callback_append(row, EWL_CALLBACK_HIDE,
			    ewl_tree_row_hide_cb, NULL);

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

		ewl_container_child_append(EWL_CONTAINER(row), cell);
		ewl_widget_show(cell);

		if (children[i]) {
			ewl_container_child_append(EWL_CONTAINER(cell),
						   children[i]);
		}
	}

	/*
	 * Place the new row in the tree.
	 */
	if (prow && w->parent) {
		if (EWL_TREE_NODE(w->parent)->expanded == EWL_TREE_NODE_EXPANDED)
			ewl_widget_show(node);
		ewl_container_child_append(EWL_CONTAINER(w->parent), node);
	}
	else {
		ewl_container_child_append(EWL_CONTAINER(tree), node);
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
Ewl_Widget *
ewl_tree_text_row_add(Ewl_Tree *tree, Ewl_Row *prow, char **text)
{
	int i;
	Ewl_Widget **texts;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, NULL);

	texts = NEW(Ewl_Widget *, tree->ncols);
	if (!texts)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	for (i = 0; i < tree->ncols; i++) {
		texts[i] = ewl_label_new();
		ewl_object_fill_policy_set(EWL_OBJECT(texts[i]),
				EWL_FLAG_FILL_NONE);
		ewl_object_alignment_set(EWL_OBJECT(texts[i]),
					 EWL_FLAG_ALIGN_LEFT);
		ewl_widget_show(texts[i]);

		if (text) {
			ewl_label_text_set(EWL_LABEL(texts[i]), text[i]);
		}
		else {
			ewl_label_text_set(EWL_LABEL(texts[i]), NULL);
		}
	}

	row = ewl_tree_row_add(tree, prow, texts);

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
Ewl_Widget *
ewl_tree_entry_row_add(Ewl_Tree *tree, Ewl_Row *prow, char **text)
{
	int i;
	Ewl_Widget **entries;
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, NULL);

	entries = NEW(Ewl_Widget *, tree->ncols);
	if (!entries)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	for (i = 0; i < tree->ncols; i++) {
		if (text) {
			entries[i] = ewl_entry_new();
			ewl_text_text_set(EWL_TEXT(entries[i]), text[i]);
		}
		else
			entries[i] = ewl_entry_new();
			ewl_text_text_set(EWL_TEXT(entries[i]), NULL);
		ewl_widget_show(entries[i]);
	}

	row = ewl_tree_row_add(tree, prow, entries);

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
void
ewl_tree_row_remove(Ewl_Tree *tree, Ewl_Row *row)
{
	Ewl_Widget *w;
	Ewl_Container *c;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("row", row);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);
	DCHECK_TYPE("row", row, EWL_ROW_TYPE);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);
	c = EWL_CONTAINER(row);

	if (c->children) {
		while ((w = ecore_dlist_goto_first(c->children)))
			ewl_container_child_remove(c, w);
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
void
ewl_tree_row_destroy(Ewl_Tree *tree, Ewl_Row *row)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("row", row);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);
	DCHECK_TYPE("row", row, EWL_ROW_TYPE);

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
void
ewl_tree_columns_set(Ewl_Tree *tree, unsigned short columns)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("columns", columns);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);

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
void
ewl_tree_row_expand_set(Ewl_Row *row, Ewl_Tree_Node_Flags expanded)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("row", row);
	DCHECK_TYPE("row", row, EWL_ROW_TYPE);

	node = EWL_TREE_NODE(EWL_WIDGET(row)->parent);
	if (node && node->expanded != expanded) {
		if (!expanded || expanded == EWL_TREE_NODE_COLLAPSED) {
			if (!expanded && node->handle)
				ewl_widget_hide(node->handle);
			ewl_tree_node_collapse(EWL_TREE_NODE(node));
		}
		else
			ewl_tree_node_expand(EWL_TREE_NODE(node));

		node->expanded = expanded;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: the tree to set the selection mode
 * @param mode: the new selection mode for the tree
 * @brief Change the selection mode for a specified tree.
 * @return Returns no value.
 */
void
ewl_tree_mode_set(Ewl_Tree *tree, Ewl_Tree_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);

	if (tree->mode == mode)
		DRETURN(DLEVEL_STABLE);

	tree->mode = mode;

	if (mode == EWL_TREE_MODE_NONE)
		ewl_tree_selected_clear(tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: the tree to get the selection mode
 * @brief Retrieve the current selection mode of a tree.
 * @return Returns the current selection mode of the tree.
 */
Ewl_Tree_Mode
ewl_tree_mode_get(Ewl_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, EWL_TREE_MODE_NONE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, EWL_TREE_MODE_NONE);

	DRETURN_INT(tree->mode, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to retrieve selected rows
 * @brief Retrieves a list of selected rows from a tree.
 * @return Returns a list of selected rows on success, NULL on failure.
 */
Ecore_List *
ewl_tree_selected_get(Ewl_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, NULL);

	DRETURN_PTR(tree->selected, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to clear the current selection
 * @brief Clear the current selection from a tree.
 * @return Returns no value.
 */
void
ewl_tree_selected_clear(Ewl_Tree *tree)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE_TYPE);

	while ((w = ecore_list_remove_first(tree->selected)))
		ewl_widget_state_set(w, "tree-deselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to find the row in
 * @param row: The row number to find 
 * @return Returns the given row, or NULL if not found
 */
Ewl_Widget *
ewl_tree_row_find(Ewl_Tree *tree, int row)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE_TYPE, NULL);

	child = ewl_container_child_get(EWL_CONTAINER(tree), row);
	if (child) {
		DRETURN_PTR(EWL_TREE_NODE(child)->row, DLEVEL_STABLE);
	} else {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
}

/**
 * @param row: the row to retrieve a column from
 * @param i: the column to retreive from the row
 * @brief Retreives the actual widget added via row_add instead of the cell
 * @return Returns the widget in the column of the row sent
 *
 * The behavior of this function is undefined if columns are not accounted for
 * in the children of the row (if NULLs were passed in from row_add).
 */
Ewl_Widget *
ewl_tree_row_column_get(Ewl_Row *row, int i)
{
	Ewl_Widget *w, *cell;
	Ecore_List *children;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("row", row, NULL);
	DCHECK_TYPE_RET("row", row, EWL_ROW_TYPE, NULL);

	children = EWL_CONTAINER(row)->children;

	w = NULL;
	cell = ecore_dlist_goto_index(children, i);
	if( cell )
		w = ecore_dlist_goto_first(EWL_CONTAINER(cell)->children);

	DRETURN_PTR(w, DLEVEL_STABLE);
}

static void
ewl_tree_row_signal(Ewl_Tree *tree __UNUSED__, Ewl_Widget *row)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("row", row);
	DCHECK_TYPE("row", row, EWL_WIDGET_TYPE);

	if (ewl_tree_row_pos & 1) {
		ewl_widget_state_set(row, "odd");
	}
	else {
		ewl_widget_state_set(row, "even");
	}
	ewl_tree_row_pos++;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_node_row_walk_signal(Ewl_Tree *tree, Ewl_Tree_Node *node)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_container_child_iterate_begin(EWL_CONTAINER(node));
	while ((child = ewl_container_child_next(EWL_CONTAINER(node)))) {
		if (ewl_widget_type_is(child, EWL_TREE_NODE_TYPE)) {
			if (VISIBLE(child)) {
				ewl_tree_node_row_walk_signal(tree, EWL_TREE_NODE(child));
			}
		}
		else if (ewl_widget_type_is(child, EWL_ROW_TYPE)) {
			ewl_tree_row_signal(tree, child);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_row_walk_signal(Ewl_Tree *tree)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_tree_row_pos = 0;
	ewl_container_child_iterate_begin(EWL_CONTAINER(tree));
	while ((child = ewl_container_child_next(EWL_CONTAINER(tree)))) {
		if (ewl_widget_type_is(child, EWL_TREE_NODE_TYPE)) {
			ewl_tree_node_row_walk_signal(tree, EWL_TREE_NODE(child));
		}
		else if (ewl_widget_type_is(child, EWL_ROW_TYPE)) {
			ewl_tree_row_signal(tree, child);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	int x, width, height;
	double scroll;
	Ewl_Tree *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	tree = EWL_TREE(w);
	scroll = ewl_scrollpane_hscrollbar_value_get(EWL_SCROLLPANE(tree->scrollarea));
	width = ewl_object_preferred_w_get(EWL_OBJECT(tree->header));
	x = CURRENT_X(tree);
	if (scroll > 0 && width > CURRENT_W(tree))
		x -= (int)((double)scroll * (double)(width - CURRENT_W(tree)));

	ewl_object_geometry_request(EWL_OBJECT(tree->header), x, CURRENT_Y(tree),
				    CURRENT_W(tree), 1);
	height = ewl_object_current_h_get(EWL_OBJECT(tree->header));
	ewl_object_geometry_request(EWL_OBJECT(tree->scrollarea),
				    CURRENT_X(tree), CURRENT_Y(tree) + height,
				    CURRENT_W(tree), CURRENT_H(tree) - height);

	ewl_tree_row_walk_signal(tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_header_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget *scrollarea = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_widget_configure(scrollarea);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Tree *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	tree = EWL_TREE(w);
	ecore_list_destroy(tree->selected);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_child_resize_cb(Ewl_Container *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);
	ewl_container_sum_prefer(c, EWL_ORIENTATION_VERTICAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a newly allocated node on success, NULL on failure.
 * @brief Allocate and initialize a new node
 */
Ewl_Widget *
ewl_tree_node_new()
{
	Ewl_Widget *node;

	DENTER_FUNCTION(DLEVEL_STABLE);

	node = NEW(Ewl_Tree_Node, 1);
	if (!node)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree_node_init(EWL_TREE_NODE(node))) {
		ewl_widget_destroy(node);
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
int
ewl_tree_node_init(Ewl_Tree_Node *node)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("node", node, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(node)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(node), EWL_TREE_NODE_TYPE);
	ewl_widget_inherit(EWL_WIDGET(node), EWL_TREE_NODE_TYPE);
	
	ewl_container_show_notify_set(EWL_CONTAINER(node),
				  ewl_tree_node_child_show_cb);
	ewl_container_hide_notify_set(EWL_CONTAINER(node),
				  ewl_tree_node_child_hide_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(node),
				    ewl_tree_node_resize_cb);
	ewl_container_add_notify_set(EWL_CONTAINER(node),
				    ewl_tree_node_child_add_cb);
	ewl_container_remove_notify_set(EWL_CONTAINER(node),
				    ewl_tree_node_child_del_cb);

	ewl_object_fill_policy_set(EWL_OBJECT(node), EWL_FLAG_FILL_HFILL |
							EWL_FLAG_FILL_HSHRINK);

	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
			ewl_tree_node_configure_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(node), EWL_CALLBACK_DESTROY,
			    ewl_tree_node_destroy_cb, NULL);

	node->expanded = EWL_TREE_NODE_COLLAPSED;

	ewl_widget_focusable_set(EWL_WIDGET(node), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 */
void
ewl_tree_node_expandable_set(Ewl_Tree_Node *node, int expand)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);
	DCHECK_TYPE("node", node, EWL_TREE_NODE_TYPE);

	/*
	 * The handle for expanding and collapsing the branch point at this
	 * node.
	 */
	if (expand && !node->handle) {
		node->handle = ewl_check_new();
		ewl_object_fill_policy_set(EWL_OBJECT(node->handle),
					   EWL_FLAG_FILL_NONE);
		ewl_object_alignment_set(EWL_OBJECT(node->handle),
				EWL_FLAG_ALIGN_TOP);
		ewl_container_child_prepend(EWL_CONTAINER(node), node->handle);
		ewl_callback_append(node->handle, EWL_CALLBACK_VALUE_CHANGED,
				    ewl_tree_node_toggle_cb, node);
		ewl_widget_show(node->handle);
	}
	else if (node->handle) {
		ewl_widget_destroy(node->handle);
		node->handle = NULL;
	}
}

/**
 */
int
ewl_tree_node_expandable_get(Ewl_Tree_Node *node)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("node", node, FALSE);
	DCHECK_TYPE_RET("node", node, EWL_TREE_NODE_TYPE, FALSE);

	if (node->handle)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param node: the node in the tree to collapse
 * @return Returns no value. Hides the rows below @a node.
 * @brief Collapse a node in the tree
 */
void
ewl_tree_node_collapse(Ewl_Tree_Node *node)
{
	Ewl_Widget *w;
	Ecore_List *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);
	DCHECK_TYPE("node", node, EWL_TREE_NODE_TYPE);

	if (node->expanded == EWL_TREE_NODE_COLLAPSED)
		DRETURN(DLEVEL_STABLE);

	if (!EWL_CONTAINER(node)->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Queue the parent tree for configure, this handles the issue of
	 * redrawing the alternating colors on expand and doing it early
	 * avoids duplicate or long list walks for queueing child widgets.
	 */
	if (node->tree)
		ewl_widget_configure(EWL_WIDGET(node->tree));

	tmp = ecore_list_new();

	ecore_dlist_goto_first(EWL_CONTAINER(node)->children);
	while ((w = ecore_dlist_next(EWL_CONTAINER(node)->children))) {
		if (w != node->row && w != node->handle)
			ecore_list_append(tmp, w);
	}

	while ((w = ecore_list_remove_first(tmp))) {
		ewl_widget_hide(w);
	}

	ecore_list_destroy(tmp);

	node->expanded = EWL_TREE_NODE_COLLAPSED;

	ewl_widget_state_set(EWL_WIDGET(node), "collapsed");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param node: the node in the tree to expand
 * @return Returns no value. Hides the rows below @a node.
 * @brief Expand a node in the tree
 */
void
ewl_tree_node_expand(Ewl_Tree_Node *node)
{
	Ewl_Widget *w;
	Ecore_List *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);
	DCHECK_TYPE("node", node, EWL_TREE_NODE_TYPE);

	if (node->expanded == EWL_TREE_NODE_EXPANDED)
		DRETURN(DLEVEL_STABLE);

	node->expanded = EWL_TREE_NODE_EXPANDED;

	if (!EWL_CONTAINER(node)->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Queue the parent tree for configure, this handles the issue of
	 * redrawing the alternating colors on expand and doing it early
	 * avoids duplicate or long list walks for queueing child widgets.
	 */
	if (node->tree)
		ewl_widget_configure(EWL_WIDGET(node->tree));

	tmp = ecore_list_new();

	ecore_dlist_goto_first(EWL_CONTAINER(node)->children);
	while ((w = ecore_dlist_next(EWL_CONTAINER(node)->children))) {
		if (w != node->row && w != node->handle)
			ecore_list_append(tmp, w);
	}

	while ((w = ecore_list_remove_first(tmp))) {
		ewl_widget_show(w);
	}

	ecore_list_destroy(tmp);

	ewl_widget_state_set(EWL_WIDGET(node), "expanded");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Tree_Node *node;
	Ewl_Container *c;
	Ewl_Object *child;
	int x, y, hw = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = EWL_TREE_NODE(w);
	if (!node->tree)
		DRETURN(DLEVEL_STABLE);

	c = EWL_CONTAINER(w);

	if (!c->children)
		DRETURN(DLEVEL_STABLE);

	ecore_dlist_goto_first(c->children);
	x = CURRENT_X(w);
	y = CURRENT_Y(w);

	if (node->handle) {
		ewl_object_geometry_request(EWL_OBJECT(node->handle),
				CURRENT_X(w), CURRENT_Y(w), CURRENT_W(w),
				CURRENT_H(w));
		hw = ewl_object_current_w_get(EWL_OBJECT(node->handle));
		x += hw;
	}

	/*
	 * All subsequent children are lower nodes and rows.
	 */
	while ((child = ecore_dlist_next(c->children))) {
		if (VISIBLE(child) && EWL_WIDGET(child) != node->handle) {
			ewl_object_geometry_request(child, x, y, CURRENT_W(w) - hw,
						    ewl_object_preferred_h_get(child));
			y += ewl_object_current_h_get(child);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					 void *user_data __UNUSED__)
{
	Ewl_Tree *tree;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = EWL_TREE_NODE(w);
	if (!node->tree)
		DRETURN(DLEVEL_STABLE);

	tree = EWL_TREE(node->tree);
	if (ecore_list_goto(tree->selected, node->row)) {
		ecore_list_remove(tree->selected);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_toggle_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
							void *user_data)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = EWL_TREE_NODE(user_data);

	if (node->expanded == EWL_TREE_NODE_NOEXPAND)
		DRETURN(DLEVEL_STABLE);

	if (node->expanded == EWL_TREE_NODE_EXPANDED)
		ewl_tree_node_collapse(node);
	else
		ewl_tree_node_expand(node);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_child_add_cb(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	node = EWL_TREE_NODE(c);
	if (w != node->handle && !node->row)
		node->row = node->handle;

	if (ecore_dlist_nodes(c->children) > 2 ) {
		if (node->handle && HIDDEN(node->handle))
			ewl_widget_show(node->handle);
	}
	else if (node->handle && VISIBLE(node->handle)) {
		ewl_widget_hide(node->handle);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_child_del_cb(Ewl_Container *c, Ewl_Widget *w, int idx __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	ewl_tree_node_child_add_cb(c, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_child_show_cb(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	node = EWL_TREE_NODE(c);

	if (node->handle && node->expanded) {
		ewl_container_sum_prefer(c, EWL_ORIENTATION_VERTICAL);
		if (REALIZED(node->handle) && VISIBLE(node->handle))
			ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
					PREFERRED_H(c) -
					ewl_object_preferred_h_get(EWL_OBJECT(node->handle)));
	}
	else {
		ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
					   ewl_object_preferred_h_get(EWL_OBJECT(node->row)));
	}

	ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);
	if (node->handle && REALIZED(node->handle) && VISIBLE(node->handle))
		ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) +
			ewl_object_preferred_w_get(EWL_OBJECT(node->handle)));

	if (!node->expanded && node->handle)
		ewl_widget_hide(node->handle);

	ewl_widget_configure(EWL_WIDGET(c)->parent);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_child_hide_cb(Ewl_Container *c, Ewl_Widget *w)
{
	int width;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = EWL_TREE_NODE(c);

	if (w == node->handle)
		DRETURN(DLEVEL_STABLE);

	if (ecore_dlist_nodes(c->children) < 3) {
		if (node->handle && VISIBLE(node->handle))
			ewl_widget_hide(node->handle);
	}

	ewl_object_preferred_inner_h_set(EWL_OBJECT(c), PREFERRED_H(c) -
				   ewl_object_preferred_h_get(EWL_OBJECT(w)));

	width = ewl_object_preferred_w_get(EWL_OBJECT(w));
	if (PREFERRED_W(c) >= width)
		ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_resize_cb(Ewl_Container *c, Ewl_Widget *w,
		int size __UNUSED__, Ewl_Orientation o __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_tree_node_child_show_cb(c, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_row_select_cb(Ewl_Widget *w, void *ev_data,
					void *user_data __UNUSED__)
{
	Ewl_Tree *tree;
	Ewl_Tree_Node *node;
	Ewl_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ev = ev_data;
	node = EWL_TREE_NODE(w->parent);
	tree = node->tree;

	if (tree->mode == EWL_TREE_MODE_SINGLE ||
	    !(ev->modifiers & EWL_KEY_MODIFIER_SHIFT))
		ewl_tree_selected_clear(tree);

	if (tree->mode != EWL_TREE_MODE_NONE) {
		if (ecore_list_goto(tree->selected, w) == NULL)
			ecore_list_append(tree->selected, w);
		ewl_widget_state_set(w, "tree-selected");
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_row_hide_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Tree *tree;
	Ewl_Tree_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = EWL_TREE_NODE(w->parent);
	tree = node->tree;

	if (ecore_list_goto(tree->selected, w)) {
		ecore_list_remove(tree->selected);
		ewl_widget_state_set(w, "tree-deselected");
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_hscroll_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_widget_configure(user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

