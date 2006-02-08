#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_container_child_insert_helper(Ewl_Container *pc, 
						Ewl_Widget *child, 
						int index, 
						int skip_internal);

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
	DCHECK_PARAM_PTR_RET("c", c, FALSE);

	w = EWL_WIDGET(c);

	/*
	 * Initialize the fields inherited from the widget class
	 */
	if (!ewl_widget_init(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(w, EWL_CONTAINER_TYPE);
	ewl_object_recursive_set(EWL_OBJECT(w), TRUE);

	/*
	 * Initialize the fields specific to the container class.
	 */
	c->children = ecore_list_new();

	/*
	 * All containers need to perform the function of updating the
	 * children with necessary window and evas information.
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_container_configure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_OBSCURE,
			    ewl_container_obscure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REVEAL,
			    ewl_container_reveal_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_container_reveal_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_container_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE,
			    ewl_container_unrealize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT,
			    ewl_container_reparent_cb, NULL);

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
	DCHECK_PARAM_PTR("container", container);
	DCHECK_TYPE("container", container, EWL_CONTAINER_TYPE);

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
	DCHECK_PARAM_PTR("container", container);
	DCHECK_TYPE("container", container, EWL_CONTAINER_TYPE);

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
	DCHECK_PARAM_PTR("container", container);
	DCHECK_TYPE("container", container, EWL_CONTAINER_TYPE);

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
	DCHECK_PARAM_PTR("container", container);
	DCHECK_TYPE("container", container, EWL_CONTAINER_TYPE);

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
	DCHECK_PARAM_PTR("container", container);
	DCHECK_TYPE("container", container, EWL_CONTAINER_TYPE);

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
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("pc", pc, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

	if (pc == EWL_CONTAINER(child->parent))
		DRETURN(DLEVEL_STABLE);

	if (ewl_widget_parent_of(child, EWL_WIDGET(pc))) {
		DWARNING("Attempting to make a child a parent of itself");
		DRETURN(DLEVEL_STABLE);
	}

	while (pc->redirect)
		pc = pc->redirect;

	ecore_list_append(pc->children, child);
	ewl_widget_parent_set(child, EWL_WIDGET(pc));
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
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("pc", pc, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

	if (pc == EWL_CONTAINER(child->parent))
		DRETURN(DLEVEL_STABLE);

	if (ewl_widget_parent_of(child, EWL_WIDGET(pc))) {
		DWARNING("Attempting to make a child a parent of itself");
		DRETURN(DLEVEL_STABLE);
	}

	while (pc->redirect)
		pc = pc->redirect;

	ecore_list_prepend(pc->children, child);
	ewl_widget_parent_set(child, EWL_WIDGET(pc));
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
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("pc", pc, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

	/* already inserted */
	if (pc == EWL_CONTAINER(child->parent))
		DRETURN(DLEVEL_STABLE);

	if (ewl_widget_parent_of(child, EWL_WIDGET(pc))) {
		DWARNING("Attempting to make a child a parent of itself");
		DRETURN(DLEVEL_STABLE);
	}

	/* find the real container */
	while (pc->redirect)
		pc = pc->redirect;

	/* find our insertion point */
	ecore_list_goto_first(pc->children);
	while ((cur = ecore_list_current(pc->children)))
	{
		if (skip_internal && ewl_widget_internal_is(cur)) 
		{
			ecore_list_next(pc->children);
			continue;
		}

		if (idx == index) break;

		idx++;
		ecore_list_next(pc->children);
	}

	ecore_list_insert(pc->children, child);
	ewl_widget_parent_set(child, EWL_WIDGET(pc));
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
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("pc", pc, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

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
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("pc", pc, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

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
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("pc", pc, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

	if (child == EWL_WIDGET(pc->redirect))
		pc->redirect = NULL;

	if (child->parent != EWL_WIDGET(pc)) {
		while (pc->redirect)
			pc = pc->redirect;
	}

	/*
	 * First remove reference to the parent if necessary.
	 */
	if (EWL_CONTAINER(child->parent) == pc)
		ewl_widget_parent_set(child, NULL);

	if (!pc->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Traverse the list to the child.
	 */
	temp = ecore_list_goto(pc->children, child);
	idx = ecore_list_index(pc->children);

	/*
	 * If the child isn't found, then this isn't it's parent.
	 */
	if (!temp) {
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Remove the child from the parent and set the childs parent to NULL
	 */
	ecore_list_remove(pc->children);
	if (VISIBLE(child) && REALIZED(child))
		ewl_container_child_hide_call(pc, child);
	ewl_container_child_remove_call(pc, child, idx);

	ewl_widget_configure(EWL_WIDGET(pc));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The container to get the child count from
 * @return Returns the number of child widgets
 * @brief Returns the number of child widgets in the container
 */
int
ewl_container_child_count_get(Ewl_Container *c)
{
	Ewl_Widget *child = NULL;
	Ewl_Container *container = NULL;
	int count = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, 0);
	DCHECK_TYPE_RET("c", c, EWL_CONTAINER_TYPE, 0);

	/*
	 * Find the container where children are actually added.
	 */
	container = c;
	while (container->redirect) container = container->redirect;

	ecore_list_goto_first(container->children);
	while ((child = ecore_list_next(container->children)))
	{
		if (ewl_widget_internal_is(child)) continue;
		count++;
	}

	DRETURN_INT(count, DLEVEL_STABLE);
}

/**
 * @param parent: The container to get the child from
 * @param index: The child index to return
 * @return Returns the widget at the given index, or NULL if not found
 */
Ewl_Widget *
ewl_container_child_get(Ewl_Container *parent, int index)
{
	Ewl_Container *container = NULL;
	Ewl_Widget *child;
	int count = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("parent", parent, NULL);
	DCHECK_TYPE_RET("parent", parent, EWL_CONTAINER_TYPE, NULL);

	container = parent;
	while (container->redirect) container = container->redirect;

	ecore_list_goto_first(container->children);

	while ((child = ecore_list_next(container->children))) {
		if (ewl_widget_internal_is(child)) continue;
		if (count == index) break;
		count ++;
	}

	DRETURN_PTR(((count == index) ? child : NULL), DLEVEL_STABLE);
}

/**
 * @param parent: The container to search
 * @param w: The child to search for
 * @return Returns the index of the child in the parent
 */
unsigned int
ewl_container_child_index_get(Ewl_Container *parent, Ewl_Widget *w)
{
	unsigned int idx = 0;
	Ewl_Container *container;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("parent", parent, idx);
	DCHECK_PARAM_PTR_RET("w", w, idx);
	DCHECK_TYPE_RET("parent", parent, EWL_CONTAINER_TYPE, idx);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, idx);

	container = parent;
	while (container->redirect) container = container->redirect;

	ecore_list_goto_first(container->children);
	while ((child = ecore_list_next(container->children))) {
		if (ewl_widget_internal_is(child)) continue;
		if (child == w) break;
		idx ++;
	}

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!size || !REALIZED(w) || ewl_object_queued_has(EWL_OBJECT(w),
						EWL_FLAG_QUEUED_RSCHEDULED))
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
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);
	DCHECK_TYPE("widget", widget, EWL_WIDGET_TYPE);

	if (!widget->children || ecore_list_is_empty(widget->children))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ecore_list_goto_first(widget->children);

	/*
	 * Search through the children to find an intersecting child.
	 */
	while ((child = ecore_list_next(EWL_CONTAINER(widget)->children))) {
		if (x >= (CURRENT_X(child) - INSET_LEFT(child))
		    && y >= (CURRENT_Y(child) - INSET_TOP(child))
		    && (CURRENT_X(child) + CURRENT_W(child) +
			    INSET_RIGHT(child)) >= x
		    && (CURRENT_Y(child) + CURRENT_H(child) +
			    INSET_BOTTOM(child)) >= y)
			if ((!found || LAYER(found) <= LAYER(child)) 
					&& VISIBLE(child))
				found = child;
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
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);
	DCHECK_TYPE_RET("widget", widget, EWL_WIDGET_TYPE, NULL);

	if (!widget->children || ecore_list_is_empty(widget->children))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	/*
	 * The starting point is the current widget
	 */
	child = EWL_WIDGET(widget);

	/*
	 * Now move down through the tree of widgets until the bottom layer is
	 * found.
	 */
	if (!ewl_object_state_has(EWL_OBJECT(child), EWL_FLAG_STATE_DISABLED))
		child2 = ewl_container_child_at_get(EWL_CONTAINER(child), x, y);
	while (child2) {
		if (RECURSIVE(child2))
			child = child2;
		else
			DRETURN_PTR(child2, DLEVEL_STABLE);

		if (!ewl_object_state_has(EWL_OBJECT(child),
					EWL_FLAG_STATE_DISABLED))
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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	while (c->redirect)
		c = c->redirect;

	if (!c->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Loop through removing each child and destroying it.
	 */
	ecore_list_goto_first(c->children);
	while ((w = ecore_list_current(c->children))) {
		if (!ewl_object_flags_has(EWL_OBJECT(w),
					EWL_FLAG_PROPERTY_INTERNAL,
					EWL_FLAGS_PROPERTY_MASK)) {
			ewl_widget_destroy(w);

			/*
			 * Start over in case the list was modified from a
			 * callback.
			 */
			ecore_list_goto_first(c->children);
		}
		else
			ecore_list_next(c->children);
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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	while (c->redirect)
		c = c->redirect;

	ecore_list_goto_first(c->children);

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
	DCHECK_PARAM_PTR_RET("c", c, NULL);
	DCHECK_TYPE_RET("c", c, EWL_CONTAINER_TYPE, NULL);

	while (c->redirect)
		c = c->redirect;

	if (c->iterator) {
		w = c->iterator(c);
	}
	else {
		while ((w = ecore_list_next(c->children)) &&
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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

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
ewl_container_callback_intercept(Ewl_Container *c, Ewl_Callback_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

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
ewl_container_callback_nointercept(Ewl_Container *c, Ewl_Callback_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

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
ewl_container_callback_notify(Ewl_Container *c, Ewl_Callback_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	EWL_CALLBACK_FLAG_NOTIFY(EWL_WIDGET(c), t);

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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	if (o == EWL_ORIENTATION_HORIZONTAL) {
		get_size = ewl_object_preferred_w_get;
		set_size = ewl_object_preferred_inner_w_set;
	}
	else {
		get_size = ewl_object_preferred_h_get;
		set_size = ewl_object_preferred_inner_h_set;
	}

	ecore_list_goto_first(c->children);
	while ((child = ecore_list_next(c->children))) {
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
 */
void
ewl_container_sum_prefer(Ewl_Container *c, Ewl_Orientation o)
{
	Ewl_Object *child;
	int curr_size = 0;
	int (*get_size)(Ewl_Object *object);
	void (*set_size)(Ewl_Object *object, int size);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	if (o == EWL_ORIENTATION_HORIZONTAL) {
		get_size = ewl_object_preferred_w_get;
		set_size = ewl_object_preferred_inner_w_set;
	}
	else {
		get_size = ewl_object_preferred_h_get;
		set_size = ewl_object_preferred_inner_h_set;
	}

	ecore_list_goto_first(c->children);
	while ((child = ecore_list_next(c->children))) {
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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	if (c->child_add)
		c->child_add(c, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container removing a child widget
 * @param w: the child widget removed from the container
 * @return Returns no value.
 * @brief Triggers the child_remove callback for the container @a c.
 */
void
ewl_container_child_remove_call(Ewl_Container *c, Ewl_Widget *w, int idx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* do nothing if the container is being destroyed */
	if (ewl_object_queued_has(EWL_OBJECT(c), EWL_FLAG_QUEUED_DPROCESS) 
			|| ewl_object_queued_has(EWL_OBJECT(c),
					EWL_FLAG_QUEUED_DSCHEDULED))
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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* do nothing if the container is being destroyed */
	if (ewl_object_queued_has(EWL_OBJECT(c), EWL_FLAG_QUEUED_DPROCESS) 
			|| ewl_object_queued_has(EWL_OBJECT(c),
					EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	if (c->child_hide)
		c->child_hide(c, w);

	if (c->clip_box) {
		const Evas_List *clippees;
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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	c->redirect = NULL;
	if (c->children) {
		/*
		 * Destroy any children still in the container. Do not remove
		 * in order to avoid list walking.
		 */
		while ((child = ecore_list_goto_first(c->children))) {
			ewl_widget_destroy(child);
		}

		/*
		 * Destroy the container list and set it to NULL.
		 */
		ecore_list_destroy(c->children);
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
	Ewl_Container *rc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);
	DCHECK_TYPE_RET("c", c, EWL_CONTAINER_TYPE, NULL);

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
	DCHECK_PARAM_PTR_RET("c", c, NULL);
	DCHECK_TYPE_RET("c", c, EWL_CONTAINER_TYPE, NULL);

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
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	c->redirect = rc;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When reparenting a container, it's children need the updated information
 * about the container, such as the evas.
 */
void
ewl_container_reparent_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!EWL_CONTAINER(w)->children)
		DRETURN(DLEVEL_STABLE);

	if (REALIZED(w) && w->fx_clip_box)
		evas_object_layer_set(EWL_CONTAINER(w)->clip_box,
				evas_object_layer_get(w->fx_clip_box));

	/*
	 * Reparent all of the containers children
	 */
	ecore_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		ewl_widget_reparent(child);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_container_obscure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			 void *user_data __UNUSED__)
{
	Ewl_Embed *e;
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = EWL_CONTAINER(w);

	/*
	 * Give up the clip box object in use.
	 */
	e = ewl_embed_widget_find(EWL_WIDGET(w));
	if (e && c->clip_box) {
		evas_object_hide(c->clip_box);
		ewl_embed_object_cache(e, c->clip_box);
		c->clip_box = NULL;
	}

	/*
	 * Notify children that they are now obscured, they will not receive a
	 * configure event since the parent won't get configured while
	 * obscured.
	 */
	if (c->children) {
		ecore_list_goto_first(c->children);
		while ((w = ecore_list_next(c->children))) {
			if (REALIZED(w))
				ewl_widget_obscure(w);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void 
ewl_container_reveal_cb(Ewl_Widget * w, void *ev_data __UNUSED__, 
			void *user_data __UNUSED__)
{
	Ewl_Embed *e;
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

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
		c->clip_box = evas_object_rectangle_add(e->evas);

	/*
	 * Setup the remaining properties for the clip box.
	 */
	if (c->clip_box) {
                evas_object_pass_events_set(c->clip_box, TRUE);

		if (w->fx_clip_box) {
			evas_object_clip_set(c->clip_box, w->fx_clip_box);
			evas_object_layer_set(c->clip_box,
					evas_object_layer_get(w->fx_clip_box));
		}

		evas_object_color_set(c->clip_box, 255, 255, 255, 255);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * This is the default action to be taken by containers, it involves
 * creating and showing a clip box, as well as clipping the clip box to parent
 * clip boxes.
 */
void
ewl_container_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	int i = 0;
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	c = EWL_CONTAINER(w);

	if (!c->children || ecore_list_is_empty(c->children))
		DRETURN(DLEVEL_STABLE);

	/*
	 * If this container has not yet been realized, then it's children
	 * haven't either. So we call ewl_widget_reparent to get each child
	 * to update it's evas related fields to the new information, and then
	 * realize any of them that should be visible.
	 */
	while ((child = ecore_list_goto_index(c->children, i))) {
		ewl_callback_call_with_event_data(child, EWL_CALLBACK_REPARENT,
				c);
		if (VISIBLE(child))
			ewl_realize_request(child);
		i++;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_container_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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

void
ewl_container_unrealize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	c = EWL_CONTAINER(w);

	/*
	 * Clean up the clip box of the container.
	 */
	if (c->clip_box) {
		ewl_evas_object_destroy(c->clip_box);
		c->clip_box = NULL;
	}

	/*
	 * FIXME: If called from a destroy callback, the child list may not
	 * exist at this point. Is this legitimate ordering?
	 */
	if (c->children) {
		ecore_list_goto_first(c->children);
		while ((child = ecore_list_next(c->children))) {
			ewl_widget_unrealize(child);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

