#ifndef __EWL_COLORPICKER_H
#define __EWL_COLORPICKER_H

typedef struct Ewl_ColorPicker Ewl_ColorPicker;

struct Ewl_ColorPicker
{
	Ewl_Box box;
	Ewl_Widget *spectrum;
	Ewl_Widget *range;
	int drag;
};

Ewl_Widget *ewl_colorpicker_new();
int ewl_colorpicker_init(Ewl_ColorPicker *cp);

/*
 * Internal callbacks, override at your own risk.
 */
void ewl_colorpicker_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colorpicker_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);

#endif
