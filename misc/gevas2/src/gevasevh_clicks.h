/*
 * converts evas callbacks into double/triple click gtk+ signals
 *
 * FIXME: only double click is done at the moment.
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


#ifndef INC_GTK_GEVASEVH_CLICKS__H
#define INC_GTK_GEVASEVH_CLICKS__H

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_CLICKS_SIGNAL(obj)          GTK_CHECK_CAST (obj, gevasevh_clicks_get_type (), GtkgEvasEvHClicks)
#define GTK_GEVASEVH_CLICKS_SIGNAL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_clicks_get_type (), GtkgEvasEvHClicksClass)
#define GTK_IS_GEVASEVH_CLICKS_SIGNAL(obj)       GTK_CHECK_TYPE (obj, gevasevh_clicks_get_type ())
#define GTK_GEVASEVH_CLICKS_DCLICK_MILLIS		"GtkgEvasEvHClicks::dclick_millis"
#define gevasevh_clicks_set_dclick_millis( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_CLICKS_DCLICK_MILLIS, (gpointer) val, NULL);
	 typedef struct _GtkgEvasEvHClicks GtkgEvasEvHClicks;
	typedef struct _GtkgEvasEvHClicksClass GtkgEvasEvHClicksClass;



	struct _GtkgEvasEvHClicks {
		GtkgEvasEvH evh_obj;
		gint dclick_millis;

		/* state maintain between clicks */
		gint32 last_time;
		GtkObject *last_obj;
		gint last_button;

	};

	struct _GtkgEvasEvHClicksClass {
		GtkgEvasEvHClass parent_class;

        gboolean (*sig_dclick)(GtkgEvasEvHClicks* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y);
        

    };


	guint gevasevh_clicks_get_type(void);
	GtkObject *gevasevh_clicks_new(void);

#define DCLICK_MILLIS_DEFAULT 	250

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
