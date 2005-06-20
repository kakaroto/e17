#ifndef __EWL_COLORPICKER_H
#define __EWL_COLORPICKER_H

/**
 * @file ewl_colorpicker.h
 * @defgroup Ewl_ColorPicker ColorPicker: A Simple Colour Picker widget
 *
 * @{
 */

typedef struct Ewl_ColorPicker Ewl_ColorPicker;

/**
 * @def EWL_COLORPICKER(cp)
 * Typecast a pointer to an Ewl_ColorPicker pointer.
 */
#define EWL_COLORPICKER(cp) ((Ewl_ColorPicker *) cp)

struct Ewl_ColorPicker
{
	Ewl_Box box;
	Ewl_Widget *spectrum;
	Ewl_Widget *range;
	int drag;
        Ewl_Color_Set selected;
};

Ewl_Widget 	*ewl_colorpicker_new();
int 		ewl_colorpicker_init(Ewl_ColorPicker *cp);
void            ewl_colorpicker_color_set(Ewl_ColorPicker *cp, int r, int g, int b);
void ewl_colorpicker_hue_set(Ewl_ColorPicker *cp, float h);

/*
 * Internal callbacks, override at your own risk.
 */
void ewl_colorpicker_range_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_range_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_range_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_spectrum_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_spectrum_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_spectrum_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
