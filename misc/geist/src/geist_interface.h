/* geist_interface.h

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

#ifndef GEIST_INTERFACE_H
#define GEIST_INTERFACE_H

GtkWidget *obj_win;
GtkWidget *obj_list;
gint obj_sel_handler, obj_unsel_handler;

gboolean docwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer user_data);
gboolean docwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

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
gboolean obj_addline_cb(GtkWidget * widget, gpointer * data);
gboolean obj_addpoly_cb(GtkWidget * widget, gpointer * data);
gboolean menu_cb(GtkWidget * widget, gpointer * data);
GtkWidget *geist_create_object_list(void);
GtkWidget *geist_gtk_new_document_window(geist_document *doc);
void nbook_switch_page_cb(GtkNotebook * notebook, GtkNotebookPage * page,
                     guint page_num);

void geist_update_props_window(void);

void geist_display_obj_props_window(void);
void geist_hide_obj_props_window(void);
void geist_update_obj_props_window(void);
void geist_clear_obj_props_window(void);

void geist_display_document_props_window(void);
void geist_hide_document_props_window(void);
void geist_update_document_props_window(void);
void geist_clear_document_props_window(void);

extern char *object_types[];
extern char *object_sizemodes[];
extern char *object_alignments[];


#endif
