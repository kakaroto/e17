/*
 * Gtk abstraction of Evas_Object showing a gradient
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


#ifndef INC_GTK_GEVASGRAD_H
#define INC_GTK_GEVASGRAD_H

#include <gtk/gtk.h>
#include <glib.h>
#include "gevasobj.h"
#include "gevas.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASGRAD(obj)          GTK_CHECK_CAST (obj, gevasgrad_get_type (), GtkgEvasGrad)
#define GTK_GEVASGRAD_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasgrad_get_type (), GtkgEvasGradClass)
#define GTK_IS_GEVASGRAD(obj)       GTK_CHECK_TYPE (obj, gevasgrad_get_type ())
#define GTK_GEVASGRAD_IMAGENAME		"GtkgEvasGrad::image_name"
#define gevasgrad_set_image_name( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASGRAD_IMAGENAME, (gchar*) val, NULL);
	 typedef struct GtkgEvasGrad GtkgEvasGrad;
	typedef struct GtkgEvasGradClass GtkgEvasGradClass;

	struct GtkgEvasGrad {
		GtkgEvasObj gobj;
    };

	struct GtkgEvasGradClass {
		GtkgEvasObjClass parent_class;
	};


	guint gevasgrad_get_type(void);
	GtkgEvasGrad *gevasgrad_new(GtkgEvas * gevas);

/** public members **/
	void gevasgrad_clear_gradient(GtkgEvasObj * object);
	void gevasgrad_add_color(GtkgEvasObj * object, int r, int g, int b, int a,
							 int dist);
/* 	void gevasgrad_seal(GtkgEvasObj * object); */
	void gevasgrad_set_angle( GtkgEvasObj* object, double angle);



#define GEVASTEXT_DEFAULT_FONT "helvetica"

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
