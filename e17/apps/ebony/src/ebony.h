#ifndef EBONY_H
#define EBONY_H

#include <Ebg.h>
#include <stdio.h>
#include <Imlib2.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <Evas_Engine_Software_X11.h>
#include "../config.h"

GtkWidget *ebony_status;
GtkWidget *win_ref, *bg_ref;

GList *recent_bgs;

Evas *evas;
E_Background bg;                /* the bg we're manipulating */
E_Background ebony_base_bg;     /* the canvases background */
E_Background_Layer bl;
int idle;

char image_fileselection_dir[PATH_MAX];
char bg_fileselection_dir[PATH_MAX];
char save_as_fileselection_dir[PATH_MAX];
char export_fileselection_dir[PATH_MAX];

#define MAX_RECENT_BG_COUNT 5
#define EBONY_STATUS_TO 3500
#define DRAW() \
{ \
    if(idle) gtk_idle_remove(idle); \
    idle = gtk_idle_add(redraw, NULL); \
}
#define UN(data) data = 0

struct
{
   struct
   {
      int w, h;
   }
   screen;
   struct
   {
      int v, h;
   }
   xinerama;
}
export_info;

#endif
