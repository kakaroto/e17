
/*  main.c  */

#include <gtk/gtk.h>

#include "utils.h"
#include "geist.h"
#include "main.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_gtk.h"
#include "geist_text.h"
#include "layers.h"

int call_level = 0;
GtkWidget *mainwin, *darea, *evbox, *scrollwin, *viewport;
geist_document *doc;

gboolean mainwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer user_data);
gboolean mainwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

                            gpointer user_data);
gboolean configure_cb(GtkWidget * widget, GdkEventConfigure * event,

                      gpointer user_data);
gint evbox_buttonpress_cb(GtkWidget * widget, GdkEventButton * event);
gint evbox_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event);
gint evbox_mousemove_cb(GtkWidget * widget, GdkEventMotion * event);
void idle_draw_cb(GtkWidget * widget, gpointer * data);

int
main(int argc, char *argv[])
{
   GtkWidget *hwid, *vwid;

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

   hwid = gtk_hbox_new(TRUE, 0);
   gtk_widget_show(hwid);
   gtk_container_add(GTK_CONTAINER(mainwin), hwid);

   vwid = gtk_vbox_new(TRUE, 0);
   gtk_widget_show(vwid);
   gtk_box_pack_start(GTK_BOX(hwid), vwid, TRUE, FALSE, 0);

   scrollwin = gtk_scrolled_window_new(NULL, NULL);
   gtk_widget_show(scrollwin);
   gtk_box_pack_start(GTK_BOX(vwid), scrollwin, TRUE, FALSE, 0);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwin),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   viewport = gtk_viewport_new(NULL, NULL);
   gtk_widget_show(viewport);
   gtk_container_add(GTK_CONTAINER(scrollwin), viewport);

   evbox = gtk_event_box_new();
   gtk_container_add(GTK_CONTAINER(viewport), evbox);
   gtk_widget_show(evbox);
   gtk_signal_connect(GTK_OBJECT(evbox), "button_press_event",
                      GTK_SIGNAL_FUNC(evbox_buttonpress_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(evbox), "button_release_event",
                      GTK_SIGNAL_FUNC(evbox_buttonrelease_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(evbox), "motion_notify_event",
                      GTK_SIGNAL_FUNC(evbox_mousemove_cb), NULL);

   /* The drawing area itself */
   darea = gtk_drawing_area_new();
   gtk_container_add(GTK_CONTAINER(evbox), darea);
   gtk_signal_connect_after(GTK_OBJECT(darea), "configure_event",
                            GTK_SIGNAL_FUNC(configure_cb), NULL);
   gtk_widget_show(darea);
   imlib_init(darea);


   doc = geist_document_new(400, 400);

   geist_document_add_layer(doc);

   geist_document_add_object(doc,
                             geist_image_new_from_file(20, 40,
                                                       "testimages/elogo.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(5, 5,
                                                       "testimages/globe.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(75, 35,
                                                       "testimages/bulb.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(275, 145,
                                                       "testimages/bulb.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(355, 70,
                                                       "testimages/mail.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(275, 345,
                                                       "testimages/mush.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(175, 5,
                                                       "testimages/paper.png"));
   geist_document_add_object(doc,
                             geist_text_new_with_text(175, 105, "20thcent/16",
                                                      "So this is geist..."));


   geist_document_render(doc);
   geist_document_render_selection(doc);
   geist_document_render_pmap(doc);
   gtk_window_set_default_size(GTK_WINDOW(mainwin), doc->w, doc->h);
   gtk_widget_set_usize(scrollwin, doc->w, doc->h);

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

gint
evbox_buttonpress_cb(GtkWidget * widget, GdkEventButton * event)
{
   geist_object *obj;

   D_ENTER(5);

   if (event->button == 1)
   {
      geist_list *l, *list;


      obj = geist_document_find_clicked_object(doc, event->x, event->y);
      if (!obj)
         D_RETURN(5, 1);

      if ((event->state & GDK_SHIFT_MASK))
      {
         geist_object_toggle_state(obj, SELECTED);
         geist_document_dirty_object(doc, obj);
         D_RETURN(5, 1);
      }
      else if (!geist_object_get_state(obj, SELECTED))
      {
         geist_document_unselect_all(doc);
         D(2, ("setting object state SELECTED\n"));
         geist_object_set_state(obj, SELECTED);
         geist_document_dirty_object(doc, obj);
      }

      list = geist_document_get_selected_list(doc);
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         obj->clicked_x = event->x - obj->x;
         obj->clicked_y = event->y - obj->y;
         D(2, ("setting object state DRAG\n"));
         geist_object_set_state(obj, DRAG);
         geist_object_raise(doc, obj);
         geist_document_dirty_object(doc, obj);
      }
      gtk_object_set_data_full(GTK_OBJECT(mainwin), "draglist", list, NULL);
      geist_document_render_updates(doc);
      /* geist_document_render(doc); */
   }

   D_RETURN(5, 1);
}

gint evbox_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event)
{
   geist_list *list, *l;
   geist_object *obj;

   D_ENTER(5);

   list = gtk_object_get_data(GTK_OBJECT(mainwin), "draglist");
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);

         D(2, ("unsetting object state DRAG\n"));
         geist_object_unset_state(obj, DRAG);
         geist_document_dirty_object(doc, obj);
      }
   }
   geist_list_free(list);
   gtk_object_set_data_full(GTK_OBJECT(mainwin), "draglist", NULL, NULL);
   geist_document_render_updates(doc);
   /* geist_document_render(doc); */

   D_RETURN(5, 1);
}

gint
evbox_mousemove_cb(GtkWidget * widget, GdkEventMotion * event)
{
   geist_list *l, *list;
   geist_object *obj;

   D_ENTER(5);

   list = gtk_object_get_data(GTK_OBJECT(mainwin), "draglist");
   if (list)
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         D(5, ("moving object to %f, %f\n", event->x, event->y));
         doc->up =
            imlib_update_append_rect(doc->up, obj->x, obj->y, obj->w, obj->h);
         obj->x = event->x - obj->clicked_x;
         obj->y = event->y - obj->clicked_y;
         geist_document_dirty_object(doc, obj);
      }
   geist_document_render_updates(doc);
   /* geist_document_render(doc); */

   D_RETURN(5, 1);
}
