#ifndef ETCHER_RECENT_H
#define ETCHER_RECENT_H

#include <gtk/gtk.h>

#define NUM_RECENT 5

void            recent_init(void);
void            recent_reload_entries(void);
void            recent_add_file(char *filename);
void            recent_load(int index);

#endif
