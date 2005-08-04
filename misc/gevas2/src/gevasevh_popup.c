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

#include "config.h"
/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

/* Always disable NLS, since we have no config.h; 
 * a real app would not do this of course.
 */
#undef ENABLE_NLS

#ifdef ENABLE_NLS
#include<libintl.h>
#define _(String) dgettext("gtk+",String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else							/* NLS is disabled */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,String) (String)
#define dcgettext(Domain,String,Type) (String)
#define bindtextdomain(Domain,Directory) (Domain)
#endif							/* ENABLE_NLS */


#include "gevasevh_popup.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>
#include "project.h"

static void gevasevh_popup_class_init(GtkgEvasEvHPopupClass * klass);
static void gevasevh_popup_init(GtkgEvasEvHPopup * ev);
/* GtkObject functions */
static void gevasevh_popup_destroy(GtkObject * object);
static void
gevasevh_popup_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_popup_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	M_DOWN,
	LAST_SIGNAL
};
static guint _gevasevh_popup_signals[LAST_SIGNAL] = { 0 };

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_GTK_MENU
};


GEVASEV_HANDLER_RET
gevasev_to_popup_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
						  int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_popup_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
						   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_popup_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							int _x, int _y)
{
	gboolean return_val;
	GtkgEvasEvHPopup *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_POPUP_SIGNAL(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_POPUP_SIGNAL(object);

	return_val = FALSE;

	if (_b == 3) {
        time_t tt = time( 0 );

		gtk_signal_emit(GTK_OBJECT(object),
						_gevasevh_popup_signals[M_DOWN],
						(gpointer) gevasobj,
						(gint) _b, (gint) _x, (gint) _y, &return_val);

		/**  To keep evas happy about the mouse state **/
        evas_event_feed_mouse_move( gevasobj_get_evas(gevasobj), _x, _y, tt, 0 );
        evas_event_feed_mouse_up( gevasobj_get_evas(gevasobj), _b, EVAS_BUTTON_NONE, tt, 0 );

        if (ev->user_gtk_menu) {
			GdkEvent *gev =
				gevas_get_current_event(gevasobj_get_gevas(gevasobj));

			gtk_widget_show_all(GTK_WIDGET(ev->user_gtk_menu));

			gtk_menu_popup(ev->user_gtk_menu, NULL, NULL, NULL, NULL,
						   _b, gev->button.time);
		}
	}


	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_popup_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
						  int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_popup_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_popup_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHPopup",
			sizeof(GtkgEvasEvHPopup),
			sizeof(GtkgEvasEvHPopupClass),
			(GtkClassInitFunc) gevasevh_popup_class_init,
			(GtkObjectInitFunc) gevasevh_popup_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static gboolean sig_popup_activate(GtkgEvasEvHPopup* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
//    printf("sig_popup_activate\n");
    return 0;
}

static void gevasevh_popup_class_init(GtkgEvasEvHPopupClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_popup_destroy;
	object_class->get_arg = gevasevh_popup_get_arg;
	object_class->set_arg = gevasevh_popup_set_arg;

	evh_klass->handler_mouse_in = gevasev_to_popup_mouse_in;
	evh_klass->handler_mouse_out = gevasev_to_popup_mouse_out;
	evh_klass->handler_mouse_down = gevasev_to_popup_mouse_down;
	evh_klass->handler_mouse_up = gevasev_to_popup_mouse_up;
	evh_klass->handler_mouse_move = gevasev_to_popup_mouse_move;

    klass->sig_popup_activate = sig_popup_activate;

    _gevasevh_popup_signals[M_DOWN] =
		gtk_signal_new("popup_activate",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHPopupClass, sig_popup_activate),
					   gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);

/* 	gtk_object_class_add_signals(object_class, _gevasevh_popup_signals, */
/* 								 LAST_SIGNAL); */

	gtk_object_add_arg_type(GTK_GEVASEVH_POPUP_GTK_MENU,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_GTK_MENU);


}

static void gevasevh_popup_init(GtkgEvasEvHPopup * ev)
{
	ev->user_gtk_menu = 0;
}

GtkObject *gevasevh_popup_new(void)
{
	GtkgEvasEvHPopup *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_popup_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->user_gtk_menu = 0;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_popup_destroy(GtkObject * object)
{
	GtkgEvasEvHPopup *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_POPUP_SIGNAL(object));
	ev = GTK_GEVASEVH_POPUP_SIGNAL(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_popup_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHPopup *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_POPUP_SIGNAL(object));
	ev = GTK_GEVASEVH_POPUP_SIGNAL(object);


	switch (arg_id) {
		case ARG_GTK_MENU:
			ev->user_gtk_menu = GTK_VALUE_POINTER(*arg);
			break;

		default:
			break;
	}
}

static void
gevasevh_popup_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHPopup *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_POPUP_SIGNAL(object));
	ev = GTK_GEVASEVH_POPUP_SIGNAL(object);

	switch (arg_id) {
		case ARG_GTK_MENU:
			GTK_VALUE_POINTER(*arg) = ev->user_gtk_menu;
			break;

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
