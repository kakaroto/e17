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
gboolean r_gtk_area_b_press(GtkWidget *area, GdkEventButton *event, gpointer data);
gboolean r_gtk_area_b_release(GtkWidget *area, GdkEventButton *event, gpointer data);

gint view_redraw(gpointer data);
void r_evas_create(void);
void r_evas_init(void);
void r_evas_config_event(GtkWidget *area, GdkEventConfigure *event);
void r_evas_expose_event(GtkWidget *area, GdkEventExpose *event);
void r_evas_toolbar_init(void);
int r_evas_load(char *img);
int r_evas_save(char *img);

void r_cb_init(void);

void r_browser_init(void);

void r_error(char *msg);

void r_file_load(void);
void r_filesel_hide(void);
void r_filesel_openfile(GtkWidget *widget, GtkFileSelection *fs);

void r_cb_m_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void r_b1_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void r_b2_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void r_b3_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void r_b4_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void r_b5_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void r_m_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

