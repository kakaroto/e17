#include "translators.h"

EwlEventTranslator *ewl_translator_new(EwlTranslatorCallback  cb, 
                                       char                  *desc, 
                                       EwlEventType           type)
{
	EwlEventTranslator *et = NULL;
	if (!cb)	{
		ewl_debug("ewl_event_translator_new", EWL_NULL_ERROR, "cb");
	} else {
		et = malloc(sizeof(EwlEventTranslator));
		if (!et)	{
			ewl_debug("ewl_evnt_translator_new", EWL_NULL_ERROR, " et");
		} else {
			et->cb = cb;
			et->type = type;
			if (ewl_debug_is_active())
				fprintf(stderr,"ewl_translator_new(): adding translator for %s (type %d).\n", desc, type);
		}
	}
	return et;
}

EwlEvent *cb_ewl_event_keypress_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget *w = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_keypress_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	} else if (!xev) {
		ewl_debug("cb_ewl_event_keypress_translate", EWL_NULL_ERROR, "xev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else 
		((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xkey.window);
	return (EwlEvent*) eev;
}

EwlEvent *cb_ewl_event_keyrelease_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget *w = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_keyrelease_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	} else if (!xev) {
		ewl_debug("cb_ewl_event_keyrelease_translate", EWL_NULL_ERROR, "xev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else 
		((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xkey.window);
	return (EwlEvent*) eev;
}

EwlEvent *cb_ewl_event_buttonpress_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget         *w   = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_buttonpress_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else 
		((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xbutton.window);
	((EwlEventMousedown*)eev)->x = xev->xbutton.x;
	((EwlEventMousedown*)eev)->y = xev->xbutton.y;
	
	return (EwlEvent*)eev;
}

EwlEvent *cb_ewl_event_buttonrelease_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget         *w   = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_buttonrelease_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else
		((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xbutton.window);
	((EwlEventMouseup*)eev)->x = xev->xbutton.x;
	((EwlEventMouseup*)eev)->y = xev->xbutton.y;
	
	return (EwlEvent*)eev;
}

EwlEvent *cb_ewl_event_motion_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget         *w   = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_motion_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else
		((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xmotion.window);
	((EwlEventMousemove*)eev)->x = xev->xmotion.x;
	((EwlEventMousemove*)eev)->y = xev->xmotion.y;
	
	return (EwlEvent*)eev;
}

EwlEvent *cb_ewl_event_enter_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget         *w   = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_enter_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xcrossing.window);
	return eev;
}

EwlEvent *cb_ewl_event_leave_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	EwlWidget         *w   = ewl_widget_grabbed_get();
	if (!eev)	{
		ewl_debug("cb_ewl_event_leave_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	if (w)
		((EwlEvent*)eev)->widget = w;
	else
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xcrossing.window);
	return eev;
}

EwlEvent *cb_ewl_event_focusin_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_focusin_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xfocus.window);
	return eev;
}

EwlEvent *cb_ewl_event_focusout_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_focusout_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xfocus.window);
	return eev;
}

EwlEvent *cb_ewl_event_expose_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_expose_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xexpose.window);
	((EwlEventExpose*)eev)->rect = ewl_rect_new_with_values(
	                                   &xev->xexpose.x,
	                                   &xev->xexpose.y,
	                                   &xev->xexpose.width,
	                                   &xev->xexpose.height);
	((EwlEventExpose*)eev)->count = xev->xexpose.count;
	return (EwlEvent*)eev;
}

EwlEvent *cb_ewl_event_visibility_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_visibility_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xexpose.window);
	return eev;
}

EwlEvent *cb_ewl_event_create_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_create_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xcreatewindow.window);
	return eev;
}

EwlEvent *cb_ewl_event_destroy_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_destroy_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xdestroywindow.window);
	return eev;
}

EwlEvent *cb_ewl_event_unmapnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_unmapnotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	fprintf(stderr,"unampnotify translator called.\n");
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xunmap.window);
	return eev;
}

EwlEvent *cb_ewl_event_mapnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_mapnotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	fprintf(stderr,"mampnotify translator called.\n");
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xmap.window);
	return eev;
}

EwlEvent *cb_ewl_event_maprequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_maprequest_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	fprintf(stderr,"maprequest translator called.\n");
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xmaprequest.window);
	return eev;
}

EwlEvent *cb_ewl_event_reparentnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_reparentnotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xreparent.window);
	return eev;
}

EwlEvent *cb_ewl_event_configurenotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_configurenotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xconfigure.window);
	((EwlEventConfigure*)eev)->width = xev->xconfigure.width;
	((EwlEventConfigure*)eev)->height = xev->xconfigure.height;
	return eev;
}

EwlEvent *cb_ewl_event_configurerequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_configurenotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xconfigurerequest.window);
	return eev;
}

EwlEvent *cb_ewl_event_circulatenotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_circulatenotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xcirculate.window);
	return eev;
}

EwlEvent *cb_ewl_event_circulaterequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_circulaterequest_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xcirculaterequest.window);
	return eev;
}

EwlEvent *cb_ewl_event_property_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_property_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xproperty.window);
	return eev;
}

EwlEvent *cb_ewl_event_colormap_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_colormap_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xcolormap.window);
	return eev;
}

EwlEvent *cb_ewl_event_clientmessage_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_clientmessage_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xkey.window);
	return eev;
}

EwlEvent *cb_ewl_event_selectionnotify_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_selectionnotify_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xkey.window);
	return eev;
}

EwlEvent *cb_ewl_event_selectionrequest_translate(EwlEventTranslator *t, XEvent *xev)
{
	EwlEvent  *eev = ewl_event_new_by_type(t->type);
	if (!eev)	{
		ewl_debug("cb_ewl_event_selectionrequest_translate", EWL_NULL_ERROR, "eev");
		return NULL;
	}
	((EwlEvent*)eev)->widget = ewl_window_find_by_xwin(xev->xkey.window);
	return eev;
}

