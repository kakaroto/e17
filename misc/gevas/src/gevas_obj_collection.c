/*
 * Maintains a collection of gevas objects that can have methods applied to all.
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


/*
  FIXME: make the _all() functions use a foreach() style if at all possible.


 Should use something other than Evas_List for better speed.

 Basically I want.
   A set (C) is the underlying collection. (no order, no dups)
   
   (FD A): int -> gevasobj (the resultant will be in (C))
                       this is used to lookup a frame by number in its sequence

   (FD B): string -> int   (this can find a frame by its name, the resultant is
                           then used to find the gevasobj with (FD A)
 
    FD == Functional dependancy.                            
  

*/

#include "config.h"
#include <gevas_obj_collection.h>

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


#include <gevasobj.h>
#include <gevasevh_selectable.h>

#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>


static void gevas_obj_collection_class_init(GtkgEvasObjCollectionClass * klass);
static void gevas_obj_collection_init(GtkgEvasObjCollection* ev);
/* GtkObject functions */
static void gevas_obj_collection_destroy(GtkObject * object);
static void gevas_obj_collection_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevas_obj_collection_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_DUMMY,
};


enum {
    SIG_ADD,
    SIG_REMOVE,
    SIG_LAST
};

static guint signals[SIG_LAST] = { 0 };


/*
 * List of type GtkgEvasObjCollection_T containing all the objects in this collection.
 *
 * Caller must free the return value with evas_list_free();
 */
Evas_List
gevas_obj_collection_to_evas_list( GtkgEvasObjCollection* ev )
{
	Evas_List ret = 0;
    Evas_List tl = ev->selected_objs;
    
	while(tl)
	{
		if( tl->data )
		{
			ret = evas_list_append( ret, tl->data );
		}
		tl = tl->next;
	}
return ret;
}

/*
 * Set the predicate that must pass for items to be added to this collection
 */
void gevas_obj_collection_set_add_predicate(
    GtkgEvasObjCollection* ev,
    gpointer udata,
    add_predicate_t p)
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    
    ev->m_pred = p;
    ev->m_pred_udata = udata;
}



void
gevas_obj_collection_add( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    if(!o)
        return;

    if( !ev->m_pred || ev->m_pred(ev,o,ev->m_pred_udata))
    {
        if(!evas_list_find(ev->selected_objs, o))
        {
            ev->selected_objs = evas_list_append( ev->selected_objs, o);
            ev->lastadded = o;

            gtk_signal_emit(GTK_OBJECT(ev), signals[SIG_ADD], GTK_OBJECT(o));
        }
        
    }
        
}

void
gevas_obj_collection_add_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s)
{
    Evas_List li;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for( li=s->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevas_obj_collection_add( ev, li->data );
        }
}



void
gevas_obj_collection_add_flood_area( GtkgEvasObjCollection* ev,
                                     double x, double y, double w, double h)
{
    Evas_List list;
	void* data;

    printf("gevas_obj_collection_add_flood_area() x:%f y:%f w:%f h:%f\n",x,y,w,h);
    
    
    if (!(w > 0 && h > 0))
        return;
    
    g_return_if_fail (ev != NULL);
    g_return_if_fail (ev->gevas != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    

	list = evas_objects_in_rect( gevas_get_evas(ev->gevas), x,y,w,h);

	while( list )
	{
        int x = 4;
        GtkgEvasObj* go = gevasobj_from_evasobj(ev->gevas,list->data);
        GtkgEvasEvHSelectable* s = 0;

        if( go )
        {
            gevas_obj_collection_add( ev, go );
            if( s = gevas_selectable_get_backref( ev->gevas, go ))
            {
                GtkgEvasObjCollection* c = 0;
                c = gevasevh_selectable_to_collection( s );
                gevas_obj_collection_add_all( ev, c );
                gtk_object_unref(c);
            }
        }
        list = list->next;
	}

}


void
gevas_obj_collection_add_flood( GtkgEvasObjCollection* ev,
                                GtkgEvasObjCollection_T o1,
                                GtkgEvasObjCollection_T o2)
{
	GtkgEvasObjCollection_T tl = o1; /* top left*/
	GtkgEvasObjCollection_T br = o2; /* bottom right*/
	double x=0,y=0,w=0,h=0,_x=0,_y=0,_w=0,_h=0;

    g_return_if_fail (o1 != NULL);
    g_return_if_fail (o2 != NULL);
    g_return_if_fail (o1 != o2);
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    g_return_if_fail (GTK_IS_GEVASOBJ(o1));
    g_return_if_fail (GTK_IS_GEVASOBJ(o2));


    
    /*
     * Make sure that tl and br are assigned to the correct object.
     */
    gevasobj_get_location( GTK_GEVASOBJ(tl), &x, &y );
	gevasobj_get_location( GTK_GEVASOBJ(br), &_x, &_y );
	if( x > _x && y > _y )
	{
		tl = o2;
		br = o1;
	}

    /*
     * Work out the area covered by tl -> br.
     */
	gevasobj_get_location( GTK_GEVASOBJ(tl), &x, &y );
	gevasobj_get_geometry( GTK_GEVASOBJ(br), &_x, &_y, &_w, &_h );
    w = _x + _w - x;
	h = _y + _h - y;


    gevas_obj_collection_add_flood_area( ev, x, y, w, h );
    ev->lastadded = o2;
}

GtkgEvasObjCollection_T
gevas_obj_collection_get_lastadded( GtkgEvasObjCollection* ev )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    return ev->lastadded;
}



void
gevas_obj_collection_remove( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o )
{
    g_return_if_fail (o != NULL);
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    if( o == ev->lastadded)
    {
        ev->lastadded = 0;
    }
    
	ev->selected_objs = evas_list_remove( ev->selected_objs, o );
    gtk_signal_emit(GTK_OBJECT(ev), signals[SIG_REMOVE], GTK_OBJECT(o));
    
}

void
gevas_obj_collection_remove_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s)
{
    Evas_List li;
    Evas_List lin;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (s  != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(s));

    for( li=s->selected_objs; li; )
        if(li->data)
        {
            lin = li->next;
            printf("gevas_obj_collection_remove_all() ev:%p s:%p data:%p\n",ev,s,li->data);
            gevas_obj_collection_remove( ev, li->data );
            li = lin;
        }
}




void
gevas_obj_collection_clear(  GtkgEvasObjCollection* ev )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

	ev->lastadded = 0;
    gevas_obj_collection_remove_all( ev, ev );
    
}


// need to worry about confine() from selectable in here //
// need bx,by //
void
gevas_obj_collection_move(   GtkgEvasObjCollection* ev, gint32 x, gint32 y )
{
    Evas_List li = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));


    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevasobj_move( li->data, x, y );
        }
}


void
gevas_obj_collection_move_relative( GtkgEvasObjCollection* ev, gint32 dx, gint32 dy )
{
	double x=0, y=0;
    Evas_List li = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));


    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevasobj_get_location( li->data, &x, &y );
            x+=dx;	
            y+=dy;

//    _gevas_selectable_confine( ev, ev->normal->gevas, &x, &y );
    
            gevasobj_move( li->data, x, y );
        }
}

void
gevas_obj_collection_hide( GtkgEvasObjCollection* ev )
{
    gevas_obj_collection_set_visible( ev, 0 );
}

void
gevas_obj_collection_show( GtkgEvasObjCollection* ev )
{
    gevas_obj_collection_set_visible( ev, 1 );
}

void
gevas_obj_collection_set_visible(  GtkgEvasObjCollection* ev, gboolean v )
{
    Evas_List li = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevasobj_set_visible( li->data, v );
        }
}

gboolean gevas_obj_collection_contains( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o )
{
    Evas_List li = 0;

    g_return_if_fail (o != NULL);
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for( li=ev->selected_objs; li; li = li->next)
        if(li->data == o)
        {
            return 1;
        }
    
    return 0;
}


void gevas_obj_collection_dump( GtkgEvasObjCollection* ev, Evas_List li )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for( ; li; li = li->next)
        if(li->data)
        {
            printf(" %p", li->data);
        }
    printf("\n");
    
}


/*
 * is "s" a subset of "ev"
 */
gboolean gevas_obj_collection_contains_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s)
{
    Evas_List li = 0;

    g_return_if_fail (s != NULL);
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

/*
  printf("gevas_obj_collection_contains_all() ev:%p ...\n",ev);
  printf("us:\n");   gevas_obj_collection_dump( ev, ev->selected_objs );
  printf("them:\n"); gevas_obj_collection_dump( ev, s->selected_objs );
*/  
    
    for( li=s->selected_objs; li; li = li->next)
        if(li->data)
        {
            
            if( !gevas_obj_collection_contains(ev, li->data)) 
                return 0;
        }
    
    return 1;
}




/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


static GtkObjectClass *parent_class = NULL;

guint
gevas_obj_collection_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasObjCollection",
			sizeof(GtkgEvasObjCollection),
			sizeof(GtkgEvasObjCollectionClass),
			(GtkClassInitFunc)  gevas_obj_collection_class_init,
			(GtkObjectInitFunc) gevas_obj_collection_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}

static void
gevas_obj_collection_class_init(GtkgEvasObjCollectionClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevas_obj_collection_destroy;
    object_class->get_arg = gevas_obj_collection_get_arg;
	object_class->set_arg = gevas_obj_collection_set_arg;

    signals[SIG_ADD] =
        gtk_signal_new ("add", GTK_RUN_LAST, object_class->type, 0,
                        gtk_marshal_NONE__POINTER,
                        GTK_TYPE_NONE, 1,
                        GTK_TYPE_OBJECT);
    signals[SIG_REMOVE] =
        gtk_signal_new ("remove", GTK_RUN_LAST, object_class->type, 0,
                        gtk_marshal_NONE__POINTER,
                        GTK_TYPE_NONE, 1,
                        GTK_TYPE_OBJECT);
    
    gtk_object_class_add_signals (object_class, signals, SIG_LAST);


}

static void
gevas_obj_collection_init(GtkgEvasObjCollection * ev)
{
    ev->gevas         = 0;
    ev->selected_objs = 0;
    ev->lastadded     = 0;
    ev->m_pred        = 0;
    ev->m_pred_udata  = 0;
    
}



GtkgEvasObjCollection*
gevas_obj_collection_new(GtkgEvas* _gevas)
{
	GtkgEvasObjCollection *ev;

	ev = gtk_type_new(gevas_obj_collection_get_type());
    
    ev->gevas = _gevas;

    return GTK_GEVAS_OBJ_COLLECTION(ev);
}

/* GtkObject functions */


static void
gevas_obj_collection_destroy(GtkObject * object)
{
	GtkgEvasObjCollection *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(object));
    ev = GTK_GEVAS_OBJ_COLLECTION(object);

    

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}


static void
gevas_obj_collection_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasObjCollection *ev;
	gchar *gstr;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(object));
    ev = GTK_GEVAS_OBJ_COLLECTION(object);

	switch (arg_id)
    {
        
    case ARG_DUMMY:
        break;

    default:
        break;
        
	}
}

static void
gevas_obj_collection_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasObjCollection *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(object));
    ev = GTK_GEVAS_OBJ_COLLECTION(object);

	switch (arg_id)
    {
        
    default:
        arg->type = GTK_TYPE_INVALID;
        break;
        
	}
}

