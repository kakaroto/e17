/**
 * Filename: entice.c
 * Corey Donohoe <atmos@atmos.org>
 * October 11, 2003
 * Description: entice.c is the function definitions for the functionality
 * in entice.  
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <Esmart/container.h>
#include <Esmart/E_Thumb.h>
#include <string.h>
#include "entice.h"
#include "ipc.h"
#include "image.h"
#include "keys.h"
#include "prefs.h"
#include "signals_image.h"
#include "signals_thumb.h"

static Entice *entice = NULL;


/**
 * hookup_edje_signals - Add signal callbacks for EnticeImage edje part
 * @o - a validly loaded edje object
 */
static void
hookup_edje_signals(Evas_Object * o)
{
   int i, count;
   typedef struct
   {
      void (*func) (void *data, Evas_Object * o, const char *emission,
                    const char *source);
   } edje_callbacks;

   char *signals[] = { "EnticeDeleteCurrent", "EnticeRemoveCurrent",
      "EnticeImageNext", "EnticeImagePrev",
      "EnticeZoomIn", "EnticeZoomOut",
      "EnticeZoomInFocused", "EnticeZoomOutFocused",
      "EnticeZoomDefault", "EnticeZoomFit",
      "EnticeFitWindow", "EnticeRotateLeft",
      "EnticeRotateRight", "EnticeFlipH",
      "EnticeFlipV", "EnticeFullScreen",
      "EnticeThumbsScrollNextStart", "EnticeThumbsScrollPrevStart",
      "EnticeThumbsScrollStop",
      "EnticeImageScrollEastStart", "EnticeImageScrollWestStart",
      "EnticeImageScrollNorthStart", "EnticeImageScrollSouthStart",
      "EnticeImageScrollStop", "EnticeImageModified",
      "EnticeSaveCurrent", "EnticeQuit"
   };
   edje_callbacks funcs[] = { _entice_delete_current, _entice_remove_current,
      _entice_image_next, _entice_image_prev,
      _entice_zoom_in, _entice_zoom_out,
      _entice_zoom_in_focused, _entice_zoom_out_focused,
      _entice_zoom_default, _entice_zoom_fit,
      _entice_fit_window, _entice_rotate_left,
      _entice_rotate_right, _entice_flip_horizontal,
      _entice_flip_vertical, _entice_fullscreen,
      _entice_thumbs_scroll_next_start, _entice_thumbs_scroll_prev_start,
      _entice_thumbs_scroll_stop,
      _entice_image_scroll_east_start, _entice_image_scroll_west_start,
      _entice_image_scroll_north_start, _entice_image_scroll_south_start,
      _entice_image_scroll_stop, _entice_image_modified,
      _entice_image_save, _entice_quit, NULL
   };
   count = sizeof(signals) / sizeof(char *);
   for (i = 0; i < count; i++)
      edje_object_signal_callback_add(o, signals[i], "", funcs[i].func, NULL);
   return;
}

/**
 * entice_init - Initialize entice, adding it to the passed in
 * ecore_evas
 * @ee - the ecore_evas we want to add entice to
 */
void
entice_init(Ecore_Evas * ee)
{
   Entice *e = NULL;
   Evas_Object *o = NULL;

   if ((ee) && (e = (Entice *) malloc(sizeof(Entice))))
   {
      memset(e, 0, sizeof(Entice));
      e->ee = ee;

      e_thumb_init();
      o = edje_object_add(ecore_evas_get(ee));
      edje_object_file_set(o, entice_config_theme_get(), "Entice");
      evas_object_name_set(o, "EnticeEdje");
      evas_object_move(o, 0, 0);
      hookup_edje_signals(o);
      evas_object_show(o);
      e->edje = o;

      entice_keys_callback_init(e->edje);

      e->current = evas_object_image_add(ecore_evas_get(ee));
      e->preview = evas_object_image_add(ecore_evas_get(ee));

      /* initialize container */
      e->container = e_container_new(ecore_evas_get(ee));
      e_container_padding_set(e->container, 4, 4, 4, 4);
      e_container_spacing_set(e->container, 4);
      e_container_move_button_set(e->container, 2);
      if (edje_object_part_exists(e->edje, "EnticeThumbnailArea"))
      {
         double w, h;

         edje_object_part_geometry_get(e->edje, "EnticeThumbnailArea", NULL,
                                       NULL, &w, &h);

         if (w > h)
         {
            e_container_fill_policy_set(e->container,
                                        CONTAINER_FILL_POLICY_FILL_Y |
                                        CONTAINER_FILL_POLICY_KEEP_ASPECT);
            e_container_direction_set(e->container, 0);
         }
         else
         {
            e_container_fill_policy_set(e->container,
                                        CONTAINER_FILL_POLICY_FILL_X |
                                        CONTAINER_FILL_POLICY_KEEP_ASPECT);
            e_container_direction_set(e->container, 1);
         }
         edje_object_part_swallow(e->edje, "EnticeThumbnailArea",
                                  e->container);
      }
   }
   entice = e;
}

/**
 * entice_free - free all the stuff entice allocated
 * NOTE: Make sure you call this after ecore_main_loop_quit()
 */
void
entice_free(void)
{
   if (entice)
   {
      Evas_List *l = NULL;

      edje_freeze();
      for (l = entice->thumb.list; l; l = l->next)
         evas_object_del((Evas_Object *) l->data);
      if (entice->current)
         evas_object_del(entice->current);
      if (entice->preview)
         evas_object_del(entice->preview);
      if (entice->container)
         evas_object_del(entice->container);
      if (entice->edje)
         evas_object_del(entice->edje);
      if (entice->ee)
         ecore_evas_free(entice->ee);
      free(entice);
      entice = NULL;
      entice_ipc_shutdown();
   }
}

/**
 * _entice_thumb_load - callback for loading an entice thumb
 * @_data - The E_Thumb object we're loading from, a param to the cb
 * @_e - The evas the callback object is in
 * @_o - The evas object that marshalled the callback
 * @_ev - the event type !
 */
void
_entice_thumb_load(void *_data, Evas * _e, Evas_Object * _o, void *_ev)
{
   Evas_Object *o = NULL;
   Evas_Object *tmp = NULL;
   Evas_Object *new_current = NULL, *new_scroller;

   if ((o = (Evas_Object *) _data))
   {
      int iw, ih;
      double w, h;
      int should_fit = 0;
      char buf[PATH_MAX];

      if ((entice->current) && entice_image_file_get(entice->current)
          && !strcmp(e_thumb_file_get(o),
                     entice_image_file_get(entice->current)))
         return;

      tmp = e_thumb_evas_object_get(o);
      new_current = entice_image_new(tmp);
      entice_image_file_set(new_current, e_thumb_file_get(o));
      entice_image_format_set(new_current, e_thumb_format_get(o));

      new_scroller =
         e_thumb_new(evas_object_evas_get(o), e_thumb_file_get(o));
      edje_object_part_geometry_get(entice->edje, "EnticeImage", NULL, NULL,
                                    &w, &h);
      evas_object_resize(new_current, w, h);
      evas_object_show(new_current);

      edje_object_part_geometry_get(entice->edje, "EnticeImageScroller", NULL,
                                    NULL, &w, &h);
      evas_object_resize(new_scroller, w, h);
      evas_object_show(new_scroller);

      if (entice->current)
      {
         Evas_Object *swallowed = NULL;

         entice_image_zoom_set(new_current,
                               entice_image_zoom_get(entice->current));
         if (entice_image_zoom_fit_get(entice->current))
            should_fit = 1;
         evas_object_del(entice->current);
         entice->current = NULL;

         /* clean up the old images */
         swallowed =
            edje_object_part_swallow_get(entice->preview, "EnticeImage");
         if (swallowed)
         {
            edje_object_part_unswallow(entice->current, swallowed);
            evas_object_del(swallowed);
            swallowed = NULL;
         }
         swallowed =
            edje_object_part_swallow_get(entice->current,
                                         "EnticeImageScroller");
         if (swallowed)
         {
            edje_object_part_unswallow(entice->current, swallowed);
            evas_object_del(swallowed);
            swallowed = NULL;
         }
      }
      entice->current = new_current;

      if (entice->scroller)
         evas_object_del(entice->scroller);
      entice->scroller = new_scroller;

      /* Set the text descriptions for this image */
      e_thumb_geometry_get(o, &iw, &ih);
      snprintf(buf, PATH_MAX, "%d x %d", iw, ih);
      edje_object_part_text_set(entice->edje, "EnticeFileDimensions", buf);
      edje_object_part_text_set(entice->edje, "EnticeFileName",
                                e_thumb_file_get(o));
      snprintf(buf, PATH_MAX, "Entice: %s", e_thumb_file_get(o));
      ecore_evas_title_set(entice->ee, buf);

      entice->thumb.current = evas_list_find_list(entice->thumb.list, _data);

      /* swallow the images */
      edje_object_part_swallow(entice->edje, "EnticeImage", new_current);
      edje_object_part_swallow(entice->edje, "EnticeImageScroller",
                               new_scroller);

      if (should_fit)
         entice_image_zoom_fit(new_current);

      /* let the app know it's ready to be displayed */
      edje_object_signal_emit(entice->edje, "EnticeImageDisplay", "");
   }
   return;
   _e = NULL;
   _o = NULL;
   _ev = NULL;
}

/**
 * entice_file_add - add the named file to our list of possible files
 * @file - the FULL path to the image
 * FIXME: get rid of the realpath calls
 */
int
entice_file_add(const char *file)
{
   int result = 0;
   Evas_Object *o = NULL, *edje = NULL;
   char buf[PATH_MAX], path[PATH_MAX], *ptr = NULL;

   if (file)
   {
      snprintf(buf, PATH_MAX, "%s", file);
      if ((ptr = realpath(buf, path)))
      {
         if ((o = e_thumb_new(ecore_evas_get(entice->ee), path)))
         {
            entice->thumb.list = evas_list_append(entice->thumb.list, o);
            entice->thumb.hash = evas_hash_add(entice->thumb.hash, path, o);
            evas_object_show(o);
            evas_object_resize(o, 48, 48);

            edje = edje_object_add(ecore_evas_get(entice->ee));
            edje_object_file_set(edje, entice_config_theme_get(),
                                 "EnticeThumb");
            hookup_entice_thumb_signals(edje, o);
            edje_object_part_swallow(edje, "EnticeThumb", o);
            evas_object_show(edje);

            e_container_element_append(entice->container, edje);
         }
         else
            result = 1;
      }
      else
         result = 2;
   }
   else
      result = 3;
   return (result);
}

/**
 * entice_file_add_job_cb - generate the cached thumb and send an ipc
 * message telling entice to load it
 * @data - the full or relative path to the file we want to cache
 */
void
entice_file_add_job_cb(void *data)
{
   Evas_Object *o = NULL;
   char buf[PATH_MAX], *file = NULL;

   if (entice && entice->ee && data)
   {
      file = (char *) data;

      if (file)
      {
         if (file[0] == '/')
            snprintf(buf, PATH_MAX, "%s", file);
         else if ((strlen(file) > 7) && !strncmp(file, "http://", 7))
         {
            fprintf(stderr, "http file request\n");
         }
         else
         {
            char mycwd[PATH_MAX];

            memset(mycwd, 0, sizeof(mycwd));
            if (getcwd(mycwd, PATH_MAX))
            {
               snprintf(buf, PATH_MAX, "%s/%s", mycwd, file);
            }
         }
      }
      if ((o = e_thumb_new(ecore_evas_get(entice->ee), buf)))
         evas_object_del(o);
      entice_ipc_client_request_image_load(buf);
   }
}

/**
 * entice_fil_del - delete the file from our image list
 * @file - the filename we want to nuke
 */
int
entice_file_del(const char *file)
{
   int result = 0;
   Evas_Object *o = NULL;
   char buf[PATH_MAX], path[PATH_MAX], *ptr = NULL;

   if (file)
   {
      snprintf(buf, PATH_MAX, "%s", file);
      if ((ptr = realpath(buf, path)))
      {
         if ((o = evas_hash_find(entice->thumb.hash, path)))
         {
            entice->thumb.list = evas_list_remove(entice->thumb.list, o);
            entice->thumb.hash = evas_hash_del(entice->thumb.hash, path, o);
            /* FIXME container_vomit(o); */
#if 0
            e_thumb_free(o);
#endif
         }
         else
            result = 1;
      }
      else
         result = 2;
   }
   else
      result = 3;
   return (result);
}

/**
 * entice_file_del_from_fs - delete the file from our list, and off disk
 * @file - the filename of the image we wanna nuke
 */
int
entice_file_del_from_fs(const char *file)
{
   int result = 0;
   Evas_Object *o = NULL;
   char buf[PATH_MAX], path[PATH_MAX], *ptr = NULL;

   if (file)
   {
      snprintf(buf, PATH_MAX, "%s", file);
      if ((ptr = realpath(buf, path)))
      {
         if ((o = evas_hash_find(entice->thumb.hash, path)))
         {
            entice->thumb.list = evas_list_remove(entice->thumb.list, o);
            entice->thumb.hash = evas_hash_del(entice->thumb.hash, path, o);
            /* FIXME container_vomit(o); */
#if 0
            e_thumb_free(o);
            unlink(path);

#endif
         }
         else
            result = 1;
      }
      else
         result = 2;
   }
   else
      result = 3;
   return (result);

}

/**
 * entice_load_prev - load the previous image in the list
 */
void
entice_load_prev(void)
{
   Evas_List *l = NULL;

   if (entice && entice->thumb.current)
   {
      if (entice->thumb.current->prev)
         l = entice->thumb.current->prev;
      else
         l = entice->thumb.list->last;
      if (l->data)
         _entice_thumb_load((Evas_Object *) l->data, NULL, NULL, NULL);
   }
}

/**
 * entice_load_next - load the next image in the list
 */
void
entice_load_next(void)
{
   Evas_List *l = NULL;

   if (entice && entice->thumb.current)
   {
      if (entice->thumb.current->next)
         l = entice->thumb.current->next;
      else
         l = entice->thumb.list;
      if (l->data)
         _entice_thumb_load((Evas_Object *) l->data, NULL, NULL, NULL);
   }
}

void
entice_thumbs_scroll_next_start(void)
{
   if (entice && entice->container)
   {
      e_container_scroll_start(entice->container, 1.0);
   }
}
void
entice_thumbs_scroll_prev_start(void)
{
   if (entice && entice->container)
      e_container_scroll_start(entice->container, -1.0);
}

void
entice_thumbs_scroll_stop(void)
{
   if (entice && entice->container)
   {
      e_container_scroll_stop(entice->container);
   }
}

void
entice_zoom_in(void)
{
   if (entice && entice->current)
      entice_image_zoom_in(entice->current);
}

void
entice_zoom_out(void)
{
   if (entice && entice->current)
      entice_image_zoom_out(entice->current);
}

void
entice_zoom_fit(void)
{
   if (entice && entice->current)
      entice_image_zoom_fit(entice->current);
}

void
entice_zoom_reset(void)
{
   if (entice && entice->current)
      entice_image_zoom_set(entice->current, 1.0);
}

void
entice_fullscreen_toggle(void)
{
   if (entice && entice->ee)
   {
      if (ecore_evas_fullscreen_get(entice->ee))
         ecore_evas_fullscreen_set(entice->ee, 0);
      else
         ecore_evas_fullscreen_set(entice->ee, 1);
   }
}

void
entice_resize(int w, int h)
{
   if (entice && entice->edje && entice->current)
   {
      double ww, hh;

      evas_object_resize(entice->edje, (double) w, (double) h);
      edje_object_part_geometry_get(entice->edje, "EnticeImage", NULL, NULL,
                                    &ww, &hh);
      evas_object_resize(entice->current, ww, hh);
   }
}

void
entice_main_image_scroll_east_start(void)
{
   if (entice && entice->edje && entice->current)
      entice_image_scroll_start(entice->current, ENTICE_SCROLL_EAST);
}

void
entice_main_image_scroll_west_start(void)
{
   if (entice && entice->edje && entice->current)
      entice_image_scroll_start(entice->current, ENTICE_SCROLL_WEST);
}

void
entice_main_image_scroll_north_start(void)
{
   if (entice && entice->edje && entice->current)
      entice_image_scroll_start(entice->current, ENTICE_SCROLL_NORTH);
}

void
entice_main_image_scroll_south_start(void)
{
   if (entice && entice->edje && entice->current)
      entice_image_scroll_start(entice->current, ENTICE_SCROLL_SOUTH);
}

void
entice_main_image_scroll_stop(void)
{
   if (entice && entice->edje && entice->current)
      entice_image_scroll_stop(entice->current);
}

void
entice_preview_thumb(Evas_Object * o)
{
   Evas_Object *swallowed = NULL;
   Evas_Object *newpreview = NULL;

   if (o && entice && entice->edje)
   {
      double x, y, w, h;

      if (entice->preview)
      {
         swallowed =
            edje_object_part_swallow_get(entice->edje, "EnticeImagePreview");
         if (swallowed)
         {
            /* don't repreview the same image */
            if (!strcmp(e_thumb_file_get(o), e_thumb_file_get(swallowed)))
               return;
            edje_object_part_unswallow(entice->edje, swallowed);
            evas_object_del(swallowed);
         }
      }
      if ((newpreview =
           e_thumb_new(evas_object_evas_get(o), e_thumb_file_get(o))))
      {

         edje_object_signal_emit(entice->edje, "EnticeImagePreviewPrep", "");
         edje_object_part_geometry_get(entice->edje, "EnticeImagePreview", &x,
                                       &y, &w, &h);

         /* 
          * make it almost inivisble before swallowing, without this we get
          * an artifact immediately before a preview request happens 
          */
         evas_object_resize(newpreview, 2, 2);
         edje_object_part_swallow(entice->edje, "EnticeImagePreview",
                                  newpreview);
         evas_object_show(newpreview);

         edje_object_signal_emit(entice->edje, "EnticeImagePreviewShow", "");
      }
      else
      {
         fprintf(stderr, "Unable to allocate a new preview\n");
      }
   }
}

void
entice_rotate_image_right(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if (entice_image_rotate(entice->current, 1))
         edje_object_signal_emit(entice->edje, "EnticeImageModified", "");
      edje_thaw();
   }
}
void
entice_rotate_image_left(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if (entice_image_rotate(entice->current, 3))
         edje_object_signal_emit(entice->edje, "EnticeImageModified", "");
      edje_thaw();
   }
}
void
entice_flip_vertical(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if (entice_image_flip(entice->current, 1))
         edje_object_signal_emit(entice->edje, "EnticeImageModified", "");
      edje_thaw();
   }
}
void
entice_flip_horizontal(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if (entice_image_flip(entice->current, 0))
         edje_object_signal_emit(entice->edje, "EnticeImageModified", "");
      edje_thaw();
   }
}
void
entice_save_image(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if(entice_image_save(entice->current))
	  fprintf(stderr, "Saving was successul\n");
      /* FIXME: Emit a EnticeSaveOk or something signal */
      edje_thaw();
   }
}
