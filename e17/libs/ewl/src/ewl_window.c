
#include <Ewl.h>

Ewd_List * ewl_window_list = NULL;

static void ewl_window_init(Ewl_Widget * widget);
static void ewl_window_realize(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_window_show(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_window_hide(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_window_destroy(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_window_configure(Ewl_Widget * widget, Ewl_Callback * cb);

Ewl_Widget *
ewl_window_new()
{
	Ewl_Window * window = NULL;

	window = NEW(Ewl_Window, 1);

	ewl_window_init(EWL_WIDGET(window));

	return EWL_WIDGET(window);
}

static void
ewl_window_init(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_WINDOW(widget), 0, sizeof(Ewl_Window));

	EWL_WINDOW(widget)->title = strdup("EWL!");
	widget->type = EWL_WIDGET_WINDOW;

	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
							ewl_window_realize, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW,
							ewl_window_show, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE,
							ewl_window_hide, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY,
							ewl_window_destroy, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
							ewl_window_configure, NULL);

	widget->container.recursive = TRUE;

    EWL_OBJECT(widget)->current.w = 256;
    EWL_OBJECT(widget)->current.h = 256;
    EWL_OBJECT(widget)->minimum.w = 1;
    EWL_OBJECT(widget)->minimum.h = 1;
    EWL_OBJECT(widget)->maximum.w = 2024;
    EWL_OBJECT(widget)->maximum.h = 2024;
    EWL_OBJECT(widget)->request.w = 256;
    EWL_OBJECT(widget)->request.h = 256;

	EWL_OBJECT(widget)->layer = -999;

	if (!ewl_window_list)
		ewl_window_list = ewd_list_new();

	ewd_list_append(ewl_window_list, widget);
}

static void
ewl_window_realize(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ewl_Window * window = NULL;
	char * image = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	window = EWL_WINDOW(widget);

	window->window = e_window_new(0, 0, 0, EWL_OBJECT(widget)->current.w,
											EWL_OBJECT(widget)->current.h);
	e_window_set_events(window->window, XEV_CONFIGURE);
	e_window_set_name_class(window->window, "EWL", "EWL!");
	e_window_set_min_size(window->window, EWL_OBJECT(widget)->minimum.w,
										  EWL_OBJECT(widget)->minimum.h);
	e_window_set_max_size(window->window, EWL_OBJECT(widget)->maximum.w,
										  EWL_OBJECT(widget)->maximum.h);
	e_window_set_title(window->window, window->title);

	e_window_set_delete_inform(window->window);

	EWL_WIDGET(window)->evas = evas_new_all(e_display_get(),
			window->window, 0, 0,
			EWL_OBJECT(widget)->current.w, EWL_OBJECT(widget)->current.h,
			ewl_prefs_render_method_get(),
			216,
			1024 * 1024 * 2,
			1024 * 1024 * 5,
			ewl_theme_font_path());

	widget->evas_window = evas_get_window(widget->evas);
	e_window_set_events(widget->evas_window, XEV_KEY | XEV_BUTTON |
				XEV_IN_OUT | XEV_EXPOSE | XEV_VISIBILITY | XEV_MOUSE_MOVE);


	image = ewl_theme_ebit_get("window", "default", "base");
	EWL_WINDOW(widget)->ebits_object = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(EWL_WINDOW(widget)->ebits_object, widget->evas);
	ebits_set_layer(EWL_BUTTON(widget)->ebits_object, -1000);
}

static void
ewl_window_show(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	e_window_show(EWL_WINDOW(widget)->window);
	e_window_show(widget->evas_window);
	ebits_show(EWL_WINDOW(widget)->ebits_object);

	ewl_widget_configure(widget);
}

static void
ewl_window_hide(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	e_window_hide(widget->evas_window);
	e_window_hide(EWL_WINDOW(widget)->window);
}

static void
ewl_window_destroy(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	IF_FREE(EWL_WINDOW(widget)->title);

	ebits_hide(EWL_WINDOW(widget)->ebits_object);
	ebits_free(EWL_WINDOW(widget)->ebits_object);

	evas_free(widget->evas);

	e_window_hide(widget->evas_window);
	e_window_hide(EWL_WINDOW(widget)->window);

	e_window_destroy(widget->evas_window);
	e_window_destroy(EWL_WINDOW(widget)->window);

	if (ewd_list_goto(ewl_window_list, widget))
		ewd_list_remove(ewl_window_list);

	FREE(widget);
}

static void
ewl_window_configure(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ewl_Widget * child;
	int x, y, l = 0, r = 0, t = 0, b = 0;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_WINDOW(widget)->ebits_object)
		ebits_resize(EWL_WINDOW(widget)->ebits_object,
				EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);

	EWL_OBJECT(widget)->request.x = 0;
	EWL_OBJECT(widget)->request.y = 0;

	ebits_get_insets(EWL_WINDOW(widget)->ebits_object, &l, &r, &t, &b);

	if (widget->container.clip_box)
	  {
		evas_move(widget->evas, widget->container.clip_box,
				EWL_OBJECT(widget)->request.x + l,
				EWL_OBJECT(widget)->request.y + t);
		evas_resize(widget->evas, widget->container.clip_box,
				EWL_OBJECT(widget)->request.w - (l+r),
				EWL_OBJECT(widget)->request.h - (t+b));
	  }

	e_window_resize(widget->evas_window,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);
	evas_set_output_size(widget->evas,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);
	evas_set_output_viewport(widget->evas, 0, 0,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);

	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;

	if (widget->container.children)
		ewd_list_goto_first(widget->container.children);

	x = l;
	y = t;

	if (widget->container.children)
	while ((child = ewd_list_next(widget->container.children)) != NULL)
	  {
		EWL_OBJECT(child)->request.x = x;
		EWL_OBJECT(child)->request.y = y;
		EWL_OBJECT(child)->request.w = EWL_OBJECT(widget)->current.w - l - r;
		EWL_OBJECT(child)->request.h = (EWL_OBJECT(widget)->current.h /
										widget->container.children->nodes) - b - t;
		y += EWL_OBJECT(child)->current.h + t;
		ewl_widget_configure(child);
	  }
}

Ewl_Window *
ewl_window_find_window(Window window)
{
	Ewl_Window * retwin;

	CHECK_PARAM_POINTER_RETURN("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = EWL_WINDOW(ewd_list_next(ewl_window_list))) != NULL)
	  {
		if (retwin->window == window)
			return retwin;
	  }

	return NULL;
}

Ewl_Window *
ewl_window_find_window_by_evas_window(Window window)
{
	Ewl_Window * retwin;

	CHECK_PARAM_POINTER_RETURN("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = EWL_WINDOW(ewd_list_next(ewl_window_list))) != NULL)
	  {
		if (EWL_WIDGET(retwin)->evas_window == window)
			return retwin;
	  }

	return NULL;
}

void
ewl_window_resize(Ewl_Widget * widget, int w, int h)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.w = w;
	EWL_OBJECT(widget)->current.h = h;

	e_window_resize(EWL_WINDOW(widget)->window, w, h);
}

void
ewl_window_set_min_size(Ewl_Widget * widget, int w, int h)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->minimum.w = w;
	EWL_OBJECT(widget)->minimum.h = h;

	if (!EWL_OBJECT(widget)->realized)
		return;

	e_window_set_min_size(EWL_WINDOW(widget)->window, w, h);
}

void
ewl_window_set_max_size(Ewl_Widget * widget, int w, int h)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->maximum.w = w;
	EWL_OBJECT(widget)->maximum.h = h;

	if (!EWL_OBJECT(widget)->realized)
		return;

	e_window_set_max_size(EWL_WINDOW(widget)->window, w, h);
}

void
ewl_window_set_title(Ewl_Widget * widget, char * title)
{
	CHECK_PARAM_POINTER("widget", widget);

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
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return strdup(EWL_WINDOW(widget)->title);
}
