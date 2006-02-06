#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_tree2_cb_column_free(void *data);

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

	if (!ewl_container_init(EWL_CONTAINER(tree)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(tree), EWL_TREE2_TYPE);
	ewl_widget_inherit(EWL_WIDGET(tree), EWL_TREE2_TYPE);

	tree->columns = ecore_list_new();
	ecore_list_set_free_cb(tree->columns, ewl_tree2_cb_column_free);

	tree->mode = EWL_TREE_MODE_NONE;

{
Ewl_Widget *l;
l = ewl_label_new();
ewl_label_text_set(EWL_LABEL(l), "TEST");
ewl_container_child_append(EWL_CONTAINER(tree), l);
ewl_widget_show(l);
}

	tree->header = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(tree), tree->header);
	ewl_widget_appearance_set(EWL_WIDGET(tree->header), "tree_header");
	ewl_object_fill_policy_set(EWL_OBJECT(tree->header), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(tree->header);

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
 * @param tree: The Ewl_Tree to set the data into
 * @param data: The data to set into the tree
 * @return Returns no value.
 */
void
ewl_tree2_data_set(Ewl_Tree2 *tree, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	tree->data = data;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to get the data from
 * @return Returns the data currently set into the tree or NULL on failure
 */
void *
ewl_tree2_data_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, NULL);

	DRETURN_PTR(tree->data, DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to append the column too
 * @param model: The model to use for this column
 * @param view: The view to use for this column
 * @return Returns no value.
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

	ecore_list_append(tree->columns, c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to prepend the column too
 * @param model: The model to use for this column
 * @param view: The view to use for this column
 * @return Returns no value.
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

	ecore_list_prepend(tree->columns, c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to insert the column into
 * @param model: The model to use for this column
 * @param view: The view to use for this column
 * @param idx: The index to insert into 
 * @return Returns no value.
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

	ecore_list_goto_index(tree->columns, idx);
	ecore_list_insert(tree->columns, c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to remove the column from
 * @param idx: The column index to remove
 * @return Returns no value
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to toggle the header visibility
 * @param visible: The visiblity to set the tree to (TRUE == on, FALSE == off)
 * @return Returns no value
 */
void
ewl_tree2_headers_visible_set(Ewl_Tree2 *tree, unsigned char visible)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	tree->headers_visible = !!visible;

	if (!tree->headers_visible)
		ewl_widget_hide(tree->header);
	else
		ewl_widget_show(tree->header);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the header visiblity from
 * @return Returns the current header visiblity of the tree
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
 * @param tree: The tree to get the selected cells from
 * @return Returns an Ecore_List of cells selected in the tree
 */
Ecore_List *
ewl_tree2_selected_cells_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, FALSE);

	DRETURN_PTR(tree->selected, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to clear the selected cells from
 * @return Returns no value.
 */
void
ewl_tree2_selected_cells_clear(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	if (tree->mode == EWL_TREE_MODE_NONE)
		DRETURN(DLEVEL_STABLE);

	ecore_list_clear(tree->selected);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the mode from
 * @return Returns the current Ewl_Tree_Mode of the tree
 */
Ewl_Tree_Mode 
ewl_tree2_mode_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, EWL_TREE_MODE_NONE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, EWL_TREE_MODE_NONE);

	DRETURN_INT(tree->mode, DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to set the mode into
 * @param mode: The Ewl_Tree_Mode to set into the tree
 * @return Returns no value.
 */
void 
ewl_tree2_mode_set(Ewl_Tree2 *tree, Ewl_Tree_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	if (tree->mode == mode)
		DRETURN(DLEVEL_STABLE);

	tree->mode = mode;
 
	/* if the mode is none then we don't care about the selected list */
	if (tree->mode == EWL_TREE_MODE_NONE)
	{
		if (tree->selected)
			ecore_list_destroy(tree->selected);
	}
	else
	{
		if (!tree->selected)
			tree->selected = ecore_list_new();
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the fixed row flag into
 * @param fixed: The fixed row flag to set into the tree
 * @return Returns no value.
 */
void
ewl_tree2_fixed_rows_set(Ewl_Tree2 *tree, unsigned int fixed)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_TYPE("tree", tree, EWL_TREE2_TYPE);

	tree->fixed = fixed;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the fixed row flag from
 * @return Returns the current fixed row flag of the tree
 */
unsigned int
ewl_tree2_fixed_rows_get(Ewl_Tree2 *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, FALSE);
	DCHECK_TYPE_RET("tree", tree, EWL_TREE2_TYPE, FALSE);

	DRETURN_INT(tree->fixed, DLEVEL_STABLE);
}

void
ewl_tree2_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Tree2 *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TREE2(w);

	ecore_list_destroy(t->columns);
	if (t->selected) ecore_list_destroy(t->selected);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Tree2 *tree;
	Ewl_Tree2_Column *col;
	int column = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_TREE2_TYPE);

	tree = EWL_TREE2(w);

	/* setup the headers */
	ewl_container_reset(EWL_CONTAINER(tree->header));
	ecore_list_goto_first(tree->columns);
	while ((col = ecore_list_next(tree->columns)))
	{
		Ewl_Widget *h;

		h = col->model->header_fetch(tree->data, column);
		ewl_container_child_append(EWL_CONTAINER(tree->header), h);

printf("%s\n", ewl_label_text_get(EWL_LABEL(h)));

		column ++;
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

/*
 * Ewl_Tree2_Column stuff
 */
Ewl_Tree2_Column *
ewl_tree2_column_new(void)
{
	Ewl_Tree2_Column *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = NEW(Ewl_Tree2_Column, 1);

	DRETURN_PTR(c, DLEVEL_STABLE);
}

void
ewl_tree2_column_destroy(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->model = NULL;
	c->view = NULL;
	FREE(c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree2_column_model_set(Ewl_Tree2_Column *c, Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("m", m);

	c->model = m;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Model * 
ewl_tree2_column_model_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->model, DLEVEL_STABLE);
}

void
ewl_tree2_column_view_set(Ewl_Tree2_Column *c, Ewl_View *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("v", v);

	c->view = v;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_View *
ewl_tree2_column_view_get(Ewl_Tree2_Column *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->view, DLEVEL_STABLE);
}


