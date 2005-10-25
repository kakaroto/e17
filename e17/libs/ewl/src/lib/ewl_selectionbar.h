
#ifndef __EWL_SELECTIONBAR_H__
#define __EWL_SELECTIONBAR_H__

/**
 * @file ewl_selectionbar.h
 * @defgroup Ewl_Selectionbar The selectionbar widget
 * @{
 */

/**
 * @themekey /selectionbar/file
 * @themekey /selectionbar/group
 */

typedef struct _ewl_selectionbar Ewl_Selectionbar;

#define EWL_SELECTIONBAR(selectionbar) ((Ewl_Selectionbar *) selectionbar)

struct _ewl_selectionbar {

	Ewl_Floater     floater;

	Ewl_Container  *bar;

	struct {
		Ewl_Widget     *top;
		Ewl_Widget     *bottom;
	} scroller;


	int             h, w;
	int             mouse_x;	/* store old mouse x position for animation */
	int             OPEN;	/* Boolean, open or closed */
};

Ewl_Widget *ewl_selectionbar_new(Ewl_Widget *parent);
int         ewl_selectionbar_init(Ewl_Selectionbar *s, Ewl_Widget *parent);
void        ewl_selectionbar_size_set(Ewl_Selectionbar *s, int w, int h1,
								     int h2);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_selectionbar_realize_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_selectionbar_show_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_selectionbar_configure_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_selectionbar_parent_configure_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_selectionbar_focus_in_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_selectionbar_focus_out_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_selectionbar_children_animator_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_selectionbar_mouse_move_child_cb(Ewl_Widget *w, void *user_data,
						void *ev_data);

void ewl_selectionbar_child_add_cb(Ewl_Container *parent, Ewl_Widget *child);

/**
 * @}
 */

#endif				/* __EWL_SELECTIONBAR_H__ */
