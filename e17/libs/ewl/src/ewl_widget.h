
#ifndef __EWL_WIDGET_H__
#define __EWL_WIDGET_H__

typedef struct _ewl_widget Ewl_Widget;

#define EWL_WIDGET(widget) ((Ewl_Widget *) widget)

struct _ewl_widget {
	/*
	 * These fields are the basic information about how this widget
	 * relates to others.
	 */
	Ewl_Object      object;
	Ewl_Widget     *parent;

	/*
	 * List of callbacks for this widget 
	 */
	Ewd_List       *callbacks[EWL_CALLBACK_MAX];

	Evas_Object     fx_clip_box;

	Ebits_Object   *ebits_object;
	char           *appearance;
	int             layer;

	/*
	 * This is used to determine whether this is a container class, it
	 * does break the object heirarchy, but seems the most effective way
	 * to allow searching for children.
	 */
	int             recursive;

	/*
	 * And these are for keeping track of the appearance, behavior
	 * states of the widget and the data attached to the widget.
	 */
	Ewl_State       state;
	Ewd_Hash       *theme;
	Ewd_Hash       *data;

	/*
	 * Flags for determining the current visibility of the widget as well
	 * as if it has been realized
	 */
	Ewl_Visibility  visible;

	Ewd_List       *fx;
	Ewd_List       *fx_queues;
};

/*
 * Returns a allocated widget structure
 */
Ewl_Widget     *ewl_widget_new();

/*
 * Initialize a widget to it's default values
 */
void            ewl_widget_init(Ewl_Widget * w, char *appearance);

/*
 * Signal the widget that it's parent has changed.
 */
void            ewl_widget_reparent(Ewl_Widget * widget);

/*
 * Realize the appearance of the widget.
 */
void            _ewl_widget_realize(Ewl_Widget * widget);

#define ewl_widget_realize(w) \
	if (!REALIZED(w)) { \
		w->visible |= EWL_VISIBILITY_REALIZED; \
		ewl_callback_call(w, EWL_CALLBACK_REALIZE); \
		ewl_widget_show(w); \
	}

/*
 * Mark the widget to be displayed.
 */
void            ewl_widget_show(Ewl_Widget * widget);

/*
 * Mark the widget to be hidden.
 */
void            ewl_widget_hide(Ewl_Widget * widget);

void            ewl_widget_destroy(Ewl_Widget * widget);
void            ewl_widget_configure(Ewl_Widget * widget);
void            ewl_widget_theme_update(Ewl_Widget * w);
void            ewl_widget_set_data(Ewl_Widget * w, void *k, void *v);
void            ewl_widget_del_data(Ewl_Widget * w, void *k);
void           *ewl_widget_get_data(Ewl_Widget * w, void *k);
void            ewl_widget_update_appearance(Ewl_Widget * w, char *state);
void            ewl_widget_set_appearance(Ewl_Widget * w, char *appearance);
char           *ewl_widget_get_appearance(Ewl_Widget * w);
void            __ewl_widget_configure(Ewl_Widget * w, void *event_data,
				       void *user_data);
void            __ewl_widget_theme_update(Ewl_Widget * w, void *event_data,
					  void *user_data);
void            ewl_widget_set_parent(Ewl_Widget * w, Ewl_Widget * p);
void            ewl_widget_enable(Ewl_Widget * w);
void            ewl_widget_disable(Ewl_Widget * w);

#define RECURSIVE(w) EWL_WIDGET(w)->recursive

#define REALIZED(w) (EWL_WIDGET(w)->visible & EWL_VISIBILITY_REALIZED)
#define VISIBLE(w) (EWL_WIDGET(w)->visible & EWL_VISIBILITY_SHOWN)
#define HIDDEN(w) (EWL_WIDGET(w)->visible & EWL_VISIBILITY_HIDDEN)
#define LAYER(w) EWL_WIDGET(w)->layer

#endif /* __EWL_WIDGET_H__ */
