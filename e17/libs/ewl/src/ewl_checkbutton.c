
#include <Ewl.h>

/*\
|*|
|*| The button widget is really a merge of many diffrent button types.
|*| Normal - You but whatever widget you want in here
|*| Label - You specify label for button..
|*|
\*/

static void __ewl_checkbutton_init(Ewl_CheckButton * b, const char *l);
static void __ewl_checkbutton_realize(Ewl_Widget * w, void *ev_data,
				      void *user_data);
static void __ewl_checkbutton_show(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static void __ewl_checkbutton_hide(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static void __ewl_checkbutton_destroy(Ewl_Widget * w, void *ev_data,
				      void *user_data);
static void __ewl_checkbutton_destroy_recursive(Ewl_Widget * w,
						void *ev_data,
						void *user_data);
static void __ewl_checkbutton_configure(Ewl_Widget * w, void *ev_data,
					void *user_data);
static void __ewl_checkbutton_focus_in(Ewl_Widget * w, void *ev_data,
				       void *user_data);
static void __ewl_checkbutton_focus_out(Ewl_Widget * w, void *ev_data,
					void *user_data);
static void __ewl_checkbutton_mouse_down(Ewl_Widget * w, void *ev_data,
					 void *user_data);
static void __ewl_checkbutton_mouse_up(Ewl_Widget * w, void *ev_data,
				       void *user_data);
static void __ewl_checkbutton_theme_update(Ewl_Widget * w, void *ev_data,
					   void *user_data);


Ewl_Widget *
ewl_checkbutton_new(const char *l)
{
	Ewl_CheckButton *b;

	DENTER_FUNCTION;

	b = NEW(Ewl_CheckButton, 1);

	__ewl_checkbutton_init(b, l);

	DRETURN_PTR(EWL_WIDGET(b));
}

static void
__ewl_checkbutton_init(Ewl_CheckButton * b, const char *l)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;

	/*
	 * Blank out the structure and initialize it's theme
	 */
	memset(b, 0, sizeof(Ewl_CheckButton));
	ewl_container_init(EWL_CONTAINER(b), EWL_WIDGET_BUTTON, 16, 16, 16,
			   16);

	w = EWL_WIDGET(b);

	/*
	 * Override the default recursive setting on containers. This prevents
	 * the coordinate->object mapping from searching below the button
	 * class.
	 */
	w->recursive = FALSE;

	/*
	 * Add the label if desired
	 */
	if (l)
		EWL_BUTTON(b)->label = strdup(l);

	/*
	 * Attach necessary callback mechanisms
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_checkbutton_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_SHOW,
			    __ewl_checkbutton_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE,
			    __ewl_checkbutton_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY,
			    __ewl_checkbutton_destroy, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_checkbutton_destroy_recursive, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_checkbutton_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_checkbutton_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
			    __ewl_checkbutton_mouse_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			    __ewl_checkbutton_focus_in, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_checkbutton_focus_out, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_checkbutton_theme_update, NULL);

	DLEAVE_FUNCTION;
}

/*
 * Change the state of the specified button
 */
void
ewl_checkbutton_set_checked(Ewl_Widget * w, int c)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	cb->checked = c;

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}


static void
__ewl_checkbutton_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *b;
	char *l = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_CHECKBUTTON(w);

	ewl_fx_clip_box_create(w);

	if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
		evas_set_clip(w->evas, w->fx_clip_box,
			      EWL_CONTAINER(w->parent)->clip_box);


	if (EWL_BUTTON(w)->label)
		l = strdup(EWL_BUTTON(w)->label);

	ewl_button_set_label(w, l);

	IF_FREE(l);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->children &&
	    !ewd_list_is_empty(EWL_CONTAINER(w)->children))
	  {
		  Ewl_Widget *c;

		  ewd_list_goto_first(EWL_CONTAINER(w)->children);

		  while ((c = ewd_list_next(EWL_CONTAINER(w)->children))
			 != NULL)
			  ewl_widget_show(c);
	  }

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (w->ebits_object)
	  {
		  ebits_hide(w->ebits_object);
		  ebits_unset_clip(w->ebits_object);
		  ebits_free(w->ebits_object);
	  }

	if (w->fx_clip_box)
	  {
		  evas_hide(w->evas, w->fx_clip_box);
		  evas_unset_clip(w->evas, w->fx_clip_box);
		  evas_del_object(w->evas, w->fx_clip_box);
	  }

	ewl_callback_clear(w);

	ewl_theme_deinit_widget(w);

	FREE(cb);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_destroy_recursive(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	Ewl_Widget *c;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->children)
		while ((c =
			ewd_list_remove_last(EWL_CONTAINER(w)->children))
		       != NULL)
			ewl_widget_destroy_recursive(c);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;
	Ewl_CheckButton *cb;
	int req_x, req_y, req_w, req_h;
	int cur_w, cur_h;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);
	cb = EWL_CHECKBUTTON(w);

	ewl_object_requested_geometry(EWL_OBJECT(w), &req_x, &req_y,
				      &req_w, &req_h);

	if (w->ebits_object)
	  {
		  ebits_move(w->ebits_object, req_x, req_y);
		  ebits_resize(w->ebits_object, req_w, req_h);
	  }

	if (w->fx_clip_box)
	  {
		  evas_move(w->evas, w->fx_clip_box, req_x, req_y);
		  evas_resize(w->evas, w->fx_clip_box, req_w, req_h);
	  }

	if (EWL_CONTAINER(w)->clip_box)
	  {
		  evas_move(w->evas, EWL_CONTAINER(w)->clip_box, req_x,
			    req_y);
		  evas_resize(w->evas, EWL_CONTAINER(w)->clip_box, req_w,
			      req_h);
	  }

	if (b->label_object)
	  {
		  ewl_object_get_current_size(EWL_OBJECT(b->label_object),
					      &cur_w, &cur_h);

		  ewl_object_request_geometry(EWL_OBJECT(b->label_object),
					      req_x + 19,
					      req_y + ((req_h / 2) -
						       (cur_h / 2)), cur_w,
					      cur_h);

		  ewl_widget_configure(b->label_object);

		  ewl_object_get_current_size(EWL_OBJECT(b->label_object),
					      &cur_w, &cur_h);
	  }

	ewl_object_set_current_geometry(EWL_OBJECT(w), req_x, req_y,
					19 + cur_w, req_h);
	ewl_object_set_minimum_size(EWL_OBJECT(w), 19 + cur_w, req_h);
	ewl_object_set_maximum_size(EWL_OBJECT(w), 19 + cur_w, req_h);


	DLEAVE_FUNCTION;
}


static void
__ewl_checkbutton_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_CHECKBUTTON(w);

	w->state |= EWL_STATE_PRESSED;

	b->checked = b->checked ? 0 : 1;

	ewl_widget_theme_update(w);

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_CHECKBUTTON(w);

	w->state &= ~EWL_STATE_PRESSED;

	ewl_widget_theme_update(w);

	if (w->state & EWL_STATE_HILITED)
		ewl_callback_call(w, EWL_CALLBACK_CLICKED);

	DLEAVE_FUNCTION;
}

static void
__ewl_checkbutton_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *b;
	char *v;
	char str[512];
	char state[512];

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	/*
	 * Don't want to update anything if the widget isn't realized. 
	 */
	if (!w->object.realized)
		DRETURN;

	b = EWL_CHECKBUTTON(w);

	if (w->state & EWL_STATE_PRESSED)
		snprintf(state, 512, "clicked");
	else if (w->state & EWL_STATE_HILITED)
		snprintf(state, 512, "hilited");
	else
		snprintf(state, 512, "base");


	snprintf(str, 512, "/appearance/button/check/%s-checked%i/visible",
		 state, b->checked);

	v = ewl_theme_data_get(w, str);

	if (v && !strncasecmp(v, "yes", 3))
	  {
		  char *i;

		  snprintf(str, 512, "/appearance/button/check/%s-checked%i",
			   state, b->checked);

		  i = ewl_theme_image_get(w, str);

		  if (i)
		    {
			    w->ebits_object = ebits_load(i);
			    FREE(i);

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
	 * Finally comfigure the widget to update the changes 
	 */
	ewl_widget_configure(w);
}
