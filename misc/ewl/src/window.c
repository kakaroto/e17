#include "window.h"

EwlWidget *ewl_window_new(char *type)
{
	EwlWidget *widget = EWL_WIDGET(malloc(sizeof(EwlWindow)));
	ewl_set(widget, "/window/type", ewl_string_dup(type));
	ewl_window_init(widget);
	return widget;
}

void       ewl_window_init(EwlWidget *widget)
{
	EwlWindow *window = EWL_WINDOW(widget);
	ewl_container_init(widget);
	ewl_set(widget, "/object/type", ewl_string_dup("EwlWindow"));

	ewl_callback_add(widget, "configure", ewl_window_handle_configure, NULL);
	ewl_callback_add(widget, "expose", ewl_window_handle_expose, NULL);
	ewl_callback_push(widget, "realize", ewl_window_handle_realize, NULL);
	ewl_callback_push(widget, "unrealize", ewl_window_handle_unrealize, NULL);
	ewl_callback_add(widget, "show", ewl_window_handle_show, NULL);
	ewl_callback_add(widget, "hide", ewl_window_handle_hide, NULL);

	window->evas = evas_new();
	ewl_set(widget, "/window/title", ewl_string_dup("Untitled Window"));
	/* name_hint, class_hint, decoration_hint_flag */

	return;
}

void       ewl_window_free(EwlWidget *widget)
{
	EwlWindow *window = EWL_WINDOW(widget);
	evas_free(window->evas);
	ewl_widget_free(widget);
	return;
}


Evas       ewl_window_get_evas(EwlWidget *widget)
{
	EwlWindow *window = EWL_WINDOW(widget);
	return window->evas;
}


char       ewl_window_find_by_xwin_cb(EwlListNode *node, void *data)
{
	EwlObject *object = EWL_OBJECT(node->data);
	Window    *xwin = (Window*) data,
	          *tmp  = ewl_get(object, "/window/xwindow");
	return (tmp&&(*tmp==*xwin));
}

EwlWidget *ewl_window_find_by_xwin(Window xwin)
{
	EwlListNode *node = ewl_list_find(ewl_get_object_list(),
	                                  ewl_window_find_by_xwin_cb,
	                                  &xwin);
	return node?EWL_WIDGET(node->data):NULL;
}


void       ewl_window_handle_configure(void     *object,
                                       EwlEvent *event,
                                       void     *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	EwlWindow *window = EWL_WINDOW(widget);
	int i, *t, w, h, *pad;
	UNUSED(data);

	fprintf(stderr,"window configure\n");
	t = ewl_event_get_data(event, "width");
	if (t) w = *t; else w = 320;
	t = ewl_event_get_data(event, "height");
	if (t) h = *t; else h = 240;
	ewl_set(widget, "/widget/rect", ewl_rect_new_with_values(0,0,w,h));

	/*evas_set_output(window->evas, ewl_get_display(), *xwin, vis, *cm);*/
	evas_set_output_viewport(window->evas, 0, 0, w, h);
	evas_set_output_size(window->evas, w, h);

	pad = ewl_widget_get_padding(widget);
	if (!pad)	{
		pad = malloc(sizeof(int)*4);
		for (i=0; i<4; pad[i++] = 0);
		ewl_widget_set_padding(widget,pad[0], pad[1], pad[2], pad[3]);
	}

	evas_move(window->evas, widget->background, pad[0], pad[1]);
	evas_resize(window->evas, widget->background, w - (pad[0]+pad[2]),
	                                              h - (pad[1]+pad[3]));
	if (!ewl_widget_get_flag(widget,"tile_background"))	{
		evas_set_image_fill(window->evas, widget->background,
		                    pad[0], pad[1],
		                    w - (pad[0]+pad[2]),
		                    h - (pad[1]+pad[3]));
	}
	ewl_widget_set_layer(widget, 0);
	ewl_event_queue_new("resize", widget);
	return;
}

void       ewl_window_handle_expose(void     *object,
                                    EwlEvent *event,
                                    void     *data)
{
	EwlWindow *window = EWL_WINDOW(object);
	EwlRect *expose_rect = ewl_event_get_data(event, "rect");
	UNUSED(data);
	
	fprintf(stderr,"window expose {%d,%d,%d,%d}\n", expose_rect->x, expose_rect->y, expose_rect->w, expose_rect->h);
	evas_update_rect(window->evas, expose_rect->x, expose_rect->y,
	                               expose_rect->w, expose_rect->h);

	return;
}
void       ewl_window_handle_realize(void     *object,
                                     EwlEvent *event,
                                     void     *data)
{
	EwlWidget   *widget = EWL_WIDGET(object);
	EwlWindow   *window	= EWL_WINDOW(widget);
	GC           *gc;
	XGCValues    tgc;
	/*Atom         wmhints;*/

	Screen      *screen;
	int         *depth;
	Visual      *vis;
	Colormap    *cm;
	Window      *root;
	XContext    *xid;
	Window      *xwin;
	/*XClassHint  *xclass_hint;*/
	MWMHints    *mwmhints;
	XSetWindowAttributes	*attr;

	EwlRect *rect;
	EwlIterator *i;
	char buf[1024];

	UNUSED(event);
	UNUSED(data);

	fprintf(stderr,"window realize %p\n", object);

	if (ewl_widget_is_realized(widget))
		return;

    /* you forgot to tell it that it was realised pabs (dufus ;-)) */
	ewl_widget_set_flag(EWL_WIDGET(object),"realized", TRUE);
	
	/*ewl_widget_get_theme(widget,"/EwlWindow");*/

	screen = ScreenOfDisplay(ewl_get_display(),
	                         DefaultScreen(ewl_get_display()));
	ewl_set(object, "/window/screen", screen);

	vis = evas_get_optimal_visual(window->evas, ewl_get_display());
	ewl_set(object, "/window/visual", vis);

	depth = malloc(sizeof(int));
	*depth = DefaultDepth(ewl_get_display(),
		                  DefaultScreen(ewl_get_display()));
	ewl_set(object,"/window/depth", depth);

	cm = malloc(sizeof(Colormap));
	*cm = evas_get_optimal_colormap(window->evas, ewl_get_display());
	ewl_set(object,"/window/colormap", cm);

	root = malloc(sizeof(Window));
	*root = RootWindow(ewl_get_display(), DefaultScreen(ewl_get_display()));
	ewl_set(object,"/window/root", root);
	
	xid = malloc(sizeof(XContext));
	*xid = XUniqueContext();
	ewl_set(object,"/window/xid", xid);

	attr = malloc(sizeof(XSetWindowAttributes));
	if (ewl_string_cmp("toplevel", ewl_get(object, "/window/type"),0)) {
		attr->override_redirect = False;
	} else if (ewl_string_cmp("transient", ewl_get(object, "/window/type"),0)) {
		attr->override_redirect = True;
	}
	attr->colormap = *cm;
	attr->border_pixel = 0;
	attr->save_under = 0;
	attr->event_mask =  StructureNotifyMask | ButtonPressMask |
	                    ButtonReleaseMask | PointerMotionMask |
	                    EnterWindowMask | LeaveWindowMask | KeyPressMask |
	                    KeyReleaseMask | ButtonMotionMask | ExposureMask |
	                    FocusChangeMask | PropertyChangeMask |
	                    VisibilityChangeMask;
	ewl_set(object, "/window/attributes", attr);

	mwmhints = malloc(sizeof(MWMHints));
	mwmhints->flags = 0;
	ewl_set(object, "/window/mwmhints", mwmhints);

	rect = ewl_widget_get_rect(widget);
	if (!rect)	{
		rect = ewl_rect_new_with_values(0,0,1,1);
		ewl_set(object, "/widget/rect", rect);
	}
	
	tgc.foreground = BlackPixel(ewl_get_display(),
	                            DefaultScreen(ewl_get_display()));
	gc = malloc(sizeof(GC));
	*gc = XCreateGC(ewl_get_display(), *root, GCForeground, &tgc);
	ewl_set(object, "/window/gc", gc);
	
	xwin = malloc(sizeof(Window));
	*xwin = XCreateWindow(ewl_get_display(), *root,
	                      0, 0, 10, 10,
	                      /*rect->x, rect->y, rect->w, rect->h,*/
	                      0, *depth, InputOutput, vis,
	                      /* CWOverrideRedirect | CWSaveUnder |
	                      CWBackingStore |*/ CWColormap |
		                  CWBackPixel | CWBorderPixel |
		                  CWEventMask, attr);
	ewl_set(object, "/window/xwindow", xwin);

	ewl_hash_dump(EWL_OBJECT(object)->data);

	XStoreName(ewl_get_display(), *xwin, ewl_get(object,"/window/title"));

	/* FIXME -- need hints here */

	if (ewl_widget_is_visible(EWL_WIDGET(object)))
		XMapWindow(ewl_get_display(), *xwin);
	XSync(ewl_get_display(), False);

	evas_set_output_method(window->evas, ewl_get_render_method());
	evas_set_font_cache(window->evas, ewl_get_font_cache());
	evas_set_image_cache(window->evas, ewl_get_image_cache());

	for (i=ewl_iterator_start(ewl_get_font_path_list()); i; i=ewl_iterator_next(i)) {
		snprintf(buf,1024,"%s/%s/fonts", (char*) i->data,
		         ewl_get_theme());
		evas_font_add_path(window->evas, buf);
		snprintf(buf,1024,"%s/%s", (char*) i->data, ewl_get_theme());
		evas_font_add_path(window->evas, buf);
		snprintf(buf,1024,"%s", (char*) i->data);
		evas_font_add_path(window->evas, buf);
	}

	evas_set_output(window->evas, ewl_get_display(), *xwin, vis, *cm);

	ewl_widget_get_theme(EWL_WIDGET(object), "EwlWindow");
	
	ewl_add_render(object);

	return;
}

void       ewl_window_handle_unrealize(void     *object,
                                       EwlEvent *event,
                                       void     *data)
{
	UNUSED(object);
	UNUSED(event);
	UNUSED(data);
	return;
}

void       ewl_window_handle_show(void     *object,
                                  EwlEvent *event,
                                  void     *data)
{
	Window *xwin = ewl_get(object, "/window/xwindow");
	UNUSED(event);
	UNUSED(data);

	if (xwin&&ewl_widget_is_realized(EWL_WIDGET(object)))	{
		XMapWindow(ewl_get_display(), *xwin);
		evas_show(EWL_WINDOW(object)->evas, EWL_WIDGET(object)->background);
	}

	return;
}

void       ewl_window_handle_hide(void     *object,
                                  EwlEvent *event,
                                  void     *data)
{
	Window *xwin = ewl_get(object, "/window/xwindow");
	UNUSED(event);
	UNUSED(data);

	if (xwin&&ewl_widget_is_realized(EWL_WIDGET(object)))	{
		XUnmapWindow(ewl_get_display(), *xwin);
	}

	return;
}

