/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_list2.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_scrollport.h"

static void ewl_list2_preferred_size_calc(Ewl_List2 *list);
static Ewl_Widget *ewl_list2_widget_at(Ewl_MVC *mvc, void *data,
                                        unsigned int row,
                                        unsigned int column);
static void ewl_list_draw(Ewl_List2 *list);
static void ewl_list2_cb_scroll_configure(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
static int ewl_list2_redraw_needed(Ewl_List2 *list);

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and initializes a new Ewl_List widget
 */
Ewl_Widget *
ewl_list2_new(void)
{
        Ewl_Widget *list = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        list = NEW(Ewl_List2, 1);
        if (!list)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_list2_init(EWL_LIST2(list)))
        {
                ewl_widget_destroy(list);
                list = NULL;
        }

        DRETURN_PTR(list, DLEVEL_STABLE);
}

/**
 * @param list: The list to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialises an Ewl_List widget to default values
 */
int
ewl_list2_init(Ewl_List2 *list)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, FALSE);

        if (!ewl_mvc_init(EWL_MVC(list)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        list->scrollport = ewl_scrollport_new();
        ewl_container_child_append(EWL_CONTAINER(list), list->scrollport);
        //ewl_callback_append(list->scrollport, EWL_CALLBACK_CONFIGURE,
        //                                ewl_list2_cb_scroll_configure, list);
        ewl_widget_show(list->scrollport);

        list->box = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(list->scrollport), list->box);
        ewl_object_fill_policy_set(EWL_OBJECT(list->box), EWL_FLAG_FILL_FILL);
        ewl_widget_internal_set(list->box, TRUE);
        ewl_widget_show(list->box);

        ewl_container_redirect_set(EWL_CONTAINER(list),
                                                EWL_CONTAINER(list->box));
        ewl_container_show_notify_set(EWL_CONTAINER(list),
                                                ewl_list2_cb_child_show);
        ewl_container_resize_notify_set(EWL_CONTAINER(list),
                                                ewl_list2_cb_child_resize);

        ewl_widget_appearance_set(EWL_WIDGET(list), EWL_LIST2_TYPE);
        ewl_widget_inherit(EWL_WIDGET(list), EWL_LIST2_TYPE);

        ewl_mvc_selected_change_cb_set(EWL_MVC(list),
                                                ewl_list2_cb_selected_change);

        ewl_callback_append(EWL_WIDGET(list),
                                        EWL_CALLBACK_CONFIGURE,
                                        ewl_list2_cb_configure, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param list: The list to use
 * @parem fixed: TRUE if the widgets are of fixed size, FALSE otherwise
 * @brief Allows for optimization of the list.  Ensure that the fixed dimension matches the orientation of the box (a fixed widget height with a horizontally orientated box will not produce correct behaviour).
 */
void
ewl_list2_fixed_size_set(Ewl_List2 *list, unsigned char fixed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(list);
        DCHECK_TYPE(list, EWL_LIST2_TYPE);

        if (fixed == list->fixed)
                DRETURN(DLEVEL_STABLE);

        list->fixed = fixed;

        DRETURN(DLEVEL_STABLE);
}

/**
 * @oaram list: The list to use
 * @return Returns if the list is assuming that widgets contained are of fixed size
 * @brief Returns if the list is assuming that widgets contained are of fixed size
 */
unsigned char
ewl_list2_fixed_size_get(Ewl_List2 *list)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, FALSE);
        DCHECK_TYPE_RET(list, EWL_LIST2_TYPE, FALSE);

        DRETURN_INT(list->fixed, DLEVEL_STABLE);
}

/**
 * @internal
 * @param list: The list to create widget from
 * @param idx: The index in the data to be used
 * @param w: The width of the widget
 * @param h: The height of the widget
 */
void
ewl_list2_generate_widget_size(Ewl_List2 *list, unsigned int idx, int *w,
                                                                        int *h)
{
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        void *pr_data;
        Ewl_Widget *o, *cell;

        DCHECK_PARAM_PTR(list);
        DCHECK_TYPE(list, EWL_LIST2_TYPE);

        model = ewl_mvc_model_get(EWL_MVC(list));
        view = ewl_mvc_view_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));
        pr_data = ewl_mvc_private_data_get(EWL_MVC(list));

        if ((!model) || (!view) || (!mvc_data))
        {
                DWARNING("Internal list function called without valid "
                                                                "list data");
                DRETURN(DLEVEL_STABLE);
        }

        if (idx > model->count(mvc_data))
        {
                DWARNING("Internal list function with invalid index");
                DRETURN(DLEVEL_STABLE);
        }

        cell = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(list->box), cell);
        o = view->constructor(0, pr_data);
        view->assign(o, model->fetch(mvc_data, idx, 0), idx, 0, pr_data);
        ewl_container_child_append(EWL_CONTAINER(cell), o);
        ewl_widget_realize_force(cell);
        ewl_widget_realize_force(o);
        if (w) *w = PREFERRED_W(cell);
        if (h) *h = PREFERRED_H(cell);
        ewl_widget_destroy(cell);
        ewl_widget_destroy(o);

        DRETURN(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The list to be configured
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Configures the given list
 */
void
ewl_list2_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_List2 *list;
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        void *pr_data;
        unsigned int i, count, old_fill, box_fill = EWL_FLAG_FILL_FILL;
        int ws, hs, xa, ya;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_LIST2_TYPE);

        list = EWL_LIST2(w);

        model = ewl_mvc_model_get(EWL_MVC(list));
        view = ewl_mvc_view_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));
        pr_data = ewl_mvc_private_data_get(EWL_MVC(list));

        /* if either the list isn't dirty or some of the MVC controls have
         * not been set on the list just leave this up to the box to handle */
        if ((!model) || (!view) || (!mvc_data))
                DRETURN(DLEVEL_STABLE);

        /* get the number of rows */
        count = model->count(mvc_data);
        if (!count)
        {
                ewl_container_reset(EWL_CONTAINER(list));
                DRETURN(DLEVEL_STABLE);
        }

        if (ewl_mvc_dirty_get(EWL_MVC(list)))
        {
                if (list->pref_w)
                        FREE(list->pref_w);
                if (list->pref_h)
                        FREE(list->pref_h);

                if (list->fixed)
                {
                        ewl_list2_generate_widget_size(list, 0, &ws, &hs);
                        list->pref_h = NEW(int, 1);
                        list->pref_w = NEW(int, 1);
                        list->pref_w[0] = ws;
                        list->pref_h[0] = hs;
                }
                else
                {
                        list->pref_w = NEW(int, count);
                        list->pref_h = NEW(int, count);

                        for (i = 0; i < count; i++)
                        {
                                ewl_list2_generate_widget_size(list, i, &ws,
                                                                        &hs);
                                list->pref_w[i] = ws;
                                list->pref_h[i] = hs;
                        }
                }
        
                xa = PADDING_HORIZONTAL(list->box) +
                                                INSET_HORIZONTAL(list->box);
                ya = PADDING_VERTICAL(list->box) + INSET_VERTICAL(list->box);
                if (ewl_box_orientation_get(EWL_BOX(list->box)) ==
                                        EWL_ORIENTATION_HORIZONTAL)
                {
                        ewl_scrollport_area_size_set(EWL_SCROLLPORT
                                        (list->scrollport),
                                        (list->pref_w[0] * count) + xa,
                                         list->pref_h[0] + ya);
                }
                else
                {
                        ewl_scrollport_area_size_set(EWL_SCROLLPORT
                                        (list->scrollport),
                                         list->pref_w[0] + xa,
                                        (list->pref_h[0] * count) + ya);
                }
                
        }

        /*
         * Calcuate the offset for the box position
         */
        if (ewl_scrollport_hscrollbar_flag_get(EWL_SCROLLPORT(list->scrollport))
                                        == EWL_SCROLLPORT_FLAG_ALWAYS_HIDDEN)
                box_fill |= EWL_FLAG_FILL_HSHRINKABLE;

        if (ewl_scrollport_vscrollbar_flag_get(EWL_SCROLLPORT(list->scrollport))
                                        == EWL_SCROLLPORT_FLAG_ALWAYS_HIDDEN)
                box_fill |= EWL_FLAG_FILL_VSHRINKABLE;
        
        /*
         * Set the fill policy on the box based on scrollbars visible.
         */
        old_fill = ewl_object_fill_policy_get(EWL_OBJECT(list->box));
        ewl_object_fill_policy_set(EWL_OBJECT(list->box), box_fill);

        ewl_list_draw(list);

        /*
         * Reset the default fill policy on the box to get updated sizes..
         */
        ewl_object_fill_policy_set(EWL_OBJECT(list->box), old_fill);
        
        /* 
         * Stick this at the end here to force a list redraw
         */
        if (ewl_mvc_dirty_get(EWL_MVC(list)))
                ewl_mvc_dirty_set(EWL_MVC(list), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget that was clicked
 * @param ev: The event data
 * @param data: The list widget
 * @return Returns no value
 * @brief Sets the clicked widget as selected
 */
void
ewl_list2_cb_item_clicked(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Event_Mouse_Down *event;
        const Ewl_Model *model;
        void *mvc_data;
        int row;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_LIST2_TYPE);

        event = ev;

        if (ewl_mvc_selection_mode_get(EWL_MVC(data)) ==
                                        EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        model = ewl_mvc_model_get(EWL_MVC(data));
        mvc_data = ewl_mvc_data_get(EWL_MVC(data));
        row = ewl_container_child_index_get(EWL_CONTAINER(data), w);
        if (row < 0) DRETURN(DLEVEL_STABLE);

        if ((unsigned int) row > model->count(mvc_data))
        {
                DWARNING("Don't use container function on MVC widget!");
                DRETURN(DLEVEL_STABLE);
        }

        /* set up the event structure */
        {
                Ewl_Event_MVC_Clicked mvc_event = {
                        event->base.modifiers, event->button, event->clicks,
                        model, mvc_data, row, 0
                };

                ewl_callback_call_with_event_data(EWL_WIDGET(data), 
                                        EWL_CALLBACK_MVC_CLICKED, &mvc_event);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The MVC to work with
 * @return Returns no value
 * @brief Called when the selected widgets changes
 */
void
ewl_list2_cb_selected_change(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_LIST2_TYPE);

        ewl_mvc_highlight(mvc, EWL_CONTAINER(mvc), ewl_list2_widget_at);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_list2_widget_at(Ewl_MVC *mvc, void *data __UNUSED__, unsigned int row,
                                        unsigned int column __UNUSED__)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

        w = ewl_container_child_get(EWL_CONTAINER(mvc), row);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

static void
ewl_list_draw(Ewl_List2 *list)
{
        int vx, vy, vw, vh;
        int bx, by, bw, bh;
        int off_x, off_y, set_n;
        int sval, val = 0;
        int idx = 0, off_n, n;
        int *pref;
        Ewl_Orientation o;
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        void *pr_data;
        Ewl_Widget *obj, *cell;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(list);
        DCHECK_TYPE(list, EWL_LIST2_TYPE);

        /*
         * Check to make sure we actually need a redraw
         */
        if ((!ewl_mvc_dirty_get(EWL_MVC(list))) &&
                                        (!ewl_list2_redraw_needed(list)))
        {
                printf("no redraw needed!\n");
                return;
        }

        model = ewl_mvc_model_get(EWL_MVC(list));
        view = ewl_mvc_view_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));
        pr_data = ewl_mvc_private_data_get(EWL_MVC(list));

        ewl_scrollport_visible_area_geometry_get(EWL_SCROLLPORT
                                                        (list->scrollport),
                                                        &vx, &vy, &vw, &vh);
        ewl_scrollport_area_geometry_get(EWL_SCROLLPORT(list->scrollport),
                                                        &off_x, &off_y,
                                                        NULL, NULL);
        ewl_object_current_geometry_get(EWL_OBJECT(list->box), &bx, &by,
                                                                &bw, &bh);
        ewl_container_reset(EWL_CONTAINER(list));
        off_x *= -1;
        off_y *= -1;
        o = ewl_box_orientation_get(EWL_BOX(list->box));

        if (o == EWL_ORIENTATION_VERTICAL)
        {
                off_n = off_y;
                pref = list->pref_h;
                n = vh;
        }
        else
        {
                off_n = off_x;
                pref = list->pref_w;
                n = vw;
        }

        if (list->fixed)
        {
                idx = off_n / pref[0];
                val = pref[0] * idx;
        }
        else
        {

                while (val < off_n)
                {
                        val += pref[idx];
                        idx++;
                }

                if (idx > 0)
                {
                        idx--;
                        val -= pref[idx];
                }

                set_n = off_n - val;
        }

        sval = val;
        while (val < (off_n + n))
        {
                cell = ewl_cell_new();
                ewl_cell_state_change_cb_add(EWL_CELL(cell));
                ewl_container_child_append(EWL_CONTAINER(list), cell);
                //ewl_callback_append(cell, EWL_CALLBACK_CLICED,
                //                ewl_list_cb_item_clicked, list);
                ewl_widget_show(cell);

                obj = view->constructor(0, pr_data);
                view->assign(obj, model->fetch(mvc_data, idx, 0), idx, 0,
                                                                pr_data);
                ewl_container_child_append(EWL_CONTAINER(cell), obj);
                ewl_widget_show(obj);
                
                if (list->fixed)
                        val += pref[0];
                else
                        val += pref[idx];
                idx++;
        }

        printf("Box geometry: %d %d %d %d\n", CURRENT_X(list->box),
                        CURRENT_Y(list->box), CURRENT_W(list->box),
                        CURRENT_H(list->box));
        list->set_n = set_n;
        if (o == EWL_ORIENTATION_VERTICAL)
        {
                ewl_object_geometry_request(EWL_OBJECT(list->box), vx, set_n,
                                                        vw, val - sval);
                printf("Setting box geometry: %d %d %d %d\n", vx, set_n,
                                                        vw, val - sval);
        }
        else
        {
                ewl_object_geometry_request(EWL_OBJECT(list->box), set_n, vy,
                                                        val - sval, vh);
                printf("Setting box geometry: %d %d %d %d\n", set_n, vy,
                                                        val - sval, vh);
        }

        list->val_n = val - sval;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_list2_cb_scroll_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                        void *user_data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_LIST2_TYPE);

        ewl_widget_configure(user_data);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param p: The list to work with
 * @param w: The now visible child
 * @return Returns no value
 */
void
ewl_list2_cb_child_show(Ewl_Container *p, Ewl_Widget *c __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_LIST2_TYPE);

        ewl_list2_preferred_size_calc(EWL_LIST2(p));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param p: The list to work with
 * @param w: The resized child (unused)
 * @param size: unused
 * @param o: unused
 * @return Returns no value
 */
void
ewl_list2_cb_child_resize(Ewl_Container *p, Ewl_Widget *c __UNUSED__, 
                                int size __UNUSED__, 
                                Ewl_Orientation o __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_LIST2_TYPE);

        ewl_list2_preferred_size_calc(EWL_LIST2(p));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_list2_preferred_size_calc(Ewl_List2 *list)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(list);
        DCHECK_TYPE(list, EWL_LIST2_TYPE);

        ewl_object_preferred_inner_h_set(EWL_OBJECT(list),
                        ewl_object_preferred_h_get(EWL_OBJECT(list->box)));
        ewl_object_preferred_inner_w_set(EWL_OBJECT(list),
                        ewl_object_preferred_w_get(EWL_OBJECT(list->box)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_list2_redraw_needed(Ewl_List2 *list)
{
        int off_x, off_y;
        int x, y, w, h;
        int res;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, TRUE);
        DCHECK_TYPE_RET(list, EWL_LIST2_TYPE, TRUE);

        ewl_scrollport_visible_area_geometry_get(EWL_SCROLLPORT
                                                        (list->scrollport),
                                                        &x, &y, &w, &h);
        ewl_scrollport_area_geometry_get(EWL_SCROLLPORT(list->scrollport),
                                                        &off_x, &off_y,
                                                        NULL, NULL);

        res = 1;
        if (ewl_box_orientation_get(EWL_BOX(list->box)) ==
                                                EWL_ORIENTATION_VERTICAL)
        {
                if (list->fixed)
                {
                        res *= ((abs(off_y - list->set_n)) >= list->pref_h[0]);
                        printf("off_y: %d, list->set_n: %d\n", off_y, list->set_n);
                }
                else
                {
                        res *= 1;
                }
        }
        else
        {
                if (list->fixed)
                {
                        res *= ((abs(off_x - list->set_n)) >= list->pref_w[0]);
                }
                else
                {
                        res *= 1;
                }
        }

        DRETURN_INT(res, DLEVEL_STABLE);
}
