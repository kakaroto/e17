/*
 * Gtk abstraction of Evas_Object showing some text.
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
#include "gevastext.h"

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_XXX
};

static void gevastext_class_init(GtkgEvasTextClass * klass);
static void gevastext_init(GtkgEvasText * ev);


/* GtkObject functions */
static void gevastext_destroy(GtkObject * object);
static void gevastext_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevastext_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))

GtkgEvasText *gevastext_new_full(GtkgEvas * gevas, char *font, int size,
								 char *text)
{
	GtkgEvasText *ev;
	Evas_Object eobj;

	ev = gtk_type_new(gevastext_get_type());

	gevasobj_set_gevas(ev, gevas);

	eobj = evas_add_text(EVAS(ev), font, size, text);
	_gevas_set_obj(GTK_OBJECT(ev), eobj);

	return GTK_GEVASTEXT(ev);
}

GtkgEvasText *gevastext_new(GtkgEvas * gevas)
{
	return gevastext_new_full(gevas, GEVASTEXT_DEFAULT_FONT, 12, "");
}


gchar *gevastext_get_string(GtkgEvasObj * object)
{
	return (gchar *) evas_get_text_string(EVAS(object), EVASO(object));
}

gchar *gevastext_get_font(GtkgEvasObj * object)
{
	return (gchar *) evas_get_text_font(EVAS(object), EVASO(object));
}

int gevastext_get_text_size(GtkgEvasObj * object)
{
	return evas_get_text_size(EVAS(object), EVASO(object));
}

int
gevastext_at_position(GtkgEvasObj * object, double x, double y, int *char_x,
					  int *char_y, int *char_w, int *char_h)
{
	return evas_text_at_position(EVAS(object), EVASO(object), x, y, char_x,
								 char_y, char_w, char_h);
}

void
gevastext_at(GtkgEvasObj * object, int index, int *char_x, int *char_y,
			 int *char_w, int *char_h)
{
	evas_text_at(EVAS(object), EVASO(object), index, char_x, char_y, char_w,
				 char_h);
}

void
gevastext_get_ascent_descent(GtkgEvasObj * object, double *ascent,
							 double *descent)
{
	evas_text_get_ascent_descent(EVAS(object), EVASO(object), ascent, descent);
}

double gevastext_get_ascent(GtkgEvasObj * object)
{
	double a = 0, d = 0;
	evas_text_get_ascent_descent(EVAS(object), EVASO(object), &a, &d);
	return a;
}

double gevastext_get_descent(GtkgEvasObj * object)
{
	double a = 0, d = 0;
	evas_text_get_ascent_descent(EVAS(object), EVASO(object), &a, &d);
	return d;
}

void
gevastext_get_max_ascent_descent(GtkgEvasObj * object, double *ascent,
								 double *descent)
{
	evas_text_get_max_ascent_descent(EVAS(object), EVASO(object), ascent,
									 descent);
}

double gevastext_get_max_ascent(GtkgEvasObj * object)
{
	double a = 0, d = 0;
	evas_text_get_max_ascent_descent(EVAS(object), EVASO(object), &a, &d);
	return a;
}

double gevastext_get_max_descent(GtkgEvasObj * object)
{
	double a = 0, d = 0;
	evas_text_get_max_ascent_descent(EVAS(object), EVASO(object), &a, &d);
	return d;
}

void
gevastext_get_advance(GtkgEvasObj * object, double *h_advance,
					  double *v_advance)
{
	evas_text_get_advance(EVAS(object), EVASO(object), h_advance, v_advance);
}

double gevastext_get_inset(GtkgEvasObj * object)
{
	return evas_text_get_inset(EVAS(object), EVASO(object));
}

void gevastext_set_string(GtkgEvasObj * object, const gchar * text)
{
	evas_set_text(EVAS(object), EVASO(object), (char *) text);
}

void
gevastext_set_font(GtkgEvasObj * object, const gchar * font, const int size)
{
	evas_set_font(EVAS(object), EVASO(object), (char *) font, (int) size);
}


static GtkObjectClass *parent_class = NULL;

guint gevastext_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasText",
			sizeof(GtkgEvasText),
			sizeof(GtkgEvasTextClass),
			(GtkClassInitFunc) gevastext_class_init,
			(GtkObjectInitFunc) gevastext_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasobj_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevastext_class_init(GtkgEvasTextClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gevasobj_get_type());

	object_class->destroy = gevastext_destroy;

	object_class->get_arg = gevastext_get_arg;
	object_class->set_arg = gevastext_set_arg;

	gtk_object_add_arg_type(GTK_GEVASTEXT_IMAGENAME,
							GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_XXX);

}

static void gevastext_init(GtkgEvasText * ev)
{

}

/*
GtkgEvasText*
gevastext_new (void)
{
  GtkgEvasText *ev;

  ev = gtk_type_new (gevastext_get_type ());

  return GTK_GEVASTEXT (ev);
}
*/

/* GtkObject functions */


static void gevastext_destroy(GtkObject * object)
{
	GtkgEvasText *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTEXT(object));

	ev = GTK_GEVASTEXT(object);



	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void gevastext_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasText *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTEXT(object));

	ev = GTK_GEVASTEXT(object);

	switch (arg_id) {
/*
	case ARG_XXX:
		{
		Evas e;
		Evas_Object o;
		

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

static void gevastext_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasText *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASTEXT(object));

	ev = GTK_GEVASTEXT(object);

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
