#ifndef __EWL_WIDGET_H__
#define __EWL_WIDGET_H__

typedef struct _ewl_widget Ewl_Widget;

#define EWL_WIDGET(widget) ((Ewl_Widget *) widget)

struct _ewl_widget
{
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

	Evas_Object    *fx_clip_box;

	Ebits_Object   *ebits_object;
	char           *bit_state;
	char           *appearance;
	int             layer;

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
	Ewl_Widget_Flags       flags;
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
void            ewl_widget_realize(Ewl_Widget * widget);

/*
 * Unrealize the appearance of the widget.
 */
void            ewl_widget_unrealize(Ewl_Widget * w);

/*
 * Mark the widget to be displayed.
 */
void            ewl_widget_show(Ewl_Widget * widget);

/*
 * Mark the widget to be hidden.
 */
void            ewl_widget_hide(Ewl_Widget * widget);

/*
 * Free the widget and all ot its contained data.
 */
void            ewl_widget_destroy(Ewl_Widget * widget);

/*
 * Queue the widget to be configured.
 */
void            ewl_widget_configure(Ewl_Widget * widget);

/*
 * Update the widget's appearance based on it's theme data.
 */
void            ewl_widget_theme_update(Ewl_Widget * w);

/*
 * Attach a key/value pair to a widget.
 */
void            ewl_widget_set_data(Ewl_Widget * w, void *k, void *v);

/*
 * Remove a key value pair from a widget.
 */
void            ewl_widget_del_data(Ewl_Widget * w, void *k);

/*
 * Retrieve a key value pair from a widget.
 */
void           *ewl_widget_get_data(Ewl_Widget * w, void *k);

/*
 * Change the appearance of a widget based on a state string.
 */
void            ewl_widget_update_appearance(Ewl_Widget * w, char *state);

/*
 * Change the appearance string used for determining the correct theme data.
 */
void            ewl_widget_set_appearance(Ewl_Widget * w, char *appearance);

/*
 * Retrieve the appearance string of a widget.
 */
char           *ewl_widget_get_appearance(Ewl_Widget * w);

/*
 * Change the parent of a widget.
 */
void            ewl_widget_set_parent(Ewl_Widget * w, Ewl_Widget * p);

/*
 * Activate a widget.
 */
void            ewl_widget_enable(Ewl_Widget * w);

/*
 * Deactivate a widget.
 */
void            ewl_widget_disable(Ewl_Widget * w);

/*
 * Notify a widget to rebuild it's appearance string.
 */
void            ewl_widget_rebuild_appearance(Ewl_Widget *w);

#define RECURSIVE(w) (EWL_WIDGET(w)->flags & EWL_FLAGS_RECURSIVE)

#define REALIZED(w) (EWL_WIDGET(w)->flags & EWL_FLAGS_REALIZED)
#define VISIBLE(w) (EWL_WIDGET(w)->flags & EWL_FLAGS_SHOWN)
#define OBSCURED(w) (EWL_WIDGET(w)->flags & EWL_FLAGS_OBSCURED)
#define HIDDEN(w) (!(EWL_WIDGET(w)->flags & EWL_FLAGS_SHOWN))
#define LAYER(w) EWL_WIDGET(w)->layer

#endif				/* __EWL_WIDGET_H__ */
