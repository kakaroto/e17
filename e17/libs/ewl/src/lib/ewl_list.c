#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

Ewl_Widget *
ewl_list_new(void)
{
	Ewl_Widget *list = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	list = NEW(Ewl_List, 1);
	if (!list)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_list_init(EWL_LIST(list)))
	{
		ewl_widget_destroy(list);
		list = NULL;
	}

	DRETURN_PTR(list, DLEVEL_STABLE);
}

int
ewl_list_init(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, FALSE);

	if (!ewl_box_init(EWL_BOX(list)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(list), EWL_LIST_TYPE);
	ewl_widget_inherit(EWL_WIDGET(list), EWL_LIST_TYPE);

	ewl_box_orientation_set(EWL_BOX(list), EWL_ORIENTATION_VERTICAL);

	ewl_callback_append(EWL_WIDGET(list), EWL_CALLBACK_CONFIGURE, 
						ewl_list_cb_configure, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_list_model_set(Ewl_List *list, Ewl_Model *model)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_TYPE("list", list, EWL_LIST_TYPE);

	list->model = model;
	ewl_list_dirty_set(list, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Model *
ewl_list_model_get(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, NULL);
	DCHECK_TYPE_RET("list", list, EWL_LIST_TYPE, NULL);

	DRETURN_PTR(list->model, DLEVEL_STABLE);
}

void
ewl_list_view_set(Ewl_List *list, Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_TYPE("list", list, EWL_LIST_TYPE);

	list->view = view;
	ewl_list_dirty_set(list, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_View *
ewl_list_view_get(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, NULL);
	DCHECK_TYPE_RET("list", list, EWL_LIST_TYPE, NULL);

	DRETURN_PTR(list->view, DLEVEL_STABLE);
}

void
ewl_list_data_set(Ewl_List *list, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_TYPE("list", list, EWL_LIST_TYPE);
	
	list->data = data;
	ewl_list_dirty_set(list, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void *
ewl_list_data_get(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, NULL);
	DCHECK_TYPE_RET("list", list, EWL_LIST_TYPE, NULL);

	DRETURN_PTR(list->data, DLEVEL_STABLE);
}

void
ewl_list_dirty_set(Ewl_List *list, unsigned int dirty)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_TYPE("list", list, EWL_LIST_TYPE);

	list->dirty = dirty;
	ewl_widget_configure(EWL_WIDGET(list));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_list_dirty_get(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, 0);
	DCHECK_TYPE_RET("list", list, EWL_LIST_TYPE, 0);

	DRETURN_INT(list->dirty, DLEVEL_STABLE);
}

void
ewl_list_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_List *list;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	list = EWL_LIST(w);

	/* if either the list isn't dirty or some of the MVC controls have
	 * not been set on the list just leave this up to the box to handle */
	if ((!ewl_list_dirty_get(list)) || !list->model || !list->view || !list->data)
		DRETURN(DLEVEL_STABLE);

	/* create all the widgets and pack into the container */
	ewl_container_reset(EWL_CONTAINER(list));
	for (i = 0; i < list->model->count(list->data); i++)
	{
		Ewl_Widget *o;

		o = list->view->construct();
		list->view->assign(o, list->model->fetch(list->data, i, 0));
		ewl_widget_show(o);

		ewl_container_child_append(EWL_CONTAINER(list), o);
	}

	ewl_list_dirty_set(list, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

