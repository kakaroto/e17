
#ifndef __EWL_WIDGET_H__
#define __EWL_WIDGET_H__

typedef struct _ewl_widget Ewl_Widget;
typedef enum _ewl_widget_type Ewl_Widget_Type;

#include <ewl_container.h>

enum _ewl_widget_type {
    Ewl_Widget_Window,
    Ewl_Widget_Box,
    Ewl_Widget_Button,
    Ewl_Widget_Entry,
    Ewl_Widget_Table
};

struct _ewl_widget {
	Ewl_Object object;

	Ewl_Widget * parent;

	Ewl_Widget_Type type;

	Ewl_Container container;

	/* List of callbacks for this widget */
	Ewd_List * callbacks[Ewl_Callback_Max];

	Evas evas;
	Ebits_Object ebits_object;
	Window evas_window;
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
void ewl_widget_set_ebit(Ewl_Widget * widget, char * ebit);

#endif
