
#ifndef __EWL_WIDGET_H__
#define __EWL_WIDGET_H__

typedef struct _ewl_widget Ewl_Widget;

#include <ewl_container.h>

struct _ewl_widget {
	/* Object geometry etc */
	Ewl_Object object;

	/* Parent widget */
	Ewl_Widget * parent;

	/* Container for the widget, a doubly linked list
	 * with children of the widget */
	Ewl_Container container;

	/* List of callbacks for this widget */
	Ewd_List * callbacks[Ewl_Callback_Max];

	Evas evas;
	Ebits_Object ebits_object;

	Window evas_window;
};

#define EWL_WIDGET(ptr) ((Ewl_Widget *) ptr)

/* Returns a allocated widget structure */
Ewl_Widget * ewl_widget_new();
void ewl_widget_init(Ewl_Widget * _ewl_widget);
void ewl_widget_reparent(Ewl_Widget * parent, Ewl_Widget * widget);
void ewl_widget_init(Ewl_Widget * _ewl_widget);
void ewl_widget_realize(Ewl_Widget * widget);
void ewl_widget_show(Ewl_Widget * widget);
void ewl_widget_hide(Ewl_Widget * widget);
void ewl_widget_destroy(Ewl_Widget * widget);
void ewl_widget_configure(Ewl_Widget * widget);
void ewl_widget_set_ebit(Ewl_Widget * widget, char * ebit);
void ewl_widget_clip_box_create(Ewl_Widget * widget);
void ewl_widget_clip_box_resize(Ewl_Widget * widget);

#endif
