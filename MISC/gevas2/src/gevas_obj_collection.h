/*
 * Maintains a collection of gevas objects that can have methods applied to all.
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

#ifndef INC_GTK_GEVAS_OBJ_COLLECTION__H
#define INC_GTK_GEVAS_OBJ_COLLECTION__H

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>
#include <gevasev_handler.h>
#include <gevas.h>
#include <gevasobj.h>

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */

    
    
#define GTK_GEVAS_OBJ_COLLECTION(obj) GTK_CHECK_CAST \
        (obj, gevas_obj_collection_get_type (), GtkgEvasObjCollection)
    
#define GTK_GEVAS_OBJ_COLLECTION_CLASS(klass)  GTK_CHECK_CLASS_CAST \
        (klass, gevas_obj_collection_get_type (), GtkgEvasObjCollectionClass)
        
#define GTK_IS_GEVAS_OBJ_COLLECTION(obj)       GTK_CHECK_TYPE \
        (obj, gevas_obj_collection_get_type ())

    
    typedef struct _GtkgEvasObjCollection GtkgEvasObjCollection;
    typedef struct _GtkgEvasObjCollectionClass GtkgEvasObjCollectionClass;

#include <gevasevh_selectable.h>

    
    /* All objects in the selection must be a subclass of this type */
    typedef GtkgEvasObj* GtkgEvasObjCollection_T;
    typedef gboolean (*add_predicate_t) (
        GtkgEvasObjCollection* ev,
        GtkgEvasObjCollection_T o,
        gpointer udata);
    

    
    
    struct _GtkgEvasObjCollection
    {
        GtkObject gobj;

        /* the current selection. the data stored is of type GtkgEvasObjCollection_T */
        Eina_List* selected_objs;
        

        /* Last object added to selected_objs */
        GtkgEvasObjCollection_T lastadded;


        /* This function is called whenever adding to the collection */
        /* the predicate must return 1 for the object to be added    */
        add_predicate_t m_pred;
        gpointer m_pred_udata;

        
        /* cache the gevas we are attached to */
        GtkgEvas* gevas;

    };

    struct _GtkgEvasObjCollectionClass {
        GtkObjectClass parent_class;

        gboolean (*sig_add)(GtkgEvasObjCollection* ev, GtkObject* gevasobj);
        gboolean (*sig_remove)(GtkgEvasObjCollection* ev, GtkObject* gevasobj);
        

    };


    guint gevas_obj_collection_get_type(void);
    GtkgEvasObjCollection* gevas_obj_collection_new(GtkgEvas* _gevas);
    

/* public */

    void gevas_obj_collection_set_add_predicate(
        GtkgEvasObjCollection* ev,
        gpointer udata,
        add_predicate_t p);
    
    GtkgEvasObjCollection_T
    gevas_obj_collection_element_n( GtkgEvasObjCollection* ev, gint n );
    gint gevas_obj_collection_element_idx_from_name(
        GtkgEvasObjCollection* ev,
        gint start,
        const char* name );
    gint gevas_obj_collection_element_idx_from_namei(
        GtkgEvasObjCollection* ev,
        gint start,
        const char* name );
    

    void gevas_obj_collection_add( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o );
    void gevas_obj_collection_add_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s);
    void gevas_obj_collection_add_flood( GtkgEvasObjCollection* ev,
                                         GtkgEvasObjCollection_T o1,
                                         GtkgEvasObjCollection_T o2);
    void gevas_obj_collection_add_flood_area( GtkgEvasObjCollection* ev,
                                              double x, double y,
                                              double w, double h);
    
    GtkgEvasObjCollection_T gevas_obj_collection_get_lastadded( GtkgEvasObjCollection* ev );
    
    void gevas_obj_collection_remove( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o );
    void gevas_obj_collection_remove_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s);
    void gevas_obj_collection_clear(  GtkgEvasObjCollection* ev );
    void gevas_obj_collection_move(   GtkgEvasObjCollection* ev, gint32 x, gint32 y );
    void gevas_obj_collection_move_relative( GtkgEvasObjCollection* ev, gint32 dx, gint32 dy );
    gint gevas_obj_collection_get_size(  GtkgEvasObjCollection* ev );
    void gevas_obj_collection_hide( GtkgEvasObjCollection* ev );
    void gevas_obj_collection_show( GtkgEvasObjCollection* ev );
    void gevas_obj_collection_set_visible(  GtkgEvasObjCollection* ev, gboolean v );
    gboolean gevas_obj_collection_contains( GtkgEvasObjCollection* ev, GtkgEvasObjCollection_T o );
    gboolean gevas_obj_collection_contains_all( GtkgEvasObjCollection* ev, GtkgEvasObjCollection*s);
    Eina_List* gevas_obj_collection_to_eina_list( GtkgEvasObjCollection* ev );
    
    
/* package */
    void gevas_obj_collection_dump( GtkgEvasObjCollection* ev, Eina_List* li );
    


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
