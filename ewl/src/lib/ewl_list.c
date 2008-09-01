/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_list.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static Ewl_Widget *ewl_list_widget_at(Ewl_MVC *mvc, void *data,
                                        unsigned int row,
                                        unsigned int column);

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
        DCHECK_PARAM_PTR_RET(list, FALSE);

        if (!ewl_mvc_init(EWL_MVC(list)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(list), EWL_LIST_TYPE);
        ewl_widget_inherit(EWL_WIDGET(list), EWL_LIST_TYPE);

        ewl_mvc_selected_change_cb_set(EWL_MVC(list), ewl_list_cb_selected_change);

        ewl_callback_append(EWL_WIDGET(list), EWL_CALLBACK_CONFIGURE,
                                                ewl_list_cb_configure, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
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
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        void *pr_data;
        unsigned int i, count;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_LIST_TYPE);

        list = EWL_LIST(w);

        model = ewl_mvc_model_get(EWL_MVC(list));
        view = ewl_mvc_view_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));
        pr_data = ewl_mvc_private_data_get(EWL_MVC(list));

        /* if either the list isn't dirty or some of the MVC controls have
         * not been set on the list just leave this up to the box to handle */
        if ((!ewl_mvc_dirty_get(EWL_MVC(list)))
                        || !model || !view || !mvc_data)
                DRETURN(DLEVEL_STABLE);

        /* get the number of rows */
        count = model->count(mvc_data);

        /* create all the widgets and pack into the container */
        ewl_container_reset(EWL_CONTAINER(list));
        for (i = 0; i < count; i++)
        {
                Ewl_Widget *o, *cell;

                cell = ewl_cell_new();
                ewl_cell_state_change_cb_add(EWL_CELL(cell));
                ewl_container_child_append(EWL_CONTAINER(list), cell);
                ewl_callback_append(cell, EWL_CALLBACK_CLICKED,
                                ewl_list_cb_item_clicked, list);
                ewl_widget_show(cell);

                o = view->fetch(model->fetch(mvc_data, i, 0), i, 0, pr_data);
                ewl_widget_show(o);

                ewl_container_child_append(EWL_CONTAINER(cell), o);
        }

        ewl_list_cb_selected_change(EWL_MVC(list));
        ewl_mvc_dirty_set(EWL_MVC(list), FALSE);

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
        const Ewl_Model *model;
        void *mvc_data;
        int row;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_LIST_TYPE);

        if (ewl_mvc_selection_mode_get(EWL_MVC(data)) ==
                                        EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        model = ewl_mvc_model_get(EWL_MVC(data));
        mvc_data = ewl_mvc_data_get(EWL_MVC(data));
        row = ewl_container_child_index_get(EWL_CONTAINER(data), w);
        if (row < 0) DRETURN(DLEVEL_STABLE);

        if ((unsigned int) row > model->count(mvc_data))
        {
                DWARNING("Don't use container function on MVC widget!");
                DRETURN(DLEVEL_STABLE);
        }

        ewl_mvc_handle_click(EWL_MVC(data), NULL, mvc_data, row, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The MVC to work with
 * @return Returns no value
 * @brief Called when the selected widgets changes
 */
void
ewl_list_cb_selected_change(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_LIST_TYPE);

        ewl_mvc_highlight(mvc, EWL_CONTAINER(mvc), ewl_list_widget_at);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_list_widget_at(Ewl_MVC *mvc, void *data __UNUSED__, unsigned int row,
                                        unsigned int column __UNUSED__)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

        w = ewl_container_child_get(EWL_CONTAINER(mvc), row);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

