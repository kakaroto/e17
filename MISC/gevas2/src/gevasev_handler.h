/*
 * Gtk abstraction of event. This lets evas events be chained onto many handlers that are
 * attached in an order to a gevasobj
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


#ifndef INC_GTK_GEVASEV_HANDLER_H
#define INC_GTK_GEVASEV_HANDLER_H

#include <gtk/gtkobject.h>
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH(obj)          GTK_CHECK_CAST (obj, gevasevh_get_type (), GtkgEvasEvH)
#define GTK_GEVASEVH_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_get_type (), GtkgEvasEvHClass)
#define GTK_IS_GEVASEVH(obj)       GTK_CHECK_TYPE (obj, gevasevh_get_type ())
#define GTK_GEVASEVH_GEVASOBJ		"GtkgEvasEvH::gevas_object"
#define gevasevh_set_gevasobj( evh, go ) \
	gtk_object_set(GTK_OBJECT( evh), \
        	GTK_GEVASEVH_GEVASOBJ, (gpointer) go, NULL);

enum _GEVASEV_HANDLER_RET {
	GEVASEV_HANDLER_RET_NEXT,
	GEVASEV_HANDLER_RET_CHOMP	/* event is eaten.*/
};
typedef enum _GEVASEV_HANDLER_RET GEVASEV_HANDLER_RET;

/**/
/* Higher values are called first.*/
/* */
/* FIXME: only HI is done, any other value == lower than HI*/
#define GEVASEV_HANDLER_PRIORITY_HIGHEST	 10000
#define GEVASEV_HANDLER_PRIORITY_HI		 100
#define GEVASEV_HANDLER_PRIORITY_NORMAL 	 0
#define GEVASEV_HANDLER_PRIORITY_LOW 		-100
#define GEVASEV_HANDLER_PRIORITY_LOWEST 	-10000
typedef gint32 GEVASEV_HANDLER_PRIORITY;

typedef struct _GtkgEvasEvH GtkgEvasEvH;
typedef struct _GtkgEvasEvHClass GtkgEvasEvHClass;


struct _GtkgEvasEvH 
{
	GtkObject gobj;
	GtkgEvasObj *eobj;
};

struct _GtkgEvasEvHClass {
	GtkObjectClass parent_class;

	GEVASEV_HANDLER_RET(*handler_mouse_in) (GtkObject * object,
		GtkObject * gevasobj,
		int _b, int _x, int _y);
	 GEVASEV_HANDLER_RET(*handler_mouse_out) (GtkObject * object,
		GtkObject * gevasobj, int _b,
		int _x, int _y);

	GEVASEV_HANDLER_RET(*handler_mouse_down) (GtkObject * object,
		GtkObject * gevasobj, int _b,
		int _x, int _y);

	GEVASEV_HANDLER_RET(*handler_mouse_up) (GtkObject * object,
		GtkObject * gevasobj, int _b,
		int _x, int _y);

	GEVASEV_HANDLER_RET(*handler_mouse_move) (GtkObject * object,
		GtkObject * gevasobj, int _b,
		int _x, int _y);


	/* package */
	GEVASEV_HANDLER_PRIORITY (*get_priority)( GtkgEvasEvH* evh );
};


guint gevasevh_get_type(void);
GtkObject *gevasevh_new(void);

/** Protected access only **/
GEVASEV_HANDLER_RET gevasev_handler_mouse_in(GtkObject * object,
	GtkObject * gevasobj, int _b,
	int _x, int _y);
GEVASEV_HANDLER_RET gevasev_handler_mouse_out(GtkObject * object,
	GtkObject * gevasobj, int _b,
	int _x, int _y);
GEVASEV_HANDLER_RET gevasev_handler_mouse_down(GtkObject * object,
	GtkObject * gevasobj, int _b,
	int _x, int _y);
GEVASEV_HANDLER_RET gevasev_handler_mouse_up(GtkObject * object,
	GtkObject * gevasobj, int _b,
	int _x, int _y);
GEVASEV_HANDLER_RET gevasev_handler_mouse_move(GtkObject * object,
	GtkObject * gevasobj, int _b,
	int _x, int _y);



#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
