#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_tree2.h"

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

	if (!ewl_tree2_init(EWL_TREE2(w))) {
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

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/*
 * MODEL CODE BEGINS HERE
 */

/**
 */
Ewl_Model *
ewl_model_new(void)
{
	Ewl_Model *model;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	model = NEW(Ewl_Model, 1);
	if (!ewl_model_init(model))
		FREE(model);

	DRETURN_PTR(model, DLEVEL_UNSTABLE);
}

/**
 */
int
ewl_model_init(Ewl_Model *model)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("model", model, FALSE);

	DRETURN_INT(TRUE, DLEVEL_UNSTABLE);
}

/**
 */
void
ewl_model_fetch_set(Ewl_Model *m, Ewl_Model_Fetch get)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("m", m);

	m->fetch = get;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 */
Ewl_Model_Fetch
ewl_model_fetch_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->fetch, DLEVEL_UNSTABLE);
}

/**
 */
void
ewl_model_subfetch_set(Ewl_Model *m, Ewl_Model_Fetch get)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("m", m);

	m->subfetch = get;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 */
Ewl_Model_Fetch
ewl_model_subfetch_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->subfetch, DLEVEL_UNSTABLE);
}

/**
 */
void
ewl_model_sort_set(Ewl_Model *m, Ewl_Model_Sort sort)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("m", m);

	m->sort = sort;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 */
Ewl_Model_Sort
ewl_model_sort_get(Ewl_Model *m)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	DRETURN_PTR(m->sort, DLEVEL_UNSTABLE);
}

/**
 */
void
ewl_model_count_set(Ewl_Model *m, Ewl_Model_Count count)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("m", m);

	m->count = count;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/*
 * VIEW CODE BEGINS HERE
 */

/**
 */
Ewl_View *
ewl_view_new(void)
{
	Ewl_View *view;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	view = NEW(Ewl_View, 1);
	if (!ewl_view_init(view))
		FREE(view);

	DRETURN_PTR(view, DLEVEL_UNSTABLE);
}

/**
 */
int
ewl_view_init(Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("view", view, FALSE);

	DRETURN_INT(TRUE, DLEVEL_UNSTABLE);
}

/**
 */
void
ewl_view_constructor_set(Ewl_View *v, Ewl_View_Constructor construct)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("v", v);

	v->construct = construct;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 */
Ewl_View_Constructor
ewl_view_constructor_get(Ewl_View *v)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("v", v, NULL);

	DRETURN_PTR(v->construct, DLEVEL_UNSTABLE);
}

/**
 */
void
ewl_view_assign_set(Ewl_View *v, Ewl_View_Assign assign)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("v", v);

	v->assign = assign;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 */
Ewl_View_Assign
ewl_view_assign_get(Ewl_View *v)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("v", v, NULL);

	DRETURN_PTR(v->assign, DLEVEL_UNSTABLE);
}
