/*
 * Gtk abstraction of Evas_Object showing an image.
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


#include "gevasimage.h"

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_IMAGENAME
};

static void gevasimage_class_init(GtkgEvasImageClass * klass);
static void gevasimage_init(GtkgEvasImage * ev);


/* GtkObject functions */
static void gevasimage_destroy(GtkObject * object);
static void gevasimage_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasimage_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))


void
gevasimage_set_image_fill(GtkgEvasObj * object, double x, double y, double w,
						  double h)
{
	evas_set_image_fill(EVAS(object), EVASO(object), x, y, w, h);
}

void
gevasimage_set_image_border(GtkgEvasObj * object, int l, int r, int t, int b)
{
	evas_set_image_border(EVAS(object), EVASO(object), l, r, t, b);
}

void gevasimage_get_image_size(GtkgEvasObj * object, int *w, int *h)
{
	evas_get_image_size(EVAS(object), EVASO(object), w, h);
}

void
gevasimage_get_image_border(GtkgEvasObj * object, int *l, int *r, int *t,
							int *b)
{
	evas_get_image_border(EVAS(object), EVASO(object), l, r, t, b);
}


static GtkObjectClass *parent_class = NULL;

guint gevasimage_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasImage",
			sizeof(GtkgEvasImage),
			sizeof(GtkgEvasImageClass),
			(GtkClassInitFunc) gevasimage_class_init,
			(GtkObjectInitFunc) gevasimage_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasobj_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasimage_class_init(GtkgEvasImageClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gevasobj_get_type());

	object_class->destroy = gevasimage_destroy;

	object_class->get_arg = gevasimage_get_arg;
	object_class->set_arg = gevasimage_set_arg;

	klass->set_image_fill = gevasimage_set_image_fill;
	klass->get_image_size = gevasimage_get_image_size;
	klass->set_image_border = gevasimage_set_image_border;
	klass->get_image_border = gevasimage_get_image_border;

	gtk_object_add_arg_type(GTK_GEVASIMAGE_IMAGENAME,
							GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_IMAGENAME);

}

static void gevasimage_init(GtkgEvasImage * ev)
{
    ev->image_filename =0;
    
}

GtkgEvasImage *gevasimage_new(void)
{
	GtkgEvasImage *ev;

	ev = gtk_type_new(gevasimage_get_type());

	return GTK_GEVASIMAGE(ev);
}

/* GtkObject functions */


static void gevasimage_destroy(GtkObject * object)
{
	GtkgEvasImage *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASIMAGE(object));

	ev = GTK_GEVASIMAGE(object);

    if(ev->image_filename) g_free(ev->image_filename);
    

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void gevasimage_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasImage *ev;
	gchar *gstr;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASIMAGE(object));

	ev = GTK_GEVASIMAGE(object);

	switch (arg_id) {
		case ARG_IMAGENAME:
			{
				Evas e;
				Evas_Object o;

				gstr = GTK_VALUE_STRING(*arg);
				_gevasobj_ensure_obj_free(object);
				e = _gevas_evas(object);
				o = evas_add_image_from_file(EVAS(ev), gstr);
				_gevas_set_obj(object, o);

                ev->image_filename = g_strdup(gstr);
            }
			break;


		default:
			break;
	}
}

static void gevasimage_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasImage *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASIMAGE(object));

	ev = GTK_GEVASIMAGE(object);

	switch (arg_id) {
/*    case ARG_GEVAS:
		GTK_VALUE_POINTER (*arg) = ev->gevas;
		break;
*/

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
