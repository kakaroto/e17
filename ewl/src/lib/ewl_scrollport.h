/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SCROLLPORT_H
#define EWL_SCROLLPORT_H

/**
 * @addtogroup Ewl_Scrollport Ewl_Scrollport: A Scrollable Viewing Area
 * Provides a framework for viewing large sets of widgets in a smaller viewable
 * region.  For most uses, Ewl_Scrollpane will be easier to work with.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Scrollport_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /scrollport/file
 * @themekey /scrollport/group
 */

/**
 * @def EWL_SCROLLPORT_TYPE
 * The type name for the Ewl_Scrollport widget
 */
#define EWL_SCROLLPORT_TYPE "scrollport"

/**
 * @def EWL_SCROLLPORT_IS(w)
 * Returns TRUE if the widget is an Ewl_Scrollport, FALSE otherwise
 */
#define EWL_SCROLLPORT_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SCROLLPORT_TYPE))

/**
 * The scrollport provides a framework for panning around larger widgets.
 */
typedef struct Ewl_Scrollport Ewl_Scrollport;

/**
 * @def EWL_SCROLLPORT(scrollport)
 * Typecasts a pointer to an Ewl_Scrollport pointer.
 */
#define EWL_SCROLLPORT(scrollport) ((Ewl_Scrollport *) scrollport)

/**
 * @brief Inherits from Ewl_Container and extends it to provide a framework
 * for panning around larger widgets.  Vertical and horizontal scrollbars are
 * provided, but they do not move the enclosed box.  It the the job of the user
 * to set the sizes used in this widget.
 */
struct Ewl_Scrollport
{
        Ewl_Container container;        /**< Inherit from Ewl_Container */

        Ewl_Widget *visible_area;       /**< Clips the enclosed widget */
        Ewl_Widget *hscrollbar;         /**< Horizontal scrollbar */
        Ewl_Widget *vscrollbar;         /**< Vertical scrollbar */
        Ewl_Scrollport_Flags hflag;     /**< Flags for horizontal scrollbar */
        Ewl_Scrollport_Flags vflag;     /**< Flags for vertical scrollbar */

	int area_w, area_h;
        int area_x_offset, area_y_offset;
};

Ewl_Widget		*ewl_scrollport_new(void);
int			 ewl_scrollport_init(Ewl_Scrollport *s);
Ewl_Scrollport_Flags     ewl_scrollport_hscrollbar_flag_get(Ewl_Scrollport *s);
Ewl_Scrollport_Flags     ewl_scrollport_vscrollbar_flag_get(Ewl_Scrollport *s);
void                     ewl_scrollport_hscrollbar_flag_set(Ewl_Scrollport *s,
                                                        Ewl_Scrollport_Flags f);
void                     ewl_scrollport_vscrollbar_flag_set(Ewl_Scrollport *s,
                                                        Ewl_Scrollport_Flags f);

double                   ewl_scrollport_hscrollbar_value_get(Ewl_Scrollport *s);
double                   ewl_scrollport_vscrollbar_value_get(Ewl_Scrollport *s);

void                     ewl_scrollport_hscrollbar_value_set(Ewl_Scrollport *s,
								double val);
void                     ewl_scrollport_vscrollbar_value_set(Ewl_Scrollport *s,
								double val);
Ewl_Widget              *ewl_scrollport_hscrollbar_get(Ewl_Scrollport *s);
Ewl_Widget              *ewl_scrollport_vscrollbar_get(Ewl_Scrollport *s);

double                   ewl_scrollport_hscrollbar_step_get(Ewl_Scrollport *s);
double                   ewl_scrollport_vscrollbar_step_get(Ewl_Scrollport *s);

void			 ewl_scrollport_area_geometry_get(Ewl_Scrollport *s,
                                                         int *x, int *y,
							 int *w, int *h);
void			 ewl_scrollport_visible_area_geometry_get
							(Ewl_Scrollport *s,
							 int *x, int *y,
							 int *w, int *h);
void			 ewl_scrollport_area_size_set(Ewl_Scrollport *s,
                                                         int w, int h);
void			 ewl_scrollport_visible_area_geometry_set
							(Ewl_Scrollport *s,
							 int x, int y,
                                                         int w, int h);
Ewl_Container           *ewl_scrollport_visible_area_get(Ewl_Scrollport *s);

void ewl_scrollport_cb_hscroll(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_scrollport_cb_vscroll(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_scrollport_cb_wheel_scroll(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
void ewl_scrollport_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_scrollport_cb_visible_area_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_scrollport_cb_child_show(Ewl_Container *p, Ewl_Widget *c);
void ewl_scrollport_cb_child_resize(Ewl_Container *p, Ewl_Widget *c, 
                                                int size, Ewl_Orientation o);

#endif
