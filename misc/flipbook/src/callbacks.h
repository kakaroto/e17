/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <gtk/gtk.h>


void on_preferences1_close(GtkWidget * widget, gpointer user_data);
void on_choose_movie_segment1_close(GtkWidget * widget, gpointer user_data);
void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_choose_movie_segment1_activate(GtkMenuItem * menuitem,
								   gpointer user_data);
void on_exit_application(GtkWidget *widget, gpointer user_data);
void on_open_from_file(GtkWidget *widget, gpointer user_data);
void on_open_from_template(GtkWidget *widget, gpointer user_data);
void on_open_template_close(GtkWidget *widget, gpointer user_data);
void on_movie_info(GtkWidget *widget, gpointer user_data);
void on_movie_info_close(GtkWidget *widget, gpointer user_data);

void on_play_button(GtkWidget *widget, gpointer user_data);
void on_rewind_button(GtkWidget *widget, gpointer user_data);
void on_forward_button(GtkWidget *widget, gpointer user_data);

void on_save_preferences(GtkWidget *widget, gpointer user_data);
void on_chose_segment(GtkWidget *widget, gpointer user_data);
void on_open_from_file_close(GtkWidget *widget, gpointer user_data);
void on_close_movie(GtkWidget *widget, gpointer user_data);
