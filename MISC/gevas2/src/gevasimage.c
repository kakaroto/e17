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

#include <gevasimage.h>
#include <gevas_util.h>


#ifdef BUILD_EDB_CODE
#include <Edb.h>
#endif

#include <stdio.h>
#include <string.h>

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


static void set_image_fill(GtkgEvasObj * object,
                           Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
	evas_object_image_fill_set( EVASO(object), x, y, w, h );
}

static void get_image_fill(GtkgEvasObj * object,
                           Evas_Coord* x, Evas_Coord* y,
                          Evas_Coord* w, Evas_Coord* h)
{
	evas_object_image_fill_get( EVASO(object), x, y, w, h );
}


static Evas_Coord get_image_fill_width( GtkgEvasObj * object )
{
    Evas_Coord w,h,x,y;
    gevasimage_get_image_fill( object, &x, &y, &w, &h );
    return w;
}

static Evas_Coord get_image_fill_height(GtkgEvasObj * object )
{
    Evas_Coord w,h,x,y;
    gevasimage_get_image_fill( object, &x, &y, &w, &h );
    return h;
}
    
#define VTAB ((GtkgEvasImageClass*)GTK_OBJECT_GET_CLASS(object))
void
gevasimage_set_image_fill(GtkgEvasObj * object, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
    return VTAB->set_image_fill( object, x, y, w, h );
}
void
gevasimage_get_image_fill(GtkgEvasObj * object, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
    return VTAB->get_image_fill( object, x, y, w, h );
}
Evas_Coord
gevasimage_get_image_fill_width( GtkgEvasObj * object )
{
    return VTAB->get_image_fill_width( object );
}
Evas_Coord
gevasimage_get_image_fill_height(GtkgEvasObj * object )
{
    return VTAB->get_image_fill_height( object );
}






void
gevasimage_set_image_border(GtkgEvasObj * object, int l, int r, int t, int b)
{
	evas_object_image_border_set( EVASO(object), l, r, t, b);
}

void gevasimage_get_image_size(GtkgEvasObj * object, int *w, int *h)
{
	evas_object_image_size_get( EVASO(object), w, h );
}

void gevasimage_set_image_size(  GtkgEvasObj * object, int  w, int  h)
{
	evas_object_image_size_set( EVASO(object), w, h );
}


void
gevasimage_set_smooth_scale( GtkgEvasObj* object, int v )
{
    evas_object_image_smooth_scale_set(EVASO(object), v);
}

int
gevasimage_get_smooth_scale( GtkgEvasObj* object )
{
    return evas_object_image_smooth_scale_get(EVASO(object));
}


void
gevasimage_get_image_border(GtkgEvasObj * object, int *l, int *r, int *t,
							int *b)
{
	evas_object_image_border_get( EVASO(object), l, r, t, b );
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
			(GtkClassInitFunc)  gevasimage_class_init,
			(GtkObjectInitFunc) gevasimage_init,
			/* reserved_1 */   NULL,
			/* reserved_2 */   NULL,
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

	klass->set_image_fill = set_image_fill;
	klass->get_image_fill = get_image_fill;
    klass->get_image_fill_width  = get_image_fill_width;
	klass->get_image_fill_height = get_image_fill_height;
    
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

//    fprintf(stderr,"   gevasimage_set_arg() \n");
    

	ev = GTK_GEVASIMAGE(object);

	switch (arg_id) {
		case ARG_IMAGENAME:
            if( GTK_GEVASOBJ(ev)->eobj )
            {
				Evas_Object* o = GTK_GEVASOBJ(ev)->eobj;

				gstr = GTK_VALUE_STRING(*arg);
                g_free(ev->image_filename);
                ev->image_filename = g_strdup(gstr);
                
                evas_object_image_file_set( o, ev->image_filename, NULL);
                {
                    int w=0, h=0;

                    evas_object_image_size_get( o, &w, &h );
                    evas_object_image_fill_set( o, 0, 0, w, h );
                    evas_object_resize( o, w, h );
                }
/*                 fprintf(stderr,"ARG_IMAGENAME(s): %s gevas:%p\n", */
/*                         ev->image_filename, */
/*                         GTK_GEVASOBJ(ev)->gevas ); */
            }
            else
            {
				Evas_Object* o;

				gstr = GTK_VALUE_STRING(*arg);

/*                 // Trim off leading double slash */
/*                 while( *gstr && *(gstr+1) && *gstr=='/' && *(gstr+1)=='/') */
/*                     gstr++; */

//                    fprintf(stderr,"ARG_IMAGENAME(1): %s %lx\n",gstr, EVAS(ev) );
                
				_gevasobj_ensure_obj_free(object);

                g_free(ev->image_filename);
                ev->image_filename = g_strdup(gstr);

/* 				o = evas_add_image_from_file(EVAS(ev), ev->image_filename); */
                o = evas_object_image_add( EVAS(ev) );
                evas_object_image_file_set( o, ev->image_filename, NULL);
                
				_gevas_set_obj(object, o);
//                fprintf(stderr,"ARG_IMAGENAME(e): %s %lx\n",gstr, EVAS(ev) );
                
                {
                    int w=0, h=0;

                    evas_object_image_size_get( o, &w, &h );
                    evas_object_image_fill_set( o, 0, 0, w, h );
                    evas_object_resize( o, w, h );
                }
//                fprintf(stderr,"ARG_IMAGENAME(e2): %s %lx\n",gstr, EVAS(ev) );
            }
            gevasobj_queue_redraw( GTK_GEVASOBJ( object ) );
			break;


		default:
			break;
	}
}

static void __gevasimage_load_from_rgba32data( GtkgEvasImage* object,
                                               guint32* rgbadata,
                                               int w, int h,
                                               int copydata )
{
    Evas_Coord x=0;
    Evas_Coord y=0;
    int layer = 0;
/*     fprintf( stderr, "gevasimage_load_from_rgba32data(1) x:%f y:%f w:%d h:%d layer:%d\n", */
/*              x, y, w, h, layer ); */
    GtkgEvasImage *ev;
    Evas_Object* eo;
    g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASIMAGE(object));
    ev = GTK_GEVASIMAGE(object);
    
    {
        Evas_Object* eo = GTK_GEVASOBJ(ev)->eobj;
        if( eo )
        {
            gevasobj_get_location( GTK_GEVASOBJ(object), &x, &y );
            layer = gevasobj_get_layer( GTK_GEVASOBJ(object) );
        }
    }
    
    _gevasobj_ensure_obj_free( GTK_OBJECT(ev) );
    g_free(ev->image_filename);
    ev->image_filename = g_strdup("<na>");
    
    eo = evas_object_image_add( EVAS(ev) );
    _gevas_set_obj( GTK_OBJECT(ev), eo);

//    gevasobj_resize( GTK_GEVASOBJ(ev), w, h );
    evas_object_resize( eo, w, h );
    evas_object_image_size_set( eo, w, h );
    if( copydata )
        evas_object_image_data_copy_set( eo, (int*)(rgbadata) );
    else
        evas_object_image_data_set( eo, (int*)(rgbadata) );
    
    evas_object_image_fill_set( eo, 0, 0, w, h );

/*     fprintf( stderr, "gevasimage_load_from_rgba32data(2) x:%f y:%f w:%d h:%d layer:%d\n", */
/*              x, y, w, h, layer ); */
    
    gevasobj_set_location( GTK_GEVASOBJ(object), x, y );
    gevasobj_set_layer( GTK_GEVASOBJ(object), layer );
    gevasobj_queue_redraw( GTK_GEVASOBJ( object ) );
}

void gevasimage_load_from_rgba32data( GtkgEvasImage* object,
                                      guint32* rgbadata,
                                      int w, int h )
{
    __gevasimage_load_from_rgba32data( object, rgbadata, w, h, 0 );
}

void gevasimage_load_copy_from_rgba32data( GtkgEvasImage* object,
                                           guint32* rgbadata,
                                           int w, int h )
{
    __gevasimage_load_from_rgba32data( object, rgbadata, w, h, 1 );
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
        gevasobj_move( GTK_GEVASOBJ(ev), x, y );

    x = url_args_lookup_int(hash_args, "rx", 0 );
    y = url_args_lookup_int(hash_args, "ry", 0 );
    if( x && y )
        gevasobj_move_relative( GTK_GEVASOBJ(ev), x, y );

    n = url_args_lookup_int(hash_args, "layer", 0 );
    if(n)
        gevasobj_set_layer( GTK_GEVASOBJ(ev), n );

    x = url_args_lookup_int(hash_args, "resize_x", 0 );
    y = url_args_lookup_int(hash_args, "resize_y", 0 );
    if( x && y )
        gevasobj_resize( GTK_GEVASOBJ(ev), x, y );

    n = url_args_lookup_int(hash_args, "fill_size", 0 );
    if(n)
    {
        int w,h;
        gevasimage_get_image_size(GTK_GEVASOBJ(ev), &w, &h);
        gevasimage_set_image_fill(GTK_GEVASOBJ(ev), 0,0,w,h);
    }
    

    
    n = url_args_lookup_int(hash_args, "visible", 0 );
    if(n)
        gevasobj_set_visible( GTK_GEVASOBJ(ev), n );

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
    const char* filen;
    char* p = 0;
    GHashTable* hash_args = 0;
/*     E_DB_File* edb = 0; */
    
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
                                   "/", ev->metadata_load_postfix, NULL);
    }
    else
    {
        full_buffer = g_strdup(ev->metadata_load_postfix);
    }



    filen = p = url_file_name_part_new( full_buffer );
    hash_args = url_args_to_hash( full_buffer );
    filen = gevas_trim_prefix("file:",filen);
    
/*     printf("image load_from_metadata() filen1      :%s\n",filen); */
/*     printf("image load_from_metadata() full_buffer :%s\n",full_buffer); */

    if( strstr(filen,":") || gevas_file_exists("%s",filen))
    {
/*         printf("image load_from_metadata() filen1 EXISTS!     :%s\n",filen); */
        gevasimage_set_image_name(ev, filen);
        ev->metadata_load_loaded = 1;
        setup_attribs( ev, hash_args );
    }

    if(ev->metadata_load_hash)
    {
        setup_attribs( ev, ev->metadata_load_hash);
    }
    
    hash_str_str_clean( hash_args );
    g_free(full_buffer);
    g_free(p);

}


gboolean
gevasimage_load_from_metadata( GtkgEvasObj * object, const char* loc )
{
	GtkgEvasImage *ev;
    char* no_prefix = "";
    GHashTable* h=0;

    g_return_val_if_fail(object != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASIMAGE(object),0);
	g_return_val_if_fail(GTK_IS_GEVAS(gevasobj_get_gevas(GTK_OBJECT(object))),0);
    
    ev = GTK_GEVASIMAGE(object);

    ev->metadata_load_hash = 0;
    
#ifdef BUILD_EDB_CODE
    if( strstr( loc, "#edb" ))
    {
        const char* p = 0;
/*         printf("gevasimage_load_from_metadata() edb in URL... old loc:%s\n",loc); */

        h = url_args_to_hash( loc );
        p = url_args_lookup_str( h, "prefix", "prefix" );
        ev->metadata_load_hash = h;

/*         printf("gevasimage_load_from_metadata() edb in URL... p:%s\n",p); */


        loc = gevas_metadata_lookup_string(
            gevasobj_get_gevas(GTK_OBJECT(ev)),
            loc, "", "%s", p, 0);

/*         printf("gevasimage_load_from_metadata() edb in URL... NEW loc:%s\n",loc); */
    }
#endif
    
    ev->metadata_load_loaded = 0;
    ev->metadata_load_postfix = loc;

    load_from_metadata((gpointer)no_prefix,ev);
    
    if( gevas_get_image_prefix_list(gevasobj_get_gevas(GTK_OBJECT(ev))))
    {
        g_list_foreach( gevas_get_image_prefix_list(gevasobj_get_gevas(GTK_OBJECT(ev))),
                        load_from_metadata, ev);
    }
    
    if(h) hash_str_str_clean(h);
    
    return ev->metadata_load_loaded;
}

GtkgEvasImage *gevasimage_new_from_metadata( GtkgEvas* gevas, const char* loc )
{
	GtkgEvasImage* o = gevasimage_new();
    gboolean b = 1;

    if(!loc || !strlen(loc))
        return 0;
    
    gevasobj_set_gevas(o, gevas);
    b = gevasimage_load_from_metadata( GTK_GEVASOBJ(o), loc );
    if(!b)
    {
        gtk_object_unref( GTK_OBJECT(o) );
        return 0;
    }

    g_return_val_if_fail(o != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVASIMAGE(o),0);
	g_return_val_if_fail(GTK_IS_GEVAS(gevasobj_get_gevas((GtkObject*)o)),0);
    return o;
}

void gevasimage_ensure_smallerthan_with_ratio( GtkgEvasImage* gi, int MaxDesiredWidthOrHeight )
{
    int w=0, h=0;
    double ratio = MaxDesiredWidthOrHeight;
    GtkgEvasObj* go = GTK_GEVASOBJ( gi );

    gevasimage_get_image_size(go, &w, &h); 
    if( w && h )
    {
        if( w > MaxDesiredWidthOrHeight )
        {
            ratio /= w;
            w = MaxDesiredWidthOrHeight;
            h = (int)( ratio * h );
        }
        else if( h > MaxDesiredWidthOrHeight )
        {
            ratio /= h;
            w = (int)( ratio * w );
            h = MaxDesiredWidthOrHeight;
        }
        
        gevasobj_resize( go, w, h );
        gevasimage_set_image_fill( go, 0, 0, w, h );
    }
}
