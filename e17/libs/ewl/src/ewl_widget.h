
#ifndef __EWL_WIDGET_H
#define __EWL_WIDGET_H

typedef struct _ewl_widget Ewl_Widget;
typedef enum _ewl_widget_type Ewl_Widget_Type;

enum _ewl_widget_type
{
	EWL_WIDGET_WINDOW,
	EWL_WIDGET_BOX,
	EWL_WIDGET_BUTTON,
	EWL_WIDGET_LABEL_BUTTON,
	EWL_WIDGET_CHECK_BUTTON,
	EWL_WIDGET_RADIO_BUTTON,
	EWL_WIDGET_ENTRY,
	EWL_WIDGET_TABLE,
	EWL_WIDGET_SEEKER,
	EWL_WIDGET_SPINNER,
	EWL_WIDGET_LIST,
	EWL_WIDGET_TEXT,
	EWL_WIDGET_IMAGE
};

struct _ewl_widget
{
	/*
	 * These fields are the basic information about how this widget
	 * relates to others.
	 */
	Ewl_Object object;
	Ewl_Widget *parent;
	Ewl_Widget_Type type;

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
void
ewl_widget_init(Ewl_Widget * w, int type, int req_w, int req_h,
		int max_w, int max_h);
void ewl_widget_reparent(Ewl_Widget * parent, Ewl_Widget * widget);
void ewl_widget_realize(Ewl_Widget * widget);
void ewl_widget_show(Ewl_Widget * widget);
void ewl_widget_hide(Ewl_Widget * widget);
void ewl_widget_destroy(Ewl_Widget * widget);
void ewl_widget_destroy_recursive(Ewl_Widget * widget);
void ewl_widget_configure(Ewl_Widget * widget);
void ewl_widget_theme_update(Ewl_Widget * w);
void ewl_widget_set_data(Ewl_Widget * w, char * k, void * v);
void ewl_widget_del_data(Ewl_Widget * w, char * k);
void * ewl_widget_get_data(Ewl_Widget * w, char * k);

#endif
