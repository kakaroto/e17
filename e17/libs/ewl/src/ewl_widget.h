
#ifndef __EWL_WIDGET_H
#define __EWL_WIDGET_H

typedef struct _ewl_widget Ewl_Widget;

struct _ewl_widget {
	/*
	 * These fields are the basic information about how this widget
	 * relates to others.
	 */
	Ewl_Object object;
	Ewl_Widget *parent;

	/*
	 * List of callbacks for this widget 
	 */
	Ewd_List *callbacks[EWL_CALLBACK_MAX];

	/*
	 * The following fields allow for drawing the widget and its children
	 */
	Evas evas;
	Window evas_window;
	Evas_Object fx_clip_box;
	Ebits_Object *ebits_object;
	int recursive;

	/*
	 * And these are for keeping track of the appearance and behavior
	 * states of the widget.
	 */
	Ewl_State state;
	Ewd_Hash *theme;
	Ewd_Hash *data;
};

#define EWL_WIDGET(widget) ((Ewl_Widget *) widget)

/* Returns a allocated widget structure */
Ewl_Widget *ewl_widget_new();
void ewl_widget_init(Ewl_Widget * w, int req_w, int req_h,
		     Ewl_Fill_Policy fill, Ewl_Alignment align);
void ewl_widget_reparent(Ewl_Widget * parent, Ewl_Widget * widget);
void ewl_widget_realize(Ewl_Widget * widget);
void ewl_widget_show(Ewl_Widget * widget);
void ewl_widget_hide(Ewl_Widget * widget);
void ewl_widget_destroy(Ewl_Widget * widget);
void ewl_widget_destroy_recursive(Ewl_Widget * widget);
void ewl_widget_configure(Ewl_Widget * widget);
void ewl_widget_theme_update(Ewl_Widget * w);
void ewl_widget_set_data(Ewl_Widget * w, char *k, void *v);
void ewl_widget_del_data(Ewl_Widget * w, char *k);
void *ewl_widget_get_data(Ewl_Widget * w, char *k);

#endif
