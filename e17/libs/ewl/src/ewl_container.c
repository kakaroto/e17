
#include <Ewl.h>

void            __ewl_container_realize(Ewl_Widget * w, void *ev_data,
					void *user_data);
void            __ewl_container_configure_clip_box(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);
void            __ewl_container_reparent(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_container_destroy(Ewl_Widget * w, void *ev_data,
					void *user_data);
void            __ewl_container_child_destroy(Ewl_Widget * w, void *ev_data,
					      void *user_data);

/**
 * ewl_container_init - initialize a containers default fields and callbacks
 * @c: the container to initialize
 * @appearance: the appearance key for this container
 *
 * Returns no value. Initializes the default values of the container, this
 * also sets up the widget fields of the container, so the @appearance string
 * is necessary.
 */
void
ewl_container_init(Ewl_Container * c, char *appearance, Ewl_Child_Add add,
		   Ewl_Child_Resize rs)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	w = EWL_WIDGET(c);

	/*
	 * Initialize the fields inherited from the widget class
	 */
	ewl_widget_init(w, appearance);
	w->recursive = TRUE;

	/*
	 * Initialize the fields specific to the container class.
	 */
	c->children = ewd_list_new();
	c->child_add = add;
	c->child_resize = rs;

	/*
	 * All containers need to perform the function of updating the
	 * children with necessary window and evas information.
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_container_configure_clip_box, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_container_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT,
			    __ewl_container_reparent, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
			     __ewl_container_destroy, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_container_add_notify - set the function to be called when adding children
 * @container: the container to change the add notifier
 * @add: the new add notifier for the container
 *
 * Returns no value. Changes the add nofitier function of @container to @add.
 */
void ewl_container_add_notify(Ewl_Container * container, Ewl_Child_Add add)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("container", container);

	container->child_add = add;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_container_resize_notify - set function to be called when children resize
 * @container: the container to change the resize notifier
 * @resize: the new resizenotifier for the container
 *
 * Returns no value. Changes the resize nofitier function of @container to
 * @resize.
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
 * ewl_container_append_child - add a child at the end of the container
 * @pc: the parent container that will hold the child
 * @child: the child to add to the container
 *
 * Returns no value. Attaches the child to the end of the parent containers
 * child list.
 */
void ewl_container_append_child(Ewl_Container * pc, Ewl_Widget * child)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * A widget cannot be the child of multiple widgets, so remove it
	 * from a previous parent before adding to this parent.
	 */
	if (child->parent)
		ewl_container_remove_child(EWL_CONTAINER(child->parent), child);

	/*
	 * Set the child's parent field to this container, append it to the
	 * list of the container's children and then notify the child that
	 * it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(pc);
	LAYER(child) = LAYER(pc) + 5;
	ewd_list_append(pc->children, child);

	/*
	 * Append a destroy callback to the child to remove it from the
	 * container.
	 */
	ewl_callback_prepend(child, EWL_CALLBACK_DESTROY,
			     __ewl_container_child_destroy, NULL);

	/*
	 * Now call the add function for the container.
	 */
	if (pc->child_add)
		pc->child_add(pc, child);

	/*
	 * Now let the child know it has a new parent.
	 */
	ewl_widget_reparent(child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_container_prepend_child - add a child at the end of the container
 * @pc: the parent container that will hold the child
 * @child: the child to add to the container
 *
 * Returns no value. Attaches the child to the start of the parent containers
 * child list.
 */
void ewl_container_prepend_child(Ewl_Container * pc, Ewl_Widget * child)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * A widget cannot be the child of multiple widgets, so remove it
	 * from a previous parent before adding to this parent.
	 */
	if (child->parent)
		ewl_container_remove_child(EWL_CONTAINER(child->parent), child);

	/*
	 * Set the child's parent field to this container, prepend it to the
	 * list of the container's children and then notify the child that
	 * it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(pc);
	LAYER(child) = LAYER(pc) + 5;
	ewd_list_prepend(pc->children, child);

	/*
	 * Prepend a destroy callback to the child to remove it from the
	 * container.
	 */
	ewl_callback_prepend(child, EWL_CALLBACK_DESTROY,
			     __ewl_container_child_destroy, NULL);

	/*
	 * Now call the add function for the container.
	 */
	if (pc->child_add)
		pc->child_add(pc, child);

	/*
	 * Now let the child know it has a new parent.
	 */
	ewl_widget_reparent(child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_container_insert_child - add a child at an index of the container
 * @pc: the parent container that will hold the child
 * @child: the child to add to the container
 * @index: the position in the child list to add the cihld
 *
 * Returns no value. Attaches the child to the @index position of the parent
 * containers child list.
 */
void
ewl_container_insert_child(Ewl_Container * pc, Ewl_Widget * child, int index)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * A widget cannot be the child of multiple widgets, so remove it
	 * from a previous parent before adding to this parent.
	 */
	if (child->parent)
		ewl_container_remove_child(EWL_CONTAINER(child->parent), child);

	/*
	 * Set the child's parent field to this container, insert it on the
	 * list of the container's children at the designated position and then
	 * notify the child that it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(pc);
	LAYER(child) = LAYER(pc) + 5;
	ewd_list_goto_index(pc->children, index);
	ewd_list_insert(pc->children, child);

	/*
	 * Prepend a destroy callback to the child to remove it from the
	 * container.
	 */
	ewl_callback_prepend(child, EWL_CALLBACK_DESTROY,
			     __ewl_container_child_destroy, NULL);

	/*
	 * Now call the add function for the container.
	 */
	if (pc->child_add)
		pc->child_add(pc, child);

	/*
	 * Now let the child know it has a new parent.
	 */
	ewl_widget_reparent(child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_container_remove_child - remove the specified child from the container
 * @pc: the container to search for the child to remove
 * @child: the child to remove from the container
 *
 * Returns no value. Removes the specified child from the container without
 * destroying the child.
 */
void ewl_container_remove_child(Ewl_Container * pc, Ewl_Widget * child)
{
	Ewl_Widget     *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * Traverse the list to the child.
	 */
	temp = ewd_list_goto(pc->children, child);

	/*
	 * If the child isn't found, then this isn't it's parent.
	 */
	if (!temp)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Remove the child from the parent and set the childs parent to NULL
	 */
	ewd_list_remove(pc->children);
	child->parent = NULL;

	/*
	 * Remove the callback from the child for removing it from the
	 * container upon destruction.
	 */
	ewl_callback_del(child, EWL_CALLBACK_DESTROY,
			 __ewl_container_child_destroy);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_container_resize_child - notify a container of a child pref size change
 * @w: the child widget that has had it's preferred size changed
 * @size: the amount of change in size
 * @o: the orientation of the size change
 */
void ewl_container_resize_child(Ewl_Widget * w, int size, Ewl_Orientation o)
{
	int             old_w, old_h;
	int             new_w, new_h;

	DCHECK_PARAM_PTR("w", w);

	/*
	 * If there is no parent to this widget, or it hasn't really changed
	 * size just exit. Also exit if it has no function to be notified for
	 * child resizes.
	 */
	if (!w->parent || !size || !EWL_CONTAINER(w->parent)->child_resize)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Store the parents current size so we can determine if it changes
	 */
	ewl_object_get_preferred_size(EWL_OBJECT(w->parent), &old_w, &old_h);

	/*
	 * Run the parents child resize function to allow it to update it's
	 * preferred size.
	 */
	if (EWL_CONTAINER(w->parent)->child_resize)
		EWL_CONTAINER(w->parent)->
		    child_resize(EWL_CONTAINER(w->parent), w, size, o);

	/*
	 * Get the new preferred size of the parent to see if it changed.
	 */
	ewl_object_get_preferred_size(EWL_OBJECT(w->parent), &new_w, &new_h);

	/*
	 * The parent will only end up on the configure queue if it didn't
	 * change size (ie. it's parent isn't on the configure queue).
	 */
	if (w->parent->parent)
		ewl_widget_configure(w->parent->parent);
}

/**
 * ewl_container_get_child_at - find the child at the given coordinates
 * @widget: the searched container
 * @x: the x coordinate to search for
 * @y: the y coordinate to search for
 *
 * Returns the found widget on success, NULL on failure. The given container
 * is searched to find any child that intersects the given coordinates.
 */
Ewl_Widget     *ewl_container_get_child_at(Ewl_Container * widget, int x, int y)
{
	Ewl_Widget     *found = NULL;
	Ewl_Widget     *child = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	if (!widget->children || ewd_list_is_empty(widget->children))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewd_list_goto_first(widget->children);

	/*
	 * Search through the children to find an intersecting child.
	 */
	while ((child = ewd_list_next(EWL_CONTAINER(widget)->children))) {
		if (x >= CURRENT_X(child) && y >= CURRENT_Y(child)
		    && CURRENT_X(child) + CURRENT_W(child) >= x
		    && CURRENT_Y(child) + CURRENT_H(child) >= y)
			if ((!found || LAYER(found) <= LAYER(child)) &&
			    VISIBLE(child))
				found = child;
	}

	DRETURN_PTR(found, DLEVEL_STABLE);
}

/**
 * ewl_container_get_child_at_recursive - find child that intersects coordinates
 * @widget: the widget to search for intersecting child
 * @x: the x coordinate of the intersection point
 * @y: the y coordinate of the intersection point
 *
 * Returns a point to the intersecting widget on success, NULL on failure.
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

	if (!widget->children || ewd_list_is_empty(widget->children))
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
		if (child2->recursive) {
			child = ewl_container_get_child_at(EWL_CONTAINER(child),
							   x, y);

			if (!child)
				DRETURN_PTR(child2, DLEVEL_STABLE);
		} else
			DRETURN_PTR(child2, DLEVEL_STABLE);
	}

	DRETURN_PTR((child ? child : NULL), DLEVEL_STABLE);
}

/**
 * ewl_container_reset - destroy all children of the container
 * @c: the container to reset
 *
 * Returns no value. Destroys all the children of the container but not the
 * container itself.
 */
void ewl_container_reset(Ewl_Container * c)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	/*
	 * Loop through removing each child and destroying it.
	 */
	ewd_list_goto_first(c->children);
	while ((w = ewd_list_remove_last(c->children)))
		ewl_widget_destroy(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When reparenting a container, it's children need the updated information
 * about the container, such as evas and evas_window.
 */
void __ewl_container_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_CONTAINER(w)->children)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Reparent all of the containers children
	 */
	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		ewl_widget_reparent(child);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * This is the default action to be taken by containers, it involves
 * creating and showing a clip box, as well as clipping the clip box to parent
 * clip boxes.
 */
void __ewl_container_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             i = 0;
	Ewl_Window     *win;
	Ewl_Container  *c;
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * Create the clip box for this container, this keeps children clipped
	 * to the wanted area.
	 */
	c->clip_box = evas_add_rectangle(win->evas);
	evas_set_color(win->evas, c->clip_box, 255, 255, 255, 0);
	evas_move(win->evas, c->clip_box, CURRENT_X(w), CURRENT_Y(w));
	evas_resize(win->evas, c->clip_box, CURRENT_W(w), CURRENT_H(w));
	evas_set_layer(win->evas, c->clip_box, LAYER(w));
	evas_show(win->evas, c->clip_box);

	/*
	 * Now clip the container portion of this widget to the widget
	 * fx_clip_box.
	 */
	if (w->fx_clip_box && c->clip_box)
		evas_set_clip(win->evas, c->clip_box, w->fx_clip_box);

	if (!c->children || ewd_list_is_empty(c->children))
		DRETURN(DLEVEL_STABLE);

	/*
	 * If this container has not yet been realized, then it's children
	 * haven't either. So we call ewl_widget_reparent to get each child
	 * to update it's evas related fields to the new information, and then
	 * realize any of them that should be visible.
	 */
	while ((child = ewd_list_goto_index(c->children, i))) {
		ewl_widget_reparent(child);
		if (VISIBLE(child))
			ewl_widget_realize(child);
		i++;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_container_configure_clip_box(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->clip_box) {
		Ewl_Window     *win;

		win = ewl_window_find_window_by_widget(w);

		/*
		 * Move the clip box into the new position and size of the
		 * container.
		 */
		evas_move(win->evas, EWL_CONTAINER(w)->clip_box,
			  CURRENT_X(w) + INSET_LEFT(w), CURRENT_Y(w) +
			  INSET_TOP(w));
		evas_resize(win->evas, EWL_CONTAINER(w)->clip_box,
			    CURRENT_W(w) - (INSET_LEFT(w) + INSET_RIGHT(w)),
			    CURRENT_H(w) - (INSET_TOP(w) + INSET_BOTTOM(w)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_container_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;
	Ewl_Container  *c;
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	/*
	 * Clean up the clip box of the container.
	 */
	if (c->clip_box) {
		win = ewl_window_find_window_by_widget(w);

		evas_hide(win->evas, c->clip_box);
		evas_unset_clip(win->evas, c->clip_box);
		evas_del_object(win->evas, c->clip_box);

		c->clip_box = NULL;
	}

	if (c->children) {
		/*
		 * Destroy any children still in the container.
		 */
		ewd_list_goto_first(c->children);
		while ((child = ewd_list_remove_last(c->children)))
			ewl_widget_destroy(child);

		/*
		 * Destroy the container list and set it to NULL.
		 */
		ewd_list_destroy(EWL_CONTAINER(w)->children);
		EWL_CONTAINER(w)->children = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_container_child_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->parent)
		ewl_container_remove_child(EWL_CONTAINER(w->parent), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
