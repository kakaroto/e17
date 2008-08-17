/*
 * converts evas callbacks into gtk+ signals.
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


#ifndef INC_GTK_GEVASEVH_TO_GTK_SIGNALS__H
#define INC_GTK_GEVASEVH_TO_GTK_SIGNALS__H

#include <gtk/gtkobject.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_TO_GTK_SIGNAL(obj)          GTK_CHECK_CAST (obj, gevasevh_to_gtk_signal_get_type (), GtkgEvasEvHToGtkSignals)
#define GTK_GEVASEVH_TO_GTK_SIGNAL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_to_gtk_signal_get_type (), GtkgEvasEvHToGtkSignalsClass)
#define GTK_IS_GEVASEVH_TO_GTK_SIGNAL(obj)       GTK_CHECK_TYPE (obj, gevasevh_to_gtk_signal_get_type ())
	 typedef struct _GtkgEvasEvHToGtkSignals GtkgEvasEvHToGtkSignals;
	typedef struct _GtkgEvasEvHToGtkSignalsClass GtkgEvasEvHToGtkSignalsClass;



	struct _GtkgEvasEvHToGtkSignals {
		GtkgEvasEvH evh_obj;
	};

	struct _GtkgEvasEvHToGtkSignalsClass {
		GtkgEvasEvHClass parent_class;


        gboolean (*sig_mouse_down)(GtkgEvasEvHToGtkSignals* ev,
                                   GtkObject* gevasobj, gint _b, gint _x, gint _y);
        gboolean (*sig_mouse_up)(GtkgEvasEvHToGtkSignals* ev,
                                 GtkObject* gevasobj, gint _b, gint _x, gint _y);
        gboolean (*sig_mouse_move)(GtkgEvasEvHToGtkSignals* ev,
                                   GtkObject* gevasobj, gint _b, gint _x, gint _y);
        gboolean (*sig_mouse_in)(GtkgEvasEvHToGtkSignals* ev,
                                 GtkObject* gevasobj, gint _b, gint _x, gint _y);
        gboolean (*sig_mouse_out)(GtkgEvasEvHToGtkSignals* ev,
                                  GtkObject* gevasobj, gint _b, gint _x, gint _y);
        

    };


	guint gevasevh_to_gtk_signal_get_type(void);
	GtkObject *gevasevh_to_gtk_signal_new(void);

/** Protected access only **/
	GEVASEV_HANDLER_RET gevasev_to_gtk_signal_mouse_in(GtkObject * object,
													   GtkObject * gevasobj,
													   int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_to_gtk_signal_mouse_out(GtkObject * object,
														GtkObject * gevasobj,
														int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_to_gtk_signal_mouse_down(GtkObject * object,
														 GtkObject * gevasobj,
														 int _b, int _x,
														 int _y);
	GEVASEV_HANDLER_RET gevasev_to_gtk_signal_mouse_up(GtkObject * object,
													   GtkObject * gevasobj,
													   int _b, int _x, int _y);
	GEVASEV_HANDLER_RET gevasev_to_gtk_signal_mouse_move(GtkObject * object,
														 GtkObject * gevasobj,
														 int _b, int _x,
														 int _y);

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
