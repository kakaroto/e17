/*
 * Gtk abstraction of Evas_Object showing some text.
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


#ifndef INC_GTK_GEVASTEXT_H
#define INC_GTK_GEVASTEXT_H

#include <gtk/gtk.h>
#include <glib.h>
#include "gevasobj.h"
#include "gevas.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASTEXT(obj)          GTK_CHECK_CAST (obj, gevastext_get_type (), GtkgEvasText)
#define GTK_GEVASTEXT_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevastext_get_type (), GtkgEvasTextClass)
#define GTK_IS_GEVASTEXT(obj)       GTK_CHECK_TYPE (obj, gevastext_get_type ())
#define GTK_GEVASTEXT_IMAGENAME		"GtkgEvasText::image_name"
#define gevastext_set_image_name( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASTEXT_IMAGENAME, (gchar*) val, NULL);
	 typedef struct _GtkgEvasText GtkgEvasText;
	typedef struct _GtkgEvasTextClass GtkgEvasTextClass;

	struct _GtkgEvasText {
		GtkgEvasObj gobj;
	};

	struct _GtkgEvasTextClass {
		GtkgEvasObjClass parent_class;
	};


	guint gevastext_get_type(void);
	GtkgEvasText *gevastext_new(GtkgEvas * gevas);
	GtkgEvasText *gevastext_new_full(GtkgEvas * gevas, char *font, int size,
									 char *text);
/** public members **/

	gchar *gevastext_get_string(GtkgEvasObj * object);
	const gchar *gevastext_get_font(GtkgEvasObj * object);
	int gevastext_get_text_size(GtkgEvasObj * object);
	void gevastext_get_ascent_descent(GtkgEvasObj * object, double *ascent,
									  double *descent);
	double gevastext_get_ascent(GtkgEvasObj * object);
	double gevastext_get_descent(GtkgEvasObj * object);
	void gevastext_get_max_ascent_descent(GtkgEvasObj * object, double *ascent,
										  double *descent);
	double gevastext_get_max_ascent(GtkgEvasObj * object);
	double gevastext_get_max_descent(GtkgEvasObj * object);
	void gevastext_get_advance(GtkgEvasObj * object, double *h_advance,
							   double *v_advance);
	double gevastext_get_inset(GtkgEvasObj * object);
	void gevastext_set_string(GtkgEvasObj * object, const gchar * text);
	void gevastext_set_font(GtkgEvasObj * object, const gchar * font,
							const int size);

#define GEVASTEXT_DEFAULT_FONT "helvetica"

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
