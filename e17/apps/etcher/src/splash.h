#ifndef ETCHER_SPLASH_H
#define ETCHER_SPLASH_H

#include <gtk/gtk.h>
#include <Evas.h>

void            show_splash(Evas evas, gint * idle, GtkFunction redraw_func);

#endif
