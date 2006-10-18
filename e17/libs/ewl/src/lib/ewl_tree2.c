#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

static void ewl_tree2_cb_view_change(Ewl_MVC *mvc);
static void ewl_tree2_build_tree(Ewl_Tree2 *tree);
static void ewl_tree2_cb_column_free(void *data);
static void ewl_tree2_cb_header_changed(Ewl_Widget *w, void *ev, 
							void *data);

static void ewl_tree2_cb_row_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree2_cb_cell_clicked(Ewl_Widget *w, void *ev, void *data);

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
 * @param model: The model to use for this column
 * @param view: The view to use for this column
 * @return Returns no value.
 * @brief Append a new column to the tree
 */
void
ewl_tree2_column_append(Ewl_Tree2 *tree, Ewl_Model *model, Ewl_View *view)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	c = ewl_tree2_column_new();
	if (!c)
	{
		DWARNING("Unable to create new tree column.\n");
		DRETURN(DLEVEL_STABLE);
	}

	ewl_tree2_column_model_set(c, model);
	ewl_tree2_column_view_set(c, view);
	ewl_tree2_column_mvc_set(c, EWL_MVC(tree));

	ecore_list_append(tree->columns, c);
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to prepend the column too
 * @param model: The model to use for this column
 * @param view: The view to use for this column
 * @return Returns no value.
 * @brief Prepend a new column to the tree
 */
void
ewl_tree2_column_prepend(Ewl_Tree2 *tree, Ewl_Model *model, Ewl_View *view)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	c = ewl_tree2_column_new();
	if (!c)
	{
		DWARNING("Unable to create new tree column.\n");
		DRETURN(DLEVEL_STABLE);
	}

	ewl_tree2_column_model_set(c, model);
	ewl_tree2_column_view_set(c, view);
	ewl_tree2_column_mvc_set(c, EWL_MVC(tree));

	ecore_list_prepend(tree->columns, c);
	ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to insert the column into
 * @param model: The model to use for this column
 * @param view: The view to use for this column
 * @param idx: The index to insert into 
 * @return Returns no value.
 * @brief Insert a new column into the tree
 */
void
ewl_tree2_column_insert(Ewl_Tree2 *tree, Ewl_Model *model, Ewl_View *view, 
							unsigned int idx)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	c = ewl_tree2_column_new();
	if (!c)
	{
		DWARNING("Unable to create new tree column.\n");
		DRETURN(DLEVEL_STABLE);
	}

	ewl_tree2_column_model_set(c, model);
	ewl_tree2_column_view_set(c, view);
	ewl_tree2_column_mvc_set(c, EWL_MVC(tree));

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
 * @param mode: The Ewl_Tree_Mode to set into the tree
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

	ecore_list_destroy(t->columns);

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
	int index = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	c = data;

	/* sanity check */
	if (!c->model || !c->model->sort)
	{
		DWARNING("In ewl_tree2_cb_column_sort without a sort cb.");
		DRETURN(DLEVEL_STABLE);
	}

	/* loop over the columns and reset the sort settings */
	ecore_list_goto_first(EWL_TREE2(c->parent)->columns);
	while ((col = ecore_list_next(EWL_TREE2(c->parent)->columns)))
	{
		/* skip the current column */
		if (col == c)
		{
			/* we're the index before the one we're now on */
			index = ecore_list_index(EWL_TREE2(c->parent)->columns) - 1;
			continue;
		}

		col->sort = EWL_SORT_DIRECTION_NONE;
	}

	/* update our sort direction and call the sort function, skipping
	 * over SORT_NONE */
	c->sort = ((c->sort + 1) % EWL_SORT_DIRECTION_MAX);
	if (!c->sort) c->sort ++;

	c->model->sort(ewl_mvc_data_get(c->parent), index, c->sort);
	ewl_mvc_dirty_set(c->parent, TRUE);

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

	t->rows = view->construct();
	ewl_tree2_view_tree2_set(EWL_TREE2_VIEW(t->rows), t);
	ewl_container_child_append(EWL_CONTAINER(t), t->rows);
	ewl_widget_show(t->rows);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_build_tree(Ewl_Tree2 *tree)
{
	Ewl_Tree2_Column *col;
	int column = 0, rows = 0, i;
	void *mvc_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	mvc_data = ewl_mvc_data_get(EWL_MVC(tree));

	/* setup the headers */
	ewl_container_reset(EWL_CONTAINER(tree->header));
	ecore_list_goto_first(tree->columns);
	while ((col = ecore_list_next(tree->columns)))
	{
		int r;
		char *theme_str;
		Ewl_Widget *h, *c;

		h = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(tree->header), h);
		ewl_widget_appearance_set(h, "header");
		ewl_widget_show(h);

		if (col->model->sort)
			ewl_callback_append(h, EWL_CALLBACK_CLICKED, 
						ewl_tree2_cb_column_sort, col);

		c = col->view->header_fetch(mvc_data, column);
		ewl_object_fill_policy_set(EWL_OBJECT(c), 
				EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_HFILL);
		ewl_container_child_append(EWL_CONTAINER(h), c);

		/* display the sort arrow if needed */
		if (col->model->sort)
		{
			c = ewl_button_new();
			ewl_container_child_append(EWL_CONTAINER(h), c);

			if (col->sort == EWL_SORT_DIRECTION_ASCENDING)
				theme_str = "ascending";
			else if (col->sort == EWL_SORT_DIRECTION_DESCENDING)
				theme_str = "descending";
			else
				theme_str = "blank";

			ewl_widget_appearance_set(c, theme_str);
			ewl_widget_internal_set(c, TRUE);
			ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_SHRINK);
			ewl_object_alignment_set(EWL_OBJECT(c), EWL_FLAG_ALIGN_RIGHT);
			ewl_widget_show(c);
		}

		r = col->model->count(mvc_data);
		if (r > rows) rows = r;

		column ++;
	}

	ewl_container_reset(EWL_CONTAINER(tree->rows));
	for (i = 0; i < rows; i++)
	{
		Ewl_Widget *row;

		row = ewl_row_new();
		ewl_row_header_set(EWL_ROW(row), EWL_ROW(tree->header));
		ewl_container_child_append(EWL_CONTAINER(tree->rows), row);
		ewl_attach_widget_association_set(row, tree);
		ewl_callback_append(row, EWL_CALLBACK_CLICKED,  
					ewl_tree2_cb_row_clicked, NULL);
		ewl_widget_show(row);

		if (i % 2)
			ewl_widget_state_set(row, "odd", EWL_STATE_PERSISTENT);
		else
			ewl_widget_state_set(row, "even", EWL_STATE_PERSISTENT);

		column = 0;
		ecore_list_goto_first(tree->columns);
		while((col = ecore_list_next(tree->columns)))
		{
			Ewl_Widget *cell;
			Ewl_Widget *child;
			void *val;

			cell = ewl_cell_new();
			ewl_object_fill_policy_set(EWL_OBJECT(cell),
						   EWL_FLAG_FILL_ALL);
			ewl_container_child_append(EWL_CONTAINER(row), cell);
			ewl_attach_widget_association_set(cell, row);
			ewl_callback_append(cell, EWL_CALLBACK_CLICKED,
						ewl_tree2_cb_cell_clicked, NULL);
			ewl_widget_show(cell);

			val = col->model->fetch(mvc_data, i, column);
			if (!val)
			{
				child = ewl_label_new();
				ewl_label_text_set(EWL_LABEL(child), " ");
			}
			else
			{
				child = col->view->construct();
				col->view->assign(child, val);
			}
			ewl_container_child_append(EWL_CONTAINER(cell), child);
			ewl_widget_show(child);

			column ++;
		}
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

void
ewl_tree2_cb_selected_change(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	/* XXX handle highlighting here */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	c->model = NULL;
	c->view = NULL;
	c->parent = NULL;
	c->sort = EWL_SORT_DIRECTION_NONE;

	FREE(c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The column to work with
 * @param m: The model to set
 * @return Returns no value
 * @brief Sets the given model @a m into the column @a c
 */
void
ewl_tree2_column_model_set(Ewl_Tree2_Column *c, Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("m", m);

	c->model = m;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The column to work with
 * @return Returns the model for the column
 * @brief Retrieves the model for the given column
 */
Ewl_Model * 
ewl_tree2_column_model_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->model, DLEVEL_STABLE);
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

static void
ewl_tree2_cb_row_clicked(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Tree2 *tree;
	int row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	tree = ewl_attach_widget_association_get(w);
	if (tree->type != EWL_TREE_SELECTION_TYPE_ROW)
		DRETURN(DLEVEL_STABLE);

	row = ewl_container_child_index_get(EWL_CONTAINER(tree->rows), w);
	ewl_mvc_handle_click(EWL_MVC(tree), row, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree2_cb_cell_clicked(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Row *row;
	Ewl_Tree2 *tree;
	int r, column;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	row = ewl_attach_widget_association_get(w);
	tree = ewl_attach_widget_association_get(row);
	if (tree->type != EWL_TREE_SELECTION_TYPE_CELL)
		DRETURN(DLEVEL_STABLE);

	r = ewl_container_child_index_get(EWL_CONTAINER(tree->rows), 
						EWL_WIDGET(row));
	column = ewl_container_child_index_get(EWL_CONTAINER(row), w);
	ewl_mvc_handle_click(EWL_MVC(tree), r, column);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

