/* geist_interface.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "geist.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_document_gtk.h"
#include "geist_text.h"
#include "geist_poly.h"
#include "geist_rect.h"
#include "geist_layer.h"
#include "geist_document_xml.h"
#include "geist_gtk_menu.h"
#include "geist_interface.h"

geist_document *current_doc;
GtkWidget *statusbar;

int obj_props_active = 0;
int doc_props_active = 0;

/*doc props widgets*/
GtkWidget *doc_props_window;
GtkWidget *doc_hbox;
GtkWidget *doc_name;
GtkWidget *doc_file_name;
GtkWidget *cr, *cg, *cb, *ca;
GtkWidget *w_wid, *h_wid;

/*generic obj props widgets*/
GtkWidget *obj_props_window, *obj_hbox;
GtkWidget *gen_props;
GtkWidget *table;
GtkWidget *name;
GtkWidget *sizemode_combo;
GtkWidget *alignment_combo;
GtkWidget *vis_toggle;
GtkWidget *rotation;

void refresh_name_cb(GtkWidget * widget, gpointer * obj);
void refresh_sizemode_cb(GtkWidget * widget, gpointer * obj);
void refresh_alignment_cb(GtkWidget * widget, gpointer * obj);
void rotation_cb(GtkWidget * widget, gpointer * obj);
void geist_update_statusbar(geist_document * doc);
void geist_update_document_props_window(void);
void geist_update_obj_props_window(void);
gboolean obj_addpoly_cb(GtkWidget * widget, gpointer * data);
gboolean objwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                          gpointer user_data);
gboolean objwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer user_data);
gboolean docwin_enter_cb(GtkWidget * widget, GdkEvent * event,

                         gpointer user_data);

typedef struct _geist_confirmation_dialog_data
{
   GtkWidget *dialog;
   gboolean value;
   GMainLoop *loop;
}
geist_confirmation_dialog_data;

gboolean geist_confirmation_dialog_new_with_text(char *text);


char *object_types[] = {
   "None",
   "Image",
   "Text",
   "Rect",
   "Line",
   "Poly",
   "XXXXX"
};

char *object_sizemodes[] = {
   "None",
   "Zoom",
   "Stretch",
   "XXXXX"
};

char *object_alignments[] = {
   "None",
   "Center Horizontal",
   "Center Vertical",
   "Center Both",
   "Left",
   "Right",
   "Top",
   "Bottom",
   "XXXXX"
};


void
geist_clear_statusbar(void)
{
   gint contextid;

   contextid =
      gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "props");
   gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid,
                      "[No object selected]");
}

void
geist_update_statusbar(geist_document * doc)
{
   geist_object *obj;
   gib_list *list;
   char buff[35];
   gint contextid;

   contextid =
      gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "props");
   list = geist_document_get_selected_list(doc);
   if (list)
   {
      if (gib_list_has_more_than_one_item(list))
      {
         gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid,
                            "[Multiple selection]");
      }
      else
      {
         obj = list->data;
         g_snprintf(buff, 35, "X:%d | Y:%d | W:%d | H:%d", obj->x, obj->y,
                    obj->w, obj->h);
         gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid, buff);
      }
   }
   else
      gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid,
                         "[No object selected]");
}


GtkWidget *
geist_create_object_list(void)
{
   GtkWidget *obj_table, *obj_btn, *obj_btn_hbox, *obj_scroll, *wvbox;
   GtkWidget *menubar, *menu, *menuitem;

   D_ENTER(3);
   obj_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_signal_connect(GTK_OBJECT(obj_win), "delete_event",
                      GTK_SIGNAL_FUNC(objwin_delete_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(obj_win), "destroy_event",
                      GTK_SIGNAL_FUNC(objwin_destroy_cb), NULL);
   wvbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(obj_win), wvbox);

   /* menus */
   tooltips = gtk_tooltips_new();

   menubar = gtk_menu_bar_new();
   gtk_widget_show(menubar);
   gtk_box_pack_start(GTK_BOX(wvbox), menubar, FALSE, FALSE, 0);

   menu = geist_gtk_create_submenu(menubar, "File");

   menuitem =
      geist_gtk_create_menuitem(menu, "New...", "", "New Document",
                                (GtkFunction) menu_cb, "new doc");
   menuitem =
      geist_gtk_create_menuitem(menu, "Open...", "", "Open Document",
                                (GtkFunction) menu_cb, "open doc");
   menuitem =
      geist_gtk_create_menuitem(menu, "Save", "", "Save Document",
                                (GtkFunction) menu_cb, "save doc");
   menuitem =
      geist_gtk_create_menuitem(menu, "Save as...", "", "Save Document As...",
                                (GtkFunction) menu_cb, "save doc as");

   menu = geist_gtk_create_submenu(menubar, "Add");

   menuitem =
      geist_gtk_create_menuitem(menu, "image...", "", "Add Image",
                                (GtkFunction) obj_imageadd_cb, NULL);
   menuitem =
      geist_gtk_create_menuitem(menu, "rect...", "", "Add Rectangle",
                                (GtkFunction) obj_addrect_cb, NULL);
   menuitem =
      geist_gtk_create_menuitem(menu, "text...", "", "Add Text",
                                (GtkFunction) obj_addtext_cb, NULL);
   menuitem =
      geist_gtk_create_menuitem(menu, "line...", "", "Add Line",
                                (GtkFunction) obj_addline_cb, NULL);
   menuitem =
      geist_gtk_create_menuitem(menu, "poly...", "", "Add Polygon",
                                (GtkFunction) obj_addpoly_cb, NULL);

   menu = geist_gtk_create_submenu(menubar, "Dialogs");

   menuitem =
      geist_gtk_create_menuitem(menu, "show object properties", "",
                                "Show object properties",
                                (GtkFunction) geist_display_obj_props_window,
                                NULL);
   menuitem =
      geist_gtk_create_menuitem(menu, "show document properties", "",
                                "Show document properties",
                                (GtkFunction)
                                geist_display_document_props_window, NULL);

   obj_table = gtk_table_new(3, 4, FALSE);
   gtk_container_set_border_width(GTK_CONTAINER(obj_win), 3);
   gtk_box_pack_start(GTK_BOX(wvbox), obj_table, TRUE, TRUE, 0);
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
                      GTK_SIGNAL_FUNC(obj_imageadd_cb), NULL);
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
   obj_btn = gtk_button_new_with_label("Add Line...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_addline_cb), NULL);
   gtk_box_pack_start(GTK_BOX(obj_btn_hbox), obj_btn, TRUE, TRUE, 2);
   gtk_widget_show(obj_btn);
   obj_btn = gtk_button_new_with_label("Add Poly...");
   gtk_signal_connect(GTK_OBJECT(obj_btn), "clicked",
                      GTK_SIGNAL_FUNC(obj_addpoly_cb), NULL);
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
   gtk_table_attach(GTK_TABLE(obj_table), obj_btn_hbox, 0, 3, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(obj_btn_hbox);

   /*statusbar */
   statusbar = gtk_statusbar_new();
   gtk_box_pack_end(GTK_BOX(wvbox), statusbar, FALSE, FALSE, 0);
   gtk_widget_show(statusbar);

   gtk_window_set_default_size(GTK_WINDOW(obj_win), 125, 230);
   gtk_widget_show(obj_scroll);
   gtk_widget_show(obj_table);
   gtk_widget_show(wvbox);
   gtk_widget_show(obj_list);
   D_RETURN(3, obj_win);
}



GtkWidget *
geist_gtk_new_document_window(geist_document * doc)
{
   GtkWidget *hwid, *vwid, *darea, *viewport;
   GtkWidget *scrollwin;
   GtkWidget *mvbox;
   GtkWidget *mainwin;

   D_ENTER(3);
   mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_policy(GTK_WINDOW(mainwin), TRUE, TRUE, TRUE);
   gtk_window_set_wmclass(GTK_WINDOW(mainwin), "geist", "geist");
   gtk_signal_connect(GTK_OBJECT(mainwin), "delete_event",
                      GTK_SIGNAL_FUNC(docwin_delete_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(mainwin), "destroy_event",
                      GTK_SIGNAL_FUNC(docwin_destroy_cb), NULL);
   gtk_signal_connect(GTK_OBJECT(mainwin), "enter_notify_event",
                      GTK_SIGNAL_FUNC(docwin_enter_cb), NULL);
   gtk_object_set_data(GTK_OBJECT(mainwin), "doc", doc);

   mvbox = gtk_vbox_new(FALSE, 0);
   gtk_widget_show(mvbox);
   gtk_container_add(GTK_CONTAINER(mainwin), mvbox);

   hwid = gtk_hbox_new(TRUE, 0);
   gtk_widget_show(hwid);
   gtk_box_pack_start(GTK_BOX(mvbox), hwid, TRUE, TRUE, 0);

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
   darea = gtk_drawing_area_new();
   gtk_widget_set_events(darea,
                         GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
                         GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                         GDK_POINTER_MOTION_MASK |
                         GDK_POINTER_MOTION_HINT_MASK);
   gtk_object_set_data(GTK_OBJECT(darea), "doc", doc);
   doc->darea = darea;
   doc->scrollwin = scrollwin;
   doc->win = mainwin;

   gtk_signal_connect(GTK_OBJECT(darea), "button_press_event",
                      GTK_SIGNAL_FUNC(evbox_buttonpress_cb), doc);
   gtk_signal_connect(GTK_OBJECT(darea), "button_release_event",
                      GTK_SIGNAL_FUNC(evbox_buttonrelease_cb), doc);
   gtk_signal_connect(GTK_OBJECT(darea), "motion_notify_event",
                      GTK_SIGNAL_FUNC(evbox_mousemove_cb), doc);
   gtk_container_add(GTK_CONTAINER(viewport), darea);
   gtk_signal_connect_after(GTK_OBJECT(darea), "configure_event",
                            GTK_SIGNAL_FUNC(configure_cb), doc);
   gtk_widget_show(darea);
   current_doc = doc;
   geist_document_reset_object_list(doc);
   geist_update_document_props_window();

   D_RETURN(3, mainwin);
}

gboolean docwin_delete_cb(GtkWidget * widget, GdkEvent * event,
                          gpointer user_data)
{
   geist_document *doc;

   D_ENTER(3);

   doc = gtk_object_get_data(GTK_OBJECT(widget), "doc");
   if (doc)
      geist_document_free(doc);
   geist_document_reset_object_list(NULL);
   geist_clear_obj_props_window();
   geist_clear_document_props_window();

   D_RETURN(3, FALSE);
}

gboolean docwin_destroy_cb(GtkWidget * widget, GdkEvent * event,
                           gpointer user_data)
{
   geist_document *doc;

   D_ENTER(3);
   doc = gtk_object_get_data(GTK_OBJECT(widget), "doc");
   if (doc)
      geist_document_free(doc);
   geist_document_reset_object_list(NULL);
   geist_clear_obj_props_window();
   geist_clear_document_props_window();

   D_RETURN(3, FALSE);
}

gboolean docwin_enter_cb(GtkWidget * widget, GdkEvent * event,
                         gpointer user_data)
{
   geist_document *doc;

   D_ENTER(3);
   doc = gtk_object_get_data(GTK_OBJECT(widget), "doc");
   if (doc != current_doc)
   {
      current_doc = doc;
      geist_document_reset_object_list(doc);
      geist_update_obj_props_window();
      geist_update_document_props_window();
      geist_update_props_window();
   }
   D_RETURN(3, FALSE);
}


gboolean objwin_delete_cb(GtkWidget * widget, GdkEvent * event,
                          gpointer user_data)
{
   D_ENTER(3);
   gtk_exit(0);
   D_RETURN(3, FALSE);
}

gboolean objwin_destroy_cb(GtkWidget * widget, GdkEvent * event,
                           gpointer user_data)
{
   D_ENTER(3);
   gtk_exit(0);
   D_RETURN(3, FALSE);
}

gboolean configure_cb(GtkWidget * widget, GdkEventConfigure * event,
                      gpointer user_data)
{

   D_ENTER(3);
#if 0
   geist_document *doc;

   doc = GEIST_DOCUMENT(user_data);
   if (doc)
      geist_document_render_to_window(doc);
#endif
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
      gib_list *l, *list;
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
            gtk_object_set_data_full(GTK_OBJECT(widget), "resizelist", list,
                                     NULL);
            geist_document_render_updates(doc, 1);
            D_RETURN(5, 1);
         }
         gib_list_free(list);
      }

      obj = geist_document_find_clicked_object(doc, event->x, event->y);

      gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_sel_handler);
      gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_unsel_handler);

      if (event->state & GDK_SHIFT_MASK)
      {
         if (obj)
         {
            /* shift click - multiple selections and selection toggling */
            row =
               gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                            (gpointer) obj);
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
            geist_document_dirty_selection(doc);
            geist_update_props_window();
         }
         else           /* no object clicked selection rect will be drawn */
         {
            doc->select_x = event->x;
            doc->select_y = event->y;
         }
      }
      else              /*single click */
      {
         if (!obj)
         {
            geist_document_unselect_all(doc);
            geist_clear_statusbar();
            geist_clear_obj_props_window();

            doc->select_x = event->x;
            doc->select_y = event->y;
         }
         else           /*we clicked an object */
         {
            if (!geist_object_get_state(obj, SELECTED))
            {
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
            gtk_object_set_data_full(GTK_OBJECT(widget), "draglist", list,
                                     NULL);
         }
         geist_document_render_updates(doc, 1);
         geist_update_props_window();
         geist_update_statusbar(doc);
         gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_sel_handler);
         gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_unsel_handler);
      }
   }
   D_RETURN(5, 1);
}

gint evbox_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event,
                            gpointer user_data)
{
   gib_list *list, *l;
   geist_object *obj;
   geist_document *doc;
   int dragged = 0;
   int resized = 0;

   D_ENTER(5);
   doc = GEIST_DOCUMENT(user_data);
   if (!doc)
      D_RETURN(5, 1);
   list = gtk_object_get_data(GTK_OBJECT(widget), "draglist");
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         D(2, ("unsetting object state DRAG\n"));
         geist_object_unset_state(obj, DRAG);
         /* not being dragged, don't need these */
         obj->clicked_x = obj->clicked_y = 0;
         geist_object_dirty(obj);
      }
      gib_list_free(list);
      gtk_object_set_data_full(GTK_OBJECT(widget), "draglist", NULL, NULL);
      dragged = 1;
   }
   else if ((list = gtk_object_get_data(GTK_OBJECT(widget), "resizelist")))
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         D(2, ("unsetting object state RESIZE\n"));
         geist_object_unset_state(obj, RESIZE);
         obj->resize = RESIZE_NONE;
         /* not being dragged, don't need these */
         obj->clicked_x = obj->clicked_y = 0;
         geist_object_dirty(obj);
      }
      gib_list_free(list);
      gtk_object_set_data_full(GTK_OBJECT(widget), "resizelist", NULL, NULL);
      resized = 1;
   }
   else if (doc->select_x && doc->select_y && !resized && !dragged)
   {
      int x, y, w, h;
      int row;
      gib_list *l, *ll;

      /* this is a bit hackish: we didnt resize or drag, so no object was
       * clicked. If there are updates now, they're the boundaries of the 
       * selection rect so we get the rect in which to look for objects to
       * select from them. Spares having to figure out the direction of the
       * rect again. */

      if (doc->up)
      {
         imlib_updates_get_coordinates(doc->up, &x, &y, &w, &h);


         /*select all in rect */
         for (l = doc->layers; l; l = l->next)
         {
            for (ll = GEIST_LAYER(l->data)->objects; ll; ll = ll->next)
            {
               geist_object *obj;

               obj = GEIST_OBJECT(ll->data);

               if (RECTS_INTERSECT
                   (x, y, w, h, obj->x, obj->y, obj->w, obj->h))
               {
                  /*toggle_selection */
                  gtk_signal_handler_block(GTK_OBJECT(obj_list),
                                           obj_sel_handler);
                  gtk_signal_handler_block(GTK_OBJECT(obj_list),
                                           obj_unsel_handler);

                  row =
                     gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                                  (gpointer) obj);
                  if (geist_object_get_state(obj, SELECTED))
                  {
                     geist_object_unset_state(obj, SELECTED);
                     if (row != -1)
                        gtk_clist_unselect_row(GTK_CLIST(obj_list), row, 0);
                     geist_object_dirty(obj);
                  }
                  else
                  {
                     geist_object_set_state(obj, SELECTED);
                     if (row != -1)
                        gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
                  }
                  gtk_signal_handler_unblock(GTK_OBJECT(obj_list),
                                             obj_sel_handler);
                  gtk_signal_handler_unblock(GTK_OBJECT(obj_list),
                                             obj_unsel_handler);

               }
            }
            /* need to dirty all the other selected objects, in case they go from
             * being the only thing selected to part of a multiple selection -
             * hence have to change their selection type */
            geist_document_dirty_selection(doc);
            geist_update_props_window();

         }
      }
      doc->select_x = doc->select_y = doc->old_select_w = doc->old_select_h = 0;
   }
   geist_document_render_updates(doc, 1);
   D_RETURN(5, 1);
}

gint evbox_mousemove_cb(GtkWidget * widget, GdkEventMotion * event,
                        gpointer user_data)
{
   gib_list *l, *list;
   geist_object *obj;
   geist_document *doc;
   int x, y;

   GdkModifierType state;

   D_ENTER(5);
   doc = GEIST_DOCUMENT(user_data);
   if (!doc)
      D_RETURN(5, 1);
   /* use hinted motionnotify to prevent queue backlog */
   if (event->is_hint)
      gdk_window_get_pointer(event->window, &x, &y, &state);
   else
   {
      x = event->x;
      y = event->y;
      state = event->state;
   }

   list = gtk_object_get_data(GTK_OBJECT(widget), "draglist");
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         D(5, ("moving object to %d, %d\n", x, y));
         geist_object_move(obj, x, y);
      }
      geist_update_statusbar(doc);
      geist_document_render_updates(doc, 1);
   }
   else if ((list = gtk_object_get_data(GTK_OBJECT(widget), "resizelist")))
   {
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         D(5, ("resizing object\n"));
         geist_object_resize(obj, x, y);
      }
      geist_update_statusbar(doc);
      geist_document_render_updates(doc, 1);
   }
   else
   {

      if (doc->select_x && doc->select_y)	/*we need to draw a select rect */
      {
         int rect_x, rect_y, w, h;

         /*repaint area where the last select rect was */
         geist_document_render_updates(doc, 0);

         /*figure out the direction */
         if (x >= doc->select_x)
         {
            rect_x = doc->select_x;
            w = x - doc->select_x;
         }
         else
         {
            rect_x = x;
            w = doc->select_x - x;
         }

         if (y >= doc->select_y)
         {
            rect_y = doc->select_y;
            h = y - doc->select_y;
         }
         else
         {
            rect_y = y;
            h = doc->select_y - y;
         }

         /*draw selection rect */
         gib_imlib_image_fill_rectangle(doc->im, rect_x, rect_y, w, h, 170,
                                          210, 170, 120);
         gib_imlib_image_draw_rectangle(doc->im, rect_x, rect_y, w, h, 0,
                                          0, 0, 255);

         /*render on top of everything else */
         geist_document_render_pmap_partial(doc, rect_x, rect_y, w, h);
         /* clear old rect */
         if(doc->old_select_w != 0)
         geist_document_render_to_window_partial(doc, doc->old_select_x,
                                                 doc->old_select_y, doc->old_select_w,
                                                 doc->old_select_h);
         /* clear new rect */
         geist_document_render_to_window_partial(doc, rect_x, rect_y, w, h);

         /*dirty the select rect area for the next time */
         doc->up = imlib_update_append_rect(doc->up, rect_x, rect_y, w, h);

         doc->old_select_w = w;
         doc->old_select_h = h;
         doc->old_select_x = rect_x;
         doc->old_select_y = rect_y;
      }
   }

   D_RETURN(5, 1);
}

gboolean obj_load_cb(GtkWidget * widget, gpointer data)
{
   geist_object *obj = NULL;
   char *path;

   int row;

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
         geist_document_unselect_all(current_doc);
         row =
            gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
         if (row != -1)
            gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
         geist_document_render_updates(current_doc, 1);
      }
   }
   gtk_widget_destroy((GtkWidget *) data);
   D_RETURN(3, TRUE);
}

gboolean obj_load_cancel_cb(GtkWidget * widget, gpointer data)
{
   gtk_widget_destroy((GtkWidget *) data);
   return TRUE;
}

gboolean obj_imageadd_cb(GtkWidget * widget, gpointer * data)
{
   if (doc_list)
   {
      GtkWidget *file_sel = gtk_file_selection_new("Add an Image");

      gtk_file_selection_show_fileop_buttons(GTK_FILE_SELECTION(file_sel));
      gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->ok_button),
                         "clicked", GTK_SIGNAL_FUNC(obj_load_cb),
                         (gpointer) file_sel);
      gtk_signal_connect(GTK_OBJECT
                         (GTK_FILE_SELECTION(file_sel)->cancel_button),
                         "clicked", GTK_SIGNAL_FUNC(obj_load_cancel_cb),
                         (gpointer) file_sel);
      gtk_widget_show(file_sel);
   }
   return TRUE;
}


gboolean obj_cpy_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *new;
   gib_list *l, *list;
   geist_object *obj;
   int row;

   D_ENTER(3);
   if (doc_list)
   {
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
               gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                            (gpointer) obj);
            if (row != -1)
               gtk_clist_unselect_row(GTK_CLIST(obj_list), row, 0);
            row =
               gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                            (gpointer) new);
            if (row != -1)
               gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
         }
      }
      gib_list_free(list);
      geist_document_render_updates(current_doc, 1);
   }
   D_RETURN(3, TRUE);
}

gboolean obj_del_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj;
   gib_list *l, *list;

   D_ENTER(3);
   if (doc_list)
   {
      list = geist_document_get_selected_list(current_doc);
      for (l = list; l; l = l->next)
      {
         obj = GEIST_OBJECT(l->data);
         gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_sel_handler);
         gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_unsel_handler);
         if (obj_props_active)
            geist_clear_obj_props_window();
         geist_clear_statusbar();
         gtk_clist_remove(GTK_CLIST(obj_list),
                          gtk_clist_find_row_from_data(GTK_CLIST(obj_list),
                                                       obj));
         gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_sel_handler);
         gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_unsel_handler);
         geist_document_remove_object(current_doc, obj);
         geist_object_free(obj);
      }
      gib_list_free(list);
      geist_document_render_updates(current_doc, 1);
   }
   D_RETURN(3, TRUE);
}


gboolean obj_sel_cb(GtkWidget * widget, int row, int column,
                    GdkEventButton * event, gpointer * data)
{
   GList *selection;
   geist_object *obj;

   D_ENTER(3);
   obj = (geist_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);
   if (obj)
   {
      geist_object_select(obj);
      geist_update_props_window();
      geist_update_statusbar(GEIST_OBJECT_DOC(obj));
      selection = GTK_CLIST(widget)->selection;
      if (g_list_length(selection) > 1)
         geist_document_dirty_selection(GEIST_OBJECT_DOC(obj));
      geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   }

   D_RETURN(3, TRUE);
}

gboolean obj_unsel_cb(GtkWidget * widget, int row, int column,
                      GdkEventButton * event, gpointer * data)
{
   GList *selection;
   geist_object *obj;

   D_ENTER(3);
   obj = (geist_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);
   if (obj)
   {
      geist_object_unselect(obj);
      geist_update_props_window();
      geist_update_statusbar(GEIST_OBJECT_DOC(obj));
      selection = GTK_CLIST(widget)->selection;
      if (g_list_length(selection) > 1)
         geist_document_dirty_selection(GEIST_OBJECT_DOC(obj));
      geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   }

   D_RETURN(3, TRUE);
}

gboolean obj_addtext_cb(GtkWidget * widget, gpointer * data)
{

   int row;

   geist_object *obj;

   D_ENTER(3);
   if (doc_list)
   {
      obj =
         GEIST_OBJECT(geist_text_new_with_text
                      (50, 50, "20thcent", 12, "New Text", JUST_LEFT, TRUE,
                       255, 40, 255, 0));
      geist_document_add_object(current_doc, obj);
      row = gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
      geist_document_unselect_all(current_doc);
      if (row != -1)
         gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
      geist_document_render_updates(current_doc, 1);
   }
   D_RETURN(3, TRUE);
}



gboolean obj_addrect_cb(GtkWidget * widget, gpointer * data)
{

   int row;

   geist_object *obj;

   D_ENTER(3);
   if (doc_list)
   {
      obj =
         GEIST_OBJECT(geist_rect_new_of_size(50, 50, 50, 50, 255, 0, 0, 0));
      geist_document_add_object(current_doc, obj);
      row = gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
      geist_document_unselect_all(current_doc);
      if (row != -1)
         gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
      geist_document_render_updates(current_doc, 1);
   }
   D_RETURN(3, TRUE);
}

gboolean obj_addline_cb(GtkWidget * widget, gpointer * data)
{

   int row;

   geist_object *obj;

   D_ENTER(3);
   if (doc_list)
   {
      obj = geist_line_new_from_to(50, 50, 100, 100, 255, 0, 0, 0);
      geist_document_add_object(current_doc, obj);
      geist_document_unselect_all(current_doc);
      row = gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
      if (row != -1)
         gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
      geist_document_render_updates(current_doc, 1);
   }
   D_RETURN(3, TRUE);
}


gboolean obj_addpoly_cb(GtkWidget * widget, gpointer * data)
{
   int row;
   geist_object *obj;

   D_ENTER(3);
   if (doc_list)
   {
      obj = geist_poly_new();
      obj->x = 5;
      obj->y = 5;
      geist_poly_add_point(GEIST_POLY(obj), 10, 10);
      geist_poly_add_point(GEIST_POLY(obj), 50, 10);
      geist_poly_add_point(GEIST_POLY(obj), 50, 50);
      geist_poly_add_point(GEIST_POLY(obj), 10, 50);
      GEIST_POLY(obj)->r = 128;
      GEIST_POLY(obj)->g = 0;
      GEIST_POLY(obj)->b = 200;
      GEIST_POLY(obj)->a = 255;
      GEIST_POLY(obj)->closed = TRUE;
      geist_document_add_object(current_doc, obj);
      geist_document_unselect_all(current_doc);
      row = gtk_clist_find_row_from_data(GTK_CLIST(obj_list), (gpointer) obj);
      if (row != -1)
         gtk_clist_select_row(GTK_CLIST(obj_list), row, 0);
      geist_document_render_updates(current_doc, 1);
   }
   D_RETURN(3, TRUE);
}



gboolean menu_cb(GtkWidget * widget, gpointer * data)
{
   char *item;
   GtkWidget *doc_win;

   D_ENTER(3);
   item = (char *) data;
   if (!strcmp(item, "save doc"))
   {
      if (doc_list)
      {
         if (current_doc->filename)
            geist_document_save(current_doc, current_doc->filename);
         else
            geist_document_save_as(current_doc);
      }
   }
   else if (!strcmp(item, "save doc as"))
   {
      if (doc_list)
      {
         geist_document_save_as(current_doc);
      }
   }
   else if (!strcmp(item, "new doc"))
   {
      geist_document *doc = geist_document_new(500, 500);

      doc_win = geist_gtk_new_document_window(doc);
      geist_document_render_full(doc, 1);
      gtk_widget_show(doc_win);
   }
   else if (!strcmp(item, "open doc"))
      geist_document_load();
   else
      printf("IMPLEMENT ME!\n");
   D_RETURN(3, TRUE);
}

static void
obj_vis_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *l = NULL;
   gib_list *list = NULL;

   D_ENTER(3);
   list = geist_document_get_selected_list(current_doc);
   if (gib_list_length(list) > 1)
   {
      for (l = list; l; l = l->next)
      {
         obj = l->data;
         if (geist_object_get_state(obj, VISIBLE))
            geist_object_hide(obj);
         else
            geist_object_show(obj);
      }
   }
   else
   {
      obj = list->data;
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
         geist_object_show(obj);
      else
         geist_object_hide(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   gib_list_free(list);
   D_RETURN_(3);
}

void
refresh_name_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *list = NULL;

   list = geist_document_get_selected_list(current_doc);
   D_ENTER(3);
   if (gib_list_length(list) > 1)
      printf("Implement me!\n");
   else
   {
      obj = list->data;
      if (obj->name)
         efree(obj->name);
      obj->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
   }
   gib_list_free(list);
   D_RETURN_(3);
}

void
refresh_sizemode_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *l = NULL;
   gib_list *list = NULL;

   list = geist_document_get_selected_list(current_doc);
   D_ENTER(3);
   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->sizemode =
         geist_object_get_sizemode_from_string(gtk_entry_get_text
                                               (GTK_ENTRY(widget)));
      geist_object_update_positioning(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   gib_list_free(list);
   D_RETURN_(3);
}


void
refresh_alignment_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *l = NULL;
   gib_list *list = NULL;

   list = geist_document_get_selected_list(current_doc);
   D_ENTER(3);
   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->alignment =
         geist_object_get_alignment_from_string(gtk_entry_get_text
                                                (GTK_ENTRY(widget)));
      geist_object_update_positioning(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   gib_list_free(list);
   D_RETURN_(3);
}


void
buttons_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *l = NULL;
   gib_list *list = NULL;

   D_ENTER(3);
   list = geist_document_get_selected_list(current_doc);
   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      switch (GPOINTER_TO_INT(data))
      {
        case 1:
           geist_object_update_position_relative(obj, 0, -1);
           break;
        case 2:
           geist_object_update_position_relative(obj, 0, +1);
           break;
        case 3:
           geist_object_update_position_relative(obj, -1, 0);
           break;
        case 4:
           geist_object_update_position_relative(obj, +1, 0);
           break;
        case 5:
           geist_object_update_dimensions_relative(obj, 0, +1);
           break;
        case 6:
           geist_object_update_dimensions_relative(obj, 0, -1);
           break;
        case 7:
           geist_object_update_dimensions_relative(obj, +1, 0);
           break;
        case 8:
           geist_object_update_dimensions_relative(obj, -1, 0);
           break;
        default:
           break;
      }
      geist_object_dirty(obj);
   }
   geist_update_statusbar(current_doc);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   gib_list_free(list);
   D_RETURN_(3);
}

void
rotation_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *l = NULL;
   gib_list *list = NULL;
   double angle;

   list = geist_document_get_selected_list(current_doc);
   D_ENTER(3);

   angle = GPOINTER_TO_INT(data);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      geist_object_rotate(obj, angle);
      geist_object_dirty(obj);
   }
   geist_update_statusbar(current_doc);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj), 1);
   gib_list_free(list);
   D_RETURN_(3);

}

void
geist_display_obj_props_window(void)
{
   GtkWidget *gen_table, *name_l;
   GtkWidget *sizemode_l;
   GtkWidget *alignment_l;
   GtkWidget *rot_l, *rot_clockwise_btn, *rot_counterclockwise_btn,
      *rot_45_clockwise_btn, *rot_45_counterclockwise_btn;
   GtkWidget *up, *down, *left, *right, *width_plus, *width_minus,
      *height_plus, *height_minus;
   GList *align_list = g_list_alloc();
   GList *sizemode_list = g_list_alloc();
   int i;

   D_ENTER(3);
   if (doc_list)
   {
      if (obj_props_active)
	   D_RETURN_(3);

      obj_props_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      table = gtk_table_new(2, 4, FALSE);
      gtk_container_set_border_width(GTK_CONTAINER(obj_props_window), 5);
      gtk_container_add(GTK_CONTAINER(obj_props_window), table);
      gtk_signal_connect(GTK_OBJECT(obj_props_window), "delete_event",
                         GTK_SIGNAL_FUNC(geist_hide_obj_props_window), NULL);
      gen_props = gtk_hbox_new(FALSE, 0);
      gtk_table_attach(GTK_TABLE(table), gen_props, 0, 4, 0, 1,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gen_table = gtk_table_new(8, 6, FALSE);
      gtk_container_set_border_width(GTK_CONTAINER(gen_props), 5);
      gtk_container_add(GTK_CONTAINER(gen_props), gen_table);
      vis_toggle = gtk_check_button_new_with_label("Visible");
      gtk_table_attach(GTK_TABLE(gen_table), vis_toggle, 0, 2, 0, 1,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(vis_toggle);

      name_l = gtk_label_new("Name:");
      gtk_table_attach(GTK_TABLE(gen_table), name_l, 0, 1, 1, 2,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(name_l);
      name = gtk_entry_new();
      gtk_table_attach(GTK_TABLE(gen_table), name, 1, 6, 1, 2,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(name);
      sizemode_l = gtk_label_new("sizemode");
      gtk_table_attach(GTK_TABLE(gen_table), sizemode_l, 0, 1, 2, 3,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(sizemode_l);
      sizemode_combo = gtk_combo_new();
      gtk_table_attach(GTK_TABLE(gen_table), sizemode_combo, 1, 6, 2, 3,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_container_set_border_width(GTK_CONTAINER(sizemode_combo), 5);
      gtk_widget_show(sizemode_combo);
      alignment_l = gtk_label_new("alignment");
      gtk_table_attach(GTK_TABLE(gen_table), alignment_l, 0, 1, 3, 4,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(alignment_l);
      alignment_combo = gtk_combo_new();
      gtk_table_attach(GTK_TABLE(gen_table), alignment_combo, 1, 6, 3, 4,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_container_set_border_width(GTK_CONTAINER(alignment_combo), 5);
      gtk_widget_show(alignment_combo);
      up = gtk_button_new_with_label("Up");
      gtk_table_attach(GTK_TABLE(gen_table), up, 1, 2, 4, 5,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(up);
      down = gtk_button_new_with_label("Down");
      gtk_table_attach(GTK_TABLE(gen_table), down, 1, 2, 6, 7,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(down);
      left = gtk_button_new_with_label("Left");
      gtk_table_attach(GTK_TABLE(gen_table), left, 0, 1, 5, 6,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(left);
      right = gtk_button_new_with_label("Right");
      gtk_table_attach(GTK_TABLE(gen_table), right, 2, 3, 5, 6,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(right);
      width_plus = gtk_button_new_with_label("Width +");
      gtk_table_attach(GTK_TABLE(gen_table), width_plus, 5, 6, 5, 6,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(width_plus);
      width_minus = gtk_button_new_with_label("Width -");
      gtk_table_attach(GTK_TABLE(gen_table), width_minus, 3, 4, 5, 6,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(width_minus);
      height_plus = gtk_button_new_with_label("Height +");
      gtk_table_attach(GTK_TABLE(gen_table), height_plus, 4, 5, 4, 5,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(height_plus);
      height_minus = gtk_button_new_with_label("Height -");
      gtk_table_attach(GTK_TABLE(gen_table), height_minus, 4, 5, 6, 7,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(height_minus);


      rot_l = gtk_label_new("Rotation:");
      gtk_table_attach(GTK_TABLE(gen_table), rot_l, 0, 1, 7, 8,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(rot_l);

      rot_45_counterclockwise_btn = gtk_button_new_with_label("ccw 45");
      gtk_table_attach(GTK_TABLE(gen_table), rot_45_counterclockwise_btn, 1,
                       2, 7, 8, GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(rot_45_counterclockwise_btn);

      rot_counterclockwise_btn = gtk_button_new_with_label("ccw");
      gtk_table_attach(GTK_TABLE(gen_table), rot_counterclockwise_btn, 2, 3,
                       7, 8, GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(rot_counterclockwise_btn);

      rot_clockwise_btn = gtk_button_new_with_label("clockwise");
      gtk_table_attach(GTK_TABLE(gen_table), rot_clockwise_btn, 3, 4, 7, 8,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(rot_clockwise_btn);

      rot_45_clockwise_btn = gtk_button_new_with_label("clockwise 45");
      gtk_table_attach(GTK_TABLE(gen_table), rot_45_clockwise_btn, 4, 5, 7, 8,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(rot_45_clockwise_btn);

      for (i = 0; i < ALIGN_MAX; i++)
      {
         align_list = g_list_append(align_list, object_alignments[i]);
      }

      gtk_combo_set_popdown_strings(GTK_COMBO(alignment_combo), align_list);
      for (i = 0; i < SIZEMODE_MAX; i++)
      {
         align_list = g_list_append(sizemode_list, object_sizemodes[i]);
      }

      gtk_combo_set_popdown_strings(GTK_COMBO(sizemode_combo), sizemode_list);
      gtk_signal_connect(GTK_OBJECT(vis_toggle), "clicked",
                         GTK_SIGNAL_FUNC(obj_vis_cb), NULL);
      gtk_signal_connect(GTK_OBJECT(up), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 1);
      gtk_signal_connect(GTK_OBJECT(down), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 2);
      gtk_signal_connect(GTK_OBJECT(right), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 4);
      gtk_signal_connect(GTK_OBJECT(left), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 3);
      gtk_signal_connect(GTK_OBJECT(height_plus), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 5);
      gtk_signal_connect(GTK_OBJECT(height_minus), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 6);
      gtk_signal_connect(GTK_OBJECT(width_plus), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 7);
      gtk_signal_connect(GTK_OBJECT(width_minus), "clicked",
                         GTK_SIGNAL_FUNC(buttons_cb), (gpointer) 8);
      gtk_signal_connect(GTK_OBJECT(name), "changed",
                         GTK_SIGNAL_FUNC(refresh_name_cb), NULL);
      gtk_signal_connect(GTK_OBJECT(GTK_COMBO(alignment_combo)->entry),
                         "changed", GTK_SIGNAL_FUNC(refresh_alignment_cb),
                         NULL);
      gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sizemode_combo)->entry),
                         "changed", GTK_SIGNAL_FUNC(refresh_sizemode_cb),
                         NULL);

      gtk_signal_connect(GTK_OBJECT(rot_45_counterclockwise_btn), "clicked",
                         GTK_SIGNAL_FUNC(rotation_cb), (gpointer) - 45);

      gtk_signal_connect(GTK_OBJECT(rot_counterclockwise_btn), "clicked",
                         GTK_SIGNAL_FUNC(rotation_cb), (gpointer) - 1);

      gtk_signal_connect(GTK_OBJECT(rot_clockwise_btn), "clicked",
                         GTK_SIGNAL_FUNC(rotation_cb), (gpointer) 1);

      gtk_signal_connect(GTK_OBJECT(rot_45_clockwise_btn), "clicked",
                         GTK_SIGNAL_FUNC(rotation_cb), (gpointer) 45);

      gtk_widget_show(gen_table);
      gtk_widget_show(gen_props);
      gtk_widget_show(table);
      gtk_widget_show(obj_props_window);
      obj_props_active = 1;
      geist_update_obj_props_window();
   }
   D_RETURN_(3);
}

void
geist_hide_obj_props_window(void)
{
   D_ENTER(3);
   if (obj_props_active)
   {
      gtk_widget_destroy(obj_props_window);
      obj_props_active = 0;
   }
   D_RETURN_(3);
}

void
geist_clear_obj_props_window(void)
{
   if (obj_props_active)
   {
      if (obj_hbox)
      {
         gtk_widget_destroy(obj_hbox);
         obj_hbox = NULL;
      }
      if (gen_props)
         gtk_widget_hide(gen_props);
   }
}

void
geist_update_props_window(void)
{
   D_ENTER(3);
   geist_update_obj_props_window();
   D_RETURN_(3);
}


void
geist_update_obj_props_window(void)
{
   geist_object *obj;
   GtkWidget *new_hbox;
   gib_list *list;
   geist_object *obj_first;
   gib_list *l;
   char *align_string = NULL;

   char *sizemode_string = NULL;

   D_ENTER(3);
   /*only do something if the props window is there */
   if (obj_props_active)
   {
      if (obj_hbox)
      {
         gtk_widget_destroy(obj_hbox);
         obj_hbox = NULL;
      }

      /*show generic part */
      gtk_widget_show(gen_props);
      /*block signal handlers */
      gtk_signal_handler_block_by_func(GTK_OBJECT(vis_toggle),
                                       GTK_SIGNAL_FUNC(obj_vis_cb), NULL);
      gtk_signal_handler_block_by_func(GTK_OBJECT(name), refresh_name_cb,
                                       NULL);
      gtk_signal_handler_block_by_func(GTK_OBJECT
                                       (GTK_COMBO(alignment_combo)->entry),
                                       refresh_alignment_cb, NULL);
      gtk_signal_handler_block_by_func(GTK_OBJECT
                                       (GTK_COMBO(sizemode_combo)->entry),
                                       refresh_sizemode_cb, NULL);
      list = geist_document_get_selected_list(current_doc);
      if (list)
      {
         /*update the values in the generic part */

         if (gib_list_has_more_than_one_item(list))
         {
            /*grey out the name entry box */
            gtk_entry_set_text(GTK_ENTRY(name), "");
            gtk_widget_set_sensitive(GTK_WIDGET(name), FALSE);
            obj_first = list->data;
            /*check wether all objects have the same alignment or sizemode, and if
               so, set the combo boxes, if not leave them empty */
            for (l = list; l; l = l->next)
            {
               obj = l->data;
               if (obj->alignment == obj_first->alignment)
                  align_string = geist_object_get_alignment_string(obj);
               else
                  align_string = "";
               if (obj->sizemode == obj_first->sizemode)
                  sizemode_string = geist_object_get_sizemode_string(obj);
               else
                  sizemode_string = "";
            }

            gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
                               align_string);
            gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
                               sizemode_string);
            gtk_window_set_title(GTK_WINDOW(obj_win), "[multiple selection]");
            gtk_window_set_title(GTK_WINDOW(obj_props_window),
                                 "[multiple selection]");
         }
         else
         {

            gtk_widget_set_sensitive(GTK_WIDGET(name), TRUE);
            obj = list->data;
            if (obj->name)
               gtk_entry_set_text(GTK_ENTRY(name), obj->name);
            gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
                               geist_object_get_alignment_string(obj));
            gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
                               geist_object_get_sizemode_string(obj));
            if (geist_object_get_state(obj, VISIBLE))
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vis_toggle),
                                            TRUE);
            new_hbox = obj->display_props(obj);
            obj_hbox = new_hbox;
            gtk_table_attach(GTK_TABLE(table), obj_hbox, 0, 4, 1, 2,
                             GTK_FILL | GTK_EXPAND, 0, 2, 2);
            gtk_widget_show(new_hbox);
            gtk_window_set_title(GTK_WINDOW(obj_win), obj->name);
            gtk_window_set_title(GTK_WINDOW(obj_props_window), obj->name);
         }
         gib_list_free(list);
      }
      else
         geist_clear_obj_props_window();
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(vis_toggle),
                                         GTK_SIGNAL_FUNC(obj_vis_cb), NULL);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(name), refresh_name_cb,
                                         NULL);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT
                                         (GTK_COMBO(alignment_combo)->entry),
                                         refresh_alignment_cb, NULL);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT
                                         (GTK_COMBO(sizemode_combo)->entry),
                                         refresh_sizemode_cb, NULL);
   }
   D_RETURN_(3);
}

void
refresh_doc_name_cb(GtkWidget * widget, gpointer * data)
{
   D_ENTER(3);
   efree(current_doc->name);
   current_doc->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
   D_RETURN_(3);
}

void
refresh_doc_file_name_cb(GtkWidget * widget, gpointer * data)
{
   D_ENTER(3);
   efree(current_doc->filename);
   current_doc->filename = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
   D_RETURN_(3);
}

static void
refresh_bg_cb(GtkWidget * widget, gpointer * data)
{

   switch (GPOINTER_TO_INT(data))
   {
     case 1:
        current_doc->bg_fill->r =
           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        break;
     case 2:
        current_doc->bg_fill->g =
           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        break;
     case 3:
        current_doc->bg_fill->b =
           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        break;
     case 4:
        current_doc->bg_fill->a =
           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        break;
     default:
        break;
   }

   geist_document_render_full(current_doc, 1);
}

static void
refresh_w_cb(GtkWidget * widget, gpointer * data)
{
   int w, h;

   D_ENTER(3);

   w = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   h = current_doc->h;

   geist_document_resize(current_doc, w, h);
   /*
      while(gtk_events_pending())
      gtk_main_iteration();
    */

   D_RETURN_(3);
}

static void
refresh_h_cb(GtkWidget * widget, gpointer * data)
{
   int w, h;

   D_ENTER(3);

   h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   w = current_doc->w;

   geist_document_resize(current_doc, w, h);

   D_RETURN_(3);
}



void
geist_display_document_props_window(void)
{
   GtkWidget *table, *frame, *hbox;
   GtkWidget *name_l, *file_name_l;
   GtkWidget *cr_l, *cg_l, *cb_l, *ca_l;
   GtkAdjustment *a1, *a2, *a3, *a4, *ah, *aw;
   GtkWidget *w_l, *h_l;

   D_ENTER(3);
   if (doc_list)
   {
      if (doc_props_active)
	   D_RETURN_(3);

      doc_props_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      doc_hbox = gtk_hbox_new(FALSE, 0);
      gtk_container_add(GTK_CONTAINER(doc_props_window), doc_hbox);
      table = gtk_table_new(4, 5, FALSE);
      gtk_container_set_border_width(GTK_CONTAINER(doc_props_window), 5);
      gtk_container_add(GTK_CONTAINER(doc_hbox), table);
      name_l = gtk_label_new("Name:");
      gtk_table_attach(GTK_TABLE(table), name_l, 0, 1, 0, 1,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(name_l);
      doc_name = gtk_entry_new();
      gtk_table_attach(GTK_TABLE(table), doc_name, 1, 4, 0, 1,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(doc_name);
      file_name_l = gtk_label_new("Filename:");
      gtk_table_attach(GTK_TABLE(table), file_name_l, 0, 1, 1, 2,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(file_name_l);
      doc_file_name = gtk_entry_new();
      gtk_table_attach(GTK_TABLE(table), doc_file_name, 1, 4, 1, 2,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(doc_file_name);
      a1 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
      a2 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
      a3 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
      a4 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
      frame = gtk_frame_new("Background Colour");
      hbox = gtk_hbox_new(FALSE, 0);
      gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);
      gtk_container_add(GTK_CONTAINER(frame), hbox);
      gtk_widget_show(frame);
      cr_l = gtk_label_new("R:");
      gtk_misc_set_alignment(GTK_MISC(cr_l), 1.0, 0.5);
      gtk_box_pack_start(GTK_BOX(hbox), cr_l, TRUE, FALSE, 2);
      gtk_widget_show(cr_l);
      cr = gtk_spin_button_new(GTK_ADJUSTMENT(a1), 1, 0);
      gtk_box_pack_start(GTK_BOX(hbox), cr, TRUE, FALSE, 2);
      gtk_widget_show(cr);
      cg_l = gtk_label_new("G:");
      gtk_misc_set_alignment(GTK_MISC(cg_l), 1.0, 0.5);
      gtk_box_pack_start(GTK_BOX(hbox), cg_l, TRUE, FALSE, 2);
      gtk_widget_show(cg_l);
      cg = gtk_spin_button_new(GTK_ADJUSTMENT(a2), 1, 0);
      gtk_box_pack_start(GTK_BOX(hbox), cg, TRUE, FALSE, 2);
      gtk_widget_show(cg);
      cb_l = gtk_label_new("B:");
      gtk_misc_set_alignment(GTK_MISC(cb_l), 1.0, 0.5);
      gtk_box_pack_start(GTK_BOX(hbox), cb_l, TRUE, FALSE, 2);
      gtk_widget_show(cb_l);
      cb = gtk_spin_button_new(GTK_ADJUSTMENT(a3), 1, 0);
      gtk_box_pack_start(GTK_BOX(hbox), cb, TRUE, FALSE, 2);
      gtk_widget_show(cb);
      ca_l = gtk_label_new("A:");
      gtk_misc_set_alignment(GTK_MISC(ca_l), 1.0, 0.5);
      gtk_box_pack_start(GTK_BOX(hbox), ca_l, TRUE, FALSE, 2);
      gtk_widget_show(ca_l);
      ca = gtk_spin_button_new(GTK_ADJUSTMENT(a4), 1, 0);
      gtk_box_pack_start(GTK_BOX(hbox), ca, TRUE, FALSE, 2);
      gtk_widget_show(ca);
      gtk_table_attach(GTK_TABLE(table), frame, 0, 4, 2, 3,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(hbox);

      aw = (GtkAdjustment *) gtk_adjustment_new(0, 1, 6000, 1, 10, 10);
      ah = (GtkAdjustment *) gtk_adjustment_new(0, 1, 6000, 1, 10, 10);
      frame = gtk_frame_new("Document size");
      hbox = gtk_hbox_new(FALSE, 0);
      gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);
      gtk_container_add(GTK_CONTAINER(frame), hbox);
      gtk_widget_show(frame);
      w_l = gtk_label_new("W:");
      gtk_misc_set_alignment(GTK_MISC(w_l), 1.0, 0.5);
      gtk_box_pack_start(GTK_BOX(hbox), w_l, TRUE, FALSE, 2);
      gtk_widget_show(w_l);
      w_wid = gtk_spin_button_new(GTK_ADJUSTMENT(aw), 1, 0);
      gtk_box_pack_start(GTK_BOX(hbox), w_wid, TRUE, FALSE, 2);
      gtk_widget_show(w_wid);
      h_l = gtk_label_new("H:");
      gtk_misc_set_alignment(GTK_MISC(h_l), 1.0, 0.5);
      gtk_box_pack_start(GTK_BOX(hbox), h_l, TRUE, FALSE, 2);
      gtk_widget_show(h_l);
      h_wid = gtk_spin_button_new(GTK_ADJUSTMENT(ah), 1, 0);
      gtk_box_pack_start(GTK_BOX(hbox), h_wid, TRUE, FALSE, 2);
      gtk_widget_show(h_wid);
      gtk_table_attach(GTK_TABLE(table), frame, 0, 4, 3, 4,
                       GTK_FILL | GTK_EXPAND, 0, 2, 2);
      gtk_widget_show(hbox);

      gtk_signal_connect(GTK_OBJECT(doc_props_window), "delete_event",
                         GTK_SIGNAL_FUNC(geist_hide_document_props_window),
                         NULL);
      gtk_signal_connect(GTK_OBJECT(doc_name), "changed",
                         GTK_SIGNAL_FUNC(refresh_doc_name_cb), NULL);
      gtk_signal_connect(GTK_OBJECT(doc_file_name), "changed",
                         GTK_SIGNAL_FUNC(refresh_doc_file_name_cb), NULL);
      gtk_signal_connect(GTK_OBJECT(cr), "changed",
                         GTK_SIGNAL_FUNC(refresh_bg_cb), (gpointer) 1);
      gtk_signal_connect(GTK_OBJECT(cg), "changed",
                         GTK_SIGNAL_FUNC(refresh_bg_cb), (gpointer) 2);
      gtk_signal_connect(GTK_OBJECT(cb), "changed",
                         GTK_SIGNAL_FUNC(refresh_bg_cb), (gpointer) 3);
      gtk_signal_connect(GTK_OBJECT(ca), "changed",
                         GTK_SIGNAL_FUNC(refresh_bg_cb), (gpointer) 4);
      gtk_signal_connect(GTK_OBJECT(w_wid), "changed",
                         GTK_SIGNAL_FUNC(refresh_w_cb), NULL);
      gtk_signal_connect(GTK_OBJECT(h_wid), "changed",
                         GTK_SIGNAL_FUNC(refresh_h_cb), NULL);
      gtk_widget_show(table);
      gtk_widget_show(doc_hbox);
      gtk_widget_show(doc_props_window);
      doc_props_active = 1;
      geist_update_document_props_window();
   }
   D_RETURN_(3);
}

void
geist_clear_document_props_window(void)
{
   D_ENTER(3);
   if (doc_props_active)
      gtk_widget_hide(doc_hbox);

   D_RETURN_(3);
}

void
geist_update_document_props_window(void)
{
   D_ENTER(3);
   if (doc_props_active)
   {
      gtk_widget_show(doc_hbox);

      gtk_signal_handler_block_by_func(GTK_OBJECT(doc_name),
                                       refresh_doc_name_cb, NULL);
      gtk_signal_handler_block_by_func(GTK_OBJECT(doc_file_name),
                                       refresh_doc_file_name_cb, NULL);
      gtk_signal_handler_block_by_func(GTK_OBJECT(cr), refresh_bg_cb,
                                       (gpointer) 1);
      gtk_signal_handler_block_by_func(GTK_OBJECT(cg), refresh_bg_cb,
                                       (gpointer) 2);
      gtk_signal_handler_block_by_func(GTK_OBJECT(cb), refresh_bg_cb,
                                       (gpointer) 3);
      gtk_signal_handler_block_by_func(GTK_OBJECT(ca), refresh_bg_cb,
                                       (gpointer) 4);
      gtk_signal_handler_block_by_func(GTK_OBJECT(w_wid), refresh_w_cb, NULL);
      gtk_signal_handler_block_by_func(GTK_OBJECT(h_wid), refresh_h_cb, NULL);
      if (current_doc->name)
         gtk_entry_set_text(GTK_ENTRY(doc_name), current_doc->name);
      if (current_doc->filename)
         gtk_entry_set_text(GTK_ENTRY(doc_file_name), current_doc->filename);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(cr), current_doc->bg_fill->r);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(cg), current_doc->bg_fill->g);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(cb), current_doc->bg_fill->b);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(ca), current_doc->bg_fill->a);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(w_wid), current_doc->w);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(h_wid), current_doc->h);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(doc_name),
                                         refresh_doc_name_cb, NULL);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(doc_file_name),
                                         refresh_doc_file_name_cb, NULL);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(cr), refresh_bg_cb,
                                         (gpointer) 1);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(cg), refresh_bg_cb,
                                         (gpointer) 2);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(cb), refresh_bg_cb,
                                         (gpointer) 3);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(ca), refresh_bg_cb,
                                         (gpointer) 4);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(w_wid), refresh_w_cb,
                                         NULL);
      gtk_signal_handler_unblock_by_func(GTK_OBJECT(h_wid), refresh_h_cb,
                                         NULL);
   }

   D_RETURN_(3);
}

void
geist_hide_document_props_window(void)
{
   D_ENTER(3);
   gtk_widget_destroy(doc_props_window);
   doc_props_active = 0;
   D_RETURN_(3);
}


void
conf_ok_cb(GtkWidget * widget, gpointer data)
{
   geist_confirmation_dialog_data *dialog =

      (geist_confirmation_dialog_data *) data;
   dialog->value = TRUE;
   gtk_widget_destroy(dialog->dialog);

   g_main_quit(dialog->loop);
}

void
conf_cancel_cb(GtkWidget * widget, gpointer data)
{
   geist_confirmation_dialog_data *dialog =

      (geist_confirmation_dialog_data *) data;
   dialog->value = FALSE;
   gtk_widget_destroy(dialog->dialog);

   g_main_quit(dialog->loop);
}

gboolean geist_confirmation_dialog_new_with_text(char *text)
{
   geist_confirmation_dialog_data *data;
   GMainLoop *loop;
   gboolean ret;
   GtkWidget *dialog, *label, *ok_button, *cancel_button, *table;

   D_ENTER(3);
   data =
      (geist_confirmation_dialog_data *)
      emalloc(sizeof(geist_confirmation_dialog_data));
   dialog = gtk_window_new(GTK_WINDOW_DIALOG);
   table = gtk_table_new(2, 2, TRUE);
   loop = g_main_new(FALSE);
   data->dialog = dialog;
   data->value = TRUE;
   data->loop = loop;
   gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);
   gtk_container_set_border_width(GTK_CONTAINER(table), 5);
   gtk_container_add(GTK_CONTAINER(dialog), table);
   gtk_widget_show(table);
   label = gtk_label_new(text);
   gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 0, 1);
   gtk_widget_show(label);
   ok_button = gtk_button_new_with_label("Ok");
   gtk_table_attach_defaults(GTK_TABLE(table), ok_button, 0, 1, 1, 2);
   gtk_container_set_border_width(GTK_CONTAINER(ok_button), 5);
   gtk_widget_show(ok_button);
   cancel_button = gtk_button_new_with_label("Cancel");
   gtk_table_attach_defaults(GTK_TABLE(table), cancel_button, 1, 2, 1, 2);
   gtk_container_set_border_width(GTK_CONTAINER(cancel_button), 5);
   gtk_widget_show(cancel_button);
   gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                      GTK_SIGNAL_FUNC(conf_ok_cb), (gpointer) data);
   gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
                      GTK_SIGNAL_FUNC(conf_cancel_cb), (gpointer) data);
   gtk_widget_show(dialog);
   g_main_run(loop);
   g_main_destroy(loop);
   ret = data->value;
   efree(data);
   D_RETURN(3, ret);
}
