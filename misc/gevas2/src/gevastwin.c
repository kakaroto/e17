/*
 * Gtk abstraction of a object and another in some form of binding.
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

#include <gevasevh_selectable.h>
#include <gevasevh_group_selector.h>

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


#include "gevastwin.h"

enum {
	ARG_0 = 100,				/* Skip 0, an invalid argument ID */
	ARG_MAINOBJ,
	ARG_AUXOBJ,
	ARG_ALIGNX,
	ARG_ALIGNY,
	ARG_OFFSETX,
	ARG_OFFSETY
};

static void gevastwin_class_init(GtkgEvasTwinClass * klass);
static void gevastwin_init(GtkgEvasTwin * ev);


/* GtkObject functions */
static void gevastwin_destroy(GtkObject * object);
static void gevastwin_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevastwin_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))



static GtkObjectClass *parent_class = NULL;

guint gevastwin_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasTwin",
			sizeof(GtkgEvasTwin),
			sizeof(GtkgEvasTwinClass),
			(GtkClassInitFunc) gevastwin_class_init,
			(GtkObjectInitFunc) gevastwin_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}


static void gevastwin_class_init(GtkgEvasTwinClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasObjClass *gobj = (GtkgEvasObjClass *) klass;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevastwin_destroy;

	object_class->get_arg = gevastwin_get_arg;
	object_class->set_arg = gevastwin_set_arg;

	gtk_object_add_arg_type(GTK_GEVASTWIN_MAINOBJ,
				GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_MAINOBJ);
	gtk_object_add_arg_type(GTK_GEVASTWIN_AUXOBJ,
				GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_AUXOBJ);

	gtk_object_add_arg_type(GTK_GEVASTWIN_ALIGNX,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_ALIGNX);
	gtk_object_add_arg_type(GTK_GEVASTWIN_ALIGNY,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_ALIGNY);

	gtk_object_add_arg_type(GTK_GEVASTWIN_OFFSETX,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_OFFSETX);
	gtk_object_add_arg_type(GTK_GEVASTWIN_OFFSETY,
				GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_OFFSETY);
}

static void gevastwin_init(GtkgEvasTwin * ev)
{
	ev->mainobj = 0;
	ev->auxobj = 0;
	ev->ox = 0;
	ev->oy = 5;
	ev->ax = 0;
	ev->ay = 1;
}

GtkgEvasTwin *gevastwin_new()
{
	GtkgEvasTwin *ev;

	ev = gtk_type_new(gevastwin_get_type());
	ev->mainobj = 0;
	ev->auxobj = 0;
	ev->ox = 0;
	ev->oy = 5;
	ev->ax = 0;
	ev->ay = 1;

	return GTK_GEVASTWIN(ev);
}

/* GtkObject functions */


static void gevastwin_destroy(GtkObject * object)
{
	GtkgEvasTwin *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));

	ev = GTK_GEVASTWIN(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

/*********************************************************************/

void _gevastwin_sync_obj(GtkgEvasTwin * ev, GtkgEvasObj * obj)
{
	double main_x = 0, main_y = 0, main_w = 0, main_h = 0;

	if (ev->mainobj && ev->auxobj) {
		gevasobj_get_geometry(ev->mainobj, &main_x, &main_y, &main_w, &main_h);

		if (obj == ev->auxobj) {
			ev->aux_obj_move(ev->auxobj, main_x + (ev->ax?main_w:0) + ev->ox, main_y + (ev->ay?main_h:0) + ev->oy);
			gevasobj_queue_redraw(ev->auxobj);
		}

		if (obj == ev->mainobj) {
			double ax = 0, ay = 0, ah = 0, aw = 0;

			gevasobj_get_geometry(ev->auxobj, &ax, &ay, &aw, &ah);
			ev->main_obj_move(ev->mainobj, ax - (ev->ax?main_w:0) - ev->ox, ay - (ev->ay?main_h:0) - ev->oy);
			gevasobj_queue_redraw(ev->mainobj);
		}
	}
}

void _gevastwin_move_xxx(GtkgEvasObj * object, double x, double y)
{
	GtkgEvasTwin *ev;
	gpointer d;

	d = gtk_object_get_data(GTK_OBJECT(object), GEVASTWIN_BACKWARD_LOOKUP_KEY);
	if (d) {
		ev = GTK_GEVASTWIN(d);
		if (ev->mainobj == object) {
			ev->main_obj_move(object, x, y);
			_gevastwin_sync_obj(ev, ev->auxobj);
		}
		else if (ev->auxobj == object) {
			ev->aux_obj_move(object, x, y);
			_gevastwin_sync_obj(ev, ev->mainobj);
		}
	}
}



static void gevastwin_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasTwin *ev;
	gchar *gstr;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));

	ev = GTK_GEVASTWIN(object);

	switch (arg_id) {
		case ARG_MAINOBJ:
			ev->mainobj = GTK_VALUE_POINTER(*arg);

			ev->main_obj_move = ev->mainobj->move;
			ev->mainobj->move = _gevastwin_move_xxx;
			gtk_object_set_data(GTK_OBJECT(ev->mainobj),
					    GEVASTWIN_BACKWARD_LOOKUP_KEY, ev);
			break;

		case ARG_AUXOBJ:
			ev->auxobj = GTK_VALUE_POINTER(*arg);

			ev->aux_obj_move = ev->auxobj->move;
			ev->auxobj->move = _gevastwin_move_xxx;
			_gevastwin_sync_obj(ev, ev->auxobj);
			gtk_object_set_data(GTK_OBJECT(ev->auxobj),
					    GEVASTWIN_BACKWARD_LOOKUP_KEY, ev);
			break;

		case ARG_ALIGNX:
			ev->ax = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		case ARG_ALIGNY:
			ev->ay = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		case ARG_OFFSETX:
			ev->ox = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		case ARG_OFFSETY:
			ev->oy = GTK_VALUE_INT(*arg);
			_gevastwin_sync_obj(ev, ev->auxobj);
			break;

		default:
			break;
	}
}

static void gevastwin_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasTwin *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTWIN(object));

	ev = GTK_GEVASTWIN(object);

	switch (arg_id) {
		case ARG_MAINOBJ:
			GTK_VALUE_POINTER(*arg) = ev->mainobj;
			break;
		case ARG_AUXOBJ:
			GTK_VALUE_POINTER(*arg) = ev->auxobj;
			break;
		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
