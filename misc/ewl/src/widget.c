#include "widget.h"

/* WIDGET GRAB/FOCUS FUNCTIONS */
EwlWidget    *ewl_get_grabbed()
{
	return EWL_WIDGET(ewl_get(ewl_get_state(), "/state/grabbed"));
	
}

void         ewl_grab(EwlWidget *widget)
{
	EwlWidget *old = ewl_get(ewl_get_state(), "/state/grabbed");
	if (old)	{
		ewl_widget_set_flag(old,"grabbed", FALSE);
		ewl_event_queue_new("ungrab", old);
	}
	ewl_set(ewl_get_state(), "/state/grabbed", widget);
	ewl_widget_set_flag(widget,"grabbed", TRUE);
	ewl_event_queue_new("grab", widget);
	return;
}


EwlWidget   *ewl_get_focused()
{
	return EWL_WIDGET(ewl_get(ewl_get_state(), "/state/focused"));
	
}

void         ewl_focus(EwlWidget *widget)
{
	EwlWidget *old = ewl_get(ewl_get_state(), "/state/focused");
	if (old)	{
		ewl_widget_set_flag(old,"focused", FALSE);
		ewl_event_queue_new("unfocus", old);
	}
	ewl_set(ewl_get_state(), "/state/focused", widget);
	ewl_widget_set_flag(widget,"focused", TRUE);
	ewl_event_queue_new("focus", widget);
	return;
}

/* WIDGET NEW/FREE FUNCTIONS */
EwlWidget   *ewl_widget_new()
{
	EwlWidget *widget = malloc(sizeof(EwlWidget));
	ewl_widget_init(widget);
	return widget;
}

void        ewl_widget_init(EwlWidget *widget)
{
	ewl_object_init(EWL_OBJECT(widget));
	ewl_object_set_type(widget, "EwlWidget");
	/*ewl_widget_set_flag(widget, "realized", FALSE);
	ewl_widget_set_flag(widget, "visible", FALSE);*/

	ewl_handler_set(widget,"/widget/show", ewl_widget_show_handler);
	ewl_handler_set(widget,"/widget/hide", ewl_widget_hide_handler);

	ewl_callback_add(widget, "realize", ewl_widget_realize_callback, NULL);
	ewl_callback_push(widget, "unrealize", ewl_widget_unrealize_callback, NULL);

	return;
}

void        ewl_widget_free(EwlWidget *widget)
{
	ewl_unref(widget);
	return;
}

/* WIDGET REALIZE/UNREALIZE FUNCTIONS */
void        ewl_widget_realize(EwlWidget *widget)
{
	ewl_event_queue_new("realize", widget);
	return;
}

void        ewl_widget_unrealize(EwlWidget *widget)
{
	ewl_event_queue_new("unrealize", widget);
	return;
}

void        ewl_widget_realize_callback(void     *object,
                                        EwlEvent *event,
                                        void     *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(event);
	UNUSED(data);

	ewl_widget_set_flag(widget, "realized", TRUE);
	ewl_widget_get_theme(widget, "EwlWidget");

	if (ewl_widget_is_visible(widget))
		ewl_widget_show(widget);
	return;
}

void        ewl_widget_unrealize_callback(void     *object,
                                          EwlEvent *event,
                                          void     *data)
{
	UNUSED(event); UNUSED(data);
	ewl_widget_set_flag(EWL_WIDGET(object),"realized", FALSE);

	return;
}

/* WIDGET SHOW/HIDE FUNCTIONS */
void        ewl_widget_show(EwlWidget *widget)
{
	EwlHandler handler = ewl_handler_get(widget, "/widget/show");
	EwlHash *params;
	if (handler)	{
		params = ewl_hash_new();
		ewl_hash_set(params, "widget", widget);
		handler(widget,"/widget/show", params);
		ewl_hash_free(params);
	}
}

void        ewl_widget_show_handler(void    *object,
                                    char    *type,
                                    EwlHash *params)
{
	EwlWidget *widget = EWL_WIDGET(ewl_hash_get(params,"widget"));
	UNUSED(object); UNUSED(type);

	ewl_widget_set_flag(widget, "visible", TRUE);
	if (ewl_widget_is_realized(widget))	{
		evas_show(ewl_widget_get_evas(widget),
           ewl_widget_get_background(widget));
		ewl_widget_set_flag(widget,"mapped", TRUE);
	} else {
		ewl_widget_realize(widget);
	}
	ewl_event_queue_new("show", widget);
	return;
}

void        ewl_widget_hide(EwlWidget *widget)
{
	EwlHandler handler = ewl_handler_get(widget, "/widget/hide");
	EwlHash *params;
	if (handler)	{
		params = ewl_hash_new();
		ewl_hash_set(params, "widget", widget);
		handler(widget,"/widget/hide", params);
		ewl_hash_free(params);
	}
}

void        ewl_widget_hide_handler(void    *object,
                                    char    *type,
                                    EwlHash *params)
{
	EwlWidget *widget = EWL_WIDGET(ewl_hash_get(params,"widget"));
	UNUSED(object); UNUSED(type);

	ewl_widget_set_flag(widget, "visible", FALSE);
	if (ewl_widget_is_realized(widget))	{
		evas_hide(ewl_widget_get_evas(widget),
		          ewl_widget_get_background(widget));
		ewl_widget_set_flag(widget,"mapped", FALSE);
	} else {
	}
	ewl_event_queue_new("hide", widget);
	return;
}

void        ewl_widget_resize(EwlWidget *widget)
{
	EwlHandler handler = ewl_handler_get(widget, "/widget/resize");
	EwlHash *params;
	if (handler)	{
		params = ewl_hash_new();
		ewl_hash_set(params, "widget", widget);
		handler(widget,"/widget/resize", params);
		ewl_hash_free(params);
	}
}

void        ewl_widget_resize_handler(void    *object,
                                    char    *type,
                                    EwlHash *params)
{
	EwlWidget *widget = EWL_WIDGET(ewl_hash_get(params,"widget"));
	EwlRect   *rect = ewl_widget_get_rect(widget),
	          *req  = ewl_widget_get_requested_rect(widget),
	          *min  = ewl_widget_get_min_rect(widget),
	          *max  = ewl_widget_get_max_rect(widget);
	int        layer = 0;

	UNUSED(object);
	UNUSED(type);
	UNUSED(min);
	UNUSED(max);

	if (RECT_COMPARE(rect,req))	{
		ewl_widget_set_requested_rect(widget,NULL);
		return;
	}

	if (ewl_widget_is_realized(widget) && ewl_widget_is_visible(widget)) {
		ewl_widget_set_rect(widget,req);
		ewl_widget_set_requested_rect(widget,NULL);

		evas_resize(ewl_widget_get_evas(widget),
		          ewl_widget_get_background(widget), rect->w, rect->h);
		if (!ewl_widget_get_flag(widget, "tiled_background")) {
			evas_set_image_fill(ewl_widget_get_evas(widget),
			                    ewl_widget_get_background(widget),
		                        0, 0, rect->w, rect->h);
		}
		layer = ewl_widget_get_layer(ewl_widget_get_parent(widget)) + 100;
		evas_set_layer(ewl_widget_get_evas(widget),
		               ewl_widget_get_background(widget), layer);
	}
	
	ewl_event_queue_new("resize", widget);
	return;
}

/* WIDGET RECT/PADDING FUNCTIONS */

void        ewl_widget_get_requisition_handler(void    *object,
                                               char    *type,
                                               EwlHash *params)
{
	EwlWidget      *widget = EWL_WIDGET(object);
	EwlRequisition *r;
	EwlRect        *req = ewl_widget_get_requested_rect(widget),
	               *min = ewl_widget_get_min_rect(widget),
	               *max = ewl_widget_get_max_rect(widget);
	int             min_w = min?min->w:0,
	                min_h = min?min->h:0,
	                max_w = max?max->w:0,
	                max_h = max?max->h:0;
	UNUSED(type);
	r = ewl_requisition_new_with_values(req, min_w, min_h, max_w, max_h);
	ewl_hash_set(params, "requisition", r);
	return;
}

EwlRequisition *ewl_widget_get_requisition(EwlWidget *widget)
{
	EwlHandler  handler = ewl_handler_get(widget, "/widget/get_requisition");
	EwlHash    *params;
	EwlRequisition *req = NULL;
	if (handler) {
		params = ewl_hash_new();
		ewl_hash_set(params, "widget", widget);
		handler(widget, "/widget/get_requisition", params);
		req = ewl_hash_get(params, "requisition");
		ewl_hash_free(params);
	} else {
		req = ewl_requisition_new_with_values(0, 0, 0, 0, 0);
	}
	return req;
}

EwlRect    *ewl_widget_get_rect(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/rect");
}

void        ewl_widget_set_rect(EwlWidget *widget, EwlRect *rect)
{
	ewl_rect_free(ewl_widget_get_rect(widget));
	ewl_set(widget, "/widget/rect", rect);
	return;
}


EwlRect    *ewl_widget_get_requested_rect(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/req_rect");
}

void        ewl_widget_set_requested_rect(EwlWidget *widget, EwlRect *rect)
{
	ewl_rect_free(ewl_widget_get_requested_rect(widget));
	ewl_set(widget, "/widget/req_rect", rect);
	return;
}


EwlRect    *ewl_widget_get_min_rect(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/min_rect");
}

void        ewl_widget_set_min_rect(EwlWidget *widget, EwlRect *rect)
{
	ewl_rect_free(ewl_widget_get_min_rect(widget));
	ewl_set(widget, "/widget/min_rect", rect);
	return;
}


EwlRect    *ewl_widget_get_max_rect(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/max_rect");
}

void        ewl_widget_set_max_rect(EwlWidget *widget, EwlRect *rect)
{
	ewl_rect_free(ewl_widget_get_max_rect(widget));
	ewl_set(widget, "/widget/max_rect", rect);
	return;
}

EwlRect    *ewl_widget_get_resize_rect(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/resize_rect");
}

void        ewl_widget_set_resize_rect(EwlWidget *widget, EwlRect *rect)
{
	ewl_rect_free(ewl_widget_get_resize_rect(widget));
	ewl_set(widget, "/widget/resize_rect", rect);
	return;
}


EwlRect    *ewl_widget_get_previous_rect(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/previous_rect");
}

void        ewl_widget_set_previous_rect(EwlWidget *widget, EwlRect *rect)
{
	ewl_rect_free(ewl_widget_get_previous_rect(widget));
	ewl_set(widget, "/widget/previous_rect", rect);
	return;
}


/* WIDGET PADDING FUNCTIONS */
int        *ewl_widget_get_padding(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/padding");
}

void        ewl_widget_set_padding(EwlWidget *widget,
                                   int l, int t, int b, int r)
{
	int *old, *pad = malloc(sizeof(int)*4);
	pad[0] = l; pad[1] = t; pad[2] = b; pad[3] = r;
	old = ewl_get(widget, "/widget/padding");
	if (old) free(old);
	ewl_set(widget, "/widget/padding", pad);
	return;
}

/* WIDGET FLAG FUNCTIONS */
char        ewl_widget_is_realized(EwlWidget *widget)
{
	return ewl_widget_get_flag(widget, "realized");
}

char        ewl_widget_is_visible(EwlWidget *widget)
{
	return ewl_widget_get_flag(widget, "visible");
}

char        ewl_widget_is_focused(EwlWidget *widget)
{
	return ewl_widget_get_flag(widget, "focused");
}

char        ewl_widget_is_grabbed(EwlWidget *widget)
{
	return ewl_widget_get_flag(widget, "grabbed");
}

char        ewl_widget_is_container(EwlWidget *widget)
{
	return ewl_widget_get_flag(widget, "is_container");
}

char        ewl_widget_can_resize(EwlWidget *widget)
{
	return ewl_widget_get_flag(widget, "can_resize");
}


void        ewl_widget_set_flag(EwlWidget *widget, char *flag, char value)
{
	char *f = malloc(1),
	      temp[256] = "";
	*f = value;
	sprintf(temp, "/widget/flags/%s", flag);
	ewl_set(widget, temp, f);
	return;
}

char        ewl_widget_get_flag(EwlWidget *widget, char *flag)
{
	char *f, temp[256] = "";
	sprintf(temp, "/widget/flags/%s", flag);
	f = ewl_get(widget, temp);
	return f?*f:0;
}

/* WIDGET EVAS/EVAS_OBJECT FUNCTIONS */
Evas        ewl_widget_get_evas(EwlWidget *widget)
{
	EwlWidget *tw;
	if (!ewl_widget_is_realized(widget))
       return NULL;
	for (tw = widget;
	     ewl_get(tw, "/widget/parent");
	     tw = ewl_get(tw, "/widget/parent"));
	return ewl_window_get_evas(tw);
}

void        ewl_widget_set_background(EwlWidget   *widget,
                                      Evas_Object  evas_object,
                                      char         tiled)
{
	if (ewl_widget_is_realized(widget)&&widget->background)	{
		evas_del_object(ewl_widget_get_evas(widget),
		                ewl_widget_get_background(widget));
	}
	widget->background = evas_object;
	if (ewl_widget_is_realized(widget) && 
	    (ewl_callback_find(widget, "mousedown") ||
	     ewl_callback_find(widget, "mousemove") ||
	     ewl_callback_find(widget, "mouseup")) )  {
		evas_set_pass_events(ewl_widget_get_evas(widget), evas_object, FALSE);
		if (ewl_widget_get_flag(widget, "mapped"))
			evas_show(ewl_widget_get_evas(widget), evas_object);
	} else if (ewl_widget_is_realized(widget)) {
		evas_set_pass_events(ewl_widget_get_evas(widget), evas_object, TRUE);
	}

	ewl_widget_set_flag(widget, "tiled_background", tiled);
	/* FIXME -- add evas config stuff here */

	return;
}

Evas_Object ewl_widget_get_background(EwlWidget *widget)
{
	return widget->background;
}


int         ewl_widget_get_layer(EwlWidget *widget)
{
	int *layer = 0;
	if (ewl_widget_is_realized(widget))	{
		layer = ewl_get(widget, "/widget/layer");
	}
	return layer?*layer:0;
}

void        ewl_widget_set_layer(EwlWidget *widget, 
                                 int        stacking_layer)
{
	EwlEvent *ev;
	int *ev_l = malloc(sizeof(int)),
	    *ob_l = malloc(sizeof(int));
	if (ewl_widget_is_realized(widget))	{
		evas_set_layer(ewl_widget_get_evas(widget),
		               ewl_widget_get_background(widget),
		               stacking_layer);
		*ev_l = stacking_layer; *ob_l = stacking_layer;
		ev = ewl_event_new("relayer", widget);
		ewl_event_set_data(ev, "layer", ev_l);
		ewl_set(widget, "/widget/layer", ob_l);
		ewl_event_queue(ev);
	}
	return;
}


EwlWidget  *ewl_widget_get_parent(EwlWidget *widget)
{
	return ewl_get(widget, "/widget/parent");
}

void        ewl_widget_set_parent(EwlWidget *widget, EwlWidget *parent)
{
	ewl_set(widget, "/widget/parent", parent);
	return;
}
