
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
	ewl_container_init(EWL_CONTAINER(w), EWL_WIDGET_WINDOW, 256, 256,
			   2048, 2048);

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

	window->window =
		e_window_new(0, 0, 0, EWL_OBJECT(w)->current.w,
			     EWL_OBJECT(w)->current.h);
	e_window_set_events(window->window, XEV_CONFIGURE);
	e_window_set_name_class(window->window, "EWL", "EWL!");
	e_window_set_min_size(window->window,
			      EWL_OBJECT(w)->minimum.w,
			      EWL_OBJECT(w)->minimum.h);
	e_window_set_max_size(window->window,
			      EWL_OBJECT(w)->maximum.w,
			      EWL_OBJECT(w)->maximum.h);
	e_window_set_title(window->window, window->title);

	e_window_set_delete_inform(window->window);

	font_path = ewl_theme_font_path();

	w->evas = evas_new_all(e_display_get(),
			       window->window, 0, 0,
			       EWL_OBJECT(w)->
			       current.w,
			       EWL_OBJECT(w)->
			       current.h,
			       ewl_prefs_render_method_get
			       (), 216, 1024 * 1024 * 2,
			       1024 * 1024 * 5, font_path);

	IF_FREE(font_path);

	w->evas_window = evas_get_window(w->evas);
	e_window_set_events(w->evas_window, XEV_KEY | XEV_BUTTON |
			    XEV_IN_OUT | XEV_EXPOSE | XEV_VISIBILITY |
			    XEV_MOUSE_MOVE);

	ewl_widget_theme_update(w);
}

static void
__ewl_window_show(Ewl_Widget * w, void *event_data, void *user_data)
{
	DCHECK_PARAM_PTR("w", w);

	e_window_show(EWL_WINDOW(w)->window);
	e_window_show(w->evas_window);

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

	ewl_callback_del_all(w);

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
__ewl_window_configure(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Widget *child;
	int x, y, l = 0, r = 0, t = 0, b = 0;

	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->request.x = 0;
	EWL_OBJECT(widget)->request.y = 0;

	if (widget->ebits_object)
	  {
		  ebits_resize(widget->ebits_object,
			       EWL_OBJECT(widget)->request.w,
			       EWL_OBJECT(widget)->request.h);
		  ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

		  if (EWL_CONTAINER(widget)->clip_box)
		    {
			    evas_move(widget->evas,
				      EWL_CONTAINER(widget)->clip_box,
				      REQUEST_X(widget) + l,
				      REQUEST_Y(widget) + t);
			    evas_resize(widget->evas,
					EWL_CONTAINER(widget)->clip_box,
					REQUEST_W(widget) - (l + r),
					REQUEST_H(widget) - (t + b));
		    }
	  }

	e_window_resize(widget->evas_window, REQUEST_W(widget),
				REQUEST_H(widget));
	evas_set_output_size(widget->evas, REQUEST_W(widget),
				REQUEST_H(widget));
	evas_set_output_viewport(widget->evas, 0, 0,
				REQUEST_W(widget),
				REQUEST_H(widget));

	CURRENT_W(widget) = REQUEST_W(widget);
	CURRENT_H(widget) = REQUEST_H(widget);

	if (EWL_CONTAINER(widget)->children)
		ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	x = l;
	y = t;

	if (EWL_CONTAINER(widget)->children)
		while ((child =
			ewd_list_next(EWL_CONTAINER(widget)->children)) !=
		       NULL)
		  {
			  REQUEST_X(child) = x;
			  REQUEST_Y(child) = y;
			  REQUEST_W(child) = CURRENT_W(widget) - l - r;
			  REQUEST_H(child) = (CURRENT_H(widget) /
					      ewd_list_nodes(EWL_CONTAINER
							     (widget)->
							     children)) - b -
				  t;
			  y += CURRENT_H(child) + t;
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
