
/*  main.c  */

#include <gtk/gtk.h>

#include "utils.h"
#include "geist.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_gtk.h"
#include "geist_text.h"
#include "layers.h"

int call_level = 0;
GtkWidget *mainwin, *obj_win, *darea, *evbox, *scrollwin, *viewport;
geist_document *doc;
GtkWidget *obj_list;

struct attribs
{
   GtkWidget *visible;
   GtkWidget *name;
};

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

gboolean obj_add_cb(GtkWidget * widget, gpointer * data);
gboolean obj_cpy_cb(GtkWidget * widget, gpointer * data);
gboolean obj_del_cb(GtkWidget * widget, gpointer * data);
gboolean obj_sel_cb(GtkWidget * widget, int row, int column,
                    GdkEventButton * event, gpointer * data);

int
main(int argc, char *argv[])
{
   GtkWidget *hwid, *vwid;
   GtkWidget *obj_table, *obj_btn, *obj_scroll;

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

   obj_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   obj_table = gtk_table_new(6, 1, FALSE);
   gtk_container_set_border_width(GTK_CONTAINER(obj_win), 3);
   gtk_container_add(GTK_CONTAINER(obj_win), obj_table);
   obj_scroll = gtk_scrolled_window_new(NULL, NULL);
   gtk_table_attach(GTK_TABLE(obj_table), obj_scroll, 0, 4, 1, 2,
                    GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 2, 2);
   obj_list = gtk_clist_new(2);
   gtk_clist_set_selection_mode(GTK_CLIST(obj_list), GTK_SELECTION_SINGLE);
   gtk_clist_column_titles_hide(GTK_CLIST(obj_list));
   gtk_clist_column_titles_passive(GTK_CLIST(obj_list));
   gtk_clist_set_column_visibility(GTK_CLIST(obj_list), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(obj_list), 0, TRUE);
   gtk_clist_set_column_visibility(GTK_CLIST(obj_list), 1, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(obj_list), 1, TRUE);
   gtk_signal_connect(GTK_OBJECT(obj_list), "select_row",
                      GTK_SIGNAL_FUNC(obj_sel_cb), NULL);
   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(obj_scroll),
                                         obj_list);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(obj_scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   obj_btn = gtk_button_new_with_label("Add...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_add_cb), NULL);
   gtk_table_attach(GTK_TABLE(obj_table), obj_btn, 0, 1, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Copy");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_cpy_cb), NULL);
   gtk_table_attach(GTK_TABLE(obj_table), obj_btn, 1, 2, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Delete");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_del_cb), NULL);
   gtk_table_attach(GTK_TABLE(obj_table), obj_btn, 2, 3, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_window_set_default_size(GTK_WINDOW(obj_win), 250, 200);
   gtk_widget_show(obj_btn);
   gtk_widget_show(obj_list);
   gtk_widget_show(obj_scroll);
   gtk_widget_show(obj_table);
   gtk_widget_show(obj_win);

   doc = geist_document_new(500, 500);

   geist_document_add_layer(doc);

   geist_document_add_object(doc,
                             geist_image_new_from_file(0, 0,
                                                       PREFIX
                                                       "/share/geist/images/laet.jpg"));
   geist_document_add_object(doc,
                             geist_text_new_with_text(0, 405, "20thcent/16",
                                                      "Some pr0n - I have to."));
   geist_document_add_object(doc,
                             geist_image_new_from_file(220, 140,
                                                       PREFIX
                                                       "/share/geist/images/elogo.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(125, 5,
                                                       PREFIX
                                                       "/share/geist/images/globe.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(175, 125,
                                                       PREFIX
                                                       "/share/geist/images/bulb.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(375, 145,
                                                       PREFIX
                                                       "/share/geist/images/bulb.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(415, 200,
                                                       PREFIX
                                                       "/share/geist/images/mail.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(445, 305,
                                                       PREFIX
                                                       "/share/geist/images/mush.png"));

   geist_document_add_object(doc,
                             geist_image_new_from_file(315, 405,
                                                       PREFIX
                                                       "/share/geist/images/paper.png"));
   geist_document_add_object(doc,
                             geist_text_new_with_text(275, 15, "20thcent/16",
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

gboolean
mainwin_delete_cb(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
   D_ENTER(3);
   gtk_exit(0);
   D_RETURN(3, FALSE);
}

gboolean
mainwin_destroy_cb(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
   D_ENTER(3);
   gtk_exit(0);
   D_RETURN(3, FALSE);
}


gboolean configure_cb(GtkWidget * widget, GdkEventConfigure * event,
                      gpointer user_data)
{
   D_ENTER(3);

   geist_document_render_to_gtk_window(doc, darea);

   D_RETURN(3, TRUE);
}

gint evbox_buttonpress_cb(GtkWidget * widget, GdkEventButton * event)
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
         geist_list *ll;

         geist_object_toggle_state(obj, SELECTED);
         ll = geist_document_get_selected_list(doc);
         if (ll)
         {
            geist_list *lll;

            for (lll = ll; lll; lll = lll->next)
               geist_document_dirty_object(doc, GEIST_OBJECT(lll->data));
            geist_list_free(ll);
         }
         geist_document_render_updates(doc);
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
      if (list)
      {
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
      }
      gtk_object_set_data_full(GTK_OBJECT(mainwin), "draglist", list, NULL);
      geist_document_render_updates(doc);
   }

   D_RETURN(5, 1);
}

gint
evbox_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event)
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

   D_RETURN(5, 1);
}

gint evbox_mousemove_cb(GtkWidget * widget, GdkEventMotion * event)
{
   geist_list *l, *list;
   geist_object *obj;
   GdkEventMotion *e;

   D_ENTER(5);

   if (gdk_events_pending())
   {
      if ((e = (GdkEventMotion *) gdk_event_get()) != NULL)
      {
         if (e->type == GDK_MOTION_NOTIFY)
         {
            D(5, ("skipping event, new one coming\n"));
            event = e;
         }
         else
            gdk_event_put((GdkEvent *) e);
      }
   }

   list = gtk_object_get_data(GTK_OBJECT(mainwin), "draglist");
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         D(5, ("moving object to %f, %f\n", event->x, event->y));
         geist_document_dirty_object(doc, obj);
         obj->x = event->x - obj->clicked_x;
         obj->y = event->y - obj->clicked_y;
         geist_document_dirty_object(doc, obj);
      }
      geist_document_render_updates(doc);
   }

   D_RETURN(5, 1);
}

gboolean
obj_add_cb(GtkWidget * widget, gpointer * data)
{
   fprintf(stderr, "add object\n");
   return TRUE;
}

gboolean
obj_cpy_cb(GtkWidget * widget, gpointer * data)
{

   fprintf(stderr, "cpy object\n");
   return TRUE;
}

gboolean
obj_del_cb(GtkWidget * widget, gpointer * data)
{

   fprintf(stderr, "del object\n");
   return TRUE;
}

gboolean
obj_sel_cb(GtkWidget * widget, int row, int column, GdkEventButton * event,
           gpointer * data)
{
   geist_list *l, *list;
   geist_object *obj =
      (geist_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);

   geist_document_unselect_all(doc);
   D(2, ("setting object state SELECTED\n"));
   geist_object_set_state(obj, SELECTED);
   geist_document_dirty_object(doc, obj);

   geist_document_render_updates(doc);
   return TRUE;
}
