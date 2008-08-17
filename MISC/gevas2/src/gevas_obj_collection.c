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


 Should use something other than Evas_List* for better speed.

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


/**
 * List of type GtkgEvasObjCollection_T containing all the objects in this collection.
 *
 * Caller must free the return value with evas_list_free();
 */
Evas_List*
gevas_obj_collection_to_evas_list( GtkgEvasObjCollection* ev )
{
    Evas_List* ret = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    
    {
        Evas_List* tl = ev->selected_objs;
    
        while(tl)
        {
            if( tl->data )
            {
                ret = evas_list_append( ret, tl->data );
            }
            tl = tl->next;
        }
    }
    
return ret;
}

/**
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


/**
 * Get the element at logical index n in the collection.
 */
GtkgEvasObjCollection_T
gevas_obj_collection_element_n( GtkgEvasObjCollection* ev, gint n )
{
    gint i=0;
    Evas_List* li=0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (n >= 0);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            if( i++ == n )
            {
                return li->data;
            }
        }
    return 0;
}


/**
 *
 * Given a starting index and name find the logical index of the cell
 * that has the given name. The comparison looking for name can be
 * sensitive if case_sensitive==1
 *
 * @param ev              this
 * @param start           the index to start looking from
 * @param name            the name of the cell to lookup
 * @param case_sensitive  If true, then the comparison lookup is sensitive to case.
 *                        Else strcasecmp() is used.
 *
 */
static gint gevas_obj_collection_element_idx_from_name_internal(
    GtkgEvasObjCollection* ev,
    gint start,
    const char* name,
    gboolean case_sensitive )
{
    gint i=0;
    Evas_List* li=0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    g_return_if_fail (name != NULL);
    g_return_if_fail (start >= 0);

    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            if( i >= start )
            {
                if( case_sensitive )
                {
                    if( !strcmp(name, gevasobj_get_name(li->data)))
                    {
                        return i;
                    }
                }
                else
                {
                    if( !strcasecmp(name, gevasobj_get_name(li->data)))
                    {
                        return i;
                    }
                }
            }
            ++i;
        }
    return -1;
}


/**
 *
 * Given a starting index and name find the logical index of the cell
 * that has the given name. This is a case sensitive lookup.
 *
 * @param ev              this
 * @param start           the index to start looking from
 * @param name            the name of the cell to lookup
 *
 * @see gevas_obj_collection_element_idx_from_namei
 */
gint gevas_obj_collection_element_idx_from_name(
    GtkgEvasObjCollection* ev,
    gint start,
    const char* name )
{
    return gevas_obj_collection_element_idx_from_name_internal(ev,start,name,1);
}


/**
 *  Get the logical index of the cell with a given name. The name
 *  comparison is not case sensitive.
 *
 *
 * @param ev    this
 * @param start index to start looking from
 * @param name  the name of the cell to find the index of. This is a case
 *              insensitive compare.
 *
 * @see gevas_obj_collection_element_idx_from_name
 */
gint gevas_obj_collection_element_idx_from_namei(
    GtkgEvasObjCollection* ev,
    gint start,
    const char* name )
{
    return gevas_obj_collection_element_idx_from_name_internal(ev,start,name,0);
}



/**
 * Add object 'o' to the collection 'ev'. If 'o' is already in 'ev' then
 * nothing is done. Otherwise 'o' is the last cell in 'ev'.
 *
 * Note that object 'o' is not copied, it is inserted into 'ev' as is.
 *
 * This function emits the ADD signal.
 *
 * @param ev    this
 * @param o     new object to add to ev.
 *
 * @see gevas_obj_collection_add_all
 */
void
gevas_obj_collection_add( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    /* Should this be an assert or a return... hmm */
    g_return_if_fail (o != NULL);
    if(!o) return;

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


/**
 * Set style operation. Add all cells in 's' to 'ev'.
 * If the cell in 's' already exists in 'ev' then the cell is not added.
 * All cells from 's' that are added are added in order of appearance from 's'
 * at the end of 'ev'.
 *
 * Note that all objects from 's' are not copied, they are inserted into 'ev' as is.
 *
 * @param ev this
 * @param s  the source for all new cells to add to ev.
 *
 * @see gevas_obj_collection_add
 */
void
gevas_obj_collection_add_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s)
{
    Evas_List* li;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    g_return_if_fail (s != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(s));

    for( li=s->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevas_obj_collection_add( ev, li->data );
        }
}


/**
 *  Add all the gevas objects that are in the area defined by a rectangle starting
 * at x,y and with width(w) and height(h) to 'ev'.
 *
 * Note that objects are not copied, they are added as is. ie. shallow copy.
 *
 * @param ev   this
 * @param x    x of top left of rectangle
 * @param y    y of top left of rectangle
 * @param w    width of rectangle
 * @param h    height of rectangle
 *
 * @see gevas_obj_collection_add_flood
 */
void
gevas_obj_collection_add_flood_area( GtkgEvasObjCollection* ev,
                                     double x, double y, double w, double h)
{
    Evas_List* list;
	void* data;

    /*printf("gevas_obj_collection_add_flood_area() x:%f y:%f w:%f h:%f\n",x,y,w,h);*/
    
    
    if (!(w > 0 && h > 0))
        return;
    
    g_return_if_fail (ev != NULL);
    g_return_if_fail (ev->gevas != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    
    list = evas_objects_in_rectangle_get( gevas_get_evas(ev->gevas), x,y,w,h, 0, 0 );

	while( list )
	{
        int x = 4;
        GtkgEvasObj* go = gevasobj_from_evasobj(list->data);
        GtkgEvasEvHSelectable* s = 0;

        if( go )
        {
            gevas_obj_collection_add( ev, go );
            if( s = gevas_selectable_get_backref( ev->gevas, go ))
            {
                GtkgEvasObjCollection* c = 0;
                c = gevasevh_selectable_to_collection( s );
                gevas_obj_collection_add_all( ev, c );
                gtk_object_destroy(GTK_OBJECT(c));
//                gtk_object_unref(GTK_OBJECT(c));
            }
        }
        list = list->next;
	}

}


/**
 *  Add all the gevas objects that are contained in a rectangle that
 * is just large enough to contain both of o1 and o2.
 *
 * Note that all objects are shallow copied.
 *
 * @param ev   this
 * @param o1   An object to be one corner of the bounding rectangle
 * @param o2   An object to be one corner of the bounding rectangle
 *
 * @see gevas_obj_collection_add_flood_area
 */
void
gevas_obj_collection_add_flood( GtkgEvasObjCollection* ev,
                                GtkgEvasObjCollection_T o1,
                                GtkgEvasObjCollection_T o2)
{
	GtkgEvasObjCollection_T tl = o1; /* top left*/
	GtkgEvasObjCollection_T br = o2; /* bottom right*/
	Evas_Coord x=0,y=0,w=0,h=0,_x=0,_y=0,_w=0,_h=0;

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


/**
 *  Get a pointer to the last added object for this collection. This
 * applies for set based operations aswell.
 *
 * @param ev this
 *
 */
GtkgEvasObjCollection_T
gevas_obj_collection_get_lastadded( GtkgEvasObjCollection* ev )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    return ev->lastadded;
}



/**
 *  Remove the object 'o' from the collection. If 'o' is the last added then
 * gevas_obj_collection_get_lastadded() will return 0 after this call.
 *
 * This function emits the REMOVE signal.
 *
 * @param ev this
 * @param o  object to remove
 */
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


/**
 * This is a set based operation. All items that are in 's' and that are in 'ev'
 * when this function is called are no longer in 'ev' then the function returns.
 *
 * For each item that is removed from the collection REMOVE is emitted.
 *
 * @param ev this
 * @param s  the set that contains all the objects that should be removed from ev
 *
 */
void
gevas_obj_collection_remove_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s)
{
    Evas_List* li;
    Evas_List* lin;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (s  != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(s));

    for( li=s->selected_objs; li; )
        if(li->data)
        {
            lin = li->next;
/*             printf("gevas_obj_collection_remove_all() ev:%p s:%p data:%p\n",ev,s,li->data); */
            gevas_obj_collection_remove( ev, li->data );
            li = lin;
        }
}



/**
 * Remove all of the objects in 'ev'.
 *
 * For each object that is removed from 'ev' REMOVE is emitted.
 *
 * @param ev this
 *
 */
void
gevas_obj_collection_clear(  GtkgEvasObjCollection* ev )
{
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

	ev->lastadded = 0;
    gevas_obj_collection_remove_all( ev, ev );
    
}


/**
 * For each object in the whole collection, move it to x,y.
 *
 * @param ev this
 * @param x  new x location for each object in the collection.
 * @param y  new y location for each object in the collection.
 * 
 */
/* FIXME: */
/*  need to worry about confine() from selectable in here  */
/*  need bx,by  */
void
gevas_obj_collection_move(   GtkgEvasObjCollection* ev, gint32 x, gint32 y )
{
    Evas_List* li = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));


    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevasobj_move( li->data, x, y );
        }
}


/**
 * Shift each object in the collection by dx,dy.
 *
 *
 * @param ev this
 * @param dx delta of the x value for the move of each object
 * @param dy delta of the y value for the move of each object
 *
 */
void
gevas_obj_collection_move_relative( GtkgEvasObjCollection* ev, gint32 dx, gint32 dy )
{
    Evas_List* li = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));


    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
//            gevasobj_move_relative( li->data, (double)dx, (double)dy );

            double ddx = 0;
            double ddy = 0;
            ddx += dx;
            ddy += dy;
            
            if( li->data )
            {
                Evas_Coord x=0,y=0;
//                gevasobj_get_location( GTK_GEVASOBJ(li->data), &x, &y );
                char buffer[1024];
                snprintf(buffer,200,"FIXME FIXME FIXME() size:%d getloc.x:%d getlox.y:%d %f \n",
                       sizeof(Evas_Coord), x, y, 0.0 );
            }

            gevasobj_move_relative( li->data, ddx, ddy );
        }
}


/**
 * The number of cells in this collection.
 *
 * @param ev this
 *
 */
gint
gevas_obj_collection_get_size(  GtkgEvasObjCollection* ev )
{
    Evas_List* tl = 0;
    gint ret = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for(tl = ev->selected_objs; tl; tl = tl->next)
	{
		if( tl->data )
		{
            ++ret;
		}
	}
    return ret;
}


/**
 * Hide every object in the collection
 */
void
gevas_obj_collection_hide( GtkgEvasObjCollection* ev )
{
    gevas_obj_collection_set_visible( ev, 0 );
}

/**
 * Show every object in the collection
 */
void
gevas_obj_collection_show( GtkgEvasObjCollection* ev )
{
    gevas_obj_collection_set_visible( ev, 1 );
}

/**
 * Set the visibiliy of every object in the collection
 *
 * @param ev this
 * @param v  if true then every object in the collection will be visible.
 *           otherwise every object will be hidden.
 */
void
gevas_obj_collection_set_visible(  GtkgEvasObjCollection* ev, gboolean v )
{
    Evas_List* li = 0;

    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    for( li=ev->selected_objs; li; li = li->next)
        if(li->data)
        {
            gevasobj_set_visible( li->data, v );
        }
}


/**
 *  Return true of 'ev' contains the object 'o'.
 */
gboolean gevas_obj_collection_contains( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o )
{
    Evas_List* li = 0;

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


/**
 * Debug code. Dumps some meta data about the contents of the collection to
 * stdout.
 */
void gevas_obj_collection_dump( GtkgEvasObjCollection* ev, Evas_List* li )
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


/**
 * Set based operation. Returns true of 's' is a subset of 'ev'
 */
gboolean gevas_obj_collection_contains_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s)
{
    Evas_List* li = 0;

    g_return_if_fail (s != NULL);
    g_return_if_fail (ev != NULL);
    g_return_if_fail (GTK_IS_GEVAS_OBJ_COLLECTION(ev));

    
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
/** Kruft that is needed to make this a Gtk+ Object follows. ******************/
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

static gboolean sig_add(GtkgEvasObjCollection* ev, GtkObject* gevasobj)
{
    return 0;
}
static gboolean sig_remove(GtkgEvasObjCollection* ev, GtkObject* gevasobj)
{
    return 0;
}



static void
gevas_obj_collection_class_init(GtkgEvasObjCollectionClass* klass)
{
    GObjectClass*   gobject_class = G_OBJECT_CLASS (klass);
    GtkObjectClass* object_class  = GTK_OBJECT_CLASS (klass);                      

	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevas_obj_collection_destroy;
    object_class->get_arg = gevas_obj_collection_get_arg;
	object_class->set_arg = gevas_obj_collection_set_arg;

    klass->sig_add = sig_add;
    klass->sig_remove = sig_remove;

    signals[SIG_ADD] =
        gtk_signal_new ("add",
                        GTK_RUN_LAST,
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET (GtkgEvasObjCollectionClass, sig_add),
                        gtk_marshal_NONE__POINTER,
                        GTK_TYPE_NONE, 1,
                        GTK_TYPE_OBJECT);
    
    signals[SIG_REMOVE] =
        gtk_signal_new ("remove",
                        GTK_RUN_LAST,
                        GTK_CLASS_TYPE(object_class),
                        GTK_SIGNAL_OFFSET (GtkgEvasObjCollectionClass, sig_remove),
                        gtk_marshal_NONE__POINTER,
                        GTK_TYPE_NONE, 1,
                        GTK_TYPE_OBJECT);


/*     gtk_object_class_add_signals (object_class, signals, SIG_LAST); */


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

