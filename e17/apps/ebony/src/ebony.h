#ifndef EBONY_H
#define EBONY_H

#include <Ebg.h>
#include <stdio.h>
#include <Imlib2.h>
#include <stdlib.h>
#include <gtk/gtk.h>

GtkWidget *ebony_status;
GtkWidget *win_ref;

GList *recent_bgs;

Evas evas;
E_Background bg;
E_Background_Layer bl;
int idle;

#define MAX_RECENT_BG_COUNT 5
#define EBONY_STATUS_TO 3500
#define DRAW() \
{ \
    if(idle) gtk_idle_remove(idle); \
    idle = gtk_idle_add(redraw, NULL); \
}
#define UN(data) data = 0
#endif
