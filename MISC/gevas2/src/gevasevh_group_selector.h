/*
 * Draws a ractangle over the selectable items and selects each of them on mouse up.
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


#ifndef INC_GTK_GEVASEVH_GROUP_SELECTOR__H
#define INC_GTK_GEVASEVH_GROUP_SELECTOR__H

#include <gevasev_handler.h>
#include <gevasevh_selectable.h>
#include <gevas.h>
#include <gevasobj.h>
#include <gevas_obj_collection.h>

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */

    
#define GTK_GEVASEVH_GROUP_SELECTOR(obj)          GTK_CHECK_CAST \
        (obj, gevasevh_group_selector_get_type (), GtkgEvasEvHGroupSelector)
    
#define GTK_GEVASEVH_GROUP_SELECTOR_CLASS(klass)  GTK_CHECK_CLASS_CAST \
        (klass, gevasevh_group_selector_get_type (), GtkgEvasEvHGroupSelectorClass)
    
#define GTK_IS_GEVASEVH_GROUP_SELECTOR(obj)       GTK_CHECK_TYPE \
        (obj, gevasevh_group_selector_get_type ())

/*
#define GTK_GEVASEVH_GROUP_SELECTOR_SELECTED_OBJ	"GtkgEvasEvHGroupSelector::selected_gevasobj"
#define gevasevh_group_selector_set_selected_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_GROUP_SELECTOR_SELECTED_OBJ, (gpointer) val, NULL);
*/

typedef struct _GtkgEvasEvHGroupSelector GtkgEvasEvHGroupSelector;
typedef struct _GtkgEvasEvHGroupSelectorClass GtkgEvasEvHGroupSelectorClass;

struct _GtkgEvasEvHGroupSelector 
{
	GtkgEvasEvH evh_obj;
	GtkgEvasObj *mark;	/* object that we are attaching to*/

	gint	tracking;	/* Are we tracking the mouse move.?*/
	gint	tracking_ix;	/* initial location of click that started tracking.*/
	gint	tracking_iy;
	GtkgEvasObj* rect;

	Eina_List* selected_objs;		/* the current selection*/
	GtkgEvasEvHSelectable* selected_objs_lastadded;	/* Last object added to selected_objs;*/

    
    GtkTargetList *drag_targets; /* what types of data the app is willing to create*/
    GdkDragContext *drag_context; /* context of the D&D operation started for this selection*/

    /* Are we dragging part of the selection*/
    /* (this is set by selectable.c whenever it has a mouse down and cleared again by*/
    /* selectable.c when the mouse is up. Thus we can tell right away if the selection*/
    /* is being dragged off the edge of the canvas)*/
    /**/
    gboolean drag_is_dragging;

    
    /* This keeps a collection of all the gevasobjs that are selected */
    GtkgEvasObjCollection* col;

    GtkgEvas* gevas;
    
    // objects which start a rubber band selection
    GSList *m_rubber_band_starters;
};

struct _GtkgEvasEvHGroupSelectorClass {
	GtkgEvasEvHClass parent_class;

    gint (*sig_changed)(GtkgEvasEvHGroupSelector* ev, gpointer d1);
};


guint gevasevh_group_selector_get_type(void);
GtkObject *gevasevh_group_selector_new(void);

/*public*/
    void gevasevh_group_selector_set_object( GtkgEvasEvHGroupSelector* object, GtkgEvasObj* obj );
    Eina_List* gevasevh_group_selector_get_selection_objs(GtkgEvasEvHGroupSelector* ev );
    Eina_List* gevasevh_group_selector_get_selected_selectables(GtkgEvasEvHGroupSelector* ev );
    void gevasevh_group_selector_set_drag_targets(GtkgEvasEvHGroupSelector* ev, GtkTargetList *dt );

/*package*/
    void gevasevh_group_selector_addtosel( GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o );
    void gevasevh_group_selector_floodtosel(
        GtkgEvasEvHGroupSelector* ev,
        GtkgEvasEvHSelectable* o,
        GtkgEvasObj* go);
        
    void gevasevh_group_selector_remfromsel( GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o );
    void gevasevh_group_selector_flushsel(GtkgEvasEvHGroupSelector* ev );
    void gevasevh_group_selector_movesel(GtkgEvasEvHGroupSelector* ev, gint32 dx, gint32 dy );
    gboolean gevasevh_group_selector_isinsel(GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o );
    void gevas_group_selector_get_wh(GtkgEvasEvHGroupSelector *ev,
                                     gint  cx, gint cy,
                                     Evas_Coord* x, Evas_Coord* y,
                                     Evas_Coord* rw, Evas_Coord* rh);
    void gevasevh_group_selector_dragging( GtkgEvasEvHGroupSelector *ev, gboolean d );

    GtkgEvasObjCollection* gevasevh_group_selector_get_collection( GtkgEvasEvHGroupSelector *ev );


    void gevasevh_group_selector_add_rubber_band_starter( GtkgEvasEvHGroupSelector *ev, GtkObject* obj );
    

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
