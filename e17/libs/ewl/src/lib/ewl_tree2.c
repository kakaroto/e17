/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_tree2.h"
#include "ewl_button.h"
#include "ewl_cell.h"
#include "ewl_check.h"
#include "ewl_expansion.h"
#include "ewl_label.h"
#include "ewl_paned.h"
#include "ewl_row.h"
#include "ewl_tree2_view_scrolled.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

static void ewl_tree2_cb_view_change(Ewl_MVC *mvc);
static void ewl_tree2_build_tree(Ewl_Tree2 *tree);
static void ewl_tree2_build_tree_rows(Ewl_Tree2 *tree, 
			Ewl_Model *model, void *data,
			int colour, Ewl_Widget *parent, 
			int hidden);
static void ewl_tree2_cb_column_free(void *data);
static void ewl_tree2_cb_header_changed(Ewl_Widget *w, void *ev, 
							void *data);

static void ewl_tree2_cb_row_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree2_cb_cell_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree2_cb_selected_change(Ewl_MVC *mvc);
static Ewl_Widget *ewl_tree2_widget_at(Ewl_MVC *mvc, void *data, int row, int column);

static void ewl_tree2_create_expansions_hash(Ewl_Tree2 *tree);

/**
 * @return Returns NULL on failure, a new tree widget on success.
 * @brief Allocate and initialize a new tree widget
 */
Ewl_Widget *
ewl_tree2_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Tree2, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree2_init(EWL_TREE2(w))) 
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param tree: the tree widget to be initialized
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the contents of a tree widget
 *
 * The contents of the tree widget @a tree are initialized to their defaults.
 */
int
ewl_tree2_init(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);

	if (!ewl_mvc_init(EWL_MVC(tree)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(tree), EWL_TREE2_TYPE);
	ewl_widget_inherit(EWL_WIDGET(tree), EWL_TREE2_TYPE);

	ewl_mvc_selected_change_cb_set(EWL_MVC(tree), 
					ewl_tree2_cb_selected_change);

	ewl_object_fill_policy_set(EWL_OBJECT(tree), 
				EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_FILL);

	tree->columns = ecore_list_new();
	ecore_list_set_free_cb(tree->columns, ewl_tree2_cb_column_free);

	tree->type = EWL_TREE_SELECTION_TYPE_CELL;

	tree->header = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(tree), tree->header);
	ewl_widget_appearance_set(EWL_WIDGET(tree->header), "tree_header");
	ewl_object_fill_policy_set(EWL_OBJECT(tree->header), EWL_FLAG_FILL_HFILL);
	ewl_callback_append(tree->header, EWL_CALLBACK_VALUE_CHANGED,
					ewl_tree2_cb_header_changed, tree);
	ewl_widget_show(tree->header);

	/* set the default row view */
	ewl_mvc_view_change_cb_set(EWL_MVC(tree), ewl_tree2_cb_view_change);
	ewl_mvc_view_set(EWL_MVC(tree), ewl_tree2_view_scrolled_get());

	ewl_tree2_headers_visible_set(tree, TRUE);
	ewl_tree2_fixed_rows_set(tree, FALSE);

	ewl_callback_append(EWL_WIDGET(tree), EWL_CALLBACK_CONFIGURE,
					ewl_tree2_cb_configure, NULL);
	ewl_callback_prepend(EWL_WIDGET(tree), EWL_CALLBACK_DESTROY,
					ewl_tree2_cb_destroy, NULL);

	ewl_widget_focusable_set(EWL_WIDGET(tree), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to append the column too
 * @param view: The view to use for this column
 * @param sortable: Is the column sortable
 * @return Returns no value.
 * @brief Append a new column to the tree
 */
void
ewl_tree2_column_append(Ewl_Tree2 *tree, Ewl_View *view, 
				unsigned int sortable)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	c = ewl_tree2_column_new();
	if (!c)
	{
		DWARNING("Unable to create new tree column.");
		DRETURN(DLEVEL_STABLE);
	}

	ewl_tree2_column_view_set(c, view);
	ewl_tree2_column_mvc_set(c, EWL_MVC(tree));
	ewl_tree2_column_sortable_set(c, sortable);

	ecore_list_append(tree->columns, c);
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to prepend the column too
 * @param view: The view to use for this column
 * @param sortable: Is the column sortable
 * @return Returns no value.
 * @brief Prepend a new column to the tree
 */
void
ewl_tree2_column_prepend(Ewl_Tree2 *tree, Ewl_View *view, 
				unsigned int sortable)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	c = ewl_tree2_column_new();
	if (!c)
	{
		DWARNING("Unable to create new tree column.");
		DRETURN(DLEVEL_STABLE);
	}

	ewl_tree2_column_view_set(c, view);
	ewl_tree2_column_mvc_set(c, EWL_MVC(tree));
	ewl_tree2_column_sortable_set(c, sortable);

	ecore_list_prepend(tree->columns, c);
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to insert the column into
 * @param view: The view to use for this column
 * @param idx: The index to insert into 
 * @param sortable: Is the column sortable?
 * @return Returns no value.
 * @brief Insert a new column into the tree
 */
void
ewl_tree2_column_insert(Ewl_Tree2 *tree, Ewl_View *view, unsigned int idx,
						unsigned int sortable)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	c = ewl_tree2_column_new();
	if (!c)
	{
		DWARNING("Unable to create new tree column.");
		DRETURN(DLEVEL_STABLE);
	}

	ewl_tree2_column_view_set(c, view);
	ewl_tree2_column_mvc_set(c, EWL_MVC(tree));
	ewl_tree2_column_sortable_set(c, sortable);

	ecore_list_goto_index(tree->columns, idx);
	ecore_list_insert(tree->columns, c);
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to remove the column from
 * @param idx: The column index to remove
 * @return Returns no value
 * @brief Remove a column from the tree
 */
void
ewl_tree2_column_remove(Ewl_Tree2 *tree, unsigned int idx)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	ecore_list_goto_index(tree->columns, idx);
	c = ecore_list_remove(tree->columns);

	ewl_tree2_column_destroy(c);
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to toggle the header visibility
 * @param visible: The visiblity to set the tree to (TRUE == on, FALSE == off)
 * @return Returns no value
 * @brief Toggle if the header is visible in the tree
 */
void
ewl_tree2_headers_visible_set(Ewl_Tree2 *tree, unsigned char visible)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	if (tree->headers_visible == visible)
		DRETURN(DLEVEL_STABLE);

	tree->headers_visible = !!visible;

	if (!tree->headers_visible)
		ewl_widget_hide(tree->header);
	else
		ewl_widget_show(tree->header);

	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the header visiblity from
 * @return Returns the current header visiblity of the tree
 * @brief Retrieve if the header is visible in the tree
 */
unsigned int
ewl_tree2_headers_visible_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, FALSE);

	DRETURN_INT(tree->headers_visible, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the mode from
 * @return Returns the current Ewl_Tree_Selection_Type of the tree
 * @brief Get the selection type from the tree
 */
Ewl_Tree_Selection_Type
ewl_tree2_selection_type_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, EWL_TREE_SELECTION_TYPE_CELL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE,
					EWL_TREE_SELECTION_TYPE_CELL);

	DRETURN_INT(tree->type, DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to set the mode into
 * @param type: The Ewl_Tree_Selection_Mode to set into the tree
 * @return Returns no value.
 * @brief Set the mode of the tree
 */
void 
ewl_tree2_selection_type_set(Ewl_Tree2 *tree, Ewl_Tree_Selection_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	if (tree->type == type)
		DRETURN(DLEVEL_STABLE);

	tree->type = type;

	/* if we switched types then the current set of selections isn't
	 * valid anymore so we clear them out */
	ewl_mvc_selected_clear(EWL_MVC(tree));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the fixed row flag into
 * @param fixed: The fixed row flag to set into the tree
 * @return Returns no value.
 * @brief Set the fixed row size of the tree
 */
void
ewl_tree2_fixed_rows_set(Ewl_Tree2 *tree, unsigned int fixed)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	tree->fixed = fixed;
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the fixed row flag from
 * @return Returns the current fixed row flag of the tree
 * @brief Retrieve the fixed row size of the tree
 */
unsigned int
ewl_tree2_fixed_rows_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, FALSE);

	DRETURN_INT(tree->fixed, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the widget that contains the tree rows
 * @brief Retrieves the widget containing the tree rows
 *
 * XXX This is a badly named function. Too close to the MVC view name
 */
Ewl_Widget *
ewl_tree2_view_widget_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, NULL);

	DRETURN_PTR(tree->rows, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the expansion into
 * @param data: The data that contains the expansion
 * @param row: The row to expand
 * @return Returns no value
 * @brief When the tree displays the data in @a data it will expand the give
 * @a row. This @a data is the parent of the expansion row.
 */
void
ewl_tree2_row_expand(Ewl_Tree2 *tree, void *data, unsigned int row)
{
	Ecore_List *exp;
	int i, created = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	/* nothing to do if already expanded */
	if (ewl_tree2_row_expanded_is(tree, data, row)) DRETURN(DLEVEL_STABLE);

	if (!tree->expansions)
	{
		ewl_tree2_create_expansions_hash(tree);
		exp = ecore_list_new();
		created = 1;
	}
	else
	{
		exp = ecore_hash_get(tree->expansions, data);
		if (!exp) exp = ecore_list_new();
	}

	ecore_list_goto_first(exp);
	while ((i = (int)ecore_list_next(exp)))
	{
		if (i > (int)row) break;
	}

	ecore_list_insert(exp, (void *)row);

	if (created)
		ecore_hash_set(tree->expansions, data, exp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to collapse the row of
 * @param data: The data that contains the collapsed row
 * @param row: The row to collapse
 * @return Returns no value
 * @brief Sets the given @a row to collapsed for the given @a data in @a
 * tree
 */
void
ewl_tree2_row_collapse(Ewl_Tree2 *tree, void *data, unsigned int row)
{
	Ecore_List *exp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	/* if this tree has no expansions we're done */
	if (!tree->expansions) DRETURN(DLEVEL_STABLE);

	exp = ecore_hash_get(tree->expansions, data);

	/* if no expansion points for this data we're done */
	if (!exp) DRETURN(DLEVEL_STABLE);

	/* nothing to do if the row isn't expanded */
	if (!ewl_tree2_row_expanded_is(tree, data, row)) DRETURN(DLEVEL_STABLE);

	/* we found the item so we can remove it */
	ecore_list_remove(exp);

	/* no expansions left we can remove this data from the hash */
	if (ecore_list_is_empty(exp))
	{
		ecore_hash_remove(tree->expansions, data);
		IF_FREE_LIST(exp);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param data: The set of data to work with
 * @param row: The row to check
 * @return Returns TRUE if the given row is expanded, FALSE otherwise
 * @brief Checks if @a row is expanded in @a data of @a tree
 */
unsigned int
ewl_tree2_row_expanded_is(Ewl_Tree2 *tree, void *data, unsigned int row)
{
	Ecore_List *exp;
	int i, expanded = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_PARAM_PTR_RET("data", data, FALSE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, FALSE);

	/* no expansions in the tree */
	if (!tree->expansions) DRETURN_INT(FALSE, DLEVEL_STABLE);

	exp = ecore_hash_get(tree->expansions, data);

	/* no expansions in this data we're done */
	if (!exp) DRETURN_INT(FALSE, DLEVEL_STABLE);

	/* search for this row in the expansions */
	ecore_list_goto_first(exp);
	while ((i = (int)ecore_list_current(exp)))
	{
		if (i == (int)row)
		{
			expanded = TRUE;
			break;
		}
		ecore_list_next(exp);
	}
	
	DRETURN_INT(expanded, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_tree2_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Tree2 *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TREE2(w);

	IF_FREE_LIST(t->columns);
	IF_FREE_HASH(t->expansions);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_tree2_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Tree2 *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_TREE2_TYPE);

	tree = EWL_TREE2(w);

	/* if the tree isn't dirty we're done */
	if (!ewl_mvc_dirty_get(EWL_MVC(tree))) 
		DRETURN(DLEVEL_STABLE);

	ewl_tree2_build_tree(tree);
	ewl_tree2_cb_selected_change(EWL_MVC(tree));
	ewl_mvc_dirty_set(EWL_MVC(tree), FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The header that was clicked
 * @param ev: UNUSED
 * @param data: The column related to this header
 * @return Returns no value
 * @brief Sorts the tree by the given column
 */
void
ewl_tree2_cb_column_sort(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Tree2_Column *c, *col;
	Ewl_MVC *mvc;
	Ewl_Model *model;
	int index = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	c = data;

	mvc = ewl_tree2_column_mvc_get(c);
	model = ewl_mvc_model_get(mvc);

	/* sanity check */
	if (!model || !model->sort)
	{
		DWARNING("In ewl_tree2_cb_column_sort without a sort cb.");
		DRETURN(DLEVEL_STABLE);
	}

	/* loop over the columns and reset the sort settings */
	ecore_list_goto_first(EWL_TREE2(mvc)->columns);
	while ((col = ecore_list_next(EWL_TREE2(mvc)->columns)))
	{
		/* skip the current column */
		if (col == c)
		{
			/* we're the index before the one we're now on */
			index = ecore_list_index(EWL_TREE2(mvc)->columns) - 1;
			continue;
		}

		col->sort = EWL_SORT_DIRECTION_NONE;
	}

	/* update our sort direction and call the sort function, skipping
	 * over SORT_NONE */
	c->sort = ((c->sort + 1) % EWL_SORT_DIRECTION_MAX);
	if (!c->sort) c->sort ++;

	model->sort(ewl_mvc_data_get(mvc), index, c->sort);
	ewl_mvc_dirty_set(mvc, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_view_change(Ewl_MVC *mvc)
{
	Ewl_View *view;
	Ewl_Tree2 *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_TREE2_TYPE);

	t = EWL_TREE2(mvc);
	view = ewl_mvc_view_get(mvc);

	/* destroy the old view, create a new one and redisplay the tree */
	if (t->rows) ewl_widget_destroy(t->rows);

	t->rows = view->fetch(NULL, 0, 0);
	ewl_tree2_view_tree2_set(EWL_TREE2_VIEW(t->rows), t);
	ewl_container_child_append(EWL_CONTAINER(t), t->rows);
	ewl_widget_show(t->rows);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_header_build(Ewl_Tree2 *tree, Ewl_Tree2_Column *col, void *mvc_data, int column)
{
	Ewl_Widget *h, *c;
	Ewl_Model *model;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("col", col);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	if (!tree->headers_visible) DRETURN(DLEVEL_STABLE);

	if (!col->view->header_fetch)
	{
		DWARNING("Missing header_fetch callback.");
		DRETURN(DLEVEL_STABLE);
	}

	model = ewl_mvc_model_get(EWL_MVC(tree));

	h = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(tree->header), h);
	ewl_widget_appearance_set(h, "header");
	ewl_widget_show(h);

	if (col->sortable)
		ewl_callback_append(h, EWL_CALLBACK_CLICKED, 
					ewl_tree2_cb_column_sort, col);

	c = col->view->header_fetch(mvc_data, column);
	ewl_object_fill_policy_set(EWL_OBJECT(c), 
			EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(h), c);

	/* display the sort arrow if needed */
	if (col->sortable)
	{
		char *state_str;

		c = ewl_button_new();
		ewl_container_child_append(EWL_CONTAINER(h), c);

		if (col->sort == EWL_SORT_DIRECTION_ASCENDING)
			state_str = "ascending";
		else if (col->sort == EWL_SORT_DIRECTION_DESCENDING)
			state_str = "descending";
		else
			state_str = "default";

		ewl_widget_state_set(c, state_str, EWL_STATE_PERSISTENT);
		ewl_widget_internal_set(c, TRUE);
		ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_SHRINK);
		ewl_object_alignment_set(EWL_OBJECT(c), EWL_FLAG_ALIGN_RIGHT);
		ewl_widget_show(c);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_column_build(Ewl_Row *row, Ewl_Model *model, Ewl_View *view, 
				void *mvc_data, int r, int c, Ewl_Widget *node)
{
	Ewl_Widget *cell;
	Ewl_Widget *child;
	void *val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("row", row);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("row", row, EWL_ROW_TYPE);

	cell = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(row), cell);
	ewl_callback_append(cell, EWL_CALLBACK_CLICKED,
				ewl_tree2_cb_cell_clicked, node);
	ewl_widget_show(cell);

	val = model->fetch(mvc_data, r, c);
	if (!val)
	{
		child = ewl_label_new();
		ewl_label_text_set(EWL_LABEL(child), " ");
	}
	else
	{
		child = view->fetch(val, r, c);
	}
	ewl_container_child_append(EWL_CONTAINER(cell), child);
	ewl_widget_show(child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_build_tree(Ewl_Tree2 *tree)
{
	Ewl_Tree2_Column *col;
	int column = 0;
	void *mvc_data;
	Ewl_Model *model;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	mvc_data = ewl_mvc_data_get(EWL_MVC(tree));
	model = ewl_mvc_model_get(EWL_MVC(tree));

	/* setup the headers */
	ewl_container_reset(EWL_CONTAINER(tree->header));
	ecore_list_goto_first(tree->columns);
	while ((col = ecore_list_next(tree->columns)))
	{
		ewl_tree2_header_build(tree, col, mvc_data, column);
		column ++;
	}

	if (!model)
		DRETURN(DLEVEL_STABLE);

	ewl_container_reset(EWL_CONTAINER(tree->rows));
	ewl_tree2_build_tree_rows(tree, model, mvc_data, 0, tree->rows, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_build_tree_rows(Ewl_Tree2 *tree, Ewl_Model *model, void *data,
				int colour, Ewl_Widget *parent, int hidden)
{
	Ewl_Tree2_Column *col;
	int i = 0, column, row_count = 0;

	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("parent", parent);

	row_count = model->count(data);
	if (!row_count)
		DRETURN(DLEVEL_STABLE);

	while (1)
	{
		Ewl_Widget *row, *node;

		node = ewl_tree2_node_new();
		EWL_TREE2_NODE(node)->tree = EWL_WIDGET(tree);
		EWL_TREE2_NODE(node)->row_num = i;
		ewl_mvc_model_set(EWL_MVC(node), model);
		ewl_mvc_data_set(EWL_MVC(node), data);

		ewl_container_child_append(EWL_CONTAINER(parent), node);
		if (!hidden) ewl_widget_show(node);

		row = ewl_row_new();
		ewl_row_header_set(EWL_ROW(row), EWL_ROW(tree->header));
		ewl_container_child_append(EWL_CONTAINER(node), row);
		ewl_callback_append(row, EWL_CALLBACK_CLICKED,  
					ewl_tree2_cb_row_clicked, node);
		EWL_TREE2_NODE(node)->row = row;
		ewl_widget_show(row);

		if (colour)
			ewl_widget_state_set(row, "odd", EWL_STATE_PERSISTENT);
		else
			ewl_widget_state_set(row, "even", EWL_STATE_PERSISTENT);
		colour = (colour + 1) % 2;

		/* do the current branch */
		column = 0;
		ecore_list_goto_first(tree->columns);
		while((col = ecore_list_next(tree->columns)))
		{
			ewl_tree2_column_build(EWL_ROW(row), model, col->view,
							data, i, column, node);
			column ++;
		}

		/* check if this is an expansion point */
		col = ecore_list_goto_first(tree->columns);
		if (col && model->expansion.is && model->expansion.is(data, i))
		{
			int hidden = TRUE;
			Ewl_Model *tmp_model = NULL;
			void *tmp_data;

			if (!model->expansion.data)
			{
				DWARNING("In ewl_tree2_build_tree_rows, "
					"model expandable but without "
					"expansion_data_fetch cb.");
				DRETURN(DLEVEL_STABLE);
			}

			ewl_tree2_node_expandable_set(EWL_TREE2_NODE(node), data);

			if (model->expansion.data &&
					ewl_tree2_row_expanded_is(tree, data, i))
			{
				ewl_tree2_node_expand(EWL_TREE2_NODE(node));
				hidden = FALSE;
			}

			tmp_data = model->expansion.data(data, i);
			if (model->expansion.model)
				tmp_model = model->expansion.model(data, i);

			if (!tmp_model) tmp_model = model;

			ewl_tree2_build_tree_rows(tree, tmp_model, tmp_data, 
							colour, node, hidden);
		}
		else
			ewl_tree2_node_expandable_set(EWL_TREE2_NODE(node), NULL);

		i++;

		/*
		 * Finished the rows at this level? Jump back up a level.
		 */
		if (i >= row_count) 
			break;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_column_free(void *data)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = data;
	ewl_tree2_column_destroy(c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_header_changed(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Tree2 *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	tree = data;
	ewl_widget_configure(EWL_WIDGET(tree->rows));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_row_clicked(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Tree2 *tree;
	Ewl_Tree2_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = data;
	tree = EWL_TREE2(node->tree);
	if (tree->type != EWL_TREE_SELECTION_TYPE_ROW)
		DRETURN(DLEVEL_STABLE);

	ewl_mvc_handle_click(EWL_MVC(tree), ewl_mvc_model_get(EWL_MVC(node)),
				ewl_mvc_data_get(EWL_MVC(node)),
				node->row_num, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_cell_clicked(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Row *row;
	Ewl_Tree2 *tree;
	Ewl_Tree2_Node *node;
	int column;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	row = EWL_ROW(w->parent);
	node = EWL_TREE2_NODE(data);
	tree = EWL_TREE2(node->tree);
	if (tree->type != EWL_TREE_SELECTION_TYPE_CELL)
		DRETURN(DLEVEL_STABLE);

	column = ewl_container_child_index_get(EWL_CONTAINER(row), w);

	ewl_mvc_handle_click(EWL_MVC(node->tree),
			ewl_mvc_model_get(EWL_MVC(node)),
			ewl_mvc_data_get(EWL_MVC(node)),
			node->row_num, column);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_selected_change(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	ewl_mvc_highlight(mvc, EWL_CONTAINER(EWL_TREE2(mvc)->rows), 
						ewl_tree2_widget_at);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_tree2_widget_at(Ewl_MVC *mvc, void *data __UNUSED__, int row, int column)
{
	Ewl_Widget *r, *w;
	Ewl_Tree2 *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	tree = EWL_TREE2(mvc);
	r = ewl_container_child_get(EWL_CONTAINER(tree->rows), row);

	if (tree->type == EWL_TREE_SELECTION_TYPE_ROW)
		w = r;
	else
		w = ewl_container_child_get(EWL_CONTAINER(r), column);

	DRETURN_PTR(w, DLEVEL_STABLE);
}


/*
 * Ewl_Tree2_Column stuff
 */

/**
 * @return Returns a new Ewl_Tree2_Column
 * @brief Creates a new Ewl_Tree2_Column object
 */
Ewl_Tree2_Column *
ewl_tree2_column_new(void)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = NEW(Ewl_Tree2_Column, 1);

	DRETURN_PTR(c, DLEVEL_STABLE);
}

/**
 * @param c: The column to work with
 * @return Returns no value
 * @brief Destroys the given column 
 */
void
ewl_tree2_column_destroy(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->view = NULL;
	c->parent = NULL;
	c->sort = EWL_SORT_DIRECTION_NONE;

	FREE(c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The column to work with
 * @param v: The view to set
 * @return Returns no value
 * @brief Sets the given view @a v into the column @a c
 */
void
ewl_tree2_column_view_set(Ewl_Tree2_Column *c, Ewl_View *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("v", v);

	c->view = v;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Tree2_Column to work with
 * @return Returns the view set on the given column
 * @brief Retrieves the view for the given column
 */
Ewl_View *
ewl_tree2_column_view_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->view, DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Tree2_Column to work with
 * @param mvc: The parent to set
 * @return Returns no value
 * @brief Sets @a mvc as the parent of the column @a c
 */
void
ewl_tree2_column_mvc_set(Ewl_Tree2_Column *c, Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	c->parent = mvc;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Tree2_Column to work with
 * @return Returns the MVC parent or NULL if none set
 * @brief Retrives the MVC parent
 */
Ewl_MVC *
ewl_tree2_column_mvc_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->parent, DLEVEL_STABLE);
}

/**
 * @param c: The column to work with
 * @param sortable: The sortable flag to set
 * @return Returns no value
 * @brief Sets the sortable flag of column @a c to @a sortable
 */
void
ewl_tree2_column_sortable_set(Ewl_Tree2_Column *c, unsigned int sortable)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->sortable = !!sortable;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The column to work with
 * @return Returns TRUE if the column is sortable, FALSE otherwise
 * @brief Retrieves the sort flag for the column @a c
 */
unsigned int
ewl_tree2_column_sortable_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, FALSE);

	DRETURN_INT(c->sortable, DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Tree2_Column to work with
 * @return Returns the parent tree for this column or NULL if none set
 * @brief Retrieves the parent tree for this column or NULL if none set
 */
Ewl_Tree2 *
ewl_tree2_column_tree_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->parent, DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Tree2_Column to work with
 * @param sort: The sort direction to set
 * @return Returns no value
 * @brief Sets the sort direction of the column to the given value
 */
void
ewl_tree2_column_sort_direction_set(Ewl_Tree2_Column *c, Ewl_Sort_Direction sort)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->sort = sort;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Tree2_Column to get the sort information from
 * @return Returns the current sort direction for the column or
 * EWL_SORT_DIRECTION_NONE if none set
 * @brief Retrieves the current sort information for the Ewl_Tree2_Column
 */
Ewl_Sort_Direction
ewl_tree2_column_sort_direction_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, EWL_SORT_DIRECTION_NONE);

	DRETURN_INT(c->sort, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_tree2_node_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Tree2_Node, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree2_node_init(EWL_TREE2_NODE(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

int
ewl_tree2_node_init(Ewl_Tree2_Node *node)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("node", node, FALSE);

	if (!ewl_mvc_init(EWL_MVC(node)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(node), EWL_TREE2_NODE_TYPE);
	ewl_widget_inherit(EWL_WIDGET(node), EWL_TREE2_NODE_TYPE);

	ewl_container_show_notify_set(EWL_CONTAINER(node),
				ewl_tree2_cb_node_child_show);
	ewl_container_hide_notify_set(EWL_CONTAINER(node),
				ewl_tree2_cb_node_child_hide);
	ewl_container_resize_notify_set(EWL_CONTAINER(node),
				ewl_tree2_cb_node_resize);
	ewl_container_add_notify_set(EWL_CONTAINER(node),
				ewl_tree2_cb_node_child_add);
	ewl_container_remove_notify_set(EWL_CONTAINER(node),
				ewl_tree2_cb_node_child_del);

	ewl_object_fill_policy_set(EWL_OBJECT(node), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);

	ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
					ewl_tree2_cb_node_configure, NULL);

	node->expanded = EWL_TREE_NODE_COLLAPSED;
	ewl_widget_focusable_set(EWL_WIDGET(node), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_tree2_node_expandable_set(Ewl_Tree2_Node *node, void *data)
{
	Ewl_Model *model;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("node", node);

	model = ewl_mvc_model_get(EWL_MVC(node));
	/* we only create the handle if it doesn't exist and the model has
	 * a function to query for expandability */
	if (!node->handle && model->expansion.is)
	{
		node->handle = ewl_expansion_new();
		ewl_object_fill_policy_set(EWL_OBJECT(node->handle),
						EWL_FLAG_FILL_NONE);
		ewl_object_alignment_set(EWL_OBJECT(node->handle),
						EWL_FLAG_ALIGN_TOP);
		ewl_container_child_prepend(EWL_CONTAINER(node), node->handle);
		ewl_widget_show(node->handle);
	}

	if (node->handle)
	{
		if (data)
		{
			ewl_callback_append(node->handle, EWL_CALLBACK_VALUE_CHANGED,
							ewl_tree2_cb_node_toggle, node);
			ewl_widget_enable(node->handle);
			ewl_expansion_expandable_set(EWL_EXPANSION(node->handle), TRUE);
		}
		else 
		{
			ewl_callback_del(node->handle, EWL_CALLBACK_VALUE_CHANGED,
							ewl_tree2_cb_node_toggle);
			ewl_widget_disable(node->handle);
			ewl_expansion_expandable_set(EWL_EXPANSION(node->handle), FALSE);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_tree2_node_expandable_get(Ewl_Tree2_Node *node)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("node", node, FALSE);

	DRETURN_INT((ewl_mvc_data_get(EWL_MVC(node)) ? TRUE : FALSE), DLEVEL_STABLE);
}

void
ewl_tree2_node_expand(Ewl_Tree2_Node *node)
{
	Ewl_Widget *child;
	Ecore_List *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (node->expanded == EWL_TREE_NODE_EXPANDED)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Queue the parent tree for configure, this handles the issue of
	 * redrawing the alternating colors on expand and doing it early
	 * avoids duplicate or long list walks for queueing child widgets.
	 */
	ewl_widget_configure(node->tree);

	tmp = ecore_list_new();

	ecore_dlist_goto_first(EWL_CONTAINER(node)->children);
	while ((child = ecore_dlist_next(EWL_CONTAINER(node)->children)))
	{
		if ((child != node->handle) && (child != node->row))
			ecore_list_append(tmp, child);
	}

	while ((child = ecore_list_remove_first(tmp)))
		ewl_widget_show(child);

	IF_FREE_LIST(tmp);

	ewl_tree2_row_expand(EWL_TREE2(node->tree),
			ewl_mvc_data_get(EWL_MVC(node)),
			node->row_num);

	node->expanded = EWL_TREE_NODE_EXPANDED;
	ewl_check_checked_set(EWL_CHECK(node->handle), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_node_collapse(Ewl_Tree2_Node *node)
{
	Ewl_Widget *child;
	Ecore_List *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (node->expanded == EWL_TREE_NODE_COLLAPSED)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Queue the parent tree for configure, this handles the issue of
	 * redrawing the alternating colors on expand and doing it early
	 * avoids duplicate or long list walks for queueing child widgets.
	 */
	ewl_widget_configure(node->tree);

	tmp = ecore_list_new();

	ecore_dlist_goto_first(EWL_CONTAINER(node)->children);
	while ((child = ecore_dlist_next(EWL_CONTAINER(node)->children)))
	{
		if ((child != node->handle) && (child != node->row))
			ecore_list_append(tmp, child);
	}

	while ((child = ecore_list_remove_first(tmp)))
		ewl_widget_hide(child);

	IF_FREE_LIST(tmp);

	ewl_tree2_row_collapse(EWL_TREE2(node->tree),
			ewl_mvc_data_get(EWL_MVC(node)), node->row_num);

	node->expanded = EWL_TREE_NODE_COLLAPSED;
	ewl_check_checked_set(EWL_CHECK(node->handle), FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_tree2_node_expanded_is(Ewl_Tree2_Node *node)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("node", node, FALSE);

	DRETURN_INT(((node->expanded == EWL_TREE_NODE_EXPANDED) ? TRUE : FALSE), 
								DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_configure(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Tree2_Node *node;
	Ewl_Container *c;
	Ewl_Object *child;
	int x, y, hw = 0;

	DENTER_FUNCTION(DLEVEL_STABLE); 
	DCHECK_PARAM_PTR("w", w);	
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	
	node = EWL_TREE2_NODE(w);
	if (!node->tree)
		DRETURN(DLEVEL_STABLE);
	
	c = EWL_CONTAINER(w);
	if (!c->children)
		DRETURN(DLEVEL_STABLE);

	ecore_dlist_goto_first(c->children);
	x = CURRENT_X(w);
	y = CURRENT_Y(w);

	if (node->handle) 
	{
		ewl_object_geometry_request(EWL_OBJECT(node->handle),
				CURRENT_X(w), CURRENT_Y(w), CURRENT_W(w),
				CURRENT_H(w));
		hw = ewl_object_current_w_get(EWL_OBJECT(node->handle));
		x += hw;
	}

	/*
	 * All subsequent children are lower nodes and rows.
	 */
	while ((child = ecore_dlist_next(c->children))) 
	{
		if (VISIBLE(child) && EWL_WIDGET(child) != node->handle) 
		{
			ewl_object_geometry_request(child, x, y, CURRENT_W(w) - hw,
						    ewl_object_preferred_h_get(child));
			y += ewl_object_current_h_get(child);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_toggle(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, 
							void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_tree2_node_expandable_get(EWL_TREE2_NODE(data)))
	{
		if (ewl_tree2_node_expanded_is(EWL_TREE2_NODE(data)))
			ewl_tree2_node_collapse(EWL_TREE2_NODE(data));
		else
			ewl_tree2_node_expand(EWL_TREE2_NODE(data));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_child_show(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
	Ewl_Tree2_Node *node;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	node = EWL_TREE2_NODE(c);
	if (node->handle && node->expanded) {
		ewl_container_sum_prefer(c, EWL_ORIENTATION_VERTICAL);
		if (REALIZED(node->handle) && VISIBLE(node->handle))
			ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
					PREFERRED_H(c) - 
					ewl_object_preferred_h_get(EWL_OBJECT(node->handle)));
	}
	else 
		ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
					   ewl_object_preferred_h_get(EWL_OBJECT(node->row)));
		
	ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);
	if (node->handle && REALIZED(node->handle) && VISIBLE(node->handle))
		ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) +
			ewl_object_preferred_w_get(EWL_OBJECT(node->handle)));

	if (!node->expanded && node->handle)
		ewl_widget_hide(node->handle);

	ewl_widget_configure(node->tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	int width;
	Ewl_Tree2_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	node = EWL_TREE2_NODE(c);
	if (w == node->handle)
		DRETURN(DLEVEL_STABLE);

	if (ecore_dlist_nodes(c->children) < 3)
	{
		if (node->handle && VISIBLE(node->handle))
			ewl_widget_hide(node->handle);
	}

	ewl_object_preferred_inner_h_set(EWL_OBJECT(c), 
		PREFERRED_H(c) - ewl_object_preferred_h_get(EWL_OBJECT(w)));

	width = ewl_object_preferred_w_get(EWL_OBJECT(w));
	if (PREFERRED_W(c) >= width)
		ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_resize(Ewl_Container *c, Ewl_Widget *w, int size __UNUSED__,
						     Ewl_Orientation o __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_tree2_cb_node_child_show(c, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_child_add(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
	Ewl_Tree2_Node *node;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	node = EWL_TREE2_NODE(c);
	
	if (ecore_list_nodes(c->children) > 2)
	{
		/* XXX what do we do if !node->handle? */
		if (node->handle && HIDDEN(node->handle))
			ewl_widget_show(node->handle);
	}
	else if (node->handle && VISIBLE(node->handle))
		ewl_widget_hide(node->handle);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_node_child_del(Ewl_Container *c, Ewl_Widget *w, int idx __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	ewl_tree2_cb_node_child_add(c, w);
}

static void
ewl_tree2_create_expansions_hash(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	tree->expansions = ecore_hash_new(NULL, NULL);
	ecore_hash_set_free_value(tree->expansions, 
			ECORE_FREE_CB(ecore_list_destroy));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


