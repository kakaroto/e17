#include "ewlwindow.h"

EwlWidget   *ewl_window_new()
{
	EwlWindow *win = NULL;
	FUNC_BGN("ewl_window_new");
	win = (EwlWindow*) ewl_window_new_with_values(EWL_DEFAULT_WINDOW_TITLE,
	                                              EWL_DEFAULT_WINDOW_WIDTH,
	                                              EWL_DEFAULT_WINDOW_HEIGHT,
	                                              EWL_DEFAULT_WINDOW_NAME_H,
	                                              EWL_DEFAULT_WINDOW_CLASS_H);
	if (!win)	{
		ewl_debug("ewl_window_new", EWL_NULL_ERROR, "w");
	}
	FUNC_END("ewl_window_new");
	return (EwlWidget*) win;
}

EwlWidget   *ewl_window_new_with_values(char *title, int w, int h,
                                        char *name_hint, char *class_hint)
{
	EwlWindow *win = NULL;
	FUNC_BGN("ewl_window_new_with_values");
	win = malloc(sizeof(EwlWindow));
	if (!win)	{
		ewl_debug("ewl_window_new_with_values", EWL_NULL_ERROR, "w");
	} else {
		ewl_window_init(win,title,w,h,name_hint,class_hint);
	}
	FUNC_END("ewl_window_new_with_values");
	return (EwlWidget*) win;
}

EwlBool _cb_ewl_window_event_handler(EwlWidget *widget, EwlEvent *ev,
                                     EwlData *data)
{
	EwlState          *s = ewl_state_get();
	EwlWindow         *window = (EwlWindow*) widget;
	EwlBool            r = TRUE;
	EwlEventExpose    *e_ev = (EwlEventExpose*) ev;
	EwlEventConfigure *c_ev = (EwlEventConfigure*) ev;
	int                width  = 0,
	                   height = 0;

	switch(ev->type)	{
	case EWL_EVENT_SHOW:
		ewl_widget_set_flag(widget, VISIBLE, TRUE);
		XMapWindow(s->disp, window->xwin);
		/*fprintf(stderr,"_cb_ewl_win_show called.\n");*/
		break;
	case EWL_EVENT_HIDE:
		ewl_widget_set_flag(widget, VISIBLE, FALSE);
		XUnmapWindow(s->disp, window->xwin);
		/*fprintf(stderr,"_cb_ewl_win_hide called.\n");*/
		break;
	case EWL_EVENT_EXPOSE:
		if (e_ev->count)
			break;
		ewl_widget_render(widget);

		ewl_window_set_render_context(widget);
		ewl_imlib_render_image_on_drawable_at_size(
		                       window->pmap,
		                       widget->rendered,
		                       widget->layout->rect->x,
		                       widget->layout->rect->y,
		                       widget->layout->rect->w,
		                       widget->layout->rect->h,
		                       0.0,
		                       ewl_state_render_dithered_get(),
		                       FALSE,  /* don't blend onto the drawable */
		                       ewl_state_render_antialiased_get());

		XSetWindowBackgroundPixmap(s->disp, window->xwin, window->pmap);
		XClearWindow(s->disp, window->xwin);
		break;
	case EWL_EVENT_CONFIGURE:
		/* set up new window rect and shit here */
		width =  c_ev->width;
		height = c_ev->height;
		ewl_rect_free(widget->layout->rect);
		widget->layout->rect = ewl_rect_new_with_values(0, 0, 
		                                                &width, &height);

		if (window->pmap)	
			XFreePixmap(s->disp,window->pmap);
		window->pmap = XCreatePixmap(s->disp,
		                             window->xwin,
		                             width, height,
		                             window->depth);
		ewl_widget_set_flag(widget, NEEDS_RESIZE, TRUE);
		ewl_container_resize_children(widget);
		break;
	default:
		break;
	}
	return r;
}

static EwlBool _cb_ewl_window_init_show_imlayers(EwlImLayer *layer,
                                                 EwlData    *data)
{
	ewl_imlayer_show(layer);
	return TRUE;
}

void         ewl_window_init(EwlWindow *win, char *title, int w, int h,
                             char *name_hint, char *class_hint)
{
	EwlWidget            *widget = (EwlWidget*) win;
	EwlContainer         *container = (EwlContainer*) win;
	EwlState             *s = ewl_state_get();
	XSetWindowAttributes  attr;
	XClassHint            *xch;
	MWMHints              hints;
	XGCValues             gc;
	int                   t = 0;

	FUNC_BGN("ewl_window_init");
	if (!win)	{
		ewl_debug("ewl_window_init", EWL_NULL_ERROR, "win");
		FUNC_END("ewl_window_init");
		return;
	}

	/* inita s parent type */
	ewl_container_init(container);
	ewl_container_set_max_children(widget, 1);
	ewl_widget_set_type(widget,EWL_WINDOW);
	win->pmap = 0;

	/* LAOD DB SHIT HERE */
	ewl_widget_get_theme(widget,"/EwlWindow");
	ewl_widget_imlayer_foreach(widget,
	                           _cb_ewl_window_init_show_imlayers,
	                           NULL);
	if (ewl_theme_get_int("/EwlWindow/child_padding/left",   &t))
		ewl_container_set_child_padding(widget,&t,0,0,0);
	if (ewl_theme_get_int("/EwlWindow/child_padding/top",    &t))
		ewl_container_set_child_padding(widget,0,&t,0,0);
	if (ewl_theme_get_int("/EwlWindow/child_padding/right",  &t))
		ewl_container_set_child_padding(widget,0,0,&t,0);
	if (ewl_theme_get_int("/EwlWindow/child_padding/bottom", &t)) 
		ewl_container_set_child_padding(widget,0,0,0,&t);

	/* set up callbacks */
	ewl_callback_add(widget, EWL_EVENT_SHOW,
	                 _cb_ewl_window_event_handler, NULL);
	/*ewl_callback_add(widget,EWL_EVENT_SHOW,_cb_sample_nested,NULL);*/
	ewl_callback_add(widget, EWL_EVENT_HIDE,
	                 _cb_ewl_window_event_handler, NULL);
	ewl_callback_add(widget, EWL_EVENT_EXPOSE,
	                 _cb_ewl_window_event_handler, NULL);
	ewl_callback_add(widget, EWL_EVENT_CONFIGURE,
	                 _cb_ewl_window_event_handler, NULL);

	/* init ewl attributes */
	if (w==-1)
		w = EWL_DEFAULT_WINDOW_WIDTH;
	if (h==-1)
		h = EWL_DEFAULT_WINDOW_HEIGHT;
	win->name_hint = name_hint?name_hint:EWL_DEFAULT_WINDOW_NAME_H;
	win->class_hint = class_hint?class_hint:EWL_DEFAULT_WINDOW_CLASS_H;

	win->screen = ScreenOfDisplay(s->disp, DefaultScreen(s->disp));
	win->vis = DefaultVisual(s->disp, DefaultScreen(s->disp));
	win->depth = DefaultDepth(s->disp, DefaultScreen(s->disp));
	win->cm = DefaultColormap(s->disp,DefaultScreen(s->disp));
	win->root = RootWindow(s->disp,DefaultScreen(s->disp));
	/*wid->root = DefaultRootWindow(s->disp);*/
	win->xid = XUniqueContext();
	
	/* init X attributes */
	attr.backing_store = NotUseful;
	attr.override_redirect = False;
	attr.colormap = win->cm;
	attr.border_pixel = 0;
	attr.background_pixel = 0;
	attr.save_under = 0;
	attr.event_mask = StructureNotifyMask | ButtonPressMask |
	                  ButtonReleaseMask | PointerMotionMask |
	                  EnterWindowMask | LeaveWindowMask | KeyPressMask |
	                  KeyReleaseMask | ButtonMotionMask | ExposureMask |
	                  FocusChangeMask | PropertyChangeMask |
	                  VisibilityChangeMask;
	hints.flags = 0;

	win->xwin = XCreateWindow(s->disp, win->root, 0, 0, w, h, 0, win->depth,
	            	InputOutput, win->vis,
	            	CWOverrideRedirect | CWSaveUnder | CWBackingStore |
	            	CWColormap | CWBackPixel | CWBorderPixel |CWEventMask,
	            	&attr);
	XStoreName(s->disp, win->root, title?title:EWL_DEFAULT_WINDOW_TITLE);

	xch = XAllocClassHint();
	xch->res_name = win->name_hint;
	xch->res_class = win->class_hint;
	XSetClassHint(s->disp, win->xwin, xch);
	XFree(xch);

	gc.foreground = BlackPixel(s->disp, DefaultScreen(s->disp));
	win->gc = XCreateGC(s->disp, win->root, GCForeground, &gc);

	FUNC_END("ewl_window_init");
	return;
}

void         ewl_window_pack(EwlWidget *window, EwlWidget *child)
{
	EwlWidget    *widget = (EwlWidget*) window;
	FUNC_BGN("ewl_window_pack");
	if (!window)	{
		ewl_debug("ewl_window_pack", EWL_NULL_WIDGET_ERROR, "window");
	} else if (!child) {
		ewl_debug("ewl_window_pack", EWL_NULL_WIDGET_ERROR, "child");
	} else {
		ewl_container_insert(widget,child);
	}
	FUNC_END("ewl_window_pack");
	return;
}

static char _cb_find_xwin(EwlLL *node, EwlData *data)
{
	if (!data)
		return 0;
	if (((EwlWidget*)node->data)->type != EWL_WINDOW)
		return 0;
	if (((EwlWindow*)node->data)->xwin==(Window)data)
		return 1;
	return 0;
}

EwlWidget *ewl_window_find_by_xwin(Window xwin)
{
	EwlState  *s = ewl_state_get();
	EwlLL     *ll = ewl_ll_callback_find(s->widget_list,
	                                     _cb_find_xwin,
	                                     (EwlData*)xwin);
	EwlWidget *w = NULL;
	char       t[1024] = "";
	FUNC_BGN("ewl_widget_find_by_xwin");
	if (ll)	{
		w = (EwlWidget*)ll->data;
		/*fprintf(stderr, "ewl_window_find_by_xwin(): Found window %08x.\n",
		        (unsigned int)xwin);*/
	} else {
		sprintf(t, "Couldn't find window %08x.", (unsigned int)xwin);
		ewl_debug("ewl_widget_find_by_xwin",EWL_GENERIC_ERROR,t);
	}
	FUNC_END("ewl_widget_find_by_xwin");
	return w;
}

/* MAYBE THIS SHOULD BE A PUSH/POP AND SAVED IN THE STATE STRUCT INSTEAD?*/
void         ewl_window_set_render_context(EwlWidget *widget)
{
	EwlState  *s = ewl_state_get();
	EwlWindow *window = (EwlWindow*) widget;
	FUNC_BGN("ewl_window_set_render_context");
	if (!s)	{
		ewl_debug("ewl_window_set_render_context", EWL_NULL_ERROR, "s");
	} else if (!window)	{
		ewl_debug("ewl_window_set_render_context", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		imlib_context_set_display(s->disp);
		imlib_context_set_visual(window->vis);
		imlib_context_set_colormap(window->cm);
	}
	FUNC_END("ewl_window_set_render_context");
	return;
}
