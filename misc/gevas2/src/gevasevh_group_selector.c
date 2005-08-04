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

#include "config.h"
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

#include <gevasevh_group_selector.h>
#include <gevasevh_selectable.h>

#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>
#define _gtk_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID

#include <stdio.h>

enum {
    SIG_CHANGED,
    SIG_LAST
};
static guint signals[SIG_LAST] = { 0 };

#define EMIT_0( o, sig ) \
{ \
    gboolean ret=GEVASOBJ_SIG_OK; \
\
    gtk_signal_emit(GTK_OBJECT(o), signals[sig], 0, &ret ); \
\
    if(ret != GEVASOBJ_SIG_OK) \
        return; \
}


#define EMIT_1( o, sig, a1 ) \
{ \
    gboolean ret=GEVASOBJ_SIG_OK; \
\
    gtk_signal_emit(GTK_OBJECT(o), signals[sig], a1, &ret ); \
\
    if(ret != GEVASOBJ_SIG_OK) \
        return; \
}


static void gevasevh_group_selector_class_init(GtkgEvasEvHGroupSelectorClass * klass);
static void gevasevh_group_selector_init(GtkgEvasEvHGroupSelector * ev);
/* GtkObject functions */
static void gevasevh_group_selector_destroy(GtkObject * object);
static void gevasevh_group_selector_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasevh_group_selector_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


void gevas_group_selector_get_wh( 
	GtkgEvasEvHGroupSelector *ev, 
	gint  cx, gint cy,
	Evas_Coord* x, Evas_Coord* y,
	Evas_Coord* rw, Evas_Coord* rh
	);



enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_SELECTED_OBJ,
};

/*
 * This creates a list of all the GtkgEvasObj that are selected.
 *
 * Note that a object that has many images, eg a, icon that changes when selected, both
 * of the images will be in the return list.
 *
 * caller frees return value.
 */
Evas_List* gevasevh_group_selector_get_selection_objs(GtkgEvasEvHGroupSelector* ev )
{
	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev),0);
    
    return gevas_obj_collection_to_evas_list( ev->col );
}

/*
 * Get all the selectables that we have selected. caller frees list.
 */
Evas_List* gevasevh_group_selector_get_selected_selectables(GtkgEvasEvHGroupSelector* ev )
{
    Evas_List* ret = 0;
    Evas_List* tl = 0;
    GtkgEvasEvHSelectable* s = 0;
    
	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev),0);

	for(tl = gevas_obj_collection_to_evas_list( ev->col ); tl; tl = tl->next)
    {
        if( s = gevas_selectable_get_backref( ev->gevas , tl->data) )
        {
			ret = evas_list_append( ret, s );
        }
    }

    return ret;
}




/**/
/* Flush the current selection, add selected files are first unselected.*/
/**/
void
gevasevh_group_selector_flushsel( GtkgEvasEvHGroupSelector* ev )
{
	Evas_List* tl = 0;
    GtkgEvasEvHSelectable* s=0;
    
	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));

    gevas_obj_collection_clear( ev->col );
}


/**/
/* Move all the selected items.*/
/**/
void gevasevh_group_selector_movesel(GtkgEvasEvHGroupSelector* ev, gint32 dx, gint32 dy )
{

	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));

/*     printf("gevasevh_group_selector_movesel() col.sz:%ld \n", */
/*            gevas_obj_collection_get_size(  ev->col ) ); */

    
    gevas_obj_collection_move_relative( ev->col, dx, dy );
}

static void clean( GtkgEvasObjCollection* c )
{
    if( !c ) return;
    gtk_object_destroy( GTK_OBJECT(c) );
//    gtk_object_unref( GTK_OBJECT(c) );
}


/**/
/* Check to see if a selectable is in the current selection*/
/**/
gboolean gevasevh_group_selector_isinsel(
	GtkgEvasEvHGroupSelector* ev, 
	GtkgEvasEvHSelectable* o )
{
    GtkgEvasObjCollection* c = 0;
    gboolean ret = 0;

	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));
	g_return_if_fail(ev->col != NULL);

    c = gevasevh_selectable_to_collection( o );
    ret = gevas_obj_collection_contains_all( ev->col, c );
    clean(c);
    return ret;
}

/**/
/* Add a new selectable to the current selection.*/
/**/
void
gevasevh_group_selector_addtosel( GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
    GtkgEvasObjCollection* c = 0;
    
	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));

    
    gevas_obj_collection_add_all( ev->col, c = gevasevh_selectable_to_collection( o ));
    clean(c);
    gevas_selectable_select( o , 1 );
}

/**/
/* Select all selectable objects in x,y,w,h.*/
/**/
void gevasevh_group_selector_floodselect(
	GtkgEvasEvHGroupSelector* ev,
	double x, double y, double w, double h)
{
    gevas_obj_collection_add_flood_area( ev->col, x, y, w, h );
}


/**/
/* Select all objects between the last clicked selectable and "o"*/
/**/
void gevasevh_group_selector_floodtosel( 
	GtkgEvasEvHGroupSelector* ev,
    GtkgEvasEvHSelectable* o,
    GtkgEvasObj* go)
{
    gevas_obj_collection_add_flood( ev->col, go,
                                    gevas_obj_collection_get_lastadded(ev->col));
}


/**/
/* Remove a selectable from the current selection*/
/**/
void gevasevh_group_selector_remfromsel( 
	GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
    GtkgEvasObjCollection* c = 0;
    
	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));

    gevas_selectable_select( o , 0 );
    gevas_obj_collection_remove_all( ev->col, c = gevasevh_selectable_to_collection( o ));
    clean(c);
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
/*    printf(" __drag_begin() \n");*/
    
}

static void __drag_end(GtkWidget          *widget,
                       GdkDragContext     *context,
                       gpointer data)                       
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
/*    printf("drag_end()\n");*/
}

static void __drag_data_get(GtkWidget          *widget,
                            GdkDragContext     *context,
                            GtkSelectionData   *selection_data,
                            guint               info,
                            guint               time,
                            gpointer data)
                            
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
/*    printf("__drag_data_get()\n");*/
}

    
static void __drag_data_delete(GtkWidget          *widget,
                               GdkDragContext     *context,
                               gpointer data)
{
	GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
/*    printf("__drag_data_delete()\n");*/

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
                                           1, (GdkEvent*)event);
        gtk_drag_set_icon_default (ev->drag_context);
/*        printf("__gevasevh_group_selector_leave_notify_cb() context:%p\n",ev->drag_context);*/
        
    }

    return 0;
    
}


static gboolean col_add_predicate_cb(
    GtkgEvasObjCollection* col,
    GtkgEvasObjCollection_T o,
    gpointer udata)
{
    GtkgEvasEvHGroupSelector* ev=0;
    
    g_return_if_fail(o     != NULL);
	g_return_if_fail(col   != NULL);
	g_return_if_fail(udata != NULL);
    
	g_return_if_fail( GTK_IS_GEVAS_OBJ_COLLECTION(col));
	g_return_if_fail( GTK_IS_GEVASOBJ(o));
	g_return_if_fail( GTK_GEVASEVH_GROUP_SELECTOR(udata));
    
    ev = GTK_GEVASEVH_GROUP_SELECTOR(udata);
    
    
//    printf("col_add_predicate_cb() ev:%p ev->mark:%p o:%p\n",ev,ev->mark,o);

    if( ev->mark == o )
        return 0;

    return 1;
}



static void col_item_add(
    GtkgEvasObjCollection* col,
    GtkgEvasObj* o,
    GtkgEvasEvHGroupSelector* ev )
{
    GtkgEvasEvHSelectable* s = 0;

	g_return_if_fail(col   != NULL);
	g_return_if_fail(o     != NULL);
	g_return_if_fail(ev    != NULL);
    
	g_return_if_fail( GTK_IS_GEVAS_OBJ_COLLECTION(col) );
	g_return_if_fail( GTK_IS_GEVASOBJ(o));
	g_return_if_fail( GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));

//    printf("col_item_add()\n");
    if(s = gevas_selectable_get_backref( ev->gevas , o))
    {
        gevas_selectable_select( s , 1 );
    }

    EMIT_0( ev, SIG_CHANGED )
}

static void col_item_remove(
    GtkgEvasObjCollection* col,
    GtkgEvasObj* o,
    GtkgEvasEvHGroupSelector* ev )
{
    GtkgEvasEvHSelectable* s = 0;

	g_return_if_fail(col   != NULL);
	g_return_if_fail(o     != NULL);
	g_return_if_fail(ev    != NULL);
    
	g_return_if_fail( GTK_IS_GEVAS_OBJ_COLLECTION(col) );
	g_return_if_fail( GTK_IS_GEVASOBJ(o));
	g_return_if_fail( GTK_IS_GEVASEVH_GROUP_SELECTOR(ev));
	g_return_if_fail( GTK_IS_GEVAS(ev->gevas));

/*    
    printf("col_item_remove() 1: o:%p s:%p\n",o,
           gevas_selectable_get_backref( ev->gevas , o)
        );
*/
    
    if(s = gevas_selectable_get_backref( ev->gevas , o))
    {
        gevas_selectable_select( s , 0 );
    }
    
    EMIT_0( ev, SIG_CHANGED )
}



void gevasevh_group_selector_set_object( GtkgEvasEvHGroupSelector* object, GtkgEvasObj* obj )
{
	GtkgEvasEvHGroupSelector* ev = 0;
    GtkgEvas* gevas = 0;

//    fprintf(stderr,"gevasevh_group_selector_set_object(top)\n");
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));

	ev = object;
    gevas = GTK_GEVAS(gevasobj_get_gevas(GTK_OBJECT(obj)));
	object->mark = obj;
    
    ev->gevas = gevasobj_get_gevas(GTK_OBJECT(obj));
    ev->col   = gevas_obj_collection_new(gevas);

    gtk_signal_connect( GTK_OBJECT(ev->col), "add",    GTK_SIGNAL_FUNC(col_item_add),    ev);
    gtk_signal_connect( GTK_OBJECT(ev->col), "remove", GTK_SIGNAL_FUNC(col_item_remove), ev);

    gevas_obj_collection_set_add_predicate( ev->col, ev, col_add_predicate_cb);
    

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


//    fprintf(stderr,"gevasevh_group_selector_set_object(3)\n");
    
    
/*	printf("gevasevh_group_selector_set_object() %p\n",ev);*/
	if( !ev->rect ) 
	{
		GtkgEvasObj *ct;
//        fprintf(stderr,"gevasevh_group_selector_set_object(4)\n");
		ct = ev->rect = (GtkgEvasObj*)gevasgrad_new(gevasobj_get_gevas(
			GTK_OBJECT(ev->mark)));
//        fprintf(stderr,"gevasevh_group_selector_set_object(5)\n");
		gevasgrad_add_color(ct, 255, 200, 200, 128, 8);
		gevasgrad_add_color(ct, 200, 150, 150, 128, 8);
//        fprintf(stderr,"gevasevh_group_selector_set_object(6)\n");
		gevasgrad_set_angle(ct, 315);
/* 		gevasgrad_seal(ct); */

		gevasobj_set_layer(ct, 9999);
/*		printf(" Made the gradient\n");*/

        gevasobj_add_evhandler(ev->rect, GTK_OBJECT(object));
        
	}

    gevas_add_global_event_watcher( gevas, GTK_OBJECT(ev) );
    
//    fprintf(stderr,"gevasevh_group_selector_set_object(end)\n");
}



GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
//    fprintf(stderr,"gevasev_group_selector_mouse_in()\n");
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
                                int _x, int _y);


GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
    GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

//    fprintf(stderr,"gevasev_group_selector_mouse_out() ev->tracking:%d\n",ev->tracking);
    if( ev->rect && ev->tracking && GTK_OBJECT(gevasobj) != GTK_OBJECT(ev->rect) )
	{
        return gevasev_group_selector_mouse_up( object, gevasobj, 1, _x, _y );
    }
    
    
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b, int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev = 0;
	GdkEvent *gdkev;

    g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

/*     fprintf(stderr,"gevasev_group_selector_mouse_down() gobj:%p mark:%p rect:%p\n", */
/*             gevasobj, ev->mark, ev->rect ); */
    
	if( _b != 1 )
		return GEVASEV_HANDLER_RET_NEXT;

/*     fprintf(stderr,"gevasev_group_selector_mouse_down(2) gobj:%p mark:%p rect:%p\n", */
/*             gevasobj, ev->mark, ev->rect ); */

/*	gevasev_group_selector_show(ev, ev->hot_clicked[_b - 1]);*/

    int hadModifier = 0;
    
/*	printf("got gdkev:%p\n", gdkev );*/
	gdkev = gevas_get_current_event( ev->rect->gevas );
	if( gdkev ) /*&& gdkev->type == GDK_BUTTON_PRESS )*/
	{
		GdkEventButton* gdkbev;
		printf("got gdkev button\n");
		gdkbev = (GdkEventButton*)gdkev;

		if( gdkbev->state & GDK_SHIFT_MASK )
		{
            hadModifier = 1;
/*			printf("gevasev_group_selector_mouse_down shift key\n");*/
		}
		else if( gdkbev->state & GDK_CONTROL_MASK )
		{
            hadModifier = 1;
/*			printf("gevasev_group_selector_mouse_down control key\n");*/
		}
		else 
		{
			gevasevh_group_selector_flushsel( ev );
		}
	}

    if( !g_slist_find( ev->m_rubber_band_starters, gevasobj ) ) 
    {
        int size = g_slist_length( ev->m_rubber_band_starters ); 

        if( !hadModifier )
            gevas_obj_collection_clear( ev->col ); 
        gevasevh_group_selector_floodselect( ev, _x, _y, 1, 1 ); 
        ev->selected_objs_lastadded = 0; 
        
        return GEVASEV_HANDLER_RET_NEXT; 
    } 
    
/* 	gevasobj_set_layer(ev->rect, 999); */
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
	Evas_Coord x=0,y=0,w=0,h=0;
	Evas_List* list;
	void* data;
	GtkgEvasEvHGroupSelector *ev;

//	printf("gevasev_group_selector_mouse_up() b:%d _x:%d _y:%d\n",_b,_x,_y);
    
	if( _b != 1 )
		return GEVASEV_HANDLER_RET_NEXT;

    g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
	gevasobj_hide( ev->rect );
/* 	printf("gevasev_group_selector_mouse_up() _x:%d _y:%d  tracking:%d\n", */
/*            _x,_y,ev->tracking); */
    if( !ev->tracking )
        return GEVASEV_HANDLER_RET_NEXT;
            
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
	Evas_Coord* x, Evas_Coord* y,
	Evas_Coord* rw, Evas_Coord* rh
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

/*     fprintf(stderr,"gevas_group_selector_get_wh()   cx:%d cy:%d  tix:%d tiy:%d  x:%d y:%d rw:%d rh%d\n", */
/*             cx, cy, tix, tiy, *x, *y, *rw, *rh ); */
    
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
/*         fprintf(stderr,"gevasev_group_selector_mouse_move() rect:%p tracking:%d\n", */
/*                 ev->rect , ev->tracking ); */
		Evas_Coord d1=0, d2=0, d3=0, d4=0;
		gevas_group_selector_get_wh( ev, _x, _y, &d1, &d2, &d3, &d4 );
	}

	return GEVASEV_HANDLER_RET_NEXT;
}


static gint sig_changed( GtkgEvasEvHGroupSelector* ev, gpointer d1 )
{
//    printf("sig_changed\n");
    return 0;
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
			(GtkClassInitFunc)  gevasevh_group_selector_class_init,
			(GtkObjectInitFunc) gevasevh_group_selector_init,
			/* reserved_1 */   NULL,
			/* reserved_2 */   NULL,
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

    /** signals **/
    klass->sig_changed = sig_changed;

    /** inherit **/
    evh_klass->handler_mouse_in = gevasev_group_selector_mouse_in;
	evh_klass->handler_mouse_out = gevasev_group_selector_mouse_out;
	evh_klass->handler_mouse_down = gevasev_group_selector_mouse_down;
	evh_klass->handler_mouse_up = gevasev_group_selector_mouse_up;
	evh_klass->handler_mouse_move = gevasev_group_selector_mouse_move;

/*	gtk_object_add_arg_type(GTK_GEVASEVH_GROUP_SELECTOR_SELECTED_OBJ,*/
/*		GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_SELECTED_OBJ);*/

    signals[SIG_CHANGED] =
        gtk_signal_new ("changed",
                        GTK_RUN_LAST,
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET( GtkgEvasEvHGroupSelectorClass, sig_changed),
                        _gtk_marshal_VOID__VOID,
                        GTK_TYPE_NONE,
                        0 );
/*     gtk_object_class_add_signals (object_class, signals, SIG_LAST); */
    

}

static void gevasevh_group_selector_init(GtkgEvasEvHGroupSelector * ev)
{
	ev->rect = 0;
	ev->selected_objs = 0;
    ev->col = 0;
    ev->m_rubber_band_starters = 0;
}

GtkObject *gevasevh_group_selector_new(void)
{
	GtkgEvasEvHGroupSelector *ev;
    ev = gtk_type_new(gevasevh_group_selector_get_type());
//    printf("gevasevh_group_selector_new(void) %d\n", GTK_IS_GEVASEVH(ev));
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

GtkgEvasObjCollection* gevasevh_group_selector_get_collection( GtkgEvasEvHGroupSelector *ev )
{
    return ev->col;
}


void
gevasevh_group_selector_add_rubber_band_starter( GtkgEvasEvHGroupSelector *ev, GtkObject* obj )
{
    ev->m_rubber_band_starters = g_slist_append( ev->m_rubber_band_starters, obj );
}

