
#include <Ewl.h>

void __ewl_container_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_container_configure_clip_box(Ewl_Widget * w, void *ev_data,
					void *user_data);
void __ewl_container_reparent(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_container_destroy(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_container_destroy_recursive(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void __ewl_container_child_destroy(Ewl_Widget * w, void *ev_data,
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
ewl_container_init(Ewl_Container * c, char *appearance)
{
	Ewl_Widget *w;

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
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY_RECURSIVE,
			     __ewl_container_destroy_recursive, NULL);

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
void
ewl_container_append_child(Ewl_Container * pc, Ewl_Widget * child)
{
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	if (!pc->forward)
		c = pc;
	else
		c = pc->forward;

	/*
	 * Set the child's parent field to this container, append it to the
	 * list of the container's children and then notify the child that
	 * it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(c);
	ewd_list_append(c->children, child);

	ewl_callback_prepend(child, EWL_CALLBACK_DESTROY,
			     __ewl_container_child_destroy, NULL);

	ewl_widget_reparent(child);

	if (!c->children || ewd_list_is_empty(c->children))
		evas_set_color(EWL_WIDGET(c)->evas, c->clip_box, 255,
			       255, 255, 255);

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
void
ewl_container_prepend_child(Ewl_Container * pc, Ewl_Widget * child)
{
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	if (!pc->forward)
		c = pc;
	else
		c = pc->forward;

	/*
	 * Set the child's parent field to this container, prepend it to the
	 * list of the container's children and then notify the child that
	 * it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(c);
	ewd_list_prepend(c->children, child);

	ewl_callback_prepend(child, EWL_CALLBACK_DESTROY,
			     __ewl_container_child_destroy, NULL);

	ewl_widget_reparent(child);

	if (!c->children || ewd_list_is_empty(c->children))
		evas_set_color(EWL_WIDGET(c)->evas, c->clip_box, 255,
			       255, 255, 255);

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
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	if (!pc->forward)
		c = pc;
	else
		c = pc->forward;

	/*
	 * Set the child's parent field to this container, insert it on the
	 * list of the container's children at the designated position and then
	 * notify the child that it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(c);
	ewd_list_goto_index(c->children, index);
	ewd_list_insert(c->children, child);

	ewl_callback_prepend(child, EWL_CALLBACK_DESTROY,
			     __ewl_container_child_destroy, NULL);

	ewl_widget_reparent(child);

	if (!c->children || ewd_list_is_empty(c->children))
		evas_set_color(EWL_WIDGET(c)->evas, c->clip_box, 255,
			       255, 255, 255);

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
void
ewl_container_remove_child(Ewl_Container * pc, Ewl_Widget * child)
{
	Ewl_Widget *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	temp = ewd_list_goto(pc->children, child);

	if (temp)
		ewd_list_remove(pc->children);

	child->parent = NULL;

	ewl_callback_del(child, EWL_CALLBACK_DESTROY,
			 __ewl_container_child_destroy);

	if (!pc->children || ewd_list_is_empty(pc->children))
		evas_set_color(EWL_WIDGET(pc)->evas, pc->clip_box, 0, 0, 0,
			       0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
Ewl_Widget *
ewl_container_get_child_at(Ewl_Container * widget, int x, int y)
{
	Ewl_Widget *child = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	if (!widget->children || ewd_list_is_empty(widget->children))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewd_list_goto_first(widget->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  if (x >= CURRENT_X(child) && y >= CURRENT_Y(child)
		      && CURRENT_X(child) + CURRENT_W(child) >= x
		      && CURRENT_Y(child) + CURRENT_H(child) >= y)
			  DRETURN_PTR(child, DLEVEL_STABLE);
	  }

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * ewl_container_get_child_at_recursive - find child that intersects coordinates
 * @widget: the widget to search for intersecting child
 * @x: the x coordinate of the intersection point
 * @y: the y coordinate of the intersection point
 *
 * Returns a point to the intersecting widget on success, NULL on failure.
 */
Ewl_Widget *
ewl_container_get_child_at_recursive(Ewl_Container * widget, int x, int y)
{
	/*
	 * These 2 temporary variables allow for traversing recursive
	 * containers without using high levels of recursion.
	 */
	Ewl_Widget *child = NULL;
	Ewl_Widget *child2 = NULL;

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
	while ((child2 =
		ewl_container_get_child_at(EWL_CONTAINER(child), x, y)))
	  {
		  if (child2->recursive)
		    {
			    child = ewl_container_get_child_at(EWL_CONTAINER
							       (child), x, y);

			    if (!child)
				    DRETURN_PTR(child2, DLEVEL_STABLE);
		    }
		  else
			  DRETURN_PTR(child2, DLEVEL_STABLE);
	  }

	DRETURN_PTR((child ? child : NULL), DLEVEL_STABLE);
}

void
ewl_container_set_forward(Ewl_Container * c, Ewl_Container * c2)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->forward = c2;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When reparenting a container, it's children need the updated information
 * about the container, such as evas and evas_window.
 */
void
__ewl_container_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *child;
	Ewd_List *old;
	Ewl_Container *of;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	old = EWL_CONTAINER(w)->children;
	EWL_CONTAINER(w)->children = ewd_list_new();

	of = EWL_CONTAINER(w)->forward;

	EWL_CONTAINER(w)->forward = NULL;

	while ((child = ewd_list_remove_first(old)) != NULL)
	  {
		  ewl_container_append_child(EWL_CONTAINER(w), child);
		  ewl_widget_reparent(child);
	  }

	EWL_CONTAINER(w)->forward = of;

	ewd_list_destroy(old);
}

/*
 * This is the default action to be taken by containers, it involves
 * creating and showing a clip box, as well as clipping the clip box to parent
 * clip boxes.
 */
void
__ewl_container_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	/*
	 * Create the clip box for this container, this keeps children clipped
	 * to the wanted area.
	 */
	c->clip_box = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, c->clip_box, 255, 255, 255, 0);
	evas_move(w->evas, c->clip_box, CURRENT_X(w), CURRENT_Y(w));
	evas_resize(w->evas, c->clip_box, CURRENT_W(w), CURRENT_H(w));
	evas_set_layer(w->evas, c->clip_box, LAYER(w));
	evas_show(w->evas, c->clip_box);

	/*
	 * Now clip the container portion of this widget to the widget
	 * fx_clip_box.
	 */
	if (w->fx_clip_box && c->clip_box)
		evas_set_clip(w->evas, c->clip_box, w->fx_clip_box);

	if (!c->children || ewd_list_is_empty(c->children))
		DRETURN(DLEVEL_STABLE);

	/*
	 * If this container has not yet been realized, then it's children
	 * haven't either. So we call ewl_widget_reparent to get each child
	 * to update it's evas related fields to the new information, and then
	 * realize any of them that should be visible.
	 */
	ewd_list_goto_first(c->children);

	while ((child = ewd_list_next(c->children)) != NULL)
	  {
		  ewl_widget_reparent(child);
		  ewl_widget_realize(child);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_container_configure_clip_box(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->clip_box)
	  {
		  int ll = 0, rr = 0, tt = 0, bb = 0;

		  if (w->ebits_object)
			  ebits_get_insets(w->ebits_object, &ll, &rr, &tt,
					   &bb);

		  evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
			    CURRENT_X(w) + ll, CURRENT_Y(w) + tt);
		  evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
			      CURRENT_W(w) - (ll + rr),
			      CURRENT_H(w) - (tt + bb));
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_container_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->clip_box)
	  {
		  evas_hide(w->evas, EWL_CONTAINER(w)->clip_box);
		  evas_unset_clip(w->evas, EWL_CONTAINER(w)->clip_box);
		  evas_del_object(w->evas, EWL_CONTAINER(w)->clip_box);

		  EWL_CONTAINER(w)->clip_box = NULL;
	  }

	if (EWL_CONTAINER(w)->children)
	  {
		  ewd_list_clear(EWL_CONTAINER(w)->children);
		  ewd_list_destroy(EWL_CONTAINER(w)->children);
		  EWL_CONTAINER(w)->children = NULL;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_container_destroy_recursive(Ewl_Widget * w, void *ev_data,
				  void *user_data)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	if (!c->children || ewd_list_is_empty(c->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(c->children);

	while ((child = ewd_list_remove_last(c->children)))
		ewl_widget_destroy_recursive(child);

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
