
#include <Ewl.h>

static void ewl_container_reparent(Ewl_Widget * w, void *event_data,
				   void *user_data);

void
ewl_container_init(Ewl_Container * widget, int type, int min_w, int min_h,
		   int max_w, int max_h)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	/*
	 * Initialize the fields inherited from the widget class
	 */
	ewl_widget_init(EWL_WIDGET(widget), type, min_w, min_h, max_w, max_h);
	EWL_WIDGET(widget)->recursive = TRUE;

	/*
	 * Initialize the fields specific to the container class.
	 */
	widget->children = ewd_list_new();

	/*
	 * All containers need to perform the function of updating the
	 * children with necessary window and evas information.
	 */
	ewl_callback_append(EWL_WIDGET(widget), EWL_CALLBACK_REPARENT,
			    ewl_container_reparent, NULL);

	DLEAVE_FUNCTION;
}

void
ewl_container_append_child(Ewl_Container * parent, Ewl_Widget * child)
{
	int reparent = FALSE;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	if (child->parent)
		reparent = TRUE;

	child->evas = EWL_WIDGET(parent)->evas;
	child->evas_window = EWL_WIDGET(parent)->evas_window;
	child->parent = EWL_WIDGET(parent);

	LAYER(child) = LAYER(parent) + 1;

	if (ewd_list_is_empty(parent->children))
		ewl_container_show_clip(parent);

	if (parent->free_cb)
		ewd_list_set_free_cb(parent->children, parent->free_cb);

	ewd_list_append(parent->children, child);

	if (reparent == TRUE)
		ewl_callback_call(EWL_WIDGET(child), EWL_CALLBACK_REPARENT);

	DLEAVE_FUNCTION;
}

void
ewl_container_prepend_child(Ewl_Container * parent, Ewl_Widget * child)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	child->evas = EWL_WIDGET(parent)->evas;
	child->evas_window = EWL_WIDGET(parent)->evas_window;
	child->parent = EWL_WIDGET(parent);

	EWL_OBJECT(child)->layer = EWL_OBJECT(parent)->layer + 1;

	if (ewd_list_is_empty(parent->children))
		ewl_container_show_clip(parent);

	if (parent->free_cb)
		ewd_list_set_free_cb(parent->children, parent->free_cb);

	ewd_list_prepend(parent->children, child);

	DLEAVE_FUNCTION;
}

void
ewl_container_set_free_callback(Ewl_Container * widget, Ewd_Free_Cb cb)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ewd_list_set_free_cb(widget->children, cb);

	DLEAVE_FUNCTION;
}

void
ewl_container_insert_child(Ewl_Container * parent, Ewl_Widget * child,
			   int index)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	child->evas = EWL_WIDGET(parent)->evas;
	child->evas_window = EWL_WIDGET(parent)->evas_window;
	child->parent = EWL_WIDGET(parent);

	EWL_OBJECT(child)->layer = EWL_OBJECT(parent)->layer + 1;

	if (ewd_list_is_empty(parent->children))
		ewl_container_show_clip(parent);

	if (parent->free_cb)
		ewd_list_set_free_cb(parent->children, parent->free_cb);

	ewd_list_goto_index(parent->children, index);
	ewd_list_insert(parent->children, child);

	DLEAVE_FUNCTION;
}

Ewl_Widget *
ewl_container_get_child_at(Ewl_Container * widget, int x, int y)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	if (!widget->children || ewd_list_is_empty(widget->children))
		return NULL;

	ewd_list_goto_first(widget->children);

	if (EWL_WIDGET(widget)->type == EWL_WIDGET_TABLE)
	  {
		  Ewl_Table_Child *child = NULL;

		  while ((child = ewd_list_next(widget->children)) != NULL)
		    {
			    if (x >= EWL_OBJECT(child->widget)->current.x
				&& y >= EWL_OBJECT(child->widget)->current.y
				&& EWL_OBJECT(child->widget)->current.x +
				EWL_OBJECT(child->widget)->current.w >= x
				&& EWL_OBJECT(child->widget)->current.y +
				EWL_OBJECT(child->widget)->current.h >= y)
				    return child->widget;
		    }
	  }
	else if (EWL_WIDGET(widget)->type == EWL_WIDGET_BOX)
	  {
		  Ewl_Box_Child *child = NULL;

		  while ((child = ewd_list_next(widget->children)) != NULL)
		    {
			    if (x >= EWL_OBJECT(child->widget)->current.x
				&& y >= EWL_OBJECT(child->widget)->current.y
				&& EWL_OBJECT(child->widget)->current.x +
				EWL_OBJECT(child->widget)->current.w >= x
				&& EWL_OBJECT(child->widget)->current.y +
				EWL_OBJECT(child->widget)->current.h >= y)
				    return child->widget;
		    }
	  }
	{
		Ewl_Widget *child = NULL;
		while ((child =
			ewd_list_next(EWL_CONTAINER(widget)->children)) !=
		       NULL)
		  {
			  if (x >= EWL_OBJECT(child)->current.x
			      && y >= EWL_OBJECT(child)->current.y
			      && EWL_OBJECT(child)->current.x +
			      EWL_OBJECT(child)->current.w >= x
			      && EWL_OBJECT(child)->current.y +
			      EWL_OBJECT(child)->current.h >= y)
				  return child;
		  }
	}

	return NULL;
}

Ewl_Widget *
ewl_container_get_child_at_recursive(Ewl_Container * widget, int x, int y)
{
	Ewl_Container *child = NULL;
	Ewl_Widget *child2 = NULL;

	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

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
			  return child2;
	  }

	return NULL;
}

void
ewl_container_clip_box_create(Ewl_Container * widget)
{
	DENTER_FUNCTION;

	if (!widget->clip_box)
	  {
		  widget->clip_box =
			  evas_add_rectangle(EWL_WIDGET(widget)->evas);
		  evas_set_color(EWL_WIDGET(widget)->evas, widget->clip_box,
				 255, 255, 255, 255);
		  evas_move(EWL_WIDGET(widget)->evas, widget->clip_box,
			    EWL_OBJECT(widget)->current.x,
			    EWL_OBJECT(widget)->current.y);
		  evas_resize(EWL_WIDGET(widget)->evas, widget->clip_box,
			      EWL_OBJECT(widget)->current.w,
			      EWL_OBJECT(widget)->current.h);
		  evas_set_layer(EWL_WIDGET(widget)->evas, widget->clip_box,
				 EWL_OBJECT(widget)->layer);
		  evas_show(EWL_WIDGET(widget)->evas, widget->clip_box);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_container_clip_box_resize(Ewl_Container * widget)
{
	DENTER_FUNCTION;

	if (EWL_CONTAINER(widget)->clip_box)
	  {
		  evas_move(EWL_WIDGET(widget)->evas,
			    EWL_CONTAINER(widget)->clip_box,
			    EWL_OBJECT(widget)->request.x,
			    EWL_OBJECT(widget)->request.y);
		  evas_resize(EWL_WIDGET(widget)->evas,
			      EWL_CONTAINER(widget)->clip_box,
			      EWL_OBJECT(widget)->request.w,
			      EWL_OBJECT(widget)->request.h);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_container_set_clip(Ewl_Container * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_WIDGET(widget)->parent && EWL_WIDGET(widget)->fx_clip_box &&
	    widget->clip_box)
		evas_set_clip(EWL_WIDGET(widget)->evas, widget->clip_box,
			      EWL_WIDGET(widget)->fx_clip_box);

	DLEAVE_FUNCTION;
}

void
ewl_container_show_clip(Ewl_Container * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (widget->clip_box)
		evas_show(EWL_WIDGET(widget)->evas, widget->clip_box);

	DLEAVE_FUNCTION;
}

void
ewl_container_hide_clip(Ewl_Container * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (widget->clip_box)
		evas_hide(EWL_WIDGET(widget)->evas, widget->clip_box);

	DLEAVE_FUNCTION;
}

static void
ewl_container_reparent(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Widget *child;
	Ewd_List *old;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("w", w);

	old = EWL_CONTAINER(w)->children;
	EWL_CONTAINER(w)->children = ewd_list_new();

	while ((child = ewd_list_remove_first(old)) != NULL)
	  {
		  ewl_container_append_child(EWL_CONTAINER(w), child);
		  ewl_callback_call(child, EWL_CALLBACK_REPARENT);
	  }
}
