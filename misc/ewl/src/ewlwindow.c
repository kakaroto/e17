#include "ewlwindow.h"

EwlWidget   *ewl_window_new(EwlWinType	type)
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

EwlWidget   *ewl_window_new_with_values(EwlWinType type, char *title, 
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
		/* Call this to actually do XCreateWindow */
		if (!window->xwin)
		{
			ewl_window_realize(widget);
		}
		/*fprintf(stderr,"_cb_ewl_win_show called.\n");*/
		break;
	case EWL_EVENT_HIDE:
		ewl_widget_set_flag(widget, VISIBLE, FALSE);
//		XUnmapWindow(s->disp, window->xwin);
		ewl_window_unrealize(widget);
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

void         ewl_window_init(EwlWindow *win, EwlWinType type,
							 char *title, int w, int h)
{
	EwlWidget            *widget = (EwlWidget*) win;
	EwlContainer         *container = (EwlContainer*) win;
	EwlState             *s = ewl_state_get();
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

	/* Lets setup some properties. */
	win->type	= type;
	win->title	= title?title:EWL_DEFAULT_WINDOW_TITLE;
	win->x		= EWL_DEFAULT_WINDOW_X;
	win->y		= EWL_DEFAULT_WINDOW_Y;
	win->w		= w;
	win->h		= h;
	win->name	= EWL_DEFAULT_WINDOW_NAME_H;
	win->class 	= EWL_DEFAULT_WINDOW_CLASS_H;

	win->decor_hint = TRUE;

	win->screen = ScreenOfDisplay(s->disp, DefaultScreen(s->disp));
	win->vis = DefaultVisual(s->disp, DefaultScreen(s->disp));
	win->depth = DefaultDepth(s->disp, DefaultScreen(s->disp));
	win->cm = DefaultColormap(s->disp,DefaultScreen(s->disp));
	win->root = RootWindow(s->disp,DefaultScreen(s->disp));
	/*wid->root = DefaultRootWindow(s->disp);*/
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

	win->title	=	title?title:EWL_DEFAULT_WINDOW_TITLE;

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

void	ewl_window_realize(EwlWidget *widget)
{
	EwlState	*s		= ewl_state_get();
	EwlWindow	*win	= (EwlWindow *) widget;
	Atom				  wmhints;

	FUNC_BGN("ewl_window_new_from_properties");
	if (!s)	{
		ewl_debug("ewl_window_new_from_properties", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_new_from_properties", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		fprintf(stdout, "Realizing window 0x%08x\n", (unsigned int)win);
		win->xwin = XCreateWindow(s->disp, win->root, win->x, win->y, 
						win->w, win->h, 0, win->depth,
		            	InputOutput, win->vis,
		            	CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		            	CWColormap | CWBackPixel | CWBorderPixel |CWEventMask,
		            	&win->attr);
		
		/* setting title */
		XStoreName(s->disp, win->xwin, win->title);
		
		/* setting class hint */
 		win->class_hint = XAllocClassHint();
		win->class_hint->res_name = win->name;
		win->class_hint->res_class = win->class;
		XSetClassHint(s->disp, win->xwin, win->class_hint);
		XFree(win->class_hint);
		
		/* Setting up decor */
		if (win->decor_hint == TRUE) {
			fprintf(stdout, "We want decor!\n");
		}
		else if (win->decor_hint == FALSE) {
			fprintf(stdout, "Ixnay on the decor-ay!\n");
			wmhints = XInternAtom(s->disp, "_MOTIF_WM_HINTS", True);
			if (wmhints != None) {
				MWMHints decorhints = { MWM_HINTS_DECORATIONS, 0, 0, 0, 0 };
				XChangeProperty(s->disp, win->xwin, wmhints, wmhints, 32,
					PropModeReplace, (unsigned char *)&decorhints,
					sizeof(MWMHints)/4);
			}
		}
		/* Map our window */
		XMapWindow(s->disp, win->xwin);
	}
	FUNC_END("ewl_window_new_from_properties");
	return;
}

void	ewl_window_unrealize(EwlWidget *widget)
{
	EwlState	*s		= ewl_state_get();
	EwlWindow	*win	= (EwlWindow *) widget;

	FUNC_BGN("ewl_window_unrealize");
	if (!s)	{
		ewl_debug("ewl_window_unrealize", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_unrealize", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		fprintf(stdout, "Unrealizing window 0x%08x\n", (unsigned int)win);
		XUnmapWindow(s->disp, win->xwin);
	}
	FUNC_END("ewl_window_unrealize");
	return;
}

void	ewl_window_set_property_title(EwlWidget *widget, char *title)
{
	EwlState	*s		= ewl_state_get();
	EwlWindow	*win	= (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_property_title");
	if (!s)	{
		ewl_debug("ewl_window_set_property_title", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_set_property_title", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		win->title	=	title?title:EWL_DEFAULT_WINDOW_TITLE;
	}
	FUNC_END("ewl_window_set_property_title");
	return;
}

void	ewl_window_set_property_location(EwlWidget *widget, int x, int y)
{
	EwlState	*s	= ewl_state_get();
	EwlWindow	*win	= (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_property_location");
	if (!s)	{
		ewl_debug("ewl_window_set_property_location", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_set_property_location", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {

		if (x==-1)
			x = EWL_DEFAULT_WINDOW_X;
		if (y==-1)
			y = EWL_DEFAULT_WINDOW_Y;
	
		win->x	=	x;
		win->y	=	y;

	}
	FUNC_END("ewl_window_set_property_location");
	return;
}

void	ewl_window_set_property_size(EwlWidget *widget, int w, int h)
{
	EwlState	*s	= ewl_state_get();
	EwlWindow	*win	= (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_property_size");
	if (!s)	{
		ewl_debug("ewl_window_set_property_size", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_set_property_size", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {

		if (w==-1)
			w = EWL_DEFAULT_WINDOW_X;
		if (h==-1)
			h = EWL_DEFAULT_WINDOW_Y;
	
		win->w	=	w;
		win->h	=	h;
	
	}
	FUNC_END("ewl_window_set_property_size");
	return;
}

void	ewl_window_set_property_class_hint(EwlWidget *widget, 
											char *name, char *class)
{
	EwlState	*s	= ewl_state_get();
	EwlWindow	*win = (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_property_class_hint");
	if (!s)	{
		ewl_debug("ewl_window_set_property_class_hint", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_set_property_class_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		win->name = name;
		win->class = class;
	}
	FUNC_END("ewl_window_set_property_class_hint");
	return;
}

void	ewl_window_set_property_decor_hint(EwlWidget *widget, 
											EwlBool decor_hint)
{
	EwlState	*s	= ewl_state_get();
	EwlWindow	*win = (EwlWindow *) widget;

	FUNC_BGN("ewl_window_set_property_decor_hint");
	if (!s)	{
		ewl_debug("ewl_window_set_property_decor_hint", EWL_NULL_ERROR, "s");
	} else if (!win)	{
		ewl_debug("ewl_window_set_property_decor_hint", EWL_NULL_WIDGET_ERROR,
		          "window");
	} else {
		win->decor_hint	=	decor_hint; /*?decor_hint:TRUE; */
	}
	FUNC_END("ewl_window_set_property_decor_hint");
	return;
}
