#ifndef _EBONY_INTERFACE_H_
#define _EBONY_INTERFACE_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <Evas.h>
#include <Ebg.h>

#define D(string) fprintf(stderr, string)
#define MAX_RECENT_BG_COUNT 5

int idle;
int bg_list_count;

GtkWidget* win_ref, *ebony_status;
Evas evas;
E_Background bg;
GList *recent_bgs;

GtkWidget* create_ebony_window(void);

#endif
