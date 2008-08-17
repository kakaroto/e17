/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_freebox.h"
#include "ewl_freebox_mvc.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static Ewl_Widget *ewl_freebox_mvc_widget_at(Ewl_MVC *mvc, void *data,
                                                           unsigned int row,
                                                           unsigned int column);

/**
 * @return Returns a new horizontal Ewl_Freebox_MVC widget or NULL on failure
 * @brief creates and initializes a new horizontal freebox mvc widget
 */
Ewl_Widget *
ewl_hfreebox_mvc_new(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(ewl_freebox_mvc_new(), DLEVEL_STABLE);
}

/**
 * @return Returns a new vertical Ewl_Freebox_MVC widget or NULL on failure
 * @brief creates and initializes a new vertical freebox mvc widget
 */
Ewl_Widget *
ewl_vfreebox_mvc_new(void)
{
        Ewl_Widget *fb_mvc;

        DENTER_FUNCTION(DLEVEL_STABLE);

        fb_mvc = ewl_freebox_mvc_new();
        ewl_freebox_orientation_set(EWL_FREEBOX(EWL_FREEBOX_MVC(fb_mvc)->freebox),
                                    EWL_ORIENTATION_VERTICAL);
        DRETURN_PTR(fb_mvc, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and initializes an Ewl_Freebox_MVC widget
 */
Ewl_Widget *
ewl_freebox_mvc_new(void)
{
        Ewl_Widget *fb_mvc = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        fb_mvc = NEW(Ewl_Freebox_MVC, 1);
        if (!fb_mvc)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_freebox_mvc_init(EWL_FREEBOX_MVC(fb_mvc)))
        {
                ewl_widget_destroy(fb_mvc);
                fb_mvc = NULL;
        }

        DRETURN_PTR(fb_mvc, DLEVEL_STABLE);
}

/**
 * @param fb_mvc: The fb_mvc to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Freebox_MVC widget to default values
 */
int
ewl_freebox_mvc_init(Ewl_Freebox_MVC *fb_mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fb_mvc, FALSE);

        if (!ewl_mvc_init(EWL_MVC(fb_mvc)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        
        ewl_widget_appearance_set(EWL_WIDGET(fb_mvc), EWL_FREEBOX_MVC_TYPE);
        ewl_widget_inherit(EWL_WIDGET(fb_mvc), EWL_FREEBOX_MVC_TYPE);

        fb_mvc->freebox = ewl_hfreebox_new();
        ewl_container_child_append(EWL_CONTAINER(fb_mvc), 
                                   fb_mvc->freebox);
        ewl_container_redirect_set(EWL_CONTAINER(fb_mvc), 
                                   EWL_CONTAINER(fb_mvc->freebox));
        ewl_widget_show(fb_mvc->freebox);

        ewl_mvc_selected_change_cb_set(EWL_MVC(fb_mvc), 
                                       ewl_freebox_mvc_cb_selected_change);
        ewl_callback_append(EWL_WIDGET(fb_mvc), EWL_CALLBACK_CONFIGURE,
                            ewl_freebox_mvc_cb_configure, NULL);
        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fb_mvc: The freebox mvc to use
 * @param orientation: The orientation to set
 * @return Returns no value
 * @brief Sets the orientation of the freebox mvc
 */
void
ewl_freebox_mvc_orientation_set(Ewl_Freebox_MVC *fb_mvc, 
                                Ewl_Orientation orientation)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fb_mvc);
        DCHECK_TYPE(fb_mvc, EWL_FREEBOX_MVC_TYPE);

        ewl_freebox_orientation_set(EWL_FREEBOX(fb_mvc->freebox),
                                    orientation);
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fb_mvc: The freebox mvc to use
 * @return Returns the orientation of the freebox mvc
 * brief Retrieve the current orientation of the freebox mvc
 */
Ewl_Orientation
ewl_freebox_mvc_orientation_get(Ewl_Freebox_MVC *fb_mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fb_mvc, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(fb_mvc, EWL_FREEBOX_MVC_TYPE, 
                                EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(ewl_freebox_orientation_get(EWL_FREEBOX(fb_mvc->freebox)),
                    DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The fb_mvc to be configured
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Configures the given list
 */
void
ewl_freebox_mvc_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
                                            void *data __UNUSED__)
{
        Ewl_Freebox_MVC *fb_mvc;
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        unsigned int i, count;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_FREEBOX_MVC_TYPE);

        fb_mvc = EWL_FREEBOX_MVC(w);

        model = ewl_mvc_model_get(EWL_MVC(fb_mvc));
        view = ewl_mvc_view_get(EWL_MVC(fb_mvc));
        mvc_data = ewl_mvc_data_get(EWL_MVC(fb_mvc));

        if ((!ewl_mvc_dirty_get(EWL_MVC(fb_mvc))) 
                        || !model || !view || !mvc_data)
                DRETURN(DLEVEL_STABLE);
        
        ewl_container_reset(EWL_CONTAINER(fb_mvc));
	 count = (unsigned int)model->count(mvc_data);
	 for (i = 0; i < count; i++)
        {
                Ewl_Widget *o, *cell;

                cell = ewl_cell_new();
                ewl_cell_state_change_cb_add(EWL_CELL(cell));
                ewl_container_child_append(EWL_CONTAINER(fb_mvc), cell);
                ewl_callback_append(cell, EWL_CALLBACK_CLICKED, 
                                    ewl_freebox_mvc_cb_item_clicked, fb_mvc);
                ewl_widget_show(cell);

                o = view->fetch(model->fetch(mvc_data, i, 0), i, 0);
                ewl_widget_show(o);

                ewl_container_child_append(EWL_CONTAINER(cell), o);
        }

        ewl_freebox_mvc_cb_selected_change(EWL_MVC(fb_mvc));
        ewl_mvc_dirty_set(EWL_MVC(fb_mvc), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget that was clicked
 * @param ev: The event data
 * @param data: The fb_mvc widget
 * @return Returns no value
 * @brief Sets the clicked widget as selected
 */
void
ewl_freebox_mvc_cb_item_clicked(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        const Ewl_Model *model;
        void *mvc_data;
        int row;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_FREEBOX_MVC_TYPE);

        if (ewl_mvc_selection_mode_get(EWL_MVC(data)) == 
                                       EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        model = ewl_mvc_model_get(EWL_MVC(data));
        mvc_data = ewl_mvc_data_get(EWL_MVC(data));
        row = ewl_container_child_index_get(EWL_CONTAINER(data), w);
        if (row < 0) DRETURN(DLEVEL_STABLE);

        if ((unsigned int)row > model->count(mvc_data))
        {
                DWARNING("Don't use container functions on MVC widgets!.");
                DRETURN(DLEVEL_STABLE);
        }

        ewl_mvc_handle_click(EWL_MVC(data), NULL, mvc_data, row, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The MVC to work with
 * @return Returns no value
 * @brief Called when the selected widget changes
 */
void
ewl_freebox_mvc_cb_selected_change(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_FREEBOX_MVC_TYPE);

        ewl_mvc_highlight(mvc, EWL_CONTAINER(mvc), ewl_freebox_mvc_widget_at);
        
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_freebox_mvc_widget_at(Ewl_MVC *mvc, void *data __UNUSED__, unsigned int row,
                                        unsigned int column __UNUSED__)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

        w = ewl_container_child_get(EWL_CONTAINER(mvc), row);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

