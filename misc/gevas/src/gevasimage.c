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

#include <gevasimage.h>
#include <gevas_util.h>


#include <Edb.h>


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

                // Trim off leading double slash
                while( *gstr && *(gstr+1) && *gstr=='/' && *(gstr+1)=='/')
                    gstr++;

                printf("ARG_IMAGENAME: %s\n",gstr);
                
				_gevasobj_ensure_obj_free(object);
				e = _gevas_evas(object);
                ev->image_filename = g_strdup(gstr);
				o = evas_add_image_from_file(EVAS(ev), ev->image_filename);
				_gevas_set_obj(object, o);

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


static void
setup_attribs( GtkgEvasImage* ev, GHashTable* hash_args )
{
    gint n=0;
    gint x=0;
    gint y=0;

    g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVASIMAGE(ev));

    x = url_args_lookup_int(hash_args, "x", 0 );
    y = url_args_lookup_int(hash_args, "y", 0 );
    if( x && y )
        gevasobj_move( ev, x, y );

    x = url_args_lookup_int(hash_args, "rx", 0 );
    y = url_args_lookup_int(hash_args, "ry", 0 );
    if( x && y )
        gevasobj_move_relative( ev, x, y );

    n = url_args_lookup_int(hash_args, "layer", 0 );
    if(n)
        gevasobj_set_layer( ev, n );

    x = url_args_lookup_int(hash_args, "resize_x", 0 );
    y = url_args_lookup_int(hash_args, "resize_y", 0 );
    if( x && y )
        gevasobj_resize( ev, x, y );

    n = url_args_lookup_int(hash_args, "fill_size", 0 );
    if(n)
    {
        int w,h;
        gevasimage_get_image_size(GTK_GEVASOBJ(ev), &w, &h);
        gevasimage_set_image_fill(GTK_GEVASOBJ(ev), 0,0,w,h);
    }
    

    
    n = url_args_lookup_int(hash_args, "visible", 0 );
    if(n)
        gevasobj_set_visible( ev, n );

}



static void
load_from_metadata(
    gpointer data,
    gpointer user_data
    )
{
    const char* fully_qualified_prefix = data;
    GtkgEvasImage* ev = user_data;
    char* full_buffer;
    char* filen;
    char* edb_prefix = 0;
    char* p = 0;
    GHashTable* hash_args = 0;
    E_DB_File* edb = 0;
    gboolean failed=1;
    
    g_return_if_fail(ev != NULL);
	g_return_if_fail(fully_qualified_prefix!= NULL);
    g_return_if_fail(GTK_IS_GEVASIMAGE(ev));

    if( ev->metadata_load_loaded )
    {
        return;
    }
    

    if( strlen( fully_qualified_prefix ))
    {
        full_buffer = g_strconcat( fully_qualified_prefix,
                                   "/", ev->metadata_load_postfix,0 );
    }
    else
    {
        full_buffer = g_strdup(ev->metadata_load_postfix);
    }



    filen = p = url_file_name_part_new( full_buffer );
    hash_args = url_args_to_hash( full_buffer );
    g_free(full_buffer);
    filen = gevas_trim_prefix("file:",filen);
    
    printf("image load_from_metadata() filen1      :%s\n",filen);

    gevasimage_set_image_name(ev, filen);
    ev->metadata_load_loaded = 1;
    setup_attribs( ev, hash_args );
    
    hash_str_str_clean( hash_args );
    g_free(p);

}


gboolean
gevasimage_load_from_metadata( GtkgEvasObj * object, const char* loc )
{
	GtkgEvasImage *ev;
    char* no_prefix = "";

    g_return_val_if_fail(object != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASIMAGE(object),0);
	g_return_val_if_fail(GTK_IS_GEVAS(gevasobj_get_gevas(GTK_OBJECT(object))),0);
    
    ev = GTK_GEVASIMAGE(object);

    ev->metadata_load_loaded = 0;
    ev->metadata_load_postfix = loc;

    load_from_metadata((gpointer)no_prefix,ev);
    
    if( gevas_get_image_prefix_list(gevasobj_get_gevas(GTK_OBJECT(ev))))
    {
        g_list_foreach( gevas_get_image_prefix_list(gevasobj_get_gevas(GTK_OBJECT(ev))),
                        load_from_metadata, ev);
    }
    
    return ev->metadata_load_loaded;
}

GtkgEvasImage *gevasimage_new_from_metadata( GtkgEvas* gevas, const char* loc )
{
	GtkgEvasImage* o = gevasimage_new();
    gboolean b = 1;

    if(!loc || !strlen(loc))
        return 0;
    
    gevasobj_set_gevas(o, gevas);
    b = gevasimage_load_from_metadata(o, loc );
    if(!b)
    {
        gtk_object_unref( GTK_OBJECT(o) );
        return 0;
    }
    return o;
}

