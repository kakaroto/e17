#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include "config.h"
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <Ewl.h>
#include <fnmatch.h>

#ifdef BUILD_EXIF_SUPPORT
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>
#endif

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
#include <libintl.h>
#include <locale.h>
#define _(str) gettext(str)
#define gettext_noop(str) str
#define N_(str) gettext_noop(str)
#else
#define _(str) (str)
#define gettext_noop(str) str
#define N_(str) gettext_noop(str)
#define gettext(str) ((char*) (str))
#endif

/* NLS callbacks */
char *sgettext(const char *msgid);
#define S_(str) sgettext(str)

/* Ephoto Main gui callbacks */
void create_main_gui(void);

/* Ephoto Browsing */
Ecore_List *get_directories(const char *directory);
Ecore_List *get_images(const char *directory);

/* Ephoto Exif */
#ifdef BUILD_EXIF_SUPPORT
Ecore_Hash *get_exif_data(const char *file);
void display_exif_dialog(Ewl_Widget *w, void *event, void *data);
#endif

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
Ewl_Widget *add_box(Ewl_Widget *c, Ewl_Orientation orientation, int spacing);
Ewl_Widget *add_button(Ewl_Widget *c, const char *txt, const char *img, void *cb, void *data);
Ewl_Widget *add_entry(Ewl_Widget *c, const char *txt, void *cb, void *data);
Ewl_Widget *add_icon(Ewl_Widget *c, const char *lbl, const char *img, int thumbnail, void *cb, void *data);
Ewl_Widget *add_image(Ewl_Widget *c, const char *img, int thumbnail, void *cb, void *data);
Ewl_Widget *add_label(Ewl_Widget *c, const char *lbl);
Ewl_Widget *add_menubar(Ewl_Widget *c);
Ewl_Widget *add_menu(Ewl_Widget *c, const char *lbl);
Ewl_Widget *add_menu_item(Ewl_Widget *c, const char *lbl, const char *img, void *cb, void *data);
Ewl_Widget *add_text(Ewl_Widget *c, const char *text);
Ewl_Widget *add_shadow(Ewl_Widget *c);
Ewl_Widget *add_window(const char *name, int width, int height, void *cb, void *data);

/* Ephoto Imaging */
unsigned int *flip_horizontal(Ewl_Widget *image);
unsigned int *flip_vertical(Ewl_Widget *image);
unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h, unsigned int *data);

/* Ephoto Edit View */
Ewl_Widget *add_edit_view(Ewl_Widget *c);

/* Ephoto List View */
Ewl_Widget *add_list_view(Ewl_Widget *c);
Ewl_Widget *add_ltree(Ewl_Widget *c);

/* Ephoto Normal View */
Ewl_Widget *add_normal_view(Ewl_Widget *c);
void freebox_image_clicked(Ewl_Widget *w, void *event, void *data);

/* Ephoto Single View */
Ewl_Widget *add_single_view(Ewl_Widget *c);

/* Ephoto Slideshow */
void start_slideshow(Ewl_Widget *w, void *event, void *data);

/* Ephoto Utilities*/
const char *file_size_get(int size);
const char *image_pixels_string_get(const char *file);
void image_pixels_int_get(const char *file, int *width, int *height);

/* Ephoto Main View */
void show_main_view(Ewl_Widget *w, void *event, void *data);
void show_edit_view(Ewl_Widget *w, void *event, void *data);

/* Ephoto Viewer Views */
void show_normal_view(Ewl_Widget *w, void *event, void *data);
void show_list_view(Ewl_Widget *w, void *event, void *data);
void show_single_view(Ewl_Widget *w, void *event, void *data);

/* Ephoto Global Variables */
typedef struct _Ephoto_Main Ephoto_Main;

struct _Ephoto_Main
{
	char *current_directory;
	Ecore_List *albums;
	Ecore_List *directories;
	Ecore_List *images;
	Ewl_Widget *atree;
	Ewl_Widget *browser;
	Ewl_Widget *dtree;
	Ewl_Widget *edit_vbox;
	Ewl_Widget *eimage;
	Ewl_Widget *fbox_vbox;
	Ewl_Widget *fbox;
	Ewl_Widget *list_vbox;
	Ewl_Widget *ltree;
	Ewl_Widget *main_nb;
	Ewl_Widget *main_vbox;
	Ewl_Widget *simage;
	Ewl_Widget *single_vbox;
	Ewl_Widget *toolbar;
	Ewl_Widget *view;
	Ewl_Widget *view_box;
	Ewl_Widget *win;
	sqlite3 *db;
};

extern Ephoto_Main *em;

#endif
