
#include <Ewl.h>

void
ewl_container_new(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	widget->container.children = ewd_list_new();
	ewl_container_clip_box_create(widget);
}

void
ewl_container_append_child(Ewl_Widget * parent, Ewl_Widget * child)
{
	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	child->evas = parent->evas;
	child->evas_window = parent->evas_window;
	child->parent = parent;

	EWL_OBJECT(child)->layer = EWL_OBJECT(parent)->layer+1;

	if (ewd_list_is_empty(parent->container.children))
		ewl_container_show_clip(parent);

	if (parent->container.free_cb)
		ewd_list_set_free_cb(parent->container.children,
							 parent->container.free_cb);

	ewd_list_append(parent->container.children, child);
}

void
ewl_container_prepend_child(Ewl_Widget * parent, Ewl_Widget * child)
{
	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	child->evas = parent->evas;
	child->evas_window = parent->evas_window;
	child->parent = parent;

	EWL_OBJECT(child)->layer = EWL_OBJECT(parent)->layer+1;

	if (ewd_list_is_empty(parent->container.children))
		ewl_container_show_clip(parent);

	if (parent->container.free_cb)
		ewd_list_set_free_cb(parent->container.children,
							 parent->container.free_cb);

	ewd_list_prepend(parent->container.children, child);
}

void
ewl_container_set_free_callback(Ewl_Widget * widget, Ewd_Free_Cb cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewd_list_set_free_cb(widget->container.children, cb);
}

void
ewl_container_insert_child(Ewl_Widget * parent, Ewl_Widget * child)
{
	ewd_list_insert(parent->container.children, child);
}

Ewl_Widget *
ewl_container_get_child_at(Ewl_Widget * widget, int x, int y)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	if (!widget->container.children ||
			ewd_list_is_empty(widget->container.children))
		return NULL;

	ewd_list_goto_first(widget->container.children);

	if (widget->type == Ewl_Widget_Table)
	 {
	   Ewl_Table_Child * child = NULL;

	   while ((child = ewd_list_next(widget->container.children)) != NULL)
	    {
        if (x >= EWL_OBJECT(child->child)->current.x &&        
            y >= EWL_OBJECT(child->child)->current.y &&
            EWL_OBJECT(child->child)->current.x +
			EWL_OBJECT(child->child)->current.w >= x &&
            EWL_OBJECT(child->child)->current.y +
			EWL_OBJECT(child->child)->current.h >= y)
            return child->child;
		}
	 }
	else
	 {
	  Ewl_Widget * child = NULL;
	  while ((child = ewd_list_next(widget->container.children)) != NULL)
	   {
	    if (x >= EWL_OBJECT(child)->current.x &&
	  		y >= EWL_OBJECT(child)->current.y &&
			EWL_OBJECT(child)->current.x + EWL_OBJECT(child)->current.w >= x &&
			EWL_OBJECT(child)->current.y + EWL_OBJECT(child)->current.h >= y)
			return child;
	   }
	}
	return NULL;
}

Ewl_Widget *
ewl_container_get_child_at_recursive(Ewl_Widget * widget, int x, int y)
{
	Ewl_Widget * child = NULL, * child2 = NULL;

	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	if (!widget->container.children ||
		ewd_list_is_empty(widget->container.children))
		return NULL;

	ewd_list_goto_first(widget->container.children);

	child = widget;

	while ((child2 = ewl_container_get_child_at(child, x, y)) != NULL) {
		if (child2->container.recursive)
			child = ewl_container_get_child_at(child, x, y);
		else
			return child2;
	}

	return NULL;
}

void
ewl_container_clip_box_create(Ewl_Widget * widget)
{
    if (!widget->container.clip_box) {
        int l = 0, r = 0, t = 0, b = 0;

        if (widget->ebits_object)
            ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

        widget->container.clip_box = evas_add_rectangle(widget->evas);
        evas_set_color(widget->evas, widget->container.clip_box,
                                                255, 255, 255, 255);
        evas_move(widget->evas, widget->container.clip_box,
            EWL_OBJECT(widget)->current.x +l, EWL_OBJECT(widget)->current.y +t);
        evas_resize(widget->evas, widget->container.clip_box,
            EWL_OBJECT(widget)->current.w - (l+r),
            EWL_OBJECT(widget)->current.h - (t+b));
        evas_set_layer(widget->evas, widget->container.clip_box,
                        EWL_OBJECT(widget)->layer);
    }
}

void
ewl_container_clip_box_resize(Ewl_Widget * widget)
{
    if (widget->container.clip_box) {
        int l = 0, r = 0, t = 0, b = 0;

        if (widget->ebits_object)
            ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

        evas_move(widget->evas, widget->container.clip_box,
            EWL_OBJECT(widget)->request.x +l, EWL_OBJECT(widget)->request.y +t);
        evas_resize(widget->evas, widget->container.clip_box,
            EWL_OBJECT(widget)->request.w - (l+r),
            EWL_OBJECT(widget)->request.h - (t+b));
    }
}

void
ewl_container_set_clip(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (widget->parent &&
			widget->parent->container.clip_box &&
			widget->container.clip_box)
		evas_set_clip(widget->evas, widget->container.clip_box,
									widget->parent->container.clip_box);

	if (widget->ebits_object)
		ebits_set_clip(widget->ebits_object,widget->parent->container.clip_box);
}

void
ewl_container_show_clip(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (widget->container.clip_box)
		evas_show(widget->evas, widget->container.clip_box);
}

void
ewl_container_hide_clip(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (widget->container.clip_box)
		evas_hide(widget->evas, widget->container.clip_box);
}
