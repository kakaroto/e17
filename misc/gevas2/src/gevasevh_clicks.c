/*
 * converts evas callbacks into double/triple click gtk+ signals
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


#include "gevasevh_clicks.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>
#include <gdk/gdktypes.h>
#include "project.h"

static void gevasevh_clicks_class_init(GtkgEvasEvHClicksClass * klass);
static void gevasevh_clicks_init(GtkgEvasEvHClicks * ev);
/* GtkObject functions */
static void gevasevh_clicks_destroy(GtkObject * object);
static void
gevasevh_clicks_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_clicks_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	SIG_DCLICK,
	LAST_SIGNAL
};
static guint _gevasevh_clicks_signals[LAST_SIGNAL] = { 0 };

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_DCLICK_MILLIS
};


GEVASEV_HANDLER_RET
gevasev_clicks_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
						int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_clicks_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
						 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_clicks_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
						  int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_clicks_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
						int _x, int _y)
{
	GdkEvent *gev;
	gboolean return_val = FALSE;
	GtkgEvasEvHClicks *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_CLICKS_SIGNAL(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_CLICKS_SIGNAL(object);

	gev = gevas_get_current_event(gevasobj_get_gevas(gevasobj));
	if (gev && gev->type == GDK_BUTTON_RELEASE) {
		if (gevasobj == ev->last_obj && (_b == ev->last_button)
			&& (gev->button.time - ev->last_time < ev->dclick_millis)
			) {
			gtk_signal_emit(GTK_OBJECT(object),
							_gevasevh_clicks_signals[SIG_DCLICK],
							(gpointer) gevasobj,
							(gint) _b, (gint) _x, (gint) _y, &return_val);
		}
		ev->last_time = gev->button.time;
		ev->last_obj = gevasobj;
		ev->last_button = _b;
	}

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_clicks_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
						  int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_clicks_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHClicks",
			sizeof(GtkgEvasEvHClicks),
			sizeof(GtkgEvasEvHClicksClass),
			(GtkClassInitFunc) gevasevh_clicks_class_init,
			(GtkObjectInitFunc) gevasevh_clicks_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static gboolean sig_dclick(GtkgEvasEvHClicks* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
//    printf("sig_dclick\n");
    return 0;
}



static void gevasevh_clicks_class_init(GtkgEvasEvHClicksClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_clicks_destroy;
	object_class->get_arg = gevasevh_clicks_get_arg;
	object_class->set_arg = gevasevh_clicks_set_arg;

	evh_klass->handler_mouse_in = gevasev_clicks_mouse_in;
	evh_klass->handler_mouse_out = gevasev_clicks_mouse_out;
	evh_klass->handler_mouse_down = gevasev_clicks_mouse_down;
	evh_klass->handler_mouse_up = gevasev_clicks_mouse_up;
	evh_klass->handler_mouse_move = gevasev_clicks_mouse_move;

    klass->sig_dclick = sig_dclick;

    _gevasevh_clicks_signals[SIG_DCLICK] =
		gtk_signal_new("dclick",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHClicksClass, sig_dclick),
					   gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);

/* 	gtk_object_class_add_signals(object_class, _gevasevh_clicks_signals, */
/* 								 LAST_SIGNAL); */

	gtk_object_add_arg_type(GTK_GEVASEVH_CLICKS_DCLICK_MILLIS,
							GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_DCLICK_MILLIS);


}

static void gevasevh_clicks_init(GtkgEvasEvHClicks * ev)
{
	ev->dclick_millis = DCLICK_MILLIS_DEFAULT;
}

GtkObject *gevasevh_clicks_new(void)
{
	GtkgEvasEvHClicks *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_clicks_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->dclick_millis = DCLICK_MILLIS_DEFAULT;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_clicks_destroy(GtkObject * object)
{
	GtkgEvasEvHClicks *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_CLICKS_SIGNAL(object));
	ev = GTK_GEVASEVH_CLICKS_SIGNAL(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_clicks_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHClicks *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_CLICKS_SIGNAL(object));
	ev = GTK_GEVASEVH_CLICKS_SIGNAL(object);


	switch (arg_id) {
		case ARG_DCLICK_MILLIS:
			ev->dclick_millis = GTK_VALUE_INT(*arg);
			break;

		default:
			break;
	}
}

static void
gevasevh_clicks_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHClicks *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_CLICKS_SIGNAL(object));
	ev = GTK_GEVASEVH_CLICKS_SIGNAL(object);

	switch (arg_id) {
		case ARG_DCLICK_MILLIS:
			GTK_VALUE_INT(*arg) = ev->dclick_millis;
			break;

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
