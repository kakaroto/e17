/*
 * changes the alpha value of the gevasobj on mouse in and back on mouse out
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


#ifndef INC_GTK_GEVASEVH_ALPHA__H
#define INC_GTK_GEVASEVH_ALPHA__H

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_ALPHA(obj)          GTK_CHECK_CAST (obj, gevasevh_alpha_get_type (), GtkgEvasEvHAlpha)
#define GTK_GEVASEVH_ALPHA_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_alpha_get_type (), GtkgEvasEvHAlphaClass)
#define GTK_IS_GEVASEVH_ALPHA(obj)       GTK_CHECK_TYPE (obj, gevasevh_alpha_get_type ())
#define GTK_GEVASEVH_ALPHA_HOT_VALUE		"GtkgEvasEvHAlpha::hot_value"
#define gevasevh_alpha_set_how_value( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_ALPHA_HOT_VALUE, (gint) val, NULL);
	 typedef struct _GtkgEvasEvHAlpha GtkgEvasEvHAlpha;
	typedef struct _GtkgEvasEvHAlphaClass GtkgEvasEvHAlphaClass;



	struct _GtkgEvasEvHAlpha {
		GtkgEvasEvH evh_obj;

		gint hot_value;			/* what alpha value to give the object on mouse over.*/
		gint cold_value;		/* cache for the old value for mouse_out*/
	};

	struct _GtkgEvasEvHAlphaClass {
		GtkgEvasEvHClass parent_class;
	};


	guint gevasevh_alpha_get_type(void);
	GtkObject *gevasevh_alpha_new(void);

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
