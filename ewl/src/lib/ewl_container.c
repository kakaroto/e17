/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

#include <Evas.h>

static void ewl_container_child_insert_helper(Ewl_Container *pc,
                                                Ewl_Widget *child,
                                                int index,
                                                int skip_internal);
static int ewl_container_child_count_get_helper(Ewl_Container *c, int skip);

/**
 * @return Returns a newly allocated container on success, NULL on failure.
 * @brief Allocate a new container.
 *
 * Do not use this function unless you know what you are doing! It is only
 * intended to easily create custom container. It doesn't manage the children.
 */
Ewl_Widget *
ewl_container_new(void)
{
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);

        c = NEW(Ewl_Container, 1);
        if (!c)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_container_init(c)) {
                FREE(c);
                c = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(c), DLEVEL_STABLE);
}


/**
 * @param c: the container to initialize
 * @return Returns TRUE on success, otherwise FALSE.
 * @brief Initialize a containers default fields and callbacks
 *
 * Initializes the default values of the container, this also sets up the
 * widget fields of the container.
 */
int
ewl_container_init(Ewl_Container *c)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);

        w = EWL_WIDGET(c);

        /*
         * Initialize the fields inherited from the widget class
         */
        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(w, EWL_CONTAINER_TYPE);
        ewl_widget_recursive_set(w, TRUE);

        /*
         * Initialize the fields specific to the container class.
         */
        c->children = ecore_dlist_new();

        /*
         * All containers need to perform the function of updating the
         * children with necessary window and evas information.
         */
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                            ewl_container_cb_configure, NULL);
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE,
                            ewl_container_cb_obscure, NULL);
        ewl_callback_append(w, EWL_CALLBACK_REVEAL,
                            ewl_container_cb_reveal, NULL);
        ewl_callback_append(w, EWL_CALLBACK_REALIZE,
                            ewl_container_cb_reveal, NULL);
        ewl_callback_append(w, EWL_CALLBACK_REALIZE,
                            ewl_container_cb_realize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_UNREALIZE,
                            ewl_container_cb_unrealize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_REPARENT,
                            ewl_container_cb_reparent, NULL);
        ewl_callback_append(w, EWL_CALLBACK_WIDGET_ENABLE,
                            ewl_container_cb_enable, NULL);
        ewl_callback_append(w, EWL_CALLBACK_WIDGET_DISABLE,
                            ewl_container_cb_disable, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param container: the container to change the add notifier
 * @param add: the new add notifier for the container
 * @return Returns no value.
 * @brief Set the function to be called when adding children
 *
 * Changes the add notifier function of @a container to @a add.
 */
void
ewl_container_add_notify_set(Ewl_Container *container, Ewl_Child_Add add)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(container);
        DCHECK_TYPE(container, EWL_CONTAINER_TYPE);

        container->child_add = add;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param container: the container to change the add notifier
 * @param remove: the new remove notifier for the container
 * @return Returns no value.
 * @brief Set the function to be called when removing children
 *
 * Changes the remove notifier function of @a container to @a remove.
 */
void
ewl_container_remove_notify_set(Ewl_Container *container, Ewl_Child_Remove remove)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(container);
        DCHECK_TYPE(container, EWL_CONTAINER_TYPE);

        container->child_remove = remove;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param container: the container to change the resize notifier
 * @param resize: the new resizenotifier for the container
 * @return Returns no value.
 * @brief Set function to be called when children resize
 *
 * Changes the resize notifier function of @a container to @a resize.
 */
void
ewl_container_resize_notify_set(Ewl_Container *container, Ewl_Child_Resize resize)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(container);
        DCHECK_TYPE(container, EWL_CONTAINER_TYPE);

        container->child_resize = resize;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param container: the container to change the show notifier
 * @param show: the new show notifier for the container
 * @return Returns no value.
 * @brief Set the function to be called when showing children
 *
 * Changes the show notifier function of @a container to @a show.
 */
void
ewl_container_show_notify_set(Ewl_Container *container, Ewl_Child_Show show)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(container);
        DCHECK_TYPE(container, EWL_CONTAINER_TYPE);

        container->child_show = show;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param container: the container to change the hide notifier
 * @param hide: the new show notifier for the container
 * @return Returns no value.
 * @brief Set the function to be called when hideing children
 *
 * Changes the hide notifier function of @a container to @a hide.
 */
void
ewl_container_hide_notify_set(Ewl_Container *container, Ewl_Child_Hide hide)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(container);
        DCHECK_TYPE(container, EWL_CONTAINER_TYPE);

        container->child_hide = hide;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pc: the parent container that will hold the child
 * @param child: the child to add to the container
 * @return Returns no value.
 * @brief Add a child at the end of the container
 *
 * Attaches the child to the end of the parent containers child list.
 */
void
ewl_container_child_append(Ewl_Container *pc, Ewl_Widget *child)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pc);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(pc, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        if (pc == EWL_CONTAINER(child->parent))
                DRETURN(DLEVEL_STABLE);

        if (ewl_widget_parent_of(child, EWL_WIDGET(pc))) {
                DWARNING("Attempting to make a child a parent of itself.");
                DRETURN(DLEVEL_STABLE);
        }

        while (pc->redirect)
                pc = pc->redirect;

        ewl_widget_parent_set(child, EWL_WIDGET(pc));
        ecore_dlist_append(pc->children, child);
        ewl_container_child_add_call(pc, child);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pc: the parent container that will hold the child
 * @param child: the child to add to the container
 * @return Returns no value.
 * @brief Add a child at the beginning of the container
 *
 * Attaches the child to the start of the parent containers child list.
 */
void
ewl_container_child_prepend(Ewl_Container *pc, Ewl_Widget *child)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pc);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(pc, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        if (pc == EWL_CONTAINER(child->parent))
                DRETURN(DLEVEL_STABLE);

        if (ewl_widget_parent_of(child, EWL_WIDGET(pc))) {
                DWARNING("Attempting to make a child a parent of itself.");
                DRETURN(DLEVEL_STABLE);
        }

        while (pc->redirect)
                pc = pc->redirect;

        ewl_widget_parent_set(child, EWL_WIDGET(pc));
        ecore_dlist_prepend(pc->children, child);
        ewl_container_child_add_call(pc, child);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_container_child_insert_helper(Ewl_Container *pc, Ewl_Widget *child,
                                                int index, int skip_internal)
{
        Ewl_Widget *cur;
        int idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pc);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(pc, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        /* already inserted */
        if (pc == EWL_CONTAINER(child->parent))
                DRETURN(DLEVEL_STABLE);

        if (ewl_widget_parent_of(child, EWL_WIDGET(pc))) {
                DWARNING("Attempting to make a child a parent of itself.");
                DRETURN(DLEVEL_STABLE);
        }

        /* find the real container */
        while (pc->redirect)
                pc = pc->redirect;

        /* find our insertion point */
        ecore_dlist_first_goto(pc->children);
        while ((cur = ecore_dlist_current(pc->children)))
        {
                if (skip_internal && ewl_widget_internal_is(cur))
                {
                        ecore_dlist_next(pc->children);
                        continue;
                }

                if (idx == index) break;

                idx++;
                ecore_dlist_next(pc->children);
        }

        ewl_widget_parent_set(child, EWL_WIDGET(pc));
        ecore_dlist_insert(pc->children, child);
        ewl_container_child_add_call(pc, child);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pc: the parent container that will hold the child
 * @param child: the child to add to the container
 * @param index: the position in the child list to add the child (not
 * including internal widgets
 * @return Returns no value.
 * @brief Add a child at an index of the container
 *
 * Attaches the child to the @a index position of the parent containers child
 * list.
 */
void
ewl_container_child_insert(Ewl_Container *pc, Ewl_Widget *child, int index)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pc);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(pc, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        ewl_container_child_insert_helper(pc, child, index, TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pc: the parent container that will hold the child
 * @param child: the child to add to the container
 * @param index: the position in the child list to add the cihld
 * @return Returns no value.
 * @brief Add a child at an index of the container
 *
 * Attaches the child to the @a index position of the parent containers child
 * list.
 */
void
ewl_container_child_insert_internal(Ewl_Container *pc,
                                   Ewl_Widget *child, int index)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pc);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(pc, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        ewl_container_child_insert_helper(pc, child, index, FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param pc: the container to search for the child to remove
 * @param child: the child to remove from the container
 * @return Returns no value.
 * @brief Remove the specified child from the container
 *
 * Removes the specified child from the container without destroying the child.
 */
void
ewl_container_child_remove(Ewl_Container *pc, Ewl_Widget *child)
{
        Ewl_Widget *temp;
        int idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(pc);
        DCHECK_PARAM_PTR(child);
        DCHECK_TYPE(pc, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(child, EWL_WIDGET_TYPE);

        if (child == EWL_WIDGET(pc->redirect))
                pc->redirect = NULL;

        /*
         * First remove reference to the parent if necessary.
         * Bail out after setting the parent as that will get us back here
         * with a NULL parent on the widget.
         */
        if (child->parent) {
                ewl_widget_parent_set(child, NULL);
                DRETURN(DLEVEL_STABLE);
        }

        if (!pc->children) {
                DWARNING("The container %p (%s) doesn't have a children list",
                                pc, ewl_widget_appearance_get(EWL_WIDGET(pc)));
                DRETURN(DLEVEL_STABLE);
        }

        /*
         * Traverse the list to the child.
         */
        temp = ecore_dlist_goto(pc->children, child);

        /*
         * If the child isn't found, then this isn't it's parent.
         */
        if (!temp) {
                DWARNING("The container %p (%s) is not a parent of the widget "
                                "%p (%s)!",
                                pc, ewl_widget_appearance_get(EWL_WIDGET(pc)),
                                child, ewl_widget_appearance_get(child));
                DRETURN(DLEVEL_STABLE);
        }

        /* get the index of the widget we are removing */
        idx = ecore_dlist_index(pc->children);

        /*
         * Remove the child from the parent and set the childs parent to NULL
         */
        ecore_dlist_remove(pc->children);
        if (VISIBLE(child) && REALIZED(child))
                ewl_container_child_hide_call(pc, child);

        ewl_container_child_remove_call(pc, child, idx);
        ewl_widget_configure(EWL_WIDGET(pc));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_container_child_count_get_helper(Ewl_Container *c, int skip)
{
        Ewl_Widget *child = NULL;
        Ewl_Container *container = NULL;
        int count = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, 0);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, 0);

        /*
         * Find the container where children are actually added.
         */
        container = c;
        while (container->redirect) container = container->redirect;

        ecore_dlist_first_goto(container->children);
        while ((child = ecore_dlist_next(container->children)))
        {
                if (skip && ewl_widget_internal_is(child)) continue;
                count++;
        }

        DRETURN_INT(count, DLEVEL_STABLE);
}

/**
 * @param c: The container to get the child count from
 * @return Returns the number of child widgets
 * @brief Returns the number of child widgets in the container
 */
int
ewl_container_child_count_get(Ewl_Container *c)
{
        int count = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, 0);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, 0);

        count = ewl_container_child_count_get_helper(c, TRUE);

        DRETURN_INT(count, DLEVEL_STABLE);
}

/**
 * @param c: The container to get the child count from
 * @return Returns the number of child widgets
 * @brief Returns the number of child widgets in the container
 */
int
ewl_container_child_count_internal_get(Ewl_Container *c)
{
        int count = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, 0);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, 0);

        count = ewl_container_child_count_get_helper(c, FALSE);

        DRETURN_INT(count, DLEVEL_STABLE);
}

/**
 * @param c: The container to get the child count from
 * @return Returns the number of visible child widgets
 * @brief Returns the number of visible child widgets in the container
 *
 * This function return the number of visible child widgets in this container,
 * no matter if the container is set to redirect or not.
 * 
 * @note Other than ewl_container_count_get() and
 * ewl_container_count_internal_get() it does not return the number of
 * children in the redirection end container.
 */
int
ewl_container_child_count_visible_get(Ewl_Container *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, 0);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, 0);

        DRETURN_INT(c->visible_children, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_container_child_helper_get(Ewl_Container *parent, int index,
                                                unsigned int skip)
{
        Ewl_Container *container = NULL;
        Ewl_Widget *child;
        int count = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(parent, NULL);
        DCHECK_TYPE_RET(parent, EWL_CONTAINER_TYPE, NULL);

        container = parent;
        while (container->redirect) container = container->redirect;

        ecore_dlist_first_goto(container->children);

        while ((child = ecore_dlist_next(container->children))) {
                if (skip && ewl_widget_internal_is(child)) continue;
                if (count == index) break;
                count ++;
        }

        DRETURN_PTR(((count == index) ? child : NULL), DLEVEL_STABLE);
}

/**
 * @param parent: The container to get the child from
 * @param index: The child index to return
 * @return Returns the widget at the given index, or NULL if not found
 * @brief Retrieve the child at the given index in the container.
 */
Ewl_Widget *
ewl_container_child_get(Ewl_Container *parent, int index)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(parent, NULL);
        DCHECK_TYPE_RET(parent, EWL_CONTAINER_TYPE, NULL);

        w = ewl_container_child_helper_get(parent, index, TRUE);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param parent: The container to get the child from
 * @param index: The child index to return
 * @return Returns the widget at the given index including internal widgets,
 * or NULL if not found
 * @brief Retrieves the child at the given index in the container taking
 * internal widgets into account.
 */
Ewl_Widget *
ewl_container_child_internal_get(Ewl_Container *parent, int index)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(parent, NULL);
        DCHECK_TYPE_RET(parent, EWL_CONTAINER_TYPE, NULL);

        w = ewl_container_child_helper_get(parent, index, FALSE);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

static int
ewl_container_child_index_helper_get(Ewl_Container *parent, Ewl_Widget *w,
                                                        unsigned int skip)
{
        unsigned int idx = 0;
        int ret = -1;
        Ewl_Container *container;
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(parent, ret);
        DCHECK_PARAM_PTR_RET(w, ret);
        DCHECK_TYPE_RET(parent, EWL_CONTAINER_TYPE, ret);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, ret);

        container = parent;
        while (container->redirect) container = container->redirect;

        ecore_dlist_first_goto(container->children);
        while ((child = ecore_dlist_next(container->children))) {
                if (skip && ewl_widget_internal_is(child)) continue;
                if (child == w)
                {
                        ret = idx;
                        break;
                }
                idx ++;
        }

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param parent: The container to search
 * @param w: The child to search for
 * @return Returns the index of the child in the parent or -1 if not found
 * @brief Retrieves the index of the given child in the container or -1 if
 * not found
 */
int
ewl_container_child_index_get(Ewl_Container *parent, Ewl_Widget *w)
{
        int idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(parent, idx);
        DCHECK_PARAM_PTR_RET(w, idx);
        DCHECK_TYPE_RET(parent, EWL_CONTAINER_TYPE, idx);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, idx);

        idx = ewl_container_child_index_helper_get(parent, w, TRUE);

        DRETURN_INT(idx, DLEVEL_STABLE);
}

/**
 * @param parent: The container to search
 * @param w: The child to search for
 * @return Returns the index of the child in the parent including internal
 * widgets or -1 if not found
 * @brief Retrieves the index of the given child in the container taking
 * internal widgets into account or -1 if not found
 */
int
ewl_container_child_index_internal_get(Ewl_Container *parent, Ewl_Widget *w)
{
        int idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(parent, idx);
        DCHECK_PARAM_PTR_RET(w, idx);
        DCHECK_TYPE_RET(parent, EWL_CONTAINER_TYPE, idx);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, idx);

        idx = ewl_container_child_index_helper_get(parent, w, FALSE);

        DRETURN_INT(idx, DLEVEL_STABLE);
}

/**
 * @param w: the child widget that has had it's preferred size changed
 * @param size: the amount of change in size
 * @param o: the orientation of the size change
 * @return Returns no value.
 * @brief Notify a container of a child pref size change
 */
void
ewl_container_child_resize(Ewl_Widget *w, int size, Ewl_Orientation o)
{
        int old_w, old_h;
        int new_w, new_h;
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!size || !REALIZED(w) || ewl_widget_queued_has(w,
                                                EWL_FLAG_QUEUED_SCHEDULED_REVEAL))
                DRETURN(DLEVEL_STABLE);

        c = EWL_CONTAINER(w->parent);

        /*
         * If there is no parent to this widget, or it hasn't really changed
         * size just exit. Also exit if it has no function to be notified for
         * child resizes.
         */
        if (HIDDEN(w) || !c || !c->child_resize)
                DRETURN(DLEVEL_STABLE);

        /*
         * Store the parents current size so we can determine if it changes
         */
        old_w = PREFERRED_W(w);
        old_h = PREFERRED_H(w);

        /*
         * Run the parents child resize function to allow it to update it's
         * preferred size.
         */
        c->child_resize(c, w, size, o);

        /*
         * Get the new preferred size of the parent to see if it changed.
         */
        new_w = PREFERRED_W(w);
        new_h = PREFERRED_H(w);

        /*
         * The parent will only end up on the configure queue if it didn't
         * change size (ie. it's parent isn't on the configure queue).
         */
        if (EWL_WIDGET(c)->parent)
                ewl_widget_configure(EWL_WIDGET(c)->parent);
        else
                ewl_widget_configure(EWL_WIDGET(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param widget: the searched container
 * @param x: the x coordinate to search for
 * @param y: the y coordinate to search for
 * @return Returns the found widget on success, NULL on failure.
 * @brief Find the child at the given coordinates
 *
 * The given container is searched to find any child that intersects the given
 * coordinates.
 */
Ewl_Widget *
ewl_container_child_at_get(Ewl_Container *widget, int x, int y)
{
        Ewl_Widget *found = NULL;
        Ewl_Widget *child = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(widget, NULL);
        DCHECK_TYPE_RET(widget, EWL_CONTAINER_TYPE, NULL);

        if (!widget->children || ecore_dlist_empty_is(widget->children))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ecore_dlist_first_goto(widget->children);

        /*
         * Search through the children to find an intersecting child.
         */
        while ((child = ecore_dlist_next(EWL_CONTAINER(widget)->children))) {
                if (VISIBLE(child) && !DISABLED(child)
                    && x >= (CURRENT_X(child) - INSET_LEFT(child))
                    && y >= (CURRENT_Y(child) - INSET_TOP(child))
                    && (CURRENT_X(child) + CURRENT_W(child) +
                            INSET_RIGHT(child)) >= x
                    && (CURRENT_Y(child) + CURRENT_H(child) +
                            INSET_BOTTOM(child)) >= y) {
                        if (TOPLAYERED(child)) {
                                found = child;
                                break;
                        }
                        if ((!found || ewl_widget_layer_priority_get(found) <=
                                                ewl_widget_layer_priority_get(child)))
                                found = child;
                }
        }

        DRETURN_PTR(found, DLEVEL_STABLE);
}

/**
 * @param widget: the widget to search for intersecting child
 * @param x: the x coordinate of the intersection point
 * @param y: the y coordinate of the intersection point
 * @return Returns the intersecting widget on success, NULL on failure.
 * @brief Find child that intersects coordinates
 */
Ewl_Widget *
ewl_container_child_at_recursive_get(Ewl_Container *widget, int x, int y)
{
        /*
         * These temporary variables allow for traversing recursive
         * containers without actually making recursive function calls.
         */
        Ewl_Widget *child = NULL;
        Ewl_Widget *child2 = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(widget, NULL);
        DCHECK_TYPE_RET(widget, EWL_CONTAINER_TYPE, NULL);

        if (!widget->children || ecore_dlist_empty_is(widget->children))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        /*
         * The starting point is the current widget
         */
        child = EWL_WIDGET(widget);

        /*
         * Now move down through the tree of widgets until the bottom layer is
         * found.
         */
        if (!DISABLED(child))
                child2 = ewl_container_child_at_get(EWL_CONTAINER(child), x, y);

        while (child2) {
                if (RECURSIVE(child2))
                        child = child2;
                else
                        DRETURN_PTR(child2, DLEVEL_STABLE);

                if (!DISABLED(child))
                        child2 = ewl_container_child_at_get(EWL_CONTAINER(child), x, y);
                else
                        child2 = NULL;
        }

        DRETURN_PTR((child2 ? child2 : child), DLEVEL_STABLE);
}

/**
 * @param c: the container to reset
 * @return Returns no value.
 * @brief Destroy all children of the container
 *
 * Destroys all the children of the container but not the container itself or
 * internally used widgets.
 */
void
ewl_container_reset(Ewl_Container *c)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        while (c->redirect)
                c = c->redirect;

        if (!c->children)
                DRETURN(DLEVEL_STABLE);

        /*
         * Loop through removing each child and destroying it.
         */
        ecore_dlist_first_goto(c->children);
        while ((w = ecore_dlist_current(c->children))) {
                if (!ewl_widget_flags_has(w,
                                        EWL_FLAG_PROPERTY_INTERNAL,
                                        EWL_FLAGS_PROPERTY_MASK)) {
                        ewl_widget_destroy(w);

                        /*
                         * Start over in case the list was modified from a
                         * callback.
                         */
                        ecore_dlist_first_goto(c->children);
                }
                else
                        ecore_dlist_next(c->children);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to begin iterating over it's children
 * @return Returns no value.
 * @brief Prepare the container to iterate over it's children.
 *
 * Notifies the container that it will need to start from the beginning of
 * it's child list. Do not call this if you want to begin iteration where it
 * was last left off, but be warned it's possible this won't be where you
 * expect.
 */
void
ewl_container_child_iterate_begin(Ewl_Container *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        while (c->redirect)
                c = c->redirect;

        ecore_dlist_first_goto(c->children);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to retrieve the next usable child
 * @brief Retrieve the next elligible child in a container.
 * @return Returns the next valid child on success, NULL on failure.
 */
Ewl_Widget *
ewl_container_child_next(Ewl_Container *c)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, NULL);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, NULL);

        while (c->redirect)
                c = c->redirect;

        if (c->iterator) {
                w = c->iterator(c);
        }
        else {
                while ((w = ecore_dlist_next(c->children)) &&
                                (ewl_widget_internal_is(w)));
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param c: the container to change child iterator functions
 * @param i: the iterator function for the container
 * @return Returns no value.
 * @brief Changes the iterator function for a container.
 *
 * Do not use this unless you know are writing a custom container of your own.
 */
void
ewl_container_child_iterator_set(Ewl_Container *c, Ewl_Container_Iterator i)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        c->iterator = i;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to intercept the callback
 * @param t: the type of callback to intercept
 * @return Returns no value.
 * @brief Receive a callback in place of children
 *
 * Marks the callbacks of type @a t that are directed to children to be
 * triggered on the container @a c, and not propagated to the receiving child.
 */
void
ewl_container_callback_intercept(Ewl_Container *c, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        EWL_CALLBACK_FLAG_INTERCEPT(EWL_WIDGET(c), t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to remove the interception
 * @param t: the type of callback to not intercept
 * @return Returns no value.
 * @brief Remove a callback interception from children
 *
 * Marks the callbacks of type @a t that are directed to children to be
 * propagated to the receiving child.
 */
void
ewl_container_callback_nointercept(Ewl_Container *c, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        EWL_CALLBACK_FLAG_NOINTERCEPT(EWL_WIDGET(c), t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to notify the callback
 * @param t: the type of callback to notify
 * @return Returns no value.
 * @brief receive a callback of a child
 *
 * Marks the callbacks of type @a t that are directed to children to be
 * triggered on the container @a c, and propagated to the receiving child.
 */
void
ewl_container_callback_notify(Ewl_Container *c, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        EWL_CALLBACK_FLAG_NOTIFY(EWL_WIDGET(c), t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to not notify the callback
 * @param t: the type of callback to not notify
 * @return Returns no value.
 * @brief receive a callback of a child
 *
 * Removes the notification flag set with ewl_container_callback_notify()
 */
void
ewl_container_callback_nonotify(Ewl_Container *c, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        EWL_CALLBACK_FLAG_NONOTIFY(EWL_WIDGET(c), t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to change preferred size
 * @param o: the orientation/direction to prefer the largest widget
 * @return Returns no value.
 * @brief Set preferred size to widest child
 *
 * This function can be used by any container which wishes
 * to set it's preferred width to that of it's widest child.
 */
void
ewl_container_largest_prefer(Ewl_Container *c, Ewl_Orientation o)
{
        Ewl_Object *child;
        int max_size = 0;
        int curr_size = 0;
        int (*get_size)(Ewl_Object *object);
        void (*set_size)(Ewl_Object *object, int size);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        if (o == EWL_ORIENTATION_HORIZONTAL) {
                get_size = ewl_object_preferred_w_get;
                set_size = ewl_object_preferred_inner_w_set;
        }
        else {
                get_size = ewl_object_preferred_h_get;
                set_size = ewl_object_preferred_inner_h_set;
        }

        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children))) {
                if (VISIBLE(child) && REALIZED(child)) {
                        curr_size = get_size(child);
                        if (curr_size > max_size)
                                max_size = curr_size;
                }
        }

        set_size(EWL_OBJECT(c), max_size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to use the child size sum in a specified direction
 * @param o: the orientation direction of the sum to use
 * @return Returns no value
 * @brief Set the sum orientation of the container
 */
void
ewl_container_sum_prefer(Ewl_Container *c, Ewl_Orientation o)
{
        Ewl_Object *child;
        int curr_size = 0;
        int (*get_size)(Ewl_Object *object);
        void (*set_size)(Ewl_Object *object, int size);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        if (o == EWL_ORIENTATION_HORIZONTAL) {
                get_size = ewl_object_preferred_w_get;
                set_size = ewl_object_preferred_inner_w_set;
        }
        else {
                get_size = ewl_object_preferred_h_get;
                set_size = ewl_object_preferred_inner_h_set;
        }

        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children))) {
                if (VISIBLE(child) && REALIZED(child))
                        curr_size += get_size(child);
        }

        set_size(EWL_OBJECT(c), curr_size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container receiving a new child widget
 * @param w: the child widget added to the container
 * @return Returns no value.
 * @brief Triggers the child_add callback for the container @a c.
 */
void
ewl_container_child_add_call(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* ignore unmanaged children */
        if (UNMANAGED(w))
                DRETURN(DLEVEL_STABLE);

        if (c->child_add)
                c->child_add(c, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container removing a child widget
 * @param w: the child widget removed from the container
 * @param idx: The index that the child was in when it was removed
 * @return Returns no value.
 * @brief Triggers the child_remove callback for the container @a c.
 */
void
ewl_container_child_remove_call(Ewl_Container *c, Ewl_Widget *w, int idx)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* do nothing if the container is being destroyed */
        if (DESTROYED(c))
                DRETURN(DLEVEL_STABLE);
        
        /* ignore unmanaged children */
        if (UNMANAGED(w))
                DRETURN(DLEVEL_STABLE);

        if (c->child_remove)
                c->child_remove(c, w, idx);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container receiving a new child widget
 * @param w: the child widget shown in the container
 * @return Returns no value.
 * @brief Triggers the child_show callback for the container @a c.
 */
void
ewl_container_child_show_call(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* ignore unmanaged children */
        if (UNMANAGED(w))
                DRETURN(DLEVEL_STABLE);

        c->visible_children++;
        if (c->visible_children > ecore_dlist_count(c->children))
                DWARNING("visible children count exceed total child count "
                                "(%d > %d)\n", c->visible_children,
                                ecore_dlist_count(c->children));

        if (c->child_show)
                c->child_show(c, w);

        /*
         * Only show it if there are visible children.
         */
        if (c->clip_box)
                evas_object_show(c->clip_box);

        ewl_widget_configure(EWL_WIDGET(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container receiving a new child widget
 * @param w: the child widget hidden in the container
 * @return Returns no value.
 * @brief Triggers the child_hide callback for the container @a c.
 */
void
ewl_container_child_hide_call(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* ignore unmanaged children */
        if (UNMANAGED(w))
                DRETURN(DLEVEL_STABLE);

        /* do nothing if the container is being destroyed */
        if (DESTROYED(c))
                DRETURN(DLEVEL_STABLE);

        c->visible_children--;
        if (c->visible_children < 0)
                DWARNING("visible_children is %d\n", c->visible_children);

        if (c->child_hide)
                c->child_hide(c, w);

        if (c->clip_box) {
                const Eina_List *clippees;
                clippees = evas_object_clipees_get(c->clip_box);
                if (!clippees)
                        evas_object_hide(c->clip_box);
        }

        ewl_widget_configure(EWL_WIDGET(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to destroy children
 * @return Returns no value.
 * @brief Destroy all the sub-children of the container.
 */
void
ewl_container_destroy(Ewl_Container *c)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        c->redirect = NULL;
        if (c->children) {
                /*
                 * Destroy any children still in the container. Do not remove
                 * in order to avoid list walking.
                 */
                while ((child = ecore_dlist_first_goto(c->children))) {
                        ewl_widget_destroy(child);
                }

                /*
                 * Destroy the container list and set it to NULL.
                 */
                ecore_dlist_destroy(c->children);
                c->children = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to retrieve it's redirection end container
 * @return Returns the container children are placed in, NULL if none.
 * @brief Searches for the last redirected container of the container.
 */
Ewl_Container *
ewl_container_end_redirect_get(Ewl_Container *c)
{
        Ewl_Container *rc = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, NULL);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, NULL);

        if (!c->redirect)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        while (c->redirect) {
                rc = c->redirect;
                c = c->redirect;
        }

        DRETURN_PTR(rc, DLEVEL_STABLE);
}

/**
 * @param c: the container to retrieve it's redirection container
 * @return Returns the container children are placed in, NULL if none.
 * @brief Retrieves for the redirected container of the container.
 */
Ewl_Container *
ewl_container_redirect_get(Ewl_Container *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, NULL);
        DCHECK_TYPE_RET(c, EWL_CONTAINER_TYPE, NULL);

        DRETURN_PTR(c->redirect, DLEVEL_STABLE);
}

/**
 * @param c: the container to change it's redirection container
 * @param rc: the new container to redirect children to
 * @return Returns no value.
 * @brief Changes the redirected container of the container.
 */
void
ewl_container_redirect_set(Ewl_Container *c, Ewl_Container *rc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(rc, EWL_CONTAINER_TYPE);

        c->redirect = rc;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief When reparenting a container, it's children need the updated
 * information about the container, such as the evas.
 */
void
ewl_container_cb_reparent(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        if (!EWL_CONTAINER(w)->children)
                DRETURN(DLEVEL_STABLE);

        /*
         * Reparent all of the containers children
         */
        ecore_dlist_first_goto(EWL_CONTAINER(w)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children)) != NULL) {
                ewl_widget_reparent(child);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief When enabling a container, pass the signal to the children.
 */
void
ewl_container_cb_enable(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        if (!EWL_CONTAINER(w)->children)
                DRETURN(DLEVEL_STABLE);

        /*
         * Enable all of the containers children
         */
        ecore_dlist_first_goto(EWL_CONTAINER(w)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children)) != NULL) {
                ewl_widget_enable(child);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief When enabling a container, pass the signal to the children.
 */
void
ewl_container_cb_disable(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        if (!EWL_CONTAINER(w)->children)
                DRETURN(DLEVEL_STABLE);

        /*
         * Disable all of the containers children
         */
        ecore_dlist_first_goto(EWL_CONTAINER(w)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children)) != NULL) {
                ewl_widget_disable(child);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback for when the container is obscured
 */
void
ewl_container_cb_obscure(Ewl_Widget *w, void *ev_data __UNUSED__,
                         void *user_data __UNUSED__)
{
        Ewl_Embed *e;
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        c = EWL_CONTAINER(w);

        /*
         * Give up the clip box object in use.
         */
        e = ewl_embed_widget_find(EWL_WIDGET(w));
        if (e && c->clip_box) {
                ewl_embed_object_cache(e, c->clip_box);
                c->clip_box = NULL;
        }

        /*
         * Notify children that they are now obscured, they will not receive a
         * configure event since the parent won't get configured while
         * obscured.
         */
        if (c->children) {
                ecore_dlist_first_goto(c->children);
                while ((w = ecore_dlist_next(c->children))) {
                        if (REALIZED(w))
                                ewl_widget_obscure(w);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback for when the container is revealed
 */
void
ewl_container_cb_reveal(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        Ewl_Embed *e;
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        c = EWL_CONTAINER(w);

        e = ewl_embed_widget_find(EWL_WIDGET(w));
        if (e && !c->clip_box) {
                c->clip_box = ewl_embed_object_request(e, "rectangle");
        }

        /*
         * Create the clip box for this container, this keeps children clipped
         * to the wanted area.
         */
        if (!c->clip_box)
                c->clip_box = evas_object_rectangle_add(e->canvas);

        /*
         * Setup the remaining properties for the clip box.
         */
        if (c->clip_box) {
                evas_object_pass_events_set(c->clip_box, TRUE);
                evas_object_smart_member_add(c->clip_box, w->smart_object);

                if (w->fx_clip_box) {
                        evas_object_clip_set(c->clip_box, w->fx_clip_box);
                        evas_object_stack_below(c->clip_box, w->fx_clip_box);
                }

                evas_object_color_set(c->clip_box, 255, 255, 255, 255);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief This is the default action to be taken by containers, it involves
 * creating and showing a clip box, as well as clipping the clip box to parent
 * clip boxes.
 */
void
ewl_container_cb_realize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        int i = 0;
        Ewl_Container *c;
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        c = EWL_CONTAINER(w);

        if (!c->children || ecore_dlist_empty_is(c->children))
                DRETURN(DLEVEL_STABLE);

        /*
         * If this container has not yet been realized, then it's children
         * haven't either. So we call ewl_widget_reparent to get each child
         * to update it's evas related fields to the new information, and then
         * realize any of them that should be visible.
         */
        while ((child = ecore_dlist_index_goto(c->children, i))) {
                ewl_callback_call_with_event_data(child, EWL_CALLBACK_REPARENT,
                                c);
                if (VISIBLE(child))
                        ewl_realize_request(child);
                i++;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
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
ewl_container_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        if (EWL_CONTAINER(w)->clip_box) {
                /*
                 * Move the clip box into the new position and size of the
                 * container.
                 */
                evas_object_move(EWL_CONTAINER(w)->clip_box,
                          CURRENT_X(w), CURRENT_Y(w));
                evas_object_resize(EWL_CONTAINER(w)->clip_box,
                            CURRENT_W(w), CURRENT_H(w));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback for when the container is unrealized
 */
void
ewl_container_cb_unrealize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Container *c;
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CONTAINER_TYPE);

        c = EWL_CONTAINER(w);

        /*
         * Clean up the clip box of the container.
         */
        if (c->clip_box) {
                ewl_canvas_object_destroy(c->clip_box);
                c->clip_box = NULL;
        }

        /*
         * FIXME: If called from a destroy callback, the child list may not
         * exist at this point. Is this legitimate ordering?
         */
        if (c->children) {
                ecore_dlist_first_goto(c->children);
                while ((child = ecore_dlist_next(c->children))) {
                        ewl_widget_unrealize(child);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The event data
 * @param user_data: UNUSED
 * @return Returns no data
 * @brief A callback to be used for container widgets such as scrollpane, box
 */
void
ewl_container_cb_container_focus_out(Ewl_Widget *w, void *ev_data, 
                                        void *user_data __UNUSED__)
{
        Ewl_Widget *focus_in = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        if (ev_data)
                focus_in = EWL_WIDGET(ev_data);

        /* If its a child or is disabled then don't send a signal */
        if ((focus_in) && (!ewl_widget_parent_of(w, focus_in)) &&
                                (!DISABLED(w)) && (focus_in != w))
                ewl_widget_state_set(w, "focus,out", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param: ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief A callback to be used with end widgets such as buttons, etc
 */
void
ewl_container_cb_widget_focus_out(Ewl_Widget *w, void *ev_data __UNUSED__, 
                                        void *user_data __UNUSED__)
{
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        c = EWL_CONTAINER(w);
        while (c->redirect)
                c = c->redirect;

        ecore_dlist_first_goto(c->children);
        while ((w = ecore_dlist_next(c->children)))
                ewl_widget_state_set(w, "focus,out", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief A callback to be used with end widgets
 */
void
ewl_container_cb_widget_focus_in(Ewl_Widget *w, void *ev_data __UNUSED__, 
                                 void *user_data __UNUSED__)
{
        Ewl_Container *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        c = EWL_CONTAINER(w);
        while (c->redirect)
                c = c->redirect;

        ecore_dlist_first_goto(c->children);
        while ((w = ecore_dlist_next(c->children)))
                ewl_widget_state_set(w, "focus,in", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The container to work with
 * @return Returns no value
 * @brief A convenience function to recursively show the children of a container
 */
void
ewl_container_children_show(Ewl_Container *c)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        ewl_widget_show(EWL_WIDGET(c));

        ewl_container_child_iterate_begin(c);
        while ((w = ewl_container_child_next(c)))
        {
                if (EWL_CONTAINER_IS(w))
                        ewl_container_children_show(EWL_CONTAINER(w));
                else
                        ewl_widget_show(w);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

