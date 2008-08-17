/* alb_interface.c

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

#include "alb.h"
#include "alb_document.h"
#include "alb_object.h"
#include "alb_image.h"
#include "alb_thumb.h"
#include "alb_document_gtk.h"
#include "alb_text.h"
#include "alb_poly.h"
#include "alb_rect.h"
#include "alb_layer.h"
#include "alb_document_xml.h"
#include "alb_gtk_menu.h"
#include "alb_interface.h"

alb_document *current_doc;
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

void refresh_name_cb(GtkWidget * widget,
                     gpointer * obj);
void refresh_sizemode_cb(GtkWidget * widget,
                         gpointer * obj);
void refresh_alignment_cb(GtkWidget * widget,
                          gpointer * obj);
void rotation_cb(GtkWidget * widget,
                 gpointer * obj);
void alb_update_statusbar(alb_document * doc);
void alb_update_document_props_window(void);
void alb_update_obj_props_window(void);
void cb_mnu_quit_activate(GtkMenuItem * menuitem,
                          gpointer user_data);
void cb_album_list_select_row(GtkCList * clist,
                              gint row,
                              gint column,
                              GdkEvent * event,
                              gpointer user_data);

typedef struct _alb_confirmation_dialog_data {
  GtkWidget *dialog;
  gboolean value;
  GMainLoop *loop;
} alb_confirmation_dialog_data;
gboolean win_main_delete_cb(GtkWidget * widget,
                            GdkEvent * event,
                            gpointer user_data);
gboolean win_main_destroy_cb(GtkWidget * widget,
                             GdkEvent * event,
                             gpointer user_data);

gboolean alb_confirmation_dialog_new_with_text(char *text);


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
alb_clear_statusbar(void)
{
  gint contextid;

  if (!statusbar)
    return;
  contextid = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "props");
  gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid,
                     "[No object selected]");
}

void
alb_update_statusbar(alb_document * doc)
{
  alb_object *obj;
  gib_list *list;
  char buff[35];
  gint contextid;

  if (!statusbar)
    return;
  contextid = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "props");
  list = alb_document_get_selected_list(doc);
  if (list) {
    if (gib_list_has_more_than_one_item(list)) {
      gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid,
                         "[Multiple selection]");
    } else {
      obj = list->data;
      g_snprintf(buff, 35, "X:%d | Y:%d | W:%d | H:%d", obj->x, obj->y,
                 obj->w, obj->h);
      gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid, buff);
    }
  } else
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), contextid,
                       "[No object selected]");
}


void
alb_albumlist_populate(void)
{
  struct dirent *de;
  DIR *d;

  D_ENTER(3);

  gtk_clist_freeze(GTK_CLIST(state.album_list));

  if ((d = opendir(opt.album_dir)) == NULL) {
    weprintf("couldn't open dir %s\n", opt.album_dir);
    gtk_clist_thaw(GTK_CLIST(state.album_list));
    D_RETURN_(3);
  }
  de = readdir(d);
  while (de) {
    if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
      gchar *row[1];

      row[0] = g_strdup(de->d_name);
      gtk_clist_append(GTK_CLIST(state.album_list), row);
      g_free(row[0]);
    }
    de = readdir(d);
  }
  closedir(d);
  gtk_clist_thaw(GTK_CLIST(state.album_list));

  D_RETURN_(3);
}

GtkWidget *
alb_gtk_new_interface(void)
{
  GtkWidget *win_main, *hbox, *vbox;
  GtkWidget *vb_main;
  GtkWidget *menubarmain;
  GtkWidget *mnu_file;
  GtkWidget *mnu_file_menu;
  GtkAccelGroup *mnu_file_menu_accels;
  GtkWidget *mnu_quit;
  GtkWidget *hp_album;
  GtkWidget *clist_browse;
  GtkWidget *sw_album;
  GtkWidget *vp_album;
  GtkWidget *da_album;
  GtkWidget *sw_scratch;
  GtkWidget *vp_scratch;
  GtkWidget *da_scratch;

  D_ENTER(3);

  win_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect(GTK_OBJECT(win_main), "delete_event",
                     GTK_SIGNAL_FUNC(win_main_delete_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(win_main), "destroy_event",
                     GTK_SIGNAL_FUNC(win_main_destroy_cb), NULL);
  gtk_object_set_data(GTK_OBJECT(win_main), "win_main", win_main);
  gtk_window_set_title(GTK_WINDOW(win_main), "Albatross");
  gtk_window_set_default_size(GTK_WINDOW(win_main), 800, 600);
  gtk_window_set_policy(GTK_WINDOW(win_main), TRUE, TRUE, FALSE);
  gtk_window_set_wmclass(GTK_WINDOW(win_main), "albatross_main", "albatross");

  vb_main = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vb_main);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "vb_main", vb_main,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(vb_main);
  gtk_container_add(GTK_CONTAINER(win_main), vb_main);

  menubarmain = gtk_menu_bar_new();
  gtk_widget_ref(menubarmain);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "menubarmain", menubarmain,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(menubarmain);
  gtk_box_pack_start(GTK_BOX(vb_main), menubarmain, FALSE, FALSE, 0);

  mnu_file = gtk_menu_item_new_with_label("File");
  gtk_widget_ref(mnu_file);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "mnu_file", mnu_file,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(mnu_file);
  gtk_container_add(GTK_CONTAINER(menubarmain), mnu_file);

  mnu_file_menu = gtk_menu_new();
  gtk_widget_ref(mnu_file_menu);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "mnu_file_menu",
                           mnu_file_menu,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnu_file), mnu_file_menu);
  mnu_file_menu_accels =
    gtk_menu_ensure_uline_accel_group(GTK_MENU(mnu_file_menu));

  mnu_quit = gtk_menu_item_new_with_label("Quit");
  gtk_widget_ref(mnu_quit);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "mnu_quit", mnu_quit,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(mnu_quit);
  gtk_container_add(GTK_CONTAINER(mnu_file_menu), mnu_quit);

  hp_album = gtk_hpaned_new();
  gtk_widget_ref(hp_album);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "hp_album", hp_album,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(hp_album);
  gtk_box_pack_start(GTK_BOX(vb_main), hp_album, TRUE, TRUE, 0);
  gtk_paned_set_handle_size(GTK_PANED(hp_album), 6);
  gtk_paned_set_position(GTK_PANED(hp_album), 0);

  clist_browse = gtk_clist_new(1);
  gtk_widget_ref(clist_browse);
  gtk_widget_set_usize(clist_browse, 50, -1);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "clist_browse", clist_browse,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(clist_browse);
  gtk_paned_pack1(GTK_PANED(hp_album), clist_browse, TRUE, TRUE);
  gtk_clist_set_selection_mode(GTK_CLIST(clist_browse), GTK_SELECTION_BROWSE);
  state.album_list = clist_browse;

  sw_album = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw_album),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  state.sw_album = sw_album;
  gtk_widget_ref(sw_album);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "sw_album", sw_album,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(sw_album);
  gtk_paned_pack2(GTK_PANED(hp_album), sw_album, FALSE, TRUE);

  vp_album = gtk_viewport_new(NULL, NULL);
  gtk_widget_ref(vp_album);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "vp_album", vp_album,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(vp_album);
  gtk_container_add(GTK_CONTAINER(sw_album), vp_album);

  hbox = gtk_hbox_new(TRUE, 0);
  gtk_widget_ref(hbox);
  gtk_widget_show(hbox);
  gtk_container_add(GTK_CONTAINER(vp_album), hbox);

  vbox = gtk_vbox_new(TRUE, 0);
  gtk_widget_ref(vbox);
  gtk_widget_show(vbox);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

  da_album = gtk_drawing_area_new();
  gtk_widget_ref(da_album);
  state.da_album = da_album;
  gtk_widget_set_usize(da_album, -1, 600);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "da_album", da_album,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(da_album);
  gtk_box_pack_start(GTK_BOX(vbox), da_album, FALSE, FALSE, 0);
  gtk_widget_set_events(da_album,
                        GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
                        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                        GDK_POINTER_MOTION_MASK |
                        GDK_POINTER_MOTION_HINT_MASK);

  gtk_signal_connect(GTK_OBJECT(da_album), "button_press_event",
                     GTK_SIGNAL_FUNC(evbox_buttonpress_cb), &state.album);
  gtk_signal_connect(GTK_OBJECT(da_album), "button_release_event",
                     GTK_SIGNAL_FUNC(evbox_buttonrelease_cb), &state.album);
  gtk_signal_connect(GTK_OBJECT(da_album), "motion_notify_event",
                     GTK_SIGNAL_FUNC(evbox_mousemove_cb), &state.album);
  gtk_signal_connect(GTK_OBJECT(da_album), "configure_event",
                     GTK_SIGNAL_FUNC(album_configure_cb), &state.album);

  sw_scratch = gtk_scrolled_window_new(NULL, NULL);
  state.sw_scratch = sw_scratch;
  gtk_widget_ref(sw_scratch);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "sw_scratch", sw_scratch,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(sw_scratch);
  gtk_box_pack_start(GTK_BOX(vb_main), sw_scratch, FALSE, FALSE, 0);
  gtk_widget_set_usize(sw_scratch, -1, 75);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw_scratch),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);

  vp_scratch = gtk_viewport_new(NULL, NULL);
  gtk_widget_ref(vp_scratch);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "vp_scratch", vp_scratch,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(vp_scratch);
  gtk_container_add(GTK_CONTAINER(sw_scratch), vp_scratch);

  da_scratch = gtk_drawing_area_new();
  state.da_scratch = da_scratch;
  gtk_widget_ref(da_scratch);
  gtk_object_set_data_full(GTK_OBJECT(win_main), "da_scratch", da_scratch,
                           (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_set_events(da_scratch,
                        GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
                        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                        GDK_POINTER_MOTION_MASK |
                        GDK_POINTER_MOTION_HINT_MASK);
  gtk_widget_show(da_scratch);
  gtk_container_add(GTK_CONTAINER(vp_scratch), da_scratch);
  gtk_signal_connect(GTK_OBJECT(da_scratch), "button_press_event",
                     GTK_SIGNAL_FUNC(evbox_buttonpress_cb), &state.scratch);
  gtk_signal_connect(GTK_OBJECT(da_scratch), "button_release_event",
                     GTK_SIGNAL_FUNC(evbox_buttonrelease_cb), &state.scratch);
  gtk_signal_connect(GTK_OBJECT(da_scratch), "motion_notify_event",
                     GTK_SIGNAL_FUNC(evbox_mousemove_cb), &state.scratch);
  gtk_signal_connect(GTK_OBJECT(da_scratch), "configure_event",
                     GTK_SIGNAL_FUNC(scratch_configure_cb), &state.scratch);

  alb_albumlist_populate();

  gtk_signal_connect(GTK_OBJECT(mnu_quit), "activate",
                     GTK_SIGNAL_FUNC(cb_mnu_quit_activate), NULL);
  gtk_signal_connect(GTK_OBJECT(clist_browse), "select_row",
                     GTK_SIGNAL_FUNC(cb_album_list_select_row), NULL);

  D_RETURN(3, win_main);
}

gboolean
win_main_delete_cb(GtkWidget * widget,
                   GdkEvent * event,
                   gpointer user_data)
{
  D_ENTER(3);
  gtk_exit(0);
  D_RETURN(3, FALSE);
}

gboolean
win_main_destroy_cb(GtkWidget * widget,
                    GdkEvent * event,
                    gpointer user_data)
{
  D_ENTER(3);
  gtk_exit(0);
  D_RETURN(3, FALSE);
}

void
cb_mnu_quit_activate(GtkMenuItem * menuitem,
                     gpointer user_data)
{
  D_ENTER(3);
  gtk_exit(0);
  D_RETURN_(3);
}


void
cb_album_list_select_row(GtkCList * clist,
                         gint row,
                         gint column,
                         GdkEvent * event,
                         gpointer user_data)
{
  static gint last_row = -1;

  if (last_row != row) {
    last_row = row;
    alb_select_album(alb_get_album_from_row(row));
  }
}

void
alb_select_album(char *album)
{
  struct dirent *de;
  DIR *d;
  char *albumdir, *albdir, *cachedir;
  int x, y, h;
  struct stat st;
  gib_list *files = NULL, *sizes;

  if (state.album) {
    alb_document_free(state.album);
  }
  state.album = alb_document_new(450, 600);
  state.album->darea = state.da_album;
  state.album->scrollwin = state.sw_album;
  gtk_widget_set_usize(state.album->darea, -1, 600);

  alb_document_render_full(state.album, 1);

  albumdir = g_strjoin("/", opt.album_dir, album, NULL);
  if ((d = opendir(albumdir)) == NULL) {
    weprintf("couldn't open dir %s\n", albumdir);
    D_RETURN_(3);
  }

  albdir = g_strjoin("/", albumdir, ".albatross", NULL);
  cachedir = g_strjoin("/", albumdir, ".albatross", "cache", NULL);
  if (stat(albdir, &st) == -1) {
    mkdir(albdir, 0755);
  }
  if (stat(cachedir, &st) == -1) {
    printf("making %s\n", cachedir);
    mkdir(cachedir, 0755);
  }
  free(cachedir);
  free(albdir);

  de = readdir(d);
  x = 5;
  y = 5;
  while (de) {
    if (de->d_name[0] != '.') {
      char *filename;

      filename = g_strjoin("/", albumdir, de->d_name, NULL);
      files = gib_list_add_end(files, filename);
    }
    de = readdir(d);
  }
  closedir(d);

  g_free(albumdir);

  sizes =
    alb_thumb_tesselate_constrain_w(state.album->w, &h, 100, 100, 5, 5,
                                    gib_list_length(files));
  if (gib_list_length(sizes) != gib_list_length(files)) {
    weprintf("wrong! gave it %d, got back %d\n", gib_list_length(files), gib_list_length(sizes));
  } else {
    struct point *p;
    gib_list *file, *points;
    alb_object *thumb;
    int i;

    alb_document_resize(state.album, state.album->w, h);
    gtk_widget_set_usize(state.album->darea, -1, state.album->h);
    points = sizes;
    file = files;
    for (i = 0; i < gib_list_length(files); i++) {
      p = (struct point *) points->data;
      thumb =
        alb_thumb_new_from_file(p->x, p->y, 100, 100, (char *) file->data);
      printf("%d,%d\n", p->x, p->y);
      if (thumb)
        alb_document_add_object(state.album, thumb);
      alb_document_render_updates(state.album, TRUE);
      points = points->next;
      file = file->next;
    }
    gib_list_free_and_data(sizes);
    gib_list_free_and_data(files);
  }
}

char *
alb_get_album_from_row(int row)
{
  gchar *album;

  gtk_clist_get_text(GTK_CLIST(state.album_list), row, 0, &album);
  return album;
}


gboolean
album_configure_cb(GtkWidget * widget,
                   GdkEventConfigure * event,
                   gpointer user_data)
{
  alb_document *doc;

  doc = ALB_DOCUMENT(*((alb_document **) user_data));
  if (!doc)
    return TRUE;

  if ((event->width != doc->w)) {
    printf("configure\n");
    alb_document_resize(doc, event->width, doc->h);
    alb_document_tesselate(doc);
    gtk_widget_set_usize(doc->darea, -1, doc->h);
  }
  return TRUE;
}

gboolean
scratch_configure_cb(GtkWidget * widget,
                     GdkEventConfigure * event,
                     gpointer user_data)
{
  alb_document *doc;

  doc = ALB_DOCUMENT(*((alb_document **) user_data));
  if (!doc)
    return TRUE;

  if ((event->width != doc->w) || (event->height != doc->h)) {
    alb_document_resize(doc, event->width, event->height);
    gtk_widget_set_usize(doc->darea, -1, doc->h);
  }
  return TRUE;
}

gint
evbox_buttonpress_cb(GtkWidget * widget,
                     GdkEventButton * event,
                     gpointer user_data)
{
  alb_object *obj = NULL;
  alb_document *doc;

  D_ENTER(5);
  doc = ALB_DOCUMENT(*((alb_document **) user_data));
  if (!doc)
    D_RETURN(5, 1);
  if (event->button == 1) {
    gib_list *l, *list;

    /* First check for resizes */
    list = alb_document_get_selected_list(doc);
    if (list) {
      int resize = 0;

      for (l = list; l; l = l->next) {
        obj = ALB_OBJECT(l->data);
        if ((resize = alb_object_check_resize_click(obj, event->x, event->y)))
          break;
      }

      if (resize) {
        int res_x, res_y;

        /* Click requests a resize. */
        for (l = list; l; l = l->next) {
          obj = ALB_OBJECT(l->data);
          D(5, ("setting object state RESIZE\n"));
          alb_object_set_state(obj, RESIZE);
          obj->resize = resize;
          alb_object_get_resize_box_coords(obj, resize, &res_x, &res_y);
          obj->clicked_x = res_x - event->x;
          obj->clicked_y = res_y - event->y;
          alb_object_dirty(obj);
        }
        gtk_object_set_data_full(GTK_OBJECT(widget), "resizelist", list,
                                 NULL);
        alb_document_render_updates(doc, 1);
        D_RETURN(5, 1);
      }
      gib_list_free(list);
    }

    obj = alb_document_find_clicked_object(doc, event->x, event->y);

    if (event->state & GDK_SHIFT_MASK) {
      if (obj) {
        /* shift click - multiple selections and selection toggling */
        if (alb_object_get_state(obj, SELECTED)) {
          alb_object_unselect(obj);
        } else {
          alb_object_select(obj);
        }
        alb_document_dirty_selection(doc);
        alb_update_props_window();
      } else {          /* no object clicked selection rect will be drawn */

        doc->select_x = event->x;
        doc->select_y = event->y;
      }
    } else {            /*single click */

      if (!obj) {
        alb_document_unselect_all(doc);
        alb_clear_statusbar();
        alb_clear_obj_props_window();

        doc->select_x = event->x;
        doc->select_y = event->y;
      } else {          /*we clicked an object */

        if (!alb_object_get_state(obj, SELECTED)) {
          alb_document_unselect_all(doc);
          alb_object_select(obj);
        }

        list = alb_document_get_selected_list(doc);
        if (list) {
          for (l = list; l; l = l->next) {
            /* set clicked_x,y */
            obj = ALB_OBJECT(l->data);
            obj->clicked_x = event->x - obj->x;
            obj->clicked_y = event->y - obj->y;
            D(2, ("setting object state DRAG\n"));
            alb_object_set_state(obj, DRAG);
            alb_object_raise(obj);
          }
        }
        gtk_object_set_data_full(GTK_OBJECT(widget), "draglist", list, NULL);
      }
      alb_document_render_updates(doc, 1);
      alb_update_props_window();
      alb_update_statusbar(doc);
    }
  }
  D_RETURN(5, 1);
}

gint
evbox_buttonrelease_cb(GtkWidget * widget,
                       GdkEventButton * event,
                       gpointer user_data)
{
  gib_list *list, *l;
  alb_object *obj;
  alb_document *doc;
  int dragged = 0;
  int resized = 0;

  D_ENTER(5);
  doc = ALB_DOCUMENT(*(alb_document **) user_data);
  if (!doc)
    D_RETURN(5, 1);
  list = gtk_object_get_data(GTK_OBJECT(widget), "draglist");
  if (list) {
    for (l = list; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      D(2, ("unsetting object state DRAG\n"));
      alb_object_unset_state(obj, DRAG);
      /* not being dragged, don't need these */
      obj->clicked_x = obj->clicked_y = 0;
      alb_object_dirty(obj);
    }
    gib_list_free(list);
    gtk_object_set_data_full(GTK_OBJECT(widget), "draglist", NULL, NULL);
    dragged = 1;
  } else if ((list = gtk_object_get_data(GTK_OBJECT(widget), "resizelist"))) {
    for (l = list; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      alb_object_resize_done(obj);
      alb_document_render_updates(doc, 1);
    }
    gib_list_free(list);
    gtk_object_set_data_full(GTK_OBJECT(widget), "resizelist", NULL, NULL);
    resized = 1;
  } else if (doc->select_x && doc->select_y && !resized && !dragged) {
    int x, y, w, h;
    gib_list *l, *ll;

    /* this is a bit hackish: we didnt resize or drag, so no object was
     * clicked. If there are updates now, they're the boundaries of the 
     * selection rect so we get the rect in which to look for objects to
     * select from them. Spares having to figure out the direction of the
     * rect again. */

    if (doc->up) {
      imlib_updates_get_coordinates(doc->up, &x, &y, &w, &h);


      /*select all in rect */
      for (l = doc->layers; l; l = l->next) {
        for (ll = ALB_LAYER(l->data)->objects; ll; ll = ll->next) {
          alb_object *obj;

          obj = ALB_OBJECT(ll->data);

          if (RECTS_INTERSECT(x, y, w, h, obj->x, obj->y, obj->w, obj->h)) {
            /*toggle_selection */

            if (alb_object_get_state(obj, SELECTED)) {
              alb_object_unset_state(obj, SELECTED);
              alb_object_dirty(obj);
            } else {
              alb_object_set_state(obj, SELECTED);
            }
          }
        }
        /* need to dirty all the other selected objects, in case they go from
         * being the only thing selected to part of a multiple selection -
         * hence have to change their selection type */
        alb_document_dirty_selection(doc);
        alb_update_props_window();

      }
    }
    doc->select_x = doc->select_y = doc->old_select_w = doc->old_select_h = 0;
  }
  alb_document_render_updates(doc, 1);
  D_RETURN(5, 1);
}

gint
evbox_mousemove_cb(GtkWidget * widget,
                   GdkEventMotion * event,
                   gpointer user_data)
{
  gib_list *l, *list;
  alb_object *obj;
  alb_document *doc;
  int x, y;

  GdkModifierType state;

  D_ENTER(5);
  doc = ALB_DOCUMENT(*(alb_document **) user_data);
  if (!doc)
    D_RETURN(5, 1);
  /* use hinted motionnotify to prevent queue backlog */
  if (event->is_hint)
    gdk_window_get_pointer(event->window, &x, &y, &state);
  else {
    x = event->x;
    y = event->y;
    state = event->state;
  }

  list = gtk_object_get_data(GTK_OBJECT(widget), "draglist");
  if (list) {
    for (l = list; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      D(5, ("moving object to %d, %d\n", x, y));
      alb_object_move(obj, x, y);
    }
    alb_update_statusbar(doc);
    alb_document_render_updates(doc, 1);
  } else if ((list = gtk_object_get_data(GTK_OBJECT(widget), "resizelist"))) {
    for (l = list; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      D(5, ("resizing object\n"));
      alb_object_resize(obj, x, y);
    }
    alb_update_statusbar(doc);
    alb_document_render_updates(doc, 1);
  } else {

    if (doc->select_x && doc->select_y) {	/*we need to draw a select rect */
      int rect_x, rect_y, w, h;

      /*repaint area where the last select rect was */
      alb_document_render_updates(doc, 0);

      /*figure out the direction */
      if (x >= doc->select_x) {
        rect_x = doc->select_x;
        w = x - doc->select_x;
      } else {
        rect_x = x;
        w = doc->select_x - x;
      }

      if (y >= doc->select_y) {
        rect_y = doc->select_y;
        h = y - doc->select_y;
      } else {
        rect_y = y;
        h = doc->select_y - y;
      }

      /*draw selection rect */
      gib_imlib_image_fill_rectangle(doc->im, rect_x, rect_y, w, h, 170, 210,
                                     170, 120);
      gib_imlib_image_draw_rectangle(doc->im, rect_x, rect_y, w, h, 0, 0, 0,
                                     255);

      /*render on top of everything else */
      alb_document_render_pmap_partial(doc, rect_x, rect_y, w, h);
      /* clear old rect */
      if (doc->old_select_w != 0)
        alb_document_render_to_window_partial(doc, doc->old_select_x,
                                              doc->old_select_y,
                                              doc->old_select_w,
                                              doc->old_select_h);
      /* clear new rect */
      alb_document_render_to_window_partial(doc, rect_x, rect_y, w, h);

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

gboolean
obj_load_cb(GtkWidget * widget,
            gpointer data)
{
  alb_object *obj = NULL;
  char *path;

  D_ENTER(3);
  path =
    gtk_file_selection_get_filename(GTK_FILE_SELECTION((GtkWidget *) data));
  if (path) {
    obj = alb_image_new_from_file(0, 0, path);
    if (obj) {
      alb_document_add_object(current_doc, obj);
      alb_object_show(obj);
      alb_object_raise(obj);
      alb_document_unselect_all(current_doc);
      alb_document_render_updates(current_doc, 1);
    }
  }
  gtk_widget_destroy((GtkWidget *) data);
  D_RETURN(3, TRUE);
}

gboolean
obj_load_cancel_cb(GtkWidget * widget,
                   gpointer data)
{
  gtk_widget_destroy((GtkWidget *) data);
  return TRUE;
}

gboolean
obj_imageadd_cb(GtkWidget * widget,
                gpointer * data)
{
  if (doc_list) {
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


gboolean
obj_cpy_cb(GtkWidget * widget,
           gpointer * data)
{
  alb_object *new;
  gib_list *l, *list;
  alb_object *obj;

  D_ENTER(3);
  if (doc_list) {
    list = alb_document_get_selected_list(current_doc);
    for (l = list; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      new = alb_object_duplicate(obj);
      if (new) {
        new->x += 5;
        new->y += 5;
        alb_document_add_object(current_doc, new);
      }
    }
    gib_list_free(list);
    alb_document_render_updates(current_doc, 1);
  }
  D_RETURN(3, TRUE);
}

gboolean
obj_del_cb(GtkWidget * widget,
           gpointer * data)
{
  alb_object *obj;
  gib_list *l, *list;

  D_ENTER(3);
  if (doc_list) {
    list = alb_document_get_selected_list(current_doc);
    for (l = list; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      if (obj_props_active)
        alb_clear_obj_props_window();
      alb_clear_statusbar();
      alb_document_remove_object(current_doc, obj);
      alb_object_free(obj);
    }
    gib_list_free(list);
    alb_document_render_updates(current_doc, 1);
  }
  D_RETURN(3, TRUE);
}


gboolean
obj_sel_cb(GtkWidget * widget,
           int row,
           int column,
           GdkEventButton * event,
           gpointer * data)
{
  GList *selection;
  alb_object *obj;

  D_ENTER(3);
  obj = (alb_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);
  if (obj) {
    alb_object_select(obj);
    alb_update_props_window();
    alb_update_statusbar(ALB_OBJECT_DOC(obj));
    selection = GTK_CLIST(widget)->selection;
    if (g_list_length(selection) > 1)
      alb_document_dirty_selection(ALB_OBJECT_DOC(obj));
    alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  }

  D_RETURN(3, TRUE);
}

gboolean
obj_unsel_cb(GtkWidget * widget,
             int row,
             int column,
             GdkEventButton * event,
             gpointer * data)
{
  GList *selection;
  alb_object *obj;

  D_ENTER(3);
  obj = (alb_object *) gtk_clist_get_row_data(GTK_CLIST(widget), row);
  if (obj) {
    alb_object_unselect(obj);
    alb_update_props_window();
    alb_update_statusbar(ALB_OBJECT_DOC(obj));
    selection = GTK_CLIST(widget)->selection;
    if (g_list_length(selection) > 1)
      alb_document_dirty_selection(ALB_OBJECT_DOC(obj));
    alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  }

  D_RETURN(3, TRUE);
}

gboolean
obj_addtext_cb(GtkWidget * widget,
               gpointer * data)
{

  alb_object *obj;

  D_ENTER(3);
  if (doc_list) {
    obj =
      ALB_OBJECT(alb_text_new_with_text
                 (50, 50, "20thcent", 12, "New Text", JUST_LEFT, TRUE, 255,
                  40, 255, 0));
    alb_document_add_object(current_doc, obj);
    alb_document_unselect_all(current_doc);
    alb_document_render_updates(current_doc, 1);
  }
  D_RETURN(3, TRUE);
}



gboolean
obj_addrect_cb(GtkWidget * widget,
               gpointer * data)
{
  alb_object *obj;

  D_ENTER(3);
  if (doc_list) {
    obj = ALB_OBJECT(alb_rect_new_of_size(50, 50, 50, 50, 255, 0, 0, 0));
    alb_document_add_object(current_doc, obj);
    alb_document_unselect_all(current_doc);
    alb_document_render_updates(current_doc, 1);
  }
  D_RETURN(3, TRUE);
}

gboolean
obj_addline_cb(GtkWidget * widget,
               gpointer * data)
{
  alb_object *obj;

  D_ENTER(3);
  if (doc_list) {
    obj = alb_line_new_from_to(50, 50, 100, 100, 255, 0, 0, 0);
    alb_document_add_object(current_doc, obj);
    alb_document_unselect_all(current_doc);
    alb_document_render_updates(current_doc, 1);
  }
  D_RETURN(3, TRUE);
}


static void
obj_vis_cb(GtkWidget * widget,
           gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *l = NULL;
  gib_list *list = NULL;

  D_ENTER(3);
  list = alb_document_get_selected_list(current_doc);
  if (gib_list_length(list) > 1) {
    for (l = list; l; l = l->next) {
      obj = l->data;
      if (alb_object_get_state(obj, VISIBLE))
        alb_object_hide(obj);
      else
        alb_object_show(obj);
    }
  } else {
    obj = list->data;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
      alb_object_show(obj);
    else
      alb_object_hide(obj);
  }
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  gib_list_free(list);
  D_RETURN_(3);
}

void
refresh_name_cb(GtkWidget * widget,
                gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *list = NULL;

  list = alb_document_get_selected_list(current_doc);
  D_ENTER(3);
  if (gib_list_length(list) > 1)
    printf("Implement me!\n");
  else {
    obj = list->data;
    if (obj->name)
      efree(obj->name);
    obj->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  }
  gib_list_free(list);
  D_RETURN_(3);
}

void
refresh_sizemode_cb(GtkWidget * widget,
                    gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *l = NULL;
  gib_list *list = NULL;

  list = alb_document_get_selected_list(current_doc);
  D_ENTER(3);
  for (l = list; l; l = l->next) {
    obj = l->data;
    alb_object_dirty(obj);
    obj->sizemode =
      alb_object_get_sizemode_from_string(gtk_entry_get_text
                                          (GTK_ENTRY(widget)));
    alb_object_update_positioning(obj);
    alb_object_dirty(obj);
  }
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  gib_list_free(list);
  D_RETURN_(3);
}


void
refresh_alignment_cb(GtkWidget * widget,
                     gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *l = NULL;
  gib_list *list = NULL;

  list = alb_document_get_selected_list(current_doc);
  D_ENTER(3);
  for (l = list; l; l = l->next) {
    obj = l->data;
    alb_object_dirty(obj);
    obj->alignment =
      alb_object_get_alignment_from_string(gtk_entry_get_text
                                           (GTK_ENTRY(widget)));
    alb_object_update_positioning(obj);
    alb_object_dirty(obj);
  }
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  gib_list_free(list);
  D_RETURN_(3);
}


void
buttons_cb(GtkWidget * widget,
           gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *l = NULL;
  gib_list *list = NULL;

  D_ENTER(3);
  list = alb_document_get_selected_list(current_doc);
  for (l = list; l; l = l->next) {
    obj = l->data;
    alb_object_dirty(obj);
    switch (GPOINTER_TO_INT(data)) {
      case 1:
        alb_object_update_position_relative(obj, 0, -1);
        break;
      case 2:
        alb_object_update_position_relative(obj, 0, +1);
        break;
      case 3:
        alb_object_update_position_relative(obj, -1, 0);
        break;
      case 4:
        alb_object_update_position_relative(obj, +1, 0);
        break;
      case 5:
        alb_object_update_dimensions_relative(obj, 0, +1);
        break;
      case 6:
        alb_object_update_dimensions_relative(obj, 0, -1);
        break;
      case 7:
        alb_object_update_dimensions_relative(obj, +1, 0);
        break;
      case 8:
        alb_object_update_dimensions_relative(obj, -1, 0);
        break;
      default:
        break;
    }
    alb_object_dirty(obj);
  }
  alb_update_statusbar(current_doc);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  gib_list_free(list);
  D_RETURN_(3);
}

void
rotation_cb(GtkWidget * widget,
            gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *l = NULL;
  gib_list *list = NULL;
  double angle;

  list = alb_document_get_selected_list(current_doc);
  D_ENTER(3);

  angle = GPOINTER_TO_INT(data);

  for (l = list; l; l = l->next) {
    obj = l->data;
    alb_object_dirty(obj);
    alb_object_rotate(obj, angle);
    alb_object_dirty(obj);
  }
  alb_update_statusbar(current_doc);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  gib_list_free(list);
  D_RETURN_(3);

}

void
alb_display_obj_props_window(void)
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
  if (doc_list) {
    if (obj_props_active)
      D_RETURN_(3);

    obj_props_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    table = gtk_table_new(2, 4, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(obj_props_window), 5);
    gtk_container_add(GTK_CONTAINER(obj_props_window), table);
    gtk_signal_connect(GTK_OBJECT(obj_props_window), "delete_event",
                       GTK_SIGNAL_FUNC(alb_hide_obj_props_window), NULL);
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
    gtk_table_attach(GTK_TABLE(gen_table), rot_45_counterclockwise_btn, 1, 2,
                     7, 8, GTK_FILL | GTK_EXPAND, 0, 2, 2);
    gtk_widget_show(rot_45_counterclockwise_btn);

    rot_counterclockwise_btn = gtk_button_new_with_label("ccw");
    gtk_table_attach(GTK_TABLE(gen_table), rot_counterclockwise_btn, 2, 3, 7,
                     8, GTK_FILL | GTK_EXPAND, 0, 2, 2);
    gtk_widget_show(rot_counterclockwise_btn);

    rot_clockwise_btn = gtk_button_new_with_label("clockwise");
    gtk_table_attach(GTK_TABLE(gen_table), rot_clockwise_btn, 3, 4, 7, 8,
                     GTK_FILL | GTK_EXPAND, 0, 2, 2);
    gtk_widget_show(rot_clockwise_btn);

    rot_45_clockwise_btn = gtk_button_new_with_label("clockwise 45");
    gtk_table_attach(GTK_TABLE(gen_table), rot_45_clockwise_btn, 4, 5, 7, 8,
                     GTK_FILL | GTK_EXPAND, 0, 2, 2);
    gtk_widget_show(rot_45_clockwise_btn);

    for (i = 0; i < ALIGN_MAX; i++) {
      align_list = g_list_append(align_list, object_alignments[i]);
    }

    gtk_combo_set_popdown_strings(GTK_COMBO(alignment_combo), align_list);
    for (i = 0; i < SIZEMODE_MAX; i++) {
      align_list = g_list_append(sizemode_list, object_sizemodes[i]);
    }

    gtk_combo_set_popdown_strings(GTK_COMBO(sizemode_combo), sizemode_list);
    gtk_signal_connect(GTK_OBJECT(vis_toggle), "clicked",
                       GTK_SIGNAL_FUNC(obj_vis_cb), NULL);
    gtk_signal_connect(GTK_OBJECT(up), "clicked", GTK_SIGNAL_FUNC(buttons_cb),
                       (gpointer) 1);
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
                       "changed", GTK_SIGNAL_FUNC(refresh_sizemode_cb), NULL);

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
    alb_update_obj_props_window();
  }
  D_RETURN_(3);
}

void
alb_hide_obj_props_window(void)
{
  D_ENTER(3);
  if (obj_props_active) {
    gtk_widget_destroy(obj_props_window);
    obj_props_active = 0;
  }
  D_RETURN_(3);
}

void
alb_clear_obj_props_window(void)
{
  if (obj_props_active) {
    if (obj_hbox) {
      gtk_widget_destroy(obj_hbox);
      obj_hbox = NULL;
    }
    if (gen_props)
      gtk_widget_hide(gen_props);
  }
}

void
alb_update_props_window(void)
{
  D_ENTER(3);
  alb_update_obj_props_window();
  D_RETURN_(3);
}


void
alb_update_obj_props_window(void)
{
  alb_object *obj;
  GtkWidget *new_hbox;
  gib_list *list;
  alb_object *obj_first;
  gib_list *l;
  char *align_string = NULL;

  char *sizemode_string = NULL;

  D_ENTER(3);
  /*only do something if the props window is there */
  if (obj_props_active) {
    if (obj_hbox) {
      gtk_widget_destroy(obj_hbox);
      obj_hbox = NULL;
    }

    /*show generic part */
    gtk_widget_show(gen_props);
    /*block signal handlers */
    gtk_signal_handler_block_by_func(GTK_OBJECT(vis_toggle),
                                     GTK_SIGNAL_FUNC(obj_vis_cb), NULL);
    gtk_signal_handler_block_by_func(GTK_OBJECT(name), refresh_name_cb, NULL);
    gtk_signal_handler_block_by_func(GTK_OBJECT
                                     (GTK_COMBO(alignment_combo)->entry),
                                     refresh_alignment_cb, NULL);
    gtk_signal_handler_block_by_func(GTK_OBJECT
                                     (GTK_COMBO(sizemode_combo)->entry),
                                     refresh_sizemode_cb, NULL);
    list = alb_document_get_selected_list(current_doc);
    if (list) {
      /*update the values in the generic part */

      if (gib_list_has_more_than_one_item(list)) {
        /*grey out the name entry box */
        gtk_entry_set_text(GTK_ENTRY(name), "");
        gtk_widget_set_sensitive(GTK_WIDGET(name), FALSE);
        obj_first = list->data;
        /*check wether all objects have the same alignment or sizemode, and if
           so, set the combo boxes, if not leave them empty */
        for (l = list; l; l = l->next) {
          obj = l->data;
          if (obj->alignment == obj_first->alignment)
            align_string = alb_object_get_alignment_string(obj);
          else
            align_string = "";
          if (obj->sizemode == obj_first->sizemode)
            sizemode_string = alb_object_get_sizemode_string(obj);
          else
            sizemode_string = "";
        }

        gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
                           align_string);
        gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
                           sizemode_string);
        gtk_window_set_title(GTK_WINDOW(obj_props_window),
                             "[multiple selection]");
      } else {

        gtk_widget_set_sensitive(GTK_WIDGET(name), TRUE);
        obj = list->data;
        if (obj->name)
          gtk_entry_set_text(GTK_ENTRY(name), obj->name);
        gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
                           alb_object_get_alignment_string(obj));
        gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
                           alb_object_get_sizemode_string(obj));
        if (alb_object_get_state(obj, VISIBLE))
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vis_toggle), TRUE);
        new_hbox = obj->display_props(obj);
        obj_hbox = new_hbox;
        gtk_table_attach(GTK_TABLE(table), obj_hbox, 0, 4, 1, 2,
                         GTK_FILL | GTK_EXPAND, 0, 2, 2);
        gtk_widget_show(new_hbox);
        gtk_window_set_title(GTK_WINDOW(obj_props_window), obj->name);
      }
      gib_list_free(list);
    } else
      alb_clear_obj_props_window();
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
refresh_doc_name_cb(GtkWidget * widget,
                    gpointer * data)
{
  D_ENTER(3);
  efree(current_doc->name);
  current_doc->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  D_RETURN_(3);
}

void
refresh_doc_file_name_cb(GtkWidget * widget,
                         gpointer * data)
{
  D_ENTER(3);
  efree(current_doc->filename);
  current_doc->filename = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
  D_RETURN_(3);
}

static void
refresh_bg_cb(GtkWidget * widget,
              gpointer * data)
{

  switch (GPOINTER_TO_INT(data)) {
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

  alb_document_render_full(current_doc, 1);
}

static void
refresh_w_cb(GtkWidget * widget,
             gpointer * data)
{
  int w, h;

  D_ENTER(3);

  w = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  h = current_doc->h;

  alb_document_resize(current_doc, w, h);
  /*
     while(gtk_events_pending())
     gtk_main_iteration();
   */

  D_RETURN_(3);
}

static void
refresh_h_cb(GtkWidget * widget,
             gpointer * data)
{
  int w, h;

  D_ENTER(3);

  h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  w = current_doc->w;

  alb_document_resize(current_doc, w, h);

  D_RETURN_(3);
}



void
alb_display_document_props_window(void)
{
  GtkWidget *table, *frame, *hbox;
  GtkWidget *name_l, *file_name_l;
  GtkWidget *cr_l, *cg_l, *cb_l, *ca_l;
  GtkAdjustment *a1, *a2, *a3, *a4, *ah, *aw;
  GtkWidget *w_l, *h_l;

  D_ENTER(3);
  if (doc_list) {
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
                       GTK_SIGNAL_FUNC(alb_hide_document_props_window), NULL);
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
    alb_update_document_props_window();
  }
  D_RETURN_(3);
}

void
alb_clear_document_props_window(void)
{
  D_ENTER(3);
  if (doc_props_active)
    gtk_widget_hide(doc_hbox);

  D_RETURN_(3);
}

void
alb_update_document_props_window(void)
{
  D_ENTER(3);
  if (doc_props_active) {
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
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(w_wid), refresh_w_cb, NULL);
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(h_wid), refresh_h_cb, NULL);
  }

  D_RETURN_(3);
}

void
alb_hide_document_props_window(void)
{
  D_ENTER(3);
  gtk_widget_destroy(doc_props_window);
  doc_props_active = 0;
  D_RETURN_(3);
}


void
conf_ok_cb(GtkWidget * widget,
           gpointer data)
{
  alb_confirmation_dialog_data *dialog =
    (alb_confirmation_dialog_data *) data;
  dialog->value = TRUE;
  gtk_widget_destroy(dialog->dialog);

  g_main_quit(dialog->loop);
}

void
conf_cancel_cb(GtkWidget * widget,
               gpointer data)
{
  alb_confirmation_dialog_data *dialog =
    (alb_confirmation_dialog_data *) data;
  dialog->value = FALSE;
  gtk_widget_destroy(dialog->dialog);

  g_main_quit(dialog->loop);
}

gboolean
alb_confirmation_dialog_new_with_text(char *text)
{
  alb_confirmation_dialog_data *data;
  GMainLoop *loop;
  gboolean ret;
  GtkWidget *dialog, *label, *ok_button, *cancel_button, *table;

  D_ENTER(3);
  data = (alb_confirmation_dialog_data *)
    emalloc(sizeof(alb_confirmation_dialog_data));
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
