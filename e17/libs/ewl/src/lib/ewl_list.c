#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and initializes a new Ewl_List widget
 */
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

/**
 * @param list: The list to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialises an Ewl_List widget to default values
 */
int
ewl_list_init(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, FALSE);

	if (!ewl_mvc_init(EWL_MVC(list)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(list), EWL_LIST_TYPE);
	ewl_widget_inherit(EWL_WIDGET(list), EWL_LIST_TYPE);

	ewl_callback_append(EWL_WIDGET(list), EWL_CALLBACK_CONFIGURE, 
						ewl_list_cb_configure, NULL);

	ewl_container_hide_notify_set(EWL_CONTAINER(list), ewl_list_cb_child_hide);
	ewl_container_add_notify_set(EWL_CONTAINER(list), ewl_list_cb_child_add);
	ewl_container_remove_notify_set(EWL_CONTAINER(list), ewl_list_cb_child_del);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param list: The list to work with
 * @param w: The widget to set selected
 * @return Returns no value
 * @brief Sets the selected widget in the list
 */
void
ewl_list_selected_widget_set(Ewl_List *list, Ewl_Widget *w)
{
	int idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_TYPE("list", list, EWL_LIST_TYPE);

	if (!w)
		idx = -1;
	else
		idx = ewl_container_child_index_get(EWL_CONTAINER(list), w);

	ewl_list_selected_index_set(list, idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param list: The list to work with
 * @return Returns the currently selected widget or NULL if none set
 * @brief Retrieves the currently selected widget or NULL if none set
 */
Ewl_Widget *
ewl_list_selected_widget_get(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, NULL);
	DCHECK_TYPE_RET("list", list, EWL_LIST_TYPE, NULL);

	if (list->selected == -1)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	DRETURN_PTR(ewl_container_child_get(EWL_CONTAINER(list), 
				list->selected), DLEVEL_STABLE);
}

/**
 * @param list: The list to work with
 * @param idx: The index of the widget to set selected
 * @return Returns no value
 * @brief Sets the selected widget based on index into the list
 */
void
ewl_list_selected_index_set(Ewl_List *list, int idx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_TYPE("list", list, EWL_LIST_TYPE);

	if (list->selected == idx)
		DRETURN(DLEVEL_STABLE);

	list->selected = idx;

	ewl_callback_call(EWL_WIDGET(list), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param list: The list to work with
 * @return Returns the index of the currently selected or -1 if none set
 * @brief Retrieves the currenlty selected widgets index or -1 if none set
 */
int
ewl_list_selected_index_get(Ewl_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, -1);
	DCHECK_TYPE_RET("list", list, EWL_LIST_TYPE, -1);

	DRETURN_INT(list->selected, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The list to be configured
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Configures the given list 
 */
void
ewl_list_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_List *list;
	Ewl_Model *model;
	Ewl_View *view;
	void *mvc_data;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	list = EWL_LIST(w);

	model = ewl_mvc_model_get(EWL_MVC(list));
	view = ewl_mvc_view_get(EWL_MVC(list));
	mvc_data = ewl_mvc_data_get(EWL_MVC(list));

	/* if either the list isn't dirty or some of the MVC controls have
	 * not been set on the list just leave this up to the box to handle */
	if ((!ewl_mvc_dirty_get(EWL_MVC(list))) 
			|| !model || !view || !mvc_data)
		DRETURN(DLEVEL_STABLE);

	/* create all the widgets and pack into the container */
	ewl_container_reset(EWL_CONTAINER(list));
	for (i = 0; i < model->count(mvc_data); i++)
	{
		Ewl_Widget *o;

		o = view->construct();
		view->assign(o, model->fetch(mvc_data, i, 0));
		ewl_widget_show(o);

		ewl_container_child_append(EWL_CONTAINER(list), o);
	}

	/* XXX mark the selected widget here */

	ewl_mvc_dirty_set(EWL_MVC(list), FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget that was added
 * @return Returns no value
 * @brief Adds the needed callbacks to the widget
 */
void
ewl_list_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_callback_append(w, EWL_CALLBACK_CLICKED, 
				ewl_list_cb_item_clicked, c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param idx: The index of the deleted widget
 * @return Returns no value
 * @brief Removes the selected status from the widget
 */
void
ewl_list_cb_child_del(Ewl_Container *c, Ewl_Widget *w __UNUSED__, int idx)
{
	Ewl_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	list = EWL_LIST(c);
	if (list->selected == idx)
		ewl_list_selected_index_set(list, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget that was hidden
 * @return Returns no value
 * @brief Handles the hiding of a widget in the list 
 */
void
ewl_list_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_List *list;
	Ewl_Widget *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	list = EWL_LIST(c);
	sel = ewl_list_selected_widget_get(list);
	if (sel == w)
		ewl_list_selected_index_set(list, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget that was clicked
 * @param ev: The event data
 * @param data: The list widget
 * @return Returns no value
 * @brief Sets the clicked widget as selected
 */
void
ewl_list_cb_item_clicked(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("data", data, EWL_LIST_TYPE);

	list = data;
	ewl_list_selected_widget_set(list, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

