/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_combo.h"
#include "ewl_button.h"
#include "ewl_cell.h"
#include "ewl_context_menu.h"
#include "ewl_scrollpane.h"
#include "ewl_menu.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_combo_cb_selected_change(Ewl_MVC *mvc);
Ewl_Widget *ewl_combo_submenu_new(Ewl_Combo *c, const Ewl_Model *model,
                                        const Ewl_View *view, void *mvc_data);
static void ewl_combo_popup_fill(Ewl_Combo *combo, Ewl_Container *c,
                        const Ewl_Model *model, const Ewl_View *view, 
                        void *mvc_data);

/**
 * @return Returns a pointer to a new combo on success, NULL on failure.
 * @brief Create a new combo box
 */
Ewl_Widget *
ewl_combo_new(void)
{
        Ewl_Combo *combo;

        DENTER_FUNCTION(DLEVEL_STABLE);

        combo = NEW(Ewl_Combo, 1);
        if (!combo)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_combo_init(combo))
        {
                ewl_widget_destroy(EWL_WIDGET(combo));
                combo = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(combo), DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a combo to default values
 */
int
ewl_combo_init(Ewl_Combo *combo)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(combo, FALSE);

        if (!ewl_mvc_init(EWL_MVC(combo)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(combo), EWL_COMBO_TYPE);
        ewl_widget_appearance_set(EWL_WIDGET(combo), EWL_COMBO_TYPE);
        ewl_box_orientation_set(EWL_BOX(combo), EWL_ORIENTATION_HORIZONTAL);
        ewl_mvc_selected_change_cb_set(EWL_MVC(combo),
                                        ewl_combo_cb_selected_change);

        combo->button = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(combo), combo->button);
        ewl_widget_internal_set(combo->button, TRUE);
        ewl_object_alignment_set(EWL_OBJECT(combo->button),
                                        EWL_FLAG_ALIGN_RIGHT);
        ewl_callback_append(combo->button, EWL_CALLBACK_CLICKED,
                                ewl_combo_cb_decrement_clicked, combo);
        ewl_widget_show(combo->button);

        /*
         * setup the popup
         */
        combo->popup = ewl_context_menu_new();
        ewl_popup_follow_set(EWL_POPUP(combo->popup), EWL_WIDGET(combo));
        ewl_popup_type_set(EWL_POPUP(combo->popup),
                                EWL_POPUP_TYPE_MENU_VERTICAL);
        ewl_popup_fit_to_follow_set(EWL_POPUP(combo->popup), TRUE);
        ewl_window_keyboard_grab_set(EWL_WINDOW(combo->popup), TRUE);
        ewl_window_pointer_grab_set(EWL_WINDOW(combo->popup), TRUE);
        ewl_widget_appearance_set(combo->popup, EWL_COMBO_TYPE
                                                "/"EWL_POPUP_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(combo->popup),
                                        EWL_FLAG_FILL_HFILL
                                        | EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(combo->popup),
                                EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
        ewl_callback_append(combo->popup, EWL_CALLBACK_HIDE,
                                        ewl_combo_cb_popup_hide, combo);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @param editable: Set if the combo is editable or not
 * @return Returns no value
 * @brief This will set if the displayed data in the combo is editable.
 *
 * If the editable flag is set to TRUE then the combo will always request
 * the header as the item to display. If it is FALSE then the combo will use
 * the selected widget as the display.
 */
void
ewl_combo_editable_set(Ewl_Combo *combo, unsigned int editable)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(combo);
        DCHECK_TYPE(combo, EWL_COMBO_TYPE);

        if (combo->editable == !!editable)
                DRETURN(DLEVEL_STABLE);

        combo->editable = !!editable;

        /* force the selected display to change */
        if (combo->mvc.model && combo->mvc.view)
		ewl_combo_cb_selected_change(EWL_MVC(combo));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @return Returns the editable status of the combo
 * @brief Retrieves the editable status of the combo
 */
unsigned int
ewl_combo_editable_get(Ewl_Combo *combo)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(combo, FALSE);
        DCHECK_TYPE_RET(combo, EWL_COMBO_TYPE, FALSE);

        DRETURN_INT(combo->editable, DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @param scrollable: Set if the combo is scrollable or not
 * @return Returns no value
 *
 * On true, this functions set the combo to use a scrollpane to view the
 * widget inside of it. The maximal size of it is set by a theme
 * defined value.
 */
void
ewl_combo_scrollable_set(Ewl_Combo *combo, unsigned int scrollable)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(combo);
        DCHECK_TYPE(combo, EWL_COMBO_TYPE);

        if (combo->scrollable == !!scrollable)
                DRETURN(DLEVEL_STABLE);

        combo->scrollable = !!scrollable;

        if (scrollable) {
                Ewl_Widget *scroll;
                int max_h;

                max_h = ewl_theme_data_int_get(EWL_WIDGET(combo),
                                                "/combo/popup/height");
                /* set it to sane values if it isn't defined in the theme */
                if (max_h <= 0)
                        max_h = 100;

                /* setup the new scrollpane container */
                scroll = ewl_scrollpane_new();
                ewl_object_fill_policy_set(EWL_OBJECT(scroll),
                                                EWL_FLAG_FILL_HFILL
                                                | EWL_FLAG_FILL_SHRINKABLE);
                ewl_object_maximum_h_set(EWL_OBJECT(scroll), max_h);
                ewl_widget_show(scroll);

                ewl_context_menu_container_set(EWL_CONTEXT_MENU(combo->popup),
                                                        EWL_CONTAINER(scroll));
        }
        else
                ewl_context_menu_container_set(EWL_CONTEXT_MENU(combo->popup),
                                                NULL);


        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @return Returns the scrollable status of the combo
 * @brief Retrieves the scrollable status of the combo
 */
unsigned int
ewl_combo_scrollable_get(Ewl_Combo *combo)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(combo, FALSE);
        DCHECK_TYPE_RET(combo, EWL_COMBO_TYPE, FALSE);

        DRETURN_INT(combo->scrollable, DLEVEL_STABLE);
}

/**
 * @param combo: The Ewl_Combo to use
 * @param c: The Container to use in the popup
 * @return Returns no value
 */
void
ewl_combo_popup_container_set(Ewl_Combo *combo, Ewl_Container *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(combo);
        DCHECK_TYPE(combo, EWL_COMBO_TYPE);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        combo->scrollable = FALSE;
        ewl_context_menu_container_set(EWL_CONTEXT_MENU(combo->popup), c);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The combo box
 * @return Returns no value
 * @brief Callback for when the button to expand the combo is pressed
 */
void
ewl_combo_cb_decrement_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                                void *data)
{
        Ewl_Combo *combo;
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_COMBO_TYPE);

        combo = data;
        model = ewl_mvc_model_get(EWL_MVC(combo));
        view = ewl_mvc_view_get(EWL_MVC(combo));
        mvc_data = ewl_mvc_data_get(EWL_MVC(combo));

        /* nothing to do if we have no model or view
         * Note that mvc_data == NULL is legal */
        if (!model || !view)
                DRETURN(DLEVEL_STABLE);

        ewl_widget_show(combo->popup);
        ewl_window_raise(EWL_WINDOW(combo->popup));
        ewl_widget_focus_send(EWL_WIDGET(combo->popup));

        ewl_widget_state_set(combo->button, "expanded",
                                        EWL_STATE_PERSISTENT);
        ewl_widget_state_set(EWL_WIDGET(combo), "expanded",
                                        EWL_STATE_PERSISTENT);

        if (!ewl_mvc_dirty_get(EWL_MVC(combo)))
                DRETURN(DLEVEL_STABLE);

        ewl_container_reset(EWL_CONTAINER(combo->popup));
        ewl_combo_popup_fill(combo, EWL_CONTAINER(combo->popup),
                                model, view, mvc_data);

        ewl_mvc_dirty_set(EWL_MVC(combo), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The combo popup
 * @return Returns no value
 * @brief Callback for when the button to close the combo is clicked
 */
void
ewl_combo_cb_popup_hide(Ewl_Widget *w __UNUSED__,
                                void *ev __UNUSED__, void *data)
{
        Ewl_Combo *combo;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_COMBO_TYPE);

        combo = EWL_COMBO(data);
        ewl_widget_state_set(combo->button, "collapsed", EWL_STATE_PERSISTENT);
        ewl_widget_state_set(EWL_WIDGET(combo), "collapsed",
                                        EWL_STATE_PERSISTENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


static void
ewl_combo_cb_selected_change(Ewl_MVC *mvc)
{
        const Ewl_View *view;
        Ewl_Combo *combo;
        Ewl_Widget *item = NULL;
        void *pr_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_COMBO_TYPE);

        combo = EWL_COMBO(mvc);
        view = ewl_mvc_view_get(mvc);
        pr_data = ewl_mvc_private_data_get(EWL_MVC(combo));

        if (!ewl_mvc_data_get(mvc))
                DRETURN(DLEVEL_STABLE);

        /*
         * if we haven't already build a cell for the header, build it
         * now
         */
        if (!combo->header) {
                combo->header = ewl_cell_new();
                ewl_container_child_prepend(EWL_CONTAINER(combo),
                                                        combo->header);
                ewl_object_fill_policy_set(EWL_OBJECT(combo->header),
                                                        EWL_FLAG_FILL_FILL);
                ewl_widget_show(combo->header);
        }
        else
                ewl_container_reset(EWL_CONTAINER(combo->header));

        if (ewl_mvc_selected_count_get(mvc))
        {
                Ewl_Selection_Idx *idx;
                const Ewl_Model *model;
                void *mvc_data;

                idx = ewl_mvc_selected_get(mvc);
                model = idx->sel.model;
                mvc_data = idx->sel.data;

                item = view->header_fetch(
                                        model->fetch(mvc_data, idx->row, 0),
                                        0, pr_data);

                FREE(idx);
        }

        else
        {
                const Ewl_Model *model;
                void *mvc_data;

                model = ewl_mvc_model_get(mvc);
                mvc_data = ewl_mvc_data_get(mvc);

                if (model->header)
                        item = view->header_fetch(model->header(mvc_data, 0),
                                                        0, pr_data);
                else
                        item = view->header_fetch(NULL, 0, pr_data);
        }

        if (item)
        {
                ewl_container_child_prepend(EWL_CONTAINER(combo->header), item);
                ewl_widget_show(item);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The combo box
 * @return Returns no value
 * @brief Callback for when the button to expand the combo is pressed
 */
Ewl_Widget *
ewl_combo_submenu_new(Ewl_Combo *combo, const Ewl_Model *model, 
                        const Ewl_View *view, void *mvc_data)
{
        Ewl_Widget *menu;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(combo, NULL);
        DCHECK_TYPE_RET(combo, EWL_COMBO_TYPE, NULL);

        menu = ewl_menu_new();
        ewl_widget_appearance_set(EWL_MENU(menu)->popup, EWL_COMBO_TYPE
                                                "/"EWL_POPUP_TYPE);
        ewl_widget_appearance_set(menu, EWL_COMBO_TYPE"_menu");

        /* nothing to do if we have no model/view or data */
        if (!model || !view)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_combo_popup_fill(combo, EWL_CONTAINER(menu),
                                model, view, mvc_data);

        ewl_button_label_set(EWL_BUTTON(menu), NULL);
        ewl_button_image_set(EWL_BUTTON(menu), NULL, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_HFILL);
        ewl_container_redirect_set(EWL_CONTAINER(menu),
                        EWL_CONTAINER(EWL_BUTTON(menu)->body));

        DRETURN_PTR(menu, DLEVEL_STABLE);
}

/**
 * @internal
 * @param combo: The combo
 * @param c: The container to fill
 * @param model: The model to fill the container
 * @param mvc_data: The data to fill the container
 * @return Returns no value
 * @brief fill the given container with the items
 */
static void
ewl_combo_popup_fill(Ewl_Combo *combo, Ewl_Container *c, const Ewl_Model *model,
                        const Ewl_View *view, void *mvc_data)
{
        unsigned int count, i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(combo);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(combo, EWL_COMBO_TYPE);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        if (!model->count)
                DRETURN(DLEVEL_STABLE);

        count = model->count(mvc_data);
        for (i = 0; i < count; i++)
        {
                Ewl_Widget *o, *item;

                if (model->expansion.is && model->expansion.is(mvc_data, i))
                {
                        const Ewl_Model *em;
                        const Ewl_View *ev;
                        void *ed;

                        /* if there shouldn't be a model for the expansion
                         * we us the current model */
                        if (!model->expansion.model
                                        || !(em = model->expansion.model(mvc_data, i)))
                                em = model;
                        /* if there shouldm't be a view for the expansion
                         * we us the current view */
                        if (!view->expansion
                                        || !(ev = view->expansion(mvc_data,i)))
                                ev = view;

                        /* if there is no data for the expansion use the
                         * current mvc data */
                        if (model->expansion.data)
                                ed = model->expansion.data(mvc_data, i);
                        else
                                ed = mvc_data;

                        o = ewl_combo_submenu_new(combo, em, ev, ed);
                }
                else
                {
                        o = ewl_combo_cell_new();
                        ewl_combo_cell_combo_set(EWL_COMBO_CELL(o), combo);
                        ewl_combo_cell_model_set(EWL_COMBO_CELL(o), model);
                        ewl_combo_cell_data_set(EWL_COMBO_CELL(o), mvc_data);
                }
                ewl_container_child_append(c, o);
                ewl_widget_show(o);

                if (view->constructor && model->fetch && view->assign)
                {
                        void *pr_data;

                        pr_data = ewl_mvc_private_data_get(EWL_MVC(combo));
                        item = view->constructor(0, pr_data);
                        view->assign(item, model->fetch(mvc_data, i, 0), i, 0,
                                pr_data);
                        ewl_container_child_append(EWL_CONTAINER(o), item);
                        ewl_widget_show(item);
                }
        }
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a pointer to a new combo cell  on success, NULL on failure.
 * @brief Create a new combo cell
 */
Ewl_Widget *
ewl_combo_cell_new(void)
{
        Ewl_Combo_Cell *cell;

        DENTER_FUNCTION(DLEVEL_STABLE);

        cell = NEW(Ewl_Combo_Cell, 1);
        if (!cell)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_combo_cell_init(cell))
        {
                ewl_widget_destroy(EWL_WIDGET(cell));
                cell = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(cell), DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo_Cell to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a combo cell to default values
 */
int
ewl_combo_cell_init(Ewl_Combo_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cell, FALSE);

        if (!ewl_cell_init(EWL_CELL(cell)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(cell), EWL_COMBO_CELL_TYPE);
        ewl_widget_appearance_set(EWL_WIDGET(cell), EWL_COMBO_CELL_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_HFILL);
        ewl_callback_append(EWL_WIDGET(cell), EWL_CALLBACK_CLICKED,
                                ewl_combo_cell_cb_clicked, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo_Cell to use
 * @param combo: the parent combo widget
 * @return Returns no value
 */
void
ewl_combo_cell_combo_set(Ewl_Combo_Cell *cell, Ewl_Combo *combo)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cell);
        DCHECK_TYPE(cell, EWL_COMBO_CELL_TYPE);
        DCHECK_PARAM_PTR(combo);
        DCHECK_TYPE(combo, EWL_COMBO_TYPE);

        cell->combo = combo;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo to use
 * @return Returns the parent combo of the cell
 */
Ewl_Combo *
ewl_combo_cell_combo_get(Ewl_Combo_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cell, NULL);
        DCHECK_TYPE_RET(cell, EWL_COMBO_CELL_TYPE, NULL);

        DRETURN_PTR(cell->combo, DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo_Cell to use
 * @param model: the model for the cell
 * @return Returns no value
 */
void
ewl_combo_cell_model_set(Ewl_Combo_Cell *cell, const Ewl_Model *model)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cell);
        DCHECK_TYPE(cell, EWL_COMBO_CELL_TYPE);
        DCHECK_PARAM_PTR(model);

        cell->model = model;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo to use
 * @return Returns the model of the cell
 */
const Ewl_Model *
ewl_combo_cell_model_get(Ewl_Combo_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cell, NULL);
        DCHECK_TYPE_RET(cell, EWL_COMBO_CELL_TYPE, NULL);

        DRETURN_PTR(cell->model, DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo_Cell to use
 * @param mvc_data: the data for the cell
 * @return Returns no value
 */
void
ewl_combo_cell_data_set(Ewl_Combo_Cell *cell, void *mvc_data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cell);
        DCHECK_TYPE(cell, EWL_COMBO_CELL_TYPE);

        cell->mvc_data = mvc_data;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cell: The Ewl_Combo to use
 * @return Returns the model of the cell
 */
void *
ewl_combo_cell_data_get(Ewl_Combo_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cell, NULL);
        DCHECK_TYPE_RET(cell, EWL_COMBO_CELL_TYPE, NULL);

        DRETURN_PTR(cell->mvc_data, DLEVEL_STABLE);
}
/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param data: The combo widget
 * @return Returns no value
 * @brief Callback for when a combo item is clicked
 */
void
ewl_combo_cell_cb_clicked(Ewl_Widget *w, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        Ewl_Combo *combo;
        const Ewl_Model *model;
        void *mvc_data;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_COMBO_CELL_TYPE);

        i = ewl_container_child_index_get(EWL_CONTAINER(w->parent), w);
        if (i < 0) DRETURN(DLEVEL_STABLE);

        combo = ewl_combo_cell_combo_get(EWL_COMBO_CELL(w));
        model = ewl_combo_cell_model_get(EWL_COMBO_CELL(w));
        mvc_data = ewl_combo_cell_data_get(EWL_COMBO_CELL(w));

        ewl_mvc_selected_set(EWL_MVC(combo), model, mvc_data, i, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

