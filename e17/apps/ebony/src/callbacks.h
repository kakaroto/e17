#ifndef EBONY_CALLBACKS
#define EBONY_CALLBACKS

#include "interface.h"

#define EBONY_STATUS_TO 3500
#define UN(data) data = 0 
#define DRAW() \
{ \
    if(idle) gtk_idle_remove(idle); \
    idle = gtk_idle_add(redraw, NULL); \
}

int redraw(void*);

gboolean 
drawing_area_expose_event(GtkWidget*, GdkEventExpose *ev, gpointer);

gboolean 
drawing_area_configure_event(GtkWidget*, GdkEventConfigure *ev, gpointer);

void save_bg(GtkWidget*, gpointer);
void save_bg_as(GtkWidget*, gpointer);
void new_bg(GtkWidget*, gpointer);
void open_bg(GtkWidget*, gpointer);
void open_bg_named(char* named);
void open_bg_cb(GtkWidget *, gpointer);

void advanced_widgets_show_for_color(void);
void advanced_widgets_show_for_gradient(void);
void advanced_widgets_show_for_image(void);

gint ebony_status_clear(gpointer data);
void ebony_status_message(gchar *message, gint delay);

void on_scroll_changed(GtkWidget *w, gpointer data);
#endif
