/*
 * Draws an image behind the given evasobj when told to.
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


#ifndef INC_GTK_GEVASEVH_SELECTABLE__H
#define INC_GTK_GEVASEVH_SELECTABLE__H

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_SELECTABLE(obj)          GTK_CHECK_CAST (obj, gevasevh_selectable_get_type (), GtkgEvasEvHSelectable)
#define GTK_GEVASEVH_SELECTABLE_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_selectable_get_type (), GtkgEvasEvHSelectableClass)
#define GTK_IS_GEVASEVH_SELECTABLE(obj)       GTK_CHECK_TYPE (obj, gevasevh_selectable_get_type ())


#define GTK_GEVASEVH_SELECTABLE_SELECTED_OBJ	"GtkgEvasEvHSelectable::selected_gevasobj"
#define gevasevh_selectable_set_selected_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_SELECTABLE_SELECTED_OBJ, (gpointer) val, NULL);


typedef struct _GtkgEvasEvHSelectable GtkgEvasEvHSelectable;
typedef struct _GtkgEvasEvHSelectableClass GtkgEvasEvHSelectableClass;

struct _GtkgEvasEvHSelectable 
{
	GtkgEvasEvH evh_obj;
	GtkgEvasObj *normal;
	GtkgEvasObj *selected;

	gboolean tracking;		// Are we tracking the mouse movement
	gint32 tracking_x;		// The x,y of the mouse on mouse down
	gint32 tracking_y;

	GtkObject* evh_selector;

	gint32  border_x;
	gint32  border_y;
};

#define gevasevh_selectable_gevasobj( a ) ((GTK_GEVASEVH_SELECTABLE(a))->normal)

struct _GtkgEvasEvHSelectableClass {
	GtkgEvasEvHClass parent_class;
};


guint gevasevh_selectable_get_type(void);
GtkObject *gevasevh_selectable_new(void);


// public
void gevasevh_selectable_set_normal_gevasobj(
	GtkgEvasEvHSelectable* ev, 
	GtkgEvasObj* nor );
void gevasevh_selectable_set_selector( GtkgEvasEvHSelectable* evh, GtkObject* evh_selector );




// package:
void gevas_selectable_select( GtkgEvasEvHSelectable * ev, gboolean s );
void gevas_selectable_move( GtkgEvasEvHSelectable * ev, gint32 dx, gint32 dy );
#define GEVASEVH_SELECTABLE_KEY		"GEVASEVH_SELECTABLE_KEY"



#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
