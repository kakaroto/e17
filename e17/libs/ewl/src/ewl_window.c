
#include <Ewl.h>

Ewd_List *ewl_window_list = NULL;

static void __ewl_window_init(Ewl_Window * w);
static void __ewl_window_realize(Ewl_Widget * w, void *event_data,
				 void *user_data);
static void __ewl_window_show(Ewl_Widget * w, void *event_data,
			      void *user_data);
static void __ewl_window_hide(Ewl_Widget * w, void *event_data,
			      void *user_data);
static void __ewl_window_destroy(Ewl_Widget * w, void *event_data,
				 void *user_data);
static void __ewl_window_destroy_recursive(Ewl_Widget * w, void *event_data,
					   void *user_data);
static void __ewl_window_configure(Ewl_Widget * w, void *event_data,
				   void *user_data);
static void __ewl_window_theme_update(Ewl_Widget * w, void *event_data,
				      void *user_data);

Ewl_Widget *
ewl_window_new()
{
	Ewl_Window *w;

	DENTER_FUNCTION;

	w = NEW(Ewl_Window, 1);

	__ewl_window_init(w);

	DRETURN_PTR(EWL_WIDGET(w));
}

static void
__ewl_window_init(Ewl_Window * w)
{
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Initialize the fields of the inherited container class
	 */
	memset(w, 0, sizeof(Ewl_Window));
	ewl_container_init(EWL_CONTAINER(w), EWL_WIDGET_WINDOW, 0, 0,
			   2048, 2048);

	CURRENT_W(w) = 256;
	CURRENT_H(w) = 256;
	REQUEST_W(w) = 256;
	REQUEST_H(w) = 256;

	w->title = strdup("EWL!");

	ewl_theme_init_widget(EWL_WIDGET(w));

	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			    __ewl_window_realize, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_SHOW,
			    __ewl_window_show, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_HIDE,
			    __ewl_window_hide, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			    __ewl_window_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_DESTROY_RECURSIVE,
			    __ewl_window_destroy_recursive, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			    __ewl_window_configure, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_THEME_UPDATE,
			    __ewl_window_theme_update, NULL);

	EWL_OBJECT(w)->layer = -999;

	if (!ewl_window_list)
		ewl_window_list = ewd_list_new();

	ewd_list_append(ewl_window_list, w);
}

static void
__ewl_window_realize(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Window *window;
	char *font_path;

	DCHECK_PARAM_PTR("w", w);

	window = EWL_WINDOW(w);

	window->window = e_window_new(0, 0, 0, CURRENT_W(w), CURRENT_H(w));
	e_window_set_events(window->window, XEV_CONFIGURE);
	e_window_set_name_class(window->window, "EWL", "EWL!");
	e_window_set_min_size(window->window, MIN_W(w), MIN_H(w));
	e_window_set_max_size(window->window, MAX_W(w), MAX_H(w));
	e_window_set_title(window->window, window->title);

	e_window_set_delete_inform(window->window);

	font_path = ewl_theme_font_path();

	w->evas = evas_new_all(e_display_get(),
			       window->window, 0, 0,
			       CURRENT_W(w),
			       CURRENT_H(w),
			       ewl_config_get_render_method(),
			       216, 1024 * 1024 * 2,
			       1024 * 1024 * 5, font_path);

	w->evas_window = evas_get_window(w->evas);
	e_window_set_events(w->evas_window, XEV_KEY | XEV_BUTTON |
			    XEV_IN_OUT | XEV_EXPOSE | XEV_VISIBILITY |
			    XEV_MOUSE_MOVE);

	window->bg_rect = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, window->bg_rect, 150, 255, 150, 255);
	evas_set_layer(w->evas, window->bg_rect, LAYER(w) - 1000);
	evas_show(w->evas, window->bg_rect);

	{
		Evas_Object *clip_box;

		clip_box = evas_add_rectangle(w->evas);
		evas_set_color(w->evas, clip_box, 255, 255, 255, 255);
		evas_set_layer(w->evas, clip_box, LAYER(w) - 1);
		EWL_CONTAINER(w)->clip_box = clip_box;

	}

	ewl_widget_theme_update(w);
}

static void
__ewl_window_show(Ewl_Widget * w, void *event_data, void *user_data)
{
	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, EWL_CONTAINER(w)->clip_box);
	e_window_show(EWL_WINDOW(w)->window);
	e_window_show(w->evas_window);

	if (EWL_WINDOW(w)->borderless)
		e_window_hint_set_borderless(EWL_WINDOW(w)->window);

	ewl_widget_configure(w);
}

static void
__ewl_window_hide(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DCHECK_PARAM_PTR("widget", widget);

	e_window_hide(widget->evas_window);
	e_window_hide(EWL_WINDOW(widget)->window);
}

static void
__ewl_window_destroy(Ewl_Widget * w, void *event_data, void *user_data)
{
	DCHECK_PARAM_PTR("w", w);

	IF_FREE(EWL_WINDOW(w)->title);

	if (w->ebits_object)
	  {
		  ebits_hide(w->ebits_object);
		  ebits_free(w->ebits_object);
	  }

	if (w->evas)
		evas_free(w->evas);

	e_window_hide(w->evas_window);
	e_window_hide(EWL_WINDOW(w)->window);

	e_window_destroy(w->evas_window);
	e_window_destroy(EWL_WINDOW(w)->window);

	IF_FREE(EWL_WINDOW(w)->title);

	if (ewd_list_goto(ewl_window_list, w))
		ewd_list_remove(ewl_window_list);

	ewl_callback_clear(w);

	ewl_theme_deinit_widget(w);

	FREE(w);
}

static void
__ewl_window_destroy_recursive(Ewl_Widget * widget, void *event_data,
			       void *user_data)
{
	Ewl_Widget *child;

	DCHECK_PARAM_PTR("widget", widget);

	while ((child =
		ewd_list_remove_last(EWL_CONTAINER(widget)->children)) !=
	       NULL)
		ewl_widget_destroy_recursive(child);
}

static void
__ewl_window_configure(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Widget *child;
	Ewl_Window *win;
	int x, y, width, height, l = 0, r = 0, t = 0, b = 0;

	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	if (w->ebits_object)
	  {
		  ebits_resize(w->ebits_object,
			       EWL_OBJECT(w)->request.w,
			       EWL_OBJECT(w)->request.h);
		  ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

		  if (EWL_CONTAINER(w)->clip_box)
		    {
			    evas_move(w->evas,
				      EWL_CONTAINER(w)->clip_box, l, t);
			    evas_resize(w->evas,
					EWL_CONTAINER(w)->clip_box,
					REQUEST_W(w) - (l + r),
					REQUEST_H(w) - (t + b));
		    }
	  }

	if (win->bg_rect)
	  {
		  evas_move(w->evas, win->bg_rect, 0, 0);
		  evas_resize(w->evas, win->bg_rect, REQUEST_W(w),
			      REQUEST_H(w));
	  }

	e_window_resize(w->evas_window, REQUEST_W(w), REQUEST_H(w));
	evas_set_output_size(w->evas, REQUEST_W(w), REQUEST_H(w));
	evas_set_output_viewport(w->evas, 0, 0, REQUEST_W(w), REQUEST_H(w));

	CURRENT_X(w) = REQUEST_X(w);
	CURRENT_Y(w) = REQUEST_Y(w);
	CURRENT_W(w) = REQUEST_W(w);
	CURRENT_H(w) = REQUEST_H(w);

	if (!EWL_CONTAINER(w)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	x = l;
	y = t;
	width = CURRENT_W(w);
	width -= l + r;
	height = CURRENT_H(w) / ewd_list_nodes(EWL_CONTAINER(w)->children);
	height -= t + b;

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

		  if (REQUEST_X(child) < l)
			  REQUEST_X(child) = l;
		  y += height;

		  ewl_widget_configure(child);

	  }
}

static void
__ewl_window_theme_update(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Window *win;
	char *v;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Shouldn't do anything if the widget isn't realized yet 
	 */
	if (!w->object.realized)
		DRETURN;

	win = EWL_WINDOW(w);

	/*
	 * Check if GFX should be visible or not 
	 */
	v = ewl_theme_data_get(w, "/appearance/window/default/base/visible");

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
	 * If we want to show the new GFX show it :-) 
	 */
	if (v && !strncasecmp(v, "yes", 3))
	  {
		  char *i;

		  i = ewl_theme_image_get(w,
					  "/appearance/window/default/base");

		  if (i)
		    {
			    w->ebits_object = ebits_load(i);
			    FREE(i);

			    if (w->ebits_object)
			      {
				      ebits_add_to_evas(w->ebits_object,
							w->evas);
				      ebits_set_layer(w->ebits_object,
						      w->object.layer);
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

Ewl_Window *
ewl_window_find_window(Window window)
{
	Ewl_Window *retwin;

	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL)
	  {
		  if (retwin->window == window)
			  return retwin;
	  }

	return NULL;
}

Ewl_Window *
ewl_window_find_window_by_evas_window(Window window)
{
	Ewl_Window *retwin;

	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL)
	  {
		  if (EWL_WIDGET(retwin)->evas_window == window)
			  return retwin;
	  }

	return NULL;
}

void
ewl_window_resize(Ewl_Widget * widget, int w, int h)
{
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->current.w = w;
	EWL_OBJECT(widget)->current.h = h;

	e_window_resize(EWL_WINDOW(widget)->window, w, h);
}

void
ewl_window_set_min_size(Ewl_Widget * widget, int w, int h)
{
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->minimum.w = w;
	EWL_OBJECT(widget)->minimum.h = h;

	if (!EWL_OBJECT(widget)->realized)
		return;

	e_window_set_min_size(EWL_WINDOW(widget)->window, w, h);
}

void
ewl_window_set_max_size(Ewl_Widget * widget, int w, int h)
{
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->maximum.w = w;
	EWL_OBJECT(widget)->maximum.h = h;

	if (!EWL_OBJECT(widget)->realized)
		return;

	e_window_set_max_size(EWL_WINDOW(widget)->window, w, h);
}

void
ewl_window_set_title(Ewl_Widget * widget, char *title)
{
	DCHECK_PARAM_PTR("widget", widget);

	if (strcmp(EWL_WINDOW(widget)->title, title))
	  {
		  IF_FREE(EWL_WINDOW(widget)->title);
		  EWL_WINDOW(widget)->title = strdup(title);
	  }

	if (!EWL_OBJECT(widget)->realized)
		return;

	e_window_set_title(EWL_WINDOW(widget)->window, title);
}

char *
ewl_window_get_title(Ewl_Widget * widget)
{
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	return strdup(EWL_WINDOW(widget)->title);
}

void
ewl_window_set_borderless(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	EWL_WINDOW(w)->borderless = 1;

	if (REALIZED(w))
		e_window_hint_set_borderless(EWL_WINDOW(w)->window);

	DLEAVE_FUNCTION;
}

void
ewl_window_move(Ewl_Widget * w, int x, int y)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e_window_move(EWL_WINDOW(w)->window, x, y);

	DLEAVE_FUNCTION;
}
