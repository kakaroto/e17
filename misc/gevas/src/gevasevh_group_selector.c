/*
 * Draws a ractangle over the group_selector items and selects each of them on mouse up.
 *
 *
 *
 * Copyright (C) 2001 Ben Martin.
 *
 * Original author: Ben Martin
 *
 * See COPYING for full details of copying & use of this software.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 */

/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

/* Always disable NLS, since we have no config.h; 
 * a real app would not do this of course.
 */
#undef ENABLE_NLS

#ifdef ENABLE_NLS
#include<libintl.h>
#define _(String) dgettext("gtk+",String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else							/* NLS is disabled */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,String) (String)
#define dcgettext(Domain,String,Type) (String)
#define bindtextdomain(Domain,Directory) (Domain)
#endif							/* ENABLE_NLS */


#include "gevasevh_group_selector.h"
#include "gevasevh_selectable.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>

static void gevasevh_group_selector_class_init(GtkgEvasEvHGroupSelectorClass * klass);
static void gevasevh_group_selector_init(GtkgEvasEvHGroupSelector * ev);
/* GtkObject functions */
static void gevasevh_group_selector_destroy(GtkObject * object);
static void
gevasevh_group_selector_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_group_selector_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


void gevas_group_selector_get_wh( 
	GtkgEvasEvHGroupSelector *ev, 
	gint  cx, gint cy,
	double* x, double* y,
	double* rw, double* rh
	);



enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_SELECTEDb_OBJ,
};

Evas_List gevasevh_group_selector_get_selection_objs(GtkgEvasEvHGroupSelector* ev )
{
	Evas_List ret = 0;

	Evas_List tl = ev->selected_objs;
	while(tl)
	{
		if( tl->data )
		{
			ret = evas_list_append( ret, gevasevh_selectable_gevasobj(tl->data) );
		}
		tl = tl->next;
	}
return ret;
}


/**/
/* Flush the current selection, add selected files are first unselected.*/
/**/
void gevasevh_group_selector_flushsel( 
	GtkgEvasEvHGroupSelector* ev )
{
	Evas_List tl = ev->selected_objs;
	while(tl)
	{
		if( tl->data )
			gevas_selectable_select( (GtkgEvasEvHSelectable*)tl->data, 0 );
		tl = tl->next;
	}
	ev->selected_objs = evas_list_free( ev->selected_objs );
	ev->selected_objs = 0;
	ev->selected_objs_lastadded = 0;
}

/**/
/* Move all the selected items.*/
/**/
void gevasevh_group_selector_movesel(GtkgEvasEvHGroupSelector* ev, gint32 dx, gint32 dy )
{
	Evas_List tl = ev->selected_objs;
	for( ; tl ; tl = tl->next)
	{
		if( tl->data )
		{
			gevas_selectable_move( (GtkgEvasEvHSelectable*)tl->data, dx, dy );
		}
	}
}

/**/
/* Check to see if a selectable is in the current selection*/
/**/
gboolean gevasevh_group_selector_isinsel(
	GtkgEvasEvHGroupSelector* ev, 
	GtkgEvasEvHSelectable* o )
{
	Evas_List tl = ev->selected_objs;

	if( !o ) return 0;
	for( ; tl ; tl = tl->next)
	{
		if( tl->data == o )
			return 1;
	}
	return 0;
}


/**/
/* Add a new selectable to the current selection.*/
/**/
void gevasevh_group_selector_addtosel( 
	GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
	ev->selected_objs = evas_list_append( ev->selected_objs, o);
	gevas_selectable_select( o , 1 );
	ev->selected_objs_lastadded = o;
}

/**/
/* Select all selectable objects in x,y,w,h.*/
/**/
void gevasevh_group_selector_floodselect(
	GtkgEvasEvHGroupSelector* ev,
	double x, double y, double w, double h)
{
	Evas_List list;
	void* data;

	list = evas_objects_in_rect(
		gevas_get_evas( ev->rect->gevas ), x,y,w,h);

	while( list )
	{
		data = evas_get_data( 
			gevas_get_evas( ev->rect->gevas ),
			list->data,
			GEVASEVH_SELECTABLE_KEY
		);


		if( data ) 
		{
			gevasevh_group_selector_addtosel( ev, (GtkgEvasEvHSelectable*)data );
		}
		
		list = list->next;
	}
}

/**/
/* Select all objects between the last clicked selectable and "o"*/
/**/
void gevasevh_group_selector_floodtosel( 
	GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
	GtkgEvasEvHSelectable* la = ev->selected_objs_lastadded;
	GtkgEvasEvHSelectable* tl = la; /* top left*/
	GtkgEvasEvHSelectable* br = o; /* bottom right*/
	double x=0,y=0,w=0,h=0,_x=0,_y=0,_w=0,_h=0;

/*	printf("gevasevh_group_selector_floodtosel()\n");*/

	if( !o || !la || o == la )
		return;

/*	printf("gevasevh_group_selector_floodtosel() la:%p o:%p\n",la,o);*/

	gevasobj_hide( ev->rect );
	ev->tracking = 0;

	gevasobj_get_location( GTK_GEVASOBJ(gevasevh_selectable_gevasobj(tl)), &x, &y );
	gevasobj_get_location( GTK_GEVASOBJ(gevasevh_selectable_gevasobj(br)), &_x, &_y );
	if( x > _x && y > _y )
	{
		tl = br;
		br = la;
	}

	gevasobj_get_location( GTK_GEVASOBJ(gevasevh_selectable_gevasobj(tl)), &x, &y );
	gevasobj_get_geometry( GTK_GEVASOBJ(gevasevh_selectable_gevasobj(br)), 
		&_x, &_y, &_w, &_h );

	w = _x + _w - x;
	h = _y + _h - y;

/*	printf(" x:%f y:%f w:%f h:%f\n",x,y,w,h);*/

	gevasevh_group_selector_floodselect( ev, x, y, w, h );

	ev->selected_objs_lastadded = o;
}


/**/
/* Remove a selectable from the current selection*/
/**/
void gevasevh_group_selector_remfromsel( 
	GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
	gevas_selectable_select( o , 0 );
	ev->selected_objs = evas_list_remove( ev->selected_objs, o );
	ev->selected_objs_lastadded = 0;
}



void gevasevh_group_selector_set_drag_targets(
    GtkgEvasEvHGroupSelector* ev, GtkTargetList *dt )
{
    ev->drag_targets = dt;
    gtk_target_list_ref(ev->drag_targets);
    
}



void gevasevh_group_selector_dragging( GtkgEvasEvHGroupSelector *ev, gboolean d )
{
    ev->drag_is_dragging = d;
    
}



static void __drag_begin(GtkWidget          *widget,
                         GdkDragContext     *context,
                         gpointer data)
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
//    printf(" __drag_begin() \n");
    
}

static void __drag_end(GtkWidget          *widget,
                       GdkDragContext     *context,
                       gpointer data)                       
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
//    printf("drag_end()\n");
}

static void __drag_data_get(GtkWidget          *widget,
                            GdkDragContext     *context,
                            GtkSelectionData   *selection_data,
                            guint               info,
                            guint               time,
                            gpointer data)
                            
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
//    printf("__drag_data_get()\n");
}

    
static void __drag_data_delete(GtkWidget          *widget,
                               GdkDragContext     *context,
                               gpointer data)
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
//    printf("__drag_data_delete()\n");

}




static gint __gevasevh_group_selector_leave_notify_cb(
    GtkWidget          *widget,
    GdkEventCrossing   *event,
    GtkgEvasEvHGroupSelector* ev
    )
{
/*
  printf("__gevasevh_group_selector_leave_notify_cb() w:%p event:%p ev:%p gevas:%p gwin:%d\n",
           widget, event, ev,
           GTK_WIDGET (ev->rect->gevas),
           GTK_WIDGET_NO_WINDOW(GTK_WIDGET (ev->rect->gevas))
        );
*/
    
    if( ev->drag_targets && ev->drag_is_dragging )
    {
        ev->drag_context = gtk_drag_begin (GTK_WIDGET (ev->rect->gevas),
                                           ev->drag_targets,
                                           GDK_ACTION_MOVE | GDK_ACTION_COPY,
                                           1, event);

        gtk_drag_set_icon_default (ev->drag_context);
//        printf("__gevasevh_group_selector_leave_notify_cb() context:%p\n",ev->drag_context);
        
    }

    return 0;
    
}



void gevasevh_group_selector_set_object( GtkgEvasEvHGroupSelector* object, GtkgEvasObj* obj )
{
	GtkgEvasEvHGroupSelector* ev = object;
    GtkgEvas* gevas = GTK_GEVAS(gevasobj_get_gevas(GTK_OBJECT(obj)));
	object->mark = obj;

    gtk_signal_connect(GTK_OBJECT(gevas), "leave_notify_event",
                       GTK_SIGNAL_FUNC(__gevasevh_group_selector_leave_notify_cb), ev);


    gtk_signal_connect(GTK_OBJECT(gevas), "drag_begin",
                       GTK_SIGNAL_FUNC(__drag_begin), ev);
    gtk_signal_connect(GTK_OBJECT(gevas), "drag_end",
                       GTK_SIGNAL_FUNC(__drag_end), ev);
    gtk_signal_connect(GTK_OBJECT(gevas), "drag_data_get",
                       GTK_SIGNAL_FUNC(__drag_data_get), ev);
    gtk_signal_connect(GTK_OBJECT(gevas), "drag_data_delete",
                       GTK_SIGNAL_FUNC(__drag_data_delete), ev);

    
    
//	printf("gevasevh_group_selector_set_object() %p\n",ev);
	if( !ev->rect ) 
	{
		GtkgEvasObj *ct;
		ct = ev->rect = (GtkgEvasObj*)gevasgrad_new(gevasobj_get_gevas(
			GTK_OBJECT(ev->mark)));
		gevasgrad_add_color(ct, 255, 200, 200, 128, 8);
		gevasgrad_add_color(ct, 200, 150, 150, 128, 8);
		gevasgrad_set_angle(ct, 315);
		gevasgrad_seal(ct);

		gevasobj_set_layer(ct, 9999);
/*		printf(" Made the gradient\n");*/
	}
/*	printf("gevasevh_group_selector_set_object(end)");*/
}



GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	GdkEvent *gdkev;

	if( _b != 1 )
		return GEVASEV_HANDLER_RET_NEXT;

	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

	

/*	gevasev_group_selector_show(ev, ev->hot_clicked[_b - 1]);*/

	gdkev = gevas_get_current_event( ev->rect->gevas );
/*	printf("got gdkev:%p\n", gdkev );*/
	if( gdkev ) /*&& gdkev->type == GDK_BUTTON_PRESS )*/
	{
		GdkEventButton* gdkbev;
/*		printf("got gdkev button\n");*/
		gdkbev = (GdkEventButton*)gdkev;

		if( gdkbev->state & GDK_SHIFT_MASK )
		{
/*			printf("gevasev_group_selector_mouse_down shift key\n");*/
		}
		else if( gdkbev->state & GDK_CONTROL_MASK )
		{
/*			printf("gevasev_group_selector_mouse_down control key\n");*/
		}
		else 
		{
			gevasevh_group_selector_flushsel( ev );
		}
	}

	gevasobj_move( ev->rect, _x, _y);
	gevasobj_resize( ev->rect, 2,2);
	gevasobj_show( ev->rect );
	gevasobj_queue_redraw( ev->rect );
	ev->tracking = 1;
	ev->tracking_ix = _x;
	ev->tracking_iy = _y;

//	printf("gevasev_group_selector_mouse_down() done\n");

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	double x=0,y=0,w=0,h=0;
	Evas_List list;
	void* data;
	GtkgEvasEvHGroupSelector *ev;

	if( _b != 1 )
		return GEVASEV_HANDLER_RET_NEXT;

//	printf("gevasev_group_selector_mouse_up()\n");

	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
	gevasobj_hide( ev->rect );
	ev->tracking = 0;

	gevas_group_selector_get_wh( ev, _x, _y, &x, &y, &w, &h );
	gevasevh_group_selector_floodselect( ev, x, y, w, h );
	ev->selected_objs_lastadded = 0;

/*	printf("gevasev_group_selector_mouse_up() x:%f y:%f w:%f h:%f\n",x,y,w,h);*/
/*	printf("gevasev_group_selector_mouse_up(return)\n");*/
	return GEVASEV_HANDLER_RET_NEXT;
}

/**/
/* Get the w & h of the selection box. This also moves the box to obscure to cx,cy.*/
/**/
void gevas_group_selector_get_wh( 
	GtkgEvasEvHGroupSelector *ev, 
	gint  cx, gint cy,
	double* x, double* y,
	double* rw, double* rh
	)
{
	double w=0, h=0;
	gboolean moveit = 0;
	gint tix = ev->tracking_ix;	
	gint tiy = ev->tracking_iy;	

	gevasobj_get_location( ev->rect, x, y );

	*rw = abs(cx - tix);
	if( cx < tix ) 
	{
		moveit=1;
		*x = cx;		
	}

	*rh = abs(cy - tiy);
	if( cy < tiy ) 
	{
		moveit=1;
		*y = cy;
	}

	if( moveit )
		gevasobj_move	( ev->rect , *x, *y );

	w = *rw;
	h = *rh;
	if( w < 2 ) w=2;
	if( h < 2 ) h=2;
	gevasobj_resize	( ev->rect , w, h );

}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

	if( ev->rect && ev->tracking ) 
	{
		double d1=0, d2=0, d3=0, d4=0;
		gevas_group_selector_get_wh( ev, _x, _y, &d1, &d2, &d3, &d4 );
	}

	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_group_selector_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHGroupSelector",
			sizeof(GtkgEvasEvHGroupSelector),
			sizeof(GtkgEvasEvHGroupSelectorClass),
			(GtkClassInitFunc) gevasevh_group_selector_class_init,
			(GtkObjectInitFunc) gevasevh_group_selector_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_group_selector_class_init(GtkgEvasEvHGroupSelectorClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_group_selector_destroy;
	object_class->get_arg = gevasevh_group_selector_get_arg;
	object_class->set_arg = gevasevh_group_selector_set_arg;

	evh_klass->handler_mouse_in = gevasev_group_selector_mouse_in;
	evh_klass->handler_mouse_out = gevasev_group_selector_mouse_out;
	evh_klass->handler_mouse_down = gevasev_group_selector_mouse_down;
	evh_klass->handler_mouse_up = gevasev_group_selector_mouse_up;
	evh_klass->handler_mouse_move = gevasev_group_selector_mouse_move;

/*	gtk_object_add_arg_type(GTK_GEVASEVH_GROUP_SELECTOR_SELECTED_OBJ,*/
/*		GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_SELECTED_OBJ);*/

}

static void gevasevh_group_selector_init(GtkgEvasEvHGroupSelector * ev)
{
/*	ev->selected = 0;*/
	ev->rect = 0;
	ev->selected_objs =0;
}

GtkObject *gevasevh_group_selector_new(void)
{
	GtkgEvasEvHGroupSelector *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_group_selector_get_type());
	hev = (GtkgEvasEvH *) ev;
/*	ev->selected = 0;*/
	ev->rect = 0;
	ev->selected_objs =0;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_group_selector_destroy(GtkObject * object)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

    gtk_target_list_unref(ev->drag_targets);

    /* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_group_selector_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

/*
	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			ev->selected = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->selected, object);
			break;
		default:
			break;
	}
*/
}

static void
gevasevh_group_selector_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
/*
	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->selected;
			break;
		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
*/
}
