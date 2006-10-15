#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

static void ewl_mvc_selected_change_notify(Ewl_MVC *mvc);

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

	ewl_mvc_selection_mode_set(mvc, EWL_SELECTION_MODE_SINGLE);

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

	/* let the inheriting widget know that the data has changed */
	if (mvc->cb.selected_change)
		mvc->cb.selected_change(mvc);

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
 * @param mode: The selection mode to set
 * @return Returns no value
 * @brief Sets the selection capabilities of the mvc widget
 */
void
ewl_mvc_selection_mode_set(Ewl_MVC *mvc, Ewl_Selection_Mode mode)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	mvc->selection_mode = mode;
	if (mode == EWL_SELECTION_MODE_NONE)
	{
		if (mvc->selected)
			ecore_list_destroy(mvc->selected);
		mvc->selected = NULL;
	}
	else if (!mvc->selected)
		mvc->selected = ecore_list_new();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to use
 * @return Returns the selection mode of the mvc widget
 * @brief Retrieves the selection mode of the widget
 */
Ewl_Selection_Mode
ewl_mvc_selection_mode_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, EWL_SELECTION_MODE_NONE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, EWL_SELECTION_MODE_NONE);

	DRETURN_INT(mvc->selection_mode, DLEVEL_STABLE);
}

/**
 * @param mvc: The mvc to clear
 * @return Returns no value
 * @brief clears the selection list 
 */
void
ewl_mvc_selected_clear(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);
	
	ecore_list_clear(mvc->selected);
	ewl_mvc_selected_change_notify(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param list: The list of items to set selected.
 * @return Returns no value
 * @brief Sets the list of items to select. This will remove any items it
 * needs from the list.
 */
void
ewl_mvc_selected_list_set(Ewl_MVC *mvc, Ecore_List *list)
{
	Ewl_Selection *sel;
		
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	/* make sure we're selecting and received items to select */
	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);

	ecore_list_clear(mvc->selected);

	if (!list || (ecore_list_nodes(list) == 0))
		DRETURN(DLEVEL_STABLE);

	sel = ecore_list_remove_first(list);
	ecore_list_append(mvc->selected, sel);

	if (mvc->selection_mode == EWL_SELECTION_MODE_MULTI)
	{
		while ((sel = ecore_list_remove_first(list)))
			ecore_list_append(mvc->selected, sel);
	}

	ewl_mvc_selected_change_notify(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to get the list from
 * @return Returns the list of selected indices
 * @brief Retrieves the list of selected indicies. DO NOT remove or change
 * items in this list.
 */
Ecore_List *
ewl_mvc_selected_list_get(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	DRETURN_PTR(mvc->selected, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to set the list into
 * @param srow: The start row
 * @param scolumn:  The start column
 * @param erow: The end row
 * @param ecolumn: The end column
 * @return Returns no value
 * @brief Sets the given range, inclusive, as selected in the mvc
 */
void
ewl_mvc_selected_range_add(Ewl_MVC *mvc, int srow, int scolumn,
					 int erow, int ecolumn)
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_SINGLE)
	{
		Ewl_Selection_Idx *si;

		si = NEW(Ewl_Selection_Idx, 1);
		si->sel.type = EWL_SELECTION_TYPE_INDEX;
		si->row = srow;
		si->column = scolumn;

		sel = EWL_SELECTION(si);
	}
	else
	{
		Ewl_Selection_Range *si;

		si = NEW(Ewl_Selection_Range, 1);
		si->sel.type = EWL_SELECTION_TYPE_RANGE;
		si->start.row = srow;
		si->start.column = scolumn;
		si->end.row = erow;
		si->end.column = ecolumn;

		sel = EWL_SELECTION(si);
	}

	ecore_list_append(mvc->selected, sel);
	ewl_mvc_selected_change_notify(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param row: The row to set
 * @param column: The column to set
 * @return Returns no value
 * @brief Sets the given index as selected
 */
void
ewl_mvc_selected_set(Ewl_MVC *mvc, int row, int column)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	ecore_list_clear(mvc->selected);
	ewl_mvc_selected_add(mvc, row, column);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param row: The row to add
 * @param column: The column to add
 * @return Returns no value
 * @brief Adds the given index to the selected list
 */
void
ewl_mvc_selected_add(Ewl_MVC *mvc, int row, int column)
{
	Ewl_Selection_Idx *si;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	si = NEW(Ewl_Selection_Idx, 1);
	si->sel.type = EWL_SELECTION_TYPE_INDEX;
	si->row = row;
	si->column = column;

	ecore_list_append(mvc->selected, si);
	ewl_mvc_selected_change_notify(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to get the data from
 * @return Returns the last selected item. Return must be free'd
 * @brief Retrieves the last selected item. Return must be free'd.
 */
Ewl_Selection_Idx *
ewl_mvc_selected_get(Ewl_MVC *mvc)
{
	Ewl_Selection *sel;
	Ewl_Selection_Idx *ret;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, NULL);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, NULL);

	ecore_list_goto_last(mvc->selected);
	sel = ecore_list_current(mvc->selected);
	if (!sel) DRETURN_PTR(NULL, DLEVEL_STABLE);

	ret = NEW(Ewl_Selection_Idx, 1);
	ret->sel.type = EWL_SELECTION_TYPE_INDEX;

	if (sel->type == EWL_SELECTION_TYPE_INDEX)
	{
		Ewl_Selection_Idx *si;

		si = EWL_SELECTION_IDX(sel);
		ret->row = si->row;
		ret->column = si->column;
	}
	else
	{
		Ewl_Selection_Range *si;

		si = EWL_SELECTION_RANGE(sel);
		ret->row = si->start.row;
		ret->column = si->start.column;
	}

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param int row: The row to remove
 * @param int colum: The column to remove
 * @return Returns no value
 * @brief Removes the given index from the list of selected indices
 */
void
ewl_mvc_selected_rm(Ewl_MVC *mvc, int row, int column)
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	/* XXX should this check for the same selected area bein in the list
	 * twice? What if the user does a box select on the tree, then
	 * another, larger, box select? */
	ecore_list_goto_first(mvc->selected);
	while ((sel = ecore_list_current(mvc->selected)))
	{
		if (sel->type == EWL_SELECTION_TYPE_INDEX)
		{
			Ewl_Selection_Idx *si;

			si = EWL_SELECTION_IDX(sel);
			if ((si->row == row) && (si->column == column))
			{
				ecore_list_remove(mvc->selected);
				break;
			}
		}
		else
		{
			Ewl_Selection_Range *si;
			int tmp;

			si = EWL_SELECTION_RANGE(sel);
				
			/* verify the range has the top/left most
			 * cell first */
			if (si->end.row < si->start.row)
			{
				tmp = si->end.row;
				si->end.row = si->start.row;
				si->start.row = tmp;
			}
	
			if (si->end.column < si->start.column)
			{
				tmp = si->end.column;
				si->end.column = si->start.column;
				si->start.column = tmp;
			}

			if ((si->start.row <= row) 
					&& (si->end.row >= row)
					&& (si->start.column <= column) 
					&& (si->end.column >= column))
			{
				ecore_list_remove(mvc->selected);

				DWARNING("Can't rm from range's yet\n");
				/* find top cells */
				/* find left cells */
				/* find right cells */
				/* find bottom cells */

				break;
			}
		}

		ecore_list_next(mvc->selected);
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

	DRETURN_INT(ecore_list_nodes(mvc->selected), DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param idx: The index to check for
 * @return Returns TRUE if the index is selected, FALSE otherwise
 * @brief Checks if the given index is selected or not.
 */
unsigned int
ewl_mvc_is_selected(Ewl_MVC *mvc, int row, int column)
{
	Ewl_Selection *sel;
	int ret = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, FALSE);

	ecore_list_goto_first(mvc->selected);
	while ((sel = ecore_list_next(mvc->selected)))
	{
		if (sel->type == EWL_SELECTION_TYPE_INDEX)
		{
			Ewl_Selection_Idx *si;

			si = EWL_SELECTION_IDX(sel);
			if ((si->row == row) && (si->column == column))
			{
				ret = TRUE;
				break;
			}
		}
		else
		{
			int tmp;
			Ewl_Selection_Range *si;

			si = EWL_SELECTION_RANGE(sel);
				
			/* verify the range has the top/left most
			 * cell first */
			if (si->end.row < si->start.row)
			{
				tmp = si->end.row;
				si->end.row = si->start.row;
				si->start.row = tmp;
			}
	
			if (si->end.column < si->start.column)
			{
				tmp = si->end.column;
				si->end.column = si->start.column;
				si->start.column = tmp;
			}

			if ((si->start.row <= row) 
					&& (si->end.row >= row)
					&& (si->start.column <= column) 
					&& (si->end.column >= column))
			{
				ret = TRUE;
				break;
			}
		}
	}

	DRETURN_INT(ret, DLEVEL_STABLE);
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
	if (mvc->selected && (ecore_list_nodes(mvc->selected) > 0))
		cb(mvc);

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
	if (mvc->selected)
		ecore_list_destroy(mvc->selected);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_selected_change_notify(Ewl_MVC *mvc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	/* notify any inheriting widgets */
	if (mvc->cb.selected_change)
		mvc->cb.selected_change(mvc);

	/* notify the app */
	ewl_callback_call(EWL_WIDGET(mvc), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


