/*
 * converts evas callbacks into gtk+ signals.
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

/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

#include "config.h"
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


#include "gevasevh_to_gtk_signals.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>
#include "project.h"

static void gevasevh_to_gtk_signal_class_init(GtkgEvasEvHToGtkSignalsClass *
											  klass);
static void gevasevh_to_gtk_signal_init(GtkgEvasEvHToGtkSignals * ev);
/* GtkObject functions */
static void gevasevh_to_gtk_signal_destroy(GtkObject * object);

enum {
	M_IN,
	M_OUT,
	M_MOVE,
	M_DOWN,
	M_UP,
	LAST_SIGNAL
};
static guint _gevasevh_to_gtk_signal_signals[LAST_SIGNAL] = { 0 };


GEVASEV_HANDLER_RET
gevasev_to_gtk_signal_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	gboolean return_val;
	return_val = FALSE;
	gtk_signal_emit(GTK_OBJECT(object),
					_gevasevh_to_gtk_signal_signals[M_IN],
					gevasobj, _b, _x, _y, &return_val);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_gtk_signal_mouse_out(GtkObject * object, GtkObject * gevasobj,
								int _b, int _x, int _y)
{
	gboolean return_val;
	return_val = FALSE;
	gtk_signal_emit(GTK_OBJECT(object),
					_gevasevh_to_gtk_signal_signals[M_OUT],
					gevasobj, _b, _x, _y, &return_val);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_gtk_signal_mouse_down(GtkObject * object, GtkObject * gevasobj,
								 int _b, int _x, int _y)
{
    printf("gevasev_to_gtk_signal_mouse_down()\n");
    
	gboolean return_val;
	return_val = FALSE;
	gtk_signal_emit(GTK_OBJECT(object),
					_gevasevh_to_gtk_signal_signals[M_DOWN],
					(gpointer) gevasobj,
					(gint) _b, (gint) _x, (gint) _y, &return_val);

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_gtk_signal_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	gboolean return_val;
	return_val = FALSE;
	gtk_signal_emit(GTK_OBJECT(object),
					_gevasevh_to_gtk_signal_signals[M_UP],
					gevasobj, _b, _x, _y, &return_val);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_to_gtk_signal_mouse_move(GtkObject * object, GtkObject * gevasobj,
								 int _b, int _x, int _y)
{
	gboolean return_val;
	return_val = FALSE;
	gtk_signal_emit(GTK_OBJECT(object),
					_gevasevh_to_gtk_signal_signals[M_MOVE],
					gevasobj, _b, _x, _y, &return_val);
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_to_gtk_signal_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHToGtkSignals",
			sizeof(GtkgEvasEvHToGtkSignals),
			sizeof(GtkgEvasEvHToGtkSignalsClass),
			(GtkClassInitFunc) gevasevh_to_gtk_signal_class_init,
			(GtkObjectInitFunc) gevasevh_to_gtk_signal_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}


static gboolean sig_mouse_down(GtkgEvasEvHToGtkSignals* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
    return 0;
}

static gboolean sig_mouse_up(GtkgEvasEvHToGtkSignals* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
    return 0;
}

static gboolean sig_mouse_move(GtkgEvasEvHToGtkSignals* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
    return 0;
}

static gboolean sig_mouse_in(GtkgEvasEvHToGtkSignals* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
    return 0;
}

static gboolean sig_mouse_out(GtkgEvasEvHToGtkSignals* ev,
                               GtkObject* gevasobj, gint _b, gint _x, gint _y)
{
    return 0;
}



static void
gevasevh_to_gtk_signal_class_init(GtkgEvasEvHToGtkSignalsClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_to_gtk_signal_destroy;

	evh_klass->handler_mouse_in = gevasev_to_gtk_signal_mouse_in;
	evh_klass->handler_mouse_out = gevasev_to_gtk_signal_mouse_out;
	evh_klass->handler_mouse_down = gevasev_to_gtk_signal_mouse_down;
	evh_klass->handler_mouse_up = gevasev_to_gtk_signal_mouse_up;
	evh_klass->handler_mouse_move = gevasev_to_gtk_signal_mouse_move;

    klass->sig_mouse_down = sig_mouse_down;
    klass->sig_mouse_up   = sig_mouse_up;
    klass->sig_mouse_move = sig_mouse_move;
    klass->sig_mouse_in   = sig_mouse_in;
    klass->sig_mouse_out  = sig_mouse_out;

    
	_gevasevh_to_gtk_signal_signals[M_DOWN] =
		gtk_signal_new("mouse_down",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHToGtkSignalsClass, sig_mouse_down),
                       gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);
	_gevasevh_to_gtk_signal_signals[M_UP] =
		gtk_signal_new("mouse_up",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHToGtkSignalsClass, sig_mouse_up),
					   gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);
	_gevasevh_to_gtk_signal_signals[M_MOVE] =
		gtk_signal_new("mouse_move",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHToGtkSignalsClass, sig_mouse_move),
					   gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);
	_gevasevh_to_gtk_signal_signals[M_IN] =
		gtk_signal_new("mouse_in",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHToGtkSignalsClass, sig_mouse_in),
					   gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);
	_gevasevh_to_gtk_signal_signals[M_OUT] =
		gtk_signal_new("mouse_out",
					   GTK_RUN_LAST,
					   GTK_CLASS_TYPE(object_class),
                       GTK_SIGNAL_OFFSET (GtkgEvasEvHToGtkSignalsClass, sig_mouse_out),
					   gtk_marshal_BOOL__POINTER_INT_INT_INT,
					   GTK_TYPE_BOOL, 4,
					   GTK_TYPE_POINTER,
					   GTK_TYPE_INT, GTK_TYPE_INT, GTK_TYPE_INT);


/* 	gtk_object_class_add_signals(object_class, _gevasevh_to_gtk_signal_signals, */
/* 								 LAST_SIGNAL); */

}

static void gevasevh_to_gtk_signal_init(GtkgEvasEvHToGtkSignals * ev)
{
}

GtkObject *gevasevh_to_gtk_signal_new(void)
{
	GtkgEvasEvHToGtkSignals *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_to_gtk_signal_get_type());
	hev = (GtkgEvasEvH *) ev;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_to_gtk_signal_destroy(GtkObject * object)
{
	GtkgEvasEvHToGtkSignals *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_TO_GTK_SIGNAL(object));
	ev = GTK_GEVASEVH_TO_GTK_SIGNAL(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}
