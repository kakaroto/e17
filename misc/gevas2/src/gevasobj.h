/*
 * Gtk abstraction of Evas_Object
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

#ifndef INC_GTK_GEVASOBJ_H
#define INC_GTK_GEVASOBJ_H

#include <gtk/gtkobject.h>
#include "gevas.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASOBJ(obj)          GTK_CHECK_CAST (obj, gevasobj_get_type (), GtkgEvasObj)
#define GTK_GEVASOBJ_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasobj_get_type (), GtkgEvasObjClass)
#define GTK_IS_GEVASOBJ(obj)       GTK_CHECK_TYPE (obj, gevasobj_get_type ())
#define GTK_GEVASOBJ_GEVAS			"GtkgEvasObj::gevas"
#define GTK_GEVASOBJ_IMAGENAME		"GtkgEvasObj::image_name"
#define gevasobj_set_gevas( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASOBJ_GEVAS, (gpointer) val, NULL);
#define gevasobj_set_image_name( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASOBJ_IMAGENAME, (gchar*) val, NULL);
    
    typedef struct _GtkgEvasObj GtkgEvasObj;
	typedef struct _GtkgEvasObjClass GtkgEvasObjClass;

#define GEVASOBJ_SIG_VETO 1    
#define GEVASOBJ_SIG_OK   0    

	struct _GtkgEvasObj {
		GtkObject gobj;
		GtkgEvas *gevas;
		Evas_Object* eobj;

		GSList *ev_handlers;

        /**  These are here so that gevastwin can override them
         *  on an object basis, otherwise gevastwin would have
         *  to check EVERY gevasobj added to the canvas, not 
         *  just the ones that it is interested in. A slight RAM loss.
         **/
		void (*move) (GtkgEvasObj * object, double x, double y);

        /**
         * Ability to cache the original move function
         */
		void (*original_move) (GtkgEvasObj * object, double x, double y);

	};

	struct _GtkgEvasObjClass {
		GtkObjectClass parent_class;

		Evas*(*_gevas_evas) (GtkObject * object);
		void (*_gevas_set_obj) (GtkObject * object, Evas_Object* eobj);
		void (*_gevasobj_ensure_obj_free) (GtkObject * object);

		void (*set_color) (GtkgEvasObj * object, int r, int g, int b, int a);
		void (*set_zoom_scale) (GtkgEvasObj * object, int scale);
		void (*set_layer) (GtkgEvasObj * object, int l);
		int  (*get_layer) (GtkgEvasObj * object);
		void (*set_layer_store) (GtkgEvasObj * object, int l, int store);
		void (*raise) (GtkgEvasObj * object);
		void (*lower) (GtkgEvasObj * object);
		void (*stack_above) (GtkgEvasObj * object, GtkgEvasObj * above);
		void (*stack_below) (GtkgEvasObj * object, GtkgEvasObj * below);
		void (*resize) (GtkgEvasObj * object, double w, double h);
		void (*get_geometry) (GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y,
							  Evas_Coord *w, Evas_Coord *h);
		void (*show) (GtkgEvasObj * object);
		void (*hide) (GtkgEvasObj * object);
		void (*get_color) (GtkgEvasObj * object, int *r, int *g, int *b,
						   int *a);
		void (*set_name) (GtkgEvasObj * object, gchar * name);
		gchar *(*get_name) (GtkgEvasObj * object);
		int (*get_alpha) (GtkgEvasObj * object);
		void (*set_alpha) (GtkgEvasObj * object, int a);
		void (*get_location) (GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y);
		void (*set_location) (GtkgEvasObj * object, Evas_Coord  x, Evas_Coord  y);
        void (*get_size)(GtkgEvasObj * object, Evas_Coord *w, Evas_Coord *h);

		void (*add_evhandler) (GtkgEvasObj * object, GtkObject * h);
		void (*remove_evhandler) (GtkgEvasObj * object, GtkObject * h);

        gint (*sig_move_absolute)(GtkgEvasObj* ev, gpointer d1, gpointer d2);
        gint (*sig_move_relative)(GtkgEvasObj* ev, gpointer d1, gpointer d2);
        gint (*sig_resize)       (GtkgEvasObj* ev, gpointer d1, gpointer d2);
        gint (*sig_visible)      (GtkgEvasObj* ev, gpointer d1);
        gint (*sig_layer)        (GtkgEvasObj* ev, gpointer d1);

	};


	guint gevasobj_get_type(void);
	GtkObject *gevasobj_new(void);
	void gevasobj_add_evhandler(GtkgEvasObj * object, GtkObject * h);
	void gevasobj_remove_evhandler(GtkgEvasObj * object, GtkObject * h);

	void gevasobj_queue_redraw(GtkgEvasObj * obj);

	void gevasobj_move(          GtkgEvasObj * object, double x, double y);
	void gevasobj_move_relative( GtkgEvasObj * object, double dx, double dy);

	Evas_Object* gevasobj_get_evasobj(GtkObject * object);

	GtkgEvas *gevasobj_get_gevas(GtkObject* object);

	Evas* gevasobj_get_evas(GtkObject * object);


	void gevasobj_add_evhandler(GtkgEvasObj * object, GtkObject * h);
	void gevasobj_remove_evhandler(GtkgEvasObj * object, GtkObject * h);

/** evas wrappers (public access) **/

	void  gevasobj_set_data( GtkgEvasObj * object, const char* key, void* data ); 
	void* gevasobj_get_data( GtkgEvasObj * object, const char* key ); 

	void gevasobj_set_color(GtkgEvasObj * object, int r, int g, int b, int a);
	void gevasobj_set_color_from_string(GtkgEvasObj * object, const gchar* s );
	void gevasobj_set_alpha(GtkgEvasObj * object, int a);
	void gevasobj_set_zoom_scale(GtkgEvasObj * object, int scale);
	void gevasobj_set_layer(GtkgEvasObj * object, int l);
	int  gevasobj_get_layer(GtkgEvasObj * object);
	void gevasobj_set_layer_store(GtkgEvasObj * object, int l, int store);
	void gevasobj_raise(GtkgEvasObj * object);
	void gevasobj_lower(GtkgEvasObj * object);
	void gevasobj_stack_above(GtkgEvasObj * object, GtkgEvasObj * above);
	void gevasobj_stack_below(GtkgEvasObj * object, GtkgEvasObj * below);
	void gevasobj_move(GtkgEvasObj * object, double x, double y);
	void gevasobj_resize(GtkgEvasObj * object, double w, double h);
	void gevasobj_get_geometry(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y,
							   Evas_Coord *w, Evas_Coord *h);
	void gevasobj_get_location(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y);
	void gevasobj_set_location(GtkgEvasObj * object, Evas_Coord  x, Evas_Coord  y);
    void gevasobj_get_size(    GtkgEvasObj * object, Evas_Coord *w, Evas_Coord *h);
    void gevasobj_show(GtkgEvasObj * object);
	void gevasobj_hide(GtkgEvasObj * object);
	void gevasobj_set_visible(GtkgEvasObj * object, gboolean v);
	gboolean gevasobj_get_visible( GtkgEvasObj * object );
	void gevasobj_get_color(GtkgEvasObj * object, int *r, int *g, int *b,
							int *a);
	int gevasobj_get_alpha(GtkgEvasObj * object);
	void gevasobj_set_name(GtkgEvasObj * object, gchar * name);
	gchar *gevasobj_get_name(GtkgEvasObj * object);


/** protected access only **/
	Evas* _gevas_evas(GtkObject * object);
	void _gevas_set_obj(GtkObject * object, Evas_Object* eobj);
	Evas_Object* _gevas_get_obj(GtkObject * object);
	void _gevasobj_ensure_obj_free(GtkObject * object);
	GSList *gevasobj_get_evhandlers(GtkgEvasObj * object );
    GtkgEvasObj* gevasobj_from_evasobj( Evas_Object* eo );
    



/* the GtkgEvasObj is stored at this key in the Evas_Object */
#define PRIVATE_GTK_GEVASOBJ "___gtk_gevasobj_key"

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
