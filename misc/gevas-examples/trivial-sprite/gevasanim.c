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

GtkWidget *wtoy=0;
GtkWidget *gevas=0;
GtkObject *evh_selector = 0;
GtkgEvasSprite* sprite = 0;


static gint delete_event_cb(GtkWidget * window, GdkEventAny * e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}



void setup_bg()
{
	int w, h;
	GtkgEvasImage *gevas_image;
	GtkObject *evh;

	gevas_image = gevasimage_new();
	gevasobj_set_gevas(gevas_image, gevas);
	printf(" Trying to load bg.png from %s\n",PACKAGE_DATA_DIR "/bg.png");
	printf(" please note that if the image is not at the location, this will\n");
	printf(" quitely fail and look really bad!\n");

	gevasimage_set_image_name(gevas_image, PACKAGE_DATA_DIR "/bg.png");
	gevasimage_get_image_size(GTK_GEVASOBJ(gevas_image), &w, &h);
	gevasimage_set_image_fill(GTK_GEVASOBJ(gevas_image), 0,0,w,h);
	gevasobj_move(GTK_GEVASOBJ(gevas_image), 0, 0);
	gevasobj_set_layer(GTK_GEVASOBJ(gevas_image), -9999);
	gevasobj_resize(GTK_GEVASOBJ(gevas_image), 9999,9999);
	gevasobj_show(GTK_GEVASOBJ(gevas_image));
}


void setup_sprite()
{
    sprite = gevas_sprite_new( GTK_GEVAS(gevas) );
/*    gevas_sprite_push_metadata_prefix(
        sprite,
        "edb:///cvs/writeE/misc/gevas/demo-sprite?prefix=" );
*/        
    
    gevas_sprite_load_from_metadata(
        sprite,
        "edb:///cvs/writeE/misc/gevas-examples/trivial-sprite/cell?prefix=cell" );

    gevas_sprite_move( sprite, 100, 100 );
    gevas_sprite_show( sprite );

    // this will be added to the inter frame delay.
    gevas_sprite_set_default_frame_delay( sprite, 40 );

    // using this trick we can set a delay per frame that is different
    // for each frame, using the base offset method we can reuse the same
    // array for other sprites and have them slow down on different frames.
    {
        GArray* times=0;
        gint    times_size=6;
        gint    i = 0;
        
        times = g_array_new(0,0,sizeof(gint));
        for (i = 0; i < times_size; i++)
        {
            gint v = i*1000;
            g_array_append_val(times, v );
        }
        
        
        gevas_sprite_set_inter_frame_delays( sprite, 3, times, times_size );
    }

    
    gevas_sprite_play_forever( sprite );
    
}




int main(int argc, char *argv[])
{

	GtkWidget *window;


	gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window),"delete_event", GTK_SIGNAL_FUNC(delete_event_cb), NULL);
    
    gevas_new_gtkscrolledwindow( &gevas, &wtoy );
    gtk_container_add(GTK_CONTAINER(window), wtoy);

    
    gtk_widget_set_usize(gevas, 3000, 3000);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
/*	gevas_set_render_mode( gevas, RENDER_METHOD_3D_HARDWARE ); */
//    gevas_set_render_mode(gevas, RENDER_METHOD_ALPHA_SOFTWARE);
//	gevas_set_size_request_x(gevas, 200);
//	gevas_set_size_request_y(gevas, 200);
//    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(wtoy), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);


    gtk_window_set_title(GTK_WINDOW(window), "gevas does sprites");
    gtk_widget_show_all(window);

    gevas_add_fontpath(GTK_GEVAS(gevas), PACKAGE_DATA_DIR);
    setup_bg();
    setup_sprite();
    
    
    gtk_main();
    return 0;
}




