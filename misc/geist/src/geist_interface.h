#ifndef GEIST_INTERFACE_H
#define GEIST_INTERFACE_H

GtkWidget *obj_win;
GtkWidget *obj_list;
gint obj_sel_handler, obj_unsel_handler;

gboolean mainwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer user_data);
gboolean mainwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

                            gpointer user_data);
gboolean configure_cb(GtkWidget * widget, GdkEventConfigure * event,

                      gpointer user_data);
gint evbox_buttonpress_cb(GtkWidget * widget, GdkEventButton * event,

                          gpointer user_data);
gint evbox_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event,
                            gpointer user_data);
gint evbox_mousemove_cb(GtkWidget * widget, GdkEventMotion * event,

                        gpointer user_data);
gboolean obj_imageadd_cb(GtkWidget * widget, gpointer * data);
gboolean obj_cpy_cb(GtkWidget * widget, gpointer * data);
gboolean obj_del_cb(GtkWidget * widget, gpointer * data);
gboolean obj_edit_cb(GtkWidget * widget, gpointer * data);
gboolean obj_sel_cb(GtkWidget * widget, int row, int column,
                    GdkEventButton * event, gpointer * data);
gboolean obj_unsel_cb(GtkWidget * widget, int row, int column,
                      GdkEventButton * event, gpointer * data);
gboolean obj_load_cancel_cb(GtkWidget * widget, gpointer data);
gboolean obj_addtext_ok_cb(GtkWidget * widget, gpointer * data);
gboolean obj_addtext_cb(GtkWidget * widget, gpointer * data);
gboolean obj_addrect_cb(GtkWidget * widget, gpointer * data);
gboolean menu_cb(GtkWidget * widget, gpointer * data);
GtkWidget *geist_create_main_window(void);
GtkWidget *geist_create_object_list(void);
GtkWidget *
geist_gtk_new_document_page(geist_document *doc);
void nbook_switch_page_cb(GtkNotebook * notebook, GtkNotebookPage * page,
                     guint page_num);

void geist_display_props_window(void);
void geist_hide_props_window(void);
void geist_update_props_window(void);
void geist_clear_props_window(void);

#endif
