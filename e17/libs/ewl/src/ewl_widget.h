
#ifndef __EWL_WIDGET_H
#define __EWL_WIDGET_H

typedef struct _ewl_widget Ewl_Widget;
typedef enum _ewl_widget_type Ewl_Widget_Type;

#include <ewl_container.h>

enum _ewl_widget_type {
    EWL_WIDGET_WINDOW,
    EWL_WIDGET_BOX,
    EWL_WIDGET_BUTTON,
    EWL_WIDGET_ENTRY,
    EWL_WIDGET_TABLE,
	EWL_WIDGET_SEEKER,
	EWL_WIDGET_SPINNER
};

struct _ewl_widget {
	Ewl_Object object;

	Ewl_Widget * parent;

	Ewl_Widget_Type type;

	Ewl_Container container;

	/* List of callbacks for this widget */
	Ewd_List * callbacks[EWL_CALLBACK_MAX];

	Evas evas;
	Evas_Object fx_clip_box; /* use to cover up whole widget for FX */
	Window evas_window;

	Ewl_State state;
};

#define EWL_WIDGET(widget) ((Ewl_Widget *) widget)

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

#endif
