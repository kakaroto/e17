#ifndef __EWL_TOOLTIP_H__
#define __EWL_TOOLTIP_H__

typedef struct _ewl_tooltip Ewl_Tooltip;

#define EWL_TOOLTIP(fs) ((Ewl_Tooltip *) tt)

struct _ewl_tooltip {
	Ewl_Floater *box; /* the box container */

	int         hide; /* flag to enable/disable tooltip */
};

Ewl_Widget *ewl_tooltip_new (Ewl_Widget *parent);
void ewl_tooltip_init (Ewl_Tooltip *t, Ewl_Widget *parent);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_tooltip_parent_focus_in(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_tooltip_parent_focus_out(Ewl_Widget * w, void *ev_data, void *user_data);

#endif        /* __EWL_FILEDIALOG_H__ */
