/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_box.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @brief Provides information on the orientation of a box
 */
typedef struct
{
        /*
         * Alignment is only done in the align direction.
         */
        int f1_align, f3_align;
        int a1_align, a3_align;

        /*
         * This is an indicator for filling in the proper directions.
         */
        int f_policy;

        /*
         * Function pointers for getting the dimension of the widget that we
         * care about.
         */
        void (*pref_fill_set) (Ewl_Object * ob, int size);
        int (*fill_ask) (Ewl_Object * ob);
        void (*fill_set) (Ewl_Object * ob, int size);

        int (*align_ask) (Ewl_Object * ob);
        void (*align_set) (Ewl_Object * ob, int size);
} Ewl_Box_Orientation;

/*
 * The information for the two different orientations
 */
static Ewl_Box_Orientation *ewl_box_vertical = NULL;
static Ewl_Box_Orientation *ewl_box_horizontal = NULL;

static void ewl_box_cb_shutdown(void);

/*
 * And a pointer to the currently used orientation
 */
static Ewl_Box_Orientation *ewl_box_info = NULL;

static void ewl_box_setup(void);

static int ewl_box_configure_calc(Ewl_Box * b, Ewl_Object **spread,
                                         int *fill_size, int *align_size);
static void ewl_box_configure_fill(Ewl_Object **spread, int fill_count,
                                           int *fill_size);
static void ewl_box_configure_layout(Ewl_Box * b, int *x, int *y,
                                           int *fill, int *align,
                                           int *align_size);
static void ewl_box_configure_child(Ewl_Box * b, Ewl_Object * c, int *x,
                                          int *y, int *align, int *align_size);

/**
 * @return Returns NULL on failure, or a newly allocated box on success.
 * @brief Allocate and initialize a new box with given orientation
 */
Ewl_Widget
*ewl_box_new(void)
{
        Ewl_Box *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = NEW(Ewl_Box, 1);
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_box_init(b)) {
                ewl_widget_destroy(EWL_WIDGET(b));
                b = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a newly allocated horizontal box on success.
 * @brief Allocate and initialize a new box with horizontal orientation
 */
Ewl_Widget
*ewl_hbox_new(void)
{
        Ewl_Widget *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = ewl_box_new();
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(b), EWL_ORIENTATION_HORIZONTAL);

        DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a newly allocated vertical box on success.
 * @brief Allocate and initialize a new box with vertical orientation
 */
Ewl_Widget
*ewl_vbox_new(void)
{
        Ewl_Widget *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = ewl_box_new();
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(b), EWL_ORIENTATION_VERTICAL);

        DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param b: the box to initialize
 * @return Returns no value.
 * @brief Initialize the box to starting values
 *
 * Responsible for setting up default values and callbacks
 * within a box structure.
 */
int
ewl_box_init(Ewl_Box * b)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(b, FALSE);

        w = EWL_WIDGET(b);

        /*
         * Initialize the container portion of the box
         */
        if (!ewl_container_init(EWL_CONTAINER(b))) {
                ewl_widget_destroy(EWL_WIDGET(b));
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_widget_inherit(w, EWL_BOX_TYPE);

        /*
         * Set the appearance based on default orientation.
         */
        ewl_widget_appearance_set(EWL_WIDGET(b), "hbox");

        /*
         * Setup the container size change handlers.
         */
        ewl_container_resize_notify_set(EWL_CONTAINER(b),
                                        ewl_box_cb_child_resize);
        ewl_container_show_notify_set(EWL_CONTAINER(b), ewl_box_cb_child_show);
        ewl_container_hide_notify_set(EWL_CONTAINER(b), ewl_box_cb_child_hide);

        /*
         * Attach the default layout callback.
         */
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                ewl_box_cb_configure, NULL);
        ewl_callback_del(w, EWL_CALLBACK_FOCUS_OUT, ewl_widget_cb_focus_out);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
                                ewl_container_cb_container_focus_out, NULL);

        /*
         * Check if the info structs have been created yet, if not create
         * them.
         */
        if (!ewl_box_horizontal)
                ewl_box_setup();

        /*
         * Set the box's appropriate orientation, set to a garbage value so
         * that the orientation does it's job.
         */
        b->orientation = EWL_ORIENTATION_HORIZONTAL;

        ewl_widget_focusable_set(w, FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param b: the box to change the orientation
 * @param o: the orientation to set for the box
 * @return Returns no value.
 * @brief Change the specified box's orientation
 *
 * Changes the orientation of the specified box, and
 * reconfigures it in order for the appearance to be updated.
 */
void
ewl_box_orientation_set(Ewl_Box * b, Ewl_Orientation o)
{
        Ewl_Container *c;
        Ewl_Widget *child;
        const char *appearance;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(b);
        DCHECK_TYPE(b, EWL_BOX_TYPE);

        /*
         * Set the orientation and reconfigure the widget so that child
         * widgets now have the new orientation layout
         */
        if (b->orientation == o)
                DRETURN(DLEVEL_STABLE);

        b->orientation = o;

        /* We check the previous appearance here to make sure we only change
         * the appearance if it is still set to box. Otherwise we might end
         * up wipeing out the appearance of an inheriting widget (like
         * menubar */
        appearance = ewl_widget_appearance_get(EWL_WIDGET(b));
        if ((b->orientation == EWL_ORIENTATION_HORIZONTAL)
                        && (!strcmp(appearance, "vbox")))
                ewl_widget_appearance_set(EWL_WIDGET(b), "hbox");

        else if ((b->orientation == EWL_ORIENTATION_VERTICAL)
                        && (!strcmp(appearance, "hbox")))
                ewl_widget_appearance_set(EWL_WIDGET(b), "vbox");

        /* we need to reset the preferred size of the box after chaning the
         * orientation. We'll cheat by calling ewl_box_child_show_cb foreach
         * child in the list */
        ewl_object_preferred_inner_size_set(EWL_OBJECT(b), 0, 0);

        c = EWL_CONTAINER(b);
        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children))) {
                if (!UNMANAGED(child))
                        ewl_box_cb_child_show(c, child);
        }

        ewl_widget_configure(EWL_WIDGET(b));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: the box to retrieve orientation
 * @return Returns the orientation value of the box @a b.
 * @brief Retrieves the orientation of the box
 */
Ewl_Orientation
ewl_box_orientation_get(Ewl_Box *b)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(b, 0);
        DCHECK_TYPE_RET(b, EWL_BOX_TYPE, 0);

        DRETURN_INT(b->orientation, DLEVEL_STABLE);
}

/**
 * @param b: the box to change homogeneous layout
 * @param h: the boolean value to change the layout mode to
 * @return Returns no value.
 * @brief Change the homogeneous layout of the box, setting TRUE will give all
 * children the same size.
 *
 * If the box is set to be homogeneous (TRUE) it will give all children the 
 * same size. By default the box is inhomogeneous.
 */
void
ewl_box_homogeneous_set(Ewl_Box *b, unsigned int h)
{
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(b);
        DCHECK_TYPE(b, EWL_BOX_TYPE);

        if (b->homogeneous == !!h)
                DRETURN(DLEVEL_STABLE);

        b->homogeneous = !!h;
        c = EWL_CONTAINER(b);

        if (h) {
                ewl_callback_del(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
                                ewl_box_cb_configure);
                ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
                                ewl_box_cb_configure_homogeneous, NULL);
                ewl_container_show_notify_set(c,
                                          ewl_box_cb_child_homogeneous_show);
                ewl_container_hide_notify_set(c,
                                          ewl_box_cb_child_homogeneous_show);
                ewl_container_resize_notify_set(c,
                                          ewl_box_cb_child_homogeneous_resize);
                /*
                 * calculate the new preferred size, we use here the show cb
                 * since it does exactly what we need to do
                 */
                ewl_box_cb_child_homogeneous_show(c, NULL);
        }
        else {
                int nodes;

                ewl_callback_del(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
                                ewl_box_cb_configure_homogeneous);
                ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
                                ewl_box_cb_configure, NULL);
                ewl_container_show_notify_set(c,
                                          ewl_box_cb_child_show);
                ewl_container_hide_notify_set(c,
                                          ewl_box_cb_child_hide);
                ewl_container_resize_notify_set(c,
                                          ewl_box_cb_child_resize);
                /*
                 * calculate the new preferred size
                 */
                nodes = ewl_container_child_count_visible_get(c) - 1;
                if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
                        ewl_container_largest_prefer(c,
                                                EWL_ORIENTATION_VERTICAL);
                        ewl_container_sum_prefer(c,
                                                EWL_ORIENTATION_HORIZONTAL);
                        ewl_object_preferred_inner_w_set(EWL_OBJECT(b),
                                        PREFERRED_W(b) + b->spacing * nodes);
                }
                else {
                        ewl_container_largest_prefer(c,
                                                EWL_ORIENTATION_HORIZONTAL);
                        ewl_container_sum_prefer(c,
                                                EWL_ORIENTATION_VERTICAL);
                        ewl_object_preferred_inner_h_set(EWL_OBJECT(b),
                                        PREFERRED_H(b) + b->spacing * nodes);
                }
        }

        ewl_widget_configure(EWL_WIDGET(b));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: the box to retrieve layout
 * @return Returns the homogeneous layout  value of the box @a b.
 * @brief Retrieves the layout of the box
 */
unsigned int
ewl_box_homogeneous_get(Ewl_Box *b)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(b, 0);
        DCHECK_TYPE_RET(b, EWL_BOX_TYPE, 0);

        DRETURN_INT(b->homogeneous, DLEVEL_STABLE);
}

/**
 * @param b: the box to change the spacing
 * @param s: the spacing to put between the child widgets
 * @return Returns no value.
 * @brief Changes the spacing between the objects in the box
 *
 * Adjust the spacing of the specified box and reconfigure
 * it to change the appearance.
 */
void
ewl_box_spacing_set(Ewl_Box * b, int s)
{
        int nodes;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(b);
        DCHECK_TYPE(b, EWL_BOX_TYPE);

        w = EWL_WIDGET(b);

        nodes = ewl_container_child_count_visible_get(EWL_CONTAINER(b)) - 1;

        if (nodes > 0) {
                if (b->orientation == EWL_ORIENTATION_HORIZONTAL)
                        ewl_object_preferred_inner_w_set(EWL_OBJECT(w),
                                        PREFERRED_W(w) - (nodes * b->spacing) +
                                        (nodes * s));
                else
                        ewl_object_preferred_inner_h_set(EWL_OBJECT(w),
                                        PREFERRED_H(w) - (nodes * b->spacing) +
                                        (nodes * s));
        }

        b->spacing = s;

        ewl_widget_configure(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to configure
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Configures the box
 *
 * Box layout algorithm:
 * 1. Setup variables orientation dependant pointers so that the algorithm is
 *    independant of orientation.
 * 2. Attempt to handout an even amount of space to all children,
 *    simultaneously calculating the remaining or overflow space not accepted
 *    by the children.
 * 3. Try to coerce the children to fill or shrink to compensate for space
 *    calculated to under or over run.
 * 4. Layout the position of all children based on the sizes accepted.
 */
void
ewl_box_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Box *b;

        int fill_count;
        int x, y;
        int width, height;
        int *fill, *align;
        int *fill_size, *align_size;
        Ewl_Object **spread;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_BOX_TYPE);

        b = EWL_BOX(w);

        fill_count = ewl_container_child_count_visible_get(EWL_CONTAINER(w));

        /*
         * Catch the easy case, and return.
         */
        if (!fill_count)
                DRETURN(DLEVEL_STABLE);

        spread = alloca(fill_count * sizeof(Ewl_Object *));

        /*
         * Get the starting values for the dimensions of the box.
         */
        x = CURRENT_X(w);
        y = CURRENT_Y(w);
        width = CURRENT_W(w);
        height = CURRENT_H(w);

        /*
         * These variables avoid large nested if's to handle different
         * orientations.
         */
        if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
                fill = &x;
                fill_size = &width;
                align = &y;
                align_size = &height;
                ewl_box_info = ewl_box_horizontal;
        } else {
                fill = &y;
                fill_size = &height;
                align = &x;
                align_size = &width;
                ewl_box_info = ewl_box_vertical;
        }

        /*
         * Split the children up for configuration based on alignment.
         */
        fill_count = ewl_box_configure_calc(b, spread, fill_size, align_size);

        /*
         * Spread the fill space if necessary.
         */
        ewl_box_configure_fill(spread, fill_count, fill_size);

        /*
         * Layout the children in their appropriate positions.
         */
        ewl_box_configure_layout(b, &x, &y, fill, align, align_size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to configure
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Configure a homegeneous box
 */
void
ewl_box_cb_configure_homogeneous(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        int i, num;
        int x, y;
        int width, height;
        int *fill;
        int *fill_size;
        int remainder;
        Ewl_Object *child;
        Ewl_Box *b;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_BOX_TYPE);

        if (ecore_dlist_empty_is(EWL_CONTAINER(w)->children))
                DRETURN(DLEVEL_STABLE);

        num = ewl_container_child_count_visible_get(EWL_CONTAINER(w));

        if (!num)
                DRETURN(DLEVEL_STABLE);

        b = EWL_BOX(w);

        /*
         * Get the starting values for the dimensions of the box.
         */
        x = CURRENT_X(w);
        y = CURRENT_Y(w);
        width = CURRENT_W(w);
        height = CURRENT_H(w);

        /*
         * These variables avoid large nested if's to handle different
         * orientations.
         */
        if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
                fill = &x;
                fill_size = &width;
        } else {
                fill = &y;
                fill_size = &height;
        }

        *fill_size -= b->spacing * (num - 1);
        remainder = *fill_size % num;
        *fill_size = *fill_size / num;

        i = 0;
        ecore_dlist_first_goto(EWL_CONTAINER(w)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children))) {
                if (!VISIBLE(child))
                        continue;

                i++;
                if (i == num)
                        *fill_size += remainder;
                ewl_object_place(child, x, y, width, height);
                *fill += *fill_size + b->spacing;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Split the children into three lists for layout.
 */
static int
ewl_box_configure_calc(Ewl_Box * b, Ewl_Object **spread, int *fill_size, int *align_size)
{
        int i;
        Ewl_Object *child;
        int initial;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(b, 0);
        DCHECK_TYPE_RET(b, EWL_BOX_TYPE, 0);
        DCHECK_PARAM_PTR_RET(spread, 0);

        initial = *fill_size /
                ewl_container_child_count_visible_get(EWL_CONTAINER(b));

        /*
         * Sort the children into lists dependant on their alignment within the
         * box.
         */
        i = 0;
        ecore_dlist_first_goto(EWL_CONTAINER(b)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(b)->children))) {
                int change;
                unsigned int policy;

                if (!VISIBLE(child) || UNMANAGED(child))
                        continue;
                /*
                 * Place the child on a list depending on it's matching
                 * alignment. First check for top/left alignment.
                 */
                /*
                 * Set the initial fill size to the preferred size.
                 */
                ewl_box_info->fill_set(child, initial);

                change = ewl_box_info->fill_ask(child);

                /*
                 * Figure out how much extra space is available for
                 * filling widgets.
                 */
                *fill_size -= change + b->spacing;

                /*
                 * Attempt to give the widget the full size, this will
                 * fail if the fill policy or bounds don't allow it.
                 */
                ewl_box_info->align_set(child, *align_size);

                /*
                 * If it has a fill policy for a direction we're
                 * concerned with, add it to the fill list.
                 */
                policy = ewl_object_fill_policy_get(child);
                policy &= ewl_box_info->f_policy;
                if (policy || change == initial) {
                        spread[i] = child;
                        i++;
                }
        }
        
        /* the last widget does not have spacing on its right side, so
         * we need to add it again */
        *fill_size += b->spacing;

        DRETURN_INT(i, DLEVEL_STABLE);
}

/*
 * Spread space to any widgets that have fill policy set to fill. This should
 * not be called if @num_fill or *@fill_size are equal to zero.
 */
static void
ewl_box_configure_fill(Ewl_Object **spread, int fill_count, int *fill_size)
{
        int i;
        int space;
        int temp, remainder;
        Ewl_Object *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(spread);

        /*
         * Calculate the space to give per child. Safeguard against divide by
         * zero.
         */
        space = (!fill_count ? 0 : *fill_size / fill_count);

        /*
         * As long as children keep accepting more space, we should loop
         * through and give them what's available.
         */
        while (space && fill_count) {

                i = 0;
                while ((i < fill_count) && (c = spread[i])) {

                        /*
                         * Save the current size of the child, then
                         * attempt to give it a portion of the space
                         * available.
                         */
                        temp = ewl_box_info->fill_ask(c);
                        ewl_box_info->fill_set(c, temp + space);

                        /*
                         * Determine if the child accepted any of the space
                         */
                        temp = ewl_box_info->fill_ask(c) - temp;

                        /*
                         * If the child did not accept any of the size, then
                         * it's at it's max/min and is no longer useful.
                         */
                        if (!temp) {
                                if (i + 1 < fill_count) {
                                        memmove((spread + i), (spread + i + 1),
                                                        (fill_count - i) *
                                                        sizeof(Ewl_Object *));
                                }
                                fill_count--;
                        }
                        else {
                                *fill_size -= temp;
                                i++;
                        }
                }

                /*
                 * Calculate the space to give per child.
                 */
                space = (!fill_count ? 0 : *fill_size / fill_count);
        }

        /*
         * Distribute any remaining fill space.
         */
        while (*fill_size && fill_count) {

                /*
                 * Determine the sign of the amount to be incremented.
                 */
                remainder = *fill_size / abs(*fill_size);

                /*
                 * Add the remainder sign to each child.
                 */
                i = 0;
                while (*fill_size && (i < fill_count) && (c = spread[i])) {

                        /*
                         * Store the current size of the child.
                         */
                        temp = ewl_box_info->fill_ask(c);

                        /*
                         * Attempt to give it a portion of the remaining space
                         */
                        ewl_box_info->fill_set(c, temp + remainder);

                        /*
                         * Determine if the child accepted the space
                         */
                        temp = ewl_box_info->fill_ask(c) - temp;

                        /*
                         * Remove the child if it didn't accept any space,
                         * otherwise subtract the accepted space from the
                         * total.
                         */
                        if (!temp || (*fill_size - temp < 0)) {
                                if (i + 1 < fill_count) {
                                        memmove((spread + i), (spread + i + 1),
                                                        (fill_count - i) *
                                                        sizeof(Ewl_Object *));
                                }
                                fill_count--;
                        }
                        else {
                                *fill_size -= remainder;
                                i++;
                        }
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_box_configure_layout(Ewl_Box * b, int *x, int *y, int *fill,
                           int *align, int *align_size)
{
        Ewl_Object *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(b);
        DCHECK_TYPE(b, EWL_BOX_TYPE);

        /*
         * Configure the widgets on the first list.
         */
        ecore_dlist_first_goto(EWL_CONTAINER(b)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(b)->children))) {

                if (!VISIBLE(child) || UNMANAGED(child))
                        continue;

                /*
                 * Position this child based on the determined values.
                 */
                ewl_box_configure_child(b, child, x, y, align,
                                align_size);

                /*
                 * Move to the next position for the child.
                 */
                *fill += ewl_box_info->fill_ask(child) + b->spacing;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_box_configure_child(Ewl_Box * b __UNUSED__, Ewl_Object * c,
                        int *x, int *y, int *align, int *align_size)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);

        /*
         * Adjust the alignment in the direction opposite of the
         * orientation. The first one is the simplest as it it a
         * direct use of the current coordinates.
         */
        if (ewl_object_alignment_get(c) & ewl_box_info->a1_align) {
                ewl_object_position_request(c, *x, *y);
        }

        /*
         * The second one is aligned against the furthest edge, so
         * there is some calculation to be made.
         */
        else if (ewl_object_alignment_get(c) & ewl_box_info->a3_align) {
                *align += *align_size - ewl_box_info->align_ask(c);
                ewl_object_position_request(c, *x, *y);
                *align -= *align_size - ewl_box_info->align_ask(c);
        }

        /*
         * The final one is for centering the child.
         */
        else {
                *align += (*align_size - ewl_box_info->align_ask(c)) / 2;
                ewl_object_position_request(c, *x, *y);
                *align -= (*align_size - ewl_box_info->align_ask(c)) / 2;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container
 * @param w: The widget that was added
 * @return Returns no value
 * @brief When a child gets added to the box update it's size.
 */
void
ewl_box_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
        int space = 0;
        int width, height;
        int cw, ch;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_BOX_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (ewl_container_child_count_visible_get(c) > 1)
                space = EWL_BOX(c)->spacing;

        /*
         * Get the sizes common to both calculations.
         */
        width = ewl_object_preferred_inner_w_get(EWL_OBJECT(c));
        height = ewl_object_preferred_inner_h_get(EWL_OBJECT(c));

        cw = ewl_object_preferred_w_get(EWL_OBJECT(w));
        ch = ewl_object_preferred_h_get(EWL_OBJECT(w));

        /*
         * Base the info used on the orientation of the box.
         */
        if (EWL_BOX(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
                cw += space;
                ewl_object_preferred_inner_w_set(EWL_OBJECT(c), width + cw);
                if (ch > height)
                        ewl_object_preferred_inner_h_set(EWL_OBJECT(c), ch);
        }
        else {
                ch += space;
                if (cw > width)
                        ewl_object_preferred_inner_w_set(EWL_OBJECT(c), cw);
                ewl_object_preferred_inner_h_set(EWL_OBJECT(c), height + ch);
        }

        /* printf("Box %p children visible %d after show\n", c, ++EWL_BOX(c)->shows); */

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container
 * @param w: UNUSED
 * @return Returns no value
 * @brief Update the container when a widget is shown in homegeneous mode
 */
void
ewl_box_cb_child_homogeneous_show(Ewl_Container *c,
                                        Ewl_Widget *w __UNUSED__)
{
        int numc;
        int size, space = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        numc = ewl_container_child_count_visible_get(c);
        if (numc > 1)
                space = EWL_BOX(c)->spacing;

        ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);
        ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
        if (EWL_BOX(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
                size = (PREFERRED_W(c) + space) * numc - space;
                ewl_object_preferred_inner_w_set(EWL_OBJECT(c), size);
        }
        else {
                size = (PREFERRED_H(c) + space) * numc - space;
                ewl_object_preferred_inner_h_set(EWL_OBJECT(c), size);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container
 * @param w: The widget that was hidden
 * @return Returns no value
 * @brief When a child is hidden update the container
 */
void
ewl_box_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
        int space = 0;
        Ewl_Box *b = EWL_BOX(c);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_BOX_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (ewl_container_child_count_visible_get(c) > 1)
                space = b->spacing;

        if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
                int width;
                width = ewl_object_preferred_inner_w_get(EWL_OBJECT(c));
                width -= ewl_object_preferred_w_get(EWL_OBJECT(w));
                ewl_object_preferred_inner_w_set(EWL_OBJECT(c), width - space);
                ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
        }
        else {
                int height;
                height = ewl_object_preferred_inner_h_get(EWL_OBJECT(c));
                height -= ewl_object_preferred_h_get(EWL_OBJECT(w));
                ewl_object_preferred_inner_h_set(EWL_OBJECT(c), height - space);
                ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);
        }

        /* printf("Box %p children visible %d after hide\n", c, --EWL_BOX(c)->shows); */

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The Ewl_Container containing the widget
 * @param w: The widget being resized
 * @param size: The size of the widget
 * @param o: The orientation of the widget
 * @return Returns no value
 * @brief The container resize callback used by the box
 */
void
ewl_box_cb_child_homogeneous_resize(Ewl_Container *c, Ewl_Widget *w __UNUSED__,
                        int size __UNUSED__, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_BOX_TYPE);

        /*
         * If the change is in the orientation direction, we do the same
         * like in the show cb
         */
        if (EWL_BOX(c)->orientation == o)
                ewl_box_cb_child_homogeneous_show(c, NULL);

        /*
         * Find the new largest widget in the alignment direction
         */
        else
                ewl_container_largest_prefer(c, o);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}
/**
 * @internal
 * @param c: The Ewl_Container containing the widget
 * @param w: The widget being resized
 * @param size: The size of the widget
 * @param o: The orientation of the widget
 * @return Returns no value
 * @brief The container resize callback used by the box
 */
void
ewl_box_cb_child_resize(Ewl_Container *c, Ewl_Widget *w __UNUSED__,
                        int size, Ewl_Orientation o)
{
        int align_size, fill_size;
        Ewl_Box_Orientation *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_BOX_TYPE);

        /*
         * Get the appropriate dimension setting functions based on the
         * orientation.
         */
        if (EWL_BOX(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
                fill_size = PREFERRED_W(c);
                align_size = PREFERRED_H(c);
                info = ewl_box_horizontal;
        }
        else {
                fill_size = PREFERRED_H(c);
                info = ewl_box_vertical;
        }

        /*
         * If the change is in the orientation direction, just add it to the
         * current orientation size.
         */
        if (EWL_BOX(c)->orientation == o)
                info->pref_fill_set(EWL_OBJECT(c), fill_size + size);

        /*
         * Find the new largest widget in the alignment direction
         */
        else
                ewl_container_largest_prefer(c, o);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Setup some internal variables for effectively laying out the children based
 * on orientation.
 */
static void
ewl_box_setup(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        ewl_shutdown_add(ewl_box_cb_shutdown);

        if (!ewl_box_vertical) {
                ewl_box_vertical = NEW(Ewl_Box_Orientation, 1);
                if (!ewl_box_vertical)
                        DRETURN(DLEVEL_STABLE);

                /*
                 * This sets the aligments for filling direction.
                 */
                ewl_box_vertical->f1_align = EWL_FLAG_ALIGN_TOP;
                ewl_box_vertical->f3_align = EWL_FLAG_ALIGN_BOTTOM;

                /*
                 * These are the valid fill policies for this widget.
                 */
                ewl_box_vertical->f_policy =
                        EWL_FLAG_FILL_VSHRINKABLE | EWL_FLAG_FILL_VFILL;

                /*
                 * This sets the aligments for the non-filling direction.
                 */
                ewl_box_vertical->a1_align = EWL_FLAG_ALIGN_LEFT;
                ewl_box_vertical->a3_align = EWL_FLAG_ALIGN_RIGHT;

                /*
                 * These functions allow for asking the dimensions of the
                 * children.
                 */
                ewl_box_vertical->fill_ask = ewl_object_current_h_get;

                ewl_box_vertical->align_ask = ewl_object_current_w_get;

                /*
                 * These functions allow for setting the dimensions of the
                 * children.
                 */
                ewl_box_vertical->fill_set = ewl_object_h_request;
                ewl_box_vertical->pref_fill_set = ewl_object_preferred_inner_h_set;

                ewl_box_vertical->align_set = ewl_object_w_request;
        }

        if (!ewl_box_horizontal) {
                ewl_box_horizontal = NEW(Ewl_Box_Orientation, 1);
                if (!ewl_box_horizontal)
                        DRETURN(DLEVEL_STABLE);

                /*
                 * This sets the aligments for the filling direction.
                 */
                ewl_box_horizontal->f1_align = EWL_FLAG_ALIGN_LEFT;
                ewl_box_horizontal->f3_align = EWL_FLAG_ALIGN_RIGHT;

                /*
                 * These are the valid fill policies for this widget.
                 */
                ewl_box_horizontal->f_policy =
                        EWL_FLAG_FILL_HSHRINKABLE | EWL_FLAG_FILL_HFILL;

                /*
                 * This sets the aligments for the non-filling direction.
                 */
                ewl_box_horizontal->a1_align = EWL_FLAG_ALIGN_TOP;
                ewl_box_horizontal->a3_align = EWL_FLAG_ALIGN_BOTTOM;

                /*
                 * These functions allow for asking the dimensions of the
                 * children.
                 */
                ewl_box_horizontal->fill_ask = ewl_object_current_w_get;

                ewl_box_horizontal->align_ask = ewl_object_current_h_get;

                /*
                 * These functions allow for setting the dimensions of the
                 * children.
                 */
                ewl_box_horizontal->fill_set = ewl_object_w_request;
                ewl_box_horizontal->pref_fill_set = ewl_object_preferred_inner_w_set;

                ewl_box_horizontal->align_set = ewl_object_h_request;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_box_cb_shutdown(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        IF_FREE(ewl_box_horizontal);
        IF_FREE(ewl_box_vertical);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


