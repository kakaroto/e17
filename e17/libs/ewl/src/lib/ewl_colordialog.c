#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_colordialog_respond(Ewl_Colordialog *cd, unsigned int response);

/**
 *  @return Returns a new Ewl_Colordialog widget
 *  
 *  This will display a colorpicker inside a window with an Ok and Cancel
 *  button. When the user hits one of the buttons
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
	DCHECK_PARAM_PTR_RET("cd", cd, FALSE);

	if (!ewl_dialog_init(EWL_DIALOG(cd))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_inherit(EWL_WIDGET(cd), "colordialog");

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
	ewl_button_stock_type_set(EWL_BUTTON(o), EWL_STOCK_OK);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_colordialog_cb_button_click, cd);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(cd), o);
	ewl_button_stock_type_set(EWL_BUTTON(o), EWL_STOCK_CANCEL);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_colordialog_cb_button_click, cd);
	ewl_widget_show(o);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_colordialog_has_alpha_set(Ewl_Colordialog *cd, unsigned int alpha)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");

	ewl_colorpicker_has_alpha_set(EWL_COLORPICKER(cd->picker), alpha);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_colordialog_has_alpha_get(Ewl_Colordialog *cd)
{
	unsigned int alpha;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cd", cd, FALSE);
	DCHECK_TYPE_RET("cd", cd, "colordialog", FALSE);

	alpha = ewl_colorpicker_has_alpha_get(EWL_COLORPICKER(cd->picker));
	DRETURN_INT(alpha, DLEVEL_STABLE);
}

void
ewl_colordialog_alpha_set(Ewl_Colordialog *cd, unsigned int alpha)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");
		 
	ewl_colorpicker_alpha_set(EWL_COLORPICKER(cd->picker), alpha);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_colordialog_alpha_get(Ewl_Colordialog *cd)
{
	unsigned int alpha;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cd", cd, 255);
	DCHECK_TYPE_RET("cd", cd, "colordialog", 255);

	alpha = ewl_colorpicker_alpha_get(EWL_COLORPICKER(cd->picker));

	DRETURN_INT(alpha, DLEVEL_STABLE);
}

void
ewl_colordialog_current_rgb_set(Ewl_Colordialog *cd, unsigned int r,
				unsigned int g, unsigned int b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");
	 
	ewl_colorpicker_current_rgb_set(EWL_COLORPICKER(cd->picker), r, g, b);
	 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colordialog_current_rgb_get(Ewl_Colordialog *cd, unsigned int *r,
				unsigned int *g, unsigned int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");

	ewl_colorpicker_current_rgb_get(EWL_COLORPICKER(cd->picker), r, g, b);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colordialog_previous_rgb_set(Ewl_Colordialog *cd, unsigned int r,
				unsigned int g, unsigned int b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");

	ewl_colorpicker_previous_rgb_set(EWL_COLORPICKER(cd->picker), r, g, b);
	 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colordialog_previous_rgb_get(Ewl_Colordialog *cd, unsigned int *r,
				unsigned int *g, unsigned int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");

	ewl_colorpicker_previous_rgb_get(EWL_COLORPICKER(cd->picker), r, g, b);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colordialog_color_mode_set(Ewl_Colordialog *cd, Ewl_Color_Mode type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");

	ewl_colorpicker_color_mode_set(EWL_COLORPICKER(cd->picker), type);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Color_Mode
ewl_colordialog_color_mode_get(Ewl_Colordialog *cd)
{
	Ewl_Color_Mode mode;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cd", cd, EWL_COLOR_MODE_HSV_HUE);
	DCHECK_TYPE_RET("cd", cd, "colordialog", EWL_COLOR_MODE_HSV_HUE);

	mode = ewl_colorpicker_color_mode_get(EWL_COLORPICKER(cd->picker));

	DRETURN_INT(mode, DLEVEL_STABLE);
}

Ewl_Colordialog_Event *
ewl_colordialog_event_new(void)
{
	Ewl_Colordialog_Event *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = NEW(Ewl_Colordialog_Event, 1);

	DRETURN_PTR(ev, DLEVEL_STABLE);
}

void
ewl_colordialog_event_free(Ewl_Colordialog_Event *ev)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ev", ev);

	FREE(ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colordialog_event_response_set(Ewl_Colordialog_Event *ev, 
						unsigned int resp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ev", ev);

	ev->response = resp;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_colordialog_event_response_get(Ewl_Colordialog_Event *ev)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ev", ev, 0);

	DRETURN_INT(ev->response, DLEVEL_STABLE);
}

void
ewl_colordialog_cb_button_click(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	unsigned int type;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	type = ewl_button_stock_type_get(EWL_BUTTON(w));
	ewl_colordialog_respond(EWL_COLORDIALOG(data), type);
	 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_colordialog_cb_delete_window(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	ewl_colordialog_respond(EWL_COLORDIALOG(w), EWL_STOCK_CANCEL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_colordialog_respond(Ewl_Colordialog *cd, unsigned int response)
{
	Ewl_Colordialog_Event *cd_ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cd", cd);
	DCHECK_TYPE("cd", cd, "colordialog");

	cd_ev = ewl_colordialog_event_new();
	ewl_colordialog_event_response_set(cd_ev, response);

	ewl_callback_call_with_event_data(EWL_WIDGET(cd), 
					EWL_CALLBACK_VALUE_CHANGED, cd_ev);

	ewl_colordialog_event_free(cd_ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

