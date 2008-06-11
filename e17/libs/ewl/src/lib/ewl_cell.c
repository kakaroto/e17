/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_cell.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_cell_cb_state_changed(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a newly allocated cell on success, NULL on failure.
 * @brief Allocate and initialize a new cell
 */
Ewl_Widget *
ewl_cell_new(void)
{
        Ewl_Widget *cell;

        DENTER_FUNCTION(DLEVEL_STABLE);

        cell = NEW(Ewl_Cell, 1);
        if (!cell)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_cell_init(EWL_CELL(cell))) {
                FREE(cell);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(cell, DLEVEL_STABLE);
}

/**
 * @param cell: the cell object to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the cell fields of an inheriting object
 *
 * The fields of the @a cell object are initialized to their defaults.
 */
int
ewl_cell_init(Ewl_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cell, FALSE);

        if (!ewl_container_init(EWL_CONTAINER(cell)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_FILL |
                                   EWL_FLAG_FILL_HSHRINKABLE);
        ewl_widget_appearance_set(EWL_WIDGET(cell), EWL_CELL_TYPE);
        ewl_widget_inherit(EWL_WIDGET(cell), EWL_CELL_TYPE);

        ewl_container_show_notify_set(EWL_CONTAINER(cell), ewl_cell_cb_child_show);
        ewl_container_resize_notify_set(EWL_CONTAINER(cell),
                                    ewl_cell_cb_child_resize);

        ewl_callback_append(EWL_WIDGET(cell), EWL_CALLBACK_CONFIGURE,
                            ewl_cell_cb_configure, NULL);
        ewl_container_callback_notify(EWL_CONTAINER(cell), EWL_CALLBACK_FOCUS_IN);
        ewl_container_callback_notify(EWL_CONTAINER(cell), EWL_CALLBACK_FOCUS_OUT);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback for the cell widget
 */
void
ewl_cell_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Container *c;
        Ewl_Object *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CELL_TYPE);

        c = EWL_CONTAINER(w);

        /* we need to skip all unmanaged widgets first */
        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)) && UNMANAGED(child))
                ;

        if (child)
                ewl_object_place(child, CURRENT_X(w), CURRENT_Y(w),
                                CURRENT_W(w), CURRENT_H(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to show
 * @return Returns no value
 * @brief Update the container when a child widget is shown
 */
void
ewl_cell_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CELL_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Cell's only allow one child, so remove the rest, this may cause a
         * leak, but they should know better.
         */
        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children))) {
                if (child != w)
                        ewl_container_child_remove(c, child);
        }

        ewl_object_preferred_inner_size_set(EWL_OBJECT(c),
                        ewl_object_preferred_w_get(EWL_OBJECT(w)),
                        ewl_object_preferred_h_get(EWL_OBJECT(w)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget that was resized
 * @param size: UNUSED
 * @param o: UNUSED
 * @return Returns no value
 * @brief Callback for when a child widget is resized within the container
 */
void
ewl_cell_cb_child_resize(Ewl_Container *c, Ewl_Widget *w,
                        int size __UNUSED__, Ewl_Orientation o __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CELL_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_object_preferred_inner_size_set(EWL_OBJECT(c),
                        ewl_object_preferred_w_get(EWL_OBJECT(w)),
                        ewl_object_preferred_h_get(EWL_OBJECT(w)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param cell: The cell to work with
 * @return: Returns no value
 * @brief Adds the callback to send state changes on to the cell's children
 */
void
ewl_cell_state_change_cb_add(Ewl_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cell);
        DCHECK_TYPE(cell, EWL_CELL_TYPE);

        ewl_callback_append(EWL_WIDGET(cell), EWL_CALLBACK_STATE_CHANGED,
                        ewl_cell_cb_state_changed, NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cell: The cell to work with
 * @return: Returns no value
 * @brief Removes the callback to send state changes on to the cell's children
 */
void
ewl_cell_state_change_cb_del(Ewl_Cell *cell)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cell);
        DCHECK_TYPE(cell, EWL_CELL_TYPE);

        ewl_callback_del(EWL_WIDGET(cell), EWL_CALLBACK_STATE_CHANGED,
                        ewl_cell_cb_state_changed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @ev: The Ewl_Event_State_Change struct
 * @data: UNUSED
 * @return Returns no value
 * @brief Sends the state on to the cell's children
 */
void
ewl_cell_cb_state_changed(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Widget *o;
        Ewl_Event_State_Change *e;
        const char *send_state;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_CELL_TYPE);

        e = EWL_EVENT_STATE_CHANGE(ev);

        /* Only want this for selected signals */
        if (!strcmp(e->state, "selected"))
                send_state = "parent,selected";
        else if (!strcmp(e->state, "deselect"))
                send_state = "parent,deselect";
        else if ((!strcmp(e->state, "parent,selected")) ||
                        (!strcmp(e->state, "parent,deselect")))
                send_state = e->state;
        else
                DRETURN(DLEVEL_STABLE);

        ewl_container_child_iterate_begin(EWL_CONTAINER(w));
        while ((o = ewl_container_child_next(EWL_CONTAINER(w))))
                ewl_widget_state_set(o, send_state, e->flag);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}
