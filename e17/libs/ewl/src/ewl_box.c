
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
ewl_box_new(Ewl_Orientation o)
{
	Ewl_Box *b;

	DENTER_FUNCTION;

	b = NEW(Ewl_Box, 1);
	if (b)
		__ewl_box_init(b, o);

	DRETURN_PTR(EWL_WIDGET(b));
}

/*
 * Initialize the specified box to sane starting values and the given
 * orientation
 */
static void
__ewl_box_init(Ewl_Box * b, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("b", b);

	/*
	 * Clear the object and assign some minimum, starting, and maximum
	 * dimensions
	 */
	memset(b, 0, sizeof(Ewl_Box));
	ewl_container_init(EWL_CONTAINER(b), 100, 100, EWL_FILL_POLICY_FILL,
				EWL_ALIGNMENT_CENTER);

	w = EWL_WIDGET(b);

	w->recursive = TRUE;

	/*
	 * Now attach callbacks that will be executed when events occur
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_box_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_SHOW, __ewl_box_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, __ewl_box_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_box_destroy, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_box_destroy_recursive, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_box_theme_update, NULL);
	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				    __ewl_hbox_configure, NULL);
	else
		ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				    __ewl_vbox_configure, NULL);

	/*
	 * Set the box's appropriate orientation
	 */
	b->orientation = o;
	b->spacing = 0;
}

/*
 * Change the specified box's orientation
 */
void
ewl_box_set_orientation(Ewl_Widget * w, Ewl_Orientation o)
{
	Ewl_Box *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	/*
	 * Set the orientation and reconfigure the widget so that child
	 * widgets now have the new orientation layout
	 */
	if (b->orientation == o)
		DRETURN;

	b->orientation = o;

	ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);

	if (b->orientation == EWL_ORIENTATION_HORIZONTAL)
		ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				    __ewl_hbox_configure, NULL);
	else
		ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				    __ewl_vbox_configure, NULL);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

/*
 * Adjust the spacing between the objects in the box
 */
void
ewl_box_set_spacing(Ewl_Widget * w, unsigned int s)
{
	Ewl_Box *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	b->spacing = s;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_box_child_set_padding(Ewl_Widget * w, int xp, int yp)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (xp == 0)
		ewl_widget_del_data(w, "/x_padding");
	else
		ewl_widget_set_data(w, "/x_padding", (void *) xp);
	
	if (yp == 0)
		ewl_widget_del_data(w, "/y_padding");
	else
		ewl_widget_set_data(w, "/y_padding", (void *) yp);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	{
		w->fx_clip_box = evas_add_rectangle(w->evas);
		evas_set_color(w->evas, w->fx_clip_box, 255, 255, 255, 255);
		evas_set_layer(w->evas, w->fx_clip_box, LAYER(w) - 1);
		if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
			evas_set_clip(w->evas, w->fx_clip_box,
				      EWL_CONTAINER(w->parent)->clip_box);
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

	if (EWL_CONTAINER(w)->clip_box)
	  {
		  evas_hide(w->evas, EWL_CONTAINER(w)->clip_box);
		  evas_unset_clip(w->evas, EWL_CONTAINER(w)->clip_box);
		  evas_del_object(w->evas, EWL_CONTAINER(w)->clip_box);
	  }


	if (w->fx_clip_box)
	  {
		  evas_hide(w->evas, w->fx_clip_box);
		  evas_unset_clip(w->evas, w->fx_clip_box);
		  evas_del_object(w->evas, w->fx_clip_box);
	  }

	ewl_callback_clear(w);

	ewl_theme_deinit_widget(w);

	FREE(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_destroy_recursive(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *c;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_CONTAINER(w)->children)
		DLEAVE_FUNCTION;

	while ((c = ewd_list_remove_last(EWL_CONTAINER(w)->children)) != NULL)
		ewl_widget_destroy_recursive(c);

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
	if (!REALIZED(w))
		DRETURN;

	b = EWL_BOX(w);

	/*
	 * Check if GFX should be visible or not 
	 */
	if (b->orientation == EWL_ORIENTATION_HORIZONTAL)
		v = ewl_theme_data_get(w,
				       "/appearance/box/horizontal/base/visible");
	else
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
		  if (b->orientation == EWL_ORIENTATION_HORIZONTAL)
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

	/*
	 * Finally configure the widget to update changes 
	 */
	ewl_widget_configure(w);

}

static void
__ewl_vbox_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_object_apply_requested(w);

	__ewl_vbox_configure_children(w);
	__ewl_vbox_configure_gfx(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_gfx(Ewl_Widget * w)
{
	int l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (REALIZED(w))
	  {
		  /*
		   * Adjust the size and position of the ebits object
		   */
		  if (w->ebits_object) {
		  	ebits_move(w->ebits_object, REQUEST_X(w), REQUEST_Y(w));
			ebits_resize(w->ebits_object, REQUEST_W(w), REQUEST_H(w));
			ebits_get_insets(w->ebits_object, &l, &r, &t, &b);
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
	int rh;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_vbox_configure_normal(w, &rh);

	if (f)
		__ewl_vbox_configure_fillers(w, f, rh);

	__ewl_vbox_layout_children(w);

	DLEAVE_FUNCTION;
}

static Ewd_List *
__ewl_vbox_configure_normal(Ewl_Widget * w, int *rh)
{
	Ewd_List *f = NULL;
	Ewl_Box *box;
	Ewl_Widget *c;
	int ll = 0, rr = 0, tt = 0, bb = 0;
	int xp = 0, yp = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	*rh = REQUEST_H(w) - tt - bb;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (EWL_OBJECT(c)->fill & EWL_FILL_POLICY_FILL)
		    {
			    if (!f)
				    f = ewd_list_new();

			    ewd_list_append(f, c);

			    continue;
		    }

		  if (MAXIMUM_W(c) && MAXIMUM_W(c) < CURRENT_W(c))
			  REQUEST_W(c) = MAXIMUM_W(c);
		  else if (MINIMUM_W(c) && MINIMUM_W(c) > CURRENT_W(c))
			  REQUEST_W(c) = MINIMUM_W(c);
		  else
			  REQUEST_W(c) = CURRENT_W(c);

		  REQUEST_H(c) = CURRENT_H(c);

		  *rh -= REQUEST_H(c) + box->spacing + yp;
	  }

	return f;

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rh)
{
	Ewl_Box *b;
	Ewl_Widget *c;
	int nh, ll = 0, rr = 0, tt = 0, bb = 0;
	int xp, yp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	ewd_list_goto_first(f);

	nh = (rh - ((ewd_list_nodes(f) - 1) * b->spacing)) / ewd_list_nodes(f);

	while ((c = ewd_list_next(f)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (MAXIMUM_W(c) && MAXIMUM_W(c) < REQUEST_W(w) - ll - rr)
			  REQUEST_W(c) = MAXIMUM_W(c);
		  else if (MINIMUM_W(c) && MINIMUM_W(c) > REQUEST_W(w) - ll -rr)
		  	  REQUEST_W(c) = MINIMUM_W(c);
		  else
			  REQUEST_W(c) = REQUEST_W(w) - ll - rr - xp;

		  if (MAXIMUM_H(c) && MAXIMUM_H(c) < nh)
			  REQUEST_H(c) = MAXIMUM_H(c);
		  else if (MINIMUM_H(c) && MINIMUM_H(c) > nh)
		  	  REQUEST_H(c) = MINIMUM_H(c);
		  else
			  REQUEST_H(c) = nh - yp;
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int y, l = 0, r = 0, t = 0, b = 0;
	int xp, yp;
	int min_w = 0, min_h = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	min_h -= box->spacing;

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	y = REQUEST_Y(w) + t;

	if (!EWL_CONTAINER(w)->children ||
		ewd_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  /* If the child is bigger then the box, align it left */
		  if (REQUEST_W(c) > REQUEST_W(w) - l - r)
		  	REQUEST_X(c) = REQUEST_X(w) + l + xp;
		  /* Left alignment */
		  else if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_LEFT)
			    REQUEST_X(c) = REQUEST_X(w) + l + xp;
		  /* Right */
		  else if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_RIGHT)
		    {
		 	    REQUEST_X(c) = REQUEST_X(w) + REQUEST_W(w) - l + xp;
			    REQUEST_X(c) -= REQUEST_W(c);
		    }
		  /* Center or unknown */
                  else
                    {
                            REQUEST_X(c) = REQUEST_X(w) + xp;
                            REQUEST_X(c) += (REQUEST_W(w) / 2) -
			    			(REQUEST_W(c) / 2);
                    }

		  REQUEST_Y(c) = y + yp;
		  /* Update y so the next child comes beneath it */
		  y = REQUEST_Y(c) + REQUEST_H(c) + box->spacing;

		  ewl_widget_configure(c);

		  if (MINIMUM_W(c) > min_w)
			  min_w += MINIMUM_W(c);

		  min_h += MINIMUM_H(c) + box->spacing;
	  }

	ewl_object_set_minimum_size(EWL_OBJECT(w), min_w, min_h);

	DLEAVE_FUNCTION;
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
	int l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * If the widget has been realized, then the graphics need to be
	 * updated to reflect the changes
	 */
	if (REALIZED(w))
	  {
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
	int rw;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_hbox_configure_normal(w, &rw);

	if (f)
		__ewl_hbox_configure_fillers(w, f, rw);

	__ewl_hbox_layout_children(w);

	DLEAVE_FUNCTION;
}

static Ewd_List *
__ewl_hbox_configure_normal(Ewl_Widget * w, int *rw)
{
	Ewd_List *f = NULL;
	Ewl_Box *box;
	Ewl_Widget *c;
	int ll = 0, rr = 0, tt = 0, bb = 0;
	int xp, yp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	*rw = REQUEST_W(w) - ll - rr;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		xp = (int) ewl_widget_get_data(c, "/x_padding");
		yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (EWL_OBJECT(c)->fill & EWL_FILL_POLICY_FILL)
		    {
			    if (!f)
				    f = ewd_list_new();

			    ewd_list_append(f, c);

			    continue;
		    }

		  if (MAXIMUM_H(c) < REQUEST_H(w) - tt - bb)
			  REQUEST_H(c) = MAXIMUM_H(c) - yp;
		  else if (MINIMUM_W(c) > REQUEST_W(w))
		  	  REQUEST_W(c) = MINIMUM_W(c);
		  else
			  REQUEST_H(c) = REQUEST_H(w) - tt - bb - yp;

		  REQUEST_W(c) = CURRENT_W(c);

		  *rw -= REQUEST_W(c) + box->spacing - xp;
	  }

	return f;

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rw)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int nw, ll = 0, rr = 0, tt = 0, bb = 0;
	int xp, yp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	ewd_list_goto_first(f);

	nw = (rw - (ewd_list_nodes(f) - 1) * box->spacing) / ewd_list_nodes(f);

	while ((c = ewd_list_next(f)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (MAXIMUM_H(c) && MAXIMUM_H(c) < REQUEST_H(w) - tt - bb)
			  REQUEST_H(c) = MAXIMUM_H(c);
		  else
			  REQUEST_H(c) = REQUEST_H(w) - ll - bb - yp;

		  if (MAXIMUM_W(c) && MAXIMUM_W(c) < nw)
			  REQUEST_W(c) = MAXIMUM_W(c);
		  else
			  REQUEST_W(c) = nw - xp;
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int x, l = 0, r = 0, t = 0, b = 0;
	int xp, yp;
	int min_w = 0, min_h = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	min_w -= box->spacing;

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	x = REQUEST_X(w) + l;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");


		  /* If the child is bigger then the box, align it to the TOP */
		  if (REQUEST_H(c) > REQUEST_H(w) - t - b)
		  	  REQUEST_Y(c) = REQUEST_Y(w) + t + yp;
		  /* Top alignment */
		  if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_TOP)
			  REQUEST_Y(c) = REQUEST_Y(w) + t + yp;
		  /* Bottom */
                  else if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_BOTTOM)
                          REQUEST_Y(c) = (REQUEST_Y(w) + REQUEST_H(w)) -
                                  REQUEST_H(c) - r + yp;
		  /* Center or unknown */
		  else
		    {
			    REQUEST_Y(c) = REQUEST_Y(w) + yp;
			    REQUEST_Y(c) += (REQUEST_H(w) / 2) -
			    			(REQUEST_H(c) / 2);
		    }

		  REQUEST_X(c) = x + xp;
		  x = REQUEST_X(c) + REQUEST_W(c) + box->spacing;

		  ewl_widget_configure(c);

		  min_w += MINIMUM_W(c) + box->spacing;

		  if (MINIMUM_H(c) > min_h)
		  	min_h += MINIMUM_H(c);
	  }

	ewl_object_set_minimum_size(EWL_OBJECT(w), min_w, min_h);

	DLEAVE_FUNCTION;
}
