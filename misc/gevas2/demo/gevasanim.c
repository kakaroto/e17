/******************************************************************************
*******************************************************************************
*******************************************************************************

    A test for gevas, uses the sprite code.
 
    Copyright (C) 2001 Ben Martin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    For more details see the COPYING file in the root directory of this
    distribution.


    $Id$

*******************************************************************************
*******************************************************************************
******************************************************************************/



#include "config.h"

#include <gevas.h>
#include <gevasimage.h>
#include <gevastext.h>
#include <gevasevh_alpha.h>
#include <gevasevh_drag.h>
#include <gevasevh_to_gtk_signals.h>
#include <gevasevh_emouse_over.h>
#include <gevasevh_popup.h>
#include <gevasevh_obj_changer.h>
#include <gevasevh_clicks.h>
#include <gevasevh_group_selector.h>
#include <gevasevh_selectable.h>
#include <gevastwin.h>
#include <gevasgrad.h>
#include <gevas_sprite.h>

#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

GtkWidget *wtoy=0;
GtkWidget *gevas=0;
GtkObject *evh_selector = 0;

#define DEFAULT_FONT_NAME  "verdana.ttf"
#define DEFAULT_FONT_SIZE  18

static gint delete_event_cb(GtkWidget * window, GdkEventAny * e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

void makeFastAnim()
{
    GtkgEvasSprite* sprite = 0;
    GtkgEvasImage*  gi     = 0;
    int i = 0;

    sprite = gevas_sprite_new( GTK_GEVAS(gevas) );

    for( i=1; i<7; ++i )
    {
        gchar* md = g_strdup_printf( "cell%d.png?x=270&y=120&visible=0&fill_size=1", i );
        
        gi = gevasimage_new_from_metadata( GTK_GEVAS(gevas), md );
        gevas_sprite_add( sprite, GTK_GEVASOBJ( gi ) );

        g_free( md );
    }
    gevas_sprite_set_default_frame_delay( sprite, 30 );
    gevas_sprite_play_forever( sprite );
}

void makeSelectable( GtkgEvasObj* object )
{
	GtkgEvasObj *ct;
	GtkObject *evh = gevasevh_selectable_new( evh_selector );
    gevasevh_selectable_set_confine( GTK_GEVASEVH_SELECTABLE(evh), 1 );

	gevasobj_add_evhandler(object, evh);
	gevasevh_selectable_set_normal_gevasobj(GTK_GEVASEVH_SELECTABLE(evh), object);

	ct = (GtkgEvasObj*)gevasgrad_new(gevasobj_get_gevas(GTK_OBJECT(object)));

    evas_object_color_set(gevasobj_get_evasobj(GTK_OBJECT(ct)), 255, 200, 255, 200);
    gevasgrad_add_color(ct, 120, 150, 170, 45, 8);
	gevasgrad_add_color(ct, 200, 170, 90, 150, 16);
    gevasgrad_set_angle(ct, 150);
	gevasobj_resize( ct, 200,100);
	gevasobj_set_layer(ct, 9999);
    gevasevh_selectable_set_selected_gevasobj( evh, ct );
}


int main(int argc, char *argv[])
{
    GtkgEvasObj* label;
    GtkgEvasTwin* twin;
    geTransAlphaWipe* trans;
	GtkgEvasImage*  bg     = 0;
    GtkgEvasSprite* sprite = 0;
    GtkgEvasImage*  gi     = 0;
    GtkgEvasObj*    go     = 0;
	GtkWidget *window;
    int i = 0;
    
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window),"delete_event", GTK_SIGNAL_FUNC(delete_event_cb), NULL);
    
    gevas_new_gtkscrolledwindow( (GtkgEvas**)(&gevas), &wtoy );
    gtk_container_add(GTK_CONTAINER(window), wtoy);
    gtk_widget_set_usize(gevas, 3000, 3000);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    
//	gevas_set_render_mode( gevas, RENDER_METHOD_3D_HARDWARE ); 
//  gevas_set_render_mode( gevas, RENDER_METHOD_ALPHA_SOFTWARE );


    gtk_window_set_title(GTK_WINDOW(window), "gevas does sprites");
    gtk_widget_show_all(window);

    gevas_add_metadata_prefix( GTK_GEVAS(gevas), PACKAGE_DATA_DIR );
    gevas_add_image_prefix   ( GTK_GEVAS(gevas), PACKAGE_DATA_DIR );
    gevas_add_fontpath       ( GTK_GEVAS(gevas), PACKAGE_DATA_DIR );
    gevas_add_metadata_prefix( GTK_GEVAS(gevas), "./" );
    gevas_add_image_prefix   ( GTK_GEVAS(gevas), "./" );
    gevas_add_fontpath       ( GTK_GEVAS(gevas), "./" );
    
    fprintf( stderr, "Creating the background\n");

    bg = gevasimage_new_from_metadata(
        GTK_GEVAS(gevas), "bg.png?"
        "x=0&y=0&visible=1&fill_size=1&layer=-9999&resize_x=9999&resize_y=9999" );

    /* make a group selector object */
    fprintf( stderr, "Making the background the group selector\n");
	evh_selector = gevasevh_group_selector_new();
    gevasevh_group_selector_set_object( (GtkgEvasEvHGroupSelector*)evh_selector, GTK_GEVASOBJ(bg));
    gevasobj_add_evhandler(GTK_GEVASOBJ(bg), evh_selector);
    

//    makeFastAnim();

    fprintf( stderr, "Creating a sprite\n");

    sprite = gevas_sprite_new( GTK_GEVAS(gevas) );

    fprintf( stderr, "Adding the frames\n");
    for( i=1; i<7; ++i )
    {
        gchar* md = g_strdup_printf( "cell%d.png?x=120&y=120&visible=0&fill_size=1", i );
        
        gi = gevasimage_new_from_metadata( GTK_GEVAS(gevas), md );
        gevas_sprite_add( sprite, GTK_GEVASOBJ( gi ) );

        g_free( md );
    }
    fprintf( stderr, "Setting its delays\n");
    gevas_sprite_set_default_frame_delay( sprite, 2000 );
    gevas_sprite_play_forever( sprite );

    /* frame transitions */
    fprintf( stderr, "Setting its frame transitions\n");
    {
        geTransAlphaWipe* trans = 0;
        trans = gevastrans_alphawipe_new();
        for( i=0; i<7; ++i )
            gevas_sprite_set_transition_function( sprite, i, trans );
    }

    fprintf( stderr, "Make the sprite selectable\n");
    makeSelectable( (GtkgEvasObj*)sprite );


/*     gi = gevasimage_new_from_metadata( gevas, "cell4.png?x=220&y=290&visible=0&fill_size=1" ); */
/*     gevasobj_show( GTK_GEVASOBJ( gi )); */
/*     makeSelectable( gi ); */




    sprite = gevas_sprite_new( GTK_GEVAS(gevas) );
    for( i=1; i<7; ++i )
    {
        gchar* md = g_strdup_printf( "cell%d.png?x=260&y=120&visible=0&fill_size=1", i );
        
        gi = gevasimage_new_from_metadata( GTK_GEVAS(gevas), md );
        gevas_sprite_add( sprite, GTK_GEVASOBJ( gi ) );

        g_free( md );
    }
    gevas_sprite_set_default_frame_delay( sprite, 2500 );
    gevas_sprite_play_forever( sprite );
    /* frame transitions */
    trans = gevastrans_alphawipe_new();
    {
        for( i=0; i<7; ++i )
            gevas_sprite_set_transition_function( sprite, i, trans );
    }

    gevas_add_fontpath( GTK_GEVAS(gevas), "/usr/X11R6/lib/X11/fonts/msttcorefonts" );
    twin = gevastwin_new();
    gevastwin_set_main_obj(twin, sprite);
    label = go = (GtkgEvasObj *) gevastext_new(GTK_GEVAS(gevas));
    gevastext_set_font(go, DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE );
    gevastext_set_string(go, "test label");
    gevasobj_set_layer(go, 8);
    gevasobj_show(go);
    gevasobj_move(go, 260, 180);
    gevasobj_set_color(go, 255, 255, 255, 255);
//    gevasobj_add_evhandler(go, m_evhDrag);
    gevastwin_set_aux_obj( twin, label);
//    makeSelectable( twin );
    makeSelectable( (GtkgEvasObj*)sprite );
    gevasevh_throb_new_for_twin( twin, GTK_GEVASOBJ(sprite) );

    
    fprintf( stderr, "Enter main()\n");
    gtk_main();
    return 0;
}




