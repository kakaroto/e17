
#include <Ewl.h>

/**
 * @param c: the container to initialize
 * @param appearance: the appearance key for this container
 * @param add: the function to call when children added to container
 * @param remove: the function to call when children removed from container
 * @param rs: the function to call when children of container are resized
 * @return Returns TRUE on success, otherwise FALSE.
 * @brief Initialize a containers default fields and callbacks
 *
 * Initializes the default values of the container, this also sets up the
 * widget fields of the container, so the @a appearance string is necessary.
 */
int 
ewl_container_init(Ewl_Container * c, char *appearance)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, FALSE);

	w = EWL_WIDGET(c);

	/*
	 * Initialize the fields inherited from the widget class
	 */
	if (!ewl_widget_init(w, appearance))
		DRETURN_INT(0, DLEVEL_STABLE);

	ewl_object_set_recursive(EWL_OBJECT(w), TRUE);

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
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_container_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE,
			    ewl_container_unrealize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT,
			    ewl_container_reparent_cb, NULL);

	DRETURN_INT(1, DLEVEL_STABLE);
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
ewl_container_add_notify(Ewl_Container * container, Ewl_Child_Add add)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("container", container);

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
ewl_container_remove_notify(Ewl_Container * container, Ewl_Child_Remove remove)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("container", container);

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
ewl_container_resize_notify(Ewl_Container * container, Ewl_Child_Resize resize)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("container", container);

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
ewl_container_show_notify(Ewl_Container * container, Ewl_Child_Show show)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("container", container);

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
ewl_container_hide_notify(Ewl_Container * container, Ewl_Child_Hide hide)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("container", container);

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
void ewl_container_append_child(Ewl_Container * pc, Ewl_Widget * child)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	if (ewl_container_parent_of(child, EWL_WIDGET(pc))) {
		DWARNING("Attempting to make a child a parent of itself");
		DRETURN(DLEVEL_STABLE);
	}

	while (pc->redirect)
		pc = pc->redirect;

	if (ecore_list_is_empty(pc->children) && pc->clip_box)
		evas_object_show(pc->clip_box);

	ecore_list_append(pc->children, child);
	ewl_widget_set_parent(child, EWL_WIDGET(pc));
	ewl_container_call_child_add(pc, child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param pc: the parent container that will hold the child
 * @param child: the child to add to the container
 * @return Returns no value.
 * @brief Add a child at the end of the container
 *
 * Attaches the child to the start of the parent containers child list.
 */
void ewl_container_prepend_child(Ewl_Container * pc, Ewl_Widget * child)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	if (ewl_container_parent_of(child, EWL_WIDGET(pc))) {
		DWARNING("Attempting to make a child a parent of itself");
		DRETURN(DLEVEL_STABLE);
	}

	while (pc->redirect)
		pc = pc->redirect;

	if (ecore_list_is_empty(pc->children) && pc->clip_box)
		evas_object_show(pc->clip_box);

	ecore_list_prepend(pc->children, child);
	ewl_widget_set_parent(child, EWL_WIDGET(pc));
	ewl_container_call_child_add(pc, child);

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
ewl_container_insert_child(Ewl_Container * pc, Ewl_Widget * child, int index)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	if (ewl_container_parent_of(child, EWL_WIDGET(pc))) {
		DWARNING("Attempting to make a child a parent of itself");
		DRETURN(DLEVEL_STABLE);
	}

	while (pc->redirect)
		pc = pc->redirect;

	if (ecore_list_is_empty(pc->children) && pc->clip_box)
		evas_object_show(pc->clip_box);

	ecore_list_goto_index(pc->children, index);
	ecore_list_insert(pc->children, child);
	ewl_widget_set_parent(child, EWL_WIDGET(pc));
	ewl_container_call_child_add(pc, child);

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
void ewl_container_remove_child(Ewl_Container * pc, Ewl_Widget * child)
{
	Ewl_Widget     *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

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
		ewl_widget_set_parent(child, NULL);

	if (!pc->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Traverse the list to the child.
	 */
	temp = ecore_list_goto(pc->children, child);

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
	ewl_container_call_child_remove(pc, child);

	if (ecore_list_is_empty(pc->children) && pc->clip_box)
		evas_object_hide(pc->clip_box);

	ewl_widget_configure(EWL_WIDGET(pc));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the child widget that has had it's preferred size changed
 * @param size: the amount of change in size
 * @param o: the orientation of the size change
 * @return Returns no value.
 * @brief Notify a container of a child pref size change
 */
void ewl_container_resize_child(Ewl_Widget * w, int size, Ewl_Orientation o)
{
	int             old_w, old_h;
	int             new_w, new_h;
	Ewl_Container  *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("w", w);

	if (!size || ewl_in_realize_phase() || !REALIZED(w))
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
Ewl_Widget     *ewl_container_get_child_at(Ewl_Container * widget, int x, int y)
{
	Ewl_Widget     *found = NULL;
	Ewl_Widget     *child = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

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
			if ((!found || LAYER(found) <= LAYER(child)) &&
			    VISIBLE(child))
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
Ewl_Widget     *ewl_container_get_child_at_recursive(Ewl_Container * widget,
						     int x, int y)
{
	/*
	 * These temporary variables allow for traversing recursive
	 * containers without actually making recursive function calls.
	 */
	Ewl_Widget     *child = NULL;
	Ewl_Widget     *child2 = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	if (!widget->children || ecore_list_is_empty(widget->children))
		return NULL;

	/*
	 * The starting point is the current widget
	 */
	child = EWL_WIDGET(widget);

	/*
	 * Now move down through the tree of widgets until the bottom layer is
	 * found.
	 */
	while ((child2 = ewl_container_get_child_at(EWL_CONTAINER(child),
						    x, y))) {
		if (RECURSIVE(child2))
			child = child2;
		else
			DRETURN_PTR(child2, DLEVEL_STABLE);
	}

	DRETURN_PTR((child2 ? child2 : child), DLEVEL_STABLE);
}

/**
 * @param c: the container to compare against, safe to pass a non-container
 * @param w: the widget to check parentage
 * @return Returns TRUE if @a c is a parent of @a w, otherwise returns FALSE.
 * @brief Determine if a container is a parent of a widget
 */
int ewl_container_parent_of(Ewl_Widget *c, Ewl_Widget *w)
{
	Ewl_Widget *parent;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("c", c, FALSE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	parent = w;
	while ((parent = parent->parent)) {
		if (parent == c)
			DRETURN_INT(TRUE, DLEVEL_STABLE);
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param c: the container to reset
 * @return Returns no value.
 * @brief Destroy all children of the container
 *
 * Destroys all the children of the container but not the container itself or
 * internally used widgets.
 */
void ewl_container_reset(Ewl_Container * c)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	while (c->redirect)
		c = c->redirect;

	if (!c->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Loop through removing each child and destroying it.
	 */
	ecore_list_goto_first(c->children);
	while ((w = ecore_list_current(c->children))) {
		if (!ewl_object_has_flags(EWL_OBJECT(w),
					EWL_FLAG_PROPERTY_INTERNAL,
					EWL_FLAGS_PROPERTY_MASK)) {
			ecore_list_remove(c->children);
			ewl_widget_destroy(w);
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
void ewl_container_child_iterate_begin(Ewl_Container *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	ecore_list_goto_first(c->children);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
ewl_container_set_child_iterator(Ewl_Container *c, Ewl_Container_Iterator i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

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
void ewl_container_intercept_callback(Ewl_Container *c, Ewl_Callback_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	EWL_CALLBACK_FLAG_INTERCEPT(EWL_WIDGET(c), t);

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
void ewl_container_notify_callback(Ewl_Container *c, Ewl_Callback_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

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
ewl_container_prefer_largest(Ewl_Container *c, Ewl_Orientation o)
{
	Ewl_Object *child;
	int max_size = 0;
	int curr_size = 0;
	int (*get_size)(Ewl_Object *object);
	void (*set_size)(Ewl_Object *object, int size);

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("c", c);

	if (o == EWL_ORIENTATION_HORIZONTAL) {
		get_size = ewl_object_get_preferred_w;
		set_size = ewl_object_set_preferred_w;
	}
	else {
		get_size = ewl_object_get_preferred_h;
		set_size = ewl_object_set_preferred_h;
	}

	ecore_list_goto_first(c->children);
	while ((child = ecore_list_next(c->children))) {
		if (VISIBLE(child)) {
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
void ewl_container_prefer_sum(Ewl_Container *c, Ewl_Orientation o)
{
	Ewl_Object *child;
	int curr_size = 0;
	int (*get_size)(Ewl_Object *object);
	void (*set_size)(Ewl_Object *object, int size);

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("c", c);

	if (o == EWL_ORIENTATION_HORIZONTAL) {
		get_size = ewl_object_get_preferred_w;
		set_size = ewl_object_set_preferred_w;
	}
	else {
		get_size = ewl_object_get_preferred_h;
		set_size = ewl_object_set_preferred_h;
	}

	ecore_list_goto_first(c->children);
	while ((child = ecore_list_next(c->children))) {
		if (VISIBLE(child))
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
void ewl_container_call_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (c->child_add && VISIBLE(w) && REALIZED(w))
		c->child_add(c, w);

	ewl_widget_configure(EWL_WIDGET(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container removing a child widget
 * @param w: the child widget removed from the container
 * @return Returns no value.
 * @brief Triggers the child_remove callback for the container @a c.
 */
void ewl_container_call_child_remove(Ewl_Container *c, Ewl_Widget *w)
{
	if (c->child_remove && VISIBLE(w))
		c->child_remove(c, w);
}

/**
 * @param c: the container receiving a new child widget
 * @param w: the child widget shown in the container
 * @return Returns no value.
 * @brief Triggers the child_show callback for the container @a c.
 */
void ewl_container_call_child_show(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (c->child_show && VISIBLE(w) && REALIZED(w))
		c->child_show(c, w);

	ewl_widget_configure(EWL_WIDGET(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container receiving a new child widget
 * @param w: the child widget hidden in the container
 * @return Returns no value.
 * @brief Triggers the child_hide callback for the container @a c.
 */
void ewl_container_call_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (c->child_hide && VISIBLE(w) && REALIZED(w))
		c->child_hide(c, w);

	ewl_widget_configure(EWL_WIDGET(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the container to destroy children
 * @return Returns no value.
 * @brief Destroy all the sub-children of the container.
 */
void ewl_container_destroy(Ewl_Container * c)
{
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->redirect = NULL;
	if (c->children) {
		/*
		 * Destroy any children still in the container.
		 */
		while ((child = ecore_list_remove_last(c->children))) {
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
Ewl_Container *ewl_container_get_end_redirect(Ewl_Container *c)
{
	Ewl_Container *rc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

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
Ewl_Container *ewl_container_get_redirect(Ewl_Container *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, NULL);

	DRETURN_PTR(c->redirect, DLEVEL_STABLE);
}

/**
 * @param c: the container to change it's redirection container
 * @param rc: the new container to redirect children to
 * @return Returns no value.
 * @brief Changes the redirected container of the container.
 */
void ewl_container_set_redirect(Ewl_Container *c, Ewl_Container *rc)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("rc", rc);

	c->redirect = rc;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When reparenting a container, it's children need the updated information
 * about the container, such as the evas.
 */
void ewl_container_reparent_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

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

/*
 * This is the default action to be taken by containers, it involves
 * creating and showing a clip box, as well as clipping the clip box to parent
 * clip boxes.
 */
void ewl_container_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             i = 0;
	Ewl_Embed      *emb;
	Ewl_Container  *c;
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	emb = ewl_embed_find_by_widget(w);

	/*
	 * Create the clip box for this container, this keeps children clipped
	 * to the wanted area.
	 */
	c->clip_box = evas_object_rectangle_add(emb->evas);
	if (c->clip_box) {
		evas_object_move(c->clip_box, CURRENT_X(w), CURRENT_Y(w));
		evas_object_resize(c->clip_box, CURRENT_W(w), CURRENT_H(w));
	}

	if (w->fx_clip_box) {
		evas_object_clip_set(c->clip_box, w->fx_clip_box);
		evas_object_layer_set(c->clip_box,
				evas_object_layer_get(w->fx_clip_box));
	}

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

	/*
	 * Only show it if there are children, otherwise we get a colored box.
	 */
	if (c->clip_box)
		evas_object_show(c->clip_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_container_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

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

void ewl_container_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = EWL_CONTAINER(w);

	/*
	 * Clean up the clip box of the container.
	 */
	if (c->clip_box) {
		ewl_evas_object_destroy(c->clip_box);
		c->clip_box = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
