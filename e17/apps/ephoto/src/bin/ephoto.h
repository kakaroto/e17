#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include "config.h"
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

/* Ephoto Main gui callbacks */
void create_main_gui(void);

/* Ephoto Browsing */
Ecore_List *get_directories(char *directory);
Ecore_List *get_images(char *directory);

/* Ephoto Exif */
Ecore_Hash *get_exif_data(char *file);
void display_exif_dialog(Ewl_Widget *w, void *event, void *data);

/* Ephoto Databasing */
sqlite3 *ephoto_db_init(void);
void ephoto_db_add_album(sqlite3 *db, char *name, char *description);
void ephoto_db_delete_album(sqlite3 *db, char *name);
Ecore_List *ephoto_db_list_albums(sqlite3 *db);
void ephoto_db_add_image(sqlite3 *db, char *album, char *name, char *path);
void ephoto_db_delete_image(sqlite3 *db, char *album, char *path);
Ecore_List *ephoto_db_list_images(sqlite3 *db, char *album);
void ephoto_db_close(sqlite3 *db);

/* Ephoto Gui */
Ewl_Widget *add_button(Ewl_Widget *c, char *txt, char *img, void *cb, void *data);
Ewl_Widget *add_image(Ewl_Widget *c, char *img, int thumbnail, void *cb, void *data);
Ewl_Widget *add_label(Ewl_Widget *c, char *lbl, int blue);
Ewl_Widget *add_shadow(Ewl_Widget *c);

/* Ephoto Imaging */
unsigned int *flip_horizontal(Ewl_Widget *image);
unsigned int *flip_vertical(Ewl_Widget *image);
unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h, unsigned int *data);

/* Ephoto List View */
Ewl_Widget *add_ltree(Ewl_Widget *c);

/* Ephoto Normal View */
void set_info(Ewl_Widget *w, void *event, void *data);

/* Ephoto Utilities*/
char *image_pixels_string_get(const char *file);
void image_pixels_int_get(const char *file, int *width, int *height);
char *file_size_get(int size);

/* Ephoto Views */
void show_normal_view(Ewl_Widget *w, void *event, void *data);
void show_edit_view(Ewl_Widget *w, void *event, void *data);
void show_list_view(Ewl_Widget *w, void *event, void *data);

/* Ephoto Global Variables */
extern Ewl_Widget *atree; 
extern Ewl_Widget *tbar;
extern Ewl_Widget *vcombo; 
extern Ewl_Widget *view_box;
extern Ewl_Widget *fbox_vbox;
extern Ewl_Widget *edit_vbox;
extern Ewl_Widget *list_vbox; 
extern Ewl_Widget *fbox;
extern Ewl_Widget *eimage;
extern Ewl_Widget *ltree; 
extern Ewl_Widget *ilabel;
extern Ewl_Widget *currenta; 
extern Ewl_Widget *currentf;
extern Ewl_Widget *currenti;
extern Ecore_List *images;

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
