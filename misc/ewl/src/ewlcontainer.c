#include "ewlcontainer.h"

EwlWidget   *ewl_container_new()
{
	EwlContainer *c = malloc(sizeof(EwlContainer));
	FUNC_BGN("ewl_container_new");
	if (!c)	{
		ewl_debug("ewl_container_new", EWL_NULL_WIDGET_ERROR, "c");
	} else {
		ewl_container_init(c);
	}
	FUNC_END("ewl_container_new");
	return (EwlWidget*)c;
}


void         ewl_container_init(EwlContainer *c)
{
	EwlWidget *widget = (EwlWidget*) c;
	int        t = 0;
	FUNC_BGN("ewl_container_init");
	ewl_widget_init(widget);
	ewl_widget_set_type(widget,EWL_CONTAINER);
	ewl_widget_set_flag(widget,HAS_CHILDREN,TRUE);
	c->children = NULL;
	c->max_children = 0; /* no limit */
	c->is_full = 0;
	c->insert= NULL;
	c->remove = NULL;
	c->propagate_events = 1;
	c->propagate = NULL;
	c->resize_children = NULL;
	for (t=0;  t<4; c->child_padding[t++]=0);

	/* LOAD DB SHIT HERRE */
	ewl_widget_get_theme(widget,"/EwlContainer");
	if (ewl_theme_get_int("/EwlContainer/child_padding/left",   &t))
		ewl_container_set_child_padding(widget,&t,0,0,0);
	if (ewl_theme_get_int("/EwlContainer/child_padding/top",    &t))
		ewl_container_set_child_padding(widget,0,&t,0,0);
	if (ewl_theme_get_int("/EwlContainer/child_padding/right",  &t))
		ewl_container_set_child_padding(widget,0,0,&t,0);
	if (ewl_theme_get_int("/EwlContainer/child_padding/bottom", &t)) 
		ewl_container_set_child_padding(widget,0,0,0,&t);


	FUNC_END("ewl_container_init");
	return;
}

static char _cb_ewl_cont_free(EwlLL *ll, EwlData *d)
{
	ewl_widget_free((EwlWidget*)ll->data);
	return 1;
}

void         ewl_container_free(EwlWidget *c)
{
	FUNC_BGN("ewl_container_free");
	if (!c)	{
		ewl_debug("ewl_container_free", EWL_NULL_WIDGET_ERROR, "c");
	} else {
		ewl_container_foreach(c,_cb_ewl_cont_free, NULL);
		free((EwlContainer*)c);
		c = NULL;
	}
	FUNC_END("ewl_container_free");
}

void         ewl_container_insert(EwlWidget *container, EwlWidget *child)
{
	EwlContainer *c = (EwlContainer*) container;
	EwlEvent     *ev = NULL;
	FUNC_BGN("ewl_container_insert");
	if (!container)	{
		ewl_debug("ewl_container_insert", EWL_NULL_WIDGET_ERROR, "container");
	} else if (c->is_full)	{
		ewl_debug("ewl_container_insert", EWL_GENERIC_ERROR,
		          "full container in ewl_container_insert()");
	} else if (!child) {
		ewl_debug("ewl_container_insert", EWL_NULL_WIDGET_ERROR, "child");
	} else if (!c->insert) {
		if (ewl_debug_is_active())
			fprintf(stderr,"ewl_container_insert(): inserting widget 0x%08x.\n",
		    	    (unsigned int) child);
		c->children = ewl_ll_insert_with_data(c->children, (EwlData*) child);
		if (c->max_children&&(ewl_ll_sizeof(c->children)>=c->max_children))
			c->is_full = 1;
		child->parent = container;
		if (ewl_widget_get_flag(child,VISIBLE))	{
			ev = ewl_event_new_by_type(EWL_EVENT_RESIZE);
			if (!ev)	{
				ewl_debug("ewl_container_insert", EWL_NULL_ERROR, "ev");
			} else {
				ev->widget = container;
				ewl_event_queue(ev);
			}
		}
	} else {
		c->insert(c, child, NULL);
	}
	FUNC_END("ewl_container_insert");
	return;
}

void         ewl_container_insert_before(EwlWidget *container, EwlWidget *child)
{
	EwlContainer *c = (EwlContainer*) container;
	EwlEvent     *ev = NULL;
	FUNC_BGN("ewl_container_insert_before");
	if (!container)	{
		ewl_debug("ewl_container_insert_before", EWL_NULL_WIDGET_ERROR,
		          "container");
	} else if (c->is_full)	{
		ewl_debug("ewl_container_insert_before", EWL_GENERIC_ERROR,
		          "full container in ewl_container_insert_before()");
	} else if (!child) {
		ewl_debug("ewl_container_insert_before",EWL_NULL_WIDGET_ERROR,"child");
	} else if (!c->insert) {
		c->children = ewl_ll_push_with_data(c->children, (EwlData*) child);
		if (c->max_children&&(ewl_ll_sizeof(c->children)>=c->max_children))
			c->is_full = 1;
		child->parent = container;
		if (ewl_widget_get_flag(child,VISIBLE))	{
			ev = ewl_event_new_by_type(EWL_EVENT_RESIZE);
			if (!ev)	{
				ewl_debug("ewl_container_insert_before", EWL_NULL_ERROR, "ev");
			} else {
				ev->widget = container;
				ewl_event_queue(ev);
			}
		}
	} else {
		c->insert(c, child, NULL);
	}
	FUNC_END("ewl_container_insert_before");
	return;
}

void         ewl_container_remove(EwlWidget *container, EwlWidget *child)
{
	EwlContainer *c = (EwlContainer*)container;
	EwlEvent     *ev = NULL;
	FUNC_BGN("ewl_container_remove");
	if (!container)	{
		ewl_debug("ewl_container_remove", EWL_NULL_WIDGET_ERROR, "container");
	} else if (!child) {
		ewl_debug("ewl_container_remove", EWL_NULL_WIDGET_ERROR, "child");
	} else if (!c->remove) {
		c->children = ewl_ll_remove_by_data(c->children,
		                                    (EwlData*)child);
		if (c->max_children&&(ewl_ll_sizeof(c->children)<c->max_children))
			c->is_full = 0;
		child->parent = NULL;
		if (ewl_widget_get_flag(child,VISIBLE))	{
			ev = ewl_event_new_by_type(EWL_EVENT_RESIZE);
			if (!ev)	{
				ewl_debug("ewl_container_insert", EWL_NULL_ERROR, "ev");
			} else {
				ev->widget = container;
				ewl_event_queue(ev);
			}
		}
	} else {
		c->remove(c, child, NULL);
	}
	FUNC_END("ewl_container_remove");
}

EwlBool _cb_ewl_cont_ev_prop(EwlLL *node, EwlData *data)
{
	EwlWidget    *widget    = (EwlWidget*) node->data;
	/*EwlContainer *container = (EwlContainer*) widget->parent;*/
	EwlEvent     *ev        = (EwlEvent*)  data;
	int           x, y;
	char          r         = FALSE;

	/* magic test here */
	switch(ev->type)	{
	case EWL_EVENT_NONE:
	case EWL_EVENT_INIT:
	case EWL_EVENT_MEDIA:
	case EWL_EVENT_SHOW:
	case EWL_EVENT_HIDE:
	case EWL_EVENT_RESIZE:
	case EWL_EVENT_MOVE:
	case EWL_EVENT_KEYDOWN:
	case EWL_EVENT_KEYUP:
	case EWL_EVENT_ENTER:
	case EWL_EVENT_LEAVE:
	case EWL_EVENT_FOCUSIN:
	case EWL_EVENT_FOCUSOUT:
	case EWL_EVENT_EXPOSE:
	case EWL_EVENT_VISIBILITY:
	case EWL_EVENT_CREATE:
	case EWL_EVENT_DESTROY:
	case EWL_EVENT_REPARENT:
	case EWL_EVENT_CONFIGURE:
	case EWL_EVENT_CIRCULATE:
	case EWL_EVENT_PROPERTY:
	case EWL_EVENT_COLORMAP:
	case EWL_EVENT_CLIENT:
		/*ev->widget = w;*/
		r = FALSE;
		break;

	case EWL_EVENT_MOUSEDOWN:
		x = ((EwlEventMousedown*)ev)->x;
		y = ((EwlEventMousedown*)ev)->y;
		if (IN_RECT(widget->layout->rect,x,y))	{
			if (ewl_debug_is_active())
				fprintf(stderr,"propagating MOUSEDOWN event from widget "
				        "0x%08x to widget 0x%08x.\n",
				        (unsigned int) widget->parent,
				        (unsigned int) widget);
			ev->widget = widget;
			((EwlEventMousedown*)ev)->x -= widget->layout->rect->x;
			((EwlEventMousedown*)ev)->y -= widget->layout->rect->y;
			r = TRUE;
		} else {
			r = FALSE;
		}
		break;
	case EWL_EVENT_MOUSEMOVE:
		/* figure out mouse coords and prop event */
		x = ((EwlEventMousemove*)ev)->x;
		y = ((EwlEventMousemove*)ev)->y;
		if (ewl_debug_is_active())	{
			fprintf(stderr,"ev x %d, ev y = %d\n", x, y);
			ewl_rect_dump(widget->layout->rect);
		}
		if (IN_RECT(widget->layout->rect,x,y))	{
			if (ewl_debug_is_active())
				fprintf(stderr,"propagating MOUSEMOVE event from widget "
				        "0x%08x to widget 0x%08x.\n",
				        (unsigned int) widget->parent,
				        (unsigned int) widget);
			ev->widget = widget;
			((EwlEventMousemove*)ev)->x -= widget->layout->rect->x;
			((EwlEventMousemove*)ev)->y -= widget->layout->rect->y;
			r = TRUE;
		} else {
			r = FALSE;
		}
		break;
	case EWL_EVENT_MOUSEUP:
		/* figure out mouse grab and prop event */
		/* figure out mouse coords and prop event */
		x = ((EwlEventMouseup*)ev)->x;
		y = ((EwlEventMouseup*)ev)->y;
		if (IN_RECT(widget->layout->rect,x,y))	{
			if (ewl_debug_is_active())
				fprintf(stderr,"propagating MOUSEUP event from widget "
				        "0x%08x to widget 0x%08x.\n",
				        (unsigned int) widget->parent,
				        (unsigned int) widget);
			ev->widget = widget;
			((EwlEventMouseup*)ev)->x -= widget->layout->rect->x;
			((EwlEventMouseup*)ev)->y -= widget->layout->rect->y;
			/*ewl_event_propagate(ev);*/
			r = TRUE;
		} else {
			r = FALSE;
		}
		break;
	case EWL_EVENT_SELECTION:
		/*ev->widget = w;*/
		r = FALSE;
		break;
	default:
		ewl_debug("_cb_ewl_cont_ev_prop",
		          EWL_GENERIC_ERROR,
		          "unknown event type");
	}

	return r;
}

void         ewl_container_event_propagate(EwlWidget *container, EwlEvent *ev)
{
	EwlLL        *l = NULL;
	EwlContainer *c = (EwlContainer*) container;
	FUNC_BGN("ewl_container_event_propagate");
	if (!container)	{
		ewl_debug("ewl_container_event_propagate", EWL_NULL_WIDGET_ERROR, 
		          "container");
	} else if (!c->propagate_events)	{
		/* no error -- just dont' friggin propagate anything */
	} else if (!ev) {
		ewl_debug("ewl_container_event_propagate", EWL_NULL_ERROR, 
		          "ev");
	} else if (!c->propagate) {
		ev->widget = container;
		if (ewl_debug_is_active())
			fprintf(stderr,"ewl_container_event_propagate: widget = 0x%08x\n",
			        (unsigned int) ev->widget);
		l = ewl_ll_callback_find(c->children,
		                         _cb_ewl_cont_ev_prop,
		                         (EwlData*) ev);
		if (!l)	{
			ev->widget = container;
		} else {
			ev->widget = (EwlWidget*) l->data;
		}
	} else {
		c->propagate(c, ev);
	}
	FUNC_END("ewl_container_event_propagate");
	return;
}

void         ewl_container_foreach(EwlWidget    *container,
                                   EwlBool     (*cb)(EwlLL   *w,
                                                     EwlData *d),
                                   EwlData      *d)
{
	EwlContainer *c = (EwlContainer*) container;
	FUNC_BGN("ewl_container_foreach");
	if (!container)	{
		ewl_debug("ewl_container_foreach", EWL_NULL_WIDGET_ERROR, "container");
	} else if (!cb) {
		ewl_debug("ewl_container_foreach", EWL_NULL_WIDGET_ERROR, "cb");
	} else {
		ewl_ll_foreach(c->children, cb, d);
	}
	FUNC_END("ewl_container_foreach");
	return;
}

unsigned int ewl_container_get_max_children(EwlWidget *c)
{
	EwlContainer *container = (EwlContainer*) c;
	unsigned int  max = 0;
	FUNC_BGN("ewl_container_get_max_children");
	if (!container)	{
		ewl_debug("ewl_container_get_max_children",EWL_NULL_WIDGET_ERROR,"c");
	} else {
		max = container->max_children;
	}
	FUNC_END("ewl_container_get_max_children");
	return max;
}

void         ewl_container_set_max_children(EwlWidget *c, unsigned int max)
{
	EwlContainer *container = (EwlContainer*) c;
	FUNC_BGN("ewl_container_set_max_children");
	if (!c)	{
		ewl_debug("ewl_container_set_max_children",EWL_NULL_WIDGET_ERROR,"c");
	} else {
		container->max_children = max;
	}
	FUNC_END("ewl_container_set_max_children");
	return;
}


EwlBool      ewl_contaner_is_full(EwlWidget *c)
{
	EwlContainer *container = (EwlContainer*) c;
	EwlBool r = 0;
	FUNC_BGN("ewl_container_is_full");
	if (!c)	{
		ewl_debug("ewl_container_is_full", EWL_NULL_WIDGET_ERROR, "c");
	} else {
		r = (container->max_children)?container->is_full:0;
	}
	FUNC_END("ewl_container_is_full");
	return r;
}

EwlBool      ewl_container_get_propagate_events(EwlWidget *c)
{
	EwlContainer *container = (EwlContainer*) c;
	EwlBool r = 0;
	FUNC_BGN("ewl_container_get_propagate_events");
	if (!c)	{
		ewl_debug("ewl_container_get_propagate_events",
		          EWL_NULL_WIDGET_ERROR, "c");
	} else {
		r = container->propagate_events;
	}
	FUNC_END("ewl_container_get_propagate_events");
	return r;
}

void         ewl_container_set_propagate_events(EwlWidget *c, EwlBool v)
{
	EwlContainer *container = (EwlContainer*) c;
	FUNC_BGN("ewl_container_set_propagate_events");
	if (!c)	{
		ewl_debug("ewl_container_set_propagate_events",
		          EWL_NULL_WIDGET_ERROR, "c");
	} else {
		container->propagate_events = v;
	}
	FUNC_END("ewl_container_set_propagate_events");
	return;
}

EwlBool      _cb_ewl_container_render_children(EwlLL *node, EwlData *data)
{
	EwlWidget *widget = (EwlWidget*) node->data;
	ewl_widget_render_onto_parent(widget);
	return TRUE;
}

void         ewl_container_render_children(EwlWidget *c)
{
	FUNC_BGN("ewl_container_render_children");
	if (!c)	{
		ewl_debug("ewl_container_render_children",EWL_NULL_WIDGET_ERROR,"c");
	} else {
		ewl_container_foreach(c,_cb_ewl_container_render_children,NULL);
	}
	FUNC_END("ewl_container_render_children");
	return;
}

EwlBool      _cb_ewl_container_resize_children(EwlLL *node, EwlData *data)
{
	EwlWidget    *widget    = (EwlWidget*) node->data;
	EwlContainer *container = (EwlContainer*) widget->parent;
	EwlRect      *rect      = NULL;
	int           x, y, w, h;
	
	if (ewl_widget_get_flag(widget, VISIBLE) /*&&
	    ewl_widget_get_flag(widget, CAN_RESIZE)*/) {
		x = container->child_padding[EWL_PADDING_LEFT];
		y = container->child_padding[EWL_PADDING_TOP];
		w = widget->parent->layout->rect->w      -
		    container->child_padding[EWL_PADDING_LEFT] -
		    container->child_padding[EWL_PADDING_RIGHT];
		h = widget->parent->layout->rect->h      -
		    container->child_padding[EWL_PADDING_TOP]  -
		    container->child_padding[EWL_PADDING_BOTTOM];
		rect = ewl_rect_new_with_values(&x, &y, &w, &h);
		/*if (ewl_debug_is_active())*/
		fprintf(stderr,"padding l=%d, t=%d, r=%d, b=%d \n",
		        container->child_padding[EWL_PADDING_LEFT], 
		        container->child_padding[EWL_PADDING_TOP], 
		        container->child_padding[EWL_PADDING_RIGHT], 
		        container->child_padding[EWL_PADDING_BOTTOM]);
			fprintf(stderr,"resizing widget %08x to ", (unsigned int) widget);
		ewl_rect_dump(rect);
		if (widget->layout->rect)
			ewl_rect_free(widget->layout->rect);
		widget->layout->rect = rect;
		if (ewl_widget_get_flag(widget, HAS_CHILDREN))
			ewl_container_resize_children(widget);
		ewl_widget_set_flag(widget, NEEDS_REFRESH, TRUE);
		ewl_widget_set_flag(widget, NEEDS_RESIZE, FALSE);
	} else {
		if (ewl_debug_is_active())
			fprintf(stderr,"widget 0x%08x cannot be resized "
			        "(visible=%s, can_resize=%s.\n",
			        (unsigned int) widget,
			        ewl_widget_get_flag(widget,VISIBLE)?"TRUE":"FALSE",
			        ewl_widget_get_flag(widget,CAN_RESIZE)?"TRUE":"FALSE");
	}
	return TRUE;
}

void         ewl_container_resize_children(EwlWidget *c)
{
	EwlContainer *container = (EwlContainer*) c;
	FUNC_BGN("ewl_container_resize_children");
	if (!c)	{
		ewl_debug("ewl_container_resize_children",EWL_NULL_WIDGET_ERROR,"c");
	} else if (!container->resize_children) {
		if (ewl_debug_is_active())
			fprintf(stderr,"resizing children\n");
		ewl_container_foreach(c,_cb_ewl_container_resize_children,NULL);
		ewl_widget_set_flag(c, NEEDS_REFRESH, TRUE);
		ewl_widget_set_flag(c, NEEDS_RESIZE, FALSE);
	} else {
		container->resize_children(container);
	}
	FUNC_END("ewl_container_resize_children");
	return;
}


EwlBool _cb_ewl_container_realize_children(EwlLL *node, EwlData *data)
{
	EwlWidget *widget = (EwlWidget*) node->data;
	EwlEvent  *ev = ewl_event_new_by_type_with_widget(EWL_EVENT_REALIZE,
	                                                  widget);
	ewl_event_queue(ev);
	return TRUE;
}

void         ewl_container_realize_children(EwlWidget *widget, EwlData *data)
{
	EwlContainer *container = (EwlContainer*) widget;
	FUNC_BGN("ewl_container_realize_children");
	if (!container)	{
		ewl_debug("ewl_container_realize_children",
		          EWL_NULL_WIDGET_ERROR, "container");
	} else if (!ewl_widget_get_flag(widget,HAS_CHILDREN)) {
		ewl_debug("ewl_container_realize_children",
		          EWL_GENERIC_ERROR, "Widget is not a Container.");
	} else {
		ewl_container_foreach(widget,
		                      _cb_ewl_container_realize_children,
		                      data);
	}
	FUNC_END("ewl_container_realize_children");
	return;
}

EwlBool _cb_ewl_container_unrealize_children(EwlLL *node, EwlData *data)
{
	EwlWidget *widget = (EwlWidget*) node->data;
	EwlEvent  *ev = ewl_event_new_by_type_with_widget(EWL_EVENT_UNREALIZE,
	                                                  widget);
	ewl_event_queue(ev);
	return TRUE;
}

void         ewl_container_unrealize_children(EwlWidget *widget, EwlData *data)
{
	EwlContainer *container = (EwlContainer*) widget;
	FUNC_BGN("ewl_container_unrealize_children");
	if (!container)	{
		ewl_debug("ewl_container_unrealize_children",
		          EWL_NULL_WIDGET_ERROR, "container");
	} else if (!ewl_widget_get_flag(widget,HAS_CHILDREN)) {
		ewl_debug("ewl_container_unrealize_children",
		          EWL_GENERIC_ERROR, "Widget is not a Container.");
	} else {
		ewl_container_foreach(widget,
		                      _cb_ewl_container_unrealize_children,
		                      data);
	}
	FUNC_END("ewl_container_unrealize_children");
	return;
}


void         ewl_container_set_child_padding(EwlWidget *c, int *l, int *t,
                                                           int *r, int *b)
{
	EwlContainer *container = (EwlContainer*) c;
	FUNC_BGN("ewl_container_set_child_padding");
	if (!c)	{
		ewl_debug("ewl_container_set_child_padding",
		          EWL_NULL_WIDGET_ERROR, "c");
	} else {
		if (l) container->child_padding[EWL_PAD_LEFT]   = *l;
		if (t) container->child_padding[EWL_PAD_TOP]    = *t;
		if (r) container->child_padding[EWL_PAD_RIGHT]  = *r;
		if (b) container->child_padding[EWL_PAD_BOTTOM] = *b;
		if (l||t||r||b)
			ewl_container_resize_children(c);
	}
	FUNC_END("ewl_container_set_child_padding");
	return;
}

void         ewl_container_get_child_padding(EwlWidget *c, int *l, int *t,
                                                           int *r, int *b)
{
	EwlContainer *container = (EwlContainer*) c;
	FUNC_BGN("ewl_container_get_child_padding");
	if (!c)	{
		ewl_debug("ewl_container_get_child_padding",
		          EWL_NULL_WIDGET_ERROR, "c");
	} else {
		if (l) *l = container->child_padding[EWL_PAD_LEFT];
		if (t) *t = container->child_padding[EWL_PAD_TOP];
		if (r) *r = container->child_padding[EWL_PAD_RIGHT];
		if (b) *b = container->child_padding[EWL_PAD_BOTTOM];
	}
	FUNC_END("ewl_container_get_child_padding");
	return;
}

