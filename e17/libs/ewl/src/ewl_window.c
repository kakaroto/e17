
#include <Ewl.h>

Ewd_List * ewl_window_list = NULL;

static void ewl_window_init(Ewl_Widget * widget);
static void ewl_window_realize(Ewl_Widget * widget, void * func_data);
static void ewl_window_show(Ewl_Widget * widget, void * func_data);
static void ewl_window_hide(Ewl_Widget * widget, void * func_data);
static void ewl_window_destroy(Ewl_Widget * widget, void * func_data);
static void ewl_window_configure(Ewl_Widget * widget, void * func_data);

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

	ewl_callback_append(widget, Ewl_Callback_Realize,
							ewl_window_realize, NULL);
	ewl_callback_append(widget, Ewl_Callback_Show,
							ewl_window_show, NULL);
	ewl_callback_append(widget, Ewl_Callback_Hide,
							ewl_window_hide, NULL);
	ewl_callback_append(widget, Ewl_Callback_Destroy,
							ewl_window_destroy, NULL);
	ewl_callback_append(widget, Ewl_Callback_Configure,
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
ewl_window_realize(Ewl_Widget * widget, void * func_data)
{
	Ewl_Window * window = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	window = EWL_WINDOW(widget);

	window->window = e_window_new(0, 0, 0, EWL_OBJECT(widget)->current.w,
											EWL_OBJECT(widget)->current.h);
	e_window_set_events(window->window, XEV_CONFIGURE);
	e_window_set_name_class(window->window, "EWL", "EWL!");

	e_window_set_title(window->window, window->title);

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


	ewl_widget_set_ebit(widget,ewl_theme_ebit_get("window", "default", "base"));

	ewl_container_new(widget);

	return;
	func_data = NULL;
}

static void
ewl_window_show(Ewl_Widget * widget, void * func_data)
{
	e_window_show(EWL_WINDOW(widget)->window);
	e_window_show(widget->evas_window);
	ebits_show(widget->ebits_object);

	return;
}

static void
ewl_window_hide(Ewl_Widget * widget, void * func_data)
{
	return;
}

static void
ewl_window_destroy(Ewl_Widget * widget, void * func_data)
{

	IF_FREE(EWL_WINDOW(widget)->title);

	FREE(widget);
}

static void
ewl_window_configure(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * child;
	int x, y, l = 0, r = 0, t = 0, b = 0;

	CHECK_PARAM_POINTER("widget", widget);

	if (widget->ebits_object)
		ebits_resize(widget->ebits_object,
				EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);

	EWL_OBJECT(widget)->request.x = 0;
	EWL_OBJECT(widget)->request.y = 0;

	ewl_container_clip_box_resize(widget);

	e_window_resize(widget->evas_window,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);
	evas_set_output_size(widget->evas,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);
	evas_set_output_viewport(widget->evas, 0, 0,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);

	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;

	ewd_list_goto_first(widget->container.children);

	if (widget->ebits_object);
		ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

	x = l;
	y = t;

	while ((child = ewd_list_next(widget->container.children)) != NULL) {
		EWL_OBJECT(child)->request.x = x;
		EWL_OBJECT(child)->request.y = y;
		EWL_OBJECT(child)->request.w = EWL_OBJECT(widget)->current.w - l - r;
		EWL_OBJECT(child)->request.h = (EWL_OBJECT(widget)->current.h /
										widget->container.children->nodes) - b - t;
		y += EWL_OBJECT(child)->current.h + t;
		ewl_callback_call(child, Ewl_Callback_Configure);
	}

	return;
	func_data = NULL;
}

Ewl_Window *
ewl_window_find_window(Window window)
{
	Ewl_Window * retwin;

	CHECK_PARAM_POINTER_RETURN("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = EWL_WINDOW(ewd_list_next(ewl_window_list))) != NULL) {
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

	while((retwin = EWL_WINDOW(ewd_list_next(ewl_window_list))) != NULL) {
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

	if (strcmp(EWL_WINDOW(widget)->title, title)) {
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
