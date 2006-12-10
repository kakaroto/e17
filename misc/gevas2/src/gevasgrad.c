/*
 * Gtk abstraction of Evas_Object showing a gradient
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

#include <Evas.h>
#include "gevasgrad.h"

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_XXX
};

static void gevasgrad_class_init(GtkgEvasGradClass * klass);
static void gevasgrad_init(GtkgEvasGrad * ev);


/* GtkObject functions */
static void gevasgrad_destroy(GtkObject * object);
static void gevasgrad_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasgrad_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))


GtkgEvasGrad *gevasgrad_new(GtkgEvas * gevas)
{
	GtkgEvasGrad *ev;
	Evas_Object* eobj;

	ev = gtk_type_new(gevasgrad_get_type());

	gevasobj_set_gevas(ev, gevas);

	eobj = evas_object_gradient_add(gevas_get_evas(gevas));
	_gevas_set_obj(GTK_OBJECT(ev), eobj);
	gevasgrad_clear_gradient(GTK_GEVASOBJ(ev));

	return GTK_GEVASGRAD(ev);
}

/* void gevasgrad_seal(GtkgEvasObj * object) */
/* { */
/* 	GtkgEvasGrad *ev; */
/* 	g_return_if_fail(object != NULL); */
/* 	g_return_if_fail(GTK_IS_GEVASGRAD(object)); */
/* 	ev = GTK_GEVASGRAD(object); */

/* 	evas_set_gradient(EVAS(ev), EVASO(ev), ev->gradient); */
/* } */

void gevasgrad_set_angle( GtkgEvasObj* object, double angle)
{
	GtkgEvasGrad *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASGRAD(object));
	ev = GTK_GEVASGRAD(object);

	evas_object_gradient_angle_set( EVASO(ev), angle);
}


void gevasgrad_clear_gradient(GtkgEvasObj * object)
{
	GtkgEvasGrad *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASGRAD(object));
	ev = GTK_GEVASGRAD(object);

    evas_object_gradient_clear( EVASO(ev) );
    
/* 	if (ev->gradient) { */
/* 		evas_gradient_free(ev->gradient); */
/* 		ev->gradient = NULL; */
/* 	} */

/* 	ev->gradient = evas_gradient_new(); */
}


void
gevasgrad_add_color(GtkgEvasObj * object, int r, int g, int b, int a, int dist)
{
	GtkgEvasGrad *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASGRAD(object));
	ev = GTK_GEVASGRAD(object);

    evas_object_gradient_color_stop_add( EVASO(ev), r, g, b, a, dist);
//    evas_object_gradient_color_add(ev->gradient, r, g, b, a, dist);
}



static GtkObjectClass *parent_class = NULL;

guint gevasgrad_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasGrad",
			sizeof(GtkgEvasGrad),
			sizeof(GtkgEvasGradClass),
			(GtkClassInitFunc) gevasgrad_class_init,
			(GtkObjectInitFunc) gevasgrad_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasobj_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasgrad_class_init(GtkgEvasGradClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gevasobj_get_type());

	object_class->destroy = gevasgrad_destroy;

	object_class->get_arg = gevasgrad_get_arg;
	object_class->set_arg = gevasgrad_set_arg;

	gtk_object_add_arg_type(GTK_GEVASGRAD_IMAGENAME,
							GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_XXX);

}

static void gevasgrad_init(GtkgEvasGrad * ev)
{
}

/*
GtkgEvasGrad*
gevasgrad_new (void)
{
  GtkgEvasGrad *ev;

  ev = gtk_type_new (gevasgrad_get_type ());

  return GTK_GEVASGRAD (ev);
}
*/

/* GtkObject functions */


static void gevasgrad_destroy(GtkObject * object)
{
	GtkgEvasGrad *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASGRAD(object));

	ev = GTK_GEVASGRAD(object);



	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void gevasgrad_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasGrad *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASGRAD(object));

	ev = GTK_GEVASGRAD(object);

	switch (arg_id) {
/*
	case ARG_XXX:
		{
		Evas e;
		Evas_Object* o;
		

		gstr = GTK_VALUE_STRING (*arg);
		_gevasobj_ensure_obj_free( object );
		e = _gevas_evas(object);			
		o = evas_add_image_from_file(EVAS(ev), gstr);
		_gevas_set_obj(object, o);
		}
		break;
*/

		default:
			break;
	}
}

static void gevasgrad_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasGrad *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASGRAD(object));

	ev = GTK_GEVASGRAD(object);

	switch (arg_id) {
/*    case ARG_XXX:
		GTK_VALUE_POINTER (*arg) = ev->gevas;
		break;
*/

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
