/*
 * Makes the attached gevasobj larger when the mouse moves over it
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


#ifndef INC_GTK_GEVASEVH_EMOUSE_OVER__H
#define INC_GTK_GEVASEVH_EMOUSE_OVER__H

#include <gtk/gtkobject.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_EMOUSE_OVER(obj)          GTK_CHECK_CAST (obj, gevasevh_emouse_over_get_type (), GtkgEvasEvHEMouseOver)
#define GTK_GEVASEVH_EMOUSE_OVER_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_emouse_over_get_type (), GtkgEvasEvHEMouseOverClass)
#define GTK_IS_GEVASEVH_EMOUSE_OVER(obj)       GTK_CHECK_TYPE (obj, gevasevh_emouse_over_get_type ())
#define GTK_GEVASEVH_EMOUSE_OVER_SCALE_X	"GtkgEvasEvHEMouseOver::scale_x"
#define GTK_GEVASEVH_EMOUSE_OVER_SCALE_Y	"GtkgEvasEvHEMouseOver::scale_y"
#define gevasevh_emouse_over_scale_x( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_EMOUSE_OVER_SCALE_X, (gdouble) val, NULL);
#define gevasevh_emouse_over_scale_y( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_EMOUSE_OVER_SCALE_Y, (gdouble) val, NULL);
	 typedef struct _GtkgEvasEvHEMouseOver GtkgEvasEvHEMouseOver;
	typedef struct _GtkgEvasEvHEMouseOverClass GtkgEvasEvHEMouseOverClass;

	struct _GtkgEvasEvHEMouseOver {
		GtkgEvasEvH evh_obj;
		gdouble scale_factor_x;
		gdouble scale_factor_y;
		gdouble old_w;			/* can not inverse scale factor due to rounding errors */
		gdouble old_h;			/* thus we save the exact original size and restore it, quicker too */
		gdouble old_x_offset;	/* how much we moved the item in the x plane */
		gdouble old_y_offset;
	};

	struct _GtkgEvasEvHEMouseOverClass {
		GtkgEvasEvHClass parent_class;
	};


	guint gevasevh_emouse_over_get_type(void);
	GtkObject *gevasevh_emouse_over_new(void);

/** Protected access only **/
	GEVASEV_HANDLER_RET gevasev_emouse_over_mouse_in(GtkObject * object,
													 GtkObject * gevasobj,
													 int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_emouse_over_mouse_out(GtkObject * object,
													  GtkObject * gevasobj,
													  int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_emouse_over_mouse_down(GtkObject * object,
													   GtkObject * gevasobj,
													   int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_emouse_over_mouse_up(GtkObject * object,
													 GtkObject * gevasobj,
													 int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_emouse_over_mouse_move(GtkObject * object,
													   GtkObject * gevasobj,
													   int _b, int _x, int _y);


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
