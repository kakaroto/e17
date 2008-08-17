/*
 * Gtk abstraction of Evas_Object
 *
 * Internal function to this file/package begin with _
 *
 * Copyright (C) 2000 Ben Martin.
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

#include "config.h"
#include <gevasobj.h>

#include <stdio.h>
#include <string.h>

#include "project.h"
#include "gevas_sprite.h"
#include "gevasevh_group_selector.h"

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



enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_GEVAS
};

enum {
    SIG_MOVE_ABSOLUTE,
    SIG_MOVE_RELATIVE,
    SIG_RESIZE,
    SIG_VISIBLE,
    SIG_LAYER,
    SIG_LAST
};

static guint signals[SIG_LAST] = { 0 };


static void gevasobj_class_init(GtkgEvasObjClass * klass);
static void gevasobj_init(GtkgEvasObj * ev);


/* GtkObject functions */
static void gevasobj_destroy(GtkObject * object);
static void gevasobj_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasobj_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasobj_finalize(GObject* object);

#define GEVAS(ev) ev->gevas
#define EVAS(ev) gevas_get_evas(GEVAS(ev))
#define EVASO(ev) _gevas_get_obj(GTK_OBJECT(ev))


typedef gboolean (*GtkSignal_BOOL__POINTER_POINTER) (GtkObject * object,
                                             gpointer arg1,
                                             gpointer arg2,
                                             gpointer user_data);
void
gtk_marshal_BOOL__POINTER_POINTER (GtkObject * object,
                                   GtkSignalFunc func,
                                   gpointer func_data, GtkArg * args)
{
  GtkSignal_BOOL__POINTER_POINTER rfunc;
  gboolean *return_val;
  return_val = GTK_RETLOC_BOOL (args[2]);
  rfunc = (GtkSignal_BOOL__POINTER_POINTER) func;
  *return_val = (*rfunc) (object, GTK_VALUE_POINTER (args[0]),
                          GTK_VALUE_POINTER (args[1]), func_data);
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

#define EMIT_2( o, sig, a1, a2 ) \
{ \
    gboolean ret=GEVASOBJ_SIG_OK; \
\
    gtk_signal_emit(GTK_OBJECT(o), signals[sig], a1, a2, &ret ); \
\
    if(ret != GEVASOBJ_SIG_OK) \
        return; \
}



void gevasobj_queue_redraw(GtkgEvasObj * obj)
{
	gevas_queue_redraw(obj->gevas);
}

Evas* _gevas_evas(GtkObject * object)
{
	GtkgEvasObj *ev;

	g_return_val_if_fail(object != NULL, NULL);
	g_return_val_if_fail(GTK_IS_GEVASOBJ(object), NULL);

	ev = GTK_GEVASOBJ(object);
	return EVAS(ev);
}

void _gevas_set_obj(GtkObject * object, Evas_Object* eobj)
{
	GtkgEvasObj *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));

	ev = GTK_GEVASOBJ(object);
	ev->eobj = eobj;

    evas_object_data_set( eobj, PRIVATE_GTK_GEVASOBJ, object);

/*     evas_object_pass_events_set( eobj, 1 ); */
/*     evas_object_repeat_events_set( eobj, 1 ); */
    
	evas_object_event_callback_add( eobj, EVAS_CALLBACK_MOUSE_IN,  __gevas_mouse_in, object);
	evas_object_event_callback_add( eobj, EVAS_CALLBACK_MOUSE_OUT, __gevas_mouse_out, object);
	evas_object_event_callback_add( eobj, EVAS_CALLBACK_MOUSE_DOWN,__gevas_mouse_down, object);
	evas_object_event_callback_add( eobj, EVAS_CALLBACK_MOUSE_UP,  __gevas_mouse_up, object);
	evas_object_event_callback_add( eobj, EVAS_CALLBACK_MOUSE_MOVE,__gevas_mouse_move, object);
    
}



Evas_Object* _gevas_get_obj(GtkObject * object)
{
	GtkgEvasObj *ev;
	g_return_val_if_fail(object != NULL, NULL);
	g_return_val_if_fail(GTK_IS_GEVASOBJ(object), NULL);
	ev = GTK_GEVASOBJ(object);
	return ev->eobj;
}

/** its now exported to the public **/
Evas_Object* gevasobj_get_evasobj(GtkObject * object)
{
	return _gevas_get_obj(object);
}

GtkgEvasObj*
gevasobj_from_evasobj( Evas_Object* eo )
{
    if( !eo ) return 0;
    return evas_object_data_get( eo, PRIVATE_GTK_GEVASOBJ );
}




GtkgEvas *gevasobj_get_gevas(GtkObject * object)
{
	GtkgEvasObj *ev;
	g_return_val_if_fail(object != NULL, NULL);
	g_return_val_if_fail(GTK_IS_GEVASOBJ(object), NULL);
	ev = GTK_GEVASOBJ(object);

	return GEVAS(ev);
}

Evas* gevasobj_get_evas(GtkObject * object)
{
	GtkgEvas *gevas = gevasobj_get_gevas(object);
	return gevas_get_evas(gevas);
}


void _gevasobj_ensure_obj_free(GtkObject * object)
{
	GtkgEvasObj *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));

	ev = GTK_GEVASOBJ(object);
	if (ev->eobj) {
/*		printf("FREE gevasobj:%d\n", (int) ev->eobj);*/
        evas_object_hide( ev->eobj );
		evas_object_del( ev->eobj );
		ev->eobj = NULL;
	}
}

void _gevasobj_set_color(GtkgEvasObj * object, int r, int g, int b, int a)
{
	evas_object_color_set( EVASO(object), r, g, b, a);
	gevasobj_queue_redraw(object);
}

void _gevasobj_set_alpha(GtkgEvasObj * object, int a)
{
	int r = 0, g = 0, b = 0, aa = 0;
	evas_object_color_get(EVASO(object), &r, &g, &b, &aa);
	evas_object_color_set(EVASO(object), r, g, b, a);
	gevasobj_queue_redraw(object);
}

void _gevasobj_set_zoom_scale(GtkgEvasObj * object, int scale)
{
    evas_object_image_smooth_scale_set( EVASO(object), scale);
}
void _gevasobj_set_layer(GtkgEvasObj * object, int l)
{
    EMIT_1( object, SIG_LAYER, &l )
        evas_object_layer_set( EVASO(object), l);
}
int _gevasobj_get_layer(GtkgEvasObj * object)
{
    return evas_object_layer_get( EVASO(object) );
}
void _gevasobj_set_layer_store(GtkgEvasObj * object, int l, int store)
{
/* 	evas_set_layer_store(EVAS(object), l, store); */
}

void _gevasobj_raise(GtkgEvasObj * object)
{
    evas_object_raise( EVASO(object) );
}

void _gevasobj_lower(GtkgEvasObj * object)
{
    evas_object_lower( EVASO(object) );
}

void _gevasobj_stack_above(GtkgEvasObj * object, GtkgEvasObj * above)
{
    evas_object_stack_above( EVASO(object), EVASO(above));
}

void _gevasobj_stack_below(GtkgEvasObj * object, GtkgEvasObj * below)
{
    evas_object_stack_below( EVASO(object), EVASO(below));
}
void _gevasobj_move(GtkgEvasObj * object, double x, double y)
{
    evas_object_move( EVASO(object), (Evas_Coord)x, (Evas_Coord)y);
	gevasobj_queue_redraw(object);
}
void _gevasobj_resize(GtkgEvasObj * object, double w, double h)
{
    evas_object_resize( EVASO(object), w, h);
	gevasobj_queue_redraw(object);
}
void _gevasobj_get_geometry(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y,
							Evas_Coord *w, Evas_Coord *h)
{
    evas_object_geometry_get( EVASO(object), x, y, w, h);
}
void _gevasobj_get_location(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y)
{
	Evas_Coord w, h;
    evas_object_geometry_get( EVASO(object), x, y, &w, &h);
}
void _gevasobj_set_location(GtkgEvasObj * object, Evas_Coord x, Evas_Coord y)
{
    evas_object_move( EVASO(object), x, y );
}
void _gevasobj_get_size(GtkgEvasObj * object, Evas_Coord *w, Evas_Coord *h)
{
	Evas_Coord x, y;
    evas_object_geometry_get( EVASO(object), &x, &y, w, h);
}

void _gevasobj_show(GtkgEvasObj * object)
{
    evas_object_show( EVASO(object));
	gevasobj_queue_redraw(object);
}

void _gevasobj_hide(GtkgEvasObj * object)
{
	evas_object_hide( EVASO(object) );
	gevasobj_queue_redraw(object);
}
void _gevasobj_get_color(GtkgEvasObj * object, int *r, int *g, int *b, int *a)
{
	evas_object_color_get( EVASO(object), r, g, b, a);
}

void _gevasobj_set_name(GtkgEvasObj * object, gchar * name)
{
    evas_object_name_set( EVASO(object), name);
}

gchar *_gevasobj_get_name(GtkgEvasObj * object)
{
    return (gchar*)evas_object_name_get( EVASO(object));
}

int _gevasobj_get_alpha(GtkgEvasObj * object)
{
	int r = 0, g = 0, b = 0, a = 0;
	evas_object_color_get( EVASO(object), &r, &g, &b, &a);
	return a;
}


/*** public members vtable versions ***/
#include "gevasev_handler.h"
void _gevasobj_add_evhandler(GtkgEvasObj * object, GtkObject * h)
{
	GtkgEvasObj *ev;
    GtkgEvasEvHClass* k;
	g_return_if_fail(h != NULL);
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH(h));
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
	ev = GTK_GEVASOBJ(object);
    k = (GtkgEvasEvHClass*)GTK_OBJECT_GET_CLASS(h);

    if( GEVASEV_HANDLER_PRIORITY_HI == k->get_priority(GTK_GEVASEVH(h)))
	{
		ev->ev_handlers = g_slist_prepend(ev->ev_handlers, h);
	}
	else
		ev->ev_handlers = g_slist_append(ev->ev_handlers, h);

    if( GTK_IS_GEVAS_SPRITE( object ) )
    {
        GtkgEvasSprite* sprite = GTK_GEVAS_SPRITE( object );

        Evas_List* li=0;
        for( li=sprite->col->selected_objs; li; li = li->next)
        {
            if(li->data)
                gevasobj_add_evhandler( GTK_GEVASOBJ( li->data ), h );
        }
        
    }

    if( GTK_IS_GEVASEVH_GROUP_SELECTOR(h) )
    {
        gevasevh_group_selector_add_rubber_band_starter( GTK_GEVASEVH_GROUP_SELECTOR(h),
                                                         GTK_OBJECT( object ));
        
    }
}

void _gevasobj_remove_evhandler(GtkgEvasObj * object, GtkObject * h)
{
	GtkgEvasObj *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
	ev = GTK_GEVASOBJ(object);
	ev->ev_handlers = g_slist_remove(ev->ev_handlers, h);
}

GSList *gevasobj_get_evhandlers(GtkgEvasObj * object )
{
	GtkgEvasObj *ev;

	g_return_val_if_fail(object != NULL, NULL);
	g_return_val_if_fail(GTK_IS_GEVASOBJ(object), NULL);
	ev = GTK_GEVASOBJ(object);
	return ev->ev_handlers;
}

static GtkObjectClass *parent_class = NULL;

guint gevasobj_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasObj",
			sizeof(GtkgEvasObj),
			sizeof(GtkgEvasObjClass),
			(GtkClassInitFunc) gevasobj_class_init,
			(GtkObjectInitFunc) gevasobj_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}

static gint sig_move_absolute(GtkgEvasObj* ev, gpointer d1, gpointer d2)
{
/*     printf("sig_move_absolute\n"); */
    return 0;
}

static gint sig_move_relative(GtkgEvasObj* ev, gpointer d1, gpointer d2)
{
/*     printf("sig_move_relative\n"); */
    return 0;
}

static gint sig_resize(GtkgEvasObj* ev, gpointer d1, gpointer d2)
{
/*     printf("sig_resize\n"); */
    return 0;
}

static gint sig_visible(GtkgEvasObj* ev, gpointer d1)
{
/*     printf("sig_visible\n"); */
    return 0;
}

static gint sig_layer(GtkgEvasObj* ev, gpointer d1)
{
/*     printf("sig_layer\n"); */
    return 0;
}


static void gevasobj_class_init(GtkgEvasObjClass * klass)
{
	GtkObjectClass *object_class;
    GObjectClass   *go;

    go           = (GObjectClass   *) klass;
    object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevasobj_destroy;

	object_class->get_arg = gevasobj_get_arg;
	object_class->set_arg = gevasobj_set_arg;
    go->finalize = gevasobj_finalize;

    /** signals **/
    klass->sig_move_absolute = sig_move_absolute;
    klass->sig_move_relative = sig_move_relative;
    klass->sig_resize        = sig_resize;
    klass->sig_visible       = sig_visible;
    klass->sig_layer         = sig_layer;
    
    /** Protected inherit **/
	klass->_gevas_evas = _gevas_evas;
	klass->_gevas_set_obj = _gevas_set_obj;
	klass->_gevasobj_ensure_obj_free = _gevasobj_ensure_obj_free;

    /** public members **/
	klass->set_color = _gevasobj_set_color;
	klass->set_zoom_scale = _gevasobj_set_zoom_scale;
	klass->set_layer = _gevasobj_set_layer;
	klass->get_layer = _gevasobj_get_layer;
	klass->set_layer_store = _gevasobj_set_layer_store;
	klass->raise = _gevasobj_raise;
	klass->lower = _gevasobj_lower;
	klass->stack_above = _gevasobj_stack_above;
	klass->stack_below = _gevasobj_stack_below;
/*  klass->move = _gevasobj_move; */
	klass->resize = _gevasobj_resize;
	klass->get_geometry = _gevasobj_get_geometry;
	klass->show = _gevasobj_show;
	klass->hide = _gevasobj_hide;
	klass->get_color = _gevasobj_get_color;
	klass->set_name = _gevasobj_set_name;
	klass->get_name = _gevasobj_get_name;
	klass->get_alpha = _gevasobj_get_alpha;
	klass->set_alpha = _gevasobj_set_alpha;
	klass->get_location = _gevasobj_get_location;
	klass->set_location = _gevasobj_set_location;
	klass->get_size = _gevasobj_get_size;

	klass->add_evhandler = _gevasobj_add_evhandler;
	klass->remove_evhandler = _gevasobj_remove_evhandler;

	gtk_object_add_arg_type(GTK_GEVASOBJ_GEVAS,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_GEVAS);


    


    signals[SIG_MOVE_ABSOLUTE] =
        gtk_signal_new ("move_absolute",
                        GTK_RUN_LAST,
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET (GtkgEvasObjClass, sig_move_absolute),
                        gtk_marshal_INT__POINTER_POINTER,
                        GTK_TYPE_INT, 2,
                        GTK_TYPE_POINTER, GTK_TYPE_POINTER);




    signals[SIG_MOVE_RELATIVE] =
        gtk_signal_new ("move_relative",
                        GTK_RUN_LAST, 
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET (GtkgEvasObjClass, sig_move_relative),
                        gtk_marshal_INT__POINTER_POINTER,
                        GTK_TYPE_INT, 2,
                        GTK_TYPE_POINTER, GTK_TYPE_POINTER);



    signals[SIG_RESIZE] =
        gtk_signal_new ("resize", GTK_RUN_LAST, 
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET (GtkgEvasObjClass, sig_resize),
                        gtk_marshal_INT__POINTER_POINTER,
                        GTK_TYPE_INT, 2,
                        GTK_TYPE_POINTER, GTK_TYPE_POINTER);

    signals[SIG_VISIBLE] =
        gtk_signal_new ("visible", GTK_RUN_LAST, 
                        GTK_CLASS_TYPE(object_class), 
                        GTK_SIGNAL_OFFSET (GtkgEvasObjClass, sig_visible),
                        gtk_marshal_INT__POINTER,
                        GTK_TYPE_INT, 1,
                        GTK_TYPE_POINTER );

    signals[SIG_LAYER] =
        gtk_signal_new ("layer", GTK_RUN_LAST, 
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET (GtkgEvasObjClass, sig_layer),
                        gtk_marshal_INT__POINTER,
                        GTK_TYPE_INT, 1,
                        GTK_TYPE_POINTER );


                        
/*     gtk_object_class_add_signals (object_class, signals, SIG_LAST); */

    
}

static void gevasobj_init(GtkgEvasObj * ev)
{

	GEVAS(ev) = NULL;
	ev->ev_handlers = NULL;
	ev->move = _gevasobj_move;
}

GtkObject *gevasobj_new(void)
{
	GtkgEvasObj *ev;

	ev = gtk_type_new(gevasobj_get_type());
	ev->eobj = NULL;
	ev->move = _gevasobj_move;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasobj_destroy(GtkObject * object)
{
    
	GtkgEvasObj *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
	ev = GTK_GEVASOBJ(object);

    
	/* printf(" gevasobj_destroy() refc:%d\n", object->ref_count); */
	_gevasobj_ensure_obj_free(object);
	GEVAS(ev) = NULL;
	g_slist_free(ev->ev_handlers);
	ev->ev_handlers = NULL;


/* 	/\* Chain up *\/ */
/* 	if (GTK_OBJECT_GET_CLASS(parent_class)->destroy) */
/* 		(*GTK_OBJECT_GET_CLASS(parent_class)->destroy) (object); */
}

static void gevasobj_finalize(GObject * object)
{
	GtkgEvasObj *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
	ev = GTK_GEVASOBJ(object);

	/* printf("FREE() gevasobj_finalize()\n"); */

	/* Chain up */
//	if (GTK_OBJECT_GET_CLASS(parent_class)->destroy)
//		(*GTK_OBJECT_GET_CLASS(parent_class)->destroy) (object);
}

/** public wrappers that call the vtable functions so that subclasses
    can modify/add to the default action **/
#define VTAB ((GtkgEvasObjClass*)GTK_OBJECT_GET_CLASS(object))
//#define VTAB ((GtkgEvasObjClass*)(((GtkObject*)object)->klass))

void gevasobj_set_color(GtkgEvasObj * object, int r, int g, int b, int a)
{
	VTAB->set_color(object, r, g, b, a);
}
void gevasobj_set_color_from_string(GtkgEvasObj * object, const gchar* s )
{
	g_return_if_fail( s != NULL);
	g_return_if_fail( strlen(s) );
    
    GtkgEvasObj *ev;
    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_GEVASOBJ(object));
    ev = GTK_GEVASOBJ(object);

/*     fprintf( stderr, "gevasobj_set_color_from_string() s:%s s.len:%ld\n", s, strlen(s));  */
    if( s[0] == '#' )
    {
        if( strlen( s ) >= 7 )
        {
            int r=0,g=0,b=0;
            sscanf( s+1, "%2x%2x%2x", &r, &g, &b );
/*             fprintf( stderr, "font-color r:%x g:%x b:%x a:%x\n", r, g, b, */
/*                      gevasobj_get_alpha(object)); */
            gevasobj_set_color( object, r, g, b, gevasobj_get_alpha(object));
        }
    }
}
void gevasobj_set_alpha(GtkgEvasObj * object, int a)
{
	VTAB->set_alpha(object, a);
}
void gevasobj_set_zoom_scale(GtkgEvasObj * object, int scale)
{
	VTAB->set_zoom_scale(object, scale);
}
void gevasobj_set_layer(GtkgEvasObj * object, int l)
{
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
    
	VTAB->set_layer(object, l);
}
int gevasobj_get_layer(GtkgEvasObj * object)
{
	g_return_val_if_fail(object != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASOBJ(object),0);    
    
	return VTAB->get_layer(object);
}
void gevasobj_set_layer_store(GtkgEvasObj * object, int l, int store)
{
	VTAB->set_layer_store(object, l, store);
}

void gevasobj_raise(GtkgEvasObj * object)
{
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));

	VTAB->raise(object);
}

void gevasobj_lower(GtkgEvasObj * object)
{
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));

	VTAB->lower(object);
}

void gevasobj_stack_above(GtkgEvasObj * object, GtkgEvasObj * above)
{
	VTAB->stack_above(object, above);
}

void gevasobj_stack_below(GtkgEvasObj * object, GtkgEvasObj * below)
{
	VTAB->stack_below(object, below);
}
void gevasobj_move(GtkgEvasObj * object, double x, double y)
{
//    printf("gevasobj_move() x:%f y:%f\n",x,y);
    Evas_Coord ex = x;
    Evas_Coord ey = y;
    EMIT_2( object, SIG_MOVE_ABSOLUTE, &ex, &ey )
        object->move(object, x, y);
}

void gevasobj_move_relative( GtkgEvasObj * object, double dx, double dy)
{
    Evas_Coord lx=0, ly=0;

    gevasobj_get_location( object, &lx, &ly );
    
    lx += dx;	
    ly += dy;
            
    EMIT_2( object, SIG_MOVE_RELATIVE, &lx, &ly )
    gevasobj_move( object, lx, ly );
}



void gevasobj_resize(GtkgEvasObj * object, double w, double h)
{
    EMIT_2( object, SIG_RESIZE, &w, &h )
    
    
	VTAB->resize(object, w, h);
}
void gevasobj_get_geometry(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y,
						   Evas_Coord *w, Evas_Coord *h)
{
	VTAB->get_geometry(object, x, y, w, h);
}
void gevasobj_get_location(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y)
{
	VTAB->get_location(object, x, y);
}
void gevasobj_set_location(GtkgEvasObj * object, Evas_Coord x, Evas_Coord y)
{
	VTAB->set_location(object, x, y);
}
void gevasobj_get_size(GtkgEvasObj * object, Evas_Coord *w, Evas_Coord *h)
{
	VTAB->get_size(object, w, h );
}
void gevasobj_show(GtkgEvasObj * object)
{
    gevasobj_set_visible( object, 1 );
}

void gevasobj_hide(GtkgEvasObj * object)
{
    gevasobj_set_visible( object, 0 );
}

void gevasobj_set_visible(GtkgEvasObj * object, gboolean v)
{
    EMIT_1( object, SIG_VISIBLE, &v )

        if( v ) VTAB->show(object);
        else    VTAB->hide(object);
}

gboolean gevasobj_get_visible( GtkgEvasObj * object )
{
    return evas_object_visible_get( EVASO(object) );
}

void gevasobj_get_color(GtkgEvasObj * object, int *r, int *g, int *b, int *a)
{
	VTAB->get_color(object, r, g, b, a);
}

int gevasobj_get_alpha(GtkgEvasObj * object)
{
	return VTAB->get_alpha(object);
}

void gevasobj_set_name(GtkgEvasObj * object, gchar * name)
{
	VTAB->set_name(object, name);
}

gchar *gevasobj_get_name(GtkgEvasObj * object)
{
	return VTAB->get_name(object);
}


void gevasobj_add_evhandler(GtkgEvasObj * object, GtkObject * h)
{
	return VTAB->add_evhandler(object, h);
}

void gevasobj_remove_evhandler(GtkgEvasObj * object, GtkObject * h)
{
	return VTAB->remove_evhandler(object, h);
}


static void gevasobj_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasObj *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));

	ev = GTK_GEVASOBJ(object);

	switch (arg_id) {
		case ARG_GEVAS:
			if (!GEVAS(ev)) {
				GEVAS(ev) = GTK_VALUE_POINTER(*arg);
				_register_gevasobj(GEVAS(ev), object);
			}
			break;

		default:
			break;
	}
}

static void gevasobj_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasObj *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS(object));

	ev = GTK_GEVASOBJ(object);

	switch (arg_id) {
		case ARG_GEVAS:
			GTK_VALUE_POINTER(*arg) = GEVAS(ev);
			break;


		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}

void  gevasobj_set_data( GtkgEvasObj* ev, const char* key, void* data )
{
    evas_object_data_set( EVASO(ev), key, data);
}


void* gevasobj_get_data( GtkgEvasObj* ev, const char* key )
{
    return evas_object_data_get( EVASO(ev), key );
}
