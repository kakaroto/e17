/**
 * Filename: entice.h
 * Corey Donohoe <atmos@atmos.org>
 * October 11, 2003
 * Description: entice.h is the function declarations for wrapping
 * functionality in entice.  Any functionality entice offers should have a
 * void function with no args and a sane name representing what
 * functionality it does offer.
 */
#ifndef ENTICE_H
#define ENTICE_H

#include<Edje.h>
#include<Ecore.h>
#include<Ecore_Evas.h>
#include<Ecore_Ipc.h>

struct _Entice
{
   struct
   {
      Evas_Hash *hash;          /* filename -> EnticeThumb(edje) Mapping */
      Evas_List *list;          /* List of E_Thumb */
      Evas_List *current;       /* Current Image's list item */
   }
   thumb;
   Evas_Hash *exiftags;         /* entice exif tag -> EnticeExif */

   Ecore_Evas *ee;              /* the evas window */
   Evas_Object *edje;           /* Main Edje_Object(theme) */
   Evas_Object *current;        /* EnticeImage Object currently visible */
   Evas_Object *preview;        /* E_Thumb */
   Evas_Object *scroller;       /* E_Thumb */
   Evas_Object *container;      /* thumbnail container the edje swallows */
};
typedef struct _Entice Entice;


int entice_init(Ecore_Evas * ee);
void entice_free(void);
void entice_resize(int w, int h);

int entice_file_add(const char *file);
int entice_file_del(const char *file);
int entice_file_del_from_fs(const char *file);
int entice_current_image_set(const char *file);
void entice_file_add_job(Ecore_Ipc_Server *server, const char *filename,
      int add_type);
void entice_file_add_job_dir(Ecore_Ipc_Server *server, const char *dirname);

void entice_thumbs_scroll_next_start(void);
void entice_thumbs_scroll_prev_start(void);
void entice_thumbs_scroll_stop(void);

void entice_main_image_scroll_east_start(void);
void entice_main_image_scroll_west_start(void);
void entice_main_image_scroll_north_start(void);
void entice_main_image_scroll_south_start(void);
void entice_main_image_scroll_stop(void);

void entice_load_next(void);
void entice_load_prev(void);
void entice_zoom_in(void);
void entice_zoom_fit(void);
void entice_zoom_out(void);
void entice_zoom_reset(void);
void entice_fullscreen_toggle(void);

void entice_rotate_image_right(void);
void entice_rotate_image_left(void);
void entice_flip_vertical(void);
void entice_flip_horizontal(void);

void entice_preview_thumb(Evas_Object * o);
void entice_delete_current(void);
void entice_remove_current(void);
void entice_save_image(void);
void entice_thumb_load_ethumb(Evas_Object * o);
void entice_dragable_image_set(int state);

void entice_image_horizontal_align_set(double align);
void entice_image_vertical_align_set(double align);
void entice_image_edit(void);

#endif
