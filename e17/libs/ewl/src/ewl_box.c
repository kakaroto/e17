#include <Ewl.h>

static void __ewl_box_init(Ewl_Box * b, Ewl_Orientation o);
static void __ewl_box_realize(Ewl_Widget * w, void *event_data,
			      void *user_data);
static void __ewl_box_show(Ewl_Widget * w, void *event_data, void *user_data);
static void __ewl_box_hide(Ewl_Widget * w, void *event_data, void *user_data);
static void __ewl_box_destroy(Ewl_Widget * w, void *event_data,
			      void *user_data);
static void __ewl_box_destroy_recursive(Ewl_Widget * w, void *event_data,
					void *user_data);
static void __ewl_box_configure(Ewl_Widget * w, void *event_data,
				void *user_data);
static void __ewl_box_theme_update(Ewl_Widget * w, void *event_data,
				   void *user_data);

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
	ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_CONFIGURE,
			    __ewl_box_configure, NULL);
	ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_THEME_UPDATE,
			    __ewl_box_theme_update, NULL);

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

static void
__ewl_box_realize(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	ewl_fx_clip_box_create(w);

	if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
	  {
		  evas_set_clip(w->evas, w->fx_clip_box,
				EWL_CONTAINER(w->parent)->clip_box);

		  evas_set_clip(w->evas, EWL_CONTAINER(w)->clip_box,
				w->fx_clip_box);
	  }

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_show(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_hide(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_destroy(Ewl_Widget * w, void *event_data, void *user_data)
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

	ewl_callback_del_all(w);

	ewl_theme_deinit_widget(w);

	FREE(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_destroy_recursive(Ewl_Widget * w, void *event_data, void *user_data)
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
 * Update the size and position of the box and its child widgets
 */
static void
__ewl_box_configure(Ewl_Widget * w, void *event_data, void *user_data)
{

	Ewl_Widget *child;
	int l = 0, r = 0, t = 0, b = 0;
	int height = 0, width = 0, x = 0, y = 0;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	CURRENT_X(w) = REQUEST_X(w);
	CURRENT_Y(w) = REQUEST_Y(w);
	CURRENT_W(w) = REQUEST_W(w);
	CURRENT_H(w) = REQUEST_H(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

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
		  ewl_fx_clip_box_resize(w);

		  /*
		   * Move the widgets clip box to the appropriate size and
		   * place
		   */
		  if (EWL_CONTAINER(w)->clip_box)
		    {
			    evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
				      CURRENT_X(w) + l, CURRENT_Y(w) + t);
			    evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
					CURRENT_W(w) - (l + r),
					CURRENT_H(w) - (t + b));
		    }
	  }

	/*
	 * If their aren't any children to this widget, then we're done
	 */
	if (!EWL_CONTAINER(w)->children || !EWL_CONTAINER(w)->children->nodes)
		DRETURN;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	/*
	 * The starting x, y values are the same whether or not this is a
	 * vertical or horizontal box.
	 */
	x = CURRENT_X(w) + l;
	y = CURRENT_Y(w) + t;

	/*
	 * The values for the width and height are dependant on the
	 * orientation of the box
	 */
	if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL)
	  {
		  width = CURRENT_W(w) - (l + r);
		  height = CURRENT_H(w) /
			  ewd_list_nodes(EWL_CONTAINER(w)->children);
		  height -= t + b;
	  }
	else
	  {
		  width = CURRENT_W(w) / EWL_CONTAINER(w)->children->nodes;
		  width -= l + r;
		  height = CURRENT_H(w) - (t + b);
	  }

	while ((child = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  REQUEST_X(child) = x;
		  REQUEST_Y(child) = y;
		  REQUEST_W(child) = width;
		  REQUEST_H(child) = height;

		  if (REQUEST_W(child) < MIN_W(child))
		    {
			    REQUEST_X(child) += (REQUEST_W(child) / 2) -
				    (MIN_W(child) / 2);
			    REQUEST_W(child) = MIN_W(child);
		    }

		  if (REQUEST_H(child) < MIN_H(child))
		    {
			    REQUEST_Y(child) += (REQUEST_H(child) / 2) -
				    (MIN_H(child) / 2);
			    REQUEST_H(child) = MIN_H(child);
		    }

		  if (REQUEST_W(child) > MAX_W(child))
		    {
			    REQUEST_X(child) += (REQUEST_W(child) / 2) -
				    (MAX_W(child) / 2);
			    REQUEST_W(child) = MAX_W(child);
		    }

		  if (REQUEST_H(child) > MAX_H(child))
		    {
			    REQUEST_Y(child) += (REQUEST_H(child) / 2) -
				    (MAX_H(child) / 2);
			    REQUEST_H(child) = MAX_H(child);
		    }

		  if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL)
		    {
			    if (REQUEST_X(child) < CURRENT_X(w) + l)
				    REQUEST_X(child) = CURRENT_X(w) + l;
			    y += height + EWL_BOX(w)->spacing;
		    }
		  else
		    {
			    if (REQUEST_Y(child) < CURRENT_Y(w) + t)
				    REQUEST_Y(child) = CURRENT_Y(w) + t;
			    x += width + EWL_BOX(w)->spacing;
		    }

		  ewl_widget_configure(child);

	  }

	DLEAVE_FUNCTION;
}

/*
 * Apply any new theme data and redraw the box
 */
static void
__ewl_box_theme_update(Ewl_Widget * w, void *event_data, void *user_data)
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
	if (v && strncasecmp(v, "yes", 3) != 0)
		goto done_update_box_theme;

	if (EWL_BOX(w)->orientation == EWL_ORIENTATION_HORIZONTAL)
		i = ewl_theme_image_get(w, "/appearance/box/horizontal/base");
	else
		i = ewl_theme_image_get(w, "/appearance/box/vertical/base");

	if (i)
	  {
		  w->ebits_object = ebits_load(i);
		  FREE(i);

		  /*
		   * Set all the clipping and layering for the image 
		   */
		  if (w->ebits_object)
		    {
			    ebits_add_to_evas(w->ebits_object, w->evas);
			    ebits_set_layer(w->ebits_object,
					    EWL_OBJECT(w)->layer);
			    ebits_set_clip(w->ebits_object, w->fx_clip_box);

			    ebits_show(w->ebits_object);
		    }
	  }

      done_update_box_theme:

	IF_FREE(v);

	/*
	 * Finally configure the widget to update changes 
	 */
	ewl_widget_configure(w);

}
