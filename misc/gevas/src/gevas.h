/*
 * A Gtk+ wrapper for an Evas.
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


#ifndef INC_GTK_GEVAS_H
#define INC_GTK_GEVAS_H

#include <gtk/gtkwidget.h>
#include <Evas.h>

#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVAS(obj)          GTK_CHECK_CAST (obj, gevas_get_type (), GtkgEvas)
#define GTK_GEVAS_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevas_get_type (), GtkgEvasClass)
#define GTK_IS_GEVAS(obj)       GTK_CHECK_TYPE (obj, gevas_get_type ())
#define GTK_GEVAS_CHECKED_BG 		"GtkgEvas::checked_bg"
#define GTK_GEVAS_RENDER_MODE 		"GtkgEvas::render_mode"
#define GTK_GEVAS_EVAS		 		"GtkgEvas::evas"
#define GTK_GEVAS_FONT_CACHE 		"GtkgEvas::font_cache"
#define GTK_GEVAS_IMAGE_CACHE 		"GtkgEvas::image_cache"
#define GTK_GEVAS_SIZE_REQUEST_X 	"GtkgEvas::size_request_x"
#define GTK_GEVAS_SIZE_REQUEST_Y 	"GtkgEvas::size_request_y"
#define GTK_GEVAS_DRAWABLE_SIZE_W 	"GtkgEvas::drawable_size_w"
#define GTK_GEVAS_DRAWABLE_SIZE_H 	"GtkgEvas::drawable_size_h"
#define GTK_GEVAS_SMOOTHNESS		"GtkgEvas::smoothness"
#define GTK_GEVAS_VIEWPORT_X		"GtkgEvas::viewport_x"
#define GTK_GEVAS_VIEWPORT_Y		"GtkgEvas::viewport_y"
#define GTK_GEVAS_VIEWPORT_W		"GtkgEvas::viewport_w"
#define GTK_GEVAS_VIEWPORT_H		"GtkgEvas::viewport_h"
#define gevas_set_checked_bg( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_CHECKED_BG, (gboolean) val, NULL);
#define gevas_set_render_mode( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_RENDER_MODE, val, NULL);
#define gevas_set_font_cache( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_FONT_CACHE, (gint)val, NULL);
#define gevas_set_image_cache( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_IMAGE_CACHE, (gint)val, NULL);
#define gevas_set_size_request_x( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_SIZE_REQUEST_X, (gint)val, NULL);
#define gevas_set_size_request_y( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_SIZE_REQUEST_Y, (gint)val, NULL);
#define gevas_set_smoothness( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_SMOOTHNESS, (gint)val, NULL);
#define gevas_set_viewport_x( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_VIEWPORT_X, (gdouble)val, NULL);
#define gevas_set_viewport_y( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_VIEWPORT_Y, (gdouble)val, NULL);
#define gevas_set_viewport_w( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_VIEWPORT_W, (gdouble)val, NULL);
#define gevas_set_viewport_h( gevas, val ) \
	  gtk_object_set(GTK_OBJECT(gevas), \
                  GTK_GEVAS_VIEWPORT_H, (gdouble)val, NULL);
#include <gtk/gtkscrolledwindow.h>
	 typedef struct _GtkgEvas GtkgEvas;
	typedef struct _GtkgEvasClass GtkgEvasClass;

struct _GtkgEvas {
	GtkWidget widget;

	Evas evas;				// the evas we are using to render stuff with.

	gboolean show_checked_bg;	// Are we showing a checked bg? (mainly for testing)
	Evas_Object checked_bg;	// Object for the bg.


	gint size_request_x;	// Size to request
	gint size_request_y;	//

	Evas_Render_Method render_method;
	guint current_idle;		// handles queue render callback

	GdkEvent *current_event;	// so that evas callbacks can get the gdk event.

	/** Handles the middle button gimp style scroll option */
	/*
	 * Note that as the default middleb_scrolls_pgate_event==0 so middle clicks 
	 * that cause the window to scroll will not be propergated to evas.
	 * If, for example, you wish to attach something to a middle double click,
	 * you will want to set middleb_scrolls_pgate_event==1 so that middle button
	 * clicks are propergated through evas.
	 */
	gboolean middleb_scrolls;			// Is middle buttons scroll feature enabled 
	gboolean middleb_scrolls_pgate_event;// should the gdk event to propergated still?
	GtkAdjustment* middleb_scrolls_xplane;// The adjustment to update for xplane scroll
	GtkAdjustment* middleb_scrolls_yplane;// The adjustment to update for yplane scroll
	gboolean scrolling;					// Is the middle button down now?
	gint scrolling_x;					// keep the last x ord so dx can be made
	gint scrolling_y;					// keep the last y ord so dy can be made
	
	/** Keeping track of all the GtkgEvasObj* items added to us **/
		GHashTable *gevasobjs;	// (lookup Evas_Object -> GtkgEvasObj*)
		GSList *gevasobjlist;	// simple collection of each GtkgEvas in a list.


	};

struct _GtkgEvasClass {

  GtkWidgetClass parent_class;

  /** protected **/
  void (*_register_gevasobj)( GtkgEvas* thisp, GtkObject* gobj );

  /** public **/
  GtkObject* (*get_object_under_mouse)( GtkgEvas* ev );
  GtkObject* (*object_in_rect)(GtkgEvas* ev, double x, double y, double w, double h);
  GtkObject* (*object_at_position)(GtkgEvas* ev, double x, double y);
  GtkObject* (*object_get_named)(GtkgEvas* ev, char *name);

};

/** protected **/

//	void gevas_

	guint gevas_get_type(void);
	GtkWidget *gevas_new(void);

	void gevas_queue_redraw(GtkgEvas * gevas);

	GdkEvent *gevas_get_current_event(GtkgEvas * gevas);
	void gevas_set_middleb_scrolls(GtkgEvas * gevas, gboolean v,
		   GtkAdjustment * ah, GtkAdjustment * av);
	void gevas_set_middleb_scrolls_pgate_event( GtkgEvas* gevas, gboolean v ); 

	GtkObject *gevas_get_object_under_mouse(GtkgEvas * ev);
	GtkObject *gevas_object_in_rect(GtkgEvas * ev, double x, double y, double w,
									double h);
	GtkObject *gevas_object_at_position(GtkgEvas * ev, double x, double y);
	GtkObject *gevas_object_get_named(GtkgEvas * ev, char *name);
	void gevas_add_fontpath(GtkgEvas * ev, gchar * path);
	void gevas_remove_fontpath(GtkgEvas * ev, gchar * path);

	Evas gevas_get_evas(GtkgEvas * gevas);
	GdkEvent *gevas_get_current_event(GtkgEvas * gevas);


#define GEVAS_CHECKED_BG_IMAGE_FILENAME    PACKAGE_DATA_DIR"/checks.png"
#define GEVAS_MAX_MIDDLEB_SCROLL_PIXELS 150


/** protected access only **/
	void __gevas_mouse_in(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
						  int _y);
	void __gevas_mouse_out(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
						   int _y);
	void __gevas_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b,
							int _x, int _y);
	void __gevas_mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
						  int _y);
	void __gevas_mouse_move(void *_data, Evas _e, Evas_Object _o, int _b,
							int _x, int _y);
	void gevas_adjust_scrollarea(GtkgEvas * ev, GtkAdjustment * plane,
								 gint * delta);
	void _register_gevasobj(GtkgEvas * thisp, GtkObject * gobj);

void gevas_get_drawable_size( GtkgEvas *object, int* w, int *h );


	void gevas_gtk_marshal_BOOL__POINTER_POINTER_INT_INT_INT(GtkObject * object,
															 GtkSignalFunc func,
															 gpointer func_data,
															 GtkArg * args);

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVAS_H__ */
