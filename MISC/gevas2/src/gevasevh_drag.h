/*
 * Gtk abstraction of left dragging an object around a gevas
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


#ifndef INC_GTK_GEVASEVH_DRAG__H
#define INC_GTK_GEVASEVH_DRAG__H

#include <gtk/gtkobject.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_DRAG(obj)          GTK_CHECK_CAST (obj, gevasevh_drag_get_type (), GtkgEvasEvHDrag)
#define GTK_GEVASEVH_DRAG_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_drag_get_type (), GtkgEvasEvHDragClass)
#define GTK_IS_GEVASEVH_DRAG(obj)       GTK_CHECK_TYPE (obj, gevasevh_drag_get_type ())
	 typedef struct _GtkgEvasEvHDrag GtkgEvasEvHDrag;
	typedef struct _GtkgEvasEvHDragClass GtkgEvasEvHDragClass;

	struct _GtkgEvasEvHDrag {
		GtkgEvasEvH evh_obj;
		gint dragging;
		gdouble click_x_offset;	/* when the mouse down occurs store the offset into the Evas_Object */
		gdouble click_y_offset;	/* so that the move handler can set x/y properly */

        /* When we are dragging we snapshot the visible area of the gevas so that
         * the users drag operations do not allow objects off the viewable area (optional) */  
        gint canvas_x;
        gint canvas_y;
        gint canvas_w;
        gint canvas_h;
        
	};

	struct _GtkgEvasEvHDragClass {
		GtkgEvasEvHClass parent_class;
	};


	guint gevasevh_drag_get_type(void);
	GtkObject *gevasevh_drag_new(void);

/** Protected access only **/
	GEVASEV_HANDLER_RET gevasev_drag_mouse_in(GtkObject * object,
											  GtkObject * gevasobj, int _b,
											  int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_drag_mouse_out(GtkObject * object,
											   GtkObject * gevasobj, int _b,
											   int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_drag_mouse_down(GtkObject * object,
												GtkObject * gevasobj, int _b,
												int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_drag_mouse_up(GtkObject * object,
											  GtkObject * gevasobj, int _b,
											  int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_drag_mouse_move(GtkObject * object,
												GtkObject * gevasobj, int _b,
												int _x, int _y);


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
