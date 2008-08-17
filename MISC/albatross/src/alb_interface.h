/* alb_interface.h

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

#ifndef ALB_INTERFACE_H
#define ALB_INTERFACE_H

gint obj_sel_handler, obj_unsel_handler;

gboolean docwin_delete_cb(GtkWidget * widget,
                          GdkEvent * event,
                          gpointer user_data);
gboolean docwin_destroy_cb(GtkWidget * widget,
                           GdkEvent * event,
                           gpointer user_data);
gboolean album_configure_cb(GtkWidget * widget,
                      GdkEventConfigure * event,
                      gpointer user_data);
gboolean scratch_configure_cb(GtkWidget * widget,
                      GdkEventConfigure * event,
                      gpointer user_data);
gint evbox_buttonpress_cb(GtkWidget * widget,
                          GdkEventButton * event,
                          gpointer user_data);
gint evbox_buttonrelease_cb(GtkWidget * widget,
                            GdkEventButton * event,
                            gpointer user_data);
gint evbox_mousemove_cb(GtkWidget * widget,
                        GdkEventMotion * event,
                        gpointer user_data);
gboolean obj_imageadd_cb(GtkWidget * widget,
                         gpointer * data);
gboolean obj_cpy_cb(GtkWidget * widget,
                    gpointer * data);
gboolean obj_del_cb(GtkWidget * widget,
                    gpointer * data);
gboolean obj_edit_cb(GtkWidget * widget,
                     gpointer * data);
gboolean obj_sel_cb(GtkWidget * widget,
                    int row,
                    int column,
                    GdkEventButton * event,
                    gpointer * data);
gboolean obj_unsel_cb(GtkWidget * widget,
                      int row,
                      int column,
                      GdkEventButton * event,
                      gpointer * data);
gboolean obj_load_cancel_cb(GtkWidget * widget,
                            gpointer data);
gboolean obj_addtext_ok_cb(GtkWidget * widget,
                           gpointer * data);
gboolean obj_addtext_cb(GtkWidget * widget,
                        gpointer * data);
gboolean obj_addrect_cb(GtkWidget * widget,
                        gpointer * data);
gboolean obj_addline_cb(GtkWidget * widget,
                        gpointer * data);
gboolean obj_addpoly_cb(GtkWidget * widget,
                        gpointer * data);
gboolean menu_cb(GtkWidget * widget,
                 gpointer * data);
GtkWidget *alb_create_object_list(void);
GtkWidget *alb_gtk_new_document_window(alb_document * doc);
void nbook_switch_page_cb(GtkNotebook * notebook,
                          GtkNotebookPage * page,
                          guint page_num);

void alb_update_props_window(void);

void alb_display_obj_props_window(void);
void alb_hide_obj_props_window(void);
void alb_update_obj_props_window(void);
void alb_clear_obj_props_window(void);

void alb_display_document_props_window(void);
void alb_hide_document_props_window(void);
void alb_update_document_props_window(void);
void alb_clear_document_props_window(void);
void alb_albumlist_populate(void);
char *alb_get_album_from_row(int row);
void alb_select_album(char *album);

GtkWidget *alb_gtk_new_interface(void);

extern char *object_types[];
extern char *object_sizemodes[];
extern char *object_alignments[];


#endif
