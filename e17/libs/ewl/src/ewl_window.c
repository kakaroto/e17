
#include <Ewl.h>

Ewd_List *ewl_window_list = NULL;

static void __ewl_window_init(Ewl_Window * w);
static void __ewl_window_realize(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_window_show(Ewl_Widget * w, void *ev_data,
			      void *user_data);
static void __ewl_window_hide(Ewl_Widget * w, void *ev_data,
			      void *user_data);
static void __ewl_window_destroy(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_window_configure(Ewl_Widget * w, void *ev_data,
				   void *user_data);

Ewl_Widget *
ewl_window_new()
{
	Ewl_Window *w;

	DENTER_FUNCTION;

	w = NEW(Ewl_Window, 1);
	if (!w)
		DRETURN_PTR(NULL);

	memset(w, 0, sizeof(Ewl_Window));
	__ewl_window_init(w);

	DRETURN_PTR(EWL_WIDGET(w));
}

static void
__ewl_window_init(Ewl_Window * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Initialize the fields of the inherited container class
	 */
	ewl_container_init(EWL_CONTAINER(w), "/appearance/window/default");
	ewl_object_set_minimum_size(EWL_OBJECT(w), 255, 255);

	w->title = strdup("EWL!");

	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			    __ewl_window_realize, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_SHOW,
			    __ewl_window_show, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_HIDE,
			    __ewl_window_hide, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			    __ewl_window_destroy, NULL);
	/*
	 * Override the default configure callbacks since the window
	 * has special needs for placement.
	 */
	ewl_callback_del_type(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			    __ewl_window_configure, NULL);

	EWL_WIDGET(w)->layer = -999;

	ewd_list_append(ewl_window_list, w);

	DLEAVE_FUNCTION;
}

static void
__ewl_window_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window *window;
	char *font_path;

	DENTER_FUNCTION;
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

	DLEAVE_FUNCTION;
}

static void
__ewl_window_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e_window_show(EWL_WINDOW(w)->window);
	e_window_show(w->evas_window);

	if (EWL_WINDOW(w)->borderless)
		e_window_hint_set_borderless(EWL_WINDOW(w)->window);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_window_hide(Ewl_Widget * widget, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("widget", widget);

	e_window_hide(widget->evas_window);
	e_window_hide(EWL_WINDOW(widget)->window);

	DLEAVE_FUNCTION;
}

static void
__ewl_window_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	IF_FREE(EWL_WINDOW(w)->title);

	e_window_hide(w->evas_window);
	e_window_hide(EWL_WINDOW(w)->window);

	e_window_destroy(w->evas_window);
	e_window_destroy(EWL_WINDOW(w)->window);

	IF_FREE(EWL_WINDOW(w)->title);

	if (ewd_list_goto(ewl_window_list, w))
		ewd_list_remove(ewl_window_list);

	DLEAVE_FUNCTION;
}

static void
__ewl_window_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *child;
	Ewl_Window *win;
	int x, y, width, height, l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);
	ewl_object_apply_requested(EWL_OBJECT(w));

	if (w->ebits_object) {
		ebits_move(w->ebits_object, 0, 0);
		ebits_resize(w->ebits_object, CURRENT_W(w), CURRENT_H(w));
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);
	}

	if (w->fx_clip_box) {
		evas_move(w->evas, w->fx_clip_box, 0, 0);
		evas_resize(w->evas, w->fx_clip_box, CURRENT_W(w), CURRENT_H(w));
	}

	if (EWL_CONTAINER(w)->clip_box) {
		evas_move(w->evas,
			  EWL_CONTAINER(w)->clip_box, l, t);
		evas_resize(w->evas,
			    EWL_CONTAINER(w)->clip_box,
			    CURRENT_W(w) - (l + r),
			    CURRENT_H(w) - (t + b));
	}

	if (win->bg_rect) {
		evas_move(w->evas, win->bg_rect, 0, 0);
		evas_resize(w->evas, win->bg_rect, CURRENT_W(w),
			    CURRENT_H(w));
	}

	e_window_resize(w->evas_window, CURRENT_W(w), CURRENT_H(w));
	evas_set_output_size(w->evas, CURRENT_W(w), CURRENT_H(w));
	evas_set_output_viewport(w->evas, 0, 0, CURRENT_W(w),
				 CURRENT_H(w));

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

	while ((child = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		ewl_object_request_geometry(EWL_OBJECT(child), x, y, width,
					    height);

		if (REQUEST_W(child) < MIN_W(child)) {
			REQUEST_X(child) += (REQUEST_W(child) / 2) -
			    (MIN_W(child) / 2);
			REQUEST_W(child) = MIN_W(child);
		}

		if (REQUEST_H(child) < MIN_H(child)) {
			REQUEST_Y(child) += (REQUEST_H(child) / 2) -
			    (MIN_H(child) / 2);
			REQUEST_H(child) = MIN_H(child);
		}

		if (REQUEST_W(child) > MAX_W(child)) {
			REQUEST_X(child) += (REQUEST_W(child) / 2) -
			    (MAX_W(child) / 2);
			REQUEST_W(child) = MAX_W(child);
		}

		if (REQUEST_H(child) > MAX_H(child)) {
			REQUEST_Y(child) += (REQUEST_H(child) / 2) -
			    (MAX_H(child) / 2);
			REQUEST_H(child) = MAX_H(child);
		}

		if (REQUEST_X(child) < l)
			REQUEST_X(child) = l;
		y += height;

		ewl_widget_configure(child);
	}

	DLEAVE_FUNCTION;
}

Ewl_Window *
ewl_window_find_window(Window window)
{
	Ewl_Window *retwin;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL)
		if (retwin->window == window)
			DRETURN_PTR(retwin);

	DRETURN_PTR(NULL);
}

Ewl_Window *
ewl_window_find_window_by_evas_window(Window window)
{
	Ewl_Window *retwin;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL) {
		if (EWL_WIDGET(retwin)->evas_window == window)
			return retwin;
	}

	DRETURN_PTR(NULL);
}

void
ewl_window_resize(Ewl_Widget * widget, int w, int h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->current.w = w;
	EWL_OBJECT(widget)->current.h = h;

	e_window_resize(EWL_WINDOW(widget)->window, w, h);

	DLEAVE_FUNCTION;
}

void
ewl_window_set_min_size(Ewl_Widget * widget, int w, int h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->minimum.w = w;
	EWL_OBJECT(widget)->minimum.h = h;

	if (!REALIZED(widget))
		return;

	e_window_set_min_size(EWL_WINDOW(widget)->window, w, h);

	DLEAVE_FUNCTION;
}

void
ewl_window_set_max_size(Ewl_Widget * widget, int w, int h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	EWL_OBJECT(w)->maximum.w = w;
	EWL_OBJECT(w)->maximum.h = h;

	if (!REALIZED(w))
		return;

	e_window_set_max_size(EWL_WINDOW(w)->window, w, h);

	DLEAVE_FUNCTION;
}

void
ewl_window_set_title(Ewl_Widget * w, char *title)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (strcmp(EWL_WINDOW(w)->title, title)) {
		IF_FREE(EWL_WINDOW(w)->title);
		EWL_WINDOW(w)->title = strdup(title);
	}

	if (!REALIZED(w))
		return;

	e_window_set_title(EWL_WINDOW(w)->window, title);

	DLEAVE_FUNCTION;
}

char *
ewl_window_get_title(Ewl_Widget * widget)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	DRETURN_PTR(strdup(EWL_WINDOW(widget)->title));
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
