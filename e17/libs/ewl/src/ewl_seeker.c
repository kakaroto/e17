#include <Ewl.h>

static void __ewl_seeker_init(Ewl_Seeker * s, Ewl_Orientation orientation);
static void __ewl_seeker_realize(Ewl_Widget * w, void *event_data,
				 void *user_data);
static void __ewl_seeker_show(Ewl_Widget * w, void *event_data,
			      void *user_data);
static void __ewl_seeker_hide(Ewl_Widget * w, void *event_data,
			      void *user_data);
static void __ewl_seeker_destroy(Ewl_Widget * w, void *event_data,
				 void *user_data);
static void __ewl_seeker_destroy_recursive(Ewl_Widget * w, void *event_data,
					   void *user_data);
static void __ewl_seeker_configure(Ewl_Widget * w, void *event_data,
				   void *user_data);
static void __ewl_seeker_theme_update(Ewl_Widget * w, void *event_data,
				      void *user_data);
static void __ewl_seeker_dragbar_mouse_move(Ewl_Widget * w, void *event_data,
					    void *user_data);
static void __ewl_seeker_mouse_up(Ewl_Widget * w, void *event_data,
				  void *user_data);
static void __ewl_seeker_mouse_down(Ewl_Widget * w, void *event_data,
				    void *user_data);
static void __ewl_seeker_focus_in(Ewl_Widget * w, void *event_data,
				  void *user_data);
static void __ewl_seeker_focus_out(Ewl_Widget * w, void *event_data,
				   void *user_data);
static void __ewl_seeker_mouse_move(Ewl_Widget * w, void *event_data,
				    void *user_data);

/*
 * Create a new seeker object with the specified orientation
 */
Ewl_Widget *
ewl_seeker_new(Ewl_Orientation orientation)
{
	Ewl_Seeker *s;

	DENTER_FUNCTION;

	s = NEW(Ewl_Seeker, 1);

	__ewl_seeker_init(s, orientation);

	DRETURN_PTR(EWL_WIDGET(s));
}

/*
 * Set the value and position of the pointer on the seekers dragbar
 */
void
ewl_seeker_set_value(Ewl_Widget * w, double v)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	EWL_SEEKER(w)->value = v;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

/*
 * Retrieve the current value of the seeker widget
 */
double
ewl_seeker_get_value(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, -1);

	DRETURN_FLOAT(EWL_SEEKER(w)->value);
}

/*
 * Specify the range of the values represented by the seeker
 */
void
ewl_seeker_set_range(Ewl_Widget * w, double r)
{
	int new_val;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	new_val = r * (EWL_SEEKER(w)->value / EWL_SEEKER(w)->range);

	EWL_SEEKER(w)->range = r;
	EWL_SEEKER(w)->value = new_val;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

/*
 * Retrieve the range of values represented by the seeker
 */
double
ewl_seeker_get_range(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, -1);

	DRETURN_FLOAT(EWL_SEEKER(w)->range);
}

/*
 * Initialize the seeker to some sane starting values
 */
static void
__ewl_seeker_init(Ewl_Seeker * s, Ewl_Orientation orientation)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("s", s);


	w = EWL_WIDGET(s);

	/*
	 * Initialize the widget fields and set appropriate orienation and
	 * type
	 */
	memset(s, 0, sizeof(Ewl_Seeker));
	ewl_container_init(EWL_CONTAINER(w), EWL_WIDGET_SEEKER, 100, 15,
			   512, 15);

	s->orientation = orientation;

	/*
	 * Add necessary configuration callbacks
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_seeker_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_SHOW, __ewl_seeker_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, __ewl_seeker_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY,
			    __ewl_seeker_destroy, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_seeker_destroy_recursive, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_seeker_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_seeker_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_seeker_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
			    __ewl_seeker_mouse_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			    __ewl_seeker_focus_in, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_seeker_focus_out, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
			    __ewl_seeker_mouse_move, NULL);

	/*
	 * Set the starting range and values
	 */
	s->range = 100;
	s->value = 0;

	DLEAVE_FUNCTION;
}

/*
 * Draw the representation of the seeker
 */
static void
__ewl_seeker_realize(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;
	Ewl_Widget *dragbar;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEEKER(w);

	ewl_fx_clip_box_create(w);

	/*
	 * We have a parent and a clip_box? ok, then we should tell evas to
	 * use them
	 */
	if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
		evas_set_clip(w->evas, w->fx_clip_box,
			      EWL_CONTAINER(w->parent)->clip_box);

	/*
	 * Create a button that will act as the dragbar and append it to the
	 * seeker.
	 */
	dragbar = ewl_button_new(NULL);

	/*
	 * Override the default theme for buttons
	 */
	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		ewl_theme_data_set(dragbar, "/appearance/button/default/base",
				   "/appearance/seeker/horizontal/dragbar.bits.db");
	else
		ewl_theme_data_set(dragbar, "/appearance/button/default/base",
				   "/appearance/seeker/vertical/dragbar.bits.db");

	/*
	 * Append a callback for catching mouse movements on the dragbar and
	 * add the dragbar to the seeker
	 */
	ewl_callback_append(dragbar, EWL_CALLBACK_MOUSE_MOVE,
			    __ewl_seeker_dragbar_mouse_move, NULL);
	ewl_container_append_child(EWL_CONTAINER(s), dragbar);
	ewl_widget_show(EWL_WIDGET(dragbar));

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_show(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_hide(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_destroy(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_del_all(w);

	ewl_theme_deinit_widget(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_destroy_recursive(Ewl_Widget * w, void *event_data,
			       void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

/*
 * On a configure event we need to adjust the seeker to fit into it's new
 * coords and position as well as move the dragbar to the correct size and
 * position.
 */
static void
__ewl_seeker_configure(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;
	Ewl_Widget *dragbar;
	double val;
	int new_x, new_y, new_w, new_h;
	int req_x, req_y, req_w, req_h;
	int l, r, t, b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * If we don't have a visible ebits object then it's pointless to
	 * continue
	 */
	if (!w->ebits_object)
		DRETURN;

	/*
	 * Retrieve the new geometry of the widget
	 */
	ewl_object_requested_geometry(EWL_OBJECT(w), &req_x, &req_y,
				      &req_w, &req_h);

	s = EWL_SEEKER(w);

	/*
	 * Move the base ebits object to the correct size and position
	 */
	ebits_move(w->ebits_object, req_x, req_y);
	ebits_resize(w->ebits_object, req_w, req_h);

	/*
	 * Now move the clipbox to the correct size and position
	 */
	evas_move(w->evas, w->fx_clip_box, req_x, req_y);
	evas_resize(w->evas, w->fx_clip_box, req_w, req_h);

	ewl_object_set_current_geometry(EWL_OBJECT(w),
					req_x, req_y, req_w, req_h);

	/*
	 * Get the dragbar's current size
	 */
	ewd_list_goto_first(EWL_CONTAINER(s)->children);
	dragbar = ewd_list_current(EWL_CONTAINER(s)->children);
	ewl_object_get_current_geometry(EWL_OBJECT(dragbar), &new_x, &new_y,
					&new_w, &new_h);

	/*
	 * Calculate the dragbar's new position
	 */
	ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	val = s->value / s->range;

	new_x = req_x + l;
	new_y = req_y + t;
	new_h = req_h - (t + b);

	new_x += ((req_w - (l + r)) - new_w) * val;

	/*
	 * Now apply the new position to the dragbar
	 */
	ewl_object_request_geometry(EWL_OBJECT(dragbar), new_x, new_y,
				    new_w, new_h);
	ewl_widget_configure(dragbar);

	DLEAVE_FUNCTION;
}

/*
 * The theme for the seeker has been updated, so reread any theme info we need
 */
static void
__ewl_seeker_theme_update(Ewl_Widget * w, void *event_data, void *user_data)
{
	char *i = NULL;
	Ewl_Seeker *s;
	Ewl_Widget *dragbar;
	char *v;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Shouldn't do anything if the widget isn't realized yet 
	 */
	if (!w->object.realized)
		DRETURN;

	s = EWL_SEEKER(w);

	/*
	 * Check if GFX should be visible or not 
	 */
	if (s->orientation == EWL_ORIENTATION_VERTICAL)
		v = ewl_theme_data_get(w,
				       "/appearance/seeker/vertical/base/visible");
	else
		v = ewl_theme_data_get(w,
				       "/appearance/seeker/horizontal/base/visible");

	/*
	 * Check if this piece should be visible, if so grab the path to it's
	 * bit file.
	 */
	if (v && *v == 'y')
	  {
		  if (s->orientation == EWL_ORIENTATION_VERTICAL)
			  i = ewl_theme_image_get(w,
						  "/appearance/seeker/horizontal/base");
		  else
			  i = ewl_theme_image_get(w,
						  "/appearance/seeker/horizontal/base");
		  FREE(v);
	  }

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
	 * If there is a base image to place, load it
	 */
	if (i)
	  {
		  w->ebits_object = ebits_load(i);
		  FREE(i);

		  if (w->ebits_object)
		    {
			    ebits_add_to_evas(w->ebits_object, w->evas);
			    ebits_set_layer(w->ebits_object,
					    EWL_OBJECT(w)->layer);
			    ebits_set_clip(w->ebits_object, w->fx_clip_box);

			    ebits_show(w->ebits_object);
		    }
	  }

	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	dragbar = ewd_list_current(EWL_CONTAINER(w)->children);

	ewl_object_set_layer(EWL_OBJECT(dragbar), EWL_OBJECT(w)->layer + 1);
	ewl_widget_theme_update(dragbar);

	/*
	 * Finally configure the widget to update changes 
	 */
	ewl_widget_configure(w);
}

static void
__ewl_seeker_mouse_move(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Widget *dragbar;

	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("event_data", event_data);

	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	dragbar = ewd_list_current(EWL_CONTAINER(w)->children);

	ewl_callback_call_with_event_data(EWL_WIDGET(dragbar),
		EWL_CALLBACK_MOUSE_MOVE, event_data);
}

/*
 * Move the cursor to the correct position
 */
static void
__ewl_seeker_dragbar_mouse_move(Ewl_Widget * w, void *event_data,
				void *user_data)
{
	Ewl_Widget *parent;
	Ev_Mouse_Move *ev;

	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("event_data", event_data);

	ev = event_data;

	/*
	 * If the pointer is not pressed we don't care about mouse movements.
	 */
	if (!(w->state & EWL_STATE_PRESSED))
		DRETURN;

	parent = EWL_WIDGET(w)->parent;

	/*
	 * The direction of the dragbar move depends on the orientation of the
	 * parent seeker.
	 */
	if (EWL_SEEKER(parent)->orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		if (user_data != NULL)
		  {
			printf("%i\n", user_data);
		  }
	  }
	else
	  {

	  }

	ewl_widget_theme_update(parent);

	DLEAVE_FUNCTION;
}

/*
 * Catch a mouse down event and act on it accordingly
 */
static void
__ewl_seeker_mouse_down(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;
	Ewl_Widget *dragbar;

	Ev_Mouse_Move *ev = (Ev_Mouse_Move *) event_data;
	int x, y, width, height;
	int tmp;

	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("event_data", event_data);

	w->state |= EWL_STATE_PRESSED;
	s = EWL_SEEKER(w);

	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	dragbar = ewd_list_current(EWL_CONTAINER(w)->children);
	ewl_object_get_current_geometry(EWL_OBJECT(dragbar), &x, &y,
					&width, &height);

	/*
	 * A click occurred on the dragbar, so we need to increment
	 * the value by the jump amount.
	 */
	if (EWL_SEEKER(w)->orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		  if (ev->x > x + width)
		    {
	  		tmp = ev->x - (x + width);
			ewl_callback_set_user_data(dragbar,
				EWL_CALLBACK_MOUSE_MOVE,
				__ewl_seeker_dragbar_mouse_move,
				(void *) tmp);
		    }
	  }
	else
	  {
                  if (ev->y > y + height)
                    {
                        tmp = ev->y - (y + height);
                        ewl_callback_set_user_data(dragbar, 
                                EWL_CALLBACK_MOUSE_MOVE,
                                __ewl_seeker_dragbar_mouse_move, 
                                (void *) tmp);
                    }
	  }

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_mouse_up(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Widget * dragbar;

	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("event_data", event_data);

	w->state &= ~EWL_STATE_PRESSED;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	dragbar = ewd_list_current(EWL_CONTAINER(w)->children);

	ewl_callback_set_user_data(dragbar, 
				   EWL_CALLBACK_MOUSE_MOVE,
				   __ewl_seeker_dragbar_mouse_move, 
				   NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_focus_in(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_focus_out(Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}
