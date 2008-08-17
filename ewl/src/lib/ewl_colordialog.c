/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_colordialog.h"
#include "ewl_button.h"
#include "ewl_colorpicker.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_colordialog_respond(Ewl_Colordialog *cd, unsigned int response);

/**
 *  @return Returns a new Ewl_Colordialog widget
 *  @brief Create a new Ewl_Colordialog widget
 */
Ewl_Widget *
ewl_colordialog_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Colordialog, 1);
        if (!w)
        {
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        if (!ewl_colordialog_init(EWL_COLORDIALOG(w)))
        {
                ewl_widget_destroy(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param cd: the color dialog to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a color dialog to starting values.
 */
int
ewl_colordialog_init(Ewl_Colordialog *cd)
{
        Ewl_Widget *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cd, FALSE);

        if (!ewl_dialog_init(EWL_DIALOG(cd))) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_widget_inherit(EWL_WIDGET(cd), EWL_COLORDIALOG_TYPE);

        ewl_dialog_action_position_set(EWL_DIALOG(cd), EWL_POSITION_BOTTOM);
        ewl_window_title_set(EWL_WINDOW(cd), "Ewl Colordialog");
        ewl_window_name_set(EWL_WINDOW(cd), "Ewl Colordialog");
        ewl_window_class_set(EWL_WINDOW(cd), "Ewl Colordialog");

        ewl_callback_append(EWL_WIDGET(cd), EWL_CALLBACK_DELETE_WINDOW,
                                ewl_colordialog_cb_delete_window, NULL);

        ewl_dialog_active_area_set(EWL_DIALOG(cd), EWL_POSITION_TOP);

        /* create the color picker */
        cd->picker = ewl_colorpicker_new();
        ewl_widget_internal_set(cd->picker, TRUE);
        ewl_container_child_append(EWL_CONTAINER(cd), cd->picker);
        ewl_object_fill_policy_set(EWL_OBJECT(cd->picker),
                                        EWL_FLAG_FILL_FILL);
        ewl_widget_show(cd->picker);

        ewl_dialog_active_area_set(EWL_DIALOG(cd), EWL_POSITION_BOTTOM);

        /* create the buttons */
        o = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(cd), o);
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_colordialog_cb_button_click, cd);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(cd), o);
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_CANCEL);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_colordialog_cb_button_click, cd);
        ewl_widget_show(o);

        /* enable alpha channel by default */
        ewl_colordialog_has_alpha_set(cd, TRUE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to set the alpha value on
 * @param alpha: If the color dialog should show the alpha option
 * @return Returns no value
 * @brief This will set if the color dialog will show the alpha information
 */
void
ewl_colordialog_has_alpha_set(Ewl_Colordialog *cd, unsigned int alpha)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_has_alpha_set(EWL_COLORPICKER(cd->picker), alpha);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to get the alpha information from
 * @return Returns TRUE if the color dialog is showing alpha information
 * @brief This retrieves weither or not the color dialog is showing alpha
 * information.
 */
unsigned int
ewl_colordialog_has_alpha_get(Ewl_Colordialog *cd)
{
        unsigned int alpha;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cd, FALSE);
        DCHECK_TYPE_RET(cd, EWL_COLORDIALOG_TYPE, FALSE);

        alpha = ewl_colorpicker_has_alpha_get(EWL_COLORPICKER(cd->picker));

        DRETURN_INT(alpha, DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to set the alpha into
 * @param alpha: The alpha value to set into the color dialog
 * @return Returns no value
 * @brief Sets the alpha value of the @p cd dialog to @p alpha
 */
void
ewl_colordialog_alpha_set(Ewl_Colordialog *cd, unsigned int alpha)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_alpha_set(EWL_COLORPICKER(cd->picker), alpha);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to get the alpha value from
 * @return Returns the current alpha value selected in the dialog
 * @brief This retrieves the current alpha value selected in the dialog
 */
unsigned int
ewl_colordialog_alpha_get(Ewl_Colordialog *cd)
{
        unsigned int alpha;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cd, 255);
        DCHECK_TYPE_RET(cd, EWL_COLORDIALOG_TYPE, 255);

        alpha = ewl_colorpicker_alpha_get(EWL_COLORPICKER(cd->picker));

        DRETURN_INT(alpha, DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to set the rgb values into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @return Returns no value.
 * @brief This sets the given rbg information into the color dialog
 */
void
ewl_colordialog_current_rgb_set(Ewl_Colordialog *cd, unsigned int r,
                                unsigned int g, unsigned int b)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_current_rgb_set(EWL_COLORPICKER(cd->picker), r, g, b);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to get the rgb information from
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @return Returns no value.
 * @brief This will retrieve the rgb values from the color dialog
 */
void
ewl_colordialog_current_rgb_get(Ewl_Colordialog *cd, unsigned int *r,
                                unsigned int *g, unsigned int *b)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_current_rgb_get(EWL_COLORPICKER(cd->picker), r, g, b);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to set the previous rgb values into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @param a: The alpha value to set
 * @return Returns no value.
 * @brief This sets the given rgba values as the color to display in the
 * previous color box of the color dialog.
 */
void
ewl_colordialog_previous_rgba_set(Ewl_Colordialog *cd, unsigned int r,
                                unsigned int g, unsigned int b,
                                unsigned int a)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_previous_rgba_set(EWL_COLORPICKER(cd->picker), r, g, b, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to get the rgb information from
 * @param r: Where to store the red value
 * @param g: Where to store the green value
 * @param b: Where to store the blue value
 * @param a: Where to store the alpha value
 * @return Returns no value.
 * @brief This will retrieve the value current set into the previous color box of
 * the color dialog rgba values from the color dialog
 */
void
ewl_colordialog_previous_rgba_get(Ewl_Colordialog *cd, unsigned int *r,
                                unsigned int *g, unsigned int *b, 
                                unsigned int *a)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_previous_rgba_get(EWL_COLORPICKER(cd->picker), r, g, b, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to set the mode of
 * @param type: The mode to set on the color dialog
 * @return Returns no value.
 * @brief Sets the color dialog mode to @p type
 */
void
ewl_colordialog_color_mode_set(Ewl_Colordialog *cd, Ewl_Color_Mode type)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        ewl_colorpicker_color_mode_set(EWL_COLORPICKER(cd->picker), type);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cd: The color dialog to get the mode from
 * @return Returns the current mode of the color dialog
 * @brief This will retrive the current Ewl_Color_Mode set on the color
 * dialog
 */
Ewl_Color_Mode
ewl_colordialog_color_mode_get(Ewl_Colordialog *cd)
{
        Ewl_Color_Mode mode;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cd, EWL_COLOR_MODE_HSV_HUE);
        DCHECK_TYPE_RET(cd, EWL_COLORDIALOG_TYPE, EWL_COLOR_MODE_HSV_HUE);

        mode = ewl_colorpicker_color_mode_get(EWL_COLORPICKER(cd->picker));

        DRETURN_INT(mode, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The colourdialog
 * @return Returns no value
 * @brief The button click callback
 */
void
ewl_colordialog_cb_button_click(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        unsigned int type;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_STOCK_TYPE);
        DCHECK_TYPE(data, EWL_COLORDIALOG_TYPE);

        type = ewl_stock_type_get(EWL_STOCK(w));
        ewl_colordialog_respond(EWL_COLORDIALOG(data), type);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The delete window callback
 */
void
ewl_colordialog_cb_delete_window(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_COLORDIALOG_TYPE);

        ewl_colordialog_respond(EWL_COLORDIALOG(w), EWL_STOCK_CANCEL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_colordialog_respond(Ewl_Colordialog *cd, unsigned int response)
{
        Ewl_Event_Action_Response cd_ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cd);
        DCHECK_TYPE(cd, EWL_COLORDIALOG_TYPE);

        cd_ev.response = response;
        ewl_callback_call_with_event_data(EWL_WIDGET(cd),
                                        EWL_CALLBACK_VALUE_CHANGED, &cd_ev);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

