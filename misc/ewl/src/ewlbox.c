#include "ewlbox.h"

EwlWidget *ewl_hbox_new(EwlBool homogeneous)
{
	EwlWidget *b = ewl_box_new(EWL_HBOX, homogeneous);
	FUNC_BGN("ewl_hbox_new");
	if (!b)	{
		ewl_debug("ewl_hbox_new", EWL_NULL_WIDGET_ERROR, "b");
	}
	FUNC_BGN("ewl_hbox_new");
	return b;
}

EwlWidget *ewl_vbox_new(EwlBool homogeneous)
{
	EwlWidget *b = ewl_box_new(EWL_VBOX, homogeneous);
	FUNC_BGN("ewl_vbox_new");
	if (!b)	{
		ewl_debug("ewl_vbox_new", EWL_NULL_WIDGET_ERROR, "b");
	}
	FUNC_BGN("ewl_vbox_new");
	return b;
}

EwlWidget *ewl_lbox_new()
{
	EwlWidget *b = ewl_box_new(EWL_LBOX, FALSE);
	FUNC_BGN("ewl_lbox_new");
	if (!b)	{
		ewl_debug("ewl_lbox_new", EWL_NULL_WIDGET_ERROR, "b");
	}
	FUNC_BGN("ewl_lbox_new");
	return b;
}

EwlWidget *ewl_box_new(EwlType type, EwlBool homogeneous)
{
	EwlBox *b = malloc(sizeof(EwlBox));
	FUNC_BGN("ewl_box_new");
	if (!b)	{
		ewl_debug("ewl_box_new", EWL_NULL_WIDGET_ERROR, "b");
	} else if (type!=EWL_HBOX&&type!=EWL_VBOX&&type!=EWL_LBOX)	{
		ewl_debug("ewl_box_new", EWL_OUT_OF_BOUNDS_ERROR, "type");
	} else {
		ewl_widget_set_type((EwlWidget*)b, type);
		ewl_box_init((EwlWidget*)b);
	}
	FUNC_BGN("ewl_box_new");
	return (EwlWidget*) b;
}

void       ewl_box_init(EwlWidget *widget)
{
	EwlContainer *container = (EwlContainer*) widget;
	EwlType       type = ewl_widget_get_type(widget);
	FUNC_BGN("ewl_box_init");
	ewl_container_init(container);
	ewl_widget_set_type(widget,type);
	ewl_widget_set_flag(widget, CAN_RESIZE, TRUE);

	ewl_callback_add(widget,EWL_EVENT_RESIZE,
	                 ewl_box_handle_resize, NULL);
	ewl_callback_add(widget,EWL_EVENT_REALIZE,
	                 ewl_box_handle_realize, NULL);
	container->resize_children = ewl_box_handle_resize_children;

	FUNC_END("ewl_box_init");
	return;
}

EwlBool    ewl_box_handle_realize(EwlWidget *widget,
                                  EwlEvent  *ev,
                                  EwlData   *data)
{
	FUNC_END("ewl_box_handle_resize");

	ewl_widget_get_theme(widget,"/EwlBox");
	switch (widget->type)	{
	case EWL_VBOX:
		ewl_widget_get_theme(widget,"/EwlVBox");
		break;
	case EWL_HBOX:
		ewl_widget_get_theme(widget,"/EwlHBox");
		break;
	case EWL_LBOX:
		ewl_widget_get_theme(widget,"/EwlLBox");
		break;
	default:
		break;
	}

	FUNC_END("ewl_box_handle_resize");
	return TRUE;
}


EwlType    ewl_box_get_type(EwlWidget *box)
{
	EwlType t = 0;
	FUNC_BGN("ewl_box_get_type");
	if (!box)	{
		ewl_debug("ewl_box_get_type", EWL_NULL_WIDGET_ERROR, "box");
	} else {
		t = ewl_widget_get_type(box);
	}
	FUNC_END("ewl_box_get_type");
	return t;
}

void       ewl_box_set_type(EwlWidget *box, EwlType type)
{
	FUNC_BGN("ewl_box_set_type");
	if (!box)	{
		ewl_debug("ewl_box_set_type", EWL_NULL_WIDGET_ERROR, "box");
	} else if (type<EWL_HBOX||type>EWL_LBOX)	{
		ewl_debug("ewl_box_set_type", EWL_OUT_OF_BOUNDS_ERROR, "type");
	} else {
		ewl_widget_set_type(box, type);
	}
	FUNC_BGN("ewl_box_new");
	return;
}


void       ewl_box_pack_start(EwlWidget *box, EwlWidget *child)
{

	return;
}

void       ewl_box_pack_end(EwlWidget *box, EwlWidget *child)
{
	FUNC_BGN("ewl_box_pack_end");
	if (!box)	{
		ewl_debug("ewl_box_pack_end", EWL_NULL_WIDGET_ERROR, "box");
	} else if (!child)	{
		ewl_debug("ewl_box_pack_end", EWL_NULL_WIDGET_ERROR, "child");
	} else {
		ewl_container_insert(box,child);
	}
	FUNC_END("ewl_box_pack_end");
	return;
}


void       ewl_box_remove(EwlWidget *box, EwlWidget *child)
{
	FUNC_BGN("ewl_box_remove");
	if (!box)	{
		ewl_debug("ewl_box_remove", EWL_NULL_WIDGET_ERROR, "box");
	} else if (!child)	{
		ewl_debug("ewl_box_remove", EWL_NULL_WIDGET_ERROR, "child");
	} else {
		ewl_container_remove(box,child);
	}
	FUNC_END("ewl_box_remove");
	return;
}

EwlBool ewl_box_handle_resize(EwlWidget *widget, EwlEvent *ev, EwlData *d)
{

	return TRUE;
}

EwlBool      ewl_hbox_resize_children_foreach(EwlLL *node, EwlData *data)
{
	EwlWidget    *widget    = (EwlWidget*) node->data;
	EwlWidget    *tw        = NULL;
	EwlContainer *container = (EwlContainer*) widget->parent;
	EwlRect      *rect      = NULL;
	int           x, y, w, h, *fp;
	
	if (ewl_widget_get_flag(widget, VISIBLE) /*&&
	    ewl_widget_get_flag(widget, CAN_RESIZE)*/) {
		
		/* x and y are WRONG -- htey need to be built recursively
		   from the parent tree -- this will fixe the offset problem */
		fp = ewl_widget_get_full_padding(widget);
		x = fp[0];
		y = fp[1];
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
		if (ewl_widget_get_flag(widget,REALIZED) && 
		    ewl_widget_get_background(widget))	{
			evas_move(ewl_widget_get_evas(widget),
			          ewl_widget_get_background(widget), x, y);
			evas_resize(ewl_widget_get_evas(widget),
			          ewl_widget_get_background(widget), w, h);
			evas_set_image_fill(ewl_widget_get_evas(widget),
			                    ewl_widget_get_background(widget),
			                    0, 0, w, h);
		}
		ewl_event_queue_new(widget,EWL_EVENT_RESIZE,NULL);
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

EwlBool      ewl_vbox_resize_children_foreach(EwlLL *node, EwlData *data)
{
	EwlWidget    *widget    = (EwlWidget*) node->data;
	EwlWidget    *tw        = NULL;
	EwlContainer *container = (EwlContainer*) widget->parent;
	EwlRect      *rect      = NULL;
	int           x, y, w, h, *fp;
	
	if (ewl_widget_get_flag(widget, VISIBLE) /*&&
	    ewl_widget_get_flag(widget, CAN_RESIZE)*/) {
		
		/* x and y are WRONG -- htey need to be built recursively
		   from the parent tree -- this will fixe the offset problem */
		fp = ewl_widget_get_full_padding(widget);
		x = fp[0];
		y = fp[1];
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
		if (ewl_widget_get_flag(widget,REALIZED) && 
		    ewl_widget_get_background(widget))	{
			evas_move(ewl_widget_get_evas(widget),
			          ewl_widget_get_background(widget), x, y);
			evas_resize(ewl_widget_get_evas(widget),
			          ewl_widget_get_background(widget), w, h);
			evas_set_image_fill(ewl_widget_get_evas(widget),
			                    ewl_widget_get_background(widget),
			                    0, 0, w, h);
		}
		ewl_event_queue_new(widget,EWL_EVENT_RESIZE,NULL);
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

EwlBool      ewl_lbox_resize_children_foreach(EwlLL *node, EwlData *data)
{
	EwlWidget    *widget    = (EwlWidget*) node->data;
	EwlWidget    *tw        = NULL;
	EwlContainer *container = (EwlContainer*) widget->parent;
	EwlRect      *rect      = NULL;
	int           x, y, w, h, *fp;
	
	if (ewl_widget_get_flag(widget, VISIBLE) /*&&
	    ewl_widget_get_flag(widget, CAN_RESIZE)*/) {
		
		/* x and y are WRONG -- htey need to be built recursively
		   from the parent tree -- this will fixe the offset problem */
		fp = ewl_widget_get_full_padding(widget);
		x = fp[0];
		y = fp[1];
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
		if (ewl_widget_get_flag(widget,REALIZED) && 
		    ewl_widget_get_background(widget))	{
			evas_move(ewl_widget_get_evas(widget),
			          ewl_widget_get_background(widget), x, y);
			evas_resize(ewl_widget_get_evas(widget),
			          ewl_widget_get_background(widget), w, h);
			evas_set_image_fill(ewl_widget_get_evas(widget),
			                    ewl_widget_get_background(widget),
			                    0, 0, w, h);
		}
		ewl_event_queue_new(widget,EWL_EVENT_RESIZE,NULL);
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

void         ewl_box_handle_resize_children(EwlWidget *c)
{
	EwlContainer *container = (EwlContainer*) c;
	FUNC_BGN("ewl_box_handle_resize_children");
	if (!c)	{
		ewl_debug("ewl_box_handle_resize_children",
		          EWL_NULL_WIDGET_ERROR, "c");
	} else {
		if (ewl_debug_is_active())
			fprintf(stderr,"resizing children\n");
		switch (c->type)	{
		case EWL_HBOX:
			ewl_container_foreach(c,
			                      ewl_hbox_resize_children_foreach,
			                      NULL);
			break;
		case EWL_VBOX:
			ewl_container_foreach(c,
			                      ewl_vbox_resize_children_foreach,
			                      NULL);
			break;
		case EWL_LBOX:
			ewl_container_foreach(c,
			                      ewl_lbox_resize_children_foreach,
			                      NULL);
			break;
		default:
			break;
		}
		ewl_widget_set_flag(c, NEEDS_REFRESH, TRUE);
		ewl_widget_set_flag(c, NEEDS_RESIZE, FALSE);
	}
	FUNC_END("ewl_box_handle_resize_children");
	return;
}


