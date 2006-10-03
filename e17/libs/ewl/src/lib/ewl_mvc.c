#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/**
 * @param mvc: The MVC to initialize
 * @return Returns TRUE on success or FALSE if unsuccessful
 * @brief Initializes an MVC widget ot default values
 */
int
ewl_mvc_init(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);

	if (!ewl_box_init(EWL_BOX(mvc)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(mvc), EWL_MVC_TYPE);
	ewl_box_orientation_set(EWL_BOX(mvc), EWL_ORIENTATION_VERTICAL);

	ewl_callback_append(EWL_WIDGET(mvc), EWL_CALLBACK_DESTROY,
					ewl_mvc_cb_destroy, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param view: The view to set
 * @return Returns no value
 * @brief Sets the given view onto the MVC
 */
void
ewl_mvc_view_set(Ewl_MVC *mvc, Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->view == view)
		DRETURN(DLEVEL_STABLE);

	mvc->view = view;
	if (mvc->cb.view_change)
		mvc->cb.view_change(mvc);

	ewl_mvc_dirty_set(mvc, TRUE);
		
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the current view set on the MVC
 * @brief Retrives the current view set on the MVC
 */
Ewl_View *
ewl_mvc_view_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->view, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param cb: The callback to set
 * @return Returns no value
 * @brief This callback will be called whenever the ewl_mvc_view_set routine is
 * called to notify the inheriting widget that the view has changed
 */
void
ewl_mvc_view_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc))
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->cb.view_change = cb;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param model: The model to set
 * @return Returns no value
 * @brief Sets the given model into the tree
 */
void
ewl_mvc_model_set(Ewl_MVC *mvc, Ewl_Model *model)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_PARAM_PTR("model", model);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->model == model)
		DRETURN(DLEVEL_STABLE);

	mvc->model = model;
	ewl_mvc_dirty_set(mvc, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the current model set into the MVC widget
 * @brief Retrieves the model set into the MVC widget
 */
Ewl_Model * 
ewl_mvc_model_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->model, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param data: The data to set on the MVC
 * @return Returns no value
 * @brief Sets the given data @a data into the MVC widget @a mvc
 */
void
ewl_mvc_data_set(Ewl_MVC *mvc, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->data = data;
	ewl_mvc_dirty_set(mvc, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the data set onto the MVC widget
 * @brief Retrieves the data set into the MVC widget
 */
void *
ewl_mvc_data_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->data, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to work with
 * @param dirty: The dirty status to set
 * @return Returns no value.
 * @brief Sets the dirty status of the MVC widget @a mvc to the @dirty state
 */
void
ewl_mvc_dirty_set(Ewl_MVC *mvc, unsigned int dirty)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->dirty == dirty)
		DRETURN(DLEVEL_STABLE);

	mvc->dirty = !!dirty;
	ewl_widget_configure(EWL_WIDGET(mvc));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget use
 * @return Returns the dirty status of the MVC widget
 * @brief Retrieves the dirty status of the MVC widget
 */
unsigned int
ewl_mvc_dirty_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, FALSE);

	DRETURN_INT(mvc->dirty, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to use
 * @param multi: Is this widget multiselect capable
 * @return Returns no value
 * @brief Sets the multiselect capabilities of the mvc widget
 */
void
ewl_mvc_multiselect_set(Ewl_MVC *mvc, unsigned int multi)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->multiselect = !!multi;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to use
 * @return Returns the multiselect setting of the mvc widget
 * @brief Retrieves the multiselect setting of the widget
 */
unsigned int
ewl_mvc_multiselect_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, FALSE);

	DRETURN_INT(mvc->multiselect, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param list: The list of indicies to set selected, list is -1 terminated
 * @return Returns no value
 * @brief Sets the list of indices selected 
 */
void
ewl_mvc_selected_list_set(Ewl_MVC *mvc, int *list)
{
	int i, size;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (!list)
	{
		mvc->selected.count = 0;
		mvc->selected.items = realloc(mvc->selected.items, 2 * (sizeof(int)));
		mvc->selected.items[0] = -1;
	}
	else if (mvc->multiselect)
	{
		mvc->selected.count = 0;
		for (i = 0; list[i] != -1; i++)
			mvc->selected.count ++;

		size = (mvc->selected.count + 1) * (sizeof(int));
		mvc->selected.items = realloc(mvc->selected.items, size);
		mvc->selected.items[mvc->selected.count] = -1;
	
		if (mvc->selected.count > 0)
			memcpy(mvc->selected.items, list, size);
	}
	else
	{
		mvc->selected.count = 1;
		mvc->selected.items = realloc(mvc->selected.items, 2 * sizeof(int));
		mvc->selected.items[0] = list[0];
		mvc->selected.items[1] = -1;
	}

	if (mvc->cb.selected_change)
		mvc->cb.selected_change(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to get the list from
 * @return Returns the list of selected indices, list is -1 terminated.
 * @brief Retrieves the list of selected indicies
 */
const int *
ewl_mvc_selected_list_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->selected.items, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to set the list into
 * @param start: The range start
 * @param end: The range end
 * @return Returns no value
 * @brief Sets the given range, inclusive, as selected in the mvc
 */
void
ewl_mvc_selected_range_set(Ewl_MVC *mvc, int start, int end)
{
	int t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (start < 0) start = 0;
	if (end < 0) end = 0;

	/* make sure the start comes first */
	if (end < start)
	{
		t = start;
		start = end;
		end = t;
	}

	/* this isn't multiselect so make this one item, the start */
	if (!mvc->multiselect)
		end = start;

	t = (end - start) + 1;
	mvc->selected.count = t;
	mvc->selected.items = realloc(mvc->selected.items, (t + 1) * sizeof(int));
	mvc->selected.items[mvc->selected.count] = -1;

	for (t = 0; t < mvc->selected.count; t++)
		mvc->selected.items[t] = start + t;

	if (mvc->cb.selected_change)
		mvc->cb.selected_change(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param i: The index to set selected
 * @return Returns no value
 * @brief Sets the given index as selected
 */
void
ewl_mvc_selected_set(Ewl_MVC *mvc, int i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->selected.count = 1;
	mvc->selected.items = realloc(mvc->selected.items, 2 * sizeof(int));
	mvc->selected.items[0] = i;
	mvc->selected.items[1] = -1;

	if (mvc->cb.selected_change)
		mvc->cb.selected_change(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param i: The index to add to the selected list
 * @return Returns no value
 * @brief Adds the given index to the selected list
 */
void
ewl_mvc_selected_add(Ewl_MVC *mvc, int i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->selected.count ++;
	mvc->selected.items = realloc(mvc->selected.items, 
				(mvc->selected.count + 1) * sizeof(int));
	mvc->selected.items[mvc->selected.count - 1] = i;
	mvc->selected.items[mvc->selected.count] = -1;

	if (mvc->cb.selected_change)
		mvc->cb.selected_change(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to get the data from
 * @return Returns the last selected item
 * @brief Retrieves the last selected item
 */
int
ewl_mvc_selected_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, -1);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, -1);

	if (mvc->selected.count == 0)
		DRETURN_INT(-1, DLEVEL_STABLE);

	DRETURN_INT(mvc->selected.items[mvc->selected.count - 1], 
							DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param idx: The index to remove
 * @return Returns no value
 * @brief Removes the given index from the list of selected indices
 */
void
ewl_mvc_selected_rm(Ewl_MVC *mvc, int idx)
{
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	for (i = 0; i < mvc->selected.count; i++)
	{
		if (mvc->selected.items[i] == idx)
		{
			mvc->selected.count --;
			memmove((mvc->selected.items + i), 
					(mvc->selected.items + i + 1),
					((mvc->selected.count - i) * sizeof(int)));
			mvc->selected.items[mvc->selected.count] = -1;
			break;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to work with
 * @return Returns the number of items selected in the MVC
 * @brief Retrives the number of items selected in the widget
 */
int
ewl_mvc_selected_count_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, 0);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, 0);

	DRETURN_INT(mvc->selected.count, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param idx: The index to check for
 * @return Returns TRUE if the index is selected, FALSE otherwise
 * @brief Checks if the given index is selected or not.
 */
unsigned int
ewl_mvc_is_selected(Ewl_MVC *mvc, int idx)
{
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, FALSE);

	for (i = 0; i < mvc->selected.count; i++)
	{
		if (mvc->selected.items[i] == idx)
			DRETURN_INT(TRUE, DLEVEL_STABLE);
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The MVC to set the callback into
 * @param cb: The callback to set
 * @return Returns no value
 * @brief Sets the given callback into the MVC widget
 */
void
ewl_mvc_selected_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc))
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->cb.selected_change = cb;
	if (mvc->selected.count > 0)
		cb(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/** 
 * @internal
 * @param c: The container
 * @param w: UNUSED
 * @param idx: The index removed from
 * @return Returns no value
 * @brief Checks if the given widget index is in the selected array and
 * removes it
 */
void
ewl_mvc_cb_child_del(Ewl_Container *c, Ewl_Widget *w __UNUSED__, int idx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	if (ewl_mvc_is_selected(EWL_MVC(c), idx))
		ewl_mvc_selected_rm(EWL_MVC(c), idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget
 * @return Returns no value
 * @brief Checks if the widget is in the selected list and removes it
 */
void
ewl_mvc_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	int idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	idx = ewl_container_child_index_get(c, w);
	if (idx > -1 && ewl_mvc_is_selected(EWL_MVC(c), idx))
		ewl_mvc_selected_rm(EWL_MVC(c), idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The wiget to destroy
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Cleans up the given widget
 */
void
ewl_mvc_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_MVC *mvc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	mvc = EWL_MVC(w);
	if (mvc->selected.items)
		FREE(mvc->selected.items);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


