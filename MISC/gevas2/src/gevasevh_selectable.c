/*
 * Draws an image behind the given evasobj when told to.
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
 *
 *
 *
 ****************************************************************************
 *
 *
 * Note that most of the interesting stuff that is done in this class is 
 * effected through callbacks used by group_selector.
 *
 * For example, to select this selectable, we tell the group_selector to 
 * add us to its selection, it in turn calls
 * void gevas_selectable_select( GtkgEvasEvHSelectable * ev, gboolean s )
 * in our class to actually make the selection visible.
 *
 *
 *
 */


//
// confine and selectable_move()
//

/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

#include "config.h"
#include "gevas_sprite.h"

#define GEVASEVH_SELECTABLE_KEY		"GEVAS::GEVASEVH_SELECTABLE_KEY"

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


#include <gevasevh_selectable.h>
#include <gevasevh_group_selector.h>

#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>
#include <gdk/gdktypes.h>

static void gevasevh_selectable_class_init(GtkgEvasEvHSelectableClass * klass);
static void gevasevh_selectable_init(GtkgEvasEvHSelectable * ev);
/* GtkObject functions */
static void gevasevh_selectable_destroy(GtkObject * object);
static void
gevasevh_selectable_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_selectable_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_SELECTED_OBJ,
};





static gint col_item_move_relative(
    GtkgEvasObj* o,
    double* x, double* y,
	GtkgEvasEvHSelectable *ev )
{
    GtkgEvasEvHSelectable* s = 0;

/*     printf("col_item_move_relative(1) x:%p y:%p ev:%p o:%p\n", x,y,ev,o ); */
    
    g_return_val_if_fail(x     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(y     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(ev    != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(o     != NULL, GEVASOBJ_SIG_VETO);

	g_return_val_if_fail( GTK_IS_GEVASOBJ(o), GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(ev), GEVASOBJ_SIG_VETO);

/*     printf("col_item_move_rel(2) x:%f y:%f\n",*x,*y); */

    
    if( o != ev->normal && o != ev->selected )
        return GEVASOBJ_SIG_OK;
    
    if( ev->confine )
    {
        gint vx, vy, vw, vh;
        Evas_Coord objw, objh;

/*         printf("col_item_move_relative(3) ev->gevas:%p o->eobj:%p\n", ev->gevas, o->eobj ); */
        
        
        gevas_get_viewport_area( ev->gevas, &vx, &vy, &vw, &vh );
        gevasobj_get_size( o, &objw, &objh );

//        printf("col_item_move_relative(4) x:%d y:%d objw:%f objh:%f\n",*x,*y,objw,objh); 

          // selected image is larger than standard image, and we dont mind of the lips
          // go over the edge of the display.
          if( o == ev->selected )
          {
              vx -=   ev->border_x;
              vw += 2*ev->border_x;
 
              vy -=   ev->border_y;
              vh += 2*ev->border_y;
          }     
         
        if( *x < vx ) *x = vx;
        if( *y < vy ) *y = vy;
        if( *x+objw > vx+vw ) *x = vx+vw-objw;
        if( *y+objh > vy+vh ) *y = vy+vh-objh;

/*         printf("gevas_selectable_move() vx:%d vy:%d vw:%d vh:%d\n",vx,vy,vw,vh); */
    }
    
    return GEVASOBJ_SIG_OK;
}



GtkgEvasEvHSelectable*
gevas_selectable_get_backref(
    GtkgEvas* gevas,
    GtkgEvasObj* o)
{
	g_return_val_if_fail(gevas != NULL, 0);
	g_return_val_if_fail(GTK_IS_GEVAS(gevas), 0);

    if(!o)
        return 0;

    g_return_val_if_fail(GTK_IS_GEVASOBJ(o),0);

    if( !o->eobj )
    {
        return 0;
    }
    
    return evas_object_data_get( _gevas_get_obj(GTK_OBJECT(o)),
                                 GEVASEVH_SELECTABLE_KEY );
}

void gevas_selectable_set_backref(GtkgEvasEvHSelectable * ev, GtkgEvasObj* o )
{
	g_return_if_fail(o != NULL);
	g_return_if_fail(o->gevas != NULL);
	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(ev));
	g_return_if_fail(GTK_IS_GEVASOBJ(o));
	g_return_if_fail(GTK_IS_GEVAS(o->gevas));
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(ev->evh_selector));

    evas_object_data_set( _gevas_get_obj(GTK_OBJECT(o)),
                          GEVASEVH_SELECTABLE_KEY, ev );

    gtk_signal_connect( GTK_OBJECT(o), "move_relative",
                        GTK_SIGNAL_FUNC(col_item_move_relative), ev);

    
/*     printf("gevas_selectable_set_backref() ev:%p o:%p evas:%p reverse lookup:%p\n", */
/*            ev,o, */
/*            gevas_get_evas( o->gevas ), */
/*            gevas_selectable_get_backref( o->gevas ,o)); */
    
    
}



/**/
/* We need to veto a drag handler for group dragging.*/
/**/
static GEVASEV_HANDLER_PRIORITY gevasev_selectable_get_priority( GtkgEvasEvH* evh )
{
	return GEVASEV_HANDLER_PRIORITY_HI;
}

static void sprite_item_add(
    GtkgEvasObjCollection* col,
    GtkgEvasObj* o,
    GtkgEvasEvHSelectable* ev )
{
    gevas_selectable_set_backref( ev, o );
}


/**/
/* Attachs the handler, we also put outself in the evasobj so that things can*/
/* be done faster later.*/
/**/
void gevasevh_selectable_set_normal_gevasobj(
	GtkgEvasEvHSelectable* ev, 
	GtkgEvasObj* nor )
{
        Eina_List* li;
	ev->normal = nor;

    if( !ev->gevas )
    {
        ev->gevas = nor->gevas;
    }

//    printf("Setting backref for ev:%p on obj:%p\n",ev,ev->normal);
    gevas_selectable_set_backref( ev, ev->normal );

    if( GTK_IS_GEVAS_SPRITE( nor ) )
    {
        GtkgEvasSprite* sprite = GTK_GEVAS_SPRITE( nor );

        gtk_signal_connect( GTK_OBJECT(sprite->col), "add",
                            GTK_SIGNAL_FUNC(sprite_item_add), ev );

	    li=0;
        for( li=sprite->col->selected_objs; li; li = li->next)
        {
            if(li->data)
                gevas_selectable_set_backref( ev, GTK_GEVASOBJ( li->data ) );
        }
        
    }
}

void gevasevh_selectable_set_selector( GtkgEvasEvHSelectable* evh, GtkObject* evh_selector )
{
	evh->evh_selector = evh_selector;
}


/**/
/* A callback type function used by group_selector to select and unselect objects*/
/* as it sees fit.*/
/**/
void gevas_selectable_select( GtkgEvasEvHSelectable * ev, gboolean s )
{
	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(ev));
	g_return_if_fail(ev->gevas != NULL);
	g_return_if_fail(GTK_IS_GEVAS(ev->gevas));

	if( s )
	{
		Evas_Coord x=0, y=0, w=0, h=0;
		gint32 bx = ev->border_x;
		gint32 by = ev->border_y;
		int lay=0;
        
// 		printf("gevas_selectable_select() x:%d y:%d bx:%d by:%d\n",x,y,bx,by); 
		gevasobj_get_geometry( ev->normal, &x, &y, &w, &h );
		gevasobj_move( ev->selected, x - bx, y - by);
		gevasobj_resize( ev->selected, w + 2*bx, h + 2*by);

		gevasobj_set_layer( ev->selected, gevasobj_get_layer(ev->normal) - 1);
        gevasobj_show( ev->selected );
	}
	else {
/*		printf("hiding for selectable\n");*/
		gevasobj_hide( ev->selected );
	}

//    gevas_queue_redraw( GTK_GEVAS(ev->gevas) );
}

gboolean
gevas_selectable_is_selected( GtkgEvasEvHSelectable * ev )
{
    return gevasobj_get_visible( ev->selected );
}



void gevasevh_selectable_set_confine( GtkgEvasEvHSelectable* evh, gboolean c )
{
    evh->confine = c;
}






/*
 * Caller frees return value.
 */
GtkgEvasObjCollection*
gevasevh_selectable_to_collection( GtkgEvasEvHSelectable* ev )
{
    GtkgEvasObjCollection* c = 0;

	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(ev),0);
	g_return_val_if_fail(ev->gevas != NULL,0);
    
    c = gevas_obj_collection_new( ev->gevas );

    gevas_obj_collection_add( c, ev->normal );
    gevas_obj_collection_add( c, ev->selected );

    if( GTK_IS_GEVAS_SPRITE( ev->normal ) )
    {
        GtkgEvasSprite* sprite = GTK_GEVAS_SPRITE( ev->normal );

        Eina_List* li=0;
        for( li=sprite->col->selected_objs; li; li = li->next)
        {
            if(li->data)
                gevas_obj_collection_add( c, GTK_GEVASOBJ( li->data ) );
        }
        
    }
    

    return c;
}



GEVASEV_HANDLER_RET
gevasev_selectable_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

/*	gevasev_selectable_show(ev, ev->hot);*/
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

/*	gevasev_selectable_show(ev, ev->cold);*/
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	GdkEvent *gdkev;
    GtkgEvasEvHGroupSelector *evh_sel;

	if( _b != 1 )
		return GEVASEV_HANDLER_RET_NEXT;

	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);
    evh_sel = GTK_GEVASEVH_GROUP_SELECTOR(ev->evh_selector);
	g_return_val_if_fail(ev != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(evh_sel != NULL, GEVASEV_HANDLER_RET_NEXT);

    gevasevh_group_selector_dragging(
        GTK_GEVASEVH_GROUP_SELECTOR(ev->evh_selector), 1 );
    ev->tracking = 1;
	ev->tracking_x = _x;
	ev->tracking_y = _y;

//    fprintf(stderr,"gevasev_selectable_mouse_down() _x:%d _y:%d\n", _x, _y );
    
	gdkev = gevas_get_current_event( ev->normal->gevas );
//	printf("gevasev_selectable_mouse_down() got gdkev:%p\n", gdkev );
	if( gdkev ) /*&& gdkev->type == GDK_BUTTON_PRESS )*/
	{
		GdkEventButton* gdkbev;
        

/*		printf("got gdkev button\n");*/

		gdkbev = (GdkEventButton*)gdkev;

		if( gdkbev->state & GDK_SHIFT_MASK )
		{
//			printf("gevasev_selectable_mouse_down() shift key\n");
			gevasevh_group_selector_floodtosel( evh_sel, ev, GTK_GEVASOBJ(gevasobj) );
			return GEVASEV_HANDLER_RET_NEXT;
		}
		if( gdkbev->state & GDK_CONTROL_MASK )
		{
/* 			printf("gevasev_selectable_mouse_down() control key isinsel:%d\n", */
/*                    gevasevh_group_selector_isinsel( evh_sel, ev )); */
            
			if( gevasevh_group_selector_isinsel( evh_sel, ev ))
				gevasevh_group_selector_remfromsel( evh_sel, ev );
			else
				gevasevh_group_selector_addtosel( evh_sel, ev );
			return GEVASEV_HANDLER_RET_NEXT;
		}

	}

/*	printf("selectable down\n");*/


	if( !gevasevh_group_selector_isinsel( evh_sel, ev ))
	{
		gevasevh_group_selector_flushsel( evh_sel );
		gevasevh_group_selector_addtosel( evh_sel, ev );
	}
/*	printf("selectable down(ret)\n");*/


	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;

	if( _b != 1 )
		return GEVASEV_HANDLER_RET_NEXT;

	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

/*	gevas_selectable_select( ev , 0 );*/

/*	gevasevh_group_selector_remfromsel( ev->evh_selector, ev );*/
/*	printf("gevasev_selectable_mouse_up()\n");*/
    gevasevh_group_selector_dragging(
        GTK_GEVASEVH_GROUP_SELECTOR(ev->evh_selector), 0 );
	ev->tracking = 0;

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
    GtkgEvasEvHGroupSelector *evh_sel;

    g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);
    evh_sel = GTK_GEVASEVH_GROUP_SELECTOR(ev->evh_selector);

    
    
/*     printf("gevasev_selectable_mouse_move() this:%p tracking:%d tx:%d ty:%d _x:%d _y:%d\n", */
/*            object, ev->tracking, ev->tracking_x, ev->tracking_y, _x, _y ); */
/*     if( object ) */
/*     { */
/*         Evas_Coord x=0,y=0; */
/*         gevasobj_get_location( GTK_GEVASOBJ(gevasobj), &x, &y ); */
/*         printf("gevasev_selectable_mouse_move() size:%d getloc.x:%d getlox.y:%d\n", */
/*                sizeof(Evas_Coord), x, y ); */
/*     } */
    
    
    if( ev->tracking )
	{
		gint32 dx=0, dy=0;

		dx = _x - ev->tracking_x;
		dy = _y - ev->tracking_y;
/*         printf("selectable_mouse_move() ev:%p  dx:%d dy:%d  tx:%d ty:%d\n",ev, dx,dy, */
/*                ev->tracking_x, ev->tracking_y );  */

        gevasevh_group_selector_movesel( evh_sel, dx, dy );
		ev->tracking_x = _x;
		ev->tracking_y = _y;

        
        return GEVASEV_HANDLER_RET_CHOMP;
	}

	if( gevasevh_group_selector_isinsel( evh_sel, ev ))
	{
/*		printf("gevasev_selectable_mouse_move() is in sel...\n");*/
		return GEVASEV_HANDLER_RET_CHOMP;
	}

	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_selectable_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHSelectable",
			sizeof(GtkgEvasEvHSelectable),
			sizeof(GtkgEvasEvHSelectableClass),
			(GtkClassInitFunc) gevasevh_selectable_class_init,
			(GtkObjectInitFunc) gevasevh_selectable_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_selectable_class_init(GtkgEvasEvHSelectableClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_selectable_destroy;
	object_class->get_arg = gevasevh_selectable_get_arg;
	object_class->set_arg = gevasevh_selectable_set_arg;

	evh_klass->handler_mouse_in = gevasev_selectable_mouse_in;
	evh_klass->handler_mouse_out = gevasev_selectable_mouse_out;
	evh_klass->handler_mouse_down = gevasev_selectable_mouse_down;
	evh_klass->handler_mouse_up = gevasev_selectable_mouse_up;
	evh_klass->handler_mouse_move = gevasev_selectable_mouse_move;
	evh_klass->get_priority = gevasev_selectable_get_priority;

	gtk_object_add_arg_type(GTK_GEVASEVH_SELECTABLE_SELECTED_OBJ,
		GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_SELECTED_OBJ);

}

static void gevasevh_selectable_init(GtkgEvasEvHSelectable * ev)
{
    ev->gevas = 0;
	ev->selected = 0;
}

GtkObject *gevasevh_selectable_new( GtkObject* _evh_selector )
{
	GtkgEvasEvHSelectable *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_selectable_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->selected = 0;
	ev->border_x = 5;
	ev->border_y = 5;
    ev->evh_selector = _evh_selector;

    return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_selectable_destroy(GtkObject * object)
{
	GtkgEvasEvHSelectable *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object));
	ev = GTK_GEVASEVH_SELECTABLE(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}


static gint col_item_move_absolute(
    GtkgEvasObj* o,
    Evas_Coord* x, Evas_Coord* y,
	GtkgEvasEvHSelectable *ev )
{
    GtkgEvasEvHSelectable* s = 0;

//    printf("col_item_move_absolute(1) x:%p y:%p ev:%p o:%p\n", x,y,ev,o );
    
	g_return_val_if_fail(o     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(x     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(y     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(ev    != NULL, GEVASOBJ_SIG_VETO);
    
	g_return_val_if_fail( GTK_IS_GEVASOBJ(o), GEVASOBJ_SIG_VETO);
	g_return_val_if_fail( GTK_IS_GEVASEVH_SELECTABLE(ev), GEVASOBJ_SIG_VETO);

//    printf("col_item_move_absolute(2) x:%f y:%f\n",*x,*y);

    if( o == ev->selected )
    {

		gevasobj_get_location( ev->normal, x, y );
        *x -= ev->border_x;
        *y -= ev->border_y;
        
    }

    return GEVASOBJ_SIG_OK;
}


static void
gevasevh_selectable_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHSelectable *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object));
	ev = GTK_GEVASEVH_SELECTABLE(object);


	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			ev->selected = GTK_VALUE_POINTER(*arg);

            gevasobj_add_evhandler(ev->selected, object);
            gevas_selectable_set_backref( ev, ev->selected );

            gtk_signal_connect( GTK_OBJECT(ev->selected), "move_absolute",
                                GTK_SIGNAL_FUNC(col_item_move_absolute), ev);

            
            break;
		default:
			break;
	}
}

static void
gevasevh_selectable_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHSelectable *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object));
	ev = GTK_GEVASEVH_SELECTABLE(object);

	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->selected;
			break;
		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
