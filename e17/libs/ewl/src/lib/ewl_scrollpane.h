/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_SCROLLPANE_H
#define EWL_SCROLLPANE_H

/**
 * @addtogroup Ewl_Scrollpane Ewl_Scrollpane: A Scrollable Viewing Area
 * Provides a scrollable area for viewing large sets of widgets in a smaller
 * viewable region.
 *
 * @remarks Inherits from Ewl_Container.
 * @image html Ewl_Scrollpane_inheritance.png
 *
 * @{
 */

/**
 * @themekey /scrollpane/file
 * @themekey /scrollpane/group
 */

/**
 * @def EWL_SCROLLPANE_TYPE
 * The type name for the Ewl_Scrollpane widget
 */
#define EWL_SCROLLPANE_TYPE "scrollpane"

/**
 * @def EWL_SCROLLPANE_IS(w)
 * Returns TRUE if the widget is an Ewl_Scrollpane, FALSE otherwise
 */
#define EWL_SCROLLPANE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SCROLLPANE_TYPE))

/**
 * The scrollpane provides a way to pan around large collections of images.
 */
typedef struct Ewl_Scrollpane Ewl_Scrollpane;

/**
 * @def EWL_SCROLLPANE(scrollpane)
 * Typecasts a pointer to an Ewl_Scrollpane pointer.
 */
#define EWL_SCROLLPANE(scrollpane) ((Ewl_Scrollpane *) scrollpane)

/**
 * Inherits from Ewl_Container and extends it to enclose a single widget in
 * vertical and horizontal scrollbars for panning around displaying different
 * regions of the enclosed widget.
 */
struct Ewl_Scrollpane
{
	Ewl_Container container;  /**< Inherit from Ewl_Container */

	Ewl_Widget *overlay; 	/**< Clips the enclosed widget */
	Ewl_Widget *box;  	/**< Lays out enclosed widget */
	Ewl_Widget *hscrollbar; /**< Horizontal scrollbar */
	Ewl_Widget *vscrollbar; /**< Vertical scrollbar */
	Ewl_Scrollpane_Flags hflag;      /**< Flags for horizontal scrollbar */
	Ewl_Scrollpane_Flags vflag;      /**< Flags for vertical scrollbar */
};

Ewl_Widget 	*ewl_scrollpane_new(void);
int 		 ewl_scrollpane_init(Ewl_Scrollpane *s);

void 		 ewl_scrollpane_hscrollbar_flag_set(Ewl_Scrollpane *s,
						   Ewl_Scrollpane_Flags f);
void 		 ewl_scrollpane_vscrollbar_flag_set(Ewl_Scrollpane *s,
						   Ewl_Scrollpane_Flags f);

Ewl_Scrollpane_Flags ewl_scrollpane_hscrollbar_flag_get(Ewl_Scrollpane *s);
Ewl_Scrollpane_Flags ewl_scrollpane_vscrollbar_flag_get(Ewl_Scrollpane *s);

double 		 ewl_scrollpane_hscrollbar_value_get(Ewl_Scrollpane *s);
double 		 ewl_scrollpane_vscrollbar_value_get(Ewl_Scrollpane *s);

void 		 ewl_scrollpane_hscrollbar_value_set(Ewl_Scrollpane *s, double val);
void 		 ewl_scrollpane_vscrollbar_value_set(Ewl_Scrollpane *s, double val);

double 		 ewl_scrollpane_hscrollbar_step_get(Ewl_Scrollpane *s);
double 		 ewl_scrollpane_vscrollbar_step_get(Ewl_Scrollpane *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_scrollpane_cb_configure(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_scrollpane_cb_focus_jump(Ewl_Widget *w, void *ev_data,
 						void *user_data);
void ewl_scrollpane_cb_hscroll(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_scrollpane_cb_vscroll(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_scrollpane_cb_wheel_scroll(Ewl_Widget *w, void *ev_data,
							void *user_data);

void ewl_scrollpane_cb_child_resize(Ewl_Container *parent, Ewl_Widget *child);

/**
 * @}
 */

#endif
