/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_mvc.h"
#include "ewl_highlight.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/* XXX There is a bit of behaviour that maybe considered a bug in here. If
 * the user has a list of say 4 items. The user clicks on item 2 then
 * control clicks on item one, then shift clicks on item 4. Item 2 will now
 * be in the selected list twice. A rm _should_ remove both instances, but
 * this could be confusing for the user. 
 *
 * The only solution is to do a selected_is for each index that is added to
 * the selected list, and for each range, check if that range intersects
 * anything else in the selected list. This could be slow and painful.
 *
 * Leaving it with the same item possibly in the list multiple times for
 * now.
 */

static void ewl_mvc_highlight_do(Ewl_MVC *mvc, Ewl_Container *c, 
				Ewl_Selection *sel, Ewl_Widget *w);
static void ewl_mvc_selected_change_notify(Ewl_MVC *mvc);
static void ewl_mvc_selected_rm_item(Ewl_MVC *mvc, Ewl_Selection *sel, 
					unsigned int row, unsigned int column);
static void ewl_mvc_cb_sel_free(void *data);

static void ewl_mvc_cb_highlight_destroy(Ewl_Widget *w, void *ev, void *data);

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

	/* new data, clear out the old selection list */
	ewl_mvc_selected_clear(mvc);

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
 * @brief Sets the dirty status of the MVC widget @a mvc to the @a dirty state
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
		IF_FREE_LIST(mvc->selected);
	}
	else if (!mvc->selected)
	{
		mvc->selected = ecore_list_new();
		ecore_list_set_free_cb(mvc->selected, ewl_mvc_cb_sel_free);
	}

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
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);	

	while ((sel = ecore_list_remove_first(mvc->selected)))
		ewl_mvc_cb_sel_free(sel);

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

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);

	while ((sel = ecore_list_remove_first(mvc->selected)))
		ewl_mvc_cb_sel_free(sel);

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

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN_PTR(NULL, DLEVEL_STABLE);	

	DRETURN_PTR(mvc->selected, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to set the list into
 * @param model: The model to use for this data. If NULL the model from the
 * MVC will be used 
 * @param data: The parent data containing the index selection
 * @param srow: The start row
 * @param scolumn:  The start column
 * @param erow: The end row
 * @param ecolumn: The end column
 * @return Returns no value
 * @brief Sets the given range, inclusive, as selected in the mvc
 */
void
ewl_mvc_selected_range_add(Ewl_MVC *mvc, Ewl_Model *model, void *data, 
				unsigned int srow, unsigned int scolumn,
				unsigned int erow, unsigned int ecolumn)
{
	Ewl_Selection *sel;
	Ewl_Model *mod;
	unsigned int tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);	

	if (model) mod = model;
	else mod = ewl_mvc_model_get(mvc);

	/* make sure the start comes before the end */
	if (erow < srow)
	{
		tmp = erow;
		erow = srow;
		srow = tmp;
	}

	if (ecolumn < scolumn)
	{
		tmp = ecolumn;
		ecolumn = scolumn;
		scolumn = tmp;
	}

	if (mvc->selection_mode == EWL_SELECTION_MODE_SINGLE)
		sel = ewl_mvc_selection_index_new(mod, data, srow, scolumn);
	else
		sel = ewl_mvc_selection_range_new(mod, data, srow, scolumn, 
							erow, ecolumn);

	ecore_list_append(mvc->selected, sel);
	ewl_mvc_selected_change_notify(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param model: The model to work with the data. If NULL the model from the
 * MVC will be used
 * @param data: The parent data containing the index selection
 * @param row: The row to set
 * @param column: The column to set
 * @return Returns no value
 * @brief Sets the given index as selected
 */
void
ewl_mvc_selected_set(Ewl_MVC *mvc, Ewl_Model *model, void *data, 
				unsigned int row, unsigned int column)
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);	

	while ((sel = ecore_list_remove_first(mvc->selected)))
		ewl_mvc_cb_sel_free(sel);

	ewl_mvc_selected_add(mvc, model, data, row, column);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param model: The model to work with. If NULL the model from the MVC will
 * be used
 * @param data: The parent data containing the index selection
 * @param row: The row to add
 * @param column: The column to add
 * @return Returns no value
 * @brief Adds the given index to the selected list
 */
void
ewl_mvc_selected_add(Ewl_MVC *mvc, Ewl_Model *model, void *data, 
			unsigned int row, unsigned int column)
{
	Ewl_Selection *si;
	Ewl_Model *mod;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);

	if (model) mod = model;
	else mod = ewl_mvc_model_get(mvc);

	si = ewl_mvc_selection_index_new(mod, data, row, column);
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

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ecore_list_goto_last(mvc->selected);
	sel = ecore_list_current(mvc->selected);
	if (!sel) DRETURN_PTR(NULL, DLEVEL_STABLE);

	ret = NEW(Ewl_Selection_Idx, 1);
	ret->sel.type = EWL_SELECTION_TYPE_INDEX;
	ret->sel.model = sel->model;
	ret->sel.data = sel->data;
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
 * @param data: The parent data containing the index selection
 * @param row: The row to remove
 * @param column: The column to remove
 * @return Returns no value
 * @brief Removes the given index from the list of selected indices
 */
void
ewl_mvc_selected_rm(Ewl_MVC *mvc, void *data __UNUSED__, unsigned int row, 
			unsigned int column)
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN(DLEVEL_STABLE);	

	/* We walk the entire list. The reason for this is that you can have
	 * the same cell in the list multiple times. This can happen if
	 * they've single selected something, then did a multiselection over
	 * top of it again. */
	ecore_list_goto_first(mvc->selected);
	while ((sel = ecore_list_current(mvc->selected)))
	{
		if (sel->type == EWL_SELECTION_TYPE_INDEX)
		{
			Ewl_Selection_Idx *si;

			si = EWL_SELECTION_IDX(sel);
			if ((si->row == row) && (si->column == column))
			{
				ewl_mvc_selected_rm_item(mvc, sel, row, column);
				continue;
			}
		}
		else
		{
			Ewl_Selection_Range *si;

			si = EWL_SELECTION_RANGE(sel);
			if ((si->start.row <= row) 
					&& (si->end.row >= row)
					&& (si->start.column <= column) 
					&& (si->end.column >= column))
			{
				ewl_mvc_selected_rm_item(mvc, sel, row, column);
				continue;
			}
		}
		ecore_list_next(mvc->selected);
	}
	ewl_mvc_selected_change_notify(mvc);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to work with
 * @return Returns the number of items selected in the MVC
 * @brief Retrives the number of items selected in the widget
 */
unsigned int
ewl_mvc_selected_count_get(Ewl_MVC *mvc)
{
	unsigned int count = 0;
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, 0);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, 0);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN_INT(0, DLEVEL_STABLE);	

	/* make sure we only return 1 or 0 for the single select case */
	if (mvc->selection_mode == EWL_SELECTION_MODE_SINGLE)
	{
		if (ecore_list_nodes(mvc->selected))
			DRETURN_INT(1, DLEVEL_STABLE);

		DRETURN_INT(0, DLEVEL_STABLE);
	}

	ecore_list_goto_first(mvc->selected);
	while ((sel = ecore_list_next(mvc->selected)))
	{
		if (sel->type == EWL_SELECTION_TYPE_INDEX)
			count ++;
		else if (sel->type == EWL_SELECTION_TYPE_RANGE)
		{
			Ewl_Selection_Range *r;
			unsigned int rows = 0, columns = 0;

			r = EWL_SELECTION_RANGE(sel);
			rows = (r->end.row - r->start.row) + 1;
			columns = (r->end.column - r->start.column) + 1;

			count += (rows * columns);
		}
	}

	DRETURN_INT(count, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param data: UNUSED
 * @param row: The row to check for
 * @param column: The column to check for
 * @return Returns TRUE if the index is selected, FALSE otherwise
 * @brief Checks if the given index is selected or not.
 */
unsigned int
ewl_mvc_selected_is(Ewl_MVC *mvc, void *data __UNUSED__, unsigned int row, 
			unsigned int column)
{
	Ewl_Selection *sel;
	unsigned int ret = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mvc", mvc, FALSE);
	DCHECK_TYPE_RET("mvc", mvc, EWL_MVC_TYPE, FALSE);

	if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
		DRETURN_INT(FALSE, DLEVEL_STABLE);	

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
			Ewl_Selection_Range *si;

			si = EWL_SELECTION_RANGE(sel);
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
 * @param model: The model to work with this data
 * @param data: The parent data containing the index selection
 * @param row: The row to create the index selection for
 * @param column: The column to create the index for
 * @return Returns a new Ewl_Selection_Idx based on the @a row and @a column
 * @brief Creates a new index selection based on given values
 */
Ewl_Selection *
ewl_mvc_selection_index_new(Ewl_Model *model, void *data, unsigned int row, 
				unsigned int column)
{
	Ewl_Selection_Idx *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sel = NEW(Ewl_Selection_Idx, 1);
	sel->sel.model = model;
	sel->sel.type = EWL_SELECTION_TYPE_INDEX;
	sel->sel.data = data;
	sel->row = row;
	sel->column = column;

	DRETURN_PTR(sel, DLEVEL_STABLE);
}

/**
 * @param model: The model to work with this data
 * @param data: The data that we're working with
 * @param srow: The start row
 * @param scolumn: The start column
 * @param erow: The end row
 * @param ecolumn: The end column
 * @return Returns a new Ewl_Selection_Range based on given values
 * @brief Creates a new range selection based on given values
 */
Ewl_Selection *
ewl_mvc_selection_range_new(Ewl_Model *model, void *data, unsigned int srow, 
				unsigned int scolumn, unsigned int erow, 
				unsigned int ecolumn)
{
	Ewl_Selection_Range *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sel = NEW(Ewl_Selection_Range, 1);
	sel->sel.model = model;
	sel->sel.type = EWL_SELECTION_TYPE_RANGE;
	sel->sel.data = data;
	sel->start.row = srow;
	sel->start.column = scolumn;
	sel->end.row = erow;
	sel->end.column = ecolumn;

	DRETURN_PTR(sel, DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The mvc to work with
 * @param model: A model to use. If NULL the MVC model will be used
 * @param data: The data the model was working with
 * @param row: The row to add
 * @param column: The column to add
 * @return Returns no value
 * @brief Handles the click of the given cell
 */
void
ewl_mvc_handle_click(Ewl_MVC *mvc, Ewl_Model *model, void *data, 
			unsigned int row, unsigned int column)
{
	unsigned int modifiers;
	int multi_select = FALSE;
	Ewl_Model *mod;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (ewl_mvc_selection_mode_get(mvc) == EWL_SELECTION_MODE_MULTI)
		multi_select = TRUE;

	if (model) mod = model;
	else mod = ewl_mvc_model_get(mvc);

	modifiers = ewl_ev_modifiers_get();
	if (multi_select && (modifiers & EWL_KEY_MODIFIER_SHIFT))
	{
		/* is this the first click? */
		if (ewl_mvc_selected_count_get(mvc) > 0)
		{
			Ewl_Selection *sel;
			void *sdata;
			unsigned int srow, scolumn;
			Ewl_Model *smod;

			/* A shift will add the current position into a 
			 * range with the last selected item. If the 
			 * last selected is a range, it will take the 
			 * start position */
			sel = ecore_list_goto_last(mvc->selected);
			if (sel->type == EWL_SELECTION_TYPE_INDEX)
			{
				Ewl_Selection_Idx *idx;

				idx = EWL_SELECTION_IDX(sel);
				sdata = sel->data;
				srow = idx->row;
				scolumn = idx->column;
				smod = EWL_SELECTION(idx)->model;

				if (sel->highlight)
				{
					ewl_widget_destroy(sel->highlight);
					sel->highlight = NULL;
				}
			}
			else
			{
				Ewl_Selection_Range *idx;
				unsigned int i, k;

				idx = EWL_SELECTION_RANGE(sel);
				sdata = sel->data;
				srow = idx->start.row;
				scolumn = idx->start.column;
				smod = EWL_SELECTION(idx)->model;

				if (sel->highlight)
				{
					Ewl_Widget *w;

					while ((w = ecore_list_remove_first(
								sel->highlight)))
						ewl_widget_destroy(w);
				}

				/* XXX this is not good. We probably want to
				 * find a better way to determine duplicates
				 * then what this is doing.
				 * 
				 * determine if any of the range's widgets 
				 * are in the list already. if so we need 
				 * to remove them from the range. 
				 * Do a selected_rm on all the duplicate 
				 * points until we have no duplicates */
				for (i = srow; i <= row; i++)
				{
					for (k = scolumn; k <= column; k++)
					{
						if (ewl_mvc_selected_is(mvc, data, i, k))
							ewl_mvc_selected_rm(mvc, 
									data, i, k);
					}
				}

			}
			ecore_list_remove(mvc->selected);

			ewl_mvc_selected_range_add(mvc, smod, data, srow, scolumn,
							row, column);
		}
		else
			ewl_mvc_selected_set(mvc, mod, data, row, column);
	}
	else if (multi_select && (modifiers & EWL_KEY_MODIFIER_CTRL))
	{
		if (ewl_mvc_selected_is(mvc, data, row, column))
			ewl_mvc_selected_rm(mvc, data, row, column);
		else
			ewl_mvc_selected_add(mvc, mod, data, row, column);
	}
	else
		ewl_mvc_selected_set(mvc, mod, data, row, column);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The Ewl_MVC widget to highlight
 * @param c: The Ewl_Container to put the highlight widgets into
 * @param widget: The callback to get the widget for a given index
 * @return Returns no value
 * @brief This will run through the list of selected widgets and create a
 * highlight widget for each if needed.
 */
void
ewl_mvc_highlight(Ewl_MVC *mvc, Ewl_Container *c,
	Ewl_Widget *(*widget)(Ewl_MVC *mvc, void *data, unsigned int row, 
					unsigned int column))
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_PARAM_PTR("widget", widget);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	if (!mvc->selected || !REALIZED(mvc)) 
		DRETURN(DLEVEL_STABLE);

	ecore_list_goto_first(mvc->selected);
	while ((sel = ecore_list_next(mvc->selected)))
	{
		Ewl_Widget *w;

		/* if it's already highlighted we can skip it */
		if (sel->highlight) continue;

		if (sel->type == EWL_SELECTION_TYPE_INDEX)
		{
			Ewl_Selection_Idx *idx;

			idx = EWL_SELECTION_IDX(sel);
			w = widget(mvc, sel->data, idx->row, idx->column);
			if (w) ewl_mvc_highlight_do(mvc, c, sel, w);
		}
		else
		{
			unsigned int i, k;
			Ewl_Selection_Range *idx;

			idx = EWL_SELECTION_RANGE(sel);
			for (i = idx->start.row; i <= idx->end.row; i++)
			{
				for (k = idx->start.column; 
						k <= idx->end.column; k++)
				{
					w = widget(mvc, sel->data, i, k);
					if (w) ewl_mvc_highlight_do(mvc, c,
								sel, w);
				}
			}
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
	IF_FREE_LIST(mvc->selected);

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

/* This will remove @a sel from the mvc widget @a mvc. Then, if @a sel is a
 * range selection it will remove @a row/@a column from the given selection
 * and add up to 4 new ranges into the @a mvc widget.
 * 
 * The range remove works like this:
 *   - first allocate as much space off the top to the first range.
 *     - this will go from the top left corner, to the selected (row - 1)
 *       and far right edge.
 *   - second, from the deletion row, far left to bottom row and (deletion column - 1) 
 *     make this the second range
 *   - third, from the deletoin row, (deletion column + 1) to the bottom
 *     right corner this is the third range
 *   - fourth, from deletion row + 1, deletion column to bottom row,
 *     deletion column, this is the fourth range
 * 
 * If a range would be only one item, we make it an Ewl_Selection_Index as
 * needed. Steps can be skipped if they would result in a zero item range.
 */
static void
ewl_mvc_selected_rm_item(Ewl_MVC *mvc, Ewl_Selection *sel, unsigned int row, 
				unsigned int column)
{
	Ewl_Selection_Range *si;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mvc", mvc);
	DCHECK_PARAM_PTR("sel", sel);
	DCHECK_TYPE("mvc", mvc, EWL_MVC_TYPE);

	ecore_list_goto(mvc->selected, sel);
	ecore_list_remove(mvc->selected);

	/* done if this is an index */
	if (sel->type != EWL_SELECTION_TYPE_RANGE)
	{
		if (sel->highlight)
		{
			ewl_widget_destroy(sel->highlight);
			sel->highlight = NULL;
		}

		DRETURN(DLEVEL_STABLE);
	}

	/* Clear out the highlights. 
	 * 
	 * XXX Might want to make this smarter and move the highlight widgets 
	 * into the correct range arrays as needed 
	 */
	if (sel->highlight)
	{
		Ewl_Widget *w;

		while ((w = ecore_list_remove_first(sel->highlight)))
			ewl_widget_destroy(w);
	}

	si = EWL_SELECTION_RANGE(sel);

	/* find top cells */
	if (row > si->start.row)
	{
		Ewl_Selection *n;
		unsigned int erow;

		erow = (row - 1);

		/* one item left in the grouping */
		if ((((si->start.row - erow) + 1) * 
				((si->start.column - si->end.column) + 1)) == 1)
			n = ewl_mvc_selection_index_new(sel->model, sel->data,
							si->start.row,
							si->start.column);

		else
			n = ewl_mvc_selection_range_new(sel->model, sel->data,
							si->start.row,
							si->start.column,
							erow, si->end.column);
		
		ecore_list_append(mvc->selected, n);
	}

	/* find left cells */
	if (column > si->start.column)
	{
		Ewl_Selection *n;
		unsigned int ecolumn;

		ecolumn = (column - 1);
		if ((((si->end.row - row) + 1) * 
				((si->start.column - ecolumn) + 1)) == 1)
			n = ewl_mvc_selection_index_new(sel->model, sel->data, 
							row, si->start.column);

		else
			n = ewl_mvc_selection_range_new(sel->model, sel->data, 
							row, si->start.column,
							si->end.row, ecolumn);

		ecore_list_append(mvc->selected, n);
	}

	/* find right cells */
	if (column < si->end.column)
	{
		Ewl_Selection *n;
		unsigned int scolumn;

		scolumn = column + 1;
		if ((((si->end.row - row) + 1) * 
				((scolumn - si->end.column) + 1)) == 1)
			n = ewl_mvc_selection_index_new(sel->model, sel->data, 
							row, si->end.column);

		else
			n = ewl_mvc_selection_range_new(sel->model, sel->data, 
							row, scolumn,
							si->end.row,
							si->end.column);

		ecore_list_append(mvc->selected, n);
	}

	/* find bottom cells */
	if (row < si->end.row)
	{
		Ewl_Selection *n;
		unsigned int srow;

		srow = row + 1;
		if ((((srow - si->end.row) + 1) * 
				((column - column) + 1)) == 1)
			n = ewl_mvc_selection_index_new(sel->model, sel->data, 
							si->end.row, column);

		else
			n = ewl_mvc_selection_range_new(sel->model, sel->data, 
							srow, column,
							si->end.row, column);

		ecore_list_append(mvc->selected, n);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_highlight_do(Ewl_MVC *mvc __UNUSED__, Ewl_Container *c, 
				Ewl_Selection *sel, Ewl_Widget *w)
{
	Ewl_Widget *h;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("sel", sel);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	h = ewl_highlight_new();
	ewl_highlight_follow_set(EWL_HIGHLIGHT(h), w);
	ewl_container_child_append(EWL_CONTAINER(c), h);
	ewl_callback_prepend(h, EWL_CALLBACK_DESTROY, 
			ewl_mvc_cb_highlight_destroy, sel);
	ewl_widget_show(h);

	if (sel->type == EWL_SELECTION_TYPE_INDEX)
		sel->highlight = h;
	else
	{
		if (!sel->highlight)
			sel->highlight = ecore_list_new();

		ecore_list_append(sel->highlight, h);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_cb_sel_free(void *data)
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	sel = data;
	if (sel->highlight)
	{
		if (sel->type == EWL_SELECTION_TYPE_INDEX)
			ewl_widget_destroy(sel->highlight);
		else
		{
			Ewl_Widget *w;

			while ((w = ecore_list_remove_first(sel->highlight)))
				ewl_widget_destroy(w);

			IF_FREE_LIST(sel->highlight);
		}
		sel->highlight = NULL;
	}
	FREE(data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_cb_highlight_destroy(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Selection *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sel = data;
	if (sel->type == EWL_SELECTION_TYPE_INDEX)
		sel->highlight = NULL;
	
	else
	{
		Ewl_Widget *cur;

		ecore_list_goto(sel->highlight, w);
		cur = ecore_list_current(sel->highlight);
		if (cur == w) ecore_list_remove(sel->highlight);
	}
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



