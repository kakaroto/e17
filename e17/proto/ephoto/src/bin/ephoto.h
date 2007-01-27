#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include "config.h"
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Ewl.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Main gui callbacks */
void create_main_gui(void);

/* Ephoto Browsing */
Ecore_List *get_directories(char *directory);
Ecore_List *get_images(char *directory);

/* Ephoto Databasing */
sqlite3 *ephoto_db_init(void);
void ephoto_db_add_album(sqlite3 *db, char *name, char *description);
void ephoto_db_delete_album(sqlite3 *db, char *name);
Ecore_List *ephoto_db_list_albums(sqlite3 *db);
void ephoto_db_add_image(sqlite3 *db, char *album, char *name, char *path);
void ephoto_db_delete_image(sqlite3 *db, char *album, char *path);
Ecore_List *ephoto_db_list_images(sqlite3 *db, char *album);
void ephoto_db_close(sqlite3 *db);

/* Ephoto Imaging */
unsigned int *flip_horizontal(Ewl_Widget *image);
unsigned int *flip_vertical(Ewl_Widget *image);
unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h, unsigned int *data);

/* NLS */
#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define _(str) gettext(str)
# define gettext_noop(str) str
# define N_(str) gettext_noop(str)
#else
# define _(str) (str)
# define gettext_noop(str) str
# define N_(str) gettext_noop(str)
# define gettext(str) ((char*) (str))
#endif

/* NLS callbacks */
char *sgettext(const char *msgid);
#define S_(str) sgettext(str)

#endif
