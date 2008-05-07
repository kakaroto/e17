/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_overlay.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a new overlay container on success, or NULL on failure.
 * @brief Allocate and initialize a new overlay container
 */
Ewl_Widget *
ewl_overlay_new(void)
{
        Ewl_Overlay *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Overlay, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_overlay_init(w)) {
                ewl_widget_destroy(EWL_WIDGET(w));
                w = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(w), DLEVEL_STABLE);
}

/**
 * @param w: the overlay to be initialized to default values and callbacks
 * @return Returns TRUE or FALSE depending on if initialization succeeds.
 * @brief initialize a overlay to default values and callbacks
 *
 * Sets the values and callbacks of a overlay @a w to their defaults.
 */
int
ewl_overlay_init(Ewl_Overlay *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);

        /*
         * Initialize the fields of the inherited container class
         */
        if (!ewl_container_init(EWL_CONTAINER(w)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(w), EWL_OVERLAY_TYPE);
        ewl_widget_inherit(EWL_WIDGET(w), EWL_OVERLAY_TYPE);

        ewl_container_show_notify_set(EWL_CONTAINER(w),
                                        ewl_overlay_cb_child_show);
        ewl_container_resize_notify_set(EWL_CONTAINER(w),
                                        ewl_overlay_cb_child_resize);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);

        /*
         * Override the default configure callbacks since the overlay
         * has special needs for placement.
         */
        ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
                             ewl_overlay_cb_configure, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_overlay_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Object *o;
        Ewl_Object *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_OVERLAY_TYPE);

        o = EWL_OBJECT(w);

        /*
         * Configure each of the child widgets.
         */
        ecore_dlist_first_goto(EWL_CONTAINER(w)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children))) {
                int width, height;
                /* ignore unmanaged widgets */
                if (UNMANAGED(child))
                        continue;
                /*
                 * Try to give the child the full size of the overlay from it's
                 * base position. The object will constrict it based on the
                 * fill policy. Don't add the TOP and LEFT insets since
                 * they've already been accounted for.
                 */
                width = CURRENT_W(w) + CURRENT_X(w);
                       width -= ewl_object_current_x_get(child);
                height = CURRENT_H(w) + CURRENT_Y(w);
                height -= ewl_object_current_y_get(child);
                ewl_object_size_request(child, width, height);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param o: The container to work with
 * @param child: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_overlay_cb_child_show(Ewl_Container *o, Ewl_Widget *child)
{
        int size;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(o);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(o, EWL_OVERLAY_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        size = ewl_object_current_x_get(EWL_OBJECT(child));
               size += ewl_object_preferred_w_get(EWL_OBJECT(child));
               size -= CURRENT_X(o);
        if (size > PREFERRED_W(o))
                ewl_object_preferred_inner_w_set(EWL_OBJECT(o), size);

        size = ewl_object_current_y_get(EWL_OBJECT(child)) +
                ewl_object_preferred_h_get(EWL_OBJECT(child)) - CURRENT_Y(o);
        if (size > PREFERRED_H(o))
                ewl_object_preferred_inner_h_set(EWL_OBJECT(o), size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param size: UNUSED
 * @param o: UNUSED
 * @return Returns no value
 * @brief The child resize callback
 */
void
ewl_overlay_cb_child_resize(Ewl_Container *c, Ewl_Widget *w,
               int size __UNUSED__, Ewl_Orientation o __UNUSED__)
{
        int maxw = 0, maxh = 0;
        Ewl_Overlay *overlay;
        Ewl_Object *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_OVERLAY_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        child = EWL_OBJECT(w);
        overlay = EWL_OVERLAY(c);

        ecore_dlist_first_goto(EWL_CONTAINER(overlay)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(overlay)->children))) {
                int cs;

                /*
                 * FIXME: Do we really want to do this?
                 * Move children within the bounds of the viewable area
                 */
                if (ewl_object_current_x_get(child) < CURRENT_X(overlay))
                        ewl_object_x_request(child, CURRENT_X(overlay));
                if (ewl_object_current_y_get(child) < CURRENT_Y(overlay))
                        ewl_object_y_request(child, CURRENT_Y(overlay));

                cs = ewl_object_current_x_get(child) +
                        ewl_object_preferred_w_get(child);

                /*
                 * Check the width and x position vs. overlay width.
                 */
                if (maxw < cs)
                        maxw = cs;

                cs = ewl_object_current_y_get(child) +
                        ewl_object_preferred_h_get(child);

                /*
                 * Check the height and y position vs. overlay height.
                 */
                if (maxh < cs)
                        maxh = cs;

        }

        ewl_object_preferred_inner_size_set(EWL_OBJECT(overlay), maxw, maxh);
        ewl_object_size_request(EWL_OBJECT(c),
                                ewl_object_current_w_get(EWL_OBJECT(c)),
                                ewl_object_current_h_get(EWL_OBJECT(c)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

