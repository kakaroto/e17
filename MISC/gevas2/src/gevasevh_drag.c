/*
 * Gtk abstraction of left dragging an object around a gevas
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


#include "gevasevh_drag.h"


static void gevasevh_drag_class_init(GtkgEvasEvHDragClass * klass);
static void gevasevh_drag_init(GtkgEvasEvHDrag * ev);
/* GtkObject functions */
static void gevasevh_drag_destroy(GtkObject * object);


GEVASEV_HANDLER_RET
gevasev_drag_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b, int _x,
					  int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_drag_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b, int _x,
					   int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


GEVASEV_HANDLER_RET
gevasev_drag_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
						int _x, int _y)
{
	GtkgEvasEvHDrag *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_DRAG(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_DRAG(object);

//    printf("gevasev_drag_mouse_down() _x:%d _y:%d\n",_x,_y);
    if (_b == 1) {
		Evas_Coord x = 0, y = 0, w = 0, h = 0;
		gevasobj_get_geometry(GTK_GEVASOBJ(gevasobj), &x, &y, &w, &h);
//        printf("gevasev_drag_mouse_down(2) _x:%d _y:%d\n",_x,_y);
		ev->dragging = 1;
		ev->click_x_offset = _x - x;
		ev->click_y_offset = _y - y;
/*         printf("gevasev_drag_mouse_down(3) _x:%d _y:%d\n",_x,_y); */
/*         printf("Starting to drag coffx:%f coffy:%f _x:%d _y:%d x:%d y:%d\n", */
/*                ev->click_x_offset, ev->click_y_offset, */
/*                _x, _y, x, y  */
/*             ); */
    }
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_drag_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b, int _x,
					  int _y)
{
	GtkgEvasEvHDrag *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_DRAG(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_DRAG(object);
	if (_b == 1) {
		ev->dragging = 0;
	}
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_drag_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
						int _x, int _y)
{
	GtkgEvasEvHDrag *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_DRAG(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_DRAG(object);
	if (ev->dragging && _b == 1)
    {
        gint x = _x - ev->click_x_offset;
        gint y = _y - ev->click_y_offset;

/*        printf("gevasev_drag_mouse_move() x:%d y:%d _x:%d _y:%d\n",x,y,_x,_y);*/
        
        
        if( x < 0 ) x = 0;
        if( y < 0 ) y = 0;
        
        gevasobj_move(GTK_GEVASOBJ(gevasobj), x, y );
        
/*         { */
/*             Evas_Coord cx = 0, cy = 0, cw = 0, ch = 0; */
/*             gevasobj_get_geometry(GTK_GEVASOBJ(gevasobj), &cx, &cy, &cw, &ch); */
/*             gevasobj_move_relative(GTK_GEVASOBJ(gevasobj), x - cx, y - cy ); */
/*             printf("gevasev_drag_mouse_move() x:%d y:%d _x:%d _y:%d\n",x,y,_x,_y); */
/*         } */
    }
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_drag_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHDrag",
			sizeof(GtkgEvasEvHDrag),
			sizeof(GtkgEvasEvHDragClass),
			(GtkClassInitFunc) gevasevh_drag_class_init,
			(GtkObjectInitFunc) gevasevh_drag_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_drag_class_init(GtkgEvasEvHDragClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_drag_destroy;

	evh_klass->handler_mouse_in = gevasev_drag_mouse_in;
	evh_klass->handler_mouse_out = gevasev_drag_mouse_out;
	evh_klass->handler_mouse_down = gevasev_drag_mouse_down;
	evh_klass->handler_mouse_up = gevasev_drag_mouse_up;
	evh_klass->handler_mouse_move = gevasev_drag_mouse_move;
}

static void gevasevh_drag_init(GtkgEvasEvHDrag * ev)
{
	ev->dragging = 0;
}

GtkObject *gevasevh_drag_new(void)
{
	GtkgEvasEvHDrag *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_drag_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->dragging = 0;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_drag_destroy(GtkObject * object)
{
	GtkgEvasEvHDrag *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_DRAG(object));
	ev = GTK_GEVASEVH_DRAG(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}
