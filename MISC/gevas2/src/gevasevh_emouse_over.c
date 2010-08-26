/*
 * Makes the attached gevasobj larger when the mouse moves over it
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


#include "gevasevh_emouse_over.h"
#include "gevasimage.h"

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_SCALE_X,
	ARG_SCALE_Y
};


static void gevasevh_emouse_over_class_init(GtkgEvasEvHEMouseOverClass * klass);
static void gevasevh_emouse_over_init(GtkgEvasEvHEMouseOver * ev);
/* GtkObject functions */
static void gevasevh_emouse_over_destroy(GtkObject * object);
static void
gevasevh_emouse_over_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_emouse_over_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


GEVASEV_HANDLER_RET
gevasev_emouse_over_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	Evas_Coord x = 0, y = 0, w = 0, h = 0;
    double sw = 0, sh = 0;
	GtkgEvasEvHEMouseOver *ev;
	GtkgEvasObj *ggobj = GTK_GEVASOBJ(gevasobj);
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_EMOUSE_OVER(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_EMOUSE_OVER(object);

	gevasobj_get_geometry(ggobj, &x, &y, &w, &h);
	sw = w * ev->scale_factor_x;
	sh = h * ev->scale_factor_y;
	gevasobj_resize(ggobj, sw, sh);
	ev->old_w = w;
	ev->old_h = h;
	ev->old_x_offset = ABS((sw - w) / 2);
	ev->old_y_offset = ABS((sh - h) / 2);
	gevasobj_move(ggobj, x - ev->old_x_offset, y - ev->old_y_offset);

	gevasimage_set_image_fill(ggobj, 0.0, 0.0, sw, sh);
	gevasobj_queue_redraw(ggobj);


	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_emouse_over_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	Evas_Coord x = 0, y = 0, w = 0, h = 0;
	GtkgEvasEvHEMouseOver *ev;
	GtkgEvasObj *ggobj = GTK_GEVASOBJ(gevasobj);
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_EMOUSE_OVER(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_EMOUSE_OVER(object);

	gevasobj_get_geometry(ggobj, &x, &y, &w, &h);
	gevasobj_resize(ggobj, ev->old_w, ev->old_h);
	gevasobj_move(ggobj, x + ev->old_x_offset, y + ev->old_y_offset);
	gevasimage_set_image_fill(ggobj, 0.0, 0.0, ev->old_w, ev->old_h);
	gevasobj_queue_redraw(ggobj);

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_emouse_over_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_emouse_over_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_emouse_over_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_emouse_over_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHEMouseOver",
			sizeof(GtkgEvasEvHEMouseOver),
			sizeof(GtkgEvasEvHEMouseOverClass),
			(GtkClassInitFunc) gevasevh_emouse_over_class_init,
			(GtkObjectInitFunc) gevasevh_emouse_over_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_emouse_over_class_init(GtkgEvasEvHEMouseOverClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_emouse_over_destroy;
	object_class->get_arg = gevasevh_emouse_over_get_arg;
	object_class->set_arg = gevasevh_emouse_over_set_arg;

	evh_klass->handler_mouse_in = gevasev_emouse_over_mouse_in;
	evh_klass->handler_mouse_out = gevasev_emouse_over_mouse_out;
	evh_klass->handler_mouse_down = gevasev_emouse_over_mouse_down;
	evh_klass->handler_mouse_up = gevasev_emouse_over_mouse_up;
	evh_klass->handler_mouse_move = gevasev_emouse_over_mouse_move;

	gtk_object_add_arg_type(GTK_GEVASEVH_EMOUSE_OVER_SCALE_X,
							GTK_TYPE_DOUBLE, GTK_ARG_READWRITE, ARG_SCALE_X);
	gtk_object_add_arg_type(GTK_GEVASEVH_EMOUSE_OVER_SCALE_Y,
							GTK_TYPE_DOUBLE, GTK_ARG_READWRITE, ARG_SCALE_Y);
}

static void gevasevh_emouse_over_init(GtkgEvasEvHEMouseOver * ev)
{
	ev->scale_factor_x = 1.1;
	ev->scale_factor_y = 1.1;
}

GtkObject *gevasevh_emouse_over_new(void)
{
	GtkgEvasEvHEMouseOver *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_emouse_over_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->scale_factor_x = 1.1;
	ev->scale_factor_y = 1.1;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_emouse_over_destroy(GtkObject * object)
{
	GtkgEvasEvHEMouseOver *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_EMOUSE_OVER(object));
	ev = GTK_GEVASEVH_EMOUSE_OVER(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}


static void
gevasevh_emouse_over_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHEMouseOver *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_EMOUSE_OVER(object));
	ev = GTK_GEVASEVH_EMOUSE_OVER(object);

	switch (arg_id) {
		case ARG_SCALE_X:
			ev->scale_factor_x = GTK_VALUE_DOUBLE(*arg);
			break;

		case ARG_SCALE_Y:
			ev->scale_factor_y = GTK_VALUE_DOUBLE(*arg);
			break;

		default:
			break;
	}
}

static void
gevasevh_emouse_over_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHEMouseOver *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_EMOUSE_OVER(object));
	ev = GTK_GEVASEVH_EMOUSE_OVER(object);

	switch (arg_id) {
		case ARG_SCALE_X:
			GTK_VALUE_DOUBLE(*arg) = ev->scale_factor_x;
			break;

		case ARG_SCALE_Y:
			GTK_VALUE_DOUBLE(*arg) = ev->scale_factor_y;
			break;

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
