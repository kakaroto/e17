
#include <Ewl.h>


static void __ewl_box_init(Ewl_Box * b, Ewl_Orientation o);
static void __ewl_box_realize(Ewl_Widget * w, void *ev_data,
			      void *user_data);
static void __ewl_box_show(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_box_hide(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_box_destroy(Ewl_Widget * w, void *ev_data,
			      void *user_data);
static void __ewl_box_destroy_recursive(Ewl_Widget * w, void *ev_data,
					void *user_data);
static void __ewl_box_theme_update(Ewl_Widget * w, void *ev_data,
				   void *user_data);

static void __ewl_box_configure(Ewl_Widget * w, void *ev_data,
				void *user_data);
static int __ewl_box_adjust(Ewl_Widget * w);
static void __ewl_box_configure_gfx(Ewl_Widget * w);
static void __ewl_hbox_configure_children(Ewl_Widget * w);

static void __ewl_vbox_configure_children(Ewl_Widget * w);

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
	ewl_container_init(EWL_CONTAINER(b), 10, 10, EWL_FILL_POLICY_FILL,
			   EWL_ALIGNMENT_CENTER);

	w = EWL_WIDGET(b);

	w->recursive = TRUE;

	/*
	 * Now attach callbacks that will be executed when events occur
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_box_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_SHOW, __ewl_box_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, __ewl_box_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_box_destroy,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_box_destroy_recursive, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_box_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_box_configure,
			    NULL);

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
ewl_box_set_orientation(Ewl_Box * b, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	/*
	 * Set the orientation and reconfigure the widget so that child
	 * widgets now have the new orientation layout
	 */
	if (b->orientation == o)
		DRETURN;

	b->orientation = o;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

/*
 * Adjust the spacing between the objects in the box
 */
void
ewl_box_set_spacing(Ewl_Box * b, unsigned int s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	b->spacing = s;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_box_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

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

	if (w->ebits_object) {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	}

	if (EWL_CONTAINER(w)->clip_box) {
		evas_hide(w->evas, EWL_CONTAINER(w)->clip_box);
		evas_unset_clip(w->evas, EWL_CONTAINER(w)->clip_box);
		evas_del_object(w->evas, EWL_CONTAINER(w)->clip_box);
	}


	if (w->fx_clip_box) {
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

	while ((c =
		ewd_list_remove_last(EWL_CONTAINER(w)->children)) != NULL)
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
	else if (b->orientation == EWL_ORIENTATION_VERTICAL)
		v = ewl_theme_data_get(w,
				       "/appearance/box/vertical/base/visible");

	/*
	 * Destroy old image (if any)
	 */
	if (w->ebits_object) {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	}

	/*
	 * If the graphics aren't to be shown, just jump to the end for
	 * cleanup
	 */
	if (v && !strncasecmp(v, "yes", 3)) {
		if (b->orientation == EWL_ORIENTATION_HORIZONTAL)
			i = ewl_theme_image_get(w,
						"/appearance/box/horizontal/base");
		else if (b->orientation == EWL_ORIENTATION_VERTICAL)
			i = ewl_theme_image_get(w,
						"/appearance/box/vertical/base");

		if (i) {
			w->ebits_object = ebits_load(i);
			FREE(i);

			/*
			 * Set all the clipping and layering for the image 
			 */
			if (w->ebits_object) {
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
__ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Adjust the box so that it has the correct minimum size for it's
	 * contents. If it gets adjusted, then we need to let it's parent
	 * know. The parent will call configure on this widget again, so we
	 * need to return here.
	 */
	if (__ewl_box_adjust(w)) {
		ewl_widget_configure(w->parent);
		return;
	}

	ewl_object_apply_requested(EWL_OBJECT(w));

	if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL) {
		__ewl_vbox_configure_children(w);
	} else {
		__ewl_hbox_configure_children(w);
	}
	__ewl_box_configure_gfx(w);

	DLEAVE_FUNCTION;
}

/*
 * This function adjusts the minimum size of the box to require that it
 * is at least as big as its children.
 */
static int
__ewl_box_adjust(Ewl_Widget * w)
{
	Ewl_Widget *c;
	int min_w = 0, min_h = 0;

	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((c = ewd_list_next(EWL_CONTAINER(w)->children))) {

		/*
		 * A vertical box will sum the minimum heights of it's children
		 * and keep track of the largest minimum width, and the
		 * horizontal will do the opposite.
		 */
		if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL) {
			if (min_w < MIN_W(c))
				min_w = MIN_W(c);
			min_h += MIN_H(c) + EWL_BOX(w)->spacing;
		} else {
			min_w += MIN_W(c) + EWL_BOX(w)->spacing;
			if (min_h < MIN_H(c))
				min_h = MIN_H(c);
		}
	}

	/*
	 * Apply the new calculated minimum sizes
	 */
	if (min_w > MIN_W(w) || min_h > MIN_H(w)) {
		ewl_object_set_minimum_size(EWL_OBJECT(w), min_w, min_h);

		return TRUE;
	}

	return FALSE;
}

static void
__ewl_box_configure_gfx(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * If the widget has been realized, then the graphics need to be
	 * updated to reflect the changes
	 */
	if (!REALIZED(w))
		DRETURN;

	/*
	 * Adjust the size and position of the ebits object
	 */
	if (w->ebits_object) {
		ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));
		ebits_resize(w->ebits_object, CURRENT_W(w), CURRENT_H(w));
	}

	if (w->fx_clip_box) {
		evas_move(w->evas, w->fx_clip_box, CURRENT_X(w),
			  CURRENT_Y(w));
		evas_resize(w->evas, w->fx_clip_box, CURRENT_W(w),
			    CURRENT_H(w));
	}

	/*
	 * Move the widgets clip box to the appropriate size and
	 * place
	 */
	if (EWL_CONTAINER(w)->clip_box) {
		evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
			  CURRENT_X(w), CURRENT_Y(w));
		evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
			    CURRENT_W(w), CURRENT_H(w));
	}

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_configure_children(Ewl_Widget * w)
{
	Ewl_Widget *c;

	/*
	 * Lists for determining the alignment of children
	 */
	Ewd_List *ll = NULL, *cl = NULL, *rl = NULL;

	/*
	 * Temporary ints for placing the next child in a specifies alignment
	 */
	int left_x = 0, right_x = 0, center_x = 0;
	int top_y = 0, bottom_y = 0, center_y = 0;

	int rem_w, fillers = 0, center_size = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (ewd_list_nodes(EWL_CONTAINER(w)->children) == 0)
		DRETURN;

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &left_x, &top_y,
				 &right_x, &bottom_y);

	/*
	 * Set up some temporary variables for layout and calculate starting
	 * positions from the ebits insets
	 */
	rem_w = CURRENT_W(w);
	left_x += CURRENT_X(w);
	right_x = CURRENT_X(w) + CURRENT_W(w) - right_x;
	center_x = left_x + ((right_x - left_x) / 2);

	top_y += CURRENT_Y(w);
	bottom_y = CURRENT_Y(w) + CURRENT_H(w) - bottom_y;
	center_y = top_y + ((bottom_y - top_y) / 2);

	/*
	 * Sort the list by position order to allow for easy placement
	 */
	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((c = ewd_list_next(EWL_CONTAINER(w)->children))) {

		/*
		 * Break up the children into one of three lists depending on
		 * alignment
		 */
		if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_LEFT) {
			if (!ll)
				ll = ewd_dlist_new();
			ewd_list_append(ll, c);
		} else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_RIGHT) {
			if (!rl)
				rl = ewd_dlist_new();
			ewd_list_append(rl, c);
		} else {
			if (!cl)
				cl = ewd_dlist_new();
			ewd_list_append(cl, c);
			center_size += CURRENT_W(c);
		}

		if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL)
			fillers++;

		/*
		 * Since this is a horizontal box, we can set the vertical
		 * positions the first time through the function.
		 */
		if (CURRENT_H(c) > bottom_y - top_y)
			REQUEST_H(c) = bottom_y - top_y;

		if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_TOP)
			REQUEST_Y(c) = top_y;
		else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_BOTTOM)
			REQUEST_Y(c) = bottom_y - REQUEST_H(c);
		else
			REQUEST_Y(c) = center_y - (REQUEST_H(c) / 2);

		rem_w -= CURRENT_W(c);
	}

	/*
	 * Traverse the left list to determine the position of the widgets
	 * with a left alignment policy
	 */
	if (ll)
		ewd_list_goto_first(ll);
	while (ll && (c = ewd_list_next(ll))) {

		/*
		 * If we are short on space start setting widgets down to
		 * their minimum size. Fillers should have their size
		 * increased by any available space.
		 */
		REQUEST_X(c) = left_x;
		if (rem_w < 0) {
			REQUEST_W(c) = MIN_W(c);
			left_x += MIN_W(c) + EWL_BOX(w)->spacing;
			rem_w += CURRENT_W(c) - MIN_W(c);
		} else if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL) {
			REQUEST_W(c) += rem_w / fillers;
			left_x += REQUEST_W(c) + EWL_BOX(w)->spacing;
			rem_w -= rem_w / fillers;
			fillers--;
		} else
			left_x += CURRENT_W(c) + EWL_BOX(w)->spacing;

		ewl_widget_configure(c);

	}

	if (ll) {
		ewd_list_clear(ll);
		ewd_list_destroy(ll);
	}

	/*
	 * Traverse the right list to determine the position of the widgets
	 * with a right alignment policy
	 */
	if (rl)
		ewd_list_goto_first(rl);
	while (rl && (c = ewd_list_next(rl))) {

		/*
		 * If we are short on space start setting widgets down to
		 * their minimum size. Fillers should have their size
		 * increased by any available space.
		 */
		if (rem_w < 0) {
			REQUEST_W(c) = MIN_W(c);
			right_x -= MIN_W(c) + EWL_BOX(w)->spacing;
			rem_w += CURRENT_W(c) - MIN_W(c);
		} else if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL) {
			REQUEST_W(c) += rem_w / fillers;
			right_x -= REQUEST_W(c) + EWL_BOX(w)->spacing;
			rem_w -= rem_w / fillers;
			fillers--;
		} else
			right_x -= CURRENT_W(c) + EWL_BOX(w)->spacing;

		REQUEST_X(c) = right_x;

		ewl_widget_configure(c);
	}

	if (rl) {
		ewd_list_clear(rl);
		ewd_list_destroy(rl);
	}

	/*
	 * Traverse the center list to determine the position of the widgets
	 * with a center alignment policy
	 */
	if (cl) {
		ewd_list_goto_first(cl);

		if (fillers > 0)
			center_x = left_x;
		else
			center_x =
			    left_x + ((right_x - left_x) - center_size)
			    / 2;
	}

	while (cl && (c = ewd_list_next(cl))) {

		/*
		 * If we are short on space start setting widgets down to
		 * their minimum size. Fillers should have their size
		 * increased by any available space.
		 */
		REQUEST_X(c) = center_x;
		if (rem_w < 0) {
			REQUEST_W(c) = MIN_W(c);
			center_x += MIN_W(c) + EWL_BOX(w)->spacing;
			rem_w += CURRENT_W(c) - MIN_W(c);
		} else if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL) {
			REQUEST_W(c) += rem_w / fillers;
			center_x += REQUEST_W(c) + EWL_BOX(w)->spacing;
			rem_w -= rem_w / fillers;
			fillers--;
		} else
			center_x += CURRENT_W(c) + EWL_BOX(w)->spacing;

		ewl_widget_configure(c);
	}

	if (cl) {
		ewd_list_clear(cl);
		ewd_list_destroy(cl);
	}

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_children(Ewl_Widget * w)
{
	Ewl_Widget *c;

	/*
	 * Lists for determining the alignment of children
	 */
	Ewd_List *tl = NULL, *cl = NULL, *bl = NULL;

	/*
	 * Temporary ints for placing the next child in a specifies alignment
	 */
	int left_x = 0, right_x = 0, center_x = 0;
	int top_y = 0, bottom_y = 0, center_y = 0;

	int rem_h, fillers = 0, center_size = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (ewd_list_nodes(EWL_CONTAINER(w)->children) == 0)
		DRETURN;

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &left_x, &top_y,
				 &right_x, &bottom_y);

	/*
	 * Set up some temporary variables for layout and calculate starting
	 * positions from the ebits insets
	 */
	rem_h = CURRENT_H(w);
	top_y += CURRENT_Y(w);
	bottom_y = CURRENT_Y(w) + CURRENT_H(w) - bottom_y;
	center_y = top_y + ((bottom_y - top_y) / 2);

	left_x += CURRENT_X(w);
	right_x = CURRENT_X(w) + CURRENT_W(w) - right_x;
	center_x = left_x + ((right_x - left_x) / 2);

	/*
	 * Sort the list by position order to allow for easy placement
	 */
	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((c = ewd_list_next(EWL_CONTAINER(w)->children))) {

		/*
		 * Break up the children into one of three lists depending on
		 * alignment
		 */
		if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_TOP) {
			if (!tl)
				tl = ewd_dlist_new();
			ewd_list_append(tl, c);
		} else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_BOTTOM) {
			if (!bl)
				bl = ewd_dlist_new();
			ewd_list_append(bl, c);
		} else {
			if (!cl)
				cl = ewd_dlist_new();
			ewd_list_append(cl, c);
			center_size += CURRENT_H(c);
		}

		if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL)
			fillers++;

		/*
		 * Since this is a vertical box, we can set the horizontal
		 * positions the first time through the function.
		 */
		if (CURRENT_W(c) > right_x - left_x)
			REQUEST_W(c) = right_x - left_x;

		if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_LEFT)
			REQUEST_X(c) = left_x;
		else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_RIGHT)
			REQUEST_X(c) = right_x - REQUEST_W(c);
		else
			REQUEST_X(c) = center_x - (REQUEST_W(c) / 2);

		rem_h -= CURRENT_H(c);
	}

	/*
	 * Traverse the left list to determine the position of the widgets
	 * with a left alignment policy
	 */
	if (tl)
		ewd_list_goto_first(tl);
	while (tl && (c = ewd_list_next(tl))) {

		/*
		 * If we are short on space start setting widgets down to
		 * their minimum size. Fillers should have their size
		 * increased by any available space.
		 */
		REQUEST_Y(c) = top_y;
		if (rem_h < 0) {
			REQUEST_H(c) = MIN_H(c);
			top_y += MIN_H(c);
			rem_h += CURRENT_H(c) - MIN_H(c);
		} else if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL) {
			REQUEST_Y(c) += rem_h / fillers;
			top_y += REQUEST_H(c);
			rem_h -= rem_h / fillers;
			fillers--;
		} else
			top_y += CURRENT_H(c);

		ewl_widget_configure(c);

	}

	if (tl) {
		ewd_list_clear(tl);
		ewd_list_destroy(tl);
	}

	/*
	 * Traverse the right list to determine the position of the widgets
	 * with a right alignment policy
	 */
	if (bl)
		ewd_list_goto_first(bl);
	while (bl && (c = ewd_list_next(bl))) {

		/*
		 * If we are short on space start setting widgets down to
		 * their minimum size. Fillers should have their size
		 * increased by any available space.
		 */
		if (rem_h < 0) {
			REQUEST_H(c) = MIN_H(c);
			bottom_y -= MIN_H(c);
			rem_h += CURRENT_H(c) - MIN_H(c);
		} else if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL) {
			REQUEST_H(c) += rem_h / fillers;
			bottom_y -= REQUEST_H(c);
			rem_h -= rem_h / fillers;
			fillers--;
		} else
			bottom_y -= CURRENT_H(c);

		REQUEST_Y(c) = bottom_y;

		ewl_widget_configure(c);
	}

	if (bl) {
		ewd_list_clear(bl);
		ewd_list_destroy(bl);
	}

	/*
	 * Traverse the center list to determine the position of the widgets
	 * with a center alignment policy
	 */
	if (cl) {
		ewd_list_goto_first(cl);

		if (fillers > 0)
			center_y = top_y;
		else
			center_y =
			    top_y + ((bottom_y - top_y) - center_size)
			    / 2;
	}

	while (cl && (c = ewd_list_next(cl))) {

		/*
		 * If we are short on space start setting widgets down to
		 * their minimum size. Fillers should have their size
		 * increased by any available space.
		 */
		REQUEST_Y(c) = center_y;
		if (rem_h < 0) {
			REQUEST_H(c) = MIN_H(c);
			center_y += MIN_H(c);
			rem_h += CURRENT_H(c) - MIN_H(c);
		} else if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_FILL) {
			REQUEST_H(c) += rem_h / fillers;
			center_y += REQUEST_H(c);
			rem_h -= rem_h / fillers;
			fillers--;
		} else
			center_y += CURRENT_H(c);

		ewl_widget_configure(c);
	}

	if (cl) {
		ewd_list_clear(cl);
		ewd_list_destroy(cl);
	}

	DLEAVE_FUNCTION;
}
