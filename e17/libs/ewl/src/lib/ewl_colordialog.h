#ifndef __EWL_COLORDIALOG_H
#define __EWL_COLORDIALOG_H

/**
 * @file ewl_colordialog.h
 * @defgroup Ewl_ColorDialog ColorDialog: A Simple Colour Dialog widget
 *
 * @{
 */

typedef struct Ewl_ColorDialog Ewl_ColorDialog;

/**
 * @def EWL_COLORDIALOG(cd)
 * Typecast a pointer to an Ewl_ColorDialog pointer.
 */
#define EWL_COLORDIALOG(cd) ((Ewl_ColorDialog *) cd)

struct Ewl_ColorDialog
{
	Ewl_Box box;
	Ewl_Widget *preview;
	Ewl_Widget *picker;
	Ewl_Widget *red_entry;
	Ewl_Widget *green_entry;
	Ewl_Widget *blue_entry;
	Ewl_Widget *hue_entry;
	Ewl_Widget *saturation_entry;
	Ewl_Widget *value_entry;
	Ewl_Color_Set selected;
};

Ewl_Widget 	*ewl_colordialog_new(int r, int g, int b);
int 		 ewl_colordialog_init(Ewl_ColorDialog *cd, int r, int g, int b);

/*
 * Internal callbacks, override at your own risk.
 */
void ewl_colordialog_redvalue_changed(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_greenvalue_changed(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_bluevalue_changed(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_huevalue_changed(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_saturationvalue_changed(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_valuevalue_changed(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_color_valuechanged_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_colordialog_button_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif

