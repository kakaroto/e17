/* stalk_interface.c

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

#include "stalk.h"
#include "stalk_interface.h"
#include "stalk_window.h"

GtkWidget *statusbar;

gboolean objwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                          gpointer data);
gboolean objwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer data);
gboolean docwin_enter_cb(GtkWidget * widget, GdkEvent * event,

                         gpointer data);
gboolean docwin_delete_cb(GtkWidget * widget, GdkEvent * event,

                          gpointer data);
gboolean docwin_destroy_cb(GtkWidget * widget, GdkEvent * event,

                           gpointer data);
gboolean docwin_enter_cb(GtkWidget * widget, GdkEvent * event,

                         gpointer data);
gboolean scrollbar_changed_cb(GtkWidget * widget,
                          gpointer data);

void
stalk_create_main_window(stalk_window * swin)
{
   GtkWidget *hbox;

   swin->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_policy(GTK_WINDOW(swin->win), TRUE, TRUE, TRUE);
   gtk_window_set_wmclass(GTK_WINDOW(swin->win), "stalk", "stalk");
   gtk_signal_connect(GTK_OBJECT(swin->win), "delete_event",
                      GTK_SIGNAL_FUNC(docwin_delete_cb), swin);
   gtk_signal_connect(GTK_OBJECT(swin->win), "destroy_event",
                      GTK_SIGNAL_FUNC(docwin_destroy_cb), swin);
   gtk_signal_connect(GTK_OBJECT(swin->win), "enter_notify_event",
                      GTK_SIGNAL_FUNC(docwin_enter_cb), swin);
   gtk_signal_connect(GTK_OBJECT(swin->win), "configure_event",
                      GTK_SIGNAL_FUNC(stalk_window_configure_event), swin);

   hbox = gtk_hbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(swin->win), hbox);
   gtk_widget_show(hbox);

   swin->darea = gtk_drawing_area_new();
   gtk_widget_set_usize(swin->darea, opt.w, opt.h);
   gtk_widget_set_events(swin->darea,
                         GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
                         GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                         GDK_POINTER_MOTION_MASK |
                         GDK_POINTER_MOTION_HINT_MASK);
   gtk_box_pack_start(GTK_BOX(hbox), swin->darea, TRUE, TRUE, 0);

   swin->adjustment =
      gtk_adjustment_new(opt.max_lines, 1.0, opt.max_lines + 1, 1.0, 5.0, 1.0);
   swin->scrollbar = gtk_vscrollbar_new((GtkAdjustment *)swin->adjustment);
   gtk_signal_connect(GTK_OBJECT(swin->adjustment), "value_changed",
                      GTK_SIGNAL_FUNC(scrollbar_changed_cb), swin);
   gtk_widget_show(swin->scrollbar);
   gtk_box_pack_start(GTK_BOX(hbox), swin->scrollbar, FALSE, FALSE, 0);

   gtk_signal_connect(GTK_OBJECT(swin->darea), "button_press_event",
                      GTK_SIGNAL_FUNC(darea_buttonpress_cb), swin);
   gtk_signal_connect(GTK_OBJECT(swin->darea), "button_release_event",
                      GTK_SIGNAL_FUNC(darea_buttonrelease_cb), swin);
   gtk_signal_connect(GTK_OBJECT(swin->darea), "motion_notify_event",
                      GTK_SIGNAL_FUNC(darea_mousemove_cb), swin);
   gtk_signal_connect_after(GTK_OBJECT(swin->darea), "configure_event",
                            GTK_SIGNAL_FUNC(configure_cb), swin);
   gtk_widget_show(swin->darea);
}

gboolean docwin_delete_cb(GtkWidget * widget, GdkEvent * event,
                          gpointer data)
{
   gtk_exit(0);
   return TRUE;
}

gboolean docwin_destroy_cb(GtkWidget * widget, GdkEvent * event,
                           gpointer data)
{
   return TRUE;
}

gboolean docwin_enter_cb(GtkWidget * widget, GdkEvent * event,
                         gpointer data)
{
   return TRUE;
}


gboolean configure_cb(GtkWidget * widget, GdkEventConfigure * event,
                      gpointer data)
{
   return TRUE;
}

gint darea_buttonpress_cb(GtkWidget * widget, GdkEventButton * event,
                          gpointer data)
{
   return TRUE;
}

gint darea_buttonrelease_cb(GtkWidget * widget, GdkEventButton * event,
                            gpointer data)
{
   return TRUE;
}

gint darea_mousemove_cb(GtkWidget * widget, GdkEventMotion * event,
                        gpointer data)
{
   return TRUE;
}


gboolean menu_cb(GtkWidget * widget, gpointer * data)
{
   char *item;

   item = (char *) data;
   printf("IMPLEMENT ME!\n");
   return TRUE;
}

gboolean scrollbar_changed_cb(GtkWidget * widget,
                          gpointer data)
{
   GtkAdjustment *adj;
   stalk_window *win;
   int offset;

   win = STALK_WINDOW(data);
   adj = GTK_ADJUSTMENT(win->adjustment);
   offset = opt.max_lines - (int)adj->value;
   stalk_window_change_offset(win, offset);
   return TRUE;
}

