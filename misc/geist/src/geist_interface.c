#include "geist.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_document_gtk.h"
#include "geist_text.h"
#include "geist_rect.h"
#include "geist_layer.h"
#include "geist_document_xml.h"
#include "geist_gtk_menu.h"
#include "geist_interface.h"

GtkWidget *mainwin;
geist_document *current_doc;

GtkWidget *
geist_create_main_window(void)
{
   GtkWidget *mvbox, *menubar, *menu, *menuitem;
   GtkWidget *nbook;
   GtkWidget *mainwin;

   D_ENTER(3);

   mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_policy(GTK_WINDOW(mainwin), TRUE, TRUE, TRUE);
   gtk_window_set_wmclass(GTK_WINDOW(mainwin), "geist", "geist");
   gtk_signal_connect(GTK_OBJECT(mainwin), "delete_event",
                      GTK_SIGNAL_FUNC(mainwin_delete_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(mainwin), "destroy_event",
                      GTK_SIGNAL_FUNC(mainwin_destroy_cb), NULL);
   gtk_widget_show(mainwin);

   mvbox = gtk_vbox_new(FALSE, 0);
   gtk_widget_show(mvbox);
   gtk_container_add(GTK_CONTAINER(mainwin), mvbox);

   /* menus */
   tooltips = gtk_tooltips_new();

   menubar = gtk_menu_bar_new();
   gtk_widget_show(menubar);
   gtk_box_pack_start(GTK_BOX(mvbox), menubar, FALSE, FALSE, 0);
   menu = geist_gtk_create_submenu(menubar, "File");
   menuitem =
      geist_gtk_create_menuitem(menu, "Save as...", "", "Save Document As...",
                                (GtkFunction) menu_cb, "save doc as");

   nbook = gtk_notebook_new();
   gtk_notebook_set_tab_pos(GTK_NOTEBOOK(nbook), GTK_POS_BOTTOM);
   gtk_widget_show(nbook);
   gtk_box_pack_start(GTK_BOX(mvbox), nbook, TRUE, TRUE, 0);

   gtk_object_set_data(GTK_OBJECT(mainwin), "notebook", nbook);
   D_RETURN(3, mainwin);
}

GtkWidget *
geist_create_object_list(void)
{
   GtkWidget *obj_table, *obj_btn, *obj_btn_hbox, *obj_scroll;

   D_ENTER(3);

   obj_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   obj_table = gtk_table_new(3, 4, FALSE);
   gtk_container_set_border_width(GTK_CONTAINER(obj_win), 3);
   gtk_container_add(GTK_CONTAINER(obj_win), obj_table);
   obj_scroll = gtk_scrolled_window_new(NULL, NULL);
   gtk_table_attach(GTK_TABLE(obj_table), obj_scroll, 0, 4, 1, 2,
                    GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 2, 2);
   obj_list = gtk_clist_new(2);
   gtk_clist_set_selection_mode(GTK_CLIST(obj_list), GTK_SELECTION_EXTENDED);
   gtk_clist_column_titles_hide(GTK_CLIST(obj_list));
   gtk_clist_column_titles_passive(GTK_CLIST(obj_list));
   gtk_clist_set_column_visibility(GTK_CLIST(obj_list), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(obj_list), 0, TRUE);
   gtk_clist_set_column_visibility(GTK_CLIST(obj_list), 1, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(obj_list), 1, TRUE);
   obj_sel_handler =
      gtk_signal_connect(GTK_OBJECT(obj_list), "select_row",
                         GTK_SIGNAL_FUNC(obj_sel_cb), NULL);
   obj_unsel_handler =
      gtk_signal_connect(GTK_OBJECT(obj_list), "unselect_row",
                         GTK_SIGNAL_FUNC(obj_unsel_cb), NULL);
   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(obj_scroll),
                                         obj_list);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(obj_scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   obj_btn_hbox = gtk_hbox_new(FALSE, 0);
   obj_btn = gtk_button_new_with_label("Add Image...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_add_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Add Text...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_addtext_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Add Rectangle...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_addrect_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);

   obj_btn = gtk_button_new_with_label("Copy");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_cpy_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Delete");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_del_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Properties...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_edit_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);
   gtk_table_attach(GTK_TABLE(obj_table), obj_btn_hbox, 0, 3, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(obj_btn_hbox);
   gtk_window_set_default_size(GTK_WINDOW(obj_win), 125, 230);
   gtk_widget_show(obj_list);
   gtk_widget_show(obj_scroll);
   gtk_widget_show(obj_table);
   gtk_widget_show(obj_win);

   D_RETURN(3, obj_list);
}

geist_document *
geist_gtk_new_document_page(GtkWidget * parent, int w, int h, char *name)
{
   GtkWidget *hwid, *vwid, *darea, *viewport;
   GtkWidget *label, *scrollwin, *evbox;
   geist_document *doc;

   D_ENTER(3);

   current_doc = doc = geist_document_new(w, h);

   hwid = gtk_hbox_new(TRUE, 0);
   gtk_widget_show(hwid);
   label = gtk_label_new(name);
   gtk_widget_show(label);
   gtk_notebook_append_page(GTK_NOTEBOOK(parent), hwid, label);

   vwid = gtk_vbox_new(TRUE, 0);
   gtk_widget_show(vwid);
   gtk_box_pack_start(GTK_BOX(hwid), vwid, TRUE, FALSE, 0);

   scrollwin = gtk_scrolled_window_new(NULL, NULL);
   gtk_widget_set_usize(scrollwin, doc->w, doc->h);
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

   darea = gtk_drawing_area_new();
   gtk_object_set_data(GTK_OBJECT(darea), "doc", doc);
   doc->darea = darea;

   gtk_signal_connect(GTK_OBJECT(evbox), "button_press_event",
                      GTK_SIGNAL_FUNC(evbox_buttonpress_cb), doc);
   gtk_signal_connect(GTK_OBJECT(evbox), "button_release_event",
                      GTK_SIGNAL_FUNC(evbox_buttonrelease_cb), doc);
   gtk_signal_connect(GTK_OBJECT(evbox), "motion_notify_event",
                      GTK_SIGNAL_FUNC(evbox_mousemove_cb), doc);

   /* The drawing area itself */
   gtk_container_add(GTK_CONTAINER(evbox), darea);
   gtk_signal_connect_after(GTK_OBJECT(darea), "configure_event",
                            GTK_SIGNAL_FUNC(configure_cb), doc);
   gtk_widget_show(darea);

   D_RETURN(3, doc);
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
   geist_document *doc;

   D_ENTER(3);

   doc = GEIST_DOCUMENT(user_data);
   if (doc)
      geist_document_render_to_window(doc);

   D_RETURN(3, TRUE);
}

gint evbox_buttonpress_cb(GtkWidget * widget, GdkEventButton * event,
                          gpointer user_data)
{
   geist_object *obj = NULL;
   geist_document *doc;

   D_ENTER(5);

   doc = GEIST_DOCUMENT(user_data);
   if (!doc)
      D_RETURN(5, 1);

   if (event->button == 1)
   {
      geist_list *l, *list;
      int row;

      /* First check for resizes */
      list = geist_document_get_selected_list(doc);
      if (list)
      {
         int resize = 0;

         for (l = list; l; l = l->next)
         {
            obj = GEIST_OBJECT(l->data);
            if (
                (resize =
                 geist_object_check_resize_click(obj, event->x, event->y)))
               break;
         }

         if (resize)
         {
            int res_x, res_y;

            /* Click requests a resize. */
            for (l = list; l; l = l->next)
            {
               obj = GEIST_OBJECT(l->data);
               D(5, ("setting object state RESIZE\n"));
               geist_object_set_state(obj, RESIZE);
               obj->resize = resize;
               geist_object_get_resize_box_coords(obj, resize, &res_x,
                                                  &res_y);
               obj->clicked_x = res_x - event->x;
               obj->clicked_y = res_y - event->y;
               geist_object_dirty(obj);
            }
            gtk_object_set_data_full(GTK_OBJECT(mainwin), "resizelist", list,
                                     NULL);
            geist_document_render_updates(doc);
            D_RETURN(5, 1);
         }
         geist_list_free(list);
      }

      obj = geist_document_find_clicked_object(doc, event->x, event->y);
      if (!obj)
      {
         geist_document_unselect_all(doc);
         geist_document_render_updates(doc);
         D_RETURN(5, 1);
      }
      gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_sel_handler);
      gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_unsel_handler);

      if (event->state & GDK_SHIFT_MASK)
      {
         /* shift click - multiple selections and selection toggling */
         row =
            gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
         if (geist_object_get_state(obj, SELECTED))
         {
            geist_object_unselect(obj);
            if (row != -1)
               gtk_clist_unselect_row(GTK_CLIST(obj_list), row, 0);
         }
         else
         {
            geist_object_select(obj);
            if (row != -1)
               gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
         }

         /* need to dirty all the other selected objects, in case they go from
          * being the only thing selected to part of a multiple selection -
          * hence have to change their selection type */
         geist_document_dirty_selection(doc);
      }
      else
      {
         if (!geist_object_get_state(obj, SELECTED))
         {
            /* Single click selection */
            geist_document_unselect_all(doc);
            gtk_clist_unselect_all(GTK_CLIST(obj_list));
            geist_object_select(obj);
            row =
               gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                            (gpointer) obj);
            if (row != -1)
               gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);

         }

         list = geist_document_get_selected_list(doc);
         if (list)
         {
            for (l = list; l; l = l->next)
            {
               /* set clicked_x,y */
               obj = GEIST_OBJECT(l->data);
               obj->clicked_x = event->x - obj->x;
               obj->clicked_y = event->y - obj->y;
               D(2, ("setting object state DRAG\n"));
               geist_object_set_state(obj, DRAG);
               geist_object_raise(obj);
            }
         }
         gtk_object_set_data_full(GTK_OBJECT(mainwin), "draglist", list,
                                  NULL);
      }
      geist_document_render_updates(doc);
      gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_sel_handler);
      gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_unsel_handler);
   }
   D_RETURN(5, 1);
}

gint
evbox_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event,
                       gpointer user_data)
{
   geist_list *list, *l;
   geist_object *obj;
   geist_document *doc;

   D_ENTER(5);

   doc = GEIST_DOCUMENT(user_data);
   if (!doc)
      D_RETURN(5, 1);

   list = gtk_object_get_data(GTK_OBJECT(mainwin), "draglist");
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);

         D(2, ("unsetting object state DRAG\n"));
         geist_object_unset_state(obj, DRAG);
         geist_object_dirty(obj);
      }
      geist_list_free(list);
      gtk_object_set_data_full(GTK_OBJECT(mainwin), "draglist", NULL, NULL);
   }
   else
   {
      list = gtk_object_get_data(GTK_OBJECT(mainwin), "resizelist");
      if (list)
      {
         for (l = list; l; l = l->next)
         {
            obj = GEIST_OBJECT(l->data);

            D(2, ("unsetting object state RESIZE\n"));
            geist_object_unset_state(obj, RESIZE);
            obj->resize = RESIZE_NONE;
            geist_object_dirty(obj);
         }
         geist_list_free(list);
         gtk_object_set_data_full(GTK_OBJECT(mainwin), "resizelist", NULL,
                                  NULL);

      }
   }
   geist_document_render_updates(doc);

   D_RETURN(5, 1);
}

gint
evbox_mousemove_cb(GtkWidget * widget, GdkEventMotion * event,
                   gpointer user_data)
{
   geist_list *l, *list;
   geist_object *obj;
   GdkEventMotion *e;
   geist_document *doc;

   D_ENTER(5);

   doc = GEIST_DOCUMENT(user_data);
   if (!doc)
      D_RETURN(5, 1);

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
         geist_object_move(obj, event->x, event->y);
      }
      geist_document_render_updates(doc);
   }
   else
   {
      list = gtk_object_get_data(GTK_OBJECT(mainwin), "resizelist");
      if (list)
      {
         for (l = list; l; l = l->next)
         {
            obj = GEIST_OBJECT(l->data);
            D(5, ("resizing object\n"));
            geist_object_resize(obj, event->x + obj->clicked_x,
                                event->y + obj->clicked_y);
         }
         geist_document_render_updates(doc);
      }

   }

   D_RETURN(5, 1);
}

gboolean
obj_load_cb(GtkWidget * widget, gpointer data)
{
   geist_object *obj = NULL;
   char *path;

   D_ENTER(3);

   path =
      gtk_file_selection_get_filename(GTK_FILE_SELECTION((GtkWidget *) data));
   if (path)
   {
      obj = geist_image_new_from_file(0, 0, path);
      if (obj)
      {
         geist_document_add_object(current_doc, obj);
         geist_object_show(obj);
         geist_object_raise(obj);
         geist_object_display_props(obj);
         geist_document_render_updates(current_doc);
      }
   }
   gtk_widget_destroy((GtkWidget *) data);
   D_RETURN(3, TRUE);
}

gboolean
obj_load_cancel_cb(GtkWidget * widget, gpointer data)
{
   gtk_widget_destroy((GtkWidget *) data);
   return TRUE;
}

gboolean
obj_add_cb(GtkWidget * widget, gpointer * data)
{
   GtkWidget *file_sel = gtk_file_selection_new("Add an Image");

   /* TODO Needs to know the current doc somehow... */
   gtk_file_selection_show_fileop_buttons(GTK_FILE_SELECTION(file_sel));
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->ok_button),
                      "clicked", GTK_SIGNAL_FUNC(obj_load_cb),
                      (gpointer) file_sel);
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->cancel_button),
                      "clicked", GTK_SIGNAL_FUNC(obj_load_cancel_cb),
                      (gpointer) file_sel);
   gtk_widget_show(file_sel);
   return TRUE;
}


gboolean
obj_cpy_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *new;
   geist_list *l, *list;
   geist_object *obj;
   int row;

   D_ENTER(3);

   list = geist_document_get_selected_list(current_doc);
   for (l = list; l; l = l->next)
   {
      obj = GEIST_OBJECT(l->data);
      new = geist_object_duplicate(obj);
      if (new)
      {
         new->x += 5;
         new->y += 5;
         geist_document_add_object(current_doc, new);

         row =
            gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
         if (row != -1)
            gtk_clist_unselect_row(GTK_CLIST(obj_list), row, 0);

         row =
            gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) new);
         if (row != -1)
            gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
      }
   }
   geist_list_free(list);
   geist_document_render_updates(current_doc);

   D_RETURN(3, TRUE);
}

gboolean
obj_del_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj;
   geist_list *l, *list;

   D_ENTER(3);

   list = geist_document_get_selected_list(current_doc);
   for (l = list; l; l = l->next)
   {
      obj = GEIST_OBJECT(l->data);
      gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_sel_handler);
      gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_unsel_handler);

      if (obj->props_window)
      {
         gtk_widget_destroy(obj->props_window);
         obj->props_window = NULL;
      }

      gtk_clist_remove(GTK_CLIST(obj_list),
                       gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                                    obj));
      gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_sel_handler);
      gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_unsel_handler);
      geist_document_remove_object(current_doc, obj);
      geist_object_free(obj);
   }
   geist_list_free(list);
   geist_document_render_updates(current_doc);

   D_RETURN(3, TRUE);
}

gboolean
obj_edit_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj;
   geist_list *l, *list;

   D_ENTER(3);

   list = geist_document_get_selected_list(current_doc);
   for (l = list; l; l = l->next)
   {
      obj = GEIST_OBJECT(l->data);
      geist_object_display_props(obj);
   }
   geist_list_free(list);
   D_RETURN(3, TRUE);
}

gboolean
obj_sel_cb(GtkWidget * widget, int row, int column, GdkEventButton * event,
           gpointer * data)
{
   GList *selection;
   geist_object *obj;

   D_ENTER(3);

   obj = (geist_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);
   if (obj)
   {
      geist_object_select(obj);

      selection = GTK_CLIST(widget)->selection;
      if (g_list_length(selection) > 1)
         geist_document_dirty_selection(GEIST_OBJECT_DOC(obj));

      geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   }

   D_RETURN(3, TRUE);
}

gboolean
obj_unsel_cb(GtkWidget * widget, int row, int column, GdkEventButton * event,
             gpointer * data)
{
   GList *selection;
   geist_object *obj;

   D_ENTER(3);

   obj = (geist_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);
   if (obj)
   {
      geist_object_unselect(obj);

      selection = GTK_CLIST(widget)->selection;
      if (g_list_length(selection) > 1)
         geist_document_dirty_selection(GEIST_OBJECT_DOC(obj));
      geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   }

   D_RETURN(3, TRUE);
}

gboolean
obj_addtext_cb(GtkWidget * widget, gpointer * data)
{

   int row;
   geist_object *obj;

   obj =
      GEIST_OBJECT(geist_text_new_with_text
                   (50, 50, "cinema.ttf", 12, "New Text", 50, 50, 255, 0));
   geist_document_add_object(current_doc, obj);

   row = gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
   if (row != -1)
      gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);

   geist_object_display_props(obj);
   geist_document_render_updates(current_doc);

   return TRUE;
}



gboolean
obj_addrect_cb(GtkWidget * widget, gpointer * data)
{

   int row;
   geist_object *obj;

   obj = GEIST_OBJECT(geist_rect_new_of_size(50, 50, 50, 50, 255, 0, 0, 0));
   geist_document_add_object(current_doc, obj);

   row = gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
   if (row != -1)
      gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);

   geist_object_display_props(obj);
   geist_document_render_updates(current_doc);

   return TRUE;
}

gboolean
menu_cb(GtkWidget * widget, gpointer * data)
{
   char *item;

   D_ENTER(3);

   item = (char *) data;

   if (!strcmp(item, "save doc as"))
      geist_document_save_as(current_doc);
   else
      printf("IMPLEMENT ME!\n");

   D_RETURN(3, TRUE);
}

