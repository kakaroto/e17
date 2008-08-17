/*
 * changes the gevasobj to another on mouse in/out and mouse click (1,2,3)
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


#include "gevasevh_obj_changer.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>

static void gevasevh_obj_changer_class_init(GtkgEvasEvHObjChangerClass * klass);
static void gevasevh_obj_changer_init(GtkgEvasEvHObjChanger * ev);
/* GtkObject functions */
static void gevasevh_obj_changer_destroy(GtkObject * object);
static void
gevasevh_obj_changer_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_obj_changer_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_COLD_OBJ,
	ARG_HOT_OBJ,
	ARG_HOTCLK1_OBJ,
	ARG_HOTCLK2_OBJ,
	ARG_HOTCLK3_OBJ
};

void gevasev_obj_changer_show(GtkgEvasEvHObjChanger * ev, GtkgEvasObj * newobj)
{
	if (newobj && (newobj != ev->current)) {
		Evas_Coord x = 0, y = 0;
		if (ev->current) {
			gevasobj_get_location(ev->current, &x, &y);
			gevasobj_hide(ev->current);
		}
		gevasobj_show(newobj);
		gevasobj_move(newobj, x, y);
/*		ev->old_layer = gevasobj_get_layer( newobj ); */
		if (newobj != ev->cold) {
			gevasobj_set_layer(newobj, GTK_GEVASEVH_OBJ_CHANGER_TOP_LAYER);
		}
		gevasobj_queue_redraw(GTK_GEVASOBJ(newobj));

		ev->current = newobj;
	}
}


GEVASEV_HANDLER_RET
gevasev_obj_changer_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);

	gevasev_obj_changer_show(ev, ev->hot);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_obj_changer_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);

	gevasev_obj_changer_show(ev, ev->cold);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_obj_changer_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);
	gevasev_obj_changer_show(ev, ev->hot_clicked[_b - 1]);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_obj_changer_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);
	gevasev_obj_changer_show(ev, ev->hot);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_obj_changer_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_obj_changer_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHObjChanger",
			sizeof(GtkgEvasEvHObjChanger),
			sizeof(GtkgEvasEvHObjChangerClass),
			(GtkClassInitFunc) gevasevh_obj_changer_class_init,
			(GtkObjectInitFunc) gevasevh_obj_changer_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_obj_changer_class_init(GtkgEvasEvHObjChangerClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_obj_changer_destroy;
	object_class->get_arg = gevasevh_obj_changer_get_arg;
	object_class->set_arg = gevasevh_obj_changer_set_arg;

	evh_klass->handler_mouse_in = gevasev_obj_changer_mouse_in;
	evh_klass->handler_mouse_out = gevasev_obj_changer_mouse_out;
	evh_klass->handler_mouse_down = gevasev_obj_changer_mouse_down;
	evh_klass->handler_mouse_up = gevasev_obj_changer_mouse_up;
	evh_klass->handler_mouse_move = gevasev_obj_changer_mouse_move;

	gtk_object_add_arg_type(GTK_GEVASEVH_OBJ_CHANGER_COLD_OBJ,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_COLD_OBJ);
	gtk_object_add_arg_type(GTK_GEVASEVH_OBJ_CHANGER_HOT_OBJ,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_HOT_OBJ);
	gtk_object_add_arg_type(GTK_GEVASEVH_OBJ_CHANGER_HOTCLK1_OBJ,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE,
							ARG_HOTCLK1_OBJ);
	gtk_object_add_arg_type(GTK_GEVASEVH_OBJ_CHANGER_HOTCLK2_OBJ,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE,
							ARG_HOTCLK2_OBJ);
	gtk_object_add_arg_type(GTK_GEVASEVH_OBJ_CHANGER_HOTCLK3_OBJ,
							GTK_TYPE_POINTER, GTK_ARG_READWRITE,
							ARG_HOTCLK3_OBJ);


}

static void gevasevh_obj_changer_init(GtkgEvasEvHObjChanger * ev)
{
	ev->cold = 0;
	ev->hot = 0;
}

GtkObject *gevasevh_obj_changer_new(void)
{
	GtkgEvasEvHObjChanger *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_obj_changer_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->cold = 0;
	ev->hot = 0;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_obj_changer_destroy(GtkObject * object)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object));
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_obj_changer_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object));
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);


	switch (arg_id) {
		case ARG_COLD_OBJ:
			ev->cold = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->cold, object);
			ev->current = ev->cold;
			break;
		case ARG_HOT_OBJ:
			ev->hot = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->hot, object);
			break;
		case ARG_HOTCLK1_OBJ:
			ev->hot_clicked[0] = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->hot_clicked[0], object);
			break;
		case ARG_HOTCLK2_OBJ:
			ev->hot_clicked[1] = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->hot_clicked[1], object);
			break;
		case ARG_HOTCLK3_OBJ:
			ev->hot_clicked[2] = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->hot_clicked[2], object);
			break;

		default:
			break;
	}
}

static void
gevasevh_obj_changer_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHObjChanger *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_OBJ_CHANGER(object));
	ev = GTK_GEVASEVH_OBJ_CHANGER(object);

	switch (arg_id) {
		case ARG_COLD_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->cold;
			break;
		case ARG_HOT_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->hot;
			break;
		case ARG_HOTCLK1_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->hot_clicked[0];
			break;
		case ARG_HOTCLK2_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->hot_clicked[1];
			break;
		case ARG_HOTCLK3_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->hot_clicked[2];
			break;

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
