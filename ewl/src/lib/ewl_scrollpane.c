/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_scrollpane.h"
#include "ewl_scrollport.h"
#include "ewl_box.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a new scrollpane on success, NULL on failure.
 * @brief Create a new scrollpane
 */
Ewl_Widget *
ewl_scrollpane_new(void)
{
        Ewl_Scrollpane *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Scrollpane, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_scrollpane_init(s)) {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to initialize
 * @return Returns no value.
 * @brief Initialize the fields of a scrollpane
 *
 * Sets up default callbacks and field values for the scrollpane @a s.
 */
int
ewl_scrollpane_init(Ewl_Scrollpane *s)
{
        Ewl_Widget *w;
        Ewl_Container *va;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        w = EWL_WIDGET(s);

        if (!ewl_scrollport_init(EWL_SCROLLPORT(s)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_SCROLLPANE_TYPE);
        ewl_widget_inherit(w, EWL_SCROLLPANE_TYPE);
        ewl_widget_focusable_set(EWL_WIDGET(s), TRUE);

        ewl_container_callback_notify(EWL_CONTAINER(s), EWL_CALLBACK_FOCUS_IN);
        ewl_container_callback_notify(EWL_CONTAINER(s), EWL_CALLBACK_FOCUS_OUT);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                        ewl_scrollpane_cb_configure, NULL);


        /* Remove the default focus out callback and replace with our own */
        ewl_callback_del(w, EWL_CALLBACK_FOCUS_OUT, ewl_widget_cb_focus_out);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
                                ewl_container_cb_container_focus_out, NULL);

        /*
         * Create the container to hold the contents and it's configure
         * callback to position it's child.
         */
        s->box = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(s->box), EWL_FLAG_FILL_FILL);
        ewl_container_child_append(EWL_CONTAINER(s), s->box);
        ewl_widget_internal_set(s->box, TRUE);
        ewl_widget_show(s->box);

        /* after we added our internal widgets we can redirect the 
         * scrollpane to the content box */
        ewl_container_redirect_set(EWL_CONTAINER(s), EWL_CONTAINER(s->box));
        
        /*
         * Append necessary callbacks for the scrollpane.
         */
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
                        ewl_scrollpane_cb_focus_jump, NULL);
        
        va = ewl_scrollport_visible_area_get(EWL_SCROLLPORT(s));
        ewl_container_show_notify_set(va, ewl_scrollpane_cb_container_show);
        ewl_container_resize_notify_set(va, ewl_scrollpane_cb_container_resize);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Move the contents of the scrollbar into place
 */
void
ewl_scrollpane_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Scrollpane *sp;
        int ax, ay, aw, ah;
        int vw, vh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_SCROLLPANE_TYPE);

        sp = EWL_SCROLLPANE(w);
        /*
         * Move the box into position
         */
        ewl_scrollport_area_geometry_get(EWL_SCROLLPORT(w), &ax, &ay, &aw, &ah);
        ewl_scrollport_visible_area_geometry_get(EWL_SCROLLPORT(w), NULL, NULL,
                        &vw, &vh);
        ewl_object_geometry_request(EWL_OBJECT(sp->box), ax, ay,
                        (aw < vw) ? vw : aw, (ah < vh) ? vh : ah);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param p: The scrollbar to work with
 * @param w: The now visible child
 * @return Returns no value
 */
void
ewl_scrollpane_cb_container_show(Ewl_Container *p, Ewl_Widget *c __UNUSED__)
{
        Ewl_Scrollpane *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(EWL_WIDGET(p)->parent, EWL_SCROLLPANE_TYPE);

        s = EWL_SCROLLPANE(EWL_WIDGET(p)->parent);
        ewl_scrollport_area_size_set(EWL_SCROLLPORT(s),
                        ewl_object_preferred_w_get(EWL_OBJECT(s->box)),
                        ewl_object_preferred_h_get(EWL_OBJECT(s->box)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param p: The scrollbar to work with
 * @param w: The resized child (unused)
 * @param size: unused
 * @param o: unused
 * @return Returns no value
 */
void
ewl_scrollpane_cb_container_resize(Ewl_Container *p, Ewl_Widget *c __UNUSED__, 
                                int size __UNUSED__, 
                                Ewl_Orientation o __UNUSED__)
{
        Ewl_Scrollpane *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(EWL_WIDGET(p)->parent, EWL_SCROLLPANE_TYPE);

        s = EWL_SCROLLPANE(EWL_WIDGET(p)->parent);
        ewl_scrollport_area_size_set(EWL_SCROLLPORT(s),
                        ewl_object_preferred_w_get(EWL_OBJECT(s->box)),
                        ewl_object_preferred_h_get(EWL_OBJECT(s->box)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The focus jump callback
 */
void
ewl_scrollpane_cb_focus_jump(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        int fx, fy, fw, fh;
        Ewl_Embed *emb;
        Ewl_Widget *focus;
        Ewl_Scrollpane *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_SCROLLPANE_TYPE);

        emb = ewl_embed_widget_find(w);
        if (!emb)
                DRETURN(DLEVEL_STABLE);

        s = EWL_SCROLLPANE(w);
        /*
         * Get the focused widget and stop if its an internal one.
         */
        focus = ewl_embed_focused_widget_get(emb);
        if (!focus || !ewl_widget_parent_of(s->box, focus) ||
                        ewl_widget_onscreen_is(focus))
                DRETURN(DLEVEL_STABLE);

        ewl_object_current_geometry_get(EWL_OBJECT(focus), &fx, &fy, &fw, &fh);
        ewl_scrollport_visible_area_geometry_set(EWL_SCROLLPORT(w), fx, fy,
                                                                fw, fh);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

