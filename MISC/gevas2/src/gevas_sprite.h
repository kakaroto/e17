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

#ifndef INC_GTK_GEVAS_SPRITE__H
#define INC_GTK_GEVAS_SPRITE__H

#include <gevasev_handler.h>
#include <gevas.h>
#include <gevasobj.h>
#include <gevasimage.h>

#include <gtk/gtkobject.h>
#include <gtk/gtk.h>

#include "gevastrans_alphawipe.h"
#include "gevas_obj_collection.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */

    
    
#define GTK_GEVAS_SPRITE(obj) GTK_CHECK_CAST \
        (obj, gevas_sprite_get_type (), GtkgEvasSprite)
    
#define GTK_GEVAS_SPRITE_CLASS(klass)  GTK_CHECK_CLASS_CAST \
        (klass, gevas_sprite_get_type (), GtkgEvasSpriteClass)
        
#define GTK_IS_GEVAS_SPRITE(obj)       GTK_CHECK_TYPE \
        (obj, gevas_sprite_get_type ())

    
    typedef struct _GtkgEvasSprite      GtkgEvasSprite;
    typedef struct _GtkgEvasSpriteClass GtkgEvasSpriteClass;
    typedef GtkgEvasObj*                GtkgEvasSprite_T;

    struct _GtkgEvasSprite
    {
        GtkgEvasImage gobj;

        GtkgEvasObjCollection* col;

        // # frames to play, -1 == play forever
        gint frames_to_play; 
        gint current_frame;
        gboolean playing_backwards;

        // ID of the timer so that clear() can cancel the animation 
        guint m_timerID;

        // These are both temp data used in gevas_sprite_load_from_metadata //
        gboolean       metadata_load_loaded;
        const char *   metadata_load_postfix;

        // Frame rate time delays
        gint     default_frame_delay_ms; // default
        gint     frame_delay_ms_base;    // starting location for frame_delay_ms
        gint     frame_delay_ms_size;    // count(frame_delay_ms) from 0
        GArray*  frame_delay_ms;         // delay between frames from frame_delay_ms_base
                                         // elements are sizeof(gint) in frame_delay_ms 

        gint     frame_trans_f_size;     // count(frame_trans_f) from 0
        GArray*  frame_trans_f;          // transition function to use between frames
    };

    struct _GtkgEvasSpriteClass
    {
		GtkgEvasImageClass parent_class;
    };


    guint           gevas_sprite_get_type(void);
    GtkgEvasSprite* gevas_sprite_new( GtkgEvas* _gevas );
    GtkgEvasSprite* gevas_sprite_new_from_metadata( GtkgEvas* gevas, const char* loc );



/* public */

    void gevas_sprite_set_default_frame_delay( GtkgEvasSprite* ev, gint ms );
    void gevas_sprite_set_inter_frame_delays(
        GtkgEvasSprite* ev,
        gint base,
        GArray* times,
        gint    times_size);
    
    void gevas_sprite_add(           GtkgEvasSprite* ev, GtkgEvasSprite_T o );
    void gevas_sprite_add_all(       GtkgEvasSprite* ev, GtkgEvasSprite*s);
    void gevas_sprite_remove(        GtkgEvasSprite* ev, GtkgEvasSprite_T o );
    void gevas_sprite_remove_all(    GtkgEvasSprite* ev, GtkgEvasSprite*s);
    void gevas_sprite_clear(         GtkgEvasSprite* ev );
    void gevas_sprite_move(          GtkgEvasSprite* ev, gint32 x, gint32 y );
    void gevas_sprite_move_relative( GtkgEvasSprite* ev, gint32 dx, gint32 dy );
    gint gevas_sprite_get_size(      GtkgEvasSprite* ev );
    void gevas_sprite_hide(          GtkgEvasSprite* ev );
    void gevas_sprite_show(          GtkgEvasSprite* ev );
    void gevas_sprite_set_visible(   GtkgEvasSprite* ev, gboolean v );
    gboolean gevas_sprite_contains(  GtkgEvasSprite* ev, GtkgEvasSprite_T o );
    gboolean gevas_sprite_contains_all( GtkgEvasSprite* ev, GtkgEvasSprite*s);

    gboolean gevas_sprite_load_from_metadata( GtkgEvasSprite* ev, const char* loc );

    gint gevas_sprite_get_current_frame( GtkgEvasSprite* ev );
    void gevas_sprite_set_current_frame( GtkgEvasSprite* ev, gint f );
    gboolean gevas_sprite_set_current_frame_by_name( GtkgEvasSprite* ev,const char*n);
    gboolean gevas_sprite_set_current_frame_by_namei( GtkgEvasSprite* ev,const char*n);
    void gevas_sprite_advance_n( GtkgEvasSprite* ev, gint n );
    void gevas_sprite_retard_n ( GtkgEvasSprite* ev, gint n );
    void gevas_sprite_jumpto_start( GtkgEvasSprite* ev );
    void gevas_sprite_jumpto_end  ( GtkgEvasSprite* ev );

//    void gevas_sprite_play( GtkgEvasSprite* ev );
    void gevas_sprite_play_n( GtkgEvasSprite* ev, gint n );
    void gevas_sprite_play_to_end( GtkgEvasSprite* ev );
    void gevas_sprite_play_one_loop( GtkgEvasSprite* ev );
    void gevas_sprite_play_forever( GtkgEvasSprite* ev );

    void gevas_sprite_set_play_backwards( GtkgEvasSprite* ev, gboolean v );

    void gevas_sprite_set_transition_function( GtkgEvasSprite* ev,
                                               int framenum,
                                               geTransAlphaWipe* trans );
    
/* package */


    /************************************************************/
    /************************************************************/
    /************************************************************/

    /* Much better to move to using edje to do many timed events */
    
/*     struct _GevasSpriteGroupTimer */
/*     { */
/*         gint   default_frame_delay_ms; // default */
/*         guint  m_timerID; */
/*         GList* m_clients; */
/*     }; */
/*     typedef struct _GevasSpriteGroupTimer GevasSpriteGroupTimer; */
    
/*     GevasSpriteGroupTimer* */
/*     new_gevas_group_timer( gint frame_delay ); */

/*     void */
/*     free_gevas_group_timer( GevasSpriteGroupTimer* gt ); */
    
/*     void gevas_group_timer_add_client( GevasSpriteGroupTimer* gt, */
/*                                        GtkFunction fn, gpointer data ); */
    
/*     void gevas_sprite_attach_to_group_timer( GtkgEvasSprite* ev, GevasSpriteGroupTimer* gt ); */
    


#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/*  */
