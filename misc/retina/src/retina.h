/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <Evas.h>
#include <Edb.h>

#define QUEUE_DRAW \
if (current_idle) gtk_idle_remove(current_idle);\
current_idle = gtk_idle_add(view_redraw, NULL);

/* Function Prototypes */
void r_gtk_init(void);
void r_gtk_area_b_press(GtkWidget *area, GdkEventButton *event);

void r_evas_init(void);
void r_evas_config_event(GtkWidget *area, GdkEventConfigure *event);
void r_evas_expose_event(GtkWidget *area, GdkEventExpose *event);
void r_evas_toolbar_init(void);
int r_evas_load(char *img);
int r_evas_save(char *img);

void r_cb_init(void);

gint view_redraw(gpointer data);
