#ifndef __EWL_TOOLTIP_H__
#define __EWL_TOOLTIP_H__

typedef struct _ewl_tooltip Ewl_Tooltip;

#define EWL_TOOLTIP(tt) ((Ewl_Tooltip *) tt)

struct _ewl_tooltip {
	Ewl_Floater *box;   /* the box container */

	Ewl_Widget  *text;  /* the text displaying in the tooltip */

	double      delay;  /* how long before tooltip will display in secs */
	int         hide;   /* flag to enable/disable tooltip */

	Ecore_Timer *timer; /* pointer to the focus timer */
};

Ewl_Widget *ewl_tooltip_new (Ewl_Widget *parent);
void ewl_tooltip_init (Ewl_Tooltip *t, Ewl_Widget *parent);
void ewl_tooltip_set_text (Ewl_Tooltip *t, char *text);
void ewl_tooltip_set_delay (Ewl_Tooltip *t, double delay);

/*
 * Internally used callbacks, override at your own risk.
 */
int ewl_tooltip_focus_timer (void *data);
void ewl_tooltip_parent_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_tooltip_parent_focus_in(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_tooltip_parent_focus_out(Ewl_Widget * w, void *ev_data, void *user_data);

#endif        /* __EWL_FILEDIALOG_H__ */
