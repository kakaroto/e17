#ifndef ETCHER_SPLASH_H
#define ETCHER_SPLASH_H

#include <gtk/gtk.h>
#include <Evas.h>

void                show_splash(Evas *evas, gint * idle,
				GtkFunction redraw_func);
void                e_evas_object_image_file_set(Evas_Object *_o, 
						 char *file, char *key);

#endif
