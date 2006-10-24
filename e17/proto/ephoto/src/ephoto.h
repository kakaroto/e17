#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Ewl.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "config.h"

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

char *sgettext(const char *s);
#define S_(str) sgettext(str)

/* Main Window Calls */
void destroy_cb(Ewl_Widget *w, void *event, void *data);
void populate_browser(Ewl_Widget *w, void *event, void *data);
void populate_images(Ewl_Widget *w, void *event, void *data);
void add_album(Ewl_Widget *w, void *event, void *data);
void go_up(Ewl_Widget *w, void *event, void *data);
void go_home(Ewl_Widget *w, void *event, void *data);
void go_favorites(Ewl_Widget *w, void *event, void *data);
void entry_change(Ewl_Widget *w, void *event, void *data);
void create_slideshow_config(Ewl_Widget *w, void *event, void *data);
void start_slideshow(Ewl_Widget *w, void *event, void *data);
void view_images(Ewl_Widget *w, void *event, void *data);

/* Databasing calls */
sqlite3 *ephoto_db_init(void);
void ephoto_db_close(sqlite3 *db);
void ephoto_db_add_album(sqlite3 *db, char *name, char *description);
void ephoto_db_list_albums(sqlite3 *db);

typedef struct _Main Main;

struct _Main
{
 Ewl_Widget *win;
 Ewl_Widget *entry;
 Ewl_Widget *browser;
 Ewl_Widget *albums;
 Ewl_Widget *viewer;
 Ewl_Widget *viewer_freebox;
};

extern Main *m;
extern char *current_directory;
extern Ecore_List *current_thumbs;
extern Ewl_Widget *vwin;

#endif
