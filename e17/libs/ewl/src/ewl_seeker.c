#include <Ewl.h>

static void __ewl_seeker_init (Ewl_Seeker * s, Ewl_Orientation orientation);
static void __ewl_seeker_realize (Ewl_Widget * w, void *event_data,
				  void *user_data);
static void __ewl_seeker_configure (Ewl_Widget * w, void *event_data,
				    void *user_data);
static void __ewl_seeker_theme_update (Ewl_Widget * w, void *event_data,
				       void *user_data);
static void __ewl_seeker_drag_button_mouse_down (Ewl_Widget * w,
						 void *event_data,
						 void *user_data);
static void __ewl_seeker_drag_button_mouse_up (Ewl_Widget * w,
					       void *event_data,
					       void *user_data);
static void __ewl_seeker_drag_button_mouse_move (Ewl_Widget * w,
						 void *event_data,
						 void *user_data);
static void __ewl_seeker_mouse_down (Ewl_Widget * w, void *event_data,
				     void *user_data);
static void __ewl_seeker_focus_in (Ewl_Widget * w, void *event_data,
				   void *user_data);
static void __ewl_seeker_focus_out (Ewl_Widget * w, void *event_data,
				    void *user_data);
static void __ewl_seeker_mouse_move (Ewl_Widget * w, void *event_data,
				     void *user_data);
static void __ewl_seeker_update_drag_button (Ewl_Widget * w, int in_x,
					     int in_y);

/*
 * Create a new seeker object with the specified orientation
 */
Ewl_Widget *
ewl_seeker_new (Ewl_Orientation orientation)
{
	Ewl_Seeker *s;

	DENTER_FUNCTION;

	s = NEW (Ewl_Seeker, 1);
	if (!s)
		DRETURN_PTR (NULL);

	memset (s, 0, sizeof (Ewl_Seeker));
	__ewl_seeker_init (s, orientation);

	DRETURN_PTR (EWL_WIDGET (s));
}

/*
 * Set the value and position of the pointer on the seekers drag_button
 */
void
ewl_seeker_set_value (Ewl_Widget * w, double v)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	if (v > EWL_SEEKER (w)->range)
		v = EWL_SEEKER (w)->range;

	EWL_SEEKER (w)->value = v;

	ewl_widget_configure (w);

	DLEAVE_FUNCTION;
}

/*
 * Retrieve the current value of the seeker widget
 */
double
ewl_seeker_get_value (Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET ("w", w, -1);

	DLEAVE_FUNCTION;
	DRETURN_FLOAT (EWL_SEEKER (w)->value);
}

/*
 * Specify the range of the values represented by the seeker
 */
void
ewl_seeker_set_range (Ewl_Widget * w, double r)
{
	int new_val;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	new_val = r * (EWL_SEEKER (w)->value / EWL_SEEKER (w)->range);

	EWL_SEEKER (w)->range = r;
	EWL_SEEKER (w)->value = new_val;

	ewl_widget_configure (w);

	DLEAVE_FUNCTION;
}

/*
 * Retrieve the range of values represented by the seeker
 */
double
ewl_seeker_get_range (Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET ("w", w, -1);

	DLEAVE_FUNCTION;
	DRETURN_FLOAT (EWL_SEEKER (w)->range);
}

/*
 * Initialize the seeker to some sane starting values
 */
static void
__ewl_seeker_init (Ewl_Seeker * s, Ewl_Orientation orientation)
{
	Ewl_Widget *w;
	Ewl_Widget *drag_button;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("s", s);

	w = EWL_WIDGET (s);

	drag_button = ewl_button_new (NULL);

	/*
	 * Initialize the widget fields and set appropriate orientation and
	 * type
	 */
	if (orientation == EWL_ORIENTATION_HORIZONTAL)
	  {

		  ewl_container_init (EWL_CONTAINER (w),
				      "/appearance/seeker/horizontal");

		  /*
		   * Set up the minimum sizes for the seeker background and drag
		   * button
		   */
		  ewl_object_set_minimum_size (EWL_OBJECT (w), 100, 15);
		  ewl_object_set_maximum_size (EWL_OBJECT (w), MAX_W (w), 15);
		  ewl_object_set_minimum_size (EWL_OBJECT (drag_button), 5,
					       5);

		  /*
		   * Override the buttons default appearance
		   */
		  ewl_theme_data_set (drag_button,
				      "/appearance/button/default/clicked",
				      "/appearance/seeker/horizontal/dragbar-clicked.bits.db");
		  ewl_theme_data_set (drag_button,
				      "/appearance/button/default/hilited",
				      "/appearance/seeker/horizontal/dragbar-hilited.bits.db");
		  ewl_theme_data_set (drag_button,
				      "/appearance/button/default/base",
				      "/appearance/seeker/horizontal/dragbar-base.bits.db");
	  }
	else
	  {
		  ewl_container_init (EWL_CONTAINER (w),
				      "/appearance/seeker/vertical");

		  /*
		   * Set up the minimum sizes for the seeker background and drag
		   * button
		   */
		  ewl_object_set_minimum_size (EWL_OBJECT (w), 15, 100);
		  ewl_object_set_maximum_size (EWL_OBJECT (w), 15, MAX_H (w));
		  ewl_object_set_minimum_size (EWL_OBJECT (drag_button), 5,
					       5);

		  /*
		   * Override the buttons default appearance
		   */
		  ewl_theme_data_set (drag_button,
				      "/appearance/button/default/clicked",
				      "/appearance/seeker/vertical/dragbar-clicked.bits.db");
		  ewl_theme_data_set (drag_button,
				      "/appearance/button/default/hilited",
				      "/appearance/seeker/vertical/dragbar-hilited.bits.db");
		  ewl_theme_data_set (drag_button,
				      "/appearance/button/default/base",
				      "/appearance/seeker/vertical/dragbar-base.bits.db");
	  }

	ewl_object_set_fill_policy (EWL_OBJECT (w), EWL_FILL_POLICY_NORMAL);

	/*
	 * Set the starting orientation, range and values
	 */
	s->orientation = orientation;
	s->range = 100;
	s->value = 0;

	/*
	 * Add necessary configuration callbacks
	 */
	ewl_callback_append (w, EWL_CALLBACK_REALIZE, __ewl_seeker_realize,
			     NULL);
	ewl_callback_append (w, EWL_CALLBACK_CONFIGURE,
			     __ewl_seeker_configure, NULL);
	ewl_callback_append (w, EWL_CALLBACK_THEME_UPDATE,
			     __ewl_seeker_theme_update, NULL);
	ewl_callback_append (w, EWL_CALLBACK_MOUSE_DOWN,
			     __ewl_seeker_mouse_down, NULL);
	ewl_callback_append (w, EWL_CALLBACK_FOCUS_IN,
			     __ewl_seeker_focus_in, NULL);
	ewl_callback_append (w, EWL_CALLBACK_FOCUS_OUT,
			     __ewl_seeker_focus_out, NULL);
	ewl_callback_append (w, EWL_CALLBACK_MOUSE_MOVE,
			     __ewl_seeker_mouse_move, NULL);

	ewl_container_append_child (EWL_CONTAINER (s), drag_button);

	/*
	 * Append a callback for catching mouse movements on the drag_button and
	 * add the drag_button to the seeker
	 */
	ewl_callback_append (drag_button, EWL_CALLBACK_MOUSE_DOWN,
			     __ewl_seeker_drag_button_mouse_down, NULL);
	ewl_callback_append (drag_button, EWL_CALLBACK_MOUSE_UP,
			     __ewl_seeker_drag_button_mouse_up, NULL);
	ewl_callback_append (drag_button, EWL_CALLBACK_MOUSE_MOVE,
			     __ewl_seeker_drag_button_mouse_move, NULL);
	s->drag_button = drag_button;

	DLEAVE_FUNCTION;
}

/*
 * Draw the representation of the seeker
 */
static void
__ewl_seeker_realize (Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	ewl_widget_show (EWL_SEEKER (w)->drag_button);

	DLEAVE_FUNCTION;
}

/*
 * On a configure event we need to adjust the seeker to fit into it's new
 * coords and position as well as move the drag_button to the correct size and
 * position.
 */
static void
__ewl_seeker_configure (Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;
	int x, y, width, height;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	/*
	 * If we don't have a visible ebits object then it's pointless to
	 * continue
	 */
	if (!REALIZED (w))
		DRETURN;

	s = EWL_SEEKER (w);
	if (!s->drag_button)
		DRETURN;

	ewl_object_get_current_geometry (EWL_OBJECT (s->drag_button), &x, &y,
					 &width, &height);

	__ewl_seeker_update_drag_button (s->drag_button, x, y);
	ewl_widget_configure (EWL_WIDGET (s->drag_button));

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_mouse_move (Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Widget *drag_button;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR ("w", w);
	DCHECK_PARAM_PTR ("event_data", event_data);

	drag_button = ewd_list_goto_first (EWL_CONTAINER (w)->children);

	ewl_callback_call_with_event_data (EWL_WIDGET (drag_button),
					   EWL_CALLBACK_MOUSE_MOVE,
					   event_data);
	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_drag_button_mouse_down (Ewl_Widget * w, void *event_data,
				     void *user_data)
{
	Ev_Mouse_Down *ev;
	Ewl_Seeker *s;
	int tmp;
	int x, y, width, height;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);
	DCHECK_PARAM_PTR ("event_data", event_data);

	ev = event_data;

	s = EWL_SEEKER (w->parent);

	ewl_object_get_current_geometry (EWL_OBJECT (w), &x, &y, &width,
					 &height);

	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		tmp = (x + width) - ev->x;
	else
		tmp = (y + height) - ev->y;

	s->dragstart = tmp;

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_drag_button_mouse_up (Ewl_Widget * w, void *event_data,
				   void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	EWL_SEEKER (w->parent)->dragstart = 0;

	DLEAVE_FUNCTION;
}

/*
 * Move the cursor to the correct position
 */
static void
__ewl_seeker_drag_button_mouse_move (Ewl_Widget * w, void *event_data,
				     void *user_data)
{
	Ev_Mouse_Move *ev;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR ("w", w);
	DCHECK_PARAM_PTR ("event_data", event_data);

	/*
	 * If the pointer is not pressed we don't care about mouse movements.
	 */
	if (!(w->state & EWL_STATE_PRESSED))
		DRETURN;

	if (EWL_SEEKER (w->parent)->dragstart < 1)
		DRETURN;

	ev = event_data;

	__ewl_seeker_update_drag_button (w, ev->x, ev->y);
	ewl_widget_configure (w);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_update_drag_button (Ewl_Widget * w, int in_x, int in_y)
{
	Ewl_Seeker *s;
	double val, old_val;
	int x, y, width, height;
	int req_x, req_y;
	int l, r, t, b;

	s = EWL_SEEKER (w->parent);

	ewl_object_get_current_geometry (EWL_OBJECT (s), &x, &y, &width,
					 &height);

	if (EWL_WIDGET (s)->ebits_object)
		ebits_get_insets (EWL_WIDGET (s)->ebits_object, &l, &r, &t,
				  &b);

	/*
	 * Adjust the width and height to fit within the insets of the bit
	 */
	x += l;
	y += t;
	width = width - (l + r);
	height = height - (t + b);

	/*
	 * The direction of the drag_button move depends on the orientation of the
	 * parent seeker.
	 */
	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
	  {

		  /*
		   * Use the current x value along with the starting drag
		   * position to determine the new position
		   */
		  req_x = in_x - s->dragstart;

		  /*
		   * Check the boundaries to make sure we're not off the end of
		   * the bar
		   */
		  if (req_x < x)
			  req_x = x;

		  if (req_x + height > x + width)
			  req_x = x + width - height;

		  /*
		   * Calculate the new value for the seeker
		   */
		  val = (double) (req_x - x) / (double) (width);

		  /*
		   * Adjust the values for assigning geometry to the object
		   */
		  x = req_x;
		  width = height;

	  }
	else
	  {

		  /*
		   * Use the current y value along with the starting drag
		   * position to determine the new position
		   */
		  req_y = in_y - s->dragstart;

		  /*
		   * Check the boundaries to make sure we're not off the end of
		   * the bar
		   */
		  if (req_y < y)
			  req_y = y;

		  if (req_y + width > y + height)
			  req_y = y + height - width;

		  /*
		   * Calculate the new value for the seeker
		   */
		  val = (double) (req_y - y) / (double) (height);

		  /*
		   * Adjust the values for assigning geometry to the object
		   */
		  y = req_y;
		  height = width;
	  }

	/*
	 * Calculate the new value and determine if the VALUE_CHANGED callback
	 * needs to be triggered
	 */
	old_val = s->value;
	s->value = val * s->range;

	if (s->value > s->range)
		s->value = s->range;

	if (old_val != s->value)
		ewl_callback_call (EWL_WIDGET (s),
				   EWL_CALLBACK_VALUE_CHANGED);

	ewl_object_request_geometry (EWL_OBJECT (w), x, y, width, height);
}

/*
 * Catch a mouse down event and act on it accordingly
 */
static void
__ewl_seeker_mouse_down (Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;
	Ev_Mouse_Move *ev;
	int x, y, width, height;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR ("w", w);
	DCHECK_PARAM_PTR ("event_data", event_data);

	ev = event_data;

	s = EWL_SEEKER (w);
	if (!s)
		DRETURN;

	ewl_object_get_current_geometry (EWL_OBJECT (s->drag_button), &x, &y,
					 &width, &height);

	ewl_widget_configure (w);

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_focus_in (Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	s = EWL_SEEKER (w);

	if (!s->drag_button)
		DRETURN;

	if (!(s->drag_button->state & EWL_STATE_PRESSED))
		ewl_widget_update_appearance (s->drag_button, "hiliited");

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_focus_out (Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Seeker *s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	s = EWL_SEEKER (w);

	if (!s->drag_button)
		DRETURN;

	if (!(s->drag_button->state & EWL_STATE_PRESSED))
		ewl_widget_update_appearance (s->drag_button, "normal");

	DLEAVE_FUNCTION;
}

static void
__ewl_seeker_theme_update (Ewl_Widget * w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	if (EWL_SEEKER (w)->drag_button)
		ewl_widget_theme_update (EWL_SEEKER (w)->drag_button);

	DLEAVE_FUNCTION;
}
