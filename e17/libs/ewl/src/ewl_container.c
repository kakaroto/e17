
#include <Ewl.h>

static void __ewl_container_configure(Ewl_Widget * w, void *event_data,
				      void *user_data);
static void __ewl_container_realize(Ewl_Widget * w, void *event_data,
				    void *user_data);
static void __ewl_container_reparent(Ewl_Widget * w, void *event_data,
				     void *user_data);
static void __ewl_container_destroy(Ewl_Widget * w, void *event_data,
				    void *user_data);
static void __ewl_container_destroy_recursive(Ewl_Widget * w,
					      void *event_data,
					      void *user_data);

void
ewl_container_init(Ewl_Container * c, char *appearance)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
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
			    __ewl_container_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_container_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT,
			    __ewl_container_reparent, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
			     __ewl_container_destroy, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY_RECURSIVE,
			     __ewl_container_destroy_recursive, NULL);

	DLEAVE_FUNCTION;
}

void
ewl_container_append_child(Ewl_Container * pc, Ewl_Widget * child)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * Set the child's parent field to this container, append it to the
	 * list of the container's children and then notify the child that
	 * it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(pc);
	ewd_list_append(pc->children, child);

	ewl_widget_reparent(child);

	if (!pc->children || ewd_list_is_empty(pc->children))
		evas_set_color(EWL_WIDGET(pc)->evas, pc->clip_box, 255, 255,
			       255, 255);

	DLEAVE_FUNCTION;
}

void
ewl_container_prepend_child(Ewl_Container * pc, Ewl_Widget * child)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * Set the child's parent field to this container, prepend it to the
	 * list of the container's children and then notify the child that
	 * it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(pc);
	ewd_list_prepend(pc->children, child);

	ewl_widget_reparent(child);

	if (!pc->children || ewd_list_is_empty(pc->children))
		evas_set_color(EWL_WIDGET(pc)->evas, pc->clip_box, 255, 255,
			       255, 255);

	DLEAVE_FUNCTION;
}

void
ewl_container_insert_child(Ewl_Container * pc, Ewl_Widget * child, int index)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	/*
	 * Set the child's parent field to this container, insert it on the
	 * list of the container's children at the designated position and then
	 * notify the child that it's parent has been changed.
	 */
	child->parent = EWL_WIDGET(pc);
	ewd_list_goto_index(pc->children, index);
	ewd_list_insert(pc->children, child);
	ewl_widget_reparent(child);

	if (!pc->children || ewd_list_is_empty(pc->children))
		evas_set_color(EWL_WIDGET(pc)->evas, pc->clip_box, 255, 255,
			       255, 255);

	DLEAVE_FUNCTION;
}

void
ewl_container_remove_child(Ewl_Container * pc, Ewl_Widget * child)
{
	Ewl_Widget *temp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("pc", pc);
	DCHECK_PARAM_PTR("child", child);

	ewd_list_goto_first(pc->children);

	for (temp = ewd_list_current(pc->children); temp != child;
	     ewd_list_next(pc->children));

	if (temp)
		ewd_list_remove(pc->children);

	if (!pc->children || ewd_list_is_empty(pc->children))
		evas_set_color(EWL_WIDGET(pc)->evas, pc->clip_box, 0, 0, 0,
			       0);

	DLEAVE_FUNCTION;
}

Ewl_Widget *
ewl_container_get_child_at(Ewl_Container * widget, int x, int y)
{
	Ewl_Widget *child = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	if (!widget->children || ewd_list_is_empty(widget->children))
		DRETURN_PTR(NULL);

	ewd_list_goto_first(widget->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  if (x >= CURRENT_X(child) && y >= CURRENT_Y(child)
		      && CURRENT_X(child) + CURRENT_W(child) >= x
		      && CURRENT_Y(child) + CURRENT_H(child) >= y)
			  DRETURN_PTR(child);
	  }

	DRETURN_PTR(NULL);
}

Ewl_Widget *
ewl_container_get_child_at_recursive(Ewl_Container * widget, int x, int y)
{
	Ewl_Container *child = NULL;
	Ewl_Widget *child2 = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	if (!widget->children || ewd_list_is_empty(widget->children))
		return NULL;

	ewd_list_goto_first(widget->children);

	child = widget;

	while ((child2 = ewl_container_get_child_at(child, x, y)) != NULL)
	  {
		  if (child2->recursive)
			  child = EWL_CONTAINER(ewl_container_get_child_at
						(child, x, y));
		  else
			  DRETURN_PTR(child2);
	  }

	DRETURN_PTR(NULL);
}

/*
 * When reparenting a container, it's children need the updated information
 * about the container, such as evas and evas_window.
 */
static void
__ewl_container_reparent(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Widget *child;
	Ewd_List *old;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	old = EWL_CONTAINER(w)->children;
	EWL_CONTAINER(w)->children = ewd_list_new();

	while ((child = ewd_list_remove_first(old)) != NULL)
	  {
		  ewl_container_append_child(EWL_CONTAINER(w), child);
		  ewl_widget_reparent(child);
	  }

	ewd_list_destroy(old);
}

/*
 * This is the default action to be taken by containers, it involves
 * creating and showing a clip box, as well as clipping the clip box to parent
 * clip boxes.
 */
static void
__ewl_container_realize(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION;
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
		DRETURN;

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

	DLEAVE_FUNCTION;
}

static void
__ewl_container_configure(Ewl_Widget * w, void *event_data, void *user_data)
{
	int l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->clip_box)
	  {
		  if (w->ebits_object)
			  ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

		  evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
			    CURRENT_X(w) + l, CURRENT_Y(w) + t);
		  evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
			      CURRENT_W(w) - (l + r), CURRENT_H(w) - (t + b));
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_container_destroy(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
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

	DLEAVE_FUNCTION;
}

static void
__ewl_container_destroy_recursive(Ewl_Widget * w, void *event_data,
				  void *user_data)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	if (!c->children || ewd_list_is_empty(c->children))
		DRETURN;

	ewd_list_goto_first(c->children);

	while ((child = ewd_list_remove_last(c->children)))
		ewl_widget_destroy_recursive(child);

	DLEAVE_FUNCTION;
}
