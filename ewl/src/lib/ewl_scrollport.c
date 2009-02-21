/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_scrollport.h"
#include "ewl_scrollport_kinetic.h"
#include "ewl_scrollbar.h"
#include "ewl_range.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a new scrollport on success, NULL on failure
 * @brief Create a new scrollpane
 */
Ewl_Widget *
ewl_scrollport_new(void)
{
        Ewl_Scrollport *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Scrollport, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_scrollport_init(s))
        {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to initialize
 * @return Returns no value.
 * @brief Creates a new scrollport
 */
int
ewl_scrollport_init(Ewl_Scrollport *s)
{
        Ewl_Widget *w;
        const char *kst;
        Ewl_Kinetic_Scroll type;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        w = EWL_WIDGET(s);

        if (!ewl_container_init(EWL_CONTAINER(s)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_SCROLLPORT_TYPE);
        ewl_widget_inherit(w, EWL_SCROLLPORT_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_FILL |
                                        EWL_FLAG_FILL_SHRINK);

        s->hflag = EWL_SCROLLPORT_FLAG_AUTO_VISIBLE;
        s->vflag = EWL_SCROLLPORT_FLAG_AUTO_VISIBLE;

        s->visible_area = ewl_container_new();
        ewl_container_child_append(EWL_CONTAINER(s), s->visible_area);
        ewl_widget_internal_set(s->visible_area, TRUE);
        ewl_callback_append(s->visible_area, EWL_CALLBACK_CONFIGURE,
                        ewl_scrollport_cb_visible_area_configure, NULL);
        ewl_widget_show(s->visible_area);

        /*
         * Notify callbacks for size changes
         */
        ewl_container_show_notify_set(EWL_CONTAINER(s),
                                        ewl_scrollport_cb_child_show);
        ewl_container_resize_notify_set(EWL_CONTAINER(s),
                                        ewl_scrollport_cb_child_resize);

        /*
         * Create the scrollbars for the scrollpane.
         */
        s->hscrollbar = ewl_hscrollbar_new();
        ewl_container_child_append(EWL_CONTAINER(s), s->hscrollbar);
        ewl_widget_internal_set(s->hscrollbar, TRUE);
        ewl_widget_show(s->hscrollbar);

        s->vscrollbar = ewl_vscrollbar_new();
        ewl_widget_internal_set(s->vscrollbar, TRUE);
        ewl_container_child_append(EWL_CONTAINER(s), s->vscrollbar);
        ewl_widget_show(s->vscrollbar);

        ewl_container_redirect_set(EWL_CONTAINER(s),
                        EWL_CONTAINER(s->visible_area));

        /*
         * Append necessary callbacks for the scrollpane.
         */
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                        ewl_scrollport_cb_configure, NULL);

        /*
         * We need to know when the scrollbars have value changes in order to
         * know when to scroll.
         */
        ewl_callback_append(s->hscrollbar, EWL_CALLBACK_VALUE_CHANGED,
                                        ewl_scrollport_cb_hscroll, w);
        ewl_callback_append(s->vscrollbar, EWL_CALLBACK_VALUE_CHANGED,
                                        ewl_scrollport_cb_vscroll, w);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_WHEEL,
                                ewl_scrollport_cb_wheel_scroll, NULL);

        /*
         * Setup kinetic scrolling info here
         */
        /* XXX I think, this should be configurable and not theme defined */
        kst = ewl_theme_data_str_get(w, "kscroll_type");

        if (kst && !strcmp(kst, "embedded"))
                type = EWL_KINETIC_SCROLL_EMBEDDED;
        else if (kst && !strcmp(kst, "normal"))
                type = EWL_KINETIC_SCROLL_NORMAL;
        else
                type = EWL_KINETIC_SCROLL_NONE;
                
        ewl_scrollport_kinetic_scrolling_set(s, type);
        
        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the scrollport that contains the scrollbar to change
 * @param f: the flags to set on the horizontal scrollbar in @a s
 * @return Returns no value.
 * @brief Set flags for horizontal scrollbar
 *
 * The scrollbar flags for the horizontal scrollbar are set to @a f.
 */
void
ewl_scrollport_hscrollbar_flag_set(Ewl_Scrollport *s, Ewl_Scrollport_Flags f)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        s->hflag = f;
        ewl_widget_configure(EWL_WIDGET(s));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollport that contains the scrollbar to change
 * @param f: the flags to set on the vertical scrollbar in @a s
 * @return Returns no value.
 * @brief Set flags for vertical scrollbar
 *
 * The scrollbar flags for the vertical scrollbar are set to @a f.
 */
void
ewl_scrollport_vscrollbar_flag_set(Ewl_Scrollport *s, Ewl_Scrollport_Flags f)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        s->vflag = f;
        ewl_widget_configure(EWL_WIDGET(s));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollport that contains the scrollbar to retrieve
 * @return Returns the flags of the horizontal scrollbar, 0 on failure.
 * @brief Get flags for horizontal scrollbar
 */
Ewl_Scrollport_Flags
ewl_scrollport_hscrollbar_flag_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, 0);

        DRETURN_INT(s->hflag, DLEVEL_STABLE);
}

/**
 * @param s: the scrollport that contains the scrollbar to retrieve
 * @return Returns the flags of the vertical scrollbar on success, 0 on failure.
 * @brief Get flags for vertical scrollbar
 */
Ewl_Scrollport_Flags
ewl_scrollport_vscrollbar_flag_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, 0);

        DRETURN_INT(s->vflag, DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve it's horizontal scrollbar value
 * @return Returns the value of the horizontal scrollbar in @a s on success.
 * @brief Retrieves the value of the horizontal scrollbar in @a s.
 */
double
ewl_scrollport_hscrollbar_value_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0.0);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, 0.0);

        DRETURN_FLOAT(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->hscrollbar)),
                                                                DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve it's vertical scrollbar value
 * @return Returns the value of the vertical scrollbar in @a s on success.
 * @brief Retrieves the value of the vertical scrollbar in @a s.
 */
double
ewl_scrollport_vscrollbar_value_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0.0);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, 0.0);

        DRETURN_FLOAT(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->vscrollbar)),
                                                                DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to set the horizontal scrollbar value
 * @param val: the value to set the scrollbar too
 * @return Returns nothing
 * @brief Set the value of the horizontal scrollbar in @a s to @a val
 */
void
ewl_scrollport_hscrollbar_value_set(Ewl_Scrollport *s, double val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        ewl_scrollbar_value_set(EWL_SCROLLBAR(s->hscrollbar), val);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to set the vertical scrollbar value
 * @param val: the value to set the scrollbar too
 * @return Returns nothing
 * @brief Set the value of the vertical scrollbar in @a s to @a val
 */
void
ewl_scrollport_vscrollbar_value_set(Ewl_Scrollport *s, double val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        ewl_scrollbar_value_set(EWL_SCROLLBAR(s->vscrollbar), val);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve its vertical scrollbar stepping
 * @return Returns the value of the stepping of the vertical scrollbar
 *                in @a s on success.
 * @brief Retrieves the value of the stepping of the vertical scrollbar in @a s.
 */
double
ewl_scrollport_hscrollbar_step_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0.0);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, 0.0);

        DRETURN_FLOAT(ewl_scrollbar_step_get(EWL_SCROLLBAR(s->hscrollbar)),
                                                                DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve its vertical scrollbar stepping
 * @return Returns the value of the stepping of the vertical scrollbar
 *                in @a s on success.
 * @brief Retrieves the value of the stepping of the vertical scrollbar in @a s.
 */
double
ewl_scrollport_vscrollbar_step_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0.0);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, 0.0);

        DRETURN_FLOAT(ewl_scrollbar_step_get(EWL_SCROLLBAR(s->vscrollbar)),
                                                                DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve the vertical scrollbar from
 * @return Returns the vertical scrollbar
 * @brief Retrieves the vertical scrollbar
 */
Ewl_Widget *
ewl_scrollport_vscrollbar_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, NULL);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, NULL);

        DRETURN_PTR(s->vscrollbar, DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve the vertical scrollbar from
 * @return Returns the vertical scrollbar
 * @brief Retrieves the vertical scrollbar
 */
Ewl_Widget *
ewl_scrollport_hscrollbar_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, NULL);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, NULL);

        DRETURN_PTR(s->hscrollbar, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Move the scrollbars into correct positions
 */
void
ewl_scrollport_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Scrollport *s;
        int vs_width = 0, hs_height = 0, wadd = 0, hadd = 0;
        int content_w, content_h, b_width, b_height;
        double hstep = 1.0, vstep = 1.0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        s = EWL_SCROLLPORT(w);

        /*
         * Size of the scrollport
         */
        content_w = CURRENT_W(w);
        content_h = CURRENT_H(w);
        b_width = s->area_w;
        b_height= s->area_h;

        /*
         * Get the space needed by the scrollbars
         */
        vs_width = ewl_object_preferred_w_get(EWL_OBJECT(s->vscrollbar));
        hs_height = ewl_object_preferred_h_get(EWL_OBJECT(s->hscrollbar));

        if (VISIBLE(s->hscrollbar))
        {
                content_h -= hs_height;
                hadd = hs_height;
        }
        if (VISIBLE(s->vscrollbar))
        {
                content_w -= vs_width;
                wadd = vs_width;
        }

        if (content_w < b_width)
                hstep = (double)content_w / (double)b_width;
        if (content_h < b_height)
                vstep = (double)content_h / (double)b_height;

        /*
         * Determine visibility of scrollbars based on the flags.
         */
        if (s->hflag == EWL_SCROLLPORT_FLAG_NONE ||
                        (hstep < 1.0 &&
                         s->hflag == EWL_SCROLLPORT_FLAG_AUTO_VISIBLE))
        {
                content_h = content_h + hadd - hs_height;
                ewl_widget_show(s->hscrollbar);
        }
        else {
                ewl_widget_hide(s->hscrollbar);

                /*
                 * Adjust the scrollbar step based on visibility
                 */
                content_h += hadd;
                vstep = (double)content_h / (double)b_height;
        }

        if (s->vflag == EWL_SCROLLPORT_FLAG_NONE ||
                        (vstep < 1.0 &&
                         s->vflag == EWL_SCROLLPORT_FLAG_AUTO_VISIBLE))
        {
                content_w = content_w + wadd - vs_width;
                ewl_widget_show(s->vscrollbar);
        }
        else {
                ewl_widget_hide(s->vscrollbar);

                /*
                 * Adjust the scrollbar step based on visibility
                 */
                content_w += wadd;
                hstep = (double)content_w / (double)b_width;
        }

        b_width = (int)(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->hscrollbar))
                                        * (double)(b_width - content_w));
        b_height = (int)(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->vscrollbar))
                                        * (double)(b_height - content_h));
        
        /*
         * For some reason, tabbing around in the tree test will cause very
         * strange behaviour in the main scrollpane.  Everything ends up
         * following the right and bottom edges instead of top and left.
         * Anyways, b_width and b_height should both be >= 0
         */
        if (b_width < 0)
                b_width = 0;
        if (b_height < 0)
                b_height = 0;

        /* Assign the offset values */
        s->area_x_offset = CURRENT_X(s) - b_width;
        s->area_y_offset = CURRENT_Y(s) - b_height;

        /*
         * Assign the step values to the scrollbars to adjust scale.
         */
        ewl_scrollbar_step_set(EWL_SCROLLBAR(s->hscrollbar), hstep);
        ewl_scrollbar_step_set(EWL_SCROLLBAR(s->vscrollbar), vstep);

        /*
         * Position the horizontal scrollbar.
         */
        ewl_object_geometry_request(EWL_OBJECT(s->hscrollbar),
                                        CURRENT_X(w), CURRENT_Y(w) + content_h,
                                        content_w, hs_height);
        
        /*
         * Position the vertical scrollbar.
         */
        ewl_object_geometry_request(EWL_OBJECT(s->vscrollbar),
                                        CURRENT_X(w) + content_w, CURRENT_Y(w),
                                        vs_width, content_h);

        /*
         * Now move the box into position. For the scrollpane to work we move
         * the box relative to the scroll value.
         */
        ewl_object_geometry_request(EWL_OBJECT(s->visible_area),
                                        CURRENT_X(w), CURRENT_Y(w),
                                        content_w, content_h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 */
void
ewl_scrollport_cb_visible_area_configure(Ewl_Widget *w,
                        void *ev_data __UNUSED__, void *user_data __UNUSED__)
{
        Ewl_Container *c;
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        c = EWL_CONTAINER(w);

        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)))
                ewl_widget_configure(child);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to get the geometry from
 * @param x: the current x offset of the content
 * @param y: the current y offset of the content
 * @param w: used to return the width
 * @param h: used to return the height
 * @Return Returns no value
 * @brief Gets the geometry used to calculate the scrollbar sizes
 */
void
ewl_scrollport_area_geometry_get(Ewl_Scrollport *s, int *x, int *y, int *w, int *h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        if (x) *x = s->area_x_offset;
        if (y) *y = s->area_y_offset;
        if (w) *w = s->area_w;
        if (h) *h = s->area_h;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollport to get the geometry from
 * @param x: Used to return the x offset
 * @param y: Used to return the y offset
 * @param w: Used to return the width
 * @param h: Used to return the height
 * @Return Returns no value
 * @brief Gets the geometry of the visible area
 */
void
ewl_scrollport_visible_area_geometry_get(Ewl_Scrollport *s, int *x, int *y,
                                                                int *w, int *h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        if (x) *x = CURRENT_X(s->visible_area); 
        if (y) *y = CURRENT_Y(s->visible_area);
        if (w) *w = CURRENT_W(s->visible_area);
        if (h) *h = CURRENT_H(s->visible_area);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollport to set the geometry of
 * @param w: The content width
 * @param h: The content height
 * @Return Returns no value
 * @brief Sets the content geometry
 */
void
ewl_scrollport_area_size_set(Ewl_Scrollport *s, int w, int h)
{
        int hs_w = 0, hs_h = 0, vs_w = 0, vs_h = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        if (s->hflag != EWL_SCROLLPORT_FLAG_ALWAYS_HIDDEN)
                ewl_object_preferred_size_get(EWL_OBJECT(s->hscrollbar), 
                                &hs_w, &hs_h);

        if (s->vflag != EWL_SCROLLPORT_FLAG_ALWAYS_HIDDEN)
                ewl_object_preferred_size_get(EWL_OBJECT(s->vscrollbar), 
                                &vs_w, &vs_h);
        s->area_w = w;
        s->area_h = h;
        ewl_object_minimum_h_set(EWL_OBJECT(s), hs_h + vs_h);
        ewl_object_preferred_inner_h_set(EWL_OBJECT(s), vs_h + h);
        ewl_object_minimum_w_set(EWL_OBJECT(s), hs_w + vs_w);
        ewl_object_preferred_inner_w_set(EWL_OBJECT(s), vs_w + w);
        /* configure to adjust the scrollbars */
        ewl_widget_configure(EWL_WIDGET(s));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollport to set the visible geometry of
 * @param x: The horizontal offset of the visible content
 * @param y: The vertical offset of the visible content
 * @Return Returns no value
 * @brief Sets the visible current geometry
 */
void
ewl_scrollport_visible_area_geometry_set(Ewl_Scrollport *s, int x, int y,
                                                                int w, int h)
{
        int endcoord = 0;
        double value = 0.0;
        Ewl_Widget *bar = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLPORT_TYPE);

        /*
         * Adjust horizontally to show the focused widget
         */
        if (x < CURRENT_X(s->visible_area))
        {
                bar = s->hscrollbar;
                endcoord = x;
        }
        else if (x + w > CURRENT_X(s->visible_area) 
                        + CURRENT_W(s->visible_area))
        {
                bar = s->hscrollbar;
                endcoord = x + w;
        }

        if (bar)
        {
                value = (double)endcoord /
                        (double)s->area_x_offset + s->area_w;
                ewl_scrollbar_value_set(EWL_SCROLLBAR(bar), value);
                bar = NULL;
        }

        /*
         * Adjust vertically to show the focused widget
         */
        if (y < CURRENT_Y(s->visible_area))
        {
                bar = s->vscrollbar;
                endcoord = y;
        }
        else if (y+ h > CURRENT_Y(s->visible_area) +
                        CURRENT_H(s->visible_area))
        {
                bar = s->vscrollbar;
                endcoord = y + h;
        }

        /*
         * Adjust the value of the scrollbar to jump to the position
         */
        if (bar)
        {
                value = (double)endcoord /
                        (double)s->area_y_offset + s->area_h;
                ewl_scrollbar_value_set(EWL_SCROLLBAR(bar), value);
        }

        ewl_widget_configure(EWL_WIDGET(s));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollport to retrieve its visible area container
 * @return Returns the container holding the children added to the scrollport.
 * @brief Retrieves the visible area container
 *
 * This function returns the container, that holds the children added to the
 * scrollport. This is a plain container, so you can add the container
 * callbacks to keep track of children size changes. You must not change the
 * position and geometry of it.
 */
Ewl_Container *
ewl_scrollport_visible_area_get(Ewl_Scrollport *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, NULL);
        DCHECK_TYPE_RET(s, EWL_SCROLLPORT_TYPE, NULL);

        DRETURN_PTR(EWL_CONTAINER(s->visible_area), DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The scrollbar
 * @return Returns no value
 * @brief When a horizontal scrollbar is clicked we to notify the users
 */
void
ewl_scrollport_cb_hscroll(Ewl_Widget *w __UNUSED__,
                void *ev_data __UNUSED__, void *user_data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_SCROLLPORT_TYPE);

        ewl_callback_call(user_data, EWL_CALLBACK_VALUE_CHANGED);
        ewl_widget_configure(user_data);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 */
/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The scrollbar
 * @return Returns no value
 * @brief When a vertical scrollbar is clicked we need to notify the users
 */
void
ewl_scrollport_cb_vscroll(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                                void *user_data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_SCROLLPORT_TYPE);

        ewl_callback_call(user_data, EWL_CALLBACK_VALUE_CHANGED);
        ewl_widget_configure(user_data);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param cb: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Wheel data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The wheel scroll callback
 */
void
ewl_scrollport_cb_wheel_scroll(Ewl_Widget *cb, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Scrollport *s;
        Ewl_Event_Mouse_Wheel *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cb);
        DCHECK_TYPE(cb, EWL_WIDGET_TYPE);

        s = EWL_SCROLLPORT(cb);
        ev = ev_data;
        ewl_scrollport_vscrollbar_value_set(s,
                        ewl_scrollport_vscrollbar_value_get(s) +
                        ev->z * ewl_scrollport_vscrollbar_step_get(s));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_scrollport_cb_child_show(Ewl_Container *p, Ewl_Widget *c)
{
        Ewl_Scrollport *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_SCROLLPORT_TYPE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_WIDGET_TYPE);

        s = EWL_SCROLLPORT(p);
        /* we don't change the size here actually but the function does
         * everything we need to do here, i.e. reseting the preferred
         * and the minimum size */
        ewl_scrollport_area_size_set(s, s->area_w, s->area_h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_scrollport_cb_child_resize(Ewl_Container *p, Ewl_Widget *c, 
                                                int size, Ewl_Orientation o)
{
        Ewl_Scrollport *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_WIDGET_TYPE);
        DCHECK_TYPE(p, EWL_SCROLLPORT_TYPE);

        s = EWL_SCROLLPORT(p);

        if (o == EWL_ORIENTATION_VERTICAL)
                ewl_object_preferred_inner_h_set(EWL_OBJECT(s),
                                PREFERRED_H(s) + size);
        else
                ewl_object_preferred_inner_w_set(EWL_OBJECT(s),
                                PREFERRED_W(s) + size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

