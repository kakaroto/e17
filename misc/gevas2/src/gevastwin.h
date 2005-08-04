/*
 * Gtk abstraction of a object and another in some form of binding.
 *
 * Note that for best results set mainobj first, so that auxobj can be placed into its
 * correct location right away
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


#ifndef INC_GTK_GEVASTWIN_H
#define INC_GTK_GEVASTWIN_H

#include <gtk/gtkobject.h>
#include "gevasobj.h"
#include "gevas.h"
#include <gevas_sprite.h>

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASTWIN(obj)          GTK_CHECK_CAST (obj, gevastwin_get_type (), GtkgEvasTwin)
#define GTK_GEVASTWIN_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevastwin_get_type (), GtkgEvasTwinClass)
#define GTK_IS_GEVASTWIN(obj)       GTK_CHECK_TYPE (obj, gevastwin_get_type ())
#define GTK_GEVASTWIN_MAINOBJ		"GtkgEvasTwin::main_obj"
#define GTK_GEVASTWIN_AUXOBJ		"GtkgEvasTwin::aux_obj"
#define GTK_GEVASTWIN_ALIGNX		"GtkgEvasTwin::align_x"
#define GTK_GEVASTWIN_ALIGNY		"GtkgEvasTwin::align_y"
#define GTK_GEVASTWIN_OFFSETX		"GtkgEvasTwin::offset_x"
#define GTK_GEVASTWIN_OFFSETY		"GtkgEvasTwin::offset_y"
#define gevastwin_set_main_obj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASTWIN_MAINOBJ, (gpointer) val, NULL);
#define gevastwin_set_aux_obj( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASTWIN_AUXOBJ, (gpointer) val, NULL);
	 typedef struct _GtkgEvasTwin GtkgEvasTwin;
	typedef struct _GtkgEvasTwinClass GtkgEvasTwinClass;

	struct _GtkgEvasTwin {
		GtkObject gobj;

		GtkgEvasObj *mainobj;
		GtkgEvasObj *auxobj;
        GtkgEvasSprite* sprite;

		int ox,oy;
		int ax,ay;

		void (*main_obj_move) (GtkgEvasObj * object, double x, double y);
		void (*aux_obj_move) (GtkgEvasObj * object, double x, double y);

        GtkgEvasObjCollection* extra_objects;
    };

	struct _GtkgEvasTwinClass {
		GtkObjectClass parent_class;
	};


	guint gevastwin_get_type(void);
	GtkgEvasTwin *gevastwin_new();
    void gevastwin_sync_obj( GtkgEvasTwin* ev, GtkgEvasObj* object );

    void gevastwin_add_extra_objects( GtkgEvasTwin* ev, GtkgEvasObjCollection* v );
    void gevastwin_add_extra_object( GtkgEvasTwin* ev, GtkgEvasObj* obj );
    

#define GEVASTWIN_BACKWARD_LOOKUP_KEY "___gevastwin_backward_key"


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
