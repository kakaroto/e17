
#include "widget.h"

EwlWidget *ewl_widget_new()
{
	EwlWidget *w = NULL;
	FUNC_BGN("ewl_widget_new");
	w = malloc(sizeof(EwlWidget));
	if (!w)	{
		ewl_debug("ewl_widget_new", EWL_NULL_ERROR, "w");
	} else {
		ewl_widget_init(w);
	}
	FUNC_END("ewl_widget_new");
	return w;
}

EwlBool _cb_ewl_widget_event_handler(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
	EwlBool propagate = TRUE;

	switch (ev->type)	{
	case EWL_EVENT_RESIZE:
		/*if (ewl_debug_is_active())*/
			fprintf(stderr,"caught EWL_EVENT_RESIZE for widget %08x.\n",
				(unsigned int) w);
		ewl_widget_set_flag(w, NEEDS_RESIZE, 1);
		break;
	default:
		break;
	}

	return propagate;
}

void       ewl_widget_init(EwlWidget *w)
{
	int   t   = 0;
	FUNC_BGN("ewl_widget_init");
	if (!w)	{
		ewl_debug("ewl_widget_init",EWL_NULL_ERROR,"w");
		FUNC_END("ewl_widget_init");
		return;
	}

	w->name = 0;
	w->type = EWL_WIDGET;
	w->parent = NULL;
	w->root = 0;
	w->flags = 0;
	ewl_widget_set_flag(w, NEEDS_RESIZE, TRUE);
	ewl_widget_set_flag(w, NEEDS_REFRESH, TRUE);
	ewl_widget_set_flag(w, CAN_RESIZE, TRUE);

	/*ewl_widget_set_rect(w,&t,&t,&t,&t);*/
	ewl_widget_set_padding(w,&t,&t,&t,&t);
	w->layers=  0;
	w->layout = ewl_layout_new();
	w->rlayout = ewl_rlayout_new();

	w->event_callbacks = 0;
	w->render = NULL;
	/*w->rendered = NULL;*/
	w->bg = NULL;

	ewl_callback_add(w, EWL_EVENT_REALIZE,
	                   ewl_widget_handle_realize, NULL);
	ewl_callback_add(w, EWL_EVENT_UNREALIZE,
	                 ewl_widget_handle_unrealize, NULL);
	/*ewl_callback_add(w, EWL_EVENT_RESIZE,
	                   _cb_ewl_widget_event_handler, NULL);*/
	ewl_widget_add(w);

	FUNC_END("ewl_widget_init");
	return;
}

void       ewl_widget_free(EwlWidget *w)
{
	FUNC_BGN("ewl_widget_free");
	if (!w)	{
		ewl_debug("ewl_widget_free",EWL_NULL_ERROR,"w");
	} else {
		free(w);
		w = NULL;
	}
	FUNC_END("ewl_widget_free");
	return;
	
}

void             ewl_widget_realize(EwlWidget *widget)
{
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_widget_realize");
	if (!widget) {
		ewl_debug("ewl_widget_realize", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		ev = ewl_event_new_by_type_with_widget(EWL_EVENT_REALIZE, widget);
		ewl_event_queue(ev);
	}
	FUNC_END("ewl_widget_realize");
	return;
}

void             ewl_widget_unrealize(EwlWidget *widget)
{
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_widget_unrealize");
	if (!widget) {
		ewl_debug("ewl_widget_unrealize", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		ev = ewl_event_new_by_type_with_widget(EWL_EVENT_UNREALIZE, widget);
		ewl_event_queue(ev);
	}
	FUNC_END("ewl_widget_unrealize");
	return;
}

void       ewl_widget_set_flags(EwlWidget *w, EwlFlag f)
{
	FUNC_BGN("ewl_widget_set_flags");
	if (!w)	{
		ewl_debug("ewl_widget_set_flags",EWL_NULL_ERROR,"w");
	} else {
		w->flags = f;
	}
	FUNC_END("ewl_widget_set_flags");
	return;
}


void       ewl_widget_set_flag(EwlWidget *w, EwlFlag f, EwlBool v)
{
	FUNC_BGN("ewl_widget_set_flag");
	if (!w)	{
		ewl_debug("ewl_widget_set_flag",EWL_NULL_ERROR,"w");
	} else {
		if (v)	{
			w->flags |= f;
		} else if (w->flags|f)	{
			w->flags ^= f;
		}
	}
	FUNC_END("ewl_widget_set_flag");
	return;
}

EwlBool    ewl_widget_get_flag(EwlWidget *w, EwlFlag f)
{
	EwlFlag r = 0;
	FUNC_BGN("ewl_widget_get_flag");
	if (!w)	{
		ewl_debug("ewl_widget_get_flag",EWL_NULL_ERROR,"w");
	} else {
		r = w->flags & f;
	}
	FUNC_END("ewl_widget_get_flag");
	return r;
}


void       ewl_widget_set_state(EwlWidget *w, EwlWidgetState state)
{
	FUNC_BGN("ewl_widget_set_state");
	if (!w)	{
		ewl_debug("ewl_widget_set_state", EWL_NULL_WIDGET_ERROR, "w");
	} else if (state >= EWL_STATE_LAST)	{
		ewl_debug("ewl_widget_set_state", EWL_OUT_OF_BOUNDS_ERROR, "state");
	} else {
		w->state = state;
	}
	FUNC_END("ewl_widget_set_state");
	return;
}

EwlWidgetState   ewl_widget_get_state(EwlWidget *w)
{
	EwlWidgetState r = 0;
	FUNC_BGN("ewl_widget_get_state");
	if (!w)	{
		ewl_debug("ewl_widget_get_state", EWL_NULL_WIDGET_ERROR, "w");
	} else {
		r = w->state;
	}
	FUNC_BGN("ewl_widget_get_state");
	return r;
}


void       ewl_widget_set_type(EwlWidget *w, EwlWidgetType type)
{
	FUNC_BGN("ewl_widget_set_type");
	if (!w)	{
		ewl_debug("ewl_widget_set_type", EWL_NULL_WIDGET_ERROR, "w");
	} else if (type >= EWL_TYPE_LAST)	{
		ewl_debug("ewl_widget_set_type", EWL_OUT_OF_BOUNDS_ERROR, "type");
	} else {
		w->type = type;
	}
	FUNC_END("ewl_widget_set_type");
	return;
}

EwlWidgetType   ewl_widget_get_type(EwlWidget *w)
{
	EwlWidgetType r = 0;
	FUNC_BGN("ewl_widget_get_type");
	if (!w)	{
		ewl_debug("ewl_widget_get_type", EWL_NULL_WIDGET_ERROR, "w");
	} else {
		r = w->type;
	}
	FUNC_BGN("ewl_widget_get_type");
	return r;
}

char            *ewl_widget_get_type_string(EwlWidget *w)
{
	char *typestr = NULL;
	FUNC_BGN("ewl_widget_get_type_string");
	if (!w)	{
		ewl_debug("ewl_widget_get_type_string", EWL_NULL_WIDGET_ERROR, "w");
	} else if (w->type<0||w->type>=EWL_TYPE_LAST) {
		ewl_debug("ewl_widget_get_type_string", EWL_OUT_OF_BOUNDS_ERROR,
		          "w->type");
	} else {
		/* defined in types.h */
		typestr = e_string_dup(_EwlWidgetTypeStringEnum[w->type]);
	}
	FUNC_BGN("ewl_widget_get_type_string");
	return typestr;
}


EwlBool    ewl_widget_is_realized(EwlWidget *widget)
{
	EwlBool r = FALSE;
	FUNC_BGN("ewl_widget_is_realized");
	if (!widget)	{
		ewl_debug("ewl_widget_is_realized",EWL_NULL_ERROR,"w");
	} else {
		r = ewl_widget_get_flag(widget,REALIZED);
	}
	FUNC_END("ewl_widget_is_realized");
	return r;
}

EwlBool    ewl_widget_is_visible(EwlWidget *w)
{
	EwlBool r = 0;
	FUNC_BGN("ewl_widget_is_visible");
	if (!w)	{
		ewl_debug("ewl_widget_is_visible",EWL_NULL_ERROR,"w");
	} else {
		r = ewl_widget_get_flag(w,VISIBLE);
	}
	FUNC_END("ewl_widget_is_visible");
	return r;
}

EwlBool    ewl_widget_needs_resize(EwlWidget *w)
{
	EwlBool r = 0;
	FUNC_BGN("ewl_widget_needs_resize");
	if (!w)	{
		ewl_debug("ewl_widget_needs_resize",EWL_NULL_ERROR,"w");
	} else {
		r = ewl_widget_get_flag(w,NEEDS_RESIZE);
	}
	FUNC_END("ewl_widget_needs_resize");
	return r;
}

void       ewl_widget_set_needs_resize(EwlWidget *w)
{
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_widget_set_needs_resize");
	if (!w) {
		ewl_debug("ewl_widget_set_needs_resize", EWL_NULL_WIDGET_ERROR,"w");
	} else if (ewl_widget_get_flag(w,VISIBLE) &&
	           ewl_widget_get_flag(w,CAN_RESIZE) &&
	           !ewl_widget_get_flag(w,NEEDS_RESIZE)) {
		ewl_widget_set_flag(w,NEEDS_RESIZE,TRUE);
		if (!ewl_widget_get_flag(w,DONT_PROPAGATE_RESIZE) && w->parent) {
			ewl_widget_set_needs_resize(w->parent);
		} else if (!w->parent) {
			ev = ewl_event_new_by_type_with_widget(EWL_EVENT_CONFIGURE,w);
			ewl_event_queue(ev);
		}
	}
	FUNC_END("ewl_widget_set_needs_resize");
	return;
}

EwlBool    ewl_widget_needs_refresh(EwlWidget *w)
{
	EwlBool r = 0;
	FUNC_BGN("ewl_widget_needs_refresh");
	if (!w)	{
		ewl_debug("ewl_widget_needs_refresh",EWL_NULL_ERROR,"w");
	} else {
		r = ewl_widget_get_flag(w,NEEDS_REFRESH);
	}
	FUNC_END("ewl_widget_needs_refresh");
	return r;
}

void       ewl_widget_set_needs_refresh(EwlWidget *w)
{
	EwlEventExpose *ev = NULL;
	FUNC_BGN("ewl_widget_set_needs_refresh");
	if (!w) {
		ewl_debug("ewl_widget_set_needs_refresh", EWL_NULL_WIDGET_ERROR,"w");
	} else if (ewl_widget_get_flag(w, VISIBLE) &&
	           !ewl_widget_get_flag(w,NEEDS_REFRESH)) {
		ewl_widget_set_flag(w,NEEDS_REFRESH,TRUE);
		if (!ewl_widget_get_flag(w,DONT_PROPAGATE_REFRESH) && w->parent) {
			ewl_widget_set_needs_refresh(w->parent);
		} else if (!w->parent) {
			ev = (EwlEventExpose*) ewl_event_new_by_type_with_widget(
			                                         EWL_EVENT_EXPOSE,w);
			
			if (!ev) {
				ewl_debug("ewl_widget_set_needs_refresh",EWL_NULL_ERROR,"ev");
			} else {
				ev->rect = ewl_widget_get_rect(w);
				ewl_event_queue((EwlEvent*)ev);
			}
		}
	}
	FUNC_END("ewl_widget_set_needs_refresh");
	return;
}


EwlBool    ewl_widget_can_resize(EwlWidget *w)
{
	EwlBool r = 0;
	FUNC_BGN("ewl_widget_can_resize");
	if (!w)	{
		ewl_debug("ewl_widget_can_resize",EWL_NULL_ERROR,"w");
	} else {
		r = ewl_widget_get_flag(w,CAN_RESIZE);
	}
	FUNC_END("ewl_widget_can_resize");
	return r;
}

void             ewl_widget_moveresize(EwlWidget *widget,
                                       int x, int y, int w, int h)
{
	FUNC_BGN("ewl_widget_moveresize");
	ewl_widget_set_rect(widget,&x,&y,(w>-1)?&w:0,(h>-1)?&h:0);
	FUNC_END("ewl_widget_moveresize");
	return;
}

void             ewl_widget_move(EwlWidget *widget, int x, int y)
{
	FUNC_BGN("ewl_widget_move");
	ewl_widget_set_rect(widget,&x,&y,0,0);
	FUNC_END("ewl_widget_move");
	return;
}

void             ewl_widget_resize(EwlWidget *widget, int w, int h)
{
	FUNC_BGN("ewl_widget_resize");
	ewl_widget_set_rect(widget,0,0,&w,&h);
	FUNC_END("ewl_widget_resize");
	return;
}

EwlRect         *ewl_widget_get_rect(EwlWidget *widget)
{
	EwlRect *rect = NULL;
	FUNC_BGN("ewl_widget_get_rect");
	if (!widget) {
		ewl_debug("ewl_widget_get_rect", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		rect = ewl_layout_get_rect(widget->layout);
	}
	FUNC_END("ewl_widget_get_rect");
	return rect;
}


void             ewl_widget_set_rect(EwlWidget *widget,
                                     int *x, int *y, int *w, int *h)
{
	EwlRect  *rect = NULL;
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_widget_set_rect");
	if (!widget) {
		ewl_debug("ewl_widget_set_rect", EWL_NULL_WIDGET_ERROR, "widget");
	} else if (!ewl_widget_can_resize(widget)) {
		ewl_debug("ewl_widget_set_rect", EWL_GENERIC_ERROR,
		          "widget cannot be resized");
	} else {
		rect = ewl_layout_get_req_rect(widget->layout);
		ewl_rect_set(rect,x,y,w,h);
		ewl_layout_set_req_rect(widget->layout,
		                        rect);
		if (x||y) {
			ewl_widget_set_needs_refresh(widget);
			ev = ewl_event_new_by_type_with_widget(EWL_EVENT_MOVE, widget);
			ewl_event_queue(ev);
		}
		if (w||h) {
			ewl_widget_set_needs_resize(widget);
			ev = ewl_event_new_by_type_with_widget(EWL_EVENT_RESIZE,widget);
			ewl_event_queue(ev);
		}
	}
	FUNC_END("ewl_widget_set_rect");
	return;
}


/*static char _cb_exec_ewhe(EwlLL *node, EwlData *data)
{
	if (node&&((EwlEvCbLL*)node)->cb)
		((EwlEvCbLL*)node)->cb((EwlWidget*)((EwlEvent*)data)->widget,
		                       ((EwlEvent*)data), 
		                       ((EwlEvent*)data)->data);
	return 1;
}*/

static EwlBool _cb_ewl_find_event_handler(EwlLL *node, EwlData *data)
{
	EwlEvCbLL    *handler_list = (EwlEvCbLL*) node;
	EwlEventType  type = (EwlEventType) data;
	return  (handler_list->type==type);
	/*return (((EwlEvCbLL*)node)->type==((EwlEventType)data));*/
}


void       ewl_callback_add(EwlWidget *w, EwlEventType t,
	                        EwlBool  (*cb)(EwlWidget *widget,
	                                       EwlEvent   *ev,
	                                       EwlData    *data),
                            EwlData  *data)
{
	FUNC_BGN("ewl_callback_add");
	ewl_widget_callback_add(w,t,cb,data);
	FUNC_END("ewl_callback_add");
	return;
}

void       ewl_widget_callback_add(EwlWidget *w,
                                   EwlEventType t, 
	                               EwlBool (*cb)(EwlWidget *widget,
	                                             EwlEvent  *ev,
	                                             EwlData   *data),
                                   EwlData *data)
{
	EwlEvCbLL *l = NULL, *l2 = NULL;
	FUNC_BGN("ewl_widget_callback_add");
	if (!w)	{
		ewl_debug("ewl_widget_callback_add", EWL_NULL_ERROR, "w");
		FUNC_END("ewl_widget_callback_add");
		return;
	}

	l = (EwlEvCbLL*)ewl_ll_callback_find((EwlLL*)w->event_callbacks,
		                                 _cb_ewl_find_event_handler,
	                                     (EwlData*)t);
	if (l)	{
		/* other callbacks of this event type exist -- chain it to them */
		l2 = malloc(sizeof(EwlEvCbLL));
		if (!l2)	{
			ewl_debug("ewl_widget_callback_add", EWL_NULL_ERROR, "l2");
		} else {
			l2->ll.data = NULL;
			l2->ll.next = NULL;
			l2->type = t;
			l2->cb = cb;
			l2->data = data;
			l2->next = NULL;
			/*l = (EwlEvCbLL*) ewl_ll_insert((EwlLL*)l, (EwlLL*)l2);*/
			for (;l->next;l=l->next);
			l->next = l2;
		}
	} else {
		/* no other callbacks of this type -- add a new event "mask" */
		l = malloc(sizeof(EwlEvCbLL));
		if (!l)	{
			ewl_debug("ewl_widget_callback_add", EWL_NULL_ERROR, "l");
		} else {
			l->ll.data = NULL;
			l->ll.next = NULL;
			l->type = t;
			l->cb = cb;
			l->data = data;
			l->next = NULL;
			w->event_callbacks = (EwlEvCbLL*) ewl_ll_insert(
			                                  (EwlLL*) w->event_callbacks,
			                                  (EwlLL*) l);
		}
	}

	FUNC_END("ewl_widget_callback_add");
	return;
}

EwlBool  ewl_widget_handle_event(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
	EwlEvCbLL *l = NULL;
	EwlBool    r = FALSE,
	           cont = FALSE;
	FUNC_BGN("ewl_widget_handle_event");
	if (!w)	{
		ewl_debug("ewl_widget_handle_event", EWL_NULL_WIDGET_ERROR, "w");
	} else if (!ev) {
		ewl_debug("ewl_widget_handle_event", EWL_NULL_ERROR, "ev");
	} else {
		l = (EwlEvCbLL*)ewl_ll_callback_find((EwlLL*)w->event_callbacks,
		                                     _cb_ewl_find_event_handler,
		                                     (EwlData*) (ev->type));
		if (l)	{
			cont = TRUE;
			/*ev->widget = (EwlData*)w; ev->data = d;*/
			for (;l;l=l->next)
				if (cont&&((EwlEvCbLL*)l)->cb)
					cont = ((EwlEvCbLL*)l)->cb(w,ev,d);
			r = TRUE;
			/*ewl_ll_foreach((EwlLL*)l->next, _cb_exec_ewhe, (EwlData*)ev);*/
		}
	}

	FUNC_END("ewl_widget_handle_event");
	return r;
}

static EwlBool _cb_ewl_widget_render_layers(EwlLL *node, EwlData *data)
{
	int         width, height, ix, iy;
	EwlLL      *ll = NULL;
	EwlImLayer *l = (EwlImLayer*) node;
	EwlWidget  *w = (EwlWidget*) data;
	EwlImage   *im = NULL;

	if (ewl_debug_is_active())	{
		fprintf(stderr,"rendering <EwlImLayer %08x, \"%s\">\n",
		        (unsigned int) l, l->name?l->name:"Untitled Layer");
	}

	if (l->visible&&l->rect)	{
		/* this should be ntested in a nother callback, but i'm a
		   lazy fucker, and i don't want to declare a container struct or
		   make anything glboal.. so i'll do it this way...  */
		for (ll=(EwlLL*)l->images; ll; ll=ll->next)	{
			im = (EwlImage*) ll;
			if (im->visible&&im->rect&&im->im)	{
				width = ewl_imlib_image_get_width(im->im);
				height = ewl_imlib_image_get_height(im->im);
				ix = l->rect->x + im->rect->x;
				iy = l->rect->y + im->rect->y;
				ewl_imlib_blend_image_onto_image(
					w->rendered,im->im,
					l->render_alpha,
					0, 0, width, height,
					ix,
					iy,
					im->rect->w,
					im->rect->h,
					0.0,
					ewl_render_dithered_get(),
					1,
					ewl_render_antialiased_get());
			} else if (!im->rect)	{
				ewl_debug("_cb_ewl_widget_render_layers",
				          EWL_NULL_ERROR, "im->rect");
			} else if (!im->im)	{
				ewl_debug("_cb_ewl_widget_render_layers",
				          EWL_NULL_ERROR, "im->rect");
			}
		}
	}
	return 1;
}

Evas             ewl_widget_get_evas(EwlWidget *widget)
{
	EwlWidget *w = NULL;
	Evas evas = NULL;
	FUNC_BGN("ewl_widget_get_evas");
	if (!widget)	{
		ewl_debug("ewl_widget_get_evas", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		for (w=widget;w->parent;w=w->parent);
		evas = ewl_window_get_evas(w);
	}
	FUNC_END("ewl_widget_get_evas");
	return evas;
}

void   ewl_widget_render(EwlWidget *w)
{
	int width  = 0,
	    height = 0;
	/*unsigned int data = 0xff000000;*/
	FUNC_BGN("ewl_widget_render");
	if (!w)	{
		ewl_debug("ewl_widget_render",EWL_NULL_WIDGET_ERROR, "w");
	} else if (!ewl_widget_get_flag(w,VISIBLE))	{
		/* doesn't need refresh */
	} else if (!ewl_widget_get_flag(w,NEEDS_RESIZE)&&
	           !ewl_widget_get_flag(w,NEEDS_REFRESH))	{
		/* doesn't need refresh */
		fprintf(stderr,"widget 0x%08x doesn't need a resize or a refresh.\n",
		        (unsigned int) w);
	} else if (!w->render)	{
		/* no render callback */
		if (w->rendered)
			ewl_imlib_free_image(w->rendered);
		if (ewl_debug_is_active())
			fprintf(stderr, "ewl_widget_render(): old w->rendered = %08x\n",
			        (unsigned int) w->rendered);
		/*w->rendered = ewl_imlib_clone_image(w->bg);*/
		if (!w->bg)	{
			/* FIX ME */
			w->bg = imlib_create_image(1,1);
			imlib_context_set_image(w->bg);
			imlib_image_set_has_alpha(TRUE);
			ewl_imlib_image_fill_rectangle(w->bg,0, 0, 1, 1, 0, 0, 0, 0);
		}
		width = ewl_imlib_image_get_width(w->bg);
		height = ewl_imlib_image_get_height(w->bg);
		w->rendered = ewl_imlib_create_cropped_scaled_image(
		                        w->bg,
		                        0, 0,
		                        width, height,
		                        w->layout->rect->w, w->layout->rect->h,
		                        ewl_render_antialiased_get());
		if (ewl_debug_is_active())
			fprintf(stderr, "ewl_widget_render(): new w->rendered = %08x\n",
			        (unsigned int) w->rendered);
		ewl_ll_foreach((EwlLL*)w->layers,
		               _cb_ewl_widget_render_layers,
		               (EwlData*) w);
		if (ewl_widget_get_flag(w,HAS_CHILDREN))	{
			ewl_container_render_children(w);
		}
		ewl_widget_set_flag(w, NEEDS_RESIZE,  FALSE);
		ewl_widget_set_flag(w, NEEDS_REFRESH, FALSE);
	} else {
		/* is visible, needs a refresh, and has a render callback */
		w->render(w,NULL);
	}
	FUNC_END("ewl_widget_render");
	return;
}

void   ewl_widget_render_onto_parent(EwlWidget *w)
{
	int       width=0, height=0;
	char      blend_onto_parent = 0;
	FUNC_BGN("ewl_widget_render_onto_parent");
	if (!w)	{
		ewl_debug("ewl_widget_render_onto_parent",EWL_NULL_WIDGET_ERROR, "w");
	} else if (!ewl_widget_get_flag(w,NEEDS_REFRESH)&&
	           !ewl_widget_get_flag(w,NEEDS_RESIZE))	{
		/* doesn't need refresh, copy existing rendered image */
		blend_onto_parent++;
	} else {
		ewl_widget_render(w);
		blend_onto_parent++;
		/*if (w->parent)
			ewl_imlib_simple_image_blend_onto_image(w->parent->rendered,
			                                        w->rendered)*/
	}

	if (w->parent&&blend_onto_parent)	{
		width = ewl_imlib_image_get_width(w->rendered);
		height = ewl_imlib_image_get_width(w->rendered);
		ewl_imlib_blend_image_onto_image(w->parent->rendered,w->rendered,
		                                 TRUE
		                 /*ewl_widget_get_flag(w,RENDER_ALPHA)*/ ,
		                                 0, 0, width, height,
		                                 ewl_layout_get_x(w->layout) + 
		                                 w->padding[EWL_PAD_LEFT],
		                                 ewl_layout_get_y(w->layout) +
		                                 w->padding[EWL_PAD_TOP],
		                                 ewl_layout_get_w(w->layout),
		                                 ewl_layout_get_h(w->layout),
		                                 0.0,
		                                 ewl_render_dithered_get(), 1,
		                                 ewl_render_antialiased_get());
	}

	FUNC_END("ewl_widget_render_onto_parent");
	return;
}


int             *ewl_widget_get_padding(EwlWidget *w)
{
	int    i=0, *pad = NULL;
	FUNC_BGN("ewl_widget_get_padding");
	if (!w)	{
		ewl_debug("ewl_widget_get_padding", EWL_NULL_WIDGET_ERROR, "w");
	} else {
		pad = malloc(sizeof(int)*4);
		if (!pad)	{
			ewl_debug("ewl_widget_get_padding", EWL_NULL_ERROR, "pad");
		} else {
			for (i=0; i<4; i++)
				pad[i] = w->padding[i];
		}
	}
	FUNC_END("ewl_widget_set_padding");
	return pad;
}

void             ewl_widget_set_padding(EwlWidget *w, int *left, int *top,
                                        int *right, int *bottom)
{
	FUNC_BGN("ewl_widget_set_padding");
	if (!w)	{
		ewl_debug("ewl_widget_set_padding", EWL_NULL_WIDGET_ERROR, "w");
	} else {
		if (left)   w->padding[EWL_PAD_LEFT]   = *left;
		if (top)    w->padding[EWL_PAD_TOP]    = *top;
		if (right)  w->padding[EWL_PAD_RIGHT]  = *right;
		if (bottom) w->padding[EWL_PAD_BOTTOM] = *bottom;
	}
	FUNC_END("ewl_widget_set_padding");
	return;
}


void             ewl_widget_set_background(EwlWidget *w, Evas_Object im)
{
	FUNC_BGN("ewl_widget_set_background");
	if (!w)	{
		ewl_debug("ewl_widget_set_background", EWL_NULL_WIDGET_ERROR, "w");
	} else if (!im)	{
		ewl_debug("ewl_widget_set_background", EWL_NULL_ERROR, "im");
	} else {
		if (w->bg)
			evas_del_object(ewl_widget_get_evas(w), w->bg);
		w->bg = im;
		/* set stuff here */
	}
	FUNC_END("ewl_widget_set_background");
	return;
}

Evas_Object ewl_widget_get_background(EwlWidget *widget)
{
	Evas_Object obj;
	FUNC_BGN("ewl_widget_get_background");
	if (!widget)	{
		ewl_debug("ewl_widget_get_background",
		          EWL_NULL_WIDGET_ERROR, "widget");
	} else if (!ewl_widget_get_flag(widget,REALIZED)) {
		ewl_debug("ewl_widget_get_background",
		          EWL_GENERIC_ERROR, "Widget is not realized.");
	} else {
	 obj = widget->bg;
	}
	FUNC_END("ewl_widget_get_background");
	return obj;
}

void       ewl_widget_imlayer_insert(EwlWidget *w, EwlImLayer *l)
{
	FUNC_BGN("ewl_widget_imlayer_insert");
	if (!w)	{
		ewl_debug("ewl_widget_imlayer_insert", EWL_NULL_WIDGET_ERROR, "w");
	} else if (!l)	{
		ewl_debug("ewl_widget_imlayer_insert", EWL_NULL_ERROR, "l");
	} else {
		w->layers = (EwlImLayer*) ewl_ll_insert((EwlLL*) w->layers,
		                                        (EwlLL*) l);
	}
	FUNC_END("ewl_widget_imlayer_insert");
	return;
}

void       ewl_widget_imlayer_remove(EwlWidget *w, EwlImLayer *l)
{
	FUNC_BGN("ewl_widget_imlayer_remove");
	if (!w)	{
		ewl_debug("ewl_widget_imlayer_remove", EWL_NULL_WIDGET_ERROR, "w");
	} else if (!l)	{
		ewl_debug("ewl_widget_imlayer_remove", EWL_NULL_ERROR, "l");
	} else {
		w->layers = (EwlImLayer*) ewl_ll_remove((EwlLL*) w->layers,
		                                        (EwlLL*) l);
	}
	FUNC_END("ewl_widget_imlayer_remove");
	return;
}

void       ewl_widget_imlayer_push(EwlWidget *w, EwlImLayer *l)
{
	FUNC_BGN("ewl_widget_imlayer_push");
	if (!w)	{
		ewl_debug("ewl_widget_imlayer_push", EWL_NULL_WIDGET_ERROR, "w");
	} else if (!l)	{
		ewl_debug("ewl_widget_imlayer_push", EWL_NULL_ERROR, "l");
	} else {
		w->layers = (EwlImLayer*) ewl_ll_push((EwlLL*) w->layers,
		                                        (EwlLL*) l);
	}
	FUNC_END("ewl_widget_imlayer_push");
	return;
}

EwlImLayer *ewl_widget_imlayer_pop(EwlWidget *w)
{
	EwlImLayer *l = NULL;
	FUNC_BGN("ewl_widget_imlayer_pop");
	if (!w)	{
		ewl_debug("ewl_widget_imlayer_pop", EWL_NULL_WIDGET_ERROR, "w");
	} else {
		l = (EwlImLayer*) ewl_ll_pop((EwlLL*) w->layers);
		if (!l)	{
			ewl_debug("ewl_widget_imlayer_pop", EWL_NULL_ERROR, "l");
		}
	}
	FUNC_END("ewl_widget_imlayer_pop");
	return l;
}

static EwlBool _cb_ewl_widget_imlayer_foreach(EwlLL *node, EwlData *data)
{
	EwlImLayerForeachData *foreach_data = (EwlImLayerForeachData*) data;
	return foreach_data->cb((EwlImLayer*) node, foreach_data->data);
}

void       ewl_widget_imlayer_foreach(EwlWidget *widget,
                                      EwlBool  (*cb)(EwlImLayer *layer,
                                                     EwlData    *data),
                                      EwlData   *data)
{
	EwlImLayerForeachData foreach_data;
	FUNC_BGN("ewl_widget_imlayer_foreach");
	if (!widget) {
		ewl_debug("ewl_widget_imlayer_foreach", EWL_NULL_WIDGET_ERROR,
		          "widget");
	} else if (!cb) {
		ewl_debug("ewl_widget_imlayer_foreach", EWL_NULL_ERROR, "cb");
	} else {
		foreach_data.cb = cb;
		foreach_data.data = data;
		ewl_ll_foreach((EwlLL*)widget->layers,
		               _cb_ewl_widget_imlayer_foreach,
		               (EwlData*) &foreach_data);
	}
	FUNC_END("ewl_widget_imlayer_foreach");
	return;
}

EwlBool _ewl_widget_get_imlayer_by_name_cb(EwlLL *node, EwlData *data)
{
	EwlImLayer *layer = (EwlImLayer*) node;
	EwlBool     r     = FALSE;
	char       *name  = (char*) data;
	int         len   = layer->name?strlen(layer->name):0;
	if (!strncmp(layer->name,name,len))
		r = TRUE;
	return r;
}

EwlImLayer      *ewl_widget_get_imlayer_by_name(EwlWidget *widget, char *name)
{
	EwlImLayer *layer = NULL;
	FUNC_BGN("ewl_widget_get_imlayer_by_name");
	if (!widget) {
		ewl_debug("ewl_widget_get_imlayer_by_name",
		          EWL_NULL_WIDGET_ERROR,
		          "widget");
	} else if (!name) {
		ewl_debug("ewl_widget_get_imlayer_by_name",
		          EWL_NULL_ERROR,
		          "name");
	} else {
		layer = ewl_ll_callback_find((EwlLL*) (widget->layers), 
		                             _ewl_widget_get_imlayer_by_name_cb,
		                             (EwlData*) name);
	}
	FUNC_END("ewl_widget_get_imlayer_by_name");
	return layer;
}

void             ewl_widget_imlayer_show(EwlWidget *widget, char *name)
{
	EwlImLayer *layer = NULL;
	FUNC_BGN("ewl_widget_imlayer_show");
	if (!widget) {
		ewl_debug("ewl_widget_imlayer_show",
		          EWL_NULL_WIDGET_ERROR,
		          "widget");
	} else if (!name) {
		ewl_debug("ewl_widget_imlayer_show",
		          EWL_NULL_ERROR,
		          "name");
	} else {
		layer = ewl_widget_get_imlayer_by_name(widget,name);
		if (!layer) {
			ewl_debug("ewl_widget_imlayer_show",
			          EWL_NULL_ERROR,
			          "layer");
		} else {
			ewl_imlayer_show(layer);
			ewl_widget_set_needs_refresh(widget);
		}
	}
	FUNC_END("ewl_widget_imlayer_show");
	return;
}

EwlBool          ewl_widget_handle_realize(EwlWidget *widget,
                                           EwlEvent  *ev,
                                           EwlData   *data)
{
	FUNC_BGN("ewl_widget_handle_unrealize");

	ewl_widget_get_theme(widget,"/EwlWidget");

	FUNC_END("ewl_widget_handle_unrealize");
	return TRUE;
}

EwlBool          ewl_widget_handle_unrealize(EwlWidget *widget,
                                             EwlEvent  *ev,
                                             EwlData   *data)
{
	FUNC_BGN("ewl_widget_handle_unrealize");
	FUNC_END("ewl_widget_handle_unrealize");
	return TRUE;
}

void             ewl_widget_imlayer_hide(EwlWidget *widget, char *name)
{
	EwlImLayer *layer = NULL;
	FUNC_BGN("ewl_widget_imlayer_hide");
	if (!widget) {
		ewl_debug("ewl_widget_imlayer_hide",
		          EWL_NULL_WIDGET_ERROR,
		          "widget");
	} else if (!name) {
		ewl_debug("ewl_widget_imlayer_hide",
		          EWL_NULL_ERROR,
		          "name");
	} else {
		layer = ewl_widget_get_imlayer_by_name(widget,name);
		if (!layer) {
			ewl_debug("ewl_widget_imlayer_hide",
			          EWL_NULL_ERROR,
			          "layer");
		} else {
			ewl_imlayer_hide(layer);
			ewl_widget_set_needs_refresh(widget);
		}
	}
	FUNC_END("ewl_widget_imlayer_hide");
	return;
}



static EwlBool _cb_ewd(EwlLL *ll, EwlData *d)
{
	EwlEvCbLL *evll = (EwlEvCbLL*)ll;
	fprintf(stderr,"type: %d\n", evll->type);
	fprintf(stderr,"cb:   %08x\n", (unsigned int) evll->cb);
	fprintf(stderr,"data: %08x\n", (unsigned int) evll->data);
	fprintf(stderr,"next: %08x\n", (unsigned int) evll->next);
	evll=evll->next;
	if (evll)	{
		for(;evll;evll=evll->next)	{
			fprintf(stderr,"    type: %d\n", evll->type);
			fprintf(stderr,"    cb:   %08x\n", (unsigned int) evll->cb);
			fprintf(stderr,"    data: %08x\n", (unsigned int) evll->data);
			fprintf(stderr,"    next: %08x\n", (unsigned int) evll->next);
		}
	}
	return 1;
}

void      ewl_widget_dump(EwlWidget *w)
{
	FUNC_BGN("ewl_widget_dump");
	if (!w)	{
		ewl_debug("ewl_widget_dump", EWL_NULL_WIDGET_ERROR, "w");
		return;
	}
	ewl_ll_foreach((EwlLL*)w->event_callbacks, _cb_ewd, NULL);
	FUNC_END("ewl_widget_dump");
	return;
}

