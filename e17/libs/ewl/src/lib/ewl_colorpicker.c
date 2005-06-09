#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * @return Returns NULL on failure, otherwise a newly allocated color picker.
 * @brief Allocate and initialize a new color picker widget.
 */
Ewl_Widget *ewl_colorpicker_new()
{
	Ewl_ColorPicker *cp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cp = NEW(Ewl_ColorPicker, 1);
	if (!ewl_colorpicker_init(cp)) 
        {
		FREE(cp);
		cp = NULL;
	}

	DRETURN_PTR(cp, DLEVEL_STABLE);
}

/**
 * @param cp: the color picker to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a color picker to starting values.
 */
int ewl_colorpicker_init(Ewl_ColorPicker *cp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cp", cp, 0);

	if (!ewl_box_init(EWL_BOX(cp), EWL_ORIENTATION_HORIZONTAL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(cp), "colorpicker");
	ewl_widget_inherit(EWL_WIDGET(cp), "colorpicker");

	ewl_box_spacing_set(EWL_BOX(cp), 20);

	/*
	 * Setup the range spectrum region for the base color.
	 */
	cp->range = ewl_spectrum_new();
	ewl_spectrum_mode_set(EWL_SPECTRUM(cp->range),
			      EWL_PICK_MODE_HSV_HUE);
	ewl_spectrum_dimensions_set(EWL_SPECTRUM(cp->range), 1);
	ewl_callback_append(cp->range, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_colorpicker_range_down_cb, cp);
	ewl_callback_append(cp->range, EWL_CALLBACK_MOUSE_UP,
			    ewl_colorpicker_range_up_cb, cp);
	ewl_callback_append(cp->range, EWL_CALLBACK_MOUSE_MOVE,
			    ewl_colorpicker_range_move_cb, cp);
	ewl_object_preferred_inner_size_set(EWL_OBJECT(cp->range), 20, 200);
	ewl_object_fill_policy_set(EWL_OBJECT(cp->range), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(cp), cp->range);
	ewl_widget_show(cp->range);

	/*
	 * Setup the larger spectrum region for selecting shades of the base
	 * color.
	 */
	cp->spectrum = ewl_spectrum_new();
	ewl_callback_append(cp->spectrum, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_colorpicker_spectrum_down_cb, cp);
	ewl_callback_append(cp->spectrum, EWL_CALLBACK_MOUSE_UP,
			    ewl_colorpicker_spectrum_up_cb, cp);
	ewl_callback_append(cp->spectrum, EWL_CALLBACK_MOUSE_MOVE,
			    ewl_colorpicker_spectrum_move_cb, cp);
	ewl_spectrum_mode_set(EWL_SPECTRUM(cp->spectrum), EWL_PICK_MODE_RGB);
	ewl_object_preferred_inner_size_set(EWL_OBJECT(cp->spectrum), 200, 200);
	ewl_object_fill_policy_set(EWL_OBJECT(cp->spectrum), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(cp), cp->spectrum);
	ewl_widget_show(cp->spectrum);

        /*
         * A text to see the selected color
         */

        cp->preview = ewl_text_new(NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(cp->preview), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(cp->preview), EWL_FLAG_ALIGN_CENTER);
        ewl_text_color_set(EWL_TEXT(cp->preview), 0, 0, 0, 255);
	ewl_container_child_append(EWL_CONTAINER(cp), cp->preview);
        ewl_text_text_set(EWL_TEXT(cp->preview), "Preview");
	ewl_widget_show(cp->preview);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void ewl_colorpicker_range_move_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
        #undef R
        #undef G
        #undef B
        #undef A
        #define R cp->selected.r
        #define G cp->selected.g
        #define B cp->selected.b
        #define A cp->selected.a
	Ewl_ColorPicker *cp = user_data;
	Ewl_Event_Mouse_Move *ev = ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (cp->drag) 
        { 
                int x = ev->x - w->object.current.x;
                int y = ev->y - w->object.current.y;
		ewl_spectrum_color_coord_map(EWL_SPECTRUM(w), x, y, &R, &G, &B, &A);
		ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->spectrum), R, G, B);
		ewl_callback_call_with_event_data(EWL_WIDGET(cp), EWL_CALLBACK_VALUE_CHANGED, &cp->selected);

	        ewl_widget_hide(cp->preview);
                IF_FREE(cp->preview);
                cp->preview = ewl_text_new(NULL);
                ewl_object_fill_policy_set(EWL_OBJECT(cp->preview), EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(cp->preview), EWL_FLAG_ALIGN_CENTER);
                ewl_text_color_set(EWL_TEXT(cp->preview), R, G, B, 255);
	        ewl_container_child_remove(EWL_CONTAINER(cp), cp->preview);
	        ewl_container_child_append(EWL_CONTAINER(cp), cp->preview);
                ewl_text_text_set(EWL_TEXT(cp->preview), "Preview");
	        ewl_widget_show(cp->preview);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_colorpicker_range_down_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_ColorPicker *cp = user_data;
	Ewl_Event_Mouse_Down *ev = ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cp->drag = 1;

        int x = ev->x - w->object.current.x;
        int y = ev->y - w->object.current.y;
	ewl_spectrum_color_coord_map(EWL_SPECTRUM(w), x, y, &R, &G, &B, &A);
	ewl_spectrum_rgb_set(EWL_SPECTRUM(cp->spectrum), R, G, B);
	ewl_callback_call_with_event_data(EWL_WIDGET(cp), EWL_CALLBACK_VALUE_CHANGED, &cp->selected);
        ewl_text_color_set(EWL_TEXT(cp->preview), R, G, B, 255);

	ewl_widget_hide(cp->preview);
        IF_FREE(cp->preview);
        cp->preview = ewl_text_new(NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(cp->preview), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(cp->preview), EWL_FLAG_ALIGN_CENTER);
        ewl_text_color_set(EWL_TEXT(cp->preview), R, G, B, 255);
        ewl_container_child_remove(EWL_CONTAINER(cp), cp->preview);
        ewl_container_child_append(EWL_CONTAINER(cp), cp->preview);
        ewl_text_text_set(EWL_TEXT(cp->preview), "Preview");
        ewl_widget_show(cp->preview);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_colorpicker_range_up_cb(Ewl_Widget *w __UNUSED__, 
                                        void *ev_data __UNUSED__, void *user_data)
{
	Ewl_ColorPicker *cp = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cp->drag = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_colorpicker_spectrum_move_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_ColorPicker *cp = user_data;
	Ewl_Event_Mouse_Move *ev = ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (cp->drag) 
        {
                int x = ev->x - w->object.current.x;
                int y = ev->y - w->object.current.y;
		ewl_spectrum_color_coord_map(EWL_SPECTRUM(w), x, y, &R, &G, &B, &A);
	        ewl_callback_call_with_event_data(EWL_WIDGET(cp), EWL_CALLBACK_VALUE_CHANGED, &cp->selected);

	        ewl_widget_hide(cp->preview);
                IF_FREE(cp->preview);
                cp->preview = ewl_text_new(NULL);
                ewl_object_fill_policy_set(EWL_OBJECT(cp->preview), EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(cp->preview), EWL_FLAG_ALIGN_CENTER);
                ewl_text_color_set(EWL_TEXT(cp->preview), R, G, B, 255);
	        ewl_container_child_remove(EWL_CONTAINER(cp), cp->preview);
	        ewl_container_child_append(EWL_CONTAINER(cp), cp->preview);
                ewl_text_text_set(EWL_TEXT(cp->preview), "Preview");
	        ewl_widget_show(cp->preview);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_colorpicker_spectrum_down_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_ColorPicker *cp = user_data;
	Ewl_Event_Mouse_Down *ev = ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cp->drag = 1;
        
        int x = ev->x - w->object.current.x;
        int y = ev->y - w->object.current.y;
	ewl_spectrum_color_coord_map(EWL_SPECTRUM(w), x, y, &R, &G, &B, &A);
	ewl_callback_call_with_event_data(EWL_WIDGET(cp), EWL_CALLBACK_VALUE_CHANGED, &cp->selected);
        ewl_text_color_set(EWL_TEXT(cp->preview), R, G, B, 255);

	ewl_widget_hide(cp->preview);
        IF_FREE(cp->preview);
        cp->preview = ewl_text_new(NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(cp->preview), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(cp->preview), EWL_FLAG_ALIGN_CENTER);
        ewl_text_color_set(EWL_TEXT(cp->preview), R, G, B, 255);
        ewl_container_child_remove(EWL_CONTAINER(cp), cp->preview);
        ewl_container_child_append(EWL_CONTAINER(cp), cp->preview);
        ewl_text_text_set(EWL_TEXT(cp->preview), "Preview");
        ewl_widget_show(cp->preview);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_colorpicker_spectrum_up_cb(Ewl_Widget *w __UNUSED__, 
                                        void *ev_data __UNUSED__, void *user_data)
{
	Ewl_ColorPicker *cp = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cp->drag = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
