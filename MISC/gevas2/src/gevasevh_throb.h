/*
 * Gives the enstrom effect to the attached gevasobj when the mouse moves over it
 *
 *
 * Copyright (C) 2002 Ben Martin.
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


#ifndef INC_GTK_GEVASEVH_THROB__H
#define INC_GTK_GEVASEVH_THROB__H

#include <gtk/gtkobject.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"
#include "gevastwin.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_THROB(obj)          GTK_CHECK_CAST (obj, gevasevh_throb_get_type (), GtkgEvasEvHThrob)
#define GTK_GEVASEVH_THROB_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_throb_get_type (), GtkgEvasEvHThrobClass)
#define GTK_IS_GEVASEVH_THROB(obj)       GTK_CHECK_TYPE (obj, gevasevh_throb_get_type ())

    typedef struct _GtkgEvasEvHThrob GtkgEvasEvHThrob;
	typedef struct _GtkgEvasEvHThrobClass GtkgEvasEvHThrobClass;

	struct _GtkgEvasEvHThrob {
		GtkgEvasEvH evh_obj;

        guint    m_throbTimer;
        int      m_val;
        guint    m_interval;
        GTimeVal m_timeTracker;

        double t;
        Evas_Coord x, y, w, h;
        Evas_Coord ix, iy;
        int r,g,b,a;

        /* For operation on a twin */
		void (*obj_move) (GtkgEvasObj * object, double x, double y);
    };

	struct _GtkgEvasEvHThrobClass {
		GtkgEvasEvHClass parent_class;
	};


	guint gevasevh_throb_get_type(void);
	GtkgEvasEvHThrob* gevasevh_throb_new( GtkgEvasObj* go );
	GtkgEvasEvHThrob* gevasevh_throb_new_for_twin( GtkgEvasTwin* twin, GtkgEvasObj* go );

/** Protected access only **/
	GEVASEV_HANDLER_RET gevasev_throb_mouse_in(GtkObject * object,
                                               GtkObject * gevasobj,
                                               int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_throb_mouse_out(GtkObject * object,
                                                GtkObject * gevasobj,
                                                int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_throb_mouse_down(GtkObject * object,
                                                 GtkObject * gevasobj,
                                                 int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_throb_mouse_up(GtkObject * object,
                                               GtkObject * gevasobj,
                                               int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_throb_mouse_move(GtkObject * object,
                                                 GtkObject * gevasobj,
                                                 int _b, int _x, int _y);


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
