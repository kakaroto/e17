/*
 * Gtk abstraction of event. This lets evas events be chained onto many handlers that are
 * attached in an order to a gevasobj
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


#include "gevasev_handler.h"

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_GEVAS_OBJ
};

static void gevasevh_class_init(GtkgEvasEvHClass * klass);
static void gevasevh_init(GtkgEvasEvH * ev);


/* GtkObject functions */
static void gevasevh_destroy(GtkObject * object);
static void gevasevh_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasevh_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

static GEVASEV_HANDLER_PRIORITY gevasev_handler_get_priority( GtkgEvasEvH* evh )
{
	return GEVASEV_HANDLER_PRIORITY_NORMAL;
}

GEVASEV_HANDLER_RET
gevasev_handler_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
						 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_handler_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
						  int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_handler_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
						   int _x, int _y)
{
	GtkgEvasEvH *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH(object), GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH(object);

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_handler_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
						 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_handler_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
						   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvH",
			sizeof(GtkgEvasEvH),
			sizeof(GtkgEvasEvHClass),
			(GtkClassInitFunc) gevasevh_class_init,
			(GtkObjectInitFunc) gevasevh_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_class_init(GtkgEvasEvHClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevasevh_destroy;

	object_class->get_arg = gevasevh_get_arg;
	object_class->set_arg = gevasevh_set_arg;

	klass->handler_mouse_in = gevasev_handler_mouse_in;
	klass->handler_mouse_out = gevasev_handler_mouse_out;
	klass->handler_mouse_down = gevasev_handler_mouse_down;
	klass->handler_mouse_up = gevasev_handler_mouse_up;
	klass->handler_mouse_move = gevasev_handler_mouse_move;
	klass->get_priority = gevasev_handler_get_priority;

	gtk_object_add_arg_type(GTK_GEVASEVH_GEVASOBJ,
		GTK_TYPE_POINTER, GTK_ARG_WRITABLE, ARG_GEVAS_OBJ);
}

static void gevasevh_init(GtkgEvasEvH * ev)
{
	ev->eobj = NULL;
}

GtkObject *gevasevh_new(void)
{
	GtkgEvasEvH *ev;

	ev = gtk_type_new(gevasevh_get_type());
	ev->eobj = NULL;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_destroy(GtkObject * object)
{
	GtkgEvasEvH *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH(object));

	ev = GTK_GEVASEVH(object);

	ev->eobj = NULL;


	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void gevasevh_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvH *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH(object));

	ev = GTK_GEVASEVH(object);

	switch (arg_id) {
		case ARG_GEVAS_OBJ:
			ev->eobj = GTK_VALUE_POINTER(*arg);
			break;

		default:
			break;
	}
}

static void gevasevh_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvH *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS(object));

	ev = GTK_GEVASEVH(object);

	switch (arg_id) {
			/*case ARG_GEVAS:
			   GTK_VALUE_POINTER (*arg) = ev->gevas;
			   break;
			 */

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}


