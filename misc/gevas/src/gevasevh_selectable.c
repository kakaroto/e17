/*
 * Draws an image behind the given evasobj when told to.
 *
 *
 *
 * Copyright (C) 2001 Ben Martin.
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


#include "gevasevh_selectable.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>

static void gevasevh_selectable_class_init(GtkgEvasEvHSelectableClass * klass);
static void gevasevh_selectable_init(GtkgEvasEvHSelectable * ev);
/* GtkObject functions */
static void gevasevh_selectable_destroy(GtkObject * object);
static void
gevasevh_selectable_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_selectable_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_SELECTED_OBJ,
};

void gevasevh_selectable_set_normal_gevasobj(
	GtkgEvasEvHSelectable* ev, 
	GtkgEvasObj* nor )
{
	ev->normal = nor;

	evas_put_data(
		gevas_get_evas( nor->gevas ),
		_gevas_get_obj(GTK_OBJECT(nor)),
		GEVASEVH_SELECTABLE_KEY,
		ev
	);
}

void gevasevh_selectable_set_selector( GtkgEvasEvHSelectable* evh, GtkObject* evh_selector )
{
	evh->evh_selector = evh_selector;
}


//
// A callback type function used by group_selector to select and unselect objects
// as it sees fit.
//
void gevas_selectable_select( GtkgEvasEvHSelectable * ev, gboolean s )
{
	ev->tracking = s;

	if( s )
	{
		double x=0, y=0;
		int lay=0;

		printf("showing for selectable\n");
		gevasobj_get_location( ev->normal, &x, &y );
		gevasobj_move( ev->selected, x, y);


		gevasobj_set_layer( ev->selected, 
			gevasobj_get_layer( ev->normal) - 1
		);

		gevasobj_show( ev->selected );
	}
	else {
		printf("hiding for selectable\n");
		gevasobj_hide( ev->selected );
	}
}


GEVASEV_HANDLER_RET
gevasev_selectable_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

//	gevasev_selectable_show(ev, ev->hot);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

//	gevasev_selectable_show(ev, ev->cold);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

	printf("selectable down\n");
//	gevas_selectable_select( ev , 1 );

	gevasevh_group_selector_flushsel( ev->evh_selector );
	gevasevh_group_selector_addtosel( ev->evh_selector, ev );
	printf("selectable down(ret)\n");


	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

//	gevas_selectable_select( ev , 0 );

//	gevasevh_group_selector_remfromsel( ev->evh_selector, ev );
	ev->tracking = 0;

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_selectable_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHSelectable *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_SELECTABLE(object);

	if( ev->tracking )
	{
		double x=0, y=0;
		gevasobj_get_location( ev->normal, &x, &y );
		gevasobj_move( ev->selected, x, y);
	}

	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_selectable_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHSelectable",
			sizeof(GtkgEvasEvHSelectable),
			sizeof(GtkgEvasEvHSelectableClass),
			(GtkClassInitFunc) gevasevh_selectable_class_init,
			(GtkObjectInitFunc) gevasevh_selectable_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_selectable_class_init(GtkgEvasEvHSelectableClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_selectable_destroy;
	object_class->get_arg = gevasevh_selectable_get_arg;
	object_class->set_arg = gevasevh_selectable_set_arg;

	evh_klass->handler_mouse_in = gevasev_selectable_mouse_in;
	evh_klass->handler_mouse_out = gevasev_selectable_mouse_out;
	evh_klass->handler_mouse_down = gevasev_selectable_mouse_down;
	evh_klass->handler_mouse_up = gevasev_selectable_mouse_up;
	evh_klass->handler_mouse_move = gevasev_selectable_mouse_move;

	gtk_object_add_arg_type(GTK_GEVASEVH_SELECTABLE_SELECTED_OBJ,
		GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_SELECTED_OBJ);

}

static void gevasevh_selectable_init(GtkgEvasEvHSelectable * ev)
{
	ev->selected = 0;
}

GtkObject *gevasevh_selectable_new(void)
{
	GtkgEvasEvHSelectable *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_selectable_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->selected = 0;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_selectable_destroy(GtkObject * object)
{
	GtkgEvasEvHSelectable *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object));
	ev = GTK_GEVASEVH_SELECTABLE(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_selectable_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHSelectable *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object));
	ev = GTK_GEVASEVH_SELECTABLE(object);


	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			ev->selected = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->selected, object);
			break;
		default:
			break;
	}
}

static void
gevasevh_selectable_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHSelectable *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_SELECTABLE(object));
	ev = GTK_GEVASEVH_SELECTABLE(object);

	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->selected;
			break;
		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
