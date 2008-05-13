#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Eet.h>
#include <Edje.h>
#include <Ecore_Ipc.h>

#include "ipc.h"

#define THEMES PACKAGE_DATA_DIR"/data/themes"
#define FONTS  PACKAGE_DATA_DIR"/data/fonts"

extern Ecore_Evas *ee;
extern Evas       *evas;
extern Evas_Coord  scr_w;
extern Evas_Coord  scr_h;
extern int         quiet;
extern int         quitting;

void theme_init(const char *theme);
void theme_shutdown(void);
void theme_resize(void);
void theme_exit(void (*func) (void *data), void *data);
void theme_exit_abort(void);
void theme_title_set(const char *txt);
void theme_message_set(const char *txt);
void theme_progress_set(double val);
void theme_tick(void);
void theme_pulsate(void);
void theme_text_add(const char *txt);
void theme_status_set(const char *txt, int type);
void theme_text_clear();

void ipc_init(void);
void ipc_shutdown(void);
    
    
