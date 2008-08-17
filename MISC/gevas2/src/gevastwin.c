/*
 * Gtk abstraction of a object and another in some form of binding.
 *
 *
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
/* Always disable NLS, since we have no config.h; 
 * a real app would not do this of course.
 */

#include <stdio.h>

#include <gevasevh_selectable.h>
#include <gevasevh_group_selector.h>

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


#include "gevastwin.h"

enum {
	ARG_0 = 100,				/* Skip 0, an invalid argument ID */
	ARG_MAINOBJ,
	ARG_AUXOBJ,
	ARG_ALIGNX,
	ARG_ALIGNY,
	ARG_OFFSETX,
	ARG_OFFSETY
};

static void gevastwin_class_init(GtkgEvasTwinClass * klass);
static void gevastwin_init(GtkgEvasTwin * ev);


/* GtkObject functions */
static void gevastwin_destroy(GtkObject * object);
static void gevastwin_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevastwin_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))



static GtkObjectClass *parent_class = NULL;

guint gevastwin_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasTwin",
			sizeof(GtkgEvasTwin),
			sizeof(GtkgEvasTwinClass),
			(GtkClassInitFunc) gevastwin_class_init,
			(GtkObjectInitFunc) gevastwin_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}


static void gevastwin_class_init(GtkgEvasTwinClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevastwin_destroy;

	object_class->get_arg = gevastwin_get_arg;
	object_class->set_arg = gevastwin_set_arg;

	gtk_object_add_arg_type(GTK_GEVASTWIN_MAINOBJ,
				GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_MAINOBJ);
	gtk_object_add_arg_type(GTK_GEVASTWIN_AUXOBJ,
				GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_AUXOBJ);

	gtk_object_add_arg_type(GTK_GEVASTWIN_ALIGNX,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_ALIGNX);
	gtk_object_add_arg_type(GTK_GEVASTWIN_ALIGNY,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_ALIGNY);

	gtk_object_add_arg_type(GTK_GEVASTWIN_OFFSETX,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_OFFSETX);
	gtk_object_add_arg_type(GTK_GEVASTWIN_OFFSETY,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_OFFSETY);
}

static void gevastwin_init(GtkgEvasTwin * ev)
{
	ev->mainobj = 0;
	ev->auxobj  = 0;
    ev->sprite  = 0;
	ev->ox = 0;
	ev->oy = 5;
	ev->ax = 0;
	ev->ay = 1;
    ev->extra_objects = 0;
    
}

GtkgEvasTwin *gevastwin_new()
{
	GtkgEvasTwin *ev;
    ev = gtk_type_new(gevastwin_get_type());
    return GTK_GEVASTWIN(ev);
}

/* GtkObject functions */


static void gevastwin_destroy(GtkObject * object)
{
	GtkgEvasTwin *ev;

//    fprintf(stderr,"gevastwin_destroy()\n");
    
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));

	ev = GTK_GEVASTWIN(object);

    if( ev->extra_objects )
    {
        Evas_List* li = gevas_obj_collection_to_evas_list( ev->extra_objects );
        for( ; li; li = li->next)
        {
            if(li->data)
            {
                GtkgEvasObj* gobj = (GtkgEvasObj*)li->data;
                evas_object_del( gobj->eobj );
            }
        }
    }

/*     if( ev->mainobj ) */
/*         gevastwin_destroy(  ev->mainobj ); */
/*     if( ev->auxobj ) */
/*         gevastwin_destroy(  ev->auxobj ); */
    
	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

/*********************************************************************/

void _gevastwin_sync_obj(GtkgEvasTwin * ev, GtkgEvasObj * obj)
{
	Evas_Coord main_x = 0, main_y = 0, main_w = 0, main_h = 0;

	if (ev->mainobj && ev->auxobj)
    {
		gevasobj_get_geometry(ev->mainobj, &main_x, &main_y, &main_w, &main_h);

		if (obj == ev->auxobj)
        {
//			ev->aux_obj_move(ev->auxobj, main_x + (ev->ax?main_w:0) + ev->ox, main_y + (ev->ay?main_h:0) + ev->oy);

			ev->aux_obj_move(ev->auxobj, main_x, main_y + main_h + 5);
			gevasobj_queue_redraw(ev->auxobj);
		}
        else if (obj == ev->mainobj)
        {
			Evas_Coord ax = 0, ay = 0, ah = 0, aw = 0;

			gevasobj_get_geometry(ev->auxobj, &ax, &ay, &aw, &ah);
//			ev->main_obj_move(ev->mainobj, ax - (ev->ax?main_w:0) - ev->ox, ay - (ev->ay?main_h:0) - ev->oy);

			ev->main_obj_move(ev->mainobj, ax, ay - main_h - 5);
			gevasobj_queue_redraw(ev->mainobj);
		}
    }
}

void gevastwin_sync_obj( GtkgEvasTwin* ev, GtkgEvasObj* object )
{
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(ev));
    _gevastwin_sync_obj( ev, object );
}

#define GEVASTWIN_EXTRA_OBJ_MOVE_FUNC_KEY "___gevastwin_backward_move_key"

//void (*extra_move_func) (GtkgEvasObj * object, double x, double y);

void _gevastwin_move_xxx(GtkgEvasObj * object, double x, double y)
{
	GtkgEvasTwin *ev;
	gpointer d;

	d = gtk_object_get_data(GTK_OBJECT(object), GEVASTWIN_BACKWARD_LOOKUP_KEY);
	if (d) {
		ev = GTK_GEVASTWIN(d);

/*         printf("gevastwin_move_xxx(top) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                ev->mainobj, ev->auxobj, object ); */
        
		if (ev->mainobj == object) {
            printf("gevastwin_move_xxx(MO) extra object. main:%p aux:%p obj:%p\n", 
                   ev->mainobj, ev->auxobj, object ); 
			ev->main_obj_move(object, x, y);
			_gevastwin_sync_obj(ev, ev->auxobj);
		}
		else if (ev->auxobj == object) {
            printf("gevastwin_move_xxx(AO) extra object. main:%p aux:%p obj:%p\n", 
                   ev->mainobj, ev->auxobj, object ); 
			ev->aux_obj_move(object, x, y);
			_gevastwin_sync_obj(ev, ev->mainobj);
		}
        else
        {
/*             printf("gevastwin_move_xxx() extra object. main:%lp aux:%lp obj:%lp\n", */
/*                    ev->mainobj, ev->auxobj, object ); */
            
			Evas_Coord ox = 0, oy = 0, oh = 0, ow = 0;
            gevasobj_get_geometry( object, &ox, &oy, &ow, &oh);
            Evas_Coord main_x = 0, main_y = 0, main_w = 0, main_h = 0;
            gevasobj_get_geometry(ev->mainobj, &main_x, &main_y, &main_w, &main_h);

/*             printf("gevastwin_move_xxx(2) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                    ev->mainobj, ev->auxobj, object ); */
            
            Evas_Coord dx = ox - x;
            Evas_Coord dy = oy - y;

/*             printf("gevastwin_move_xxx(2.0) extra object. main:%lp aux:%lp obj:%lp dx:%ld dy:%ld\n", */
/*                    ev->mainobj, ev->auxobj, object, dx, dy ); */

/*             gevasobj_move( ev->mainobj, main_x - dx, main_y - dy ); */
            
            
            ev->main_obj_move(ev->mainobj, main_x - dx, main_y - dy );
/*             printf("gevastwin_move_xxx(2.1) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                    ev->mainobj, ev->auxobj, object ); */
            
			ev->aux_obj_move (ev->auxobj,  main_x - dx , main_y + main_h + 5 - dy );

/*             printf("gevastwin_move_xxx(3) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                    ev->mainobj, ev->auxobj, object ); */
            
            Evas_List* li = gevas_obj_collection_to_evas_list( ev->extra_objects );
            for( ; li; li = li->next)
                if(li->data)
                {
                    GtkgEvasObj* gobj = (GtkgEvasObj*)li->data;

/*                     printf("gevastwin_move_xxx(5.t) extra object. main:%lp aux:%lp obj:%lp gobj:%lp\n", */
/*                            ev->mainobj, ev->auxobj, object, gobj ); */
                    
//                    if( gobj != object )
                    {
                        gpointer d;
                        d = gtk_object_get_data(GTK_OBJECT(gobj),
                                                GEVASTWIN_EXTRA_OBJ_MOVE_FUNC_KEY );

/*                         printf("gevastwin_move_xxx(5.a) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                                ev->mainobj, ev->auxobj, object ); */
                        
                        void (*move_func) (GtkgEvasObj * object, double x, double y)
                            = d;
                        
                        gevasobj_get_geometry( gobj, &ox, &oy, &ow, &oh);
                        move_func( gobj, ox - dx, oy - dy );

/*                         printf("gevastwin_move_xxx(5.b) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                                ev->mainobj, ev->auxobj, object ); */
                        
                    }
                }
			gevasobj_queue_redraw(ev->mainobj);
			gevasobj_queue_redraw(ev->auxobj);
        }
#if 0
        else if( ev->sprite )
        {
            /* The moved part of a sprite */
            if( gevas_obj_collection_contains( ev->sprite->col, GTK_GEVASOBJ(object) ))
            {
                if( GTK_GEVASOBJ(ev->sprite)->original_move )
                    GTK_GEVASOBJ(ev->sprite)->original_move( ev->sprite, x, y );
                else
                    GTK_GEVASOBJ(ev->sprite)->move( ev->sprite, x, y );
                _gevastwin_sync_obj(ev, ev->auxobj);
            }
        }
#endif
/*         printf("gevastwin_move_xxx(bottom) extra object. main:%lp aux:%lp obj:%lp\n", */
/*                ev->mainobj, ev->auxobj, object ); */
    }
}

static void twin_object_resized( GtkgEvasObj* object,
                                 double *w,
                                 double *h,
                                 gpointer user_data )
{
	GtkgEvasTwin *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASOBJ(object));
	g_return_if_fail(user_data != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(user_data));
	ev = GTK_GEVASTWIN(user_data);

//    fprintf( stderr, "twin_object_resized(1) w:%f h:%f\n", *w, *h );
//    _gevastwin_sync_obj( ev, object );
//    fprintf( stderr, "twin_object_resized(1) w:%f h:%f\n", *w, *h );
}

/**
 * Change the functionality of move() on objec 'o' to move the twin aswell.
 */
static void setup_object_movements( GtkObject * object, GtkgEvasObj* o )
{
 	GtkgEvasTwin *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));
	ev = GTK_GEVASTWIN(object);

    if( o == ev->mainobj )
        ev->main_obj_move = o->move;
    if( o == ev->auxobj )
        ev->aux_obj_move = o->move;
        
    o->original_move = o->move;
    o->move = _gevastwin_move_xxx;
    gtk_object_set_data(GTK_OBJECT(o), GEVASTWIN_BACKWARD_LOOKUP_KEY, ev);
}

static void sprite_item_add(
    GtkgEvasObjCollection* col,
    GtkgEvasObj* o,
    GtkgEvasTwin* ev )
{
    setup_object_movements( GTK_OBJECT(ev), o );
}

void gevastwin_add_extra_object( GtkgEvasTwin* ev, GtkgEvasObj* gobj )
{
    if( !ev->extra_objects )
    {
        ev->extra_objects = gevas_obj_collection_new( GTK_GEVAS(gobj->gevas) );
    }

    gevas_obj_collection_add( ev->extra_objects, gobj );
    
    gtk_object_set_data(GTK_OBJECT(gobj),
                        GEVASTWIN_EXTRA_OBJ_MOVE_FUNC_KEY,
                        (void*)gobj->move );
    
    gobj->move = _gevastwin_move_xxx;
    gtk_object_set_data(GTK_OBJECT(gobj), GEVASTWIN_BACKWARD_LOOKUP_KEY, ev);
/*             gtk_signal_connect( GTK_OBJECT(gobj), "resize", */
/*                                 GTK_SIGNAL_FUNC(twin_object_resized), ev ); */
}


void gevastwin_add_extra_objects( GtkgEvasTwin* ev, GtkgEvasObjCollection* v )
{
    Evas_List* li = gevas_obj_collection_to_evas_list( v );
    for( ; li; li = li->next)
    {
        if(li->data)
        {
            gevastwin_add_extra_object( ev, li->data );
        }
    }
}


static void gevastwin_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasTwin *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));

	ev = GTK_GEVASTWIN(object);

	switch (arg_id) {
		case ARG_MAINOBJ:
			ev->mainobj = GTK_VALUE_POINTER(*arg);

			ev->main_obj_move = ev->mainobj->move;
			ev->mainobj->move = _gevastwin_move_xxx;
			gtk_object_set_data(GTK_OBJECT(ev->mainobj), GEVASTWIN_BACKWARD_LOOKUP_KEY, ev);

            gtk_signal_connect( GTK_OBJECT(ev->mainobj), "resize",
                                GTK_SIGNAL_FUNC(twin_object_resized), ev );

            
/*             if( GTK_IS_GEVAS_SPRITE( ev->mainobj ) ) */
/*             { */
/*                 GtkgEvasSprite* sprite = GTK_GEVAS_SPRITE( ev->mainobj ); */
/*                 ev->sprite = sprite; */
                
/*                 gtk_signal_connect( GTK_OBJECT(sprite->col), "add", */
/*                                     GTK_SIGNAL_FUNC(sprite_item_add), ev ); */

/*                 Evas_List* li=0; */
/*                 for( li=sprite->col->selected_objs; li; li = li->next) */
/*                 { */
/*                     if(li->data) */
/*                     { */
/*                         setup_object_movements( GTK_OBJECT(ev), li->data ); */
/*                     } */
/*                 } */
/*             } */
            
            
            break;

		case ARG_AUXOBJ:
			ev->auxobj = GTK_VALUE_POINTER(*arg);

			ev->aux_obj_move = ev->auxobj->move;
			ev->auxobj->move = _gevastwin_move_xxx;
			_gevastwin_sync_obj(ev, ev->auxobj);
			gtk_object_set_data(GTK_OBJECT(ev->auxobj), GEVASTWIN_BACKWARD_LOOKUP_KEY, ev);

            gtk_signal_connect( GTK_OBJECT(ev->auxobj), "resize",
                                GTK_SIGNAL_FUNC(twin_object_resized), ev );
            break;

		case ARG_ALIGNX:
			ev->ax = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		case ARG_ALIGNY:
			ev->ay = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		case ARG_OFFSETX:
			ev->ox = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		case ARG_OFFSETY:
			ev->oy = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		default:
			break;
	}
}

static void gevastwin_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasTwin *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));

	ev = GTK_GEVASTWIN(object);

	switch (arg_id) {
		case ARG_MAINOBJ:
			GTK_VALUE_POINTER(*arg) = ev->mainobj;
			break;
		case ARG_AUXOBJ:
			GTK_VALUE_POINTER(*arg) = ev->auxobj;
			break;
		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
