
/*  geist - a project scheduling tool.  Part of the GNOME project.
 *  Copyright (C) 2000.  Tom Gilbert & Michael Davies.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Handle the case that config.h wasn't generated */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#else
#  define PACKAGE "geist"
#  define VERSION "unknown"
#  define PACKAGE_LOCALE_DIR PREFIX"/share/locale"
#endif

#include <gtk/gtk.h>

#include "utils.h"
#include "geist.h"
#include "main.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_gtk.h"
#include "layers.h"
#include "geist_document.h"

int call_level = 0;
GtkWidget *mainwin, *darea, *evbox, *scrollwin, *viewport;
geist_document *doc;

gboolean mainwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer user_data);
gboolean mainwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

                            gpointer user_data);
gboolean configure_cb(GtkWidget * widget, GdkEventConfigure * event,

                      gpointer user_data);

int
main(int argc, char *argv[])
{
   D_ENTER(3);

   opt.debug_level = 5;

   printf("%s - version %s\n", PACKAGE, VERSION);


   gtk_init(&argc, &argv);

   mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_policy(GTK_WINDOW(mainwin), TRUE, TRUE, TRUE);
   gtk_window_set_wmclass(GTK_WINDOW(mainwin), "geist", "geist");
   gtk_signal_connect(GTK_OBJECT(mainwin), "delete_event",
                      GTK_SIGNAL_FUNC(mainwin_delete_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(mainwin), "destroy_event",
                      GTK_SIGNAL_FUNC(mainwin_destroy_cb), NULL);
   gtk_widget_show(mainwin);

   
   evbox = gtk_event_box_new();
   gtk_container_add(GTK_CONTAINER(mainwin), evbox);
   gtk_widget_show(evbox);

  scrollwin = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrollwin);
  gtk_container_add (GTK_CONTAINER (evbox), scrollwin);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollwin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);
  gtk_container_add (GTK_CONTAINER (scrollwin), viewport);
   
   /* The drawing area itself */
   darea = gtk_drawing_area_new();
   gtk_container_add(GTK_CONTAINER(viewport), darea);
   gtk_signal_connect_after(GTK_OBJECT(darea), "configure_event",
                            GTK_SIGNAL_FUNC(configure_cb), NULL);
   gtk_widget_show(darea);
   imlib_init(darea);
   
   
   doc = geist_document_new(400, 400);
   doc->layers = geist_list_add_front(doc->layers, geist_layer_new());

   ((geist_layer *) doc->layers->data)->objects =
      geist_list_add_front(((geist_layer *) doc->layers->data)->objects,
                           geist_image_new_from_file(20,40,"testimages/elogo.png"));

   ((geist_layer *) doc->layers->data)->objects =
      geist_list_add_front(((geist_layer *) doc->layers->data)->objects,
                           geist_image_new_from_file(5,5,"testimages/globe.png"));

   ((geist_layer *) doc->layers->data)->objects =
      geist_list_add_front(((geist_layer *) doc->layers->data)->objects,
                           geist_image_new_from_file(75,35,"testimages/bulb.png"));

   ((geist_layer *) doc->layers->data)->objects =
      geist_list_add_front(((geist_layer *) doc->layers->data)->objects,
                           geist_image_new_from_file(275,145,"testimages/bulb.png"));

   
   geist_document_render(doc);
   gtk_window_set_default_size(GTK_WINDOW(mainwin), doc->w, doc->h);
   gtk_widget_set_usize(darea, doc->w, doc->h);

   geist_document_render_to_gtk_window(doc, darea);
   gtk_main();
   D_RETURN(3, 0);
}

gboolean mainwin_delete_cb(GtkWidget * widget, GdkEvent * event,
                           gpointer user_data)
{
   D_ENTER(3);
   gtk_exit(0);
   D_RETURN(3, FALSE);
}

gboolean mainwin_destroy_cb(GtkWidget * widget, GdkEvent * event,
                            gpointer user_data)
{
   D_ENTER(3);
   gtk_exit(0);
   D_RETURN(3, FALSE);
}


gboolean
configure_cb(GtkWidget * widget, GdkEventConfigure * event,
             gpointer user_data)
{
   D_ENTER(3);

   geist_document_render_to_gtk_window(doc, darea);

   D_RETURN(3, TRUE);
}
