
#include <Ewl.h>


static void __ewl_box_init(Ewl_Box * b, Ewl_Orientation o);
static void __ewl_box_realize(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_box_show(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_box_hide(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_box_destroy(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_box_destroy_recursive(Ewl_Widget * w, void *ev_data,
					void *user_data);
static void __ewl_box_theme_update(Ewl_Widget * w, void *ev_data,
				   void *user_data);

static void __ewl_hbox_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_hbox_configure_gfx(Ewl_Widget * w);
static void __ewl_hbox_configure_children(Ewl_Widget * w);
static Ewd_List *__ewl_hbox_configure_normal(Ewl_Widget * w, int *rh);
static void __ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f,
					 int rh);
static void __ewl_hbox_layout_children(Ewl_Widget * w);

static void __ewl_vbox_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_vbox_configure_gfx(Ewl_Widget * w);
static void __ewl_vbox_configure_children(Ewl_Widget * w);
static Ewd_List *__ewl_vbox_configure_normal(Ewl_Widget * w, int *rw);
static void __ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f,
					 int rw);
static void __ewl_vbox_layout_children(Ewl_Widget * w);

/*
 * Create and initialize a new box container with the specified orientation
 */
Ewl_Widget *
ewl_box_new(int orientation)
{
	Ewl_Box *box;

	box = NEW(Ewl_Box, 1);
	if (box)
		__ewl_box_init(box, orientation);

	DRETURN_PTR(EWL_WIDGET(box));
}

/*
 * Initialize the specified box to sane starting values and the given
 * orientation
 */
static void
__ewl_box_init(Ewl_Box * box, Ewl_Orientation o)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("box", box);

	/*
	 * Clear the object and assign some minimum, starting, and maximum
	 * dimensions
	 */
	memset(box, 0, sizeof(Ewl_Box));
	ewl_container_init(EWL_CONTAINER(box), EWL_WIDGET_BOX, 10, 10,
			   2048, 2048);

	EWL_WIDGET(box)->recursive = TRUE;

	/*
	 * Now attach callbacks that will be executed when events occur
	 */
	ewl_callback_append(EWL_WIDGET(box),
			    EWL_CALLBACK_REALIZE, __ewl_box_realize, NULL);
	ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_SHOW,
			    __ewl_box_show, NULL);
	ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_HIDE,
			    __ewl_box_hide, NULL);
	ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_DESTROY,
			    __ewl_box_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(box),
			    EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_box_destroy_recursive, NULL);
	ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_THEME_UPDATE,
			    __ewl_box_theme_update, NULL);
	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_CONFIGURE,
				    __ewl_hbox_configure, NULL);
	else
		ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_CONFIGURE,
				    __ewl_vbox_configure, NULL);

	/*
	 * Set the box's appropriate orientation
	 */
	box->orientation = o;
	box->spacing = 5;
	box->homogeneous = 0;
}

/*
 * Change the specified box's orientation
 */
void
ewl_box_set_orientation(Ewl_Widget * w, Ewl_Orientation o)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	/*
	 * Set the orientation and reconfigure the widget so that child
	 * widgets now have the new orientation layout
	 */
	EWL_BOX(w)->orientation = o;
	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

/*
 * Adjust the spacing between the objects in the box
 */
void
ewl_box_set_spacing(Ewl_Widget * w, unsigned int spacing)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	/*
	 * Adjust the spacing and reconfigure to apply the new spacing
	 */
	EWL_BOX(w)->spacing = spacing;
	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

/*
 * Change whether or not the box is laid out in a homogenous style
 */
void
ewl_box_set_homogeneous(Ewl_Widget * w, unsigned int homogeneous)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	/*
	 * Set homogenous and reconfigure to apply the new setting
	 */
	EWL_BOX(w)->homogeneous = homogeneous;
	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_box_append_child(Ewl_Widget * w, Ewl_Widget * c, Ewl_Fill_Policy p,
		     Ewl_Alignment a, int x_padding, int y_padding)
{
	Ewl_Box_Child *child;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);

	child = NEW(Ewl_Box_Child, 1);

	child->widget = c;
	child->alignment = a;
	child->fill_policy = p;
	child->x_padding = x_padding;
	child->y_padding = y_padding;

	c->evas = w->evas, c->evas_window = w->evas_window;
	c->parent = w;

	LAYER(c) = LAYER(w) + 1;

	ewd_list_append(EWL_CONTAINER(w)->children, child);

	DLEAVE_FUNCTION;
}

void
ewl_box_prepend_child(Ewl_Widget * w, Ewl_Widget * c, Ewl_Fill_Policy p,
		      Ewl_Alignment a, int x_padding, int y_padding)
{
	Ewl_Box_Child *child;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);

	child = NEW(Ewl_Box_Child, 1);

	child->widget = c;
	child->alignment = a;
	child->fill_policy = p;
	child->x_padding = x_padding;
	child->y_padding = y_padding;

	c->evas = w->evas, c->evas_window = w->evas_window;
	c->parent = w;

	LAYER(c) = LAYER(w) + 1;

	ewd_list_prepend(EWL_CONTAINER(w)->children, child);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;

	{
		Evas_Object *clip_box;

		clip_box = evas_add_rectangle(w->evas);
		evas_set_color(w->evas, clip_box, 255, 255, 255, 255);
		evas_set_layer(w->evas, clip_box, LAYER(w) - 1);
		if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
			evas_set_clip(w->evas, clip_box,
				      EWL_CONTAINER(w->parent)->clip_box);
		w->fx_clip_box = clip_box;

	}

	{
		Evas_Object *clip_box;

		clip_box = evas_add_rectangle(w->evas);
		evas_set_color(w->evas, clip_box, 255, 255, 255, 255);
		evas_set_layer(w->evas, clip_box, LAYER(w));
		evas_set_clip(w->evas, clip_box, w->fx_clip_box);
		evas_show(w->evas, clip_box);

		EWL_CONTAINER(w)->clip_box = clip_box;
	}

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Box *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	if (w->ebits_object)
	  {
		  ebits_hide(w->ebits_object);
		  ebits_unset_clip(w->ebits_object);
		  ebits_free(w->ebits_object);
	  }

	evas_hide(w->evas, w->fx_clip_box);
	evas_unset_clip(w->evas, w->fx_clip_box);
	evas_del_object(w->evas, w->fx_clip_box);

	evas_hide(w->evas, EWL_CONTAINER(w)->clip_box);
	evas_unset_clip(w->evas, EWL_CONTAINER(w)->clip_box);
	evas_del_object(w->evas, EWL_CONTAINER(w)->clip_box);

	ewl_callback_clear(w);

	ewl_theme_deinit_widget(w);

	FREE(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_destroy_recursive(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *child;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	if (!EWL_CONTAINER(w)->children)
		DLEAVE_FUNCTION;

	while ((child = ewd_list_remove_last(EWL_CONTAINER(w)->children))
	       != NULL)
		ewl_widget_destroy_recursive(child);

	DLEAVE_FUNCTION;
}

/*
 * Apply any new theme data and redraw the box
 */
static void
__ewl_box_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Box *b;
	char *i = NULL;
	char *v = NULL;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	/*
	 * Shouldn't do anything if the widget isn't realized yet 
	 */
	if (!w->object.realized)
		DRETURN;

	b = EWL_BOX(w);

	/*
	 * Check if GFX should be visible or not 
	 */
	if (EWL_BOX(w)->orientation == EWL_ORIENTATION_HORIZONTAL)
		v = ewl_theme_data_get(w,
				       "/appearance/box/horizontal/base/visible");
	else if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL)
		v = ewl_theme_data_get(w,
				       "/appearance/box/vertical/base/visible");

	/*
	 * Destroy old image (if any)
	 */
	if (w->ebits_object)
	  {
		  ebits_hide(w->ebits_object);
		  ebits_unset_clip(w->ebits_object);
		  ebits_free(w->ebits_object);
	  }

	/*
	 * If the graphics aren't to be shown, just jump to the end for
	 * cleanup
	 */
	if (v && !strncasecmp(v, "yes", 3))
	  {
		  if (EWL_BOX(w)->orientation == EWL_ORIENTATION_HORIZONTAL)
			  i = ewl_theme_image_get(w,
						  "/appearance/box/horizontal/base");
		  else
			  i = ewl_theme_image_get(w,
						  "/appearance/box/vertical/base");

		  if (i)
		    {
			    w->ebits_object = ebits_load(i);
			    FREE(i);

			    /*
			     * Set all the clipping and layering for the image 
			     */
			    if (w->ebits_object)
			      {
				      ebits_add_to_evas(w->ebits_object,
							w->evas);
				      ebits_set_layer(w->ebits_object,
						      EWL_OBJECT(w)->layer);
				      ebits_set_clip(w->ebits_object,
						     w->fx_clip_box);

				      ebits_show(w->ebits_object);
			      }
		    }

	  }

	IF_FREE(v);

	/*
	 * Finally configure the widget to update changes 
	 */
	ewl_widget_configure(w);

}

static void
__ewl_hbox_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	__ewl_hbox_configure_children(w);
	__ewl_hbox_configure_gfx(w);

	ewl_object_set_current_geometry(EWL_OBJECT(w),
					REQUEST_X(w), REQUEST_Y(w),
					REQUEST_W(w), REQUEST_H(w));

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_configure_gfx(Ewl_Widget * w)
{
	int l, r, t, b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	l = 0;
	r = 0;
	t = 0;
	b = 0;

	/*
	 * If the widget has been realized, then the graphics need to be
	 * updated to reflect the changes
	 */
	if (EWL_OBJECT(w)->realized)
	  {
		  if (CUSTOM_W(w))
			  REQUEST_W(w) = CUSTOM_W(w);

		  if (CUSTOM_H(w))
			  REQUEST_H(w) = CUSTOM_H(w);

		  /*
		   * Adjust the size and position of the ebits object
		   */
		  if (w->ebits_object)
		    {
			    ebits_move(w->ebits_object, REQUEST_X(w),
				       REQUEST_Y(w));
			    ebits_resize(w->ebits_object, REQUEST_W(w),
					 REQUEST_H(w));
		    }

		  if (w->fx_clip_box)
		    {
			    evas_move(w->evas, w->fx_clip_box, REQUEST_X(w),
				      REQUEST_Y(w));
			    evas_resize(w->evas, w->fx_clip_box, REQUEST_W(w),
					REQUEST_H(w));
		    }

		  /*
		   * Move the widgets clip box to the appropriate size and
		   * place
		   */
		  if (EWL_CONTAINER(w)->clip_box)
		    {
			    evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
				      REQUEST_X(w) + l, REQUEST_Y(w) + t);
			    evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
					REQUEST_W(w) - (l + r),
					REQUEST_H(w) - (t + b));
		    }
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_configure_children(Ewl_Widget * w)
{
	Ewd_List *f;
	int rh;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_hbox_configure_normal(w, &rh);
	if (f)
		__ewl_hbox_configure_fillers(w, f, rh);

	__ewl_hbox_layout_children(w);

	DLEAVE_FUNCTION;
}

static Ewd_List *
__ewl_hbox_configure_normal(Ewl_Widget * w, int *rh)
{
	Ewd_List *f = NULL;
	Ewl_Box_Child *c;
	int l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	*rh = REQUEST_H(w) - t - b;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  Ewl_Widget *cw;

		  cw = c->widget;

		  if (c->fill_policy & EWL_FILL_POLICY_FILL)
		    {
			    if (!f)
				    f = ewd_list_new();

			    ewd_list_append(f, c->widget);

			    continue;
		    }

		  if (CUSTOM_W(cw))
			  REQUEST_W(cw) = CUSTOM_W(cw);
		  else if (MAXIMUM_W(cw) >= REQUEST_W(w))
			  REQUEST_W(cw) = REQUEST_W(w) - l - r;
		  else
			  REQUEST_W(cw) = CURRENT_W(cw);

		  if (CUSTOM_H(cw))
			  REQUEST_H(cw) = CUSTOM_H(cw);
		  else if (MINIMUM_H(cw) < CURRENT_H(cw))
			  REQUEST_H(cw) = MINIMUM_H(cw);
		  else
			  REQUEST_H(cw) = CURRENT_H(cw);

		  *rh -= REQUEST_H(cw) - c->y_padding;
	  }

	return f;

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rh)
{
	Ewl_Widget *c;
	int nh;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewd_list_goto_first(f);

	nh = rh / ewd_list_nodes(f);

	while ((c = ewd_list_next(f)) != NULL)
	  {
		  if (MAXIMUM_W(c) <= REQUEST_W(w))
			  REQUEST_W(c) = REQUEST_W(w);
		  else
			  REQUEST_W(c) = MAXIMUM_W(c);

		  if (MAXIMUM_H(c) <= nh)
		    {
			    REQUEST_H(c) = MAXIMUM_H(c);
		    }
		  else
			  REQUEST_H(c) = nh;
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box_Child *c;
	int y, l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	y = REQUEST_Y(w) + t;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  if (c->alignment == EWL_ALIGNMENT_LEFT)
		    {
			    REQUEST_X(c->widget) =
				    REQUEST_X(w) + l + c->x_padding;
		    }
		  else if (c->alignment == EWL_ALIGNMENT_CENTER)
		    {
			    REQUEST_X(c->widget) = REQUEST_X(w);
			    REQUEST_X(c->widget) +=
				    (REQUEST_W(w) / 2) -
				    (REQUEST_W(c->widget) - l - r) / 2 +
				    c->x_padding;
		    }
		  else if (c->alignment == EWL_ALIGNMENT_RIGHT)
		    {
			    REQUEST_X(c->widget) =
				    (REQUEST_X(w) + REQUEST_W(w)) -
				    REQUEST_W(c->widget) - r + c->x_padding;
		    }

		  REQUEST_Y(c->widget) = y + c->y_padding;
		  y += REQUEST_H(c->widget) + c->y_padding;

		  ewl_widget_configure(c->widget);
	  }

	DLEAVE_FUNCTION;
}



static void
__ewl_vbox_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	__ewl_vbox_configure_children(w);
	__ewl_vbox_configure_gfx(w);

	ewl_object_set_current_geometry(EWL_OBJECT(w),
					REQUEST_X(w), REQUEST_Y(w),
					REQUEST_W(w), REQUEST_H(w));

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_gfx(Ewl_Widget * w)
{
	int l, r, t, b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	l = 0;
	r = 0;
	t = 0;
	b = 0;

	/*
	 * If the widget has been realized, then the graphics need to be
	 * updated to reflect the changes
	 */
	if (EWL_OBJECT(w)->realized)
	  {
		  if (CUSTOM_W(w))
			  REQUEST_W(w) = CUSTOM_W(w);

		  if (CUSTOM_H(w))
			  REQUEST_H(w) = CUSTOM_H(w);

		  /*
		   * Adjust the size and position of the ebits object
		   */
		  if (w->ebits_object)
		    {
			    ebits_move(w->ebits_object, REQUEST_X(w),
				       REQUEST_Y(w));
			    ebits_resize(w->ebits_object, REQUEST_W(w),
					 REQUEST_H(w));
		    }

		  if (w->fx_clip_box)
		    {
			    evas_move(w->evas, w->fx_clip_box, REQUEST_X(w),
				      REQUEST_Y(w));
			    evas_resize(w->evas, w->fx_clip_box, REQUEST_W(w),
					REQUEST_H(w));
		    }

		  /*
		   * Move the widgets clip box to the appropriate size and
		   * place
		   */
		  if (EWL_CONTAINER(w)->clip_box)
		    {
			    evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
				      REQUEST_X(w) + l, REQUEST_Y(w) + t);
			    evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
					REQUEST_W(w) - (l + r),
					REQUEST_H(w) - (t + b));
		    }
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_children(Ewl_Widget * w)
{
	Ewd_List *f;
	int rw;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_vbox_configure_normal(w, &rw);
	if (f)
		__ewl_vbox_configure_fillers(w, f, rw);

	__ewl_vbox_layout_children(w);

	DLEAVE_FUNCTION;
}

static Ewd_List *
__ewl_vbox_configure_normal(Ewl_Widget * w, int *rw)
{
	Ewd_List *f = NULL;
	Ewl_Box_Child *c;
	int l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	*rw = REQUEST_H(w) - l - r;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  Ewl_Widget *cw;

		  cw = c->widget;

		  if (c->fill_policy & EWL_FILL_POLICY_FILL)
		    {
			    if (!f)
				    f = ewd_list_new();

			    ewd_list_append(f, c->widget);

			    continue;
		    }

		  if (CUSTOM_H(cw) != -1)
			  REQUEST_H(cw) = CUSTOM_H(cw);
		  else if (MAXIMUM_H(cw) >= REQUEST_H(w))
			  REQUEST_H(cw) = REQUEST_H(w) - t - b;
		  else
			  REQUEST_H(cw) = CURRENT_H(cw);

		  if (CUSTOM_W(cw) != -1)
			  REQUEST_W(cw) = CUSTOM_W(cw);
		  else if (MINIMUM_W(cw) < CURRENT_W(cw))
			  REQUEST_W(cw) = MINIMUM_W(cw);
		  else
			  REQUEST_W(cw) = CURRENT_W(cw);

		  *rw -= REQUEST_W(cw) + c->x_padding;
	  }

	return f;

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rw)
{
	Ewl_Widget *c;
	int nw, l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	ewd_list_goto_first(f);

	nw = rw / ewd_list_nodes(f);

	while ((c = ewd_list_next(f)) != NULL)
	  {
		  if (MAXIMUM_H(c) <= REQUEST_H(w))
			  REQUEST_H(c) = REQUEST_H(w) - t - b;
		  else
			  REQUEST_H(c) = MAXIMUM_H(c);

		  if (MAXIMUM_W(c) <= nw)
		    {
			    REQUEST_W(c) = MAXIMUM_W(c);
		    }
		  else
			  REQUEST_W(c) = nw;
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box_Child *c;
	int x, l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	x = REQUEST_X(w) + l;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  if (c->alignment == EWL_ALIGNMENT_LEFT)
			  REQUEST_Y(c->widget) =
				  REQUEST_Y(w) + t + c->y_padding;
		  else if (c->alignment == EWL_ALIGNMENT_CENTER)
		    {
			    REQUEST_Y(c->widget) = REQUEST_Y(w);
			    REQUEST_Y(c->widget) +=
				    (REQUEST_H(w) / 2) -
				    (REQUEST_H(c->widget) / 2) + c->y_padding;
		    }
		  else if (c->alignment == EWL_ALIGNMENT_RIGHT)
			  REQUEST_Y(c->widget) =
				  (REQUEST_Y(w) + REQUEST_H(w)) -
				  REQUEST_H(c->widget) - r + c->y_padding;

		  REQUEST_X(c->widget) = x + c->x_padding;
		  x += REQUEST_W(c->widget) + c->x_padding;

		  ewl_widget_configure(c->widget);
	  }

	DLEAVE_FUNCTION;
}
