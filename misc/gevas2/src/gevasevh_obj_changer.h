/*
 * changes the gevasobj to another on mouse in/out and mouse click (1,2,3)
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


#ifndef INC_GTK_GEVASEVH_OBJ_CHANGER__H
#define INC_GTK_GEVASEVH_OBJ_CHANGER__H

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_OBJ_CHANGER(obj)          GTK_CHECK_CAST (obj, gevasevh_obj_changer_get_type (), GtkgEvasEvHObjChanger)
#define GTK_GEVASEVH_OBJ_CHANGER_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_obj_changer_get_type (), GtkgEvasEvHObjChangerClass)
#define GTK_IS_GEVASEVH_OBJ_CHANGER(obj)       GTK_CHECK_TYPE (obj, gevasevh_obj_changer_get_type ())
#define GTK_GEVASEVH_OBJ_CHANGER_COLD_OBJ		"GtkgEvasEvHObjChanger::cold_gevasobj"
#define GTK_GEVASEVH_OBJ_CHANGER_HOT_OBJ		"GtkgEvasEvHObjChanger::hot_gevasobj"
#define GTK_GEVASEVH_OBJ_CHANGER_HOTCLK1_OBJ	"GtkgEvasEvHObjChanger::hot_clk1_gevasobj"
#define GTK_GEVASEVH_OBJ_CHANGER_HOTCLK2_OBJ	"GtkgEvasEvHObjChanger::hot_clk2_gevasobj"
#define GTK_GEVASEVH_OBJ_CHANGER_HOTCLK3_OBJ	"GtkgEvasEvHObjChanger::hot_clk3_gevasobj"
#define gevasevh_obj_changer_set_cold_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_OBJ_CHANGER_COLD_OBJ, (gpointer) val, NULL);
#define gevasevh_obj_changer_set_hot_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_OBJ_CHANGER_HOT_OBJ, (gpointer) val, NULL);
#define gevasevh_obj_changer_set_hot_click1_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_OBJ_CHANGER_HOTCLK1_OBJ, (gpointer) val, NULL);
#define gevasevh_obj_changer_set_hot_click2_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_OBJ_CHANGER_HOTCLK2_OBJ, (gpointer) val, NULL);
#define gevasevh_obj_changer_set_hot_click3_gevasobj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_OBJ_CHANGER_HOTCLK3_OBJ, (gpointer) val, NULL);
	 typedef struct _GtkgEvasEvHObjChanger GtkgEvasEvHObjChanger;
	typedef struct _GtkgEvasEvHObjChangerClass GtkgEvasEvHObjChangerClass;



	struct _GtkgEvasEvHObjChanger {
		GtkgEvasEvH evh_obj;

		GtkgEvasObj *cold;
		GtkgEvasObj *hot;
		GtkgEvasObj *hot_clicked[5];
		GtkgEvasObj *current;	/* what are we showing, state machine*/
		int old_layer;

	};

	struct _GtkgEvasEvHObjChangerClass {
		GtkgEvasEvHClass parent_class;
	};


	guint gevasevh_obj_changer_get_type(void);
	GtkObject *gevasevh_obj_changer_new(void);

#define GTK_GEVASEVH_OBJ_CHANGER_TOP_LAYER	999


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
