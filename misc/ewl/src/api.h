#ifndef _API_H_
#define _API_H_

#include "ewldb.h"
#include "timer.h"
#include "event.h"
#include "object.h"
#include "widget.h"
#include "icon.h"
#include "translators.h"
#include "container.h"
#include "box.h"
#include "x.h"
#include "window.h"

#define EWL_DEFAULT_IMAGE_CACHE_SIZE 8388608
#define EWL_DEFAULT_FONT_CACHE_SIZE  1048576

void       ewl_init(int *argc, char **argv[]);
void       ewl_main_iteration();
void       ewl_main();
char       ewl_done();
void       ewl_quit();

/* PREFERENCES/INTERNAL STATE FUNCTIONS */
EwlObject *ewl_get_state();
void       ewl_load_prefs();

/* RENDER LIST FUNCTIONS */
EwlList   *ewl_get_render_list();
void       ewl_add_render(void *object);
void       ewl_remove_render(void *object);
void       ewl_handle_renders();

/* MEDIA PATH FUNCTIONS */
EwlList   *ewl_get_path_list();
void       ewl_add_path(char *path);
void       ewl_remove_path(char *path);

/* APPLICATION/ THEME FUNCTIONS */
char      *ewl_get_application_name();
void       ewl_set_application_name(char *name);

char      *ewl_get_theme();
void       ewl_set_theme(char *theme);


int         ewl_get_render_method();
void        ewl_set_render_method(char *render_method);

/* FONT/IMAGE CACHING FUNCTIONS */
int         ewl_get_image_cache();
void        ewl_set_image_cache(int cache);
int         ewl_get_font_cache();
void        ewl_set_font_cache(int cache);

#endif /* _API_H_ */
