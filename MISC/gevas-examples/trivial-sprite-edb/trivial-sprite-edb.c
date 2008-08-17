/******************************************************************************
*******************************************************************************
*******************************************************************************

    A test for gevas, uses the sprite code, all cell images are loaded from the
    edb file.
 
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


static gint delete_event_cb(GtkWidget * window, GdkEventAny * e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char *argv[])
{
	GtkgEvasImage*  bg     = 0;
    GtkgEvasSprite* sprite = 0;
	GtkWidget *window;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window),"delete_event", GTK_SIGNAL_FUNC(delete_event_cb), NULL);
    
    gevas_new_gtkscrolledwindow( &gevas, &wtoy );
    gtk_container_add(GTK_CONTAINER(window), wtoy);
    gtk_widget_set_usize(gevas, 3000, 3000);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    
//	gevas_set_render_mode( gevas, RENDER_METHOD_3D_HARDWARE ); 
//  gevas_set_render_mode( gevas, RENDER_METHOD_ALPHA_SOFTWARE );


    gtk_window_set_title(GTK_WINDOW(window), "gevas does sprites");
    gtk_widget_show_all(window);

    gevas_add_metadata_prefix( gevas, PACKAGE_DATA_DIR );
    gevas_add_image_prefix   ( gevas, PACKAGE_DATA_DIR );
    gevas_add_fontpath       ( gevas, PACKAGE_DATA_DIR );
    gevas_add_metadata_prefix( gevas, "./" );
    gevas_add_image_prefix   ( gevas, "./" );
    gevas_add_fontpath       ( gevas, "./" );
    

//    bg = gevasimage_new_from_metadata( gevas, "data.db#edb?prefix=bg?");
    bg = gevasimage_new_from_metadata( gevas,
                                       "data.db#edb?prefix=bg/Location&"
            "x=0&y=0&visible=1&fill_size=1&layer=-9999&resize_x=9999&resize_y=9999" );
    
    /*
      Note that we can either have these in the edb under prefix/ or we can set
      them in the uri. Options in the uri override edb values.

      for example, we could have:
      "data.db#edb?prefix=cell&default_frame_delay=500&x=100&y=100&visible=1&play_forever=1" 
    */
    sprite = gevas_sprite_new_from_metadata(gevas, "data.db#edb?prefix=cell" );
    
    gtk_main();
    return 0;
}




