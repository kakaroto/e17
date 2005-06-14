#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * @return Returns NULL on failure, otherwise a newly allocated color dialog.
 * @brief Allocate and initialize a new color dialog widget.
 */
Ewl_Widget *ewl_colordialog_new(int r, int g, int b)
{
	Ewl_ColorDialog *cd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cd = NEW(Ewl_ColorDialog, 1);
        if (!cd) {
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

	if (!ewl_colordialog_init(cd, r, g, b)) {
                ewl_widget_destroy(EWL_WIDGET(cd));
		cd = NULL;
	}
	DRETURN_PTR(EWL_WIDGET(cd), DLEVEL_STABLE);
}

/**
 * @param cd: the color dialog to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a color dialog to starting values.
 */
int ewl_colordialog_init(Ewl_ColorDialog *cd, int r, int g, int b)
{
        Ewl_Widget *vbox;
        Ewl_Widget *hbox;
        Ewl_Widget *button;
        Ewl_Widget *label;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cd", cd, FALSE);

	if (!ewl_box_init(EWL_BOX(cd), EWL_ORIENTATION_HORIZONTAL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(cd), "colordialog");
	ewl_widget_inherit(EWL_WIDGET(cd), "colordialog");

	ewl_box_spacing_set(EWL_BOX(cd), 5);
        ewl_object_minimum_size_set(EWL_OBJECT(cd), 400, 200);
        ewl_object_padding_set(EWL_OBJECT(cd), 5, 5, 5, 5);

	/*
	 * Setup the picker.
	 */
	cd->picker = ewl_colorpicker_new();
	ewl_callback_append(cd->picker, EWL_CALLBACK_VALUE_CHANGED,
			    ewl_colordialog_color_valuechanged_cb, cd);
	ewl_object_fill_policy_set(EWL_OBJECT(cd->picker), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(cd), cd->picker);
	ewl_widget_show(cd->picker);

        /*
         * A preview to see the selected color, RGB values
         * HSV values, ok and cancel buttons
         */

        vbox = ewl_box_new(EWL_ORIENTATION_VERTICAL);
	ewl_box_spacing_set(EWL_BOX(vbox), 10);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
        ewl_object_alignment_set(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_CENTER);
        ewl_object_minimum_size_set(EWL_OBJECT(vbox), 60, 100);
	ewl_container_child_append(EWL_CONTAINER(cd), vbox);

        cd->preview = ewl_coloredrect_new(r, g, b, 255);
        ewl_object_fill_policy_set(EWL_OBJECT(cd->preview), EWL_FLAG_FILL_ALL);
        ewl_object_alignment_set(EWL_OBJECT(cd->preview), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(vbox), cd->preview);
	ewl_widget_show(cd->preview);

        hbox = ewl_box_new(EWL_ORIENTATION_HORIZONTAL);
        ewl_box_spacing_set(EWL_BOX(hbox), 5);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
        ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
        ewl_widget_show(hbox);

        label = ewl_text_new("R :");
        ewl_container_child_append(EWL_CONTAINER(hbox), label);
        ewl_widget_show(label);

        cd->red_entry = ewl_entry_new(NULL);
        ewl_container_child_append(EWL_CONTAINER(hbox), cd->red_entry);
	ewl_callback_append(cd->red_entry, EWL_CALLBACK_VALUE_CHANGED, ewl_colordialog_redvalue_changed, cd);
        ewl_widget_show(cd->red_entry);

        label = ewl_text_new("G :");
        ewl_container_child_append(EWL_CONTAINER(hbox), label);
        ewl_widget_show(label);

        cd->green_entry = ewl_entry_new(NULL);
        ewl_container_child_append(EWL_CONTAINER(hbox), cd->green_entry);
	ewl_callback_append(cd->green_entry, EWL_CALLBACK_VALUE_CHANGED, ewl_colordialog_greenvalue_changed, cd);
        ewl_widget_show(cd->green_entry);

        label = ewl_text_new("B :");
        ewl_container_child_append(EWL_CONTAINER(hbox), label);
        ewl_widget_show(label);

        cd->blue_entry = ewl_entry_new(NULL);
        ewl_container_child_append(EWL_CONTAINER(hbox), cd->blue_entry);
	ewl_callback_append(cd->blue_entry, EWL_CALLBACK_VALUE_CHANGED, ewl_colordialog_bluevalue_changed, cd);
        ewl_widget_show(cd->blue_entry);

        hbox = ewl_box_new(EWL_ORIENTATION_HORIZONTAL);
        ewl_box_spacing_set(EWL_BOX(hbox), 5);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
        ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
        ewl_widget_show(hbox);

        label = ewl_text_new("H :");
        ewl_container_child_append(EWL_CONTAINER(hbox), label);
        ewl_widget_show(label);

        cd->hue_entry = ewl_entry_new(NULL);
        ewl_container_child_append(EWL_CONTAINER(hbox), cd->hue_entry);
	ewl_callback_append(cd->hue_entry, EWL_CALLBACK_VALUE_CHANGED, ewl_colordialog_huevalue_changed, cd);
        ewl_widget_show(cd->hue_entry);

        label = ewl_text_new("S :");
        ewl_container_child_append(EWL_CONTAINER(hbox), label);
        ewl_widget_show(label);

        cd->saturation_entry = ewl_entry_new(NULL);
        ewl_container_child_append(EWL_CONTAINER(hbox), cd->saturation_entry);
	ewl_callback_append(cd->saturation_entry, EWL_CALLBACK_VALUE_CHANGED, ewl_colordialog_saturationvalue_changed, cd);
        ewl_widget_show(cd->saturation_entry);

        label = ewl_text_new("V :");
        ewl_container_child_append(EWL_CONTAINER(hbox), label);
        ewl_widget_show(label);

        cd->value_entry = ewl_entry_new(NULL);
        ewl_container_child_append(EWL_CONTAINER(hbox), cd->value_entry);
	ewl_callback_append(cd->value_entry, EWL_CALLBACK_VALUE_CHANGED, ewl_colordialog_valuevalue_changed, cd);
        ewl_widget_show(cd->value_entry);

        button = ewl_button_stock_with_id_new(EWL_STOCK_OK, EWL_RESPONSE_OK);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_colordialog_button_cb, cd);
	ewl_container_child_append(EWL_CONTAINER(vbox), button);
	ewl_widget_show(button);

        button = ewl_button_stock_with_id_new(EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);
        ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_colordialog_button_cb, cd);
	ewl_container_child_append(EWL_CONTAINER(vbox), button);
	ewl_widget_show(button);

	ewl_widget_show(vbox);

        struct { int r, g, b; } col;
        col.r = r; col.g = g; col.b = b;
        ewl_colordialog_color_valuechanged_cb(EWL_WIDGET(cd), &col, cd);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void ewl_colordialog_color_get(Ewl_ColorDialog *cd, int *r, int *g, int *b)
{
        *r = cd->selected.r;
        *g = cd->selected.g;
        *b = cd->selected.b;
}

void ewl_colordialog_redvalue_changed(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct { int r, g, b; } col;
        float h, s, v;
        col.r = strtol(ev_data, NULL, 10);
        col.g = cd->selected.g;
        col.b = cd->selected.b;
        if(col.r > 255)
          col.r = 255;
        if(col.r < 0)
          col.r = 0;

        // Change the base color
        ewl_spectrum_rgb_to_hsv(col.r, col.g, col.b, &h, &s, &v);
        ewl_colorpicker_hue_set(EWL_COLORPICKER(cd->picker), h);

        ewl_colordialog_color_valuechanged_cb(w, &col, cd);
}

void ewl_colordialog_greenvalue_changed(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct { int r, g, b; } col;
        float h, s, v;
        col.r = cd->selected.r;
        col.g = strtol(ev_data, NULL, 10);
        col.b = cd->selected.b;
        if(col.g > 255)
          col.g = 255;
        if(col.g < 0)
          col.g = 0;

        // Change the base color
        ewl_spectrum_rgb_to_hsv(col.r, col.g, col.b, &h, &s, &v);
        ewl_colorpicker_hue_set(EWL_COLORPICKER(cd->picker), h);

        ewl_colordialog_color_valuechanged_cb(w, &col, cd);
}

void ewl_colordialog_bluevalue_changed(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct { int r, g, b; } col;
        float h, s, v;
        col.r = cd->selected.r;
        col.g = cd->selected.g;
        col.b = strtol(ev_data, NULL, 10);
        if(col.b > 255)
          col.b = 255;
        if(col.b < 0)
          col.b = 0;

        // Change the base color
        ewl_spectrum_rgb_to_hsv(col.r, col.g, col.b, &h, &s, &v);
        ewl_colorpicker_hue_set(EWL_COLORPICKER(cd->picker), h);

        ewl_colordialog_color_valuechanged_cb(w, &col, cd);
}

void ewl_colordialog_huevalue_changed(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct { int r, g, b; } col;
        float h, s, v;
        ewl_spectrum_rgb_to_hsv(cd->selected.r, cd->selected.g, cd->selected.b, &h, &s, &v);
        h = strtod(ev_data, NULL);
        if(h > 360)
          h = 360;
        if(h < 0)
          h = 0;
        ewl_spectrum_hsv_to_rgb(h, s, v, &col.r, &col.g, &col.b);

        ewl_colorpicker_hue_set(EWL_COLORPICKER(cd->picker), h);
        ewl_colordialog_color_valuechanged_cb(w, &col, cd);
}

void ewl_colordialog_saturationvalue_changed(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct { int r, g, b; } col;
        float h, s, v;
        ewl_spectrum_rgb_to_hsv(cd->selected.r, cd->selected.g, cd->selected.b, &h, &s, &v);
        s = strtod(ev_data, NULL);
        if(s > 1)
          s = 1;
        if(s < 0)
          s = 0;
        ewl_spectrum_hsv_to_rgb(h, s, v, &col.r, &col.g, &col.b);

        ewl_colorpicker_hue_set(EWL_COLORPICKER(cd->picker), h);
        ewl_colordialog_color_valuechanged_cb(w, &col, cd);
}

void ewl_colordialog_valuevalue_changed(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct { int r, g, b; } col;
        float h, s, v;
        ewl_spectrum_rgb_to_hsv(cd->selected.r, cd->selected.g, cd->selected.b, &h, &s, &v);
        v = strtod(ev_data, NULL);
        if(v > 1)
          v = 1;
        if(v < 0)
          v = 0;
        ewl_spectrum_hsv_to_rgb(h, s, v, &col.r, &col.g, &col.b);

        ewl_colorpicker_hue_set(EWL_COLORPICKER(cd->picker), h);
        ewl_colordialog_color_valuechanged_cb(w, &col, cd);
}

void ewl_colordialog_color_valuechanged_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        struct _ewl_colordialog_color *col = ev_data;
        cd->selected.r = col->r;
        cd->selected.g = col->g;
        cd->selected.b = col->b;
        float h, s, v;
        ewl_spectrum_rgb_to_hsv(col->r, col->g, col->b, &h, &s, &v);

        // Update the entry values
        char redtext[8], greentext[8], bluetext[8];
        char huetext[8], saturationtext[8], valuetext[8];
        snprintf(redtext, 8, "%i", cd->selected.r);
        snprintf(greentext, 8, "%i", cd->selected.g);
        snprintf(bluetext, 8, "%i", cd->selected.b);
        snprintf(huetext, 8, "%.0f", h);
        snprintf(saturationtext, 8, "%.2f", s);
        snprintf(valuetext, 8, "%.2f", v);
        ewl_entry_text_set(EWL_ENTRY(cd->red_entry), redtext);
        ewl_entry_text_set(EWL_ENTRY(cd->green_entry), greentext);
        ewl_entry_text_set(EWL_ENTRY(cd->blue_entry), bluetext);
        ewl_entry_text_set(EWL_ENTRY(cd->hue_entry), huetext);
        ewl_entry_text_set(EWL_ENTRY(cd->saturation_entry), saturationtext);
        ewl_entry_text_set(EWL_ENTRY(cd->value_entry), valuetext);

        // Update the preview
        ewl_coloredrect_color_set(EWL_COLOREDRECT(cd->preview), col->r, col->g, col->b, 255);
}

void ewl_colordialog_button_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
        Ewl_ColorDialog *cd = EWL_COLORDIALOG(user_data);
        ewl_callback_call_with_event_data(EWL_WIDGET(cd), EWL_CALLBACK_VALUE_CHANGED, &EWL_BUTTON_STOCK(w)->response_id);
}

