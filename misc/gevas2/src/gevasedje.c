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


/*
  FIXME: merge the load_from_metadata() with the foreach() stuff for edb prefixs in gevas.
*/

/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

#include "config.h"

#include <gevasedje.h>
#include <gevas_util.h>



#include <stdio.h>

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



enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_IMAGENAME
};

static void gevasedje_class_init(GtkgEvasEdjeClass * klass);
static void gevasedje_init(GtkgEvasEdje * ev);


/* GtkObject functions */
static void gevasedje_destroy(GtkObject * object);
static void gevasedje_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevasedje_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))




static GtkObjectClass *parent_class = NULL;

guint gevasedje_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEdje",
			sizeof(GtkgEvasEdje),
			sizeof(GtkgEvasEdjeClass),
			(GtkClassInitFunc)  gevasedje_class_init,
			(GtkObjectInitFunc) gevasedje_init,
			/* reserved_1 */   NULL,
			/* reserved_2 */   NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasobj_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasedje_class_init(GtkgEvasEdjeClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gevasobj_get_type());

	object_class->destroy = gevasedje_destroy;

	object_class->get_arg = gevasedje_get_arg;
	object_class->set_arg = gevasedje_set_arg;

	gtk_object_add_arg_type(GTK_GEVASEDJE_IMAGENAME,
							GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_IMAGENAME);

}

static void gevasedje_init(GtkgEvasEdje * ev)
{
    ev->m_edje = 0;
}

GtkgEvasEdje *
gevasedje_new(void)
{
	GtkgEvasEdje *ev;

	ev = gtk_type_new(gevasedje_get_type());
    ev->m_edje = 0;

	return GTK_GEVASEDJE(ev);
}

GtkgEvasEdje *
gevasedje_new_with_canvas( gpointer gevas )
{
	GtkgEvasEdje *ev;

	ev = gtk_type_new(gevasedje_get_type());
    gevasobj_set_gevas( ev, gevas );
    Evas* evas = gevas_get_evas(GTK_GEVAS(gevas));
    Evas_Object* edje = edje_object_add( evas );
    ev->m_edje = edje;
    _gevas_set_obj( GTK_OBJECT(ev), ev->m_edje );

    return GTK_GEVASEDJE(ev);
}

void gevasedje_set_file( GtkgEvasEdje* gedje, const char* filename, const char* part )
{
    edje_object_file_set(gedje->m_edje, filename, part );
}



/* GtkObject functions */


static void gevasedje_destroy(GtkObject * object)
{
	GtkgEvasEdje *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEDJE(object));

	ev = GTK_GEVASEDJE(object);

//    fprintf( stderr, "m_edje:%lx eobj:%lx \n", ev->m_edje, EVASO( ev ) );
    
    /* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void gevasedje_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEdje *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEDJE(object));

//    fprintf(stderr,"   gevasedje_set_arg() \n");
    

	ev = GTK_GEVASEDJE(object);

	switch (arg_id) {


		default:
			break;
	}
}

    


static void gevasedje_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEdje *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEDJE(object));

	ev = GTK_GEVASEDJE(object);

	switch (arg_id) {

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}


void gevasedje_set_edje( GtkgEvasEdje* ev, Evas_Object* e )
{
    ev->m_edje = e;
    _gevas_set_obj( GTK_OBJECT(ev), e );
}

Evas_Object* gevasedje_get_edje( GtkgEvasEdje* ev )
{
    return ev->m_edje;
}


