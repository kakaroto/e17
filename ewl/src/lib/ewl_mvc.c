/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_mvc.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

unsigned int EWL_CALLBACK_MVC_CLICKED = 0;

static void ewl_mvc_selected_clear_private(Ewl_MVC *mvc, Ewl_Selection *set);
static unsigned int ewl_mvc_selected_goto(Ewl_MVC *mvc,
                        unsigned int row, unsigned int column);
static void ewl_mvc_selected_insert(Ewl_MVC *mvc, const Ewl_Model *model,
                        void *data, Ewl_Selection *sel,
                        unsigned int row, unsigned int column);
static void ewl_mvc_selected_range_split(Ewl_MVC *mvc,
                        Ewl_Selection_Range *range,
                        unsigned int row, unsigned int column);
static int ewl_mvc_selection_intersects(Ewl_Selection_Range *range,
                                                Ewl_Selection *sel);
static int ewl_mvc_selection_contained(Ewl_Selection_Range *a,
                                                Ewl_Selection_Range *b);
static int ewl_mvc_line_intersects(int astart, int aend, int bstart, int bend);
static void ewl_mvc_range_merge(Ecore_List *list, const Ewl_Model *model, void *data,
                        Ewl_Selection_Range *range, Ewl_Selection_Range *cur);
static Ewl_Selection *ewl_mvc_selection_make(const Ewl_Model *model, void *data,
                                        unsigned int top, unsigned int left,
                                        unsigned int bottom, unsigned int right);

static void ewl_mvc_selected_change_notify(Ewl_MVC *mvc);
static void ewl_mvc_highlight_do(Ewl_MVC *mvc, Ewl_Container *c,
                                Ewl_Selection *sel, Ewl_Widget *w);
static void ewl_mvc_cb_highlight_destroy(Ewl_Widget *w, void *ev, void *data);
static void ewl_mvc_cb_sel_free(void *data);
static void ewl_mvc_selection_free(Ewl_Selection *sel, unsigned int set_state);

/**
 * @param mvc: The MVC to initialize
 * @return Returns TRUE on success or FALSE if unsuccessful
 * @brief Initializes an MVC widget ot default values
 */
int
ewl_mvc_init(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, FALSE);

        if (!ewl_box_init(EWL_BOX(mvc)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(mvc), EWL_MVC_TYPE);
        ewl_box_orientation_set(EWL_BOX(mvc), EWL_ORIENTATION_VERTICAL);

        ewl_callback_append(EWL_WIDGET(mvc), EWL_CALLBACK_DESTROY,
                                        ewl_mvc_cb_destroy, NULL);
        ewl_callback_append(EWL_WIDGET(mvc), EWL_CALLBACK_DESTROY,
                                        ewl_mvc_cb_data_unref, NULL);

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
ewl_mvc_view_set(Ewl_MVC *mvc, const Ewl_View *view)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_PARAM_PTR(view);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

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
const Ewl_View *
ewl_mvc_view_get(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

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
ewl_mvc_model_set(Ewl_MVC *mvc, const Ewl_Model *model)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_PARAM_PTR(model);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

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
const Ewl_Model *
ewl_mvc_model_get(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        mvc->data = data;

        /* new data, clear out the old selection list */
        if (mvc->model && mvc->view)
        {
		ewl_mvc_selected_clear(mvc);
		ewl_mvc_dirty_set(mvc, TRUE);
        }

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
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

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
        DCHECK_PARAM_PTR_RET(mvc, FALSE);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, FALSE);

        DRETURN_INT(mvc->dirty, DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @param data: The private data to set on the MVC
 * @return Returns no value
 * @brief Sets the given data @a data into the MVC widget @a mvc
 */
void
ewl_mvc_private_data_set(Ewl_MVC *mvc, void *data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        mvc->private_data = data;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to work with
 * @return Returns the data set onto the MVC widget
 * @brief Retrieves the data set into the MVC widget
 */
void *
ewl_mvc_private_data_get(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

        DRETURN_PTR(mvc->private_data, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == mode)
                DRETURN(DLEVEL_STABLE);

        switch (mvc->selection_mode)
        {
                case EWL_SELECTION_MODE_SINGLE:
                        ewl_callback_del(EWL_WIDGET(mvc),
                                        EWL_CALLBACK_MVC_CLICKED,
                                        ewl_mvc_cb_clicked_single);
                        break;
                case EWL_SELECTION_MODE_MULTI:
                        ewl_callback_del(EWL_WIDGET(mvc),
                                        EWL_CALLBACK_MVC_CLICKED,
                                        ewl_mvc_cb_clicked_multi);
                        break;
                default:
                        break;
        }
        
        switch (mode)
        {
                case EWL_SELECTION_MODE_SINGLE:
                        ewl_callback_append(EWL_WIDGET(mvc),
                                        EWL_CALLBACK_MVC_CLICKED,
                                        ewl_mvc_cb_clicked_single, NULL);
                        break;
                case EWL_SELECTION_MODE_MULTI:
                        ewl_callback_append(EWL_WIDGET(mvc),
                                        EWL_CALLBACK_MVC_CLICKED,
                                        ewl_mvc_cb_clicked_multi, NULL);
                        break;
                default:
                        break;
        }

        mvc->selection_mode = mode;
        if (mode == EWL_SELECTION_MODE_NONE)
        {
                IF_FREE_LIST(mvc->selected);
        }
        else if (!mvc->selected)
        {
                mvc->selected = ecore_list_new();
                ecore_list_free_cb_set(mvc->selected, ewl_mvc_cb_sel_free);
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
        DCHECK_PARAM_PTR_RET(mvc, EWL_SELECTION_MODE_NONE);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, EWL_SELECTION_MODE_NONE);

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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        while ((sel = ecore_list_first_remove(mvc->selected)))
                ewl_mvc_selection_free(sel, TRUE);

        ewl_mvc_selected_change_notify(mvc);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_selected_clear_private(Ewl_MVC *mvc, Ewl_Selection *set)
{
        Ewl_Selection *sel;
        Ewl_Selection_Idx *i1, *i2;
        Ewl_Selection_Range *r1;
        int idx;
        Ewl_Widget *w, *h;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        i1 = EWL_SELECTION_IDX(set);

        while ((sel = ecore_list_first_remove(mvc->selected)))
        {
                /* If index, deal with accordingly */
                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        i2 = EWL_SELECTION_IDX(sel);
                        if ((i2->row != i1->row) || (i2->column != i1->column))
                                ewl_mvc_selection_free(sel, TRUE);
                        else
                                ewl_mvc_selection_free(sel, FALSE);
                }
                /* If sel contains the current selection */
                else if (ewl_mvc_selection_intersects(EWL_SELECTION_RANGE(sel),
                                                                        set))
                {
                        r1 = EWL_SELECTION_RANGE(sel);
                        idx = (i1->row - r1->start.row) *
                                (r1->end.column - r1->start.column + 1) +
                                (i1->column - r1->start.column);
                        h = ecore_list_index_goto(sel->highlight, idx);

                        ecore_list_first_goto(sel->highlight);
                        while ((w = ecore_list_next(sel->highlight)))
                        {
                                if (w != h)
                                {
                                        ewl_widget_state_set(w, "deselect",
                                                        EWL_STATE_PERSISTENT);
                                }
                        }
                        ewl_mvc_selection_free(sel, FALSE);
                }
                else
                        ewl_mvc_selection_free(sel, TRUE);
        }

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
        unsigned int count = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        count = ewl_mvc_selected_count_get(mvc);
        ewl_mvc_selected_clear(mvc);

        if (!list || (ecore_list_count(list) == 0))
        {
                if (count > 0) ewl_mvc_selected_change_notify(mvc);
                DRETURN(DLEVEL_STABLE);
        }

        ewl_mvc_selected_insert(mvc, NULL, NULL,
                        ecore_list_first_remove(list), 0, 0);

        if (mvc->selection_mode == EWL_SELECTION_MODE_MULTI
                        || mvc->selection_mode == EWL_SELECTION_MODE_USER)
        {
                while ((sel = ecore_list_first_remove(list)))
                        ewl_mvc_selected_insert(mvc, NULL, NULL, sel, 0, 0);
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
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

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
ewl_mvc_selected_range_add(Ewl_MVC *mvc, const Ewl_Model *model, void *data,
                                unsigned int srow, unsigned int scolumn,
                                unsigned int erow, unsigned int ecolumn)
{
        Ewl_Selection *sel;
        const Ewl_Model *mod;
        unsigned int tmp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

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
                ewl_mvc_selected_insert(mvc, mod, data, NULL, srow, scolumn);
        else
        {
                sel = ewl_mvc_selection_range_new(mod, data, srow, scolumn,
                                                        erow, ecolumn);
                ewl_mvc_selected_insert(mvc, NULL, NULL, sel, 0, 0);
        }

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
ewl_mvc_selected_set(Ewl_MVC *mvc, const Ewl_Model *model, void *data,
                                unsigned int row, unsigned int column)
{
        Ewl_Selection *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        sel = ewl_mvc_selection_index_new(model, data, row, column);
        ewl_mvc_selected_clear_private(mvc, sel);
        FREE(sel);
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
ewl_mvc_selected_add(Ewl_MVC *mvc, const Ewl_Model *model, void *data,
                        unsigned int row, unsigned int column)
{
        const Ewl_Model *mod;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        if (model) mod = model;
        else mod = ewl_mvc_model_get(mvc);

        ewl_mvc_selected_insert(mvc, mod, data, NULL, row, column);
        ewl_mvc_selected_change_notify(mvc);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC to get the data from
 * @return Returns the last selected item. Return must be free'd
 * @brief Retrieves the last selected item. Return must be free'd.
 *
 * If there should not be any selection or an error occured (e.g. the selection
 * mode is set to EWL_SELECTION_MODE_NONE) it will return NULL.
 */
Ewl_Selection_Idx *
ewl_mvc_selected_get(Ewl_MVC *mvc)
{
        Ewl_Selection *sel;
        Ewl_Selection_Idx *ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ecore_list_last_goto(mvc->selected);
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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        if (ewl_mvc_selected_goto(mvc, row, column))
        {
                sel = ecore_list_current(mvc->selected);

                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        sel = ecore_list_remove(mvc->selected);
                        ewl_mvc_selection_free(sel, TRUE);
                }
                else
                        ewl_mvc_selected_range_split(mvc,
                                EWL_SELECTION_RANGE(sel), row, column);

                ewl_mvc_selected_change_notify(mvc);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mvc: The MVC widget to work with
 * @return Returns the number of items selected in the MVC
 * @brief Retrives the number of items selected in the widget
 */
/* XXX We might want to just store this to cut down the time
 * to calculate it */
unsigned int
ewl_mvc_selected_count_get(Ewl_MVC *mvc)
{
        unsigned int count = 0;
        Ewl_Selection *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, 0);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, 0);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN_INT(0, DLEVEL_STABLE);

        /* make sure we only return 1 or 0 for the single select case */
        if (mvc->selection_mode == EWL_SELECTION_MODE_SINGLE)
        {
                if (ecore_list_count(mvc->selected))
                        DRETURN_INT(1, DLEVEL_STABLE);

                DRETURN_INT(0, DLEVEL_STABLE);
        }

        ecore_list_first_goto(mvc->selected);
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

/* This will look through the list and see if the given row/column is in there.
 * If it is there it will return TRUE and leave the list pointing to the matching
 * node. Returns FALSE otherwise, list should be at either the end, or the item
 * past where this item would be (so the insertion point for a create)
 */
static unsigned int
ewl_mvc_selected_goto(Ewl_MVC *mvc, unsigned int row, unsigned int column)
{
        Ewl_Selection *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, FALSE);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, FALSE);

        ecore_list_first_goto(mvc->selected);
        while ((sel = ecore_list_current(mvc->selected)))
        {
                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        Ewl_Selection_Idx *idx;
                        idx = EWL_SELECTION_IDX(sel);
                        if ((idx->row == row) && (idx->column == column))
                                DRETURN_INT(TRUE, DLEVEL_STABLE);
                }
                else
                {
                        Ewl_Selection_Range *r;

                        r = EWL_SELECTION_RANGE(sel);

                        /* see if we match within the range */
                        if ((r->start.row <= row) && (r->start.column <= column)
                                        && (r->end.row >= row)
                                        && (r->end.column >= column))
                                DRETURN_INT(TRUE, DLEVEL_STABLE);
                }
                ecore_list_next(mvc->selected);
        }

        DRETURN_INT(FALSE, DLEVEL_STABLE);
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
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, FALSE);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, FALSE);

        if (mvc->selection_mode == EWL_SELECTION_MODE_NONE)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        DRETURN_INT(ewl_mvc_selected_goto(mvc, row, column), DLEVEL_STABLE);
}

static void
ewl_mvc_selected_insert(Ewl_MVC *mvc, const Ewl_Model *model, void *data,
                   Ewl_Selection *sel, unsigned int row, unsigned int column)
{
        Ewl_Selection_Range *range;
        Ewl_Selection *cur;
        Ecore_List *intersections;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);

        if (!sel)
                sel = EWL_SELECTION(ewl_mvc_selection_index_new(model,
                                                        data, row, column));

        /* if this is an index and the index is already selected
         * then we're done. Otherwise, just insert the item into the list
         * and be done with it. */
        if (sel->type == EWL_SELECTION_TYPE_INDEX)
        {
                Ewl_Selection_Idx *idx;

                idx = EWL_SELECTION_IDX(sel);
                if (ewl_mvc_selected_goto(mvc, idx->row, idx->column))
                {
                        ewl_mvc_selection_free(sel, FALSE);
                        DRETURN(DLEVEL_STABLE);
                }

                ecore_list_append(mvc->selected, sel);
                DRETURN(DLEVEL_STABLE);
        }

        /* We've got a range we're trying to insert from here onwards */

        /* we store the intersections away and handle them later as
         * we don't want to be fiddling the list while we walk it
         * (this process will add more items to the list that we
         * don't want to check again for intersections */
        intersections = ecore_list_new();
        ecore_list_free_cb_set(intersections, ECORE_FREE_CB(free));

        range = EWL_SELECTION_RANGE(sel);
        ecore_list_first_goto(mvc->selected);
        while ((cur = ecore_list_current(mvc->selected)))
        {
                /* ecore_list_remove updates the index by itself */
                if (ewl_mvc_selection_intersects(range, cur))
                {
                        ecore_list_remove(mvc->selected);

                        /* just free indexes as their covered by the
                         * range and don't need to be re-inserted */
                        if (cur->type == EWL_SELECTION_TYPE_INDEX)
                                ewl_mvc_selection_free(cur, FALSE);
                        else
                                ecore_list_append(intersections, cur);

                }
                else
                        ecore_list_next(mvc->selected);
        }

        /* if we intersect nothing just add ourselves to the list
         * and be done with it */
        if (ecore_list_count(intersections) == 0)
                ecore_list_insert(mvc->selected, range);
        else
        {
                Ewl_Selection_Range *ptr;

                while ((ptr = ecore_list_first_remove(intersections)))
                {
                        /* if range is contained inside current then
                         * this can be the only intersection. we add
                         * current to the list, destroy range and
                         * are done
                         *
                         * We can't do this inside
                         * ewl_mvc_range_merge() as we free range in
                         * this case and keep ptr. This is backwards
                         * to what's expected by _merge()
                         */
                        if (ewl_mvc_selection_contained(ptr, range))
                        {
                                ecore_list_append(mvc->selected, ptr);
                                ewl_mvc_selection_free(EWL_SELECTION(range),
                                                FALSE);
                                range = NULL;
                                break;
                        }
                        ewl_mvc_range_merge(mvc->selected, model, data, range, ptr);
                }
                if (range) ecore_list_append(mvc->selected, range);
        }
        ecore_list_destroy(intersections);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This will take range and cur and split cur up so there is no overlap
 * between the two ranges. The @a range will _not_ be changed by this. We
 * will append into the list as needed. @a cur maybe freed by this operation
 * if it is no longer needed */
static void
ewl_mvc_range_merge(Ecore_List *list, const Ewl_Model *model, void *data,
                        Ewl_Selection_Range *range, Ewl_Selection_Range *cur)
{
        Ewl_Selection *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(list);
        DCHECK_PARAM_PTR(range);
        DCHECK_PARAM_PTR(cur);

        /* if the new pointer is totaly in range
         * then delete the original one and keep the
         * range */
        if (ewl_mvc_selection_contained(range, cur))
        {
                ewl_mvc_selection_free(EWL_SELECTION(cur), FALSE);
                DRETURN(DLEVEL_STABLE);
        }

        /* see if this is a merge of the two along one of the sides */
        if (((range->start.row == cur->start.row)
                        && (range->end.row == cur->end.row))
                || ((range->start.column == cur->start.column)
                        && (range->end.column == cur->end.column)))
        {
                range->start.row = MIN(range->start.row, cur->start.row);
                range->start.column = MIN(range->start.column, cur->start.column);
                range->end.row = MAX(range->end.row, cur->end.row);
                range->end.column = MAX(range->end.column, cur->end.column);

                ewl_mvc_selection_free(EWL_SELECTION(cur), FALSE);
                DRETURN(DLEVEL_STABLE);
        }

        /* not merged and not overlapped we're going to need to split @a cur
         * apart in order for this to mesh together
         *
         * We're going to split @a cur into, at most, 4 parts
         *
         *  1
         * - - - - - ------- - - - -
         *  2        | R    |  4
         *           |      |
         *           -------- - - - -
         *           | 3
         *
         *           |
         */

        /* find everything above (case 1) */
        if (cur->start.row < range->start.row)
        {
                sel = ewl_mvc_selection_make(model, data, cur->start.row,
                                                cur->start.column,
                                                range->start.row - 1,
                                                cur->end.column);
                ecore_list_append(list, sel);
        }

        /* find everything left (case 2) */
        if (cur->start.column < range->start.column)
        {
                sel = ewl_mvc_selection_make(model, data,
                                                MAX(range->start.row, cur->start.row),
                                                cur->start.column,
                                                cur->end.row,
                                                range->start.column - 1);
                ecore_list_append(list, sel);
        }

        /* find everything below (case 3) */
        if (cur->end.row > range->end.row)
        {
                sel = ewl_mvc_selection_make(model, data, range->end.row + 1,
                                                MAX(range->start.column, cur->start.column),
                                                cur->end.row,
                                                cur->end.column);
                ecore_list_append(list, sel);
        }

        /* find everything left (case 4) */
        if (cur->end.column > range->end.column)
        {
                sel = ewl_mvc_selection_make(model, data,
                                                MAX(range->start.row, cur->start.row),
                                                range->end.column + 1,
                                                MIN(range->end.row, cur->end.row),
                                                cur->end.column);
                ecore_list_append(list, sel);
        }
        ewl_mvc_selection_free(EWL_SELECTION(cur), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Selection *
ewl_mvc_selection_make(const Ewl_Model *model, void *data, unsigned int top,
                                unsigned int left, unsigned int bottom,
                                unsigned int right)
{
        Ewl_Selection *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((top != bottom) || (left != right))
        {
                sel = EWL_SELECTION(ewl_mvc_selection_range_new(model,
                                        data, top, left, bottom, right));
        }
        else
                sel = EWL_SELECTION(ewl_mvc_selection_index_new(model,
                                                data, top, left));

        DRETURN_PTR(sel, DLEVEL_STABLE);
}

/* This determins if there there is an intersection point between @a range
 * and @a sel. Returns TRUE if they intersect, FALSE otherwise.
 */
static int
ewl_mvc_selection_intersects(Ewl_Selection_Range *range, Ewl_Selection *sel)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(range, FALSE);
        DCHECK_PARAM_PTR_RET(sel, FALSE);

        if (sel->type == EWL_SELECTION_TYPE_INDEX)
        {
                Ewl_Selection_Idx *idx;

                idx = EWL_SELECTION_IDX(sel);
                if ((range->start.row <= idx->row) && (range->end.row >= idx->row)
                                && (range->start.column <= idx->column)
                                && (range->end.column >= idx->column))
                {
                        DRETURN_INT(TRUE, DLEVEL_STABLE);
                }
        }
        else
        {
                Ewl_Selection_Range *cur;
                cur = EWL_SELECTION_RANGE(sel);

                /* is one range completely inside another */
                if ((ewl_mvc_selection_contained(range, cur))
                                || (ewl_mvc_selection_contained(cur, range)))
                        DRETURN_INT(TRUE, DLEVEL_STABLE);

                /* if the columns intersect and the rows intersect then the
                 * boxes intersect */
                if (ewl_mvc_line_intersects(cur->start.row, cur->end.row,
                                                range->start.row, range->end.row)
                                || ewl_mvc_line_intersects(range->start.row, range->end.row,
                                                cur->start.row, cur->end.row))
                {
                        if (ewl_mvc_line_intersects(cur->start.column, cur->end.column,
                                                range->start.column, range->end.column)
                                        || ewl_mvc_line_intersects(range->start.column,
                                                        range->end.column, cur->start.column,
                                                        cur->end.column))
                        {
                                DRETURN_INT(TRUE, DLEVEL_STABLE);
                        }
                }
        }

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/*
 * this checks the following:
 *    astart <= bstart <= aend
 * or astart <= bend <= aend
 * or bstart <= astart <= bend
 * or bstart <= aend <= bend
 *
 * Returns TRUE if any of the above match, FALSE otherwise
 */
static int
ewl_mvc_line_intersects(int astart, int aend, int bstart, int bend)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((((astart <= bstart) && (bstart <= aend))
                                || ((astart <= bend) && (bend <= aend)))
                        || (((bstart <= astart) && (astart <= bend))
                                || ((bstart <= aend) && (aend <= bend))))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/* checks if range @a b is contained completely within range @a a.
 * Returns TRUE if contained. FALSE otherwise
 */
static int
ewl_mvc_selection_contained(Ewl_Selection_Range *a, Ewl_Selection_Range *b)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(a, FALSE);
        DCHECK_PARAM_PTR_RET(b, FALSE);

        if ((a->start.column <= b->start.column)
                        && (b->start.column <= a->end.column)
                        && (a->start.column <= b->end.column)
                        && (b->end.column <= a->end.column)
                        && (a->start.row <= b->start.row)
                        && (b->start.row <= a->end.row)
                        && (a->start.row <= b->end.row)
                        && (b->end.row <= a->end.row))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/* split the range into, at most, 4 ranges. This will be done similar to the
 * merge for range intersections.
 *
 *  -----------------------------
 *  |   1                       |
 *  |                           |
 *  |- - - - - - - - - - - -  - |
 *  |             |X- - - - - - |  4 will be a single line
 *  |   2                       |
 *  |             |  3          |
 *  |                           |
 *  -----------------------------
 */
static void
ewl_mvc_selected_range_split(Ewl_MVC *mvc, Ewl_Selection_Range *range,
                                unsigned int row, unsigned int column)
{
        Ewl_Selection *sel;
        const Ewl_Model *model;
        void *data;
        int idx;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_PARAM_PTR(range);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        /* make life easier by removing the range */
        ecore_list_remove(mvc->selected);
        model = EWL_SELECTION(range)->model;
        data = EWL_SELECTION(range)->data;

        /* We need to unselect the widget being split around here */
        idx = (row - range->start.row) * (range->end.column - 
                        range->start.column + 1) +
                        (column - range->start.column);
        w = ecore_list_index_goto(EWL_SELECTION(range)->highlight, idx);
        ewl_widget_state_set(w, "deselect", EWL_STATE_PERSISTENT);

        /* we have something above, case 1 */
        if (range->start.row < row)
        {
                sel = ewl_mvc_selection_make(model, data, range->start.row,
                                                        range->start.column,
                                                        row - 1,
                                                        range->end.column);
                ecore_list_append(mvc->selected, sel);
        }

        /* something left, case 2 */
        if (range->start.column < column)
        {
                sel = ewl_mvc_selection_make(model, data, row,
                                                        range->start.column,
                                                        range->end.row,
                                                        column - 1);
                ecore_list_append(mvc->selected, sel);
        }

        /* something below, case 3 */
        if (range->end.row > row)
        {
                sel = ewl_mvc_selection_make(model, data, row + 1, column,
                                                        range->end.row,
                                                        range->end.column);
                ecore_list_append(mvc->selected, sel);
        }

        /* something right, case 4 */
        if (range->end.column > row)
        {
                sel = ewl_mvc_selection_make(model, data, row, column + 1,
                                                        row, range->end.column);
                ecore_list_append(mvc->selected, sel);
        }

        ewl_mvc_selection_free(EWL_SELECTION(range), FALSE);

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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_PARAM_PTR(widget);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        if (!mvc->selected || !REALIZED(mvc))
                DRETURN(DLEVEL_STABLE);

        ecore_list_first_goto(mvc->selected);
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
                        /* w can be NULL, for example when the selected
                         * row or widget is hidden or collapsed */
                        if (w)
                                ewl_mvc_highlight_do(mvc, c, sel, w);
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
                                        if (!w) continue;
                                        
                                        ewl_mvc_highlight_do(mvc, c, sel, w);
                                }
                        }
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_highlight_do(Ewl_MVC *mvc __UNUSED__, Ewl_Container *c,
                                Ewl_Selection *sel, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(sel);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_state_set(w, "selected", EWL_STATE_PERSISTENT);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
                        ewl_mvc_cb_highlight_destroy, sel);

        if (sel->type == EWL_SELECTION_TYPE_INDEX)
                sel->highlight = w;
        else
        {
                if (!sel->highlight)
                        sel->highlight = ecore_list_new();
                ecore_list_append(sel->highlight, w);
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
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        mvc->cb.selected_change = cb;
        if (mvc->selected && (ecore_list_count(mvc->selected) > 0))
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
        DCHECK_PARAM_PTR(w);

        mvc = EWL_MVC(w);
        IF_FREE_LIST(mvc->selected);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The wiget to destroy
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Cleans up the data of the given mvc
 */
void
ewl_mvc_cb_data_unref(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_MVC *mvc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        mvc = EWL_MVC(w);

        /* unref the data, we don't need it anylonger */
        if (mvc->data && mvc->model && mvc->model->unref)
        {
                mvc->model->unref(mvc->data);
                mvc->data = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The wiget to deal with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief to be written
 */
void
ewl_mvc_cb_clicked_multi(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_MVC_Clicked *e;
        Ewl_MVC *mvc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MVC_TYPE);

        e = ev;
        mvc = EWL_MVC(w);

        if (e->modifiers & EWL_KEY_MODIFIER_SHIFT)
        {
                /* is this the first click? */
                if (ewl_mvc_selected_count_get(mvc) > 0)
                {
                        Ewl_Selection *sel;
                        void *sdata;
                        unsigned int srow, scolumn;
                        const Ewl_Model *smod;

                        /* A shift will add the current position into a
                         * range with the last selected item. If the
                         * last selected is a range, it will take the
                         * start position */
                        sel = ecore_list_last_goto(mvc->selected);
                        if (sel->type == EWL_SELECTION_TYPE_INDEX)
                        {
                                Ewl_Selection_Idx *idx;

                                idx = EWL_SELECTION_IDX(sel);
                                smod = sel->model;
                                sdata = sel->data;
                                srow = idx->row;
                                scolumn = idx->column;
                        }
                        else
                        {
                                Ewl_Selection_Range *idx;

                                idx = EWL_SELECTION_RANGE(sel);
                                smod = sel->model;
                                sdata = sel->data;
                                srow = idx->start.row;
                                scolumn = idx->start.column;
                        }

                        ewl_mvc_selected_range_add(mvc, smod, sdata, srow,
                                                        scolumn, e->row,
                                                        e->column);
                }
                else
                        ewl_mvc_selected_set(mvc, e->model, e->mvc_data,
                                        e->row, e->column);
        }
        else if (e->modifiers & EWL_KEY_MODIFIER_CTRL)
        {
                if (ewl_mvc_selected_is(mvc, e->mvc_data, e->row, e->column))
                        ewl_mvc_selected_rm(mvc, e->mvc_data, e->row,
                                                e->column);
                else
                        ewl_mvc_selected_add(mvc, e->model, e->mvc_data,
                                        e->row, e->column);
        }
        else
                ewl_mvc_selected_set(mvc, e->model, e->mvc_data, e->row,
                                        e->column);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The wiget to deal with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief to be written
 */
void
ewl_mvc_cb_clicked_single(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_MVC_Clicked *e;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MVC_TYPE);

        e = ev;
        ewl_mvc_selected_set(EWL_MVC(w), e->model, e->mvc_data, e->row,
                                        e->column);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_selected_change_notify(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        /* notify any inheriting widgets */
        if (mvc->cb.selected_change)
                mvc->cb.selected_change(mvc);

        /* notify the app */
        ewl_callback_call(EWL_WIDGET(mvc), EWL_CALLBACK_VALUE_CHANGED);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_cb_sel_free(void *data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);

        ewl_mvc_selection_free(EWL_SELECTION(data), TRUE);
        data = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_selection_free(Ewl_Selection *sel, unsigned int set_state)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(sel);

        /* we remove the destroy callback on the highlight as it will try to
         * remove itself which causes the selection to get a NULL highlight
         * and causes highlights to hang around */
        if (sel->highlight)
        {
                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        ewl_callback_del(sel->highlight, EWL_CALLBACK_DESTROY,
                                        ewl_mvc_cb_highlight_destroy);

                        if (set_state)
                                ewl_widget_state_set(sel->highlight,
                                        "deselect",EWL_STATE_PERSISTENT);
                }
                else
                {
                        Ewl_Widget *w;

                        while ((w = ecore_list_first_remove(sel->highlight)))
                        {
                                ewl_callback_del(w, EWL_CALLBACK_DESTROY,
                                                ewl_mvc_cb_highlight_destroy);
                                
                                if (set_state)
                                        ewl_widget_state_set(w, "deselect",
                                                EWL_STATE_PERSISTENT);
                        }

                        IF_FREE_LIST(sel->highlight);
                }
        }
        FREE(sel);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_mvc_cb_highlight_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data)
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

/**
 * @param model: The model to work with this data
 * @param data: The parent data containing the index selection
 * @param row: The row to create the index selection for
 * @param column: The column to create the index for
 * @return Returns a new Ewl_Selection_Idx based on the @a row and @a column
 * @brief Creates a new index selection based on given values
 */
Ewl_Selection *
ewl_mvc_selection_index_new(const Ewl_Model *model, void *data, unsigned int row,
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

        DRETURN_PTR(EWL_SELECTION(sel), DLEVEL_STABLE);
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
ewl_mvc_selection_range_new(const Ewl_Model *model, void *data, unsigned int srow,
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

        DRETURN_PTR(EWL_SELECTION(sel), DLEVEL_STABLE);
}

