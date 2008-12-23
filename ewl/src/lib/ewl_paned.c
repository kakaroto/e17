/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_paned.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

typedef struct Ewl_Paned_Pane_Info Ewl_Paned_Pane_Info;

/**
 * @brief Stores information about a pane in the paned widget
 */
struct Ewl_Paned_Pane_Info
{
        Ewl_Widget *pane;
        Ewl_Paned_Size_Info *info;
        int size;
        unsigned char fixed:1;
};

typedef struct Ewl_Paned_Layout Ewl_Paned_Layout;

/**
 * @brief Contains information on a paned layout
 */
struct Ewl_Paned_Layout
{
        int (*minimum_size)(Ewl_Object *o);
        int (*current_size)(Ewl_Object *o);
        int (*preferred_size)(Ewl_Object *o);
        int (*current_position)(Ewl_Object *o);

        void (*variable_request)(Ewl_Object *o, int size);
        void (*stable_request)(Ewl_Object *o, int size);
        void (*position_request)(Ewl_Object *o, int pos);
        void (*stable_position_request)(Ewl_Object *o, int pos);
};

static Ewl_Paned_Layout *horizontal_layout = NULL;
static Ewl_Paned_Layout *vertical_layout = NULL;
static Ewl_Paned_Layout *layout = NULL;

static void ewl_paned_cb_shutdown(void);

static void ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_paned_grabber_cb_mouse_up(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev,
                                                        void *data);

static void ewl_paned_grabbers_update(Ewl_Paned *p);
static void ewl_paned_layout_setup(void);
static int ewl_paned_pane_info_setup(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes, 
                Ewl_Paned_Layout *layout, int *resizable);

static int ewl_paned_pane_info_collect(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes,
                       Ewl_Paned_Layout *layout, int *resizable, int grabber_size);
static void ewl_paned_pane_info_layout(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes,
                                int pane_num, Ewl_Paned_Layout *layout,
                                int available, int resizable);

static int ewl_paned_widgets_place(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes, 
                int grabber_size);

static int ewl_paned_grapper_size_get(Ewl_Paned *p);

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget
 */
Ewl_Widget *
ewl_paned_new(void)
{
        Ewl_Paned *pane;

        DENTER_FUNCTION(DLEVEL_STABLE);

        pane = NEW(Ewl_Paned, 1);
        if (!pane)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_paned_init(pane))
        {
                ewl_widget_destroy(EWL_WIDGET(pane));
                pane = NULL;
        }
        DRETURN_PTR(EWL_WIDGET(pane), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with horizontal orientation */
Ewl_Widget *
ewl_hpaned_new(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(ewl_paned_new(), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with vertical orientation
 */
Ewl_Widget *
ewl_vpaned_new(void)
{
        Ewl_Widget *pane;

        DENTER_FUNCTION(DLEVEL_STABLE);

        pane = ewl_paned_new();
        if (pane)
                ewl_paned_orientation_set(EWL_PANED(pane),
                                                EWL_ORIENTATION_VERTICAL);

        DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @param p: the paned widget to initialize
 * @return Returns 1 on success or 0 on failure
 * @brief Initialize a new paned widget to default values
 */
int
ewl_paned_init(Ewl_Paned *p)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, FALSE);

        w = EWL_WIDGET(p);

        if (!ewl_container_init(EWL_CONTAINER(p)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (!horizontal_layout)
                ewl_paned_layout_setup();

        ewl_widget_appearance_set(w, EWL_PANED_TYPE);
        ewl_widget_inherit(w, EWL_PANED_TYPE);

        p->orientation = EWL_ORIENTATION_HORIZONTAL;
        ewl_container_add_notify_set(EWL_CONTAINER(p),
                                        ewl_paned_cb_child_add);
        ewl_container_remove_notify_set(EWL_CONTAINER(p),
                                        ewl_paned_cb_child_remove);
        ewl_container_show_notify_set(EWL_CONTAINER(p),
                                        ewl_paned_cb_child_show);
        ewl_container_resize_notify_set(EWL_CONTAINER(p),
                                        ewl_paned_cb_child_resize);
        ewl_container_hide_notify_set(EWL_CONTAINER(p),
                                        ewl_paned_cb_child_hide);

        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                ewl_paned_cb_configure, NULL);
        ewl_callback_append(w, EWL_CALLBACK_DESTROY,
                                ewl_paned_cb_destroy, NULL);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);
        ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_LEFT |
                                                EWL_FLAG_ALIGN_TOP);
        ewl_widget_focusable_set(w, FALSE);
        p->new_panes = TRUE;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: The paned to set the orientation on
 * @param o: The Ewl_Orientation to set
 * @return Returns no value.
 * @brief Set the orientation of the paned */
void
ewl_paned_orientation_set(Ewl_Paned *p, Ewl_Orientation o)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        if (p->orientation == o)
                DRETURN(DLEVEL_STABLE);

        p->orientation = o;

        /* loop over all the children and change the orientation of all
          * of the grabbers */
        ecore_dlist_first_goto(EWL_CONTAINER(p)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(p)->children)))
        {
                if (EWL_PANED_GRABBER_IS(child))
                        ewl_paned_grabber_paned_orientation_set(
                                        EWL_PANED_GRABBER(child), o);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The Ewl_Paned to get the orientation from
 * @return Returns the Ewl_Orientation currently set on the paned
 * @brief Get the orientation of the paned widget
 */
Ewl_Orientation
ewl_paned_orientation_get(Ewl_Paned *p)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(p->orientation, DLEVEL_STABLE);
}

/**
 * @param p: The paned to work with
 * @param child: The child widget to set the initial size
 * @param size: the size to set
 * @return Returns no value.
 * @brief Set the initial size of a pane in px
 */
void
ewl_paned_initial_size_set(Ewl_Paned *p, Ewl_Widget *child, int size)
{
        Ewl_Paned_Size_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        info = ewl_paned_size_info_add(p, child);
        info->initial_size = size;
        info->initial_size_has = TRUE;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The paned to work with
 * @param child: The child widget to get the initial size
 * @return Returns the initial size that is set to the given widget. If there
 * isn't any size set, it'll return 0.
 * @brief Get the initial size of a pane in px
 */
int
ewl_paned_initial_size_get(Ewl_Paned *p, Ewl_Widget *child)
{
        Ewl_Paned_Size_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, 0);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, 0);

        info = ewl_paned_size_info_get(p, child);
        if (!info || !info->initial_size_has)
                DRETURN_INT(0, DLEVEL_STABLE);

        DRETURN_INT(info->initial_size, DLEVEL_STABLE);
}

/**
 * @param p: The paned to work with
 * @param child: The child widget to set the fixed size flag
 * @param fixed: the fixed size flag
 * @return Returns no value.
 * @brief Set the given widget to resize only by the grabber
 */
void
ewl_paned_fixed_size_set(Ewl_Paned *p, Ewl_Widget *child, unsigned int fixed)
{
        Ewl_Paned_Size_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        info = ewl_paned_size_info_add(p, child);
        info->fixed = !!fixed;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The paned to work with
 * @param child: The child widget to get the fixed size flag
 * @return Returns if the pane is set to keep its size while resizing
 * @brief Retrieves if the pane is set to keep its size while resizing
 */
unsigned int
ewl_paned_fixed_size_get(Ewl_Paned *p, Ewl_Widget *child)
{
        Ewl_Paned_Size_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, 0);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, 0);

        info = ewl_paned_size_info_get(p, child);
        if (!info)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        DRETURN_INT(info->fixed, DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_paned_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
        Ewl_Widget *o;
        int idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_PANED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* if this is a grabber we can skipp it */
        if (EWL_PANED_GRABBER_IS(w))
                DRETURN(DLEVEL_STABLE);

        o = ewl_paned_grabber_new();
        ewl_paned_grabber_paned_orientation_set(EWL_PANED_GRABBER(o),
                                ewl_paned_orientation_get(EWL_PANED(c)));

        /* insert the grabber at the same position as the pane so the
         * grabber ends up to the left of the pane in the children list */
        idx = ewl_container_child_index_get(c, w);
        ewl_container_child_insert(c, o, idx);
        ewl_paned_grabbers_update(EWL_PANED(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param idx: The index that the child was removed from
 * @return Returns no value
 * @brief The child remove callback
 */
void
ewl_paned_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx)
{
        Ewl_Paned *p;
        Ewl_Widget *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_PANED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        p = EWL_PANED(c);
        /* skip grabbers */
        if (EWL_PANED_GRABBER_IS(w))
                DRETURN(DLEVEL_STABLE);

        /* our grabber is always to our left, since we were just removed
         * from idx that means it's at idx - 1 */
        o = ewl_container_child_internal_get(c, idx - 1);
        ewl_widget_destroy(o);

        if (p->last_pane == w)
                p->last_pane = NULL;

        ewl_paned_size_info_del(p, w);
        ewl_paned_grabbers_update(EWL_PANED(c));

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
ewl_paned_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
        int cw, ch, ww, wh;
        Ewl_Paned_Size_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_PANED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_object_preferred_size_get(EWL_OBJECT(w), &ww, &wh);
        ewl_object_preferred_inner_size_get(EWL_OBJECT(c), &cw, &ch);

        EWL_PANED(c)->new_panes = TRUE;

        info = ewl_paned_size_info_get(EWL_PANED(c), w);

        if (EWL_ORIENTATION_HORIZONTAL == EWL_PANED(c)->orientation)
        {
                if (info && info->initial_size_has)
                        cw += info->initial_size;
                else
                        cw += ww;

                if (wh > ch) ch = wh;
        }
        else
        {
                if (info && info->initial_size_has)
                        ch += info->initial_size;
                else
                        ch += wh;

                if (ww > cw) cw = ww;
        }

        ewl_object_preferred_inner_size_set(EWL_OBJECT(c), cw, ch);
        ewl_paned_grabbers_update(EWL_PANED(c));

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
ewl_paned_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size, 
                                                        Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_PANED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (o == EWL_PANED(c)->orientation)
        {
                Ewl_Paned_Size_Info *info;

                info = ewl_paned_size_info_get(EWL_PANED(c), w);
                if (!info || !info->initial_size_has)
                {
                        if (o == EWL_ORIENTATION_HORIZONTAL)
                                ewl_object_preferred_inner_w_set(
                                                EWL_OBJECT(c), PREFERRED_W(c)
                                                + size);
                        else
                                ewl_object_preferred_inner_h_set(
                                                EWL_OBJECT(c), PREFERRED_H(c)
                                                + size);
                }
        }
        else
                ewl_container_largest_prefer(c, o);

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
ewl_paned_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
        int cw, ch, ww, wh;
        Ewl_Paned_Size_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_PANED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_object_preferred_size_get(EWL_OBJECT(w), &ww, &wh);
        ewl_object_preferred_inner_size_get(EWL_OBJECT(c), &cw, &ch);
        info = ewl_paned_size_info_get(EWL_PANED(c), w);

        if (EWL_ORIENTATION_HORIZONTAL == EWL_PANED(c)->orientation)
        {
                if (info && info->initial_size_has)
                        cw -= info->initial_size;
                else
                        cw -= ww;
        }
        else
        {
                if (info && info->initial_size_has)
                        ch -= info->initial_size;
                else
                        ch -= wh;
        }

        ewl_object_preferred_inner_size_set(EWL_OBJECT(c), cw, ch);
        ewl_paned_grabbers_update(EWL_PANED(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The paned to work with
 * @param pane_num: The number of visible panes
 * @return Returns no value
 * @brief The arrange the child widgets
 *
 * This function is not to be intended to be used, if the widget is visible. Its
 * purpose is to calculate the new position of the children even if the widget
 * is hidden. This is useful if the paned serves as a size giver for a row,
 * like in it is done in the tree widget. Use this function only if you know
 * what you are doing.
 */
void
ewl_paned_arrange(Ewl_Paned *p, int pane_num)
{
        Ewl_Widget *w;
        Ewl_Paned_Pane_Info *panes;
        int available;
        int main_size, main_pos;
        int grabber_size, resizable = 0;
        int used_size;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        w = EWL_WIDGET(p);

        if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
        {
                layout = horizontal_layout;
                main_size = CURRENT_W(w);
                main_pos = CURRENT_X(w);
        }
        else
        {
                layout = vertical_layout;
                main_size = CURRENT_H(w);
                main_pos = CURRENT_Y(w);
        }

        /* we cannot place the panes if there aren't any */
        if (pane_num <= 0)
                DRETURN(DLEVEL_STABLE);

        /* setup the array holding the information about the panes */
        panes = alloca(sizeof(Ewl_Paned_Pane_Info) * pane_num);

        grabber_size = ewl_paned_grapper_size_get(p);

        /* if there are new widgets we place them first */
        if (p->new_panes)
                used_size = ewl_paned_pane_info_setup(p, panes, layout, 
                                                                &resizable);
        else
                used_size = ewl_paned_pane_info_collect(p, panes, layout,
                                                &resizable, grabber_size);

        
        available = main_size - grabber_size * (pane_num - 1) - used_size;

        ewl_paned_pane_info_layout(p, panes, pane_num, layout, available, resizable);
        /* now that all of the space is filled we can go and layout all of
         * the available widgets */
        used_size = ewl_paned_widgets_place(p, panes, grabber_size);

        p->last_size = used_size;
        p->last_pos = main_pos;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_paned_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        int pane_num;
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_PANED_TYPE);

        /* we need to now the number of panes */        
        pane_num = (ewl_container_child_count_visible_get(EWL_CONTAINER(w)) + 1)/2;

        ewl_paned_arrange(EWL_PANED(w), pane_num);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_paned_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Paned *p;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_PANED_TYPE);

        p = EWL_PANED(w);
        IF_FREE(p->info);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* Look at all of the grabbers and show/hide them as needed */
static void
ewl_paned_grabbers_update(Ewl_Paned *p)
{
        Ewl_Widget *child, *g = NULL;
        Ewl_Container *c;
        int left = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        if (p->updating_grabbers)
                DRETURN(DLEVEL_STABLE);

        p->updating_grabbers = 1;

        c = EWL_CONTAINER(p);
        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)))
        {
                /* internal means grabber */
                if (EWL_PANED_GRABBER_IS(child))
                {
                        /* if there is a widget to the left we store the
                         * grabber */
                        if (left) g = child;
                        ewl_widget_hide(child);
                }
                else if (VISIBLE(child) && !UNMANAGED(child))
                {
                        left = 1;

                        /* if we have a grabber to the left of us we need to
                         * show it as that means there is a widget to the
                         * left of us */
                        if (g)
                        {
                                ewl_widget_show(g);
                                g = NULL;
                        }
                }
        }

        p->updating_grabbers = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_paned_pane_info_setup(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes, 
                Ewl_Paned_Layout *layout, int *resizable)
{
        Ewl_Widget *child;
        Ewl_Container *c;
        int available = 0;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, 0);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, 0);
        DCHECK_PARAM_PTR_RET(layout, 0);

        c = EWL_CONTAINER(p);

        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)))
        {
                if (!VISIBLE(child) || EWL_PANED_GRABBER_IS(child) 
                                || UNMANAGED(child)) 
                        continue;

                panes->info = ewl_paned_size_info_get(p, child);
                panes->pane = child;
                
                if (panes->info && panes->info->initial_size_has)
                {
                        panes->size = panes->info->initial_size;
                        panes->fixed = TRUE;
                }
                else
                {
                        panes->size = layout->preferred_size(EWL_OBJECT(child));
                        (*resizable)++;
                        panes->fixed = FALSE;
                }

                available += panes->size;
                panes++;
        }

        p->new_panes = FALSE;

        DRETURN_INT(available, DLEVEL_STABLE);
}

static int
ewl_paned_pane_info_collect(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes, 
                Ewl_Paned_Layout *layout, int *resizable, int grabber_size)
{
        Ewl_Container *c;
        Ewl_Widget *child;
        int prev_pos;
        int i = 0;
        int available = 0;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, 0);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, 0);

        prev_pos = p->last_pos;
        c = EWL_CONTAINER(p);

        /* We haven't moved the widgets yet. Let us now get their 
         * previous position so we can determine the size of the
         * space they had */
        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)))
        {
                int pos;

                if (!VISIBLE(child) || UNMANAGED(child))
                        continue;
                if (!EWL_PANED_GRABBER_IS(child))
                {
                        panes[i].pane = child;
                        panes[i].info = ewl_paned_size_info_get(p, child);
                        if (panes[i].info && panes[i].info->fixed)
                                panes[i].fixed = TRUE;
                        else
                        {
                                panes[i].fixed = FALSE;
                                (*resizable)++;
                        }
                        
                        continue;
                }

                pos = layout->current_position(EWL_OBJECT(child));
                panes[i].size = pos - prev_pos;
                available += panes[i].size;
                prev_pos = pos + grabber_size;
                i++;
        }
        /* only the last position is not set because we have no grabber
         * at the end */
        panes[i].size = p->last_size - (prev_pos - p->last_pos);
        available += panes[i].size;
        
        DRETURN_INT(available, DLEVEL_STABLE);
}

static void
ewl_paned_pane_info_layout(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes, 
                                int pane_num, Ewl_Paned_Layout *layout, 
                                int available, int resizable)
{
        int cur_res = resizable;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        while (available != 0)
        {
                int give;
                int i;

                /* if we have no panes we don't need to calc their place,
                 * or, if cur_res is as low as it will get avoid endless
                 * loop
                 */
                if ((cur_res < 1) || (cur_res == (resizable - pane_num)))
                        DRETURN(DLEVEL_STABLE);

                /* give can also be negative, so see it as a can take or give */
                give = available / cur_res;
                /* reset the resizable pane_num now */
                cur_res = resizable;
                i = 0;
                /* to prevent rounding errors */
                if (give == 0) {
                        give = (available > 0) ? 1 : -1;
                        if (p->last_pane)
                        {
                                /* find the index to start with */
                                while (i < pane_num 
                                        &&  panes[i].pane != p->last_pane) i++;
                                /* on the next run we don't want to start 
                                 * with this widget */
                                p->last_pane = NULL;
                                i++;
                                if (i >= pane_num)
                                        i = 0;
                        }
                }

                for (;i < pane_num; i++)
                {
                        int min;

                        if (panes[i].fixed)
                        {
                                cur_res--;
                                continue;
                        }

                        min = layout->minimum_size(EWL_OBJECT(panes[i].pane));

                        if (min > panes[i].size + give) {
                                available -= panes[i].size - min;
                                panes[i].size = min;
                                cur_res--;
                        }
                        else {
                                available -= give;
                                panes[i].size += give;
                        }

                        /* if there is no space to distribute left
                         * we can stop in resizing the panes */
                        if (available == 0) {
                                p->last_pane = panes[i].pane;
                                break;
                        }
                }
        }
        
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_paned_widgets_place(Ewl_Paned *p, Ewl_Paned_Pane_Info *panes, 
                                int grabber_size)
{
        Ewl_Container *c;
        Ewl_Widget *child;
        int cur_pos, cur_size, first_pos;
        const int *x, *y, *w, *h;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, 0);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, 0);
        
        c = EWL_CONTAINER(p);

        if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
        {
                first_pos = cur_pos = CURRENT_X(p);
                x = &cur_pos;
                y = &CURRENT_Y(p);
                w = &cur_size;
                h = &CURRENT_H(p);
        }
        else
        {
                first_pos = cur_pos = CURRENT_Y(p);
                x = &CURRENT_X(p);
                y = &cur_pos;
                w = &CURRENT_W(p);
                h = &cur_size;
        }

        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)))
        {
                if (!VISIBLE(child) || UNMANAGED(child))
                        continue;

                if (EWL_PANED_GRABBER_IS(child))
                        cur_size = grabber_size;
                else
                        cur_size = (panes++)->size;

                ewl_object_place(EWL_OBJECT(child), *x, *y, *w, *h);

                cur_pos += cur_size;
        }

        DRETURN_INT(cur_pos - first_pos, DLEVEL_STABLE);
}

static int
ewl_paned_grapper_size_get(Ewl_Paned *p)
{
        Ewl_Container *c;
        Ewl_Widget *child;

        c = EWL_CONTAINER(p);
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, 0);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, 0);

        ecore_list_first_goto(c->children);
        while ((child = ecore_list_next(c->children)))
        {
                if (!VISIBLE(child) || !EWL_PANED_GRABBER_IS(child))
                        continue;
                
                       DRETURN_INT(layout->current_size(EWL_OBJECT(child)), 
                                DLEVEL_STABLE);
        }

        DRETURN_INT(0, DLEVEL_STABLE);
}


/*
 * Ewl_Paned_Size_Info stuff
 */
Ewl_Paned_Size_Info *
ewl_paned_size_info_add(Ewl_Paned *p, Ewl_Widget *w)
{
        Ewl_Paned_Size_Info *info;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, NULL);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, NULL);


        /* if we already have a info item for this widget we don't need 
         * to create one */
        info = ewl_paned_size_info_get(p, w);
        if (info)
                DRETURN_PTR(info, DLEVEL_STABLE);

        /* ok we don't have found one first we have to resize our array */
        REALLOC(p->info, Ewl_Paned_Size_Info, p->info_size + 1);

        /* now we need to find the position for the new info item */
        for (i = 0; i < p->info_size; i++)
        {
                if (p->info[i].child > w)
                        break;
        }
        /* move the rest so we can insert it */
        memmove(p->info + i + 1, p->info + i, 
                        sizeof(Ewl_Paned_Size_Info) * (p->info_size - i));
        memset(p->info + i, 0, sizeof(Ewl_Paned_Size_Info));
        p->info[i].child = w;
        p->info_size++;

        DRETURN_PTR(p->info + i, DLEVEL_STABLE);
}

static int
ewl_paned_size_info_compare(const void *key1, const void *key2)
{
        const Ewl_Paned_Size_Info *info1, *info2;

        DENTER_FUNCTION(DLEVEL_STABLE);

        info1 = key1;
        info2 = key2;

        DRETURN_INT(ecore_direct_compare(info1->child, info2->child),
                        DLEVEL_STABLE);
}

Ewl_Paned_Size_Info *
ewl_paned_size_info_get(Ewl_Paned *p, Ewl_Widget *w)
{
        Ewl_Paned_Size_Info info;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(p, NULL);
        DCHECK_TYPE_RET(p, EWL_PANED_TYPE, NULL);

        memset(&info, 0, sizeof(Ewl_Paned_Size_Info));
        info.child = w;

        DRETURN_PTR(bsearch(&info, p->info, p->info_size, 
                        sizeof(Ewl_Paned_Size_Info), 
                        ewl_paned_size_info_compare), DLEVEL_STABLE);
}

void
ewl_paned_size_info_del(Ewl_Paned *p, Ewl_Widget *w)
{
        Ewl_Paned_Size_Info *info;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_PANED_TYPE);

        info = ewl_paned_size_info_get(p, w);
        if (!info)
                DRETURN(DLEVEL_STABLE);
        
        memmove(info, info + 1, (p->info_size - (info - p->info) - 1) 
                                * sizeof(Ewl_Paned_Size_Info));
        REALLOC(p->info, Ewl_Paned_Size_Info, --p->info_size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Paned_Grabber stuff
 */

/**
 * @internal
 * @return Returns a new Ewl_Paned_Grabber widget or NULL on failure
 */
Ewl_Widget *
ewl_paned_grabber_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Paned_Grabber, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_paned_grabber_init(EWL_PANED_GRABBER(w)))
        {
                ewl_widget_destroy(w);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @internal
 * @param g: The Ewl_Paned_Grabber to initialize
 * @return Returns TRUE on success or FALSE on failure
 */
int
ewl_paned_grabber_init(Ewl_Paned_Grabber *g)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(g, FALSE);

        if (!ewl_separator_init(EWL_SEPARATOR(g)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(g), EWL_PANED_GRABBER_TYPE);
        ewl_paned_grabber_paned_orientation_set(g, EWL_ORIENTATION_VERTICAL);

        ewl_callback_append(EWL_WIDGET(g), EWL_CALLBACK_MOUSE_DOWN,
                                        ewl_paned_grabber_cb_mouse_down, NULL);
        ewl_callback_append(EWL_WIDGET(g), EWL_CALLBACK_MOUSE_UP,
                                        ewl_paned_grabber_cb_mouse_up, NULL);

        ewl_widget_internal_set(EWL_WIDGET(g), TRUE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param g: The Ewl_Paned_Grabber to set the orientation on
 * @param o: The Ewl_Orientation to set on the grabber
 * @return Returns no value.
 */
void
ewl_paned_grabber_paned_orientation_set(Ewl_Paned_Grabber *g, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(g);
        DCHECK_TYPE(g, EWL_PANED_GRABBER_TYPE);

        if (o == EWL_ORIENTATION_HORIZONTAL)
        {
                ewl_separator_orientation_set(EWL_SEPARATOR(g),
                                                EWL_ORIENTATION_VERTICAL);
                ewl_widget_appearance_set(EWL_WIDGET(g), "grabber/vertical");
                ewl_paned_grabber_show_cursor_for(g,
                                        EWL_POSITION_LEFT | EWL_POSITION_RIGHT);
        }
        else
        {
                ewl_separator_orientation_set(EWL_SEPARATOR(g),
                                                EWL_ORIENTATION_HORIZONTAL);
                ewl_widget_appearance_set(EWL_WIDGET(g), "grabber/horizontal");
                ewl_paned_grabber_show_cursor_for(g,
                                        EWL_POSITION_TOP | EWL_POSITION_BOTTOM);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param g: The Ewl_Paned_Grabber to get the orientation from
 * @return Returns the Ewl_Orientation set on the grabber
 */
Ewl_Orientation 
ewl_paned_grabber_orientation_get(Ewl_Paned_Grabber *g)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(g, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(g, EWL_PANED_GRABBER_TYPE,
                                        EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(ewl_separator_orientation_get(EWL_SEPARATOR(g)),
                                                DLEVEL_STABLE);
}

/**
 * @internal
 * @param g: The Ewl_Paned_Grabber to set the cursor for
 * @param dir: The diretions to show arrows for
 * @return Returns no value.
 *
 * @brief This will show the arrows to allow the grabber to move in the
 * directions specified by @a dir.
 */
void
ewl_paned_grabber_show_cursor_for(Ewl_Paned_Grabber *g, unsigned int dir)
{
        int pos = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(g);
        DCHECK_TYPE(g, EWL_PANED_GRABBER_TYPE);

        if ((dir & EWL_POSITION_LEFT) && (dir & EWL_POSITION_RIGHT))
                pos = EWL_MOUSE_CURSOR_SB_H_DOUBLE_ARROW;
        else if ((dir & EWL_POSITION_TOP) && (dir & EWL_POSITION_BOTTOM))
                pos = EWL_MOUSE_CURSOR_SB_V_DOUBLE_ARROW;
        else if (dir & EWL_POSITION_LEFT)
                pos = EWL_MOUSE_CURSOR_SB_LEFT_ARROW;
        else if (dir & EWL_POSITION_RIGHT)
                pos = EWL_MOUSE_CURSOR_SB_RIGHT_ARROW;
        else if (dir & EWL_POSITION_TOP)
                pos = EWL_MOUSE_CURSOR_SB_UP_ARROW;
        else
                pos = EWL_MOUSE_CURSOR_SB_DOWN_ARROW;

        ewl_attach_mouse_cursor_set(EWL_WIDGET(g), pos);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev,
                                                void *data __UNUSED__)
{
        Ewl_Paned *p;
        Ewl_Event_Mouse *event;
        int ds;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        event = ev;
        p = EWL_PANED(w->parent);

        if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
                ds = CURRENT_X(w) - event->x;
        else
                ds = CURRENT_Y(w) - event->y;
        
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
                                ewl_paned_grabber_cb_mouse_move,
                                INT_TO_INTPTR(ds));
        ewl_widget_state_set(w, "selected", EWL_STATE_PERSISTENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_up(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE,
                                ewl_paned_grabber_cb_mouse_move);
        ewl_widget_state_set(w, "default", EWL_STATE_PERSISTENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Event_Mouse *e;
        Ewl_Paned_Grabber *stop_grabber = NULL;
        Ewl_Widget *child;
        Ewl_Container *c;
        Ewl_Paned *p;
        Ewl_Widget *prev_pane = NULL, *next_pane = NULL;

        int paned_pos, paned_size;
        int prev_grabber_pos, next_grabber_pos;
        int grabber_pos, grabber_size;
        int mouse_pos, mouse_vec, mouse_offset;
        int grabber_pos_new;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        e = ev;
        p = EWL_PANED(w->parent);
        c = EWL_CONTAINER(p);
        mouse_offset = INTPTR_TO_INT(data);

        if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
        {
                layout = horizontal_layout;
                mouse_pos = e->x;
                paned_pos = CURRENT_X(p);
                paned_size = CURRENT_W(p);
        }
        else
        {
                layout = vertical_layout;
                mouse_pos = e->y;
                paned_pos = CURRENT_Y(p);
                paned_size = CURRENT_H(p);
        }

        grabber_pos = layout->current_position(EWL_OBJECT(w));
        grabber_size = layout->current_size(EWL_OBJECT(w));

        /* this is the vector pointing from the left/top edge of the grabber
         * to the mouse position, so is it neagtive the grabber will
         * be moved to the left side and is it positiv to the right
         */
        mouse_vec = mouse_pos - grabber_pos + mouse_offset;
        if (mouse_vec == 0)
                DRETURN(DLEVEL_STABLE);

        /* find the previous grabber that is blocking us */
        ecore_dlist_goto(c->children, w);

        /* move past the selected grabber */
        ecore_dlist_previous(c->children);
        while ((child = ecore_dlist_previous(c->children)))
        {
                if (!VISIBLE(child) || UNMANAGED(child)) continue;

                if (EWL_PANED_GRABBER_IS(child))
                {
                        stop_grabber = EWL_PANED_GRABBER(child);
                        break;
                }
                else
                        prev_pane = child;
        }

        /* if we didn't find a privous grabber set the paned position instead */
        if (stop_grabber)
                prev_grabber_pos =
                        layout->current_position(EWL_OBJECT(stop_grabber))
                        + grabber_size;
        else
                prev_grabber_pos = paned_pos;

        /* and now find the right/bottom pane */
        ecore_dlist_goto(c->children, w);
        stop_grabber = NULL;

        /* move past the selected grabber */
        ecore_dlist_next(c->children);
        while ((child = ecore_list_next(c->children)))
        {
                if (!VISIBLE(child) || UNMANAGED(child)) continue;

                if (EWL_PANED_GRABBER_IS(child)) {
                        stop_grabber = EWL_PANED_GRABBER(child);
                        break;
                }
                else
                        next_pane = child;
        }
        
        /* if we didn't find a prevous grabber set the paned position instead */
        if (stop_grabber)
                next_grabber_pos = 
                        layout->current_position(EWL_OBJECT(stop_grabber));
        else
                next_grabber_pos = paned_pos + paned_size;

        /*
         * now we have collected enought data to place the grabber
         * and the panes on their new places
         */
        /* we don't want to shrink the panes more that it is allowed */
        if (mouse_vec < 0) {
                /* the left side get shrinked */
                int pane_min = layout->minimum_size(EWL_OBJECT(prev_pane));
                if (grabber_pos + mouse_vec - prev_grabber_pos < pane_min)
                        grabber_pos_new = prev_grabber_pos + pane_min;
                else
                        /* note that mouse_vec is here negative! */
                        grabber_pos_new = grabber_pos + mouse_vec;
        }
        else {
                /* the right/bottom side get shrinked */
                int pane_min = layout->minimum_size(EWL_OBJECT(next_pane));
                if (next_grabber_pos - (grabber_pos + mouse_vec + grabber_size)
                                < pane_min)
                        grabber_pos_new = 
                                next_grabber_pos - pane_min - grabber_size;
                else
                        grabber_pos_new = grabber_pos + mouse_vec;
        }

        /*
         * finally we can place the stuff
         */
        if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL) {
                ewl_object_place(EWL_OBJECT(prev_pane), prev_grabber_pos,
                                        CURRENT_Y(p),
                                        grabber_pos_new - prev_grabber_pos,
                                        CURRENT_H(p));
                ewl_object_place(EWL_OBJECT(w), grabber_pos_new,
                                        CURRENT_Y(p),
                                        grabber_size,
                                        CURRENT_H(p));
                ewl_object_place(EWL_OBJECT(next_pane),
                                        grabber_pos_new + grabber_size,
                                        CURRENT_Y(p),
                                        next_grabber_pos - grabber_pos_new
                                        - grabber_size,
                                        CURRENT_H(p));
        }
        else {
                ewl_object_place(EWL_OBJECT(prev_pane),
                                        CURRENT_X(p),
                                        prev_grabber_pos,
                                        CURRENT_W(p),
                                        grabber_pos_new - prev_grabber_pos);
                ewl_object_place(EWL_OBJECT(w),
                                        CURRENT_X(p),
                                        grabber_pos_new,
                                        CURRENT_W(p),
                                        grabber_size);
                ewl_object_place(EWL_OBJECT(next_pane),
                                        CURRENT_X(p),
                                        grabber_pos_new + grabber_size,
                                        CURRENT_W(p),
                                        next_grabber_pos - grabber_pos_new
                                        - grabber_size);
        }

        /* send a value_changed callback to signal something moved */
        ewl_callback_call(EWL_WIDGET(p), EWL_CALLBACK_VALUE_CHANGED);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_layout_setup(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        ewl_shutdown_add(ewl_paned_cb_shutdown);

        horizontal_layout = NEW(Ewl_Paned_Layout, 1);
        if (!horizontal_layout)
                DRETURN(DLEVEL_STABLE);

        horizontal_layout->minimum_size = ewl_object_minimum_w_get;
        horizontal_layout->current_size = ewl_object_current_w_get;
        horizontal_layout->preferred_size = ewl_object_preferred_w_get;
        horizontal_layout->current_position = ewl_object_current_x_get;
        horizontal_layout->variable_request = ewl_object_w_request;
        horizontal_layout->stable_request = ewl_object_h_request;
        horizontal_layout->position_request = ewl_object_x_request;
        horizontal_layout->stable_position_request = ewl_object_y_request;

        vertical_layout = NEW(Ewl_Paned_Layout, 1);
        if (!vertical_layout) 
                DRETURN(DLEVEL_STABLE);
        
        vertical_layout->minimum_size = ewl_object_minimum_h_get;
        vertical_layout->current_size = ewl_object_current_h_get;
        vertical_layout->preferred_size = ewl_object_preferred_h_get;
        vertical_layout->current_position = ewl_object_current_y_get;
        vertical_layout->variable_request = ewl_object_h_request;
        vertical_layout->stable_request = ewl_object_w_request;
        vertical_layout->position_request = ewl_object_y_request;
        vertical_layout->stable_position_request = ewl_object_x_request;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_cb_shutdown(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        IF_FREE(horizontal_layout);
        IF_FREE(vertical_layout);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

