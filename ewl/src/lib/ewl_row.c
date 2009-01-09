/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_row.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_row_cb_state_changed(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a newly allocated row on success, NULL on failure.
 * @brief Allocate and initialize a new row
 */
Ewl_Widget *
ewl_row_new(void)
{
        Ewl_Widget *row;

        DENTER_FUNCTION(DLEVEL_STABLE);

        row = NEW(Ewl_Row, 1);
        if (!row)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_row_init(EWL_ROW(row))) {
                ewl_widget_destroy(row);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * @param row: the row object to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the row fields of an inheriting object
 *
 * The fields of the @a row object are initialized to their defaults.
 */
int
ewl_row_init(Ewl_Row *row)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(row, FALSE);

        if (!ewl_container_init(EWL_CONTAINER(row)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(row), EWL_ROW_TYPE);
        ewl_widget_inherit(EWL_WIDGET(row), EWL_ROW_TYPE);

        ewl_container_show_notify_set(EWL_CONTAINER(row), ewl_row_cb_child_show);
        ewl_container_hide_notify_set(EWL_CONTAINER(row), ewl_row_cb_child_hide);
        ewl_container_resize_notify_set(EWL_CONTAINER(row),
                                        ewl_row_cb_child_resize);

        ewl_object_fill_policy_set(EWL_OBJECT(row), EWL_FLAG_FILL_HFILL);

        ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_CONFIGURE,
                                ewl_row_cb_configure, NULL);
        ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_STATE_CHANGED,
                                ewl_row_cb_state_changed, NULL);

        ewl_widget_focusable_set(EWL_WIDGET(row), FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param row: the row to change the header row
 * @param header: header row for adjusting cell placement
 * @return Returns no value.
 * @brief Set the row header of constraints on cell widths
 *
 * Changes the row that cell widths and placements will be based on to @a
 * header.
 */
void
ewl_row_header_set(Ewl_Row *row, Ewl_Container *header)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(row);
        DCHECK_TYPE(row, EWL_ROW_TYPE);

        if (row->header == header)
                DRETURN(DLEVEL_STABLE);

        row->header = header;
        if (header) {
                ewl_object_fill_policy_set(EWL_OBJECT(row),
                                           EWL_FLAG_FILL_HFILL);

                ewl_widget_configure(EWL_WIDGET(header));
        }
        else
                ewl_widget_configure(EWL_WIDGET(row));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param row: the row to retrieve a columns widget from
 * @param n: the column containing the desired widget
 * @return Returns widget located in column @a n in @a row on success.
 * @brief Retrieve the widget at a specified column
 */
Ewl_Widget *
ewl_row_column_get(Ewl_Row *row, short n)
{
        Ewl_Widget *found;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(row, NULL);
        DCHECK_TYPE_RET(row, EWL_ROW_TYPE, NULL);

        found = ecore_dlist_index_goto(EWL_CONTAINER(row)->children, n + 1);

        DRETURN_PTR(found, DLEVEL_STABLE);
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
ewl_row_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Row *row;
        Ewl_Container *c;
        Ewl_Object *child;
        Ewl_Object *align;
        int x;
        int remains, nodes;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        row = EWL_ROW(w);
        c = EWL_CONTAINER(w);

        x = CURRENT_X(w);
        remains = CURRENT_W(w);

        nodes = 0;
        ewl_container_child_iterate_begin(c);
        while ((child = EWL_OBJECT(ewl_container_child_next(c)))) {
                if (VISIBLE(child))
                        nodes++;
        }

        ewl_container_child_iterate_begin(c);

        /*
         * This should be the common case, a row bounded by a set of fields,
         * for forming a table.
         */
        if (row->header) {
                int width;
                Ewl_Container *hdr;

                hdr = row->header;
                ewl_container_child_iterate_begin(hdr);

                /*
                 * Get the first child of the header.
                 */
                align = EWL_OBJECT(ewl_container_child_next(hdr));
                if (align)
                        x = MAX(ewl_object_current_x_get(align), CURRENT_X(w));
                else
                        x = CURRENT_X(w);

                /*
                 * Iterate over the children and position the header children.
                 */
                ewl_container_child_iterate_begin(hdr);
                while ((child = EWL_OBJECT(ewl_container_child_next(c)))) {
                        align = EWL_OBJECT(ewl_container_child_next(hdr));
                        if (align && VISIBLE(align))
                                width = ewl_object_current_x_get(align) +
                                                ewl_object_current_w_get(align) - x;
                        else if (nodes)
                                width = remains / nodes;
                        else
                                width = remains;

                        /*
                         * Request the necessary geometry then check what was
                         * accepted to calculate remaining steps.
                         */
                        ewl_object_place(child, x, CURRENT_Y(w), width,
                                                         CURRENT_H(w));
                        width = ewl_object_current_w_get(child);
                        x += width;
                        remains -= width;
                        nodes--;
                }
        }
        /*
         * In the uncommon case, we simply try to give out a fair amount of
         * space.
         */
        else {
                int tx = x;
                while ((child = EWL_OBJECT(ewl_container_child_next(c)))) {
                        int portion;

                        /*
                         * Ask for the child to stay the current size for now.
                         */
                        portion = ewl_object_current_w_get(child);
                        ewl_object_position_request(child, tx, CURRENT_Y(w));
                        ewl_object_w_request(child, portion);
                        ewl_object_h_request(child, CURRENT_H(w));

                        remains -= portion;
                        portion = ewl_object_current_w_get(child);
                        tx = ewl_object_current_x_get(child) + portion;
                }

                /* Divvy up remaining space */
                if (remains) {
                        tx = x;
                        nodes = ecore_dlist_count(c->children);
                               ecore_dlist_first_goto(c->children);
                        while ((child = ecore_dlist_next(c->children))) {
                                int portion;
                                int width = ewl_object_current_w_get(child);

                                if (nodes)
                                        portion = remains / nodes;
                                else
                                        portion = remains;
                                ewl_object_x_request(child, tx);
                                ewl_object_w_request(child, portion + width);
                                remains -= portion;
                                portion = ewl_object_current_w_get(child);
                                tx += portion;
                                nodes--;
                        }
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Row
 * @return Returns no value
 * @brief The header configure callback
 */
void
ewl_row_cb_header_configure(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                                        void *user_data)
{
        Ewl_Row *row;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);

        row = EWL_ROW(user_data);
        /* ewl_object_preferred_inner_w_set(EWL_OBJECT(w), CURRENT_W(row->header)); */
        ewl_widget_configure(EWL_WIDGET(row));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Row
 * @return Returns no value
 * @brief The header destroy callback
 */
void
ewl_row_cb_header_destroy(Ewl_Widget *w __UNUSED__,
                        void *ev_data __UNUSED__, void *user_data)
{
        Ewl_Row *row;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);

        row = EWL_ROW(user_data);
        row->header = NULL;
        ewl_row_header_set(row, NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_row_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_ROW_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
        ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) +
                        ewl_object_preferred_w_get(EWL_OBJECT(w)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child hide callback
 */
void
ewl_row_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_ROW_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
        ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) -
                        ewl_object_preferred_w_get(EWL_OBJECT(w)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: UNUSED
 * @param size: The child size
 * @param o: The orientation
 * @return Returns no value
 * @brief The child resize callback
 */
void
ewl_row_cb_child_resize(Ewl_Container *c, Ewl_Widget *w __UNUSED__,
                                int size, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_ROW_TYPE);

        if (o == EWL_ORIENTATION_VERTICAL)
                ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
        else
                ewl_object_preferred_inner_w_set(EWL_OBJECT(c),
                                PREFERRED_W(c) + size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @ev: The Ewl_Event_State_Change struct
 * @data: UNUSED
 * @return Returns no value
 * @brief Sends the state on to the row's children
 */
static void
ewl_row_cb_state_changed(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Widget *o;
        Ewl_Event_State_Change *e;
        const char *send_state;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_ROW_TYPE);

        e = EWL_EVENT_STATE_CHANGE(ev);

        /* Only want this for selected signals */
        if (!strcmp(e->state, "selected"))
                send_state = "parent,selected";
        else if (!strcmp(e->state, "deselect"))
                send_state = "parent,deselect";
        else
                DRETURN(DLEVEL_STABLE);

        ewl_container_child_iterate_begin(EWL_CONTAINER(w));
        while ((o = ewl_container_child_next(EWL_CONTAINER(w))))
                ewl_widget_state_set(o, send_state, e->flag);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

