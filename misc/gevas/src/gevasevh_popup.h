/*
 * converts evas callbacks into popup menu gtk+ signals..
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


#ifndef INC_GTK_GEVASEVH_POPUP__H
#define INC_GTK_GEVASEVH_POPUP__H

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>
#include "gevasev_handler.h"
#include "gevas.h"
#include "gevasobj.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVASEVH_POPUP_SIGNAL(obj)          GTK_CHECK_CAST (obj, gevasevh_popup_get_type (), GtkgEvasEvHPopup)
#define GTK_GEVASEVH_POPUP_SIGNAL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gevasevh_popup_get_type (), GtkgEvasEvHPopupClass)
#define GTK_IS_GEVASEVH_POPUP_SIGNAL(obj)       GTK_CHECK_TYPE (obj, gevasevh_popup_get_type ())
#define GTK_GEVASEVH_POPUP_GTK_MENU		"GtkgEvasEvHPopup::gtk_menu"
#define gevasevh_popup_set_gtk_menu( gevaso, val ) \
	  gtk_object_set(GTK_OBJECT(gevaso), \
                  GTK_GEVASEVH_POPUP_GTK_MENU, (gpointer) val, NULL);
	 typedef struct _GtkgEvasEvHPopup GtkgEvasEvHPopup;
	typedef struct _GtkgEvasEvHPopupClass GtkgEvasEvHPopupClass;



	struct _GtkgEvasEvHPopup {
		GtkgEvasEvH evh_obj;
		GtkMenu *user_gtk_menu;	/* User can register a static menu for popping up */
		/* or listen for popup_activate signals and make menu themself */


    };

	struct _GtkgEvasEvHPopupClass {
		GtkgEvasEvHClass parent_class;

        gboolean (*sig_popup_activate)(
            GtkgEvasEvHPopup* ev,GtkObject* gevasobj, gint _b, gint _x, gint _y);
        
	};


	guint gevasevh_popup_get_type(void);
	GtkObject *gevasevh_popup_new(void);

#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
