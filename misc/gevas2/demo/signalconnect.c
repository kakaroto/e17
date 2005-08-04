/*
 * Copyright (C) 2002 Ben Martin
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

#include <gtk/gtk.h>

#include <stdio.h>

GtkgEvasImage* gimage = 0;
GtkWidget *gevas;
GtkProgressBar*   x_coord_tracker = 0;
GtkWidget*        y_coord_tracker = 0;
GtkWidget*        e_logo_label = 0;

int CANVAS_WIDTH  = 500;
int CANVAS_HEIGHT = 500;


static gint delete_event_cb(GtkWidget * window, GdkEventAny * e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}



static gint raptor_moved(
    GtkgEvasObj* o,
    Evas_Coord* x, Evas_Coord* y,
	gpointer user_data )
{
    g_return_val_if_fail(o     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(x     != NULL, GEVASOBJ_SIG_VETO);
	g_return_val_if_fail(y     != NULL, GEVASOBJ_SIG_VETO);
    g_return_val_if_fail( GTK_IS_GEVASOBJ(o), GEVASOBJ_SIG_VETO);

/*     printf("raptor_moved() x:%d y:%d\n", *x, *y ); */
    gtk_progress_bar_set_fraction( x_coord_tracker, (1.0 * (*x)) / CANVAS_WIDTH ); 
    gtk_range_set_value( GTK_RANGE(y_coord_tracker), *y );
    
    return GEVASOBJ_SIG_OK;
}

void y_coord_changed( GtkRange *range, gpointer user_data )
{
    GtkgEvasObj* go = (GtkgEvasObj*)user_data;
    gint v = (gint)gtk_range_get_value( range );

    Evas_Coord x;
    Evas_Coord y;

    gevasobj_get_location( go, &x, &y );
	gevasobj_move( go, x, v );
//    printf("Moving to x:%d y:%d\n", x, v );
    
}

static gboolean
gtk_mouse_down_cb(GtkObject * object,
				  GtkObject * gevasobj, gint _b, gint _x, gint _y,
				  gpointer data)
{
    char buffer[1024];
	snprintf(buffer,1000,"gtk_mouse_down_cb b:%d x:%d y:%d", _b, _x, _y);
    gtk_label_set_text( e_logo_label, buffer );
	return FALSE;
}

static gboolean
gtk_mouse_up_cb(GtkObject * object,
				GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
    char buffer[1024];
	snprintf(buffer,1000,"gtk_mouse_up_cb b:%d x:%d y:%d", _b, _x, _y);
    gtk_label_set_text( e_logo_label, buffer );
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

    gevas_add_fontpath( GTK_GEVAS(gevas), PACKAGE_DATA_DIR );
    gevas_add_fontpath( GTK_GEVAS(gevas), g_get_current_dir() );
    gevas_add_fontpath( GTK_GEVAS(gevas), "/usr/X11R6/lib/X11/fonts/msttcorefonts" );

    x_coord_tracker = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    w = GTK_WIDGET( x_coord_tracker );
    gtk_widget_set_name( w, "x_coord_tracker" );
    gtk_progress_bar_set_orientation( x_coord_tracker, GTK_PROGRESS_LEFT_TO_RIGHT );
    gtk_progress_bar_set_text(     x_coord_tracker, "X position" ); 

    y_coord_tracker = gtk_hscale_new_with_range( 0, CANVAS_HEIGHT, 1 );
    w = GTK_WIDGET( y_coord_tracker );
    gtk_widget_set_name( w, "y_coord_tracker" );
//    gtk_progress_bar_set_text(     y_coord_tracker, "Y position" ); 

    e_logo_label = GTK_LABEL(gtk_label_new("Click E logo..."));
    w = GTK_WIDGET( e_logo_label );
    gtk_widget_set_name( w, "ELogoLabel" );
    
    pane = GTK_WIDGET(gtk_vbox_new(0,0));
    gtk_box_pack_start(GTK_BOX(pane), x_coord_tracker, 0, 0, 0 );
    gtk_box_pack_start(GTK_BOX(pane), y_coord_tracker, 0, 0, 0 );
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
	gtk_window_set_title(GTK_WINDOW(window), "Evas object talking with GTK+2 widgets");

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

    gtk_signal_connect( go, "move_absolute",
                        GTK_SIGNAL_FUNC( raptor_moved ), go );
    gtk_signal_connect(GTK_OBJECT ( y_coord_tracker ), "value-changed",
                       GTK_SIGNAL_FUNC(y_coord_changed), go );

/*     gtk_signal_connect_swapped( go, "move_absolute", */
/*                                 GTK_SIGNAL_FUNC( gtk_range_set_value ), y_coord_tracker ); */
/*     void        gtk_range_set_value             (GtkRange *range, */
/*                                                  gdouble value); */

    gi = gimage = gevasimage_new();
    gevasobj_set_gevas( gi, gevas );
    go = GTK_GEVASOBJ( gi );
    gevasimage_set_image_name( gi, "e_logo.png" );
    gevasobj_move(      go, 300, 100 );
    gevasobj_set_layer( go, 2 );
    gevasobj_show(      go );

    evh = gevasevh_to_gtk_signal_new();
	gevasobj_add_evhandler( GTK_GEVASOBJ( gi ), evh );
    
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_down",
					   GTK_SIGNAL_FUNC(gtk_mouse_down_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_up",
					   GTK_SIGNAL_FUNC(gtk_mouse_up_cb), NULL);
    
    
    gtk_widget_show_all(window);
    return window;
}


int main(int argc, char *argv[])
{
    GtkWidget* win1;
    
    gtk_init(&argc, &argv);

    win1 = createAndShowWindow();
//    g_timeout_add( 2000, scaleImage, &win1);
    gtk_main();
    return 0;
}
