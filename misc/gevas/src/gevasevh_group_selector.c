/*
 * Draws a ractangle over the group_selector items and selects each of them on mouse up.
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


#include "gevasevh_group_selector.h"
#include "gevasevh_selectable.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>

static void gevasevh_group_selector_class_init(GtkgEvasEvHGroupSelectorClass * klass);
static void gevasevh_group_selector_init(GtkgEvasEvHGroupSelector * ev);
/* GtkObject functions */
static void gevasevh_group_selector_destroy(GtkObject * object);
static void
gevasevh_group_selector_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_group_selector_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);


enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_SELECTED_OBJ,
};

/*
void gevasevh_group_selector_add_evh( GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
}
*/

void gevasevh_group_selector_flushsel( 
	GtkgEvasEvHGroupSelector* ev )
{
	Evas_List tl = ev->selected_objs;
	while(tl)
	{
		if( tl->data )
			gevas_selectable_select( (GtkgEvasEvHSelectable*)tl->data, 0 );
		tl = tl->next;
	}
	ev->selected_objs = evas_list_free( ev->selected_objs );
	ev->selected_objs = 0;
}

void gevasevh_group_selector_addtosel( 
	GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
	ev->selected_objs = evas_list_append( ev->selected_objs, o);
	gevas_selectable_select( o , 1 );

}

void gevasevh_group_selector_remfromsel( 
	GtkgEvasEvHGroupSelector* ev, GtkgEvasEvHSelectable* o )
{
	gevas_selectable_select( o , 0 );
}



void gevasevh_group_selector_set_object( GtkgEvasEvHGroupSelector* object, GtkgEvasObj* obj )
{
	GtkgEvasEvHGroupSelector* ev = object;

	object->mark = obj;
	printf("gevasevh_group_selector_set_object()");
	if( !ev->rect ) 
	{
		GtkgEvasObj *ct;
		ct = ev->rect = (GtkgEvasObj*)gevasgrad_new(gevasobj_get_gevas(
			GTK_OBJECT(ev->mark)));
		gevasgrad_add_color(ct, 255, 255, 255, 255, 8);
		gevasgrad_add_color(ct, 255, 255, 0, 200, 8);
		gevasgrad_add_color(ct, 255, 0, 0, 150, 8);
		gevasgrad_add_color(ct, 0, 0, 0, 0, 8);
		gevasgrad_seal(ct);

		gevasobj_set_layer(ct, 9999);
		printf(" Made the gradient\n");
	}
	printf("gevasevh_group_selector_set_object(end)");
}


/*
void gevasev_group_selector_show(GtkgEvasEvHGroupSelector * ev, GtkgEvasObj * newobj)
{
	if (newobj && (newobj != ev->current)) {
		double x = 0, y = 0;
		if (ev->current) {
			gevasobj_get_location(ev->current, &x, &y);
			gevasobj_hide(ev->current);
		}
		gevasobj_show(newobj);
		gevasobj_move(newobj, x, y);
		if (newobj != ev->cold) {
			gevasobj_set_layer(newobj, GTK_GEVASEVH_GROUP_SELECTOR_TOP_LAYER);
		}
		gevasobj_queue_redraw(GTK_GEVASOBJ(newobj));

		ev->current = newobj;
	}
}
*/

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

//	gevasev_group_selector_show(ev, ev->hot);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

//	gevasev_group_selector_show(ev, ev->cold);
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
//	gevasev_group_selector_show(ev, ev->hot_clicked[_b - 1]);

	gevasevh_group_selector_flushsel( ev );

	ev->tl_top = _y;
	ev->tl_left= _x;

	printf("_x:%f _y:%f\n", _x, _y );
	gevasobj_move( ev->rect, _x, _y);
	gevasobj_resize( ev->rect, 2,2);
	gevasobj_show( ev->rect );
	gevasobj_queue_redraw( ev->rect );
	ev->tracking = 1;
	printf("gevasev_group_selector_mouse_down() done\n");

	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	double x=0,y=0,w=0,h=0;
	Evas_List list;
	void* data;
	GtkgEvasEvHGroupSelector *ev;

	printf("gevasev_group_selector_mouse_up()\n");

	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
	gevasobj_hide( ev->rect );
	ev->tracking = 0;

	x = ev->tl_left;
	y = ev->tl_top;
	w = _x - x;
	h = _y - y;

	printf(" x:%f y:%f w:%f h:%f\n",x,y,w,h);

	list = evas_objects_in_rect(
		gevas_get_evas( ev->rect->gevas ), x,y,w,h);

	while( list )
	{
		data = evas_get_data( 
			gevas_get_evas( ev->rect->gevas ),
			list->data,
			GEVASEVH_SELECTABLE_KEY
		);


		if( data ) 
		{
			gevasevh_group_selector_addtosel( ev, (GtkgEvasEvHSelectable*)data );
		}
		
		list = list->next;
	}

	printf("gevasev_group_selector_mouse_up(return)\n");


	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_group_selector_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

	if( ev->rect && ev->tracking ) 
	{
		double w=0, h=0, x=0, y=0;

		gevasobj_get_location( ev->rect, &x, &y );

		w = _x - x;
		h = _y - y;

		if( w < 2 ) w=2;
		if( h < 2 ) h=2;

		gevasobj_resize( ev->rect , w, h);
		gevasobj_queue_redraw( ev->rect );

		printf(" w:%f _x:%f x:%f h:%f _y:%f y:%f\n",w,_x,x,h,_y,y);
	}

	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_group_selector_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHGroupSelector",
			sizeof(GtkgEvasEvHGroupSelector),
			sizeof(GtkgEvasEvHGroupSelectorClass),
			(GtkClassInitFunc) gevasevh_group_selector_class_init,
			(GtkObjectInitFunc) gevasevh_group_selector_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_group_selector_class_init(GtkgEvasEvHGroupSelectorClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_group_selector_destroy;
	object_class->get_arg = gevasevh_group_selector_get_arg;
	object_class->set_arg = gevasevh_group_selector_set_arg;

	evh_klass->handler_mouse_in = gevasev_group_selector_mouse_in;
	evh_klass->handler_mouse_out = gevasev_group_selector_mouse_out;
	evh_klass->handler_mouse_down = gevasev_group_selector_mouse_down;
	evh_klass->handler_mouse_up = gevasev_group_selector_mouse_up;
	evh_klass->handler_mouse_move = gevasev_group_selector_mouse_move;

//	gtk_object_add_arg_type(GTK_GEVASEVH_GROUP_SELECTOR_SELECTED_OBJ,
//		GTK_TYPE_POINTER, GTK_ARG_READWRITE, ARG_SELECTED_OBJ);

}

static void gevasevh_group_selector_init(GtkgEvasEvHGroupSelector * ev)
{
	ev->selected = 0;
	ev->rect = 0;
	ev->selected_objs =0;
}

GtkObject *gevasevh_group_selector_new(void)
{
	GtkgEvasEvHGroupSelector *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_group_selector_get_type());
	hev = (GtkgEvasEvH *) ev;
	ev->selected = 0;
	ev->rect = 0;
	ev->selected_objs =0;

	return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_group_selector_destroy(GtkObject * object)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void
gevasevh_group_selector_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);

/*
	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			ev->selected = GTK_VALUE_POINTER(*arg);
			gevasobj_add_evhandler(ev->selected, object);
			break;
		default:
			break;
	}
*/
}

static void
gevasevh_group_selector_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHGroupSelector *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_GROUP_SELECTOR(object));
	ev = GTK_GEVASEVH_GROUP_SELECTOR(object);
/*
	switch (arg_id) {
		case ARG_SELECTED_OBJ:
			GTK_VALUE_POINTER(*arg) = ev->selected;
			break;
		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
*/
}
