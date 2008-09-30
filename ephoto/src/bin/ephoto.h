#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "config.h"
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Edje.h>
#include <Efreet_Mime.h>
#include <Epsilon.h>
#include <Evas.h>
#include <Ewl.h>
#include <libgen.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef BUILD_EXIF_SUPPORT
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>
#endif

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

/*ephoto_albums.c*/
void show_albums(Ewl_Widget *c);
void populate_albums(Ewl_Widget *w, void *event, void *data);
void show_add_album_dialog(Ewl_Widget *w, void *event, void *data);

/*ephoto_database.c*/
void ephoto_db_init(void);
void ephoto_db_add_album(const char *name, 
				const char *description);
void ephoto_db_delete_album(const char *name);
Ecore_List *ephoto_db_list_albums();
void ephoto_db_add_images(const char *album, 
				Ecore_List *images);
void ephoto_db_delete_image(const char *album, 
				const char *path);
Ecore_List *ephoto_db_list_images(const char *album);

/*ephoto_dialogs.c*/
void about_dialog(Ewl_Widget *w, void *event, void *data);

/*ephoto_exif.c*/
#ifdef BUILD_EXIF_SUPPORT
Ecore_Hash *get_exif_data(const char *file);
void add_exif_to_container(Ewl_Widget *c);
void display_exif(Ewl_Widget *w, void *event, void *data);
#endif

/*ephoto_fsystem.c*/
void show_fsystem(Ewl_Widget *c);
void populate_fsystem(Ewl_Widget *w, void *event, void *data);

/*ephoto_gui.c*/
Ewl_Widget *add_box(Ewl_Widget *c, Ewl_Orientation o, int spacing);
Ewl_Widget *add_button(Ewl_Widget *c, char *lbl, const char *img, 
				void *cb, 
				void *data);
Ewl_Widget *add_entry(Ewl_Widget *c, char *txt, void *cb, 
				void *data);
Ewl_Widget *add_icon(Ewl_Widget *c, const char *lbl, const char *img, 
				int thumb, 
				void *cb, 
				void *data);
Ewl_Widget *add_image(Ewl_Widget *c, 
				const char *img, int thumb, 
				void *cb, 
				void *data);
Ewl_Widget *add_label(Ewl_Widget *c, const char *lbl);
Ewl_Widget *add_menubar(Ewl_Widget *c);
Ewl_Widget *add_menu(Ewl_Widget *c, const char *lbl);
Ewl_Widget *add_menu_item(Ewl_Widget *c, const char *lbl, 
				const char *img, 
				void *cb, 
				void *data);
Ewl_Widget *add_text(Ewl_Widget *c, char *text);
Ewl_Widget *add_window(const char *title, int w, int h, void *cb, 
				void *data);

/*ephoto_imaging.c*/
unsigned int *flip_horizontal(Ewl_Widget *image);
unsigned int *flip_vertical(Ewl_Widget *image);
unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
unsigned int *blur_image(Ewl_Widget *image);
unsigned int *sharpen_image(Ewl_Widget *image);
unsigned int *grayscale_image(Ewl_Widget *image);
unsigned int *sepia_image(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h, 
				unsigned int *data);

/*ephoto_import.c*/
void ephoto_import_dialog(Ewl_Widget *w, void *event, void *data);

/*ephoto_main.c*/
void create_main(void);

/*ephoto_nls.c*/
char *sgettext(const char *msgid);
#define S_(str) sgettext(str)

/*ephoto_normal_view.c*/
Ewl_Widget *add_normal_view(Ewl_Widget *c);
void freebox_image_clicked(Ewl_Widget *w, void *event, void *data);
void show_normal_view(Ewl_Widget *w, void *event, void *data);

/*ephoto_simple.c*/
void show_ephoto_simple(const char *img);

/*ephoto_single_view.c*/
Ewl_Widget *add_single_view(Ewl_Widget *c);
void show_single_view(Ewl_Widget *w, void *event, void *data);

/*ephoto_slideshow.c*/
void start_slideshow(Ewl_Widget *w, void *event, void *data);

/*ephoto_utils.c*/
const char *file_size_get(int size);
const char *image_pixels_string_get(const char *file);
void image_pixels_int_get(const char *file, int *width, 
				int *height);
Ecore_List *get_directories(const char *directory);
Ecore_List *get_images(const char *directory);

/*globals*/
typedef struct _Ephoto_Main Ephoto_Main;

struct _Ephoto_Main
{
        char *current_album;
        char *current_directory;
        Ecore_List *albums;
	Ecore_List *fsystem;
        Ecore_List *images;
        Ewl_Widget *atree;
        Ewl_Widget *ewin;
 	Ewl_Widget *fbox_vbox;
        Ewl_Widget *fbox;
        Ewl_Widget *ftree;
	Ewl_Widget *fthumb_size;
	Ewl_Widget *info;
	Ewl_Widget *scroll;
	Ewl_Widget *simage;
        Ewl_Widget *single_vbox;
        Ewl_Widget *view;
        Ewl_Widget *view_box;
	Ewl_Widget *win;
};

extern Ephoto_Main *em;

#endif
