#include "translators.h"

EwlEvent           *ewl_translate(XEvent *xev)
{
	static char initialized = 0;
	static EwlEventTranslator *translators[LASTEvent];
	int    i = 0;
	if (!initialized)	{
		initialized++;
		for (i=0; i<LASTEvent; i++)
			translators[i] = NULL;

		translators[KeyPress] = ewl_translator_new(
	    	                       cb_ewl_event_keypress_translate,
		                           "keydown");
		translators[KeyRelease] = ewl_translator_new(
		                             cb_ewl_event_keyrelease_translate,
		                             "keyup");
		translators[ButtonPress] = ewl_translator_new(
		                              cb_ewl_event_buttonpress_translate, 
		                              "mousedown");
		translators[ButtonRelease] = ewl_translator_new(
		                              cb_ewl_event_buttonrelease_translate, 
		                              "mouseup");
		translators[MotionNotify] = ewl_translator_new(
		                              cb_ewl_event_motion_translate, 
		                              "mousemove");
		translators[EnterNotify] = ewl_translator_new(
		                              cb_ewl_event_enter_translate, 
		                              "enter");
		translators[LeaveNotify] = ewl_translator_new(
		                              cb_ewl_event_leave_translate, 
		                              "leave");
		translators[FocusIn] = ewl_translator_new(
		                              cb_ewl_event_focusin_translate, 
		                              "focusin");
		translators[FocusOut] = ewl_translator_new(
		                              cb_ewl_event_focusout_translate, 
		                              "focusout");
		translators[Expose] = ewl_translator_new(
	   		                           cb_ewl_event_expose_translate, 
	   		                           "expose");
		translators[VisibilityNotify] = ewl_translator_new(
		   	                           cb_ewl_event_visibility_translate, 
		   	                           "visibility");
			translators[CreateNotify] = ewl_translator_new(
		                              cb_ewl_event_create_translate, 
		                              "create");
		translators[DestroyNotify] = ewl_translator_new(
		                              cb_ewl_event_destroy_translate, 
		                              "destroy");
		translators[UnmapNotify] = ewl_translator_new(
		                              cb_ewl_event_unmapnotify_translate, 
		                              "hide");
		translators[MapNotify] = ewl_translator_new(
		                              cb_ewl_event_mapnotify_translate, 
		                              "show");
		translators[MapRequest] = ewl_translator_new(
		                              cb_ewl_event_maprequest_translate, 
		                              "showrequest");
		translators[ReparentNotify] = ewl_translator_new(
		                              cb_ewl_event_reparentnotify_translate, 
		                              "reparent");
		translators[ConfigureNotify] = ewl_translator_new(
		                              cb_ewl_event_configurenotify_translate, 
		                              "configure");
		translators[ConfigureRequest] = ewl_translator_new(
		                              cb_ewl_event_configurerequest_translate, 
		                              "configurerequest");
		translators[CirculateNotify] = ewl_translator_new(
		                              cb_ewl_event_circulatenotify_translate, 
		                              "circulate");
		translators[CirculateRequest] = ewl_translator_new(
		                              cb_ewl_event_circulaterequest_translate, 
		                              "circulate");
		translators[PropertyNotify] = ewl_translator_new(
		                              cb_ewl_event_property_translate, 
		                              "property");
		translators[ColormapNotify] = ewl_translator_new(
		                              cb_ewl_event_colormap_translate, 
		                              "colormap");
		translators[ClientMessage] = ewl_translator_new(
		                              cb_ewl_event_clientmessage_translate, 
		                              "client");
		translators[SelectionNotify] = ewl_translator_new(
		                              cb_ewl_event_selectionnotify_translate, 
		                              "selectionnotify");
		translators[SelectionRequest] = ewl_translator_new(
		                              cb_ewl_event_selectionrequest_translate, 
		                              "selection");
	}

	if (translators[xev->type])
		return translators[xev->type]->cb(translators[xev->type], xev);

	return NULL;
}

EwlEventTranslator *ewl_translator_new(EwlTranslatorCallback  cb, 
                                       char                  *type)
{
	EwlEventTranslator *et = malloc(sizeof(EwlEventTranslator));
	et->cb = cb;
	et->type = ewl_string_dup(type);
	return et;
}

EwlEvent *cb_ewl_event_keypress_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_get_grabbed();
	if (!w)	w = ewl_window_find_by_xwin(xev->xkey.window);
	ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_keyrelease_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_get_grabbed();
	if (!w)	w = ewl_window_find_by_xwin(xev->xkey.window);
	ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_buttonpress_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_get_grabbed();
	int       *i = NULL; 

	if (!w)	w = ewl_window_find_by_xwin(xev->xbutton.window);
	ev = ewl_event_new(t->type,w);
	i = malloc(sizeof(int)); *i = xev->xbutton.x;
	ewl_event_set_data(ev, "x", i);
	i = malloc(sizeof(int)); *i = xev->xbutton.y;
	ewl_event_set_data(ev, "y", i);
	i = malloc(sizeof(int)); *i = xev->xbutton.button;
	ewl_event_set_data(ev, "button", i);
	
	return ev;
}

EwlEvent *cb_ewl_event_buttonrelease_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_get_grabbed();
	int       *i = NULL; 

	if (!w)	w = ewl_window_find_by_xwin(xev->xbutton.window);
	ev = ewl_event_new(t->type,w);
	i = malloc(sizeof(int)); *i = xev->xbutton.x;
	ewl_event_set_data(ev, "x", i);
	i = malloc(sizeof(int)); *i = xev->xbutton.y;
	ewl_event_set_data(ev, "y", i);
	i = malloc(sizeof(int)); *i = xev->xbutton.button;
	ewl_event_set_data(ev, "button", i);
	
	return ev;
}

EwlEvent *cb_ewl_event_motion_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_get_grabbed();
	int       *i = NULL; 

	if (!w)	w = ewl_window_find_by_xwin(xev->xmotion.window);
	ev = ewl_event_new(t->type,w);
	i = malloc(sizeof(int)); *i = xev->xmotion.x;
	ewl_event_set_data(ev, "x", i);
	i = malloc(sizeof(int)); *i = xev->xmotion.y;
	ewl_event_set_data(ev, "y", i);
	
	return ev;
}

EwlEvent *cb_ewl_event_enter_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_window_find_by_xwin(xev->xcrossing.window);
	ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_leave_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *ev = NULL;
	EwlWidget *w = ewl_window_find_by_xwin(xev->xcrossing.window);
	ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_focusin_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xfocus.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_focusout_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xfocus.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_expose_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xkey.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	int       *i = NULL;
	EwlRect   *rect = ewl_rect_new_with_values(xev->xexpose.x, xev->xexpose.y, xev->xexpose.width, xev->xexpose.height);
	/*i = malloc(sizeof(int)); *i = xev->xexpose.x;
	ewl_event_set_data(ev, "x", i);
	i = malloc(sizeof(int)); *i = xev->xexpose.y;
	ewl_event_set_data(ev, "y", i);
	i = malloc(sizeof(int)); *i = xev->xexpose.y;*/
	ewl_event_set_data(ev, "rect", rect);
	i = malloc(sizeof(int)); *i = xev->xexpose.count;
	ewl_event_set_data(ev, "count", i);
	
	return ev;
}

EwlEvent *cb_ewl_event_visibility_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xexpose.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_create_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xcreatewindow.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_destroy_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xdestroywindow.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_unmapnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xunmap.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_mapnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xmap.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_maprequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xmaprequest.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_reparentnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xreparent.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_configurenotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xconfigure.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	int       *i = malloc(sizeof(int)); *i = xev->xconfigure.width;
	ewl_event_set_data(ev, "width", i);
	i = malloc(sizeof(int)); *i = xev->xconfigure.height;
	ewl_event_set_data(ev, "height", i);
	return ev;
}

EwlEvent *cb_ewl_event_configurerequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xconfigurerequest.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_circulatenotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xcirculate.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_circulaterequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xcirculate.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_property_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xproperty.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_colormap_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xcolormap.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_clientmessage_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xkey.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_selectionnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xkey.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

EwlEvent *cb_ewl_event_selectionrequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlWidget *w = ewl_window_find_by_xwin(xev->xkey.window);
	EwlEvent  *ev = ewl_event_new(t->type,w);
	return ev;
}

