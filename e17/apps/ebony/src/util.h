#ifndef EBONY_UTIL_H
#define EBONY_UTIL_H

#include"ebony.h"

GList *parse_ebony_bg_list_db(void);
void write_ebony_bg_list_db(GList *);

/* strip .bg.db off the end strip prefixing directory paths */
char *get_shortname_for(const char *s);
void update_background(E_Background _bg);
void outline_evas_object(Evas_Object _o);
int move_layer_up(E_Background_Layer _bl);
int move_layer_down(E_Background_Layer _bl);
void clear_bg_db_keys(E_Background _bg);
void fill_background_images(E_Background _bg);
void redraw_gradient_object(void);

#endif
