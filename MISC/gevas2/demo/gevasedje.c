/*
 * Copyright (C) 2005 Ben Martin
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
 */

#include <gevas.h>
#include <gevasimage.h>
#include <gevasedje.h>
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

#include <gtk/gtk.h>

#include <stdio.h>

GtkgEvasImage* gimage = 0;
GtkWidget *gevas;
GtkWidget*        e_logo_label = 0;

int CANVAS_WIDTH = 1000;
int CANVAS_HEIGHT = 1000;

static gint delete_event_cb(GtkWidget * window, GdkEventAny * e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

GtkWidget* createAndShowWindow()
{
	GtkWidget *window;
    GtkWidget *wtoy;
    GtkWidget *pane;
    GtkWidget *w;
    GtkgEvasImage* gi;
    GtkgEvasObj* go;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gevas_new_gtkscrolledwindow( (GtkgEvas**)(&gevas), &wtoy );
    gevas_setup_ecore( (GtkgEvas*)gevas );

    gevas_add_fontpath( GTK_GEVAS(gevas), g_get_current_dir() );
    gevas_add_fontpath( GTK_GEVAS(gevas), "/usr/X11R6/lib/X11/fonts/msttcorefonts" );

    e_logo_label = GTK_WIDGET(gtk_label_new("Click E logo..."));
    w = GTK_WIDGET( e_logo_label );
    gtk_widget_set_name( w, "ELogoLabel" );
    
    pane = GTK_WIDGET(gtk_vbox_new(0,0));
    gtk_box_pack_start(GTK_BOX(pane), e_logo_label,    0, 0, 0 );
    gtk_box_pack_start(GTK_BOX(pane), wtoy,  1, 1, 0 );
    
    gtk_container_add(GTK_CONTAINER(window), pane);
    gtk_widget_set_usize(gevas, CANVAS_WIDTH, CANVAS_HEIGHT);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	gevas_set_size_request_x(gevas, 200);
	gevas_set_size_request_y(gevas, 200);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(wtoy),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
	gevas_set_checked_bg(gevas, 1);
	gtk_window_set_title(GTK_WINDOW(window), "gevasedje: edje objects on a gevas");

	gevas_set_middleb_scrolls(GTK_GEVAS(gevas), 1,
							  gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(wtoy)),
							  gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(wtoy)));
    
    gtk_signal_connect(GTK_OBJECT(window),
					   "delete_event", GTK_SIGNAL_FUNC(delete_event_cb), NULL);

    gi = gimage = gevasimage_new();
    gevasobj_set_gevas( gi, gevas );
    go = GTK_GEVASOBJ( gi );
    gevasimage_set_image_name( gi, "raptor.png" );
    gevasobj_move(      go, 0, 0 );
    gevasobj_set_layer( go, 1 );
    gevasobj_show(      go );

    /** This handler will let the user drag the raptor around **/
	GtkObject *evh = gevasevh_drag_new();
	gevasobj_add_evhandler( GTK_GEVASOBJ( gi ), evh );

    gi = gimage = gevasimage_new();
    gevasobj_set_gevas( gi, gevas );
    go = GTK_GEVASOBJ( gi );
    gevasimage_set_image_name( gi, "e_logo.png" );
    gevasobj_move(      go, 300, 100 );
    gevasobj_set_layer( go, 2 );
    gevasobj_show(      go );

    GtkgEvasEdje* gedje = gevasedje_new_with_canvas( gevas );
    gevasedje_set_file( gedje, "e_logo.eet", "test" );
    go = GTK_GEVASOBJ(gedje);
    gevasobj_move(      go, 300, 300 );
    gevasobj_resize(    go, 370, 350 );
    gevasobj_set_layer( go, 10 );
    gevasobj_show(      go );

    
    
    gtk_widget_show_all(window);
    
    return window;
}


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int main(int argc, char *argv[])
{
    GtkWidget* win1;
    
    ecore_init();
    edje_init();
    gtk_init(&argc, &argv);

    win1 = createAndShowWindow();

    gtk_main();
    return 0;
}
