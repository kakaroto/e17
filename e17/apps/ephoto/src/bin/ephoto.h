#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include "config.h"
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <Ewl.h>
#include <fnmatch.h>
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>
#include <libgen.h>
#include <limits.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

/* Ephoto Main gui callbacks */
void create_main_gui(void);

/* Ephoto Browsing */
Ecore_List *get_directories(const char *directory);
Ecore_List *get_images(const char *directory);

/* Ephoto Exif */
Ecore_Hash *get_exif_data(const char *file);
void display_exif_dialog(Ewl_Widget *w, void *event, void *data);

/* Ephoto Databasing */
sqlite3 *ephoto_db_init(void);
void ephoto_db_add_album(sqlite3 *db, const char *name, const char *description);
void ephoto_db_delete_album(sqlite3 *db, const char *name);
Ecore_List *ephoto_db_list_albums(sqlite3 *db);
void ephoto_db_add_image(sqlite3 *db, const char *album, const char *name, const char *path);
void ephoto_db_delete_image(sqlite3 *db, const char *album, const char *path);
Ecore_List *ephoto_db_list_images(sqlite3 *db, const char *album);
void ephoto_db_close(sqlite3 *db);

/* Ephoto Gui */
Ewl_Widget *add_button(Ewl_Widget *c, const char *txt, const char *img, void *cb, void *data);
Ewl_Widget *add_image(Ewl_Widget *c, const char *img, int thumbnail, void *cb, void *data);
Ewl_Widget *add_label(Ewl_Widget *c, const char *lbl);
Ewl_Widget *add_text(Ewl_Widget *c, const char *text);
Ewl_Widget *add_shadow(Ewl_Widget *c);

/* Ephoto Imaging */
unsigned int *flip_horizontal(Ewl_Widget *image);
unsigned int *flip_vertical(Ewl_Widget *image);
unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h, unsigned int *data);

/* Ephoto Edit View */
Ewl_Widget *add_edit_view(Ewl_Widget *c);
void add_edit_tools(Ewl_Widget *c);

/* Ephoto List View */
Ewl_Widget *add_list_view(Ewl_Widget *c);
Ewl_Widget *add_ltree(Ewl_Widget *c);

/* Ephoto Normal View */
Ewl_Widget *add_normal_view(Ewl_Widget *c);
void set_info(Ewl_Widget *w, void *event, void *data);

/* Ephoto Slideshow */
void start_slideshow(Ewl_Widget *w, void *event, void *data);

/* Ephoto Utilities*/
const char *file_size_get(int size);
const char *image_pixels_string_get(const char *file);
void image_pixels_int_get(const char *file, int *width, int *height);

/* Ephoto Views */
void show_normal_view(Ewl_Widget *w, void *event, void *data);
void show_edit_view(Ewl_Widget *w, void *event, void *data);
void show_list_view(Ewl_Widget *w, void *event, void *data);

/* Ephoto Global Variables */
typedef struct _Ephoto_Main Ephoto_Main;

struct _Ephoto_Main
{
	char **views;
	Ecore_List *albums;
	Ecore_List *images;
	Ewl_Widget *atree; 
	Ewl_Widget *currenta;
	Ewl_Widget *currentf;
	Ewl_Widget *currentl;
	Ewl_Widget *edit_tools;
	Ewl_Widget *edit_vbox;
	Ewl_Widget *eimage;
	Ewl_Widget *fbox_vbox;
	Ewl_Widget *fbox;
	Ewl_Widget *ilabel;
	Ewl_Widget *list_vbox;
	Ewl_Widget *ltree;
	Ewl_Widget *tbar;
	Ewl_Widget *toolbar;
	Ewl_Widget *view_box;
	Ewl_Widget *win;
	sqlite3 *db;
};

extern Ephoto_Main *em;

