#include "ewlwindow.h"

EwlWidget   *ewl_window_new(EwlWindowType	type)
{
	EwlWindow *win = NULL;
	FUNC_BGN("ewl_window_new");
	win = (EwlWindow*) ewl_window_new_with_values(type,
												  EWL_DEFAULT_WINDOW_TITLE,
	                                              EWL_DEFAULT_WINDOW_WIDTH,
	                                              EWL_DEFAULT_WINDOW_HEIGHT);
	if (!win)	{
		ewl_debug("ewl_window_new", EWL_NULL_ERROR, "w");
	}
	FUNC_END("ewl_window_new");
	return (EwlWidget*) win;
}

EwlWidget   *ewl_window_new_with_values(EwlWindowType type, char *title, 
										int w, int h)
{
	EwlWindow *win = NULL;
	FUNC_BGN("ewl_window_new_with_values");
	win = malloc(sizeof(EwlWindow));
	if (!win)	{
		ewl_debug("ewl_window_new_with_values", EWL_NULL_ERROR, "w");
	} else {
		ewl_window_init(win,type,title,w,h);
	}
	FUNC_END("ewl_window_new_with_values");
	return (EwlWidget*) win;
}

EwlBool _cb_ewl_window_event_handler(EwlWidget *widget, EwlEvent *ev,
                                     EwlData *data)
{
	EwlWindow         *window = (EwlWindow*) widget;
	EwlBool            r = TRUE;
	EwlRect           *rect = NULL;
	EwlEventExpose    *e_ev = (EwlEventExpose*) ev;
	EwlEventConfigure *c_ev = (EwlEventConfigure*) ev;
	int                x = 0, y = 0,
	                   width  = 0,
	                   height = 0;

	switch(ev->type)	{
	case EWL_EVENT_SHOW:
		ewl_widget_set_flag(widget, VISIBLE, TRUE);
		ewl_rect_dump(widget->layout->rect);
		if (!window->xwin)
			ewl_window_realize(widget);
		XMapWindow(ewl_get_display(), window->xwin);
		break;
	case EWL_EVENT_HIDE:
		ewl_widget_set_flag(widget, VISIBLE, FALSE);
		/* ewl_window_unrealize(widget) */
		XUnmapWindow(ewl_get_display(), window->xwin);
		break;
	case EWL_EVENT_EXPOSE:
		/*if (e_ev->count)	{
			fprintf(stderr,"expose: count = %d, rect = ", e_ev->count);
			ewl_rect_dump(e_ev->rect);
			break;
		}*/
		ewl_widget_render(widget);

		ewl_window_set_render_context(widget);
		ewl_imlib_render_image_part_on_drawable_at_size(
		                       window->pmap,
		                       widget->rendered,
		                       e_ev->rect->x,
		                       e_ev->rect->y,
		                       e_ev->rect->w,
		                       e_ev->rect->h,
		                       e_ev->rect->x,
		                       e_ev->rect->y,
		                       e_ev->rect->w,
		                       e_ev->rect->h,
		                       /*widget->layout->rect->x,
		                       widget->layout->rect->y,
		                       widget->layout->rect->w,
		                       widget->layout->rect->h,*/
		                       0.0,
		                       ewl_render_dithered_get(),
		                       FALSE,  /* don't blend onto the drawable */
		                       ewl_render_antialiased_get());

		XSetWindowBackgroundPixmap(ewl_get_display(), window->xwin, window->pmap);
		/*XClearWindow(ewl_get_display(), window->xwin);*/
		XClearArea(ewl_get_display(), window->xwin,
		           e_ev->rect->x, e_ev->rect->y,
		           e_ev->rect->w, e_ev->rect->h,
		           False);
		break;
	case EWL_EVENT_CONFIGURE:
		/* set up new window rect and shit here */
		width =  c_ev->width;
		height = c_ev->height;
		ewl_rect_free(widget->layout->rect);
		widget->layout->rect = ewl_rect_new_with_values(0, 0, 
		                                                &width, &height);
		if (window->pmap)	
			XFreePixmap(ewl_get_display(),window->pmap);
		window->pmap = XCreatePixmap(ewl_get_display(),
		                             window->xwin,
		                             width, height,
		                             window->depth);
		ewl_widget_set_flag(widget, NEEDS_RESIZE, TRUE);
		ewl_container_resize_children(widget);
		break;
	case EWL_EVENT_RESIZE:
		if (widget->layout->req->w < 1)
				widget->layout->req->w = 1;
		if (widget->layout->req->w > EWL_WINDOW_MAX_WIDTH)
				widget->layout->req->w = EWL_WINDOW_MAX_WIDTH;
		if (widget->layout->req->h < 1)
				widget->layout->req->h = 1;
		if (widget->layout->req->h > EWL_WINDOW_MAX_HEIGHT)
				widget->layout->req->h = EWL_WINDOW_MAX_HEIGHT;
		width = widget->layout->req->w;
		height = widget->layout->req->h;
		if (ewl_debug_is_active())
			fprintf(stderr,"attempting to resize window, "
			        "widget = %08x, width = %d, height = %d\n",
			        (unsigned int) widget, width, height);
		rect = widget->layout->rect;
		widget->layout->rect = ewl_rect_new_with_values(
		                                    &(rect->x), &(rect->y),
		                                    &(widget->layout->req->w),
		                                    &(widget->layout->req->h));
		ewl_rect_free(rect);
		if (!window->xwin) {
			if (ewl_debug_is_active())
				fprintf(stderr, "window not realized yet\n");
		} else {
			XResizeWindow(ewl_get_display(),window->xwin,width,height);
			/*if (ewl_debug_is_active())
				fprintf(stderr,"done... "
			        "widget = %08x, width = %d, height = %d\n",
			        (unsigned int) widget, width, height);
			if (window->pmap)	
				XFreePixmap(ewl_get_display(),window->pmap);
			window->pmap = XCreatePixmap(ewl_get_display(),
			                             window->xwin,
			                             width, height,
			                             window->depth);*/
			ewl_widget_set_flag(widget, NEEDS_RESIZE, TRUE);
			/*ewl_container_resize_children(widget);*/
		}
		break;
	case EWL_EVENT_MOVE:
		if (widget->layout->req->x < -EWL_WINDOW_MAX_WIDTH)
			widget->layout->req->x = 0;
		if (widget->layout->req->x > EWL_WINDOW_MAX_WIDTH)
			widget->layout->req->x = 0;
		if (widget->layout->req->y < -EWL_WINDOW_MAX_HEIGHT)
			widget->layout->req->y = 0;
		if (widget->layout->req->y > EWL_WINDOW_MAX_HEIGHT)
			widget->layout->req->y = 0;
		x = widget->layout->req->x;
		y = widget->layout->req->y;
		if (!window->xwin) {
			if (ewl_debug_is_active())
				fprintf(stderr,"can't move window before it's realized\n");
		} else {
			XMoveWindow(ewl_get_display(),window->xwin,x,y);
		}
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

void         ewl_window_init(EwlWindow *win, EwlWindowType type,
							 char *title, int w, int h)
{
	EwlWidget            *widget = (EwlWidget*) win;
	EwlContainer         *container = (EwlContainer*) win;
	char                 *str = NULL;
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
	ewl_widget_set_flag(widget,CAN_RESIZE, TRUE);
	win->pmap = 0;
	win->xwin = 0;

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
	ewl_callback_add(widget, EWL_EVENT_RESIZE,
	                 _cb_ewl_window_event_handler, NULL);
	ewl_callback_add(widget, EWL_EVENT_MOVE,
	                 _cb_ewl_window_event_handler, NULL);

	/* window properties */
	win->type	= type;

	fprintf(stderr,"0. ewl_window_init(): w = %d, h = %d\n", w, h);
	if (w==-1) {
		if (ewl_theme_get_int("/EwlWindow/width", &t))
			w = t;
		else 
			w = EWL_DEFAULT_WINDOW_WIDTH;
	}
	if (h==-1) {
		if (ewl_theme_get_int("/EwlWindow/height", &t))
			h = t;
		else 
			h = EWL_DEFAULT_WINDOW_HEIGHT;
	}
	fprintf(stderr,"1. ewl_window_init(): w = %d, h = %d\n", w, h);
	ewl_widget_resize(widget,w,h);
	fprintf(stderr,"2. ewl_window_init(): w = %d, h = %d\n", w, h);
		
	if (!title)
		title = ewl_theme_get_string("/EwlWindow/title");
	if (!title)
		title = e_string_dup(EWL_DEFAULT_WINDOW_TITLE);
	win->title = title;

	str = ewl_theme_get_string("/EwlWindow/name_hint");
	if (str)
		win->name_hint = str;
	else 
		win->name_hint  = e_string_dup(EWL_DEFAULT_WINDOW_NAME_H);

	str = ewl_theme_get_string("/EwlWindow/class_hint");
	if (str)
		win->class_hint = str;
	else 
		win->class_hint  = e_string_dup(EWL_DEFAULT_WINDOW_CLASS_H);

	win->decoration_hint = FALSE;

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
	EwlWindow *window = (EwlWindow*) widget;
	FUNC_BGN("ewl_window_set_render_context");
	if (!window)	{
		ewl_debug("ewl_window_set_render_context", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		imlib_context_set_display(ewl_get_display());
		imlib_context_set_visual(window->vis);
		imlib_context_set_colormap(window->cm);
	}
	FUNC_END("ewl_window_set_render_context");
	return;
}

void	ewl_window_realize(EwlWidget *widget)
{
	EwlWindow	*win	= (EwlWindow *) widget;
	XGCValues             gc;
	Atom				  wmhints;

	FUNC_BGN("ewl_window_realize");
	if (!win)	{
		ewl_debug("ewl_window_realize", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		fprintf(stderr, "realizing window 0x%08x\n", (unsigned int)win);

		win->screen = ScreenOfDisplay(ewl_get_display(), DefaultScreen(ewl_get_display()));
		win->vis = DefaultVisual(ewl_get_display(), DefaultScreen(ewl_get_display()));
		win->depth = DefaultDepth(ewl_get_display(), DefaultScreen(ewl_get_display()));
		win->cm = DefaultColormap(ewl_get_display(),DefaultScreen(ewl_get_display()));
		win->root = RootWindow(ewl_get_display(),DefaultScreen(ewl_get_display()));
		/*wid->root = DefaultRootWindow(ewl_get_display());*/
		win->xid = XUniqueContext();
		
		/* init X attributes */
		win->attr.backing_store = NotUseful;
		if (win->type == EWL_WINDOW_TOPLEVEL) {
			win->attr.override_redirect = False;
		}
		else if (win->type == EWL_WINDOW_TRANSIENT) {
			win->attr.override_redirect = True;
		}
		win->attr.colormap = win->cm;
		win->attr.border_pixel = 0;
		win->attr.background_pixel = 0;
		win->attr.save_under = 0;
		win->attr.event_mask = StructureNotifyMask | ButtonPressMask |
		                  ButtonReleaseMask | PointerMotionMask |
		                  EnterWindowMask | LeaveWindowMask | KeyPressMask |
		                  KeyReleaseMask | ButtonMotionMask | ExposureMask |
		                  FocusChangeMask | PropertyChangeMask |
		                  VisibilityChangeMask;

		win->mwmhints.flags = 0;
	
		fprintf(stderr,"ewl_window_realize: rect = ");
		ewl_rect_dump(widget->layout->req);
		if (widget->layout->req->w < 1)
				widget->layout->rect->w = 1;
		if (widget->layout->req->w > EWL_WINDOW_MAX_WIDTH)
				widget->layout->rect->w = EWL_WINDOW_MAX_WIDTH;
		if (widget->layout->req->h < 1)
				widget->layout->req->h = 1;
		if (widget->layout->req->h > EWL_WINDOW_MAX_HEIGHT)
				widget->layout->req->h = EWL_WINDOW_MAX_HEIGHT;
		ewl_layout_set_rect(widget->layout,widget->layout->req);
		
		gc.foreground = BlackPixel(ewl_get_display(), DefaultScreen(ewl_get_display()));
		win->gc = XCreateGC(ewl_get_display(), win->root, GCForeground, &gc);
		win->xwin = XCreateWindow(ewl_get_display(), win->root,
		                          widget->layout->req->x,
		                          widget->layout->req->y, 
		                          widget->layout->req->w,
		                          widget->layout->req->h,
		                          0, win->depth,
		                          InputOutput, win->vis,
		                          CWOverrideRedirect | CWSaveUnder |
		                          CWBackingStore | CWColormap |
		                          CWBackPixel | CWBorderPixel |
		                          CWEventMask, &win->attr);

		/* setting title */
		XStoreName(ewl_get_display(), win->xwin, win->title);
		
		/* setting class hint */
 		win->xclass_hint = XAllocClassHint();
		win->xclass_hint->res_name = win->name_hint;
		win->xclass_hint->res_class = win->class_hint;
		XSetClassHint(ewl_get_display(), win->xwin, win->xclass_hint);
		/*XFree(win->xclass_hint);*/
		
		/* Setting up decor */
		if (win->decoration_hint == TRUE) {
			fprintf(stderr, "We want decor!\n");
		} else if (win->decoration_hint == FALSE) {
			fprintf(stderr, "Ixnay on the decor-ay!\n");
			wmhints = XInternAtom(ewl_get_display(), "_MOTIF_WM_HINTS", True);
			if (wmhints != None) {
				MWMHints decorhints = { MWM_HINTS_DECORATIONS, 0, 0, 0, 0 };
				XChangeProperty(ewl_get_display(), win->xwin, wmhints, wmhints, 32,
					PropModeReplace, (unsigned char *)&decorhints,
					sizeof(MWMHints)/4);
			}
		}
	}
	FUNC_END("ewl_window_realize");
	return;
}

void	ewl_window_unrealize(EwlWidget *widget)
{
	EwlWindow	*win	= (EwlWindow *) widget;

	FUNC_BGN("ewl_window_unrealize");
	if (!win)	{
		ewl_debug("ewl_window_unrealize", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		fprintf(stdout, "Unrealizing window 0x%08x\n", (unsigned int)win);
		XUnmapWindow(ewl_get_display(), win->xwin);
	}
	FUNC_END("ewl_window_unrealize");
	return;
}

void	ewl_window_set_title(EwlWidget *widget, char *title)
{
	EwlWindow	*win	= (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_title");
	if (!win)	{
		ewl_debug("ewl_window_set_title", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		win->title	=	title?title:EWL_DEFAULT_WINDOW_TITLE;
	}
	FUNC_END("ewl_window_set_title");
	return;
}

char   *ewl_window_get_title(EwlWidget *widget)
{
	EwlWindow *window = (EwlWindow*) widget;
	char *title = NULL;
	FUNC_BGN("ewl_window_get_title");
	if (!window) {
		ewl_debug("ewl_window_get_title", EWL_NULL_WIDGET_ERROR, "window");
	} else {
		title = e_string_dup(window->title);
	}
	FUNC_END("ewl_window_get_title");
	return title;
}

void	ewl_window_set_class_hints(EwlWidget *widget, 
                                  char *name, char *klass)
{
	EwlWindow	*win = (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_class_hints");
	if (!win)	{
		ewl_debug("ewl_window_set_class_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		if (win->name_hint) free(win->name_hint);
		win->name_hint  = name;
		if (win->class_hint) free(win->class_hint);
		win->class_hint = klass;
	}
	FUNC_END("ewl_window_set_class_hint");
	return;
}

void	ewl_window_set_decoration_hint(EwlWidget *widget, 
											EwlBool decoration_hint)
{
	EwlWindow	*win = (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_decoration_hint");
	if (!win)	{
		ewl_debug("ewl_window_set_decoration_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		win->decoration_hint	=	decoration_hint; /*?decoration_hint:TRUE; */
	}
	FUNC_END("ewl_window_set_decoration_hint");
	return;
}


char    *ewl_window_get_name_hint(EwlWidget *widget)
{
	char      *hint = NULL;
	EwlWindow *window = (EwlWindow*) widget;
	FUNC_BGN("ewl_window_get_name_hint");
	if (!window) {
		ewl_debug("ewl_window_get_name_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		hint = e_string_dup(window->name_hint);
	}
	FUNC_END("ewl_window_get_name_hint");
	return hint;
}

char    *ewl_window_get_class_hint(EwlWidget *widget)
{
	char      *hint = NULL;
	EwlWindow *window = (EwlWindow*) widget;
	FUNC_BGN("ewl_window_get_class_hint");
	if (!window) {
		ewl_debug("ewl_window_get_class_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		hint = e_string_dup(window->class_hint);
	}
	FUNC_END("ewl_window_get_class_hint");
	return hint;
}

EwlBool  ewl_window_get_decoration_hint(EwlWidget *widget)
{
	EwlBool hint = FALSE;
	EwlWindow *window = (EwlWindow*) widget;
	FUNC_BGN("ewl_window_get_decoration_hint");
	if (!window) {
		ewl_debug("ewl_window_get_decoration_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		hint = window->decoration_hint;
	}
	FUNC_END("ewl_window_get_decoration_hint");
	return hint;
}

void	ewl_window_move(EwlWidget *widget, int x, int y)
{
	FUNC_BGN("ewl_window_move");
	fprintf(stderr,"ewl_window_move: widget=0x%08x, x = %8d, y = %8d\n",
		(unsigned int) widget, x, y);
	ewl_widget_move(widget,x,y);
	FUNC_END("ewl_window_move");
	return;
}

void	ewl_window_resize(EwlWidget *widget, int w, int h)
{
	FUNC_BGN("ewl_window_resize");
	fprintf(stderr,"ewl_window_resize: widget=0x%08x, x = %8d, y = %8d\n",
		(unsigned int) widget, h, w);
	ewl_widget_resize(widget,(w>-1)?(w):1,(h>-1)?(h):1);
	FUNC_END("ewl_window_resize");
	return;
}

void	ewl_window_moveresize(EwlWidget *widget, int x, int y, int w, int h)
{
	FUNC_BGN("ewl_window_moveresize");
	fprintf(stderr,"ewl_window_moveresize: widget=0x%08x, x = %8d, y = %8d\n",
		(unsigned int) widget, w, h);
	ewl_widget_moveresize(widget,x,y,(w>-1)?(w):0,(h>-1)?(h):0);
	FUNC_END("ewl_window_moveresize");
	return;
}

