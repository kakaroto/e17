/*
 * Gtk abstraction of Evas_Object showing an edje.
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


#ifndef INC_GTK_GEVASEDJE_H
#define INC_GTK_GEVASEDJE_H

#include <gtk/gtkobject.h>
#include <gevasobj.h>
#include <gevas.h>
#include <Edje.h>

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEDJE(obj)          GTK_CHECK_CAST (obj, gevasedje_get_type (), GtkgEvasEdje)
#define GTK_GEVASEDJE_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasedje_get_type (), GtkgEvasEdjeClass)
#define GTK_IS_GEVASEDJE(obj)       GTK_CHECK_TYPE (obj, gevasedje_get_type ())
#define GTK_GEVASEDJE_IMAGENAME		"GtkgEvasEdje::image_name"
#define gevasedje_set_image_name( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEDJE_IMAGENAME, (gchar*) val, NULL);
#define gevasedje_get_image_name( e ) \
    (GTK_GEVASEDJE(e)->image_filename \
     ? GTK_GEVASEDJE(e)->image_filename \
     : "")


typedef struct _GtkgEvasEdje GtkgEvasEdje;
typedef struct _GtkgEvasEdjeClass GtkgEvasEdjeClass;


    struct _GtkgEvasEdje {
        GtkgEvasObj gobj;

        Evas_Object* m_edje;
    };

	struct _GtkgEvasEdjeClass {
		GtkgEvasObjClass parent_class;

        Evas(*_gevas_evas) (GtkObject * object);

    };


	guint gevasedje_get_type(void);
	GtkgEvasEdje *gevasedje_new(void);
	GtkgEvasEdje *gevasedje_new_with_canvas( gpointer gevas );
    void gevasedje_set_file( GtkgEvasEdje* gedje, const char* filename, const char* part );
    
    void gevasedje_set_edje( GtkgEvasEdje* ev, Evas_Object* e );
    Evas_Object* gevasedje_get_edje( GtkgEvasEdje* ev );

    



#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
