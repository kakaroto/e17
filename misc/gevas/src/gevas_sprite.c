/*
 * Playable sprite.
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

#include "config.h"
#include <gevasobj.h>
#include <gevas_obj_collection.h>
#include <gevas_sprite.h>

#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>

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



static void gevas_sprite_class_init(GtkgEvasSpriteClass * klass);
static void gevas_sprite_init(GtkgEvasSprite* ev);
/* GtkObject functions */
static void gevas_sprite_destroy(GtkObject * object);
static void gevas_sprite_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void gevas_sprite_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

enum {
	ARG_0,				/* Skip 0, an invalid argument ID */
	ARG_DUMMY,
};


enum {
    SIG_LAST
};

static guint signals[SIG_LAST] = { 0 };




void gevas_sprite_add(           GtkgEvasSprite* ev, GtkgEvasSprite_T o )
{
	g_return_if_fail(ev != NULL);
	g_return_if_fail(o  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVASOBJ(o));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));
    
    gevas_obj_collection_add( ev->col, o );
}

void gevas_sprite_add_all(       GtkgEvasSprite* ev, GtkgEvasSprite*s)
{
	g_return_if_fail(ev != NULL);
	g_return_if_fail(s  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(s));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(s ->col));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_add_all( ev->col, s->col );
}

void gevas_sprite_remove(        GtkgEvasSprite* ev, GtkgEvasSprite_T o )
{
	g_return_if_fail(ev != NULL);
	g_return_if_fail(o  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVASOBJ(o));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_remove( ev->col, o );
}

void gevas_sprite_remove_all(    GtkgEvasSprite* ev, GtkgEvasSprite*s)
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(s  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(s));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(s ->col));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_remove_all( ev->col, s->col );
}

void gevas_sprite_clear(         GtkgEvasSprite* ev )
{
  	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_clear( ev->col );
}

void gevas_sprite_move(          GtkgEvasSprite* ev, gint32 x, gint32 y )
{
  	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_move( ev->col, x, y );
}

void gevas_sprite_move_relative( GtkgEvasSprite* ev, gint32 dx, gint32 dy )
{
  	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_move_relative( ev->col, dx, dy );
}

void gevas_sprite_hide(          GtkgEvasSprite* ev )
{
    gevas_sprite_set_visible( ev, 0 );
}

void gevas_sprite_show(          GtkgEvasSprite* ev )
{
    gevas_sprite_set_visible( ev, 1 );
}

void gevas_sprite_set_visible(   GtkgEvasSprite* ev, gboolean v )
{
  	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_obj_collection_set_visible( ev->col, v );
}

gboolean gevas_sprite_contains(  GtkgEvasSprite* ev, GtkgEvasSprite_T o )
{
	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(o  != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVAS_SPRITE(ev),0);
	g_return_val_if_fail(GTK_IS_GEVASOBJ(o),0);
	g_return_val_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col),0);
    
    return gevas_obj_collection_contains( ev->col, o );
}

gboolean gevas_sprite_contains_all( GtkgEvasSprite* ev, GtkgEvasSprite*s)
{
	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(s  != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVAS_SPRITE(ev),0);
	g_return_val_if_fail(GTK_IS_GEVAS_SPRITE(s) ,0);
	g_return_val_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(s ->col),0);
	g_return_val_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col),0);

    return gevas_obj_collection_contains_all( ev->col,
                                              GTK_GEVAS_OBJ_COLLECTION(s));
}


void gevas_sprite_push_metadata_prefix( GtkgEvasSprite* ev, const char* p )
{
	g_return_if_fail(ev != NULL);
	g_return_if_fail(p  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));

    if(!strlen(p))
        return;

    ev->metadata_prefix_list = g_list_append(ev->metadata_prefix_list,
                                             g_strdup(p));
    
}

void gevas_sprite_pop_metadata_prefix ( GtkgEvasSprite* ev, const char* p )
{
    GList *li = 0;
    
    g_return_if_fail(ev != NULL);
	g_return_if_fail(p  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));

    if(!strlen(p))
        return;

    if(li = g_list_find(ev->metadata_prefix_list, p))
    {
        g_free(li->data);
        ev->metadata_prefix_list =
            g_list_remove_link(ev->metadata_prefix_list, li);
    }
}

gboolean gevas_sprite_load_from_metadata( GtkgEvasSprite* ev, const char* loc )
{
	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(loc!= NULL,0);
	g_return_val_if_fail(GTK_IS_GEVAS_SPRITE(ev),0);


    
    // FIXME:
}

gint
gevas_sprite_get_size( GtkgEvasSprite* ev )
{
	g_return_val_if_fail(ev != NULL,0);
	g_return_val_if_fail(GTK_IS_GEVAS_SPRITE(ev),0);
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    return gevas_obj_collection_get_size( ev->col );
}



// Check to see that 0 < f < size( ev->col )
static gboolean
within_range( GtkgEvasSprite* ev, gint f )
{
    gint max = gevas_sprite_get_size( ev );
    
    return 0 < f && f <= max;
}

gint gevas_sprite_get_current_frame( GtkgEvasSprite* ev )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));
    return ev->current_frame;
}


void gevas_sprite_set_current_frame( GtkgEvasSprite* ev, gint f )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));
	g_return_if_fail(within_range(ev,f));

    ev->current_frame = f;
}


gboolean gevas_sprite_set_current_frame_by_name( GtkgEvasSprite* ev, const char* name )
{
    gint idx = -1;
    
	g_return_if_fail(ev != NULL);
	g_return_if_fail(n  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));

    idx = gevas_obj_collection_element_idx_from_name(ev->col,0,name );
    if( idx >= 0 )
    {
        ev->current_frame = idx;
        return 1;
    }
    return 0;
}

gboolean gevas_sprite_set_current_frame_by_namei( GtkgEvasSprite* ev, const char* name )
{
    gint idx = -1;
    
	g_return_if_fail(ev != NULL);
	g_return_if_fail(n  != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));

    idx = gevas_obj_collection_element_idx_from_namei(ev->col,0,name );
    if( idx >= 0 )
    {
        ev->current_frame = idx;
        return 1;
    }
    return 0;
}

void gevas_sprite_advance_n( GtkgEvasSprite* ev, gint n )
{
    gint f = 0;
    
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));
    f = n + gevas_sprite_get_current_frame(ev);
	g_return_if_fail(within_range(ev,f));

    gevas_sprite_set_current_frame(ev,f);
}

void gevas_sprite_retard_n ( GtkgEvasSprite* ev, gint n )
{
    gevas_sprite_advance_n( ev, -n );
}

void gevas_sprite_jumpto_start( GtkgEvasSprite* ev )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_sprite_set_current_frame(ev,0);
}

void gevas_sprite_jumpto_end  ( GtkgEvasSprite* ev )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    gevas_sprite_set_current_frame(ev,gevas_sprite_get_size(ev));
}


void gevas_sprite_play( GtkgEvasSprite* ev )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    ev->frames_to_play = -1;
}

void gevas_sprite_play_n( GtkgEvasSprite* ev, gint n )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    ev->frames_to_play = ABS(n);
}

void gevas_sprite_play_to_end( GtkgEvasSprite* ev )
{
    gint n = 0;
    
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    n = gevas_sprite_get_size(ev) - ev->current_frame;
    gevas_sprite_play_n( ev, n );
}

void gevas_sprite_play_one_loop( GtkgEvasSprite* ev )
{
    gint n = 0;
    
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    n = gevas_sprite_get_size(ev);
    gevas_sprite_play_n( ev, n );
}


void gevas_sprite_set_play_backwards( GtkgEvasSprite* ev, gboolean v )
{
 	g_return_if_fail(ev != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(ev));
	g_return_if_fail(GTK_IS_GEVAS_OBJ_COLLECTION(ev->col));

    ev->playing_backwards = v;
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


static GtkObjectClass *parent_class = NULL;

guint
gevas_sprite_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasSprite",
			sizeof(GtkgEvasSprite),
			sizeof(GtkgEvasSpriteClass),
			(GtkClassInitFunc)  gevas_sprite_class_init,
			(GtkObjectInitFunc) gevas_sprite_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}

static void
gevas_sprite_class_init(GtkgEvasSpriteClass * klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());

	object_class->destroy = gevas_sprite_destroy;
    object_class->get_arg = gevas_sprite_get_arg;
	object_class->set_arg = gevas_sprite_set_arg;

/*    signals[SIG_ADD] =
        gtk_signal_new ("add", GTK_RUN_LAST, object_class->type, 0,
                        gtk_marshal_NONE__POINTER,
                        GTK_TYPE_NONE, 1,
                        GTK_TYPE_OBJECT);

    gtk_object_class_add_signals (object_class, signals, SIG_LAST);
*/
    

}

static void
gevas_sprite_init(GtkgEvasSprite * ev)
{
    ev->gevas             = 0;
    ev->col               = 0;
    ev->playing_backwards = 0;
    ev->metadata_prefix_list = 0;
}



GtkgEvasSprite*
gevas_sprite_new(GtkgEvas* _gevas)
{
	GtkgEvasSprite *ev;

	ev = gtk_type_new(gevas_sprite_get_type());
    
    ev->gevas = _gevas;
    ev->col   = gevas_obj_collection_new(ev->gevas);

    return GTK_GEVAS_SPRITE(ev);
}

/* GtkObject functions */


static void
gevas_sprite_destroy(GtkObject * object)
{
	GtkgEvasSprite *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(object));
    ev = GTK_GEVAS_SPRITE(object);

    

	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}


static void
gevas_sprite_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasSprite *ev;
	gchar *gstr;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(object));
    ev = GTK_GEVAS_SPRITE(object);

	switch (arg_id)
    {
        
    case ARG_DUMMY:
        break;

    default:
        break;
        
	}
}

static void
gevas_sprite_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasSprite *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVAS_SPRITE(object));
    ev = GTK_GEVAS_SPRITE(object);

	switch (arg_id)
    {
        
    default:
        arg->type = GTK_TYPE_INVALID;
        break;
        
	}
}

