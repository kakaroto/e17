#ifndef __EWL_SCROLLPANE_H__
#define __EWL_SCROLLPANE_H__

/**
 * @defgroup Ewl_ScrollPane ScrollPane: A Scrollable Viewing Area
 * Provides a scrollable area for viewing large sets of widgets in a smaller
 * viewable region.
 *
 * @{
 */

/**
 * @themekey /scrollpane/file
 * @themekey /scrollpane/group
 */

/**
 * The scrollpane provides a way to pan around large collections of images.
 */
typedef struct Ewl_ScrollPane Ewl_ScrollPane;

/**
 * @def EWL_SCROLLPANE(scrollpane)
 * Typecasts a pointer to an Ewl_ScrollPane pointer.
 */
#define EWL_SCROLLPANE(scrollpane) ((Ewl_ScrollPane *) scrollpane)

/**
 * @struct Ewl_ScrollPane
 * Inherits from Ewl_Container and extends it to enclose a single widget in
 * vertical and horizontal scrollbars for panning around displaying different
 * regions of the enclosed widget.
 */
struct Ewl_ScrollPane
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */

	Ewl_Widget     *box; /**< The area displaying the enclosed widget */
	Ewl_Widget     *hscrollbar; /**< Horizontal scrollbar */
	Ewl_Widget     *vscrollbar; /**< Vertical scrollbar */
};

Ewl_Widget     *ewl_scrollpane_new(void);
int             ewl_scrollpane_init(Ewl_ScrollPane * s);

void            ewl_scrollpane_set_hscrollbar_flag(Ewl_ScrollPane * s,
						   Ewl_ScrollBar_Flags f);
void            ewl_scrollpane_set_vscrollbar_flag(Ewl_ScrollPane * s,
						   Ewl_ScrollBar_Flags f);

Ewl_ScrollBar_Flags ewl_scrollpane_get_hscrollbar_flag(Ewl_ScrollPane * s);
Ewl_ScrollBar_Flags ewl_scrollpane_get_vscrollbar_flag(Ewl_ScrollPane * s);

double          ewl_scrollpane_get_hscrollbar_value(Ewl_ScrollPane *s);
double          ewl_scrollpane_get_vscrollbar_value(Ewl_ScrollPane *s);

void            ewl_scrollpane_set_hscrollbar_value(Ewl_ScrollPane *s, double val);
void            ewl_scrollpane_set_vscrollbar_value(Ewl_ScrollPane *s, double val);

double          ewl_scrollpane_get_hscrollbar_step(Ewl_ScrollPane *s);
double          ewl_scrollpane_get_vscrollbar_step(Ewl_ScrollPane *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_scrollpane_configure_cb(Ewl_Widget * w, void *ev_data,
					    void *user_data);
void            ewl_scrollpane_hscroll_cb(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            ewl_scrollpane_vscroll_cb(Ewl_Widget * w, void *ev_data,
					  void *user_data);

void            ewl_scrollpane_child_show_cb(Ewl_Container * parent,
				      Ewl_Widget * child);
void            ewl_scrollpane_child_resize_cb(Ewl_Container * parent,
					       Ewl_Widget * child, int size,
					       Ewl_Orientation o);

/**
 * @}
 */

#endif				/* __EWL_SCROLLPANE_H__ */
