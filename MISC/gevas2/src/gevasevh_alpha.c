/*
 * changes the alpha value of the gevasobj on mouse in and back on mouse out
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


#include "gevasevh_alpha.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>

static void gevasevh_alpha_class_init(GtkgEvasEvHAlphaClass * klass);
static void gevasevh_alpha_init(GtkgEvasEvHAlpha * ev);
/* GtkObject functions */
static void gevasevh_alpha_destroy(GtkObject * object);
static void
gevasevh_alpha_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_alpha_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_HOT_VALUE
};


GEVASEV_HANDLER_RET
gevasev_alpha_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b, int _x,
					   int _y)
{
	GtkgEvasEvHAlpha *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_ALPHA(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_ALPHA(object);

	ev->cold_value = gevasobj_get_alpha(GTK_GEVASOBJ(gevasobj));
	gevasobj_set_alpha(GTK_GEVASOBJ(gevasobj), ev->hot_value);
	gevasobj_queue_redraw(GTK_GEVASOBJ(gevasobj));
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_alpha_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
						int _x, int _y)
{
	GtkgEvasEvHAlpha *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_ALPHA(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_ALPHA(object);

	gevasobj_set_alpha(GTK_GEVASOBJ(gevasobj), ev->cold_value);
	gevasobj_queue_redraw(GTK_GEVASOBJ(gevasobj));
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_alpha_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
						 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_alpha_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b, int _x,
					   int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_alpha_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
						 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_alpha_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHAlpha",
			sizeof(GtkgEvasEvHAlpha),
			sizeof(GtkgEvasEvHAlphaClass),
			(GtkClassInitFunc) gevasevh_alpha_class_init,
			(GtkObjectInitFunc) gevasevh_alpha_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_alpha_class_init(GtkgEvasEvHAlphaClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_alpha_destroy;
	object_class->get_arg = gevasevh_alpha_get_arg;
	object_class->set_arg = gevasevh_alpha_set_arg;

	evh_klass->handler_mouse_in = gevasev_alpha_mouse_in;
	evh_klass->handler_mouse_out = gevasev_alpha_mouse_out;
	evh_klass->handler_mouse_down = gevasev_alpha_mouse_down;
	evh_klass->handler_mouse_up = gevasev_alpha_mouse_up;
	evh_klass->handler_mouse_move = gevasev_alpha_mouse_move;

	gtk_object_add_arg_type(GTK_GEVASEVH_ALPHA_HOT_VALUE,
							GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_HOT_VALUE);


}

static void gevasevh_alpha_init(GtkgEvasEvHAlpha * ev)
{
	ev->hot_value = 255;
}

GtkObject *gevasevh_alpha_new(void)
{
	GtkgEvasEvHAlpha *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_alpha_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->hot_value = 255;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_alpha_destroy(GtkObject * object)
{
	GtkgEvasEvHAlpha *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_ALPHA(object));
	ev = GTK_GEVASEVH_ALPHA(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_alpha_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHAlpha *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_ALPHA(object));
	ev = GTK_GEVASEVH_ALPHA(object);


	switch (arg_id) {
		case ARG_HOT_VALUE:
			ev->hot_value = GTK_VALUE_INT(*arg);
			break;

		default:
			break;
	}
}

static void
gevasevh_alpha_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHAlpha *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_ALPHA(object));
	ev = GTK_GEVASEVH_ALPHA(object);

	switch (arg_id) {
		case ARG_HOT_VALUE:
			GTK_VALUE_INT(*arg) = ev->hot_value;
			break;

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
