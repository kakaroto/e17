
#include <Ewl.h>

/*\
|*|
|*| The button widget is really a merge of many diffrent button types.
|*| Normal - You but whatever widget you want in here
|*| Label - You specify label for button..
|*|
\*/

static void __ewl_button_init(Ewl_Button * b, char *label);
static void __ewl_button_realize(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_button_show(Ewl_Widget * w, void *ev_data,
			      void *user_data);
static void __ewl_button_hide(Ewl_Widget * w, void *ev_data,
			      void *user_data);
static void __ewl_button_destroy(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_button_destroy_recursive(Ewl_Widget * w, void *ev_data,
					   void *user_data);
static void __ewl_button_configure(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static void __ewl_button_focus_in(Ewl_Widget * w, void *ev_data,
				  void *user_data);
static void __ewl_button_focus_out(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static void __ewl_button_mouse_down(Ewl_Widget * w, void *ev_data,
				    void *user_data);
static void __ewl_button_mouse_up(Ewl_Widget * w, void *ev_data,
				  void *user_data);
static void __ewl_button_theme_update(Ewl_Widget * w, void *ev_data,
				      void *user_data);

static void __ewl_button_update_label(Ewl_Widget * w);
static void __ewl_button_remove_label(Ewl_Widget * w);


Ewl_Widget *
ewl_button_new(char *l)
{
	Ewl_Button *b;

	DENTER_FUNCTION;

	b = NEW(Ewl_Button, 1);

	__ewl_button_init(b, l);

	if (b->label)
	  {
		b->label_object = ewl_text_new();
		ewl_text_set_text(b->label_object, b->label);
	  }

	DRETURN_PTR(EWL_WIDGET(b));
}

static void
__ewl_button_init(Ewl_Button * b, char *label)
{
	DENTER_FUNCTION;

	/*
	 * Blank out the structure and initialize it's theme
	 */
	memset(b, 0, sizeof(Ewl_Button));
	ewl_container_init(EWL_CONTAINER(b), 80, 20,
			   EWL_FILL_POLICY_NORMAL, EWL_ALIGNMENT_CENTER);

	/*
	 * Override the default recursive setting on containers. This prevents
	 * the coordinate->object mapping from searching below the button
	 * class.
	 */
	EWL_WIDGET(b)->recursive = FALSE;

	/*
	 * Add the label if desired
	 */
	if (label)
		b->label = strdup(label);

	/*
	 * Attach necessary callback mechanisms
	 */
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_REALIZE,
			    __ewl_button_realize, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_SHOW,
			    __ewl_button_show, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_HIDE,
			    __ewl_button_hide, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_DESTROY,
			    __ewl_button_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_button_destroy_recursive, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
			    __ewl_button_configure, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_button_mouse_down, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_MOUSE_UP,
			    __ewl_button_mouse_up, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_FOCUS_IN,
			    __ewl_button_focus_in, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_FOCUS_OUT,
			    __ewl_button_focus_out, NULL);
	ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_THEME_UPDATE,
			    __ewl_button_theme_update, NULL);

	DLEAVE_FUNCTION;
}

/*
 * Change the label of the specified button
 */
void
ewl_button_set_label(Ewl_Widget * w, const char *l)
{
	Ewl_Button *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!l)
		__ewl_button_remove_label(w);
	else {
		b = EWL_BUTTON(w);

		IF_FREE(b->label);

		b->label = strdup(l);

		__ewl_button_update_label(w);
	}

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}


static void
__ewl_button_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;
	Evas_Object *clip_box;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	w->fx_clip_box = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, w->fx_clip_box, 255, 255, 255, 255);
	evas_set_layer(w->evas, w->fx_clip_box, LAYER(w) - 1);
	if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
		evas_set_clip(w->evas, w->fx_clip_box,
			      EWL_CONTAINER(w->parent)->clip_box);

	clip_box = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, clip_box, 255, 255, 255, 255);
	evas_set_layer(w->evas, clip_box, LAYER(w));
	evas_set_clip(w->evas, clip_box, w->fx_clip_box);

	EWL_CONTAINER(w)->clip_box = clip_box;

	if (b->label_object)
	  {
		void * tmp;

		b->label_object->evas = w->evas;
		b->label_object->evas_window = w->evas_window;
		b->label_object->parent = w;

		tmp = ewl_theme_data_get(w, "/appearance/button/default/text/font");
		if (tmp)
			ewl_text_set_font(b->label_object, (char *) tmp);

		tmp = ewl_theme_data_get(w, "/apperance/button/default/text/style");
		if (tmp)
			ewl_text_set_style(b->label_object, tmp);

		tmp = ewl_theme_data_get(b->label_object, "/appearance/button/default/text/font_size");


		if (tmp)
			ewl_text_set_font_size(b->label_object, (int) (tmp));

		ewl_widget_realize(b->label_object);

		evas_show(w->evas, EWL_CONTAINER(w)->clip_box);
	  }

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	if (b->label_object)
		ewl_widget_show(b->label_object);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	if (w->ebits_object) {
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

	IF_FREE(b->label);

	if (b->label_object)
	  {
		if (ewd_list_goto(EWL_CONTAINER(w)->children, b->label_object))
			ewd_list_remove(EWL_CONTAINER(w)->children);

		ewl_widget_destroy(b->label_object);

		b->label_object = NULL;
	  }

	ewl_callback_clear(w);

	ewl_theme_deinit_widget(w);

	FREE(b);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_destroy_recursive(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *c;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (EWL_CONTAINER(w)->children)
		while ((c = ewd_list_remove_last(EWL_CONTAINER(w)->children))
		       != NULL)
			ewl_widget_destroy_recursive(c);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;
	int req_x, req_y, req_w, req_h;
	int l = 0, r = 0, t = 0, bb = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	/*
	 * Retrieve the child object and make sure the button's minimum size
	 * is not smaller than the child
	 */
	if (b->label_object) {
		int cmin_w, cmin_h;
		int pmin_w, pmin_h;

		ewl_object_get_minimum_size(EWL_OBJECT(b->label_object),
				&cmin_w, &cmin_h);
		ewl_object_get_minimum_size(EWL_OBJECT(b->label_object),
				&pmin_w, &pmin_h);

		if (cmin_w > pmin_w)
			pmin_w = cmin_w;
		if (cmin_h > pmin_h)
			pmin_h = cmin_h;

		ewl_object_set_minimum_size(EWL_OBJECT(w), pmin_w, pmin_h);
	}

	ewl_object_apply_requested(EWL_OBJECT(w));

	ewl_object_requested_geometry(EWL_OBJECT(w), &req_x, &req_y,
				      &req_w, &req_h);

	if (w->ebits_object) {
		ebits_get_insets(w->ebits_object, &l, &r, &t, &bb);
		ebits_move(w->ebits_object, req_x, req_y);
		ebits_resize(w->ebits_object, req_w, req_h);
	}

	if (w->fx_clip_box) {
		evas_move(w->evas, w->fx_clip_box, req_x, req_y);
		evas_resize(w->evas, w->fx_clip_box, req_w, req_h);
	}

	if (EWL_CONTAINER(w)->clip_box) {
		evas_move(w->evas, EWL_CONTAINER(w)->clip_box, req_x + l,
			  req_y + t);
		evas_resize(w->evas, EWL_CONTAINER(w)->clip_box, req_w - l - r,
			    req_h - t - bb);
	}

	if (b->label_object)
	  {
		REQUEST_X(b->label_object) = CURRENT_X(w) + l;
		REQUEST_Y(b->label_object) = CURRENT_Y(w) + t;
		REQUEST_W(b->label_object) = CURRENT_W(w) - l - r;
		REQUEST_H(b->label_object) = CURRENT_H(w) - t - bb;

		ewl_widget_configure(b->label_object);
	  }

	DLEAVE_FUNCTION;
}


static void
__ewl_button_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	w->state |= EWL_STATE_PRESSED;

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	w->state &= ~EWL_STATE_PRESSED;

	ewl_widget_theme_update(w);

	if (w->state & EWL_STATE_HILITED)
		ewl_callback_call(w, EWL_CALLBACK_CLICKED);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;
	char *v;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Don't want to update anything if the widget isn't realized. 
	 */
	if (!REALIZED(w))
		DRETURN;

	b = EWL_BUTTON(w);

	if (w->state & EWL_STATE_PRESSED)
		v = ewl_theme_data_get(w,
				       "/appearance/button/default/clicked/visible");
	else if (w->state & EWL_STATE_HILITED)
		v = ewl_theme_data_get(w,
				       "/appearance/button/default/hilited/visible");
	else
		v = ewl_theme_data_get(w,
				       "/appearance/button/default/base/visible");

	if (w->ebits_object)
	  {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	  }

	if (v && !strncasecmp(v, "yes", 3)) {
		char *i;

		if (w->state & EWL_STATE_PRESSED)
			i = ewl_theme_image_get(w,
						"/appearance/button/default/clicked");
		else if (w->state & EWL_STATE_HILITED)
			i = ewl_theme_image_get(w,
						"/appearance/button/default/hilited");
		else
			i = ewl_theme_image_get(w,
						"/appearance/button/default/base");

		if (i) {
			w->ebits_object = ebits_load(i);

			FREE(i);

			if (w->ebits_object) {
				ebits_add_to_evas(w->ebits_object, w->evas);
				ebits_set_layer(w->ebits_object, LAYER(w));
				ebits_set_clip(w->ebits_object, w->fx_clip_box);
				ebits_show(w->ebits_object);
			}
		}
	}

	/*
	 * Finally comfigure the widget to update the changes 
	 */
	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_update_label(Ewl_Widget * w)
{
	Ewl_Button *b;
	void *tmp;
	int fs;


	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	if (!b->label)
		DRETURN;

	/*
	 * If the label object has not yet been created then do so and
	 * add it to the container button
	 */
	if (!b->label_object) {
		b->label_object = ewl_text_new();
		ewl_container_append_child(EWL_CONTAINER(w),
					   b->label_object);
	}

	if (REALIZED(w))
		ewl_widget_realize(b->label_object);

	if (VISIBLE(w))
		ewl_widget_show(b->label_object);


	/*
	 * Retrieve theme information
	 */
	tmp =
	    ewl_theme_data_get(w, "/appearance/button/default/text/font");

	if (tmp) {
		ewl_text_set_font(b->label_object, tmp);
		FREE(tmp);
	}

	tmp =
	    ewl_theme_data_get(w,
			       "/appearance/button/default/text/font_size");

	/*
	 * Apply theme info to the text
	 */
	ewl_text_set_font_size(b->label_object, (int) (tmp));
	ewl_text_set_text(b->label_object, b->label);

	DLEAVE_FUNCTION;
}

static void
__ewl_button_remove_label(Ewl_Widget * w)
{
	Ewl_Button *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	if (!b->label)
		DRETURN;

	if (b->label_object) {
		ewd_list_goto_first(EWL_CONTAINER(w)->children);
		ewd_list_remove(EWL_CONTAINER(w)->children);
		ewl_widget_destroy(b->label_object);
	}

	DLEAVE_FUNCTION;
}
