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
#include <dirent.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Thumb.h>
#include <Esmart/Esmart_Trans_X11.h>
#include <Esmart/Esmart_Draggies.h>
#include <Epsilon.h>
#include <Ecore_Ipc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "entice.h"
#include "ipc.h"
#include "image.h"
#include "keys.h"
#include "prefs.h"
#include "signals_image.h"
#include "signals_thumb.h"
#include "exif.h"

static Entice *entice = NULL;

static void entice_current_free(void);
static char *filesize_as_string(const char *filename);

/**
 * hookup_edje_signals - Add signal callbacks for entice.image edje part
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
   }
   edje_callbacks;

   char *signals[] = {
      "entice,image,current,delete",
      "entice,image,current,remove",
      "entice,image,next",
      "entice,image,prev",
      "entice,image,current,edit",
      "entice,image,current,zoom,in",
      "entice,image,current,zoom,out",
      "entice,image,current,zoom,in,focused",
      "entice,image,current,zoom,out,focused",
      "entice,image,current,zoom,reset",
      "entice,image,current,zoom,fit",
      "entice,image,current,rotate,left",
      "entice,image,current,rotate,right",
      "entice,image,current,flip,horizontal",
      "entice,image,current,flip,vertical",
      "entice,image,current,align,seek,*",
      "entice,image,current,align,drag,*",
      "entice,image,current,modified",
      "entice,image,current,save",
      "entice,thumbnail,scroll,start,next",
      "entice,thumbnail,scroll,start,prev",
      "entice,thumbnail,scroll,stop",
      "entice,window,fit,image",
      "entice,window,fullscreen",
      "entice,quit",
      "entice,debug", NULL
   };
   void (*funcs[]) (void *data, Evas_Object * o, const char *emission,
                    const char *source) =
   {
   _entice_delete_current, _entice_remove_current, _entice_image_next,
         _entice_image_prev, _entice_image_edit, _entice_zoom_in,
         _entice_zoom_out, _entice_zoom_in_focused,
         _entice_zoom_out_focused, _entice_zoom_default, _entice_zoom_fit,
         _entice_rotate_left, _entice_rotate_right,
         _entice_flip_horizontal, _entice_flip_vertical,
         _entice_image_align_seek, _entice_image_align_drag,
         _entice_image_modified, _entice_image_save,
         _entice_thumbs_scroll_next_start,
         _entice_thumbs_scroll_prev_start, _entice_thumbs_scroll_stop,
         _entice_fit_window, _entice_fullscreen, _entice_quit,
         _entice_image_signal_debug, NULL};
   count = sizeof(signals) / sizeof(char *);
   for (i = 0; i < count; i++)
      edje_object_signal_callback_add(o, signals[i], "*", funcs[i], NULL);


   edje_object_signal_callback_add(o, "drag,set", "*",
                                   _entice_image_align_drag, NULL);
   return;
}

/**
 * entice_init - Initialize entice, adding it to the passed in
 * ecore_evas
 * @ee - the ecore_evas we want to add entice to
 */
int
entice_init(Ecore_Evas * ee)
{
   int result = 0;
   int button = 1;
   int x, y, w, h;
   Entice *e = NULL;
   char *layout = NULL;
   Evas_Object *o = NULL;
   const char *str = NULL;
   Evas_Coord ew, eh;

   if ((ee) && (e = (Entice *) malloc(sizeof(Entice))))
   {
      memset(e, 0, sizeof(Entice));
      e->ee = ee;

      ecore_evas_geometry_get(ee, &x, &y, &w, &h);
      o = edje_object_add(ecore_evas_get(ee));
      if (!edje_object_file_set(o, entice_config_theme_get(), "entice"))
      {
         fprintf(stderr, "ERROR: Broken theme detected\n");
         fprintf(stderr, "Unable to find default \"entice\" group\n");
         evas_object_del(o);
         return (result);
      }

      e->exiftags = entice_exif_edje_init(o);

      evas_object_name_set(o, "EnticeEdje");
      evas_object_move(o, 0, 0);
      evas_object_resize(o, (Evas_Coord) w, (Evas_Coord) h);
      evas_object_layer_set(o, 0);

      edje_object_size_min_get(o, &ew, &eh);
      if ((ew > 0) && (eh > 0))
         ecore_evas_size_min_set(ee, (int) ew, (int) eh);

      edje_object_size_max_get(o, &ew, &eh);

      if ((ew > 0) && (eh > 0))
      {
         if (ew > INT_MAX)
            ew = INT_MAX;
         if (eh > INT_MAX)
            eh = INT_MAX;
         ecore_evas_size_max_set(ee, (int) ew, (int) eh);
      }
      hookup_edje_signals(o);
      evas_object_show(o);
      e->edje = o;

      entice_keys_callback_init(e->edje);

      e->current = evas_object_image_add(ecore_evas_get(ee));
      e->preview = evas_object_image_add(ecore_evas_get(ee));

      if (edje_object_part_exists(e->edje, "entice.thumbnail.area"))
      {
         /* initialize container */
         e->container = esmart_container_new(ecore_evas_get(ee));
         esmart_container_padding_set(e->container, 4, 4, 4, 4);
         esmart_container_spacing_set(e->container, 4);
         esmart_container_move_button_set(e->container, 2);
         if ((layout =
              edje_file_data_get(entice_config_theme_get(),
                                 "container_layout")))
         {
            esmart_container_layout_plugin_set(e->container, layout);
            free(layout);
         }
         else
            esmart_container_layout_plugin_set(e->container, "default");
         evas_object_layer_set(e->container, 0);
         evas_object_color_set(e->container, 255, 255, 255, 255);

         edje_object_part_geometry_get(e->edje, "entice.thumbnail.area", NULL,
                                       NULL, &ew, &eh);

         if (ew > eh)
         {
            esmart_container_fill_policy_set(e->container,
                                             CONTAINER_FILL_POLICY_FILL_Y |
                                             CONTAINER_FILL_POLICY_KEEP_ASPECT);
            esmart_container_direction_set(e->container, 0);
         }
         else
         {
            esmart_container_fill_policy_set(e->container,
                                             CONTAINER_FILL_POLICY_FILL_X |
                                             CONTAINER_FILL_POLICY_KEEP_ASPECT);
            esmart_container_direction_set(e->container, 1);
         }
         edje_object_part_swallow(e->edje, "entice.thumbnail.area",
                                  e->container);
      }
      if ((str = edje_object_data_get(e->edje, "entice.window.type")))
      {
         if (!strcmp(str, "shaped"))
         {
            ecore_evas_borderless_set(ee, 1);
            ecore_evas_shaped_set(ee, 1);
         }
         else if (!strcmp(str, "trans"))
         {
            o = esmart_trans_x11_new(ecore_evas_get(ee));
            evas_object_layer_set(o, 0);
            evas_object_move(o, 0, 0);
            evas_object_resize(o, (Evas_Coord) w, (Evas_Coord) h);
            evas_object_name_set(o, "trans");

            esmart_trans_x11_freshen(o, (Evas_Coord) x, (Evas_Coord) y,
                                     (Evas_Coord) w, (Evas_Coord) h);
            evas_object_show(o);
            ecore_evas_borderless_set(ee, 1);
         }
         else
         {
            o = evas_object_rectangle_add(ecore_evas_get(ee));
            evas_object_color_set(o, 255, 255, 255, 255);
            evas_object_resize(o, w, h);
            evas_object_move(o, 0, 0);
            evas_object_layer_set(o, 0);
            evas_object_show(o);
         }
      }
      if ((str = edje_object_data_get(e->edje, "entice.window.dragable")))
      {
         if (!strcmp(str, "true"))
         {
            o = esmart_draggies_new(ee);
            evas_object_resize(o, w, h);
            evas_object_move(o, 0, 0);
            evas_object_layer_set(o, -1);
            if ((str =
                 edje_object_data_get(e->edje,
                                      "entice.window.dragable.button")))
            {
               button = atoi(str);
            }
            esmart_draggies_button_set(o, button);
            evas_object_show(o);
         }
      }
   }
   entice = e;
   return (1);
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

int
entice_current_image_set(const char *file)
{
   if (entice && file)
   {
      Evas_Object *o = NULL;

      if ((o = (Evas_Object *) evas_hash_find(entice->thumb.hash, file)))
      {
         entice_thumb_load_ethumb(edje_object_part_swallow_get
                                  (o, "entice.thumb"));
         return (1);
      }
   }
   return (0);
}

Evas_Bool
entice_exiftags_foreach(Evas_Hash * hash, const char *key, void *data,
                        void *fdata)
{
   int exifintval = 0;
   char buf[PATH_MAX];
   const char *exifval = NULL;
   Entice_Exif *eexif = NULL;

#if 0
   fprintf(stderr, "%s\n", key);
#endif
   if ((eexif = (Entice_Exif *) evas_hash_find(hash, key)))
   {
      if ((exifval =
           esmart_thumb_exif_data_as_string_get((Evas_Object *) fdata,
                                                eexif->lvl, eexif->tag)))
      {
         edje_object_part_text_set(entice->edje, eexif->part, exifval);
         edje_object_signal_emit(entice->edje, "entice,exif,data,found",
                                 eexif->part);
      }
      else
         if ((exifintval =
              esmart_thumb_exif_data_as_int_get((Evas_Object *) fdata,
                                                eexif->lvl, eexif->tag)) >= 0)
      {
         snprintf(buf, PATH_MAX, "%i", exifintval);
         edje_object_part_text_set(entice->edje, eexif->part, buf);
         edje_object_signal_emit(entice->edje, "entice,exif,data,found",
                                 eexif->part);

      }
      else
      {
         edje_object_signal_emit(entice->edje, "entice,exif,data,notfound",
                                 eexif->part);
#if 0
         fprintf(stderr, "ERROR: %s : 0x%04x : 0x%04x\n", eexif->part,
                 eexif->lvl, eexif->tag);
#endif
      }

   }
   return (1);
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
   char *str = NULL;
   const char *tmpstr = NULL;
   Evas_Object *o = NULL;
   Evas_Object *tmp = NULL;
   Evas_Object *new_current = NULL, *new_scroller;
   Evas_Object *thumb_edje = NULL;

   if ((o = (Evas_Object *) _data))
   {
      int iw, ih;
      Evas_Coord w, h;
      int should_fit = 0;
      char buf[PATH_MAX];

      if ((entice->current) && entice_image_file_get(entice->current)
          && !strcmp(esmart_thumb_file_get(o),
                     entice_image_file_get(entice->current)))
         return;
      esmart_thumb_freshen(o);

      if (entice_image_file_get(entice->current)
          && (thumb_edje =
              evas_hash_find(entice->thumb.hash,
                             entice_image_file_get(entice->current))))
         edje_object_signal_emit(thumb_edje, "entice,thumb,unloaded", "");

      edje_object_signal_emit(entice->edje, "entice,image,display,before",
                              "");

      tmp =
         esmart_thumb_evas_object_get(o,
                                      entice_config_image_auto_orient_get());
      if ((new_current = entice_image_new(tmp)))
      {
         entice_image_file_set(new_current, esmart_thumb_file_get(o));
         entice_image_format_set(new_current, esmart_thumb_format_get(o));
         entice_image_save_quality_set(new_current,
                                       entice_config_image_quality_get());
         edje_object_part_geometry_get(entice->edje, "entice.image", NULL,
                                       NULL, &w, &h);
         evas_object_resize(new_current, w, h);
         evas_object_layer_set(new_current, evas_object_layer_get(o));
         evas_object_show(new_current);

         new_scroller =
            esmart_thumb_new(evas_object_evas_get(o),
                             esmart_thumb_file_get(o));

         edje_object_part_geometry_get(entice->edje, "entice.scroller", NULL,
                                       NULL, &w, &h);
         evas_object_move(new_scroller, -9999, -9999);
         evas_object_resize(new_scroller, w, h);
         evas_object_layer_set(new_scroller, evas_object_layer_get(o));
         evas_object_show(new_scroller);

         if (entice->current)
         {
            entice_image_zoom_set(new_current,
                                  entice_image_zoom_get(entice->current));
            entice_image_x_align_set(new_current,
                                     entice_image_x_align_get(entice->
                                                              current));
            entice_image_y_align_set(new_current,
                                     entice_image_y_align_get(entice->
                                                              current));
            if (entice_image_zoom_fit_get(entice->current))
               should_fit = 1;
            entice_current_free();
         }
         entice->current = new_current;
         if ((thumb_edje =
              evas_hash_find(entice->thumb.hash,
                             entice_image_file_get(entice->current))))
         {
            edje_object_signal_emit(thumb_edje, "entice,thumb,loaded", "");
            esmart_container_scroll_to(entice->container, thumb_edje);
         }

         if (entice->scroller)
            evas_object_del(entice->scroller);
         entice->scroller = new_scroller;

         /* Set the text descriptions for this image */
         esmart_thumb_geometry_get(o, &iw, &ih);
         snprintf(buf, PATH_MAX, "%d x %d", iw, ih);
         edje_object_part_text_set(entice->edje,
                                   "entice.image.current.dimensions", buf);
         edje_object_part_text_set(entice->edje,
                                   "entice.image.current.filename.full",
                                   esmart_thumb_file_get(o));
         if ((tmpstr = strrchr(esmart_thumb_file_get(o), '/')))
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.filename.short",
                                      tmpstr + 1);
         if ((str = filesize_as_string(esmart_thumb_file_get(o))))
         {
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.filesize", str);
            free(str);
         }
         if (esmart_thumb_exif_get(o))
         {
            evas_hash_foreach(entice->exiftags, entice_exiftags_foreach, o);
            edje_object_signal_emit(entice->edje, "entice,image,exif,present",
                                    "");
         }
         else
         {
            edje_object_signal_emit(entice->edje, "entice,image,exif,absent",
                                    "");
         }
         snprintf(buf, PATH_MAX, "Entice: %s", esmart_thumb_file_get(o));
         ecore_evas_title_set(entice->ee, buf);

         entice->thumb.current =
            evas_list_find_list(entice->thumb.list, _data);

         /* swallow the images */
         edje_object_part_swallow(entice->edje, "entice.image", new_current);
         edje_object_part_swallow(entice->edje, "entice.scroller",
                                  new_scroller);

         if (should_fit)
            entice_image_zoom_fit(new_current);

         /* let the app know it's ready to be displayed */
         edje_object_signal_emit(entice->edje, "entice,image,display", "");
      }
      else
      {
         fprintf(stderr, "Error Loading Source Image !!!!\n");
      }
   }
   return;
   _e = NULL;
   _o = NULL;
   _ev = NULL;
}

/**
 * entice_file_add - add the named file to our list of possible files
 * @file - the FULL path to the image
 */
int
entice_file_add(const char *file)
{
   int result = 0;
   Evas_Object *o = NULL, *edje = NULL;
   char buf[PATH_MAX];

   if (file)
   {
      snprintf(buf, PATH_MAX, "%s", file);
      if ((evas_hash_find(entice->thumb.hash, buf)) == NULL)
      {
         if ((o = esmart_thumb_new(ecore_evas_get(entice->ee), buf)))
         {
            evas_object_layer_set(o,
                                  evas_object_layer_get(entice->container));
            evas_object_move(o, -9999, -9999);
            entice->thumb.list = evas_list_append(entice->thumb.list, o);
            evas_object_show(o);

            edje = edje_object_add(ecore_evas_get(entice->ee));
            if (edje_object_file_set
                (edje, entice_config_theme_get(), "entice.thumb"))
            {
               evas_object_layer_set(edje,
                                     evas_object_layer_get(entice->
                                                           container));
               if (edje_object_part_exists(edje, "entice.thumb"))
               {
                  evas_object_resize(o, (Evas_Coord) 48, (Evas_Coord) 48);
                  hookup_entice_thumb_signals(edje, o);
                  evas_object_layer_set(o,
                                        evas_object_layer_get(entice->
                                                              container));
                  edje_object_part_swallow(edje, "entice.thumb", o);

                  esmart_container_element_append(entice->container, edje);
                  entice->thumb.hash =
                     evas_hash_add(entice->thumb.hash, buf, edje);
                  evas_object_show(edje);


               }
               else
               {
                  fprintf(stderr,
                          "Broken Theme!!! You didn't define an "
                          "entice.thumb part\n");
                  result = 1;
               }
            }
            else
            {
               fprintf(stderr,
                       "Broken Theme!!! You didn't define an "
                       "entice.thumb group\n");
               result = 1;
            }
         }
         else
         {
            result = 1;
         }
         if (result)
         {
            evas_object_del(edje);
         }
      }
      else
         result = 2;
   }
   else
      result = 3;

   if (evas_list_count(entice->thumb.list) == 1)
      _entice_thumb_load(o, NULL, NULL, NULL);
   return (result);
}

int
entice_file_is_dir(char *file)
{
   struct stat st;

   if (file && !stat(file, &st)&& S_ISDIR(st.st_mode))
     return 1;
   return 0;
}

void
entice_file_add_job_dir(Ecore_Ipc_Server *server, const char *dirname)
{
   DIR *d = NULL;
   struct dirent *dent = NULL;
   char buf[PATH_MAX];

   if (!dirname || ! (d = opendir(dirname)))
     return;
   while ((dent = readdir(d)))
     {
      	if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")
	      || (dent->d_name[0] == '.'))
	  continue;
	snprintf(buf, PATH_MAX, "%s/%s", dirname, dent->d_name);
	if (!entice_file_is_dir(buf))
	  entice_file_add_job(server, buf, IPC_FILE_APPEND);
     }
   closedir(d);
}

/**
 * entice_file_add_job - generate the cached thumb and send an ipc
 * message telling entice to load it
 * @data - the full or relative path to the file we want to cache
 * @add_type - whether to display or show the image
 */
void
entice_file_add_job(Ecore_Ipc_Server *server, const char *filename, 
      int add_type)
{
   Epsilon *e = NULL;
   char buf[PATH_MAX];

   if (!filename)
     return;

   if (filename[0] == '/')
     snprintf(buf, PATH_MAX-1, "%s", filename);
   else if ((strlen(filename) > 7) && !strncmp(filename, "http://", 7))
     fprintf(stderr, "Entice Compiled without http loading support\n");
   else
     {
      	char mycwd[PATH_MAX];
	memset(mycwd, 0, sizeof(mycwd));
	if (getcwd(mycwd, PATH_MAX))
	  snprintf(buf, PATH_MAX, "%s/%s", mycwd, filename);
     }

   if (entice_file_is_dir(buf))
     {
      	entice_file_add_job_dir(server, buf);
	return;
     }

   entice_ipc_client_request_image_load(server, buf, add_type);

   epsilon_init();
   if (!(e = epsilon_new(buf)) || (epsilon_exists(e) == EPSILON_FAIL &&
	 epsilon_generate(e) == EPSILON_FAIL))
     fprintf(stderr, "Unable to thumbnail %s\n", filename);

   epsilon_free(e);
}

static void
entice_current_free(void)
{
   if (entice && entice->edje && entice->current)
   {
      Evas_Object *swallowed = NULL;

      if (entice->current)
         evas_object_del(entice->current);
      entice->current = evas_object_image_add(ecore_evas_get(entice->ee));

      /* clean up the old images */
      swallowed = edje_object_part_swallow_get(entice->edje, "entice.image");
      if (swallowed)
      {
         edje_object_part_unswallow(entice->edje, swallowed);
         evas_object_del(swallowed);
         swallowed = NULL;
      }
      swallowed =
         edje_object_part_swallow_get(entice->edje, "entice.scroller");
      if (swallowed)
      {
         edje_object_part_unswallow(entice->edje, swallowed);
         evas_object_del(swallowed);
         swallowed = NULL;
      }
   }

}

/**
 * entice_file_remove - remove the file from our image list
 * @file - the filename we want to nuke
 */
int
entice_file_remove(const char *file)
{
   int result = 0;
   char buf[PATH_MAX];
   Evas_Object *o = NULL, *obj = NULL;

   if (file)
   {
      snprintf(buf, PATH_MAX, "%s", file);
      if ((o = evas_hash_find(entice->thumb.hash, buf)))
      {
         entice->thumb.hash = evas_hash_del(entice->thumb.hash, buf, o);

         /* scroll backwards in the list, if we're at the tail */
         if (evas_list_count(entice->thumb.list) > 2)
         {
            if (entice->thumb.current == evas_list_last(entice->thumb.list))
               entice->thumb.current = evas_list_prev(entice->thumb.current);
         }
         if (!(entice->thumb.current = evas_list_prev(entice->thumb.current)))
         {
            entice->thumb.current = evas_list_last(entice->thumb.list);
         }
         if ((obj = edje_object_part_swallow_get(o, "entice.thumb")))
         {
            entice->thumb.list = evas_list_remove(entice->thumb.list, obj);
            evas_object_del(obj);
         }
         esmart_container_element_remove(entice->container, o);
         evas_object_del(o);
         if (evas_list_count(entice->thumb.list) == 0)
         {
            entice->thumb.current = NULL;
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.filename.full",
                                      "");
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.filename.short",
                                      "");
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.filesize",
                                      "0 KB");
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.dimensions",
                                      "0x0");
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.height", "");
            edje_object_part_text_set(entice->edje,
                                      "entice.image.current.width", "");
            entice->thumb.list = evas_list_free(entice->thumb.list);
            entice_current_free();
         }
      }
      else
         result = 1;
   }
   else
      result = 2;
   return (result);
}

/**
 * entice_file_delete - delete the file from our list, and off disk
 * @file - the filename of the image we wanna nuke
 */
int
entice_file_delete(const char *file)
{
   int result = 0;
   char buf[PATH_MAX];

   if (file)
   {
      snprintf(buf, PATH_MAX, "%s", file);
      result = entice_file_remove(buf);
      if (!result)
         unlink(buf);
   }
   else
      result = 2;
   return (result);

}

void
entice_delete_current(void)
{
   if (entice && entice->current)
   {
      int result = 0;

      edje_object_freeze(entice->edje);
      result = entice_file_delete(entice_image_file_get(entice->current));
      edje_object_thaw(entice->edje);
      if (!result)
         edje_object_signal_emit(entice->edje, "entice,image,next", "");
   }
}
void
entice_remove_current(void)
{
   if (entice && entice->current)
   {
      int result = 0;

      edje_object_freeze(entice->edje);
      result = entice_file_remove(entice_image_file_get(entice->current));
      edje_object_thaw(entice->edje);
      if (!result)
         edje_object_signal_emit(entice->edje, "entice,image,next", "");
   }

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
         l = evas_list_last(entice->thumb.list);
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
      esmart_container_scroll_start(entice->container, 1.0);
   }
}
void
entice_thumbs_scroll_prev_start(void)
{
   if (entice && entice->container)
      esmart_container_scroll_start(entice->container, -1.0);
}

void
entice_thumbs_scroll_stop(void)
{
   if (entice && entice->container)
   {
      esmart_container_scroll_stop(entice->container);
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
   int ex = 0, ey = 0, ew = 0, eh = 0;
   Evas_Object *o = NULL;

   if (entice && entice->edje && entice->current)
   {
      Evas_Coord ww, hh;

      evas_object_resize(entice->edje, (Evas_Coord) w, (Evas_Coord) h);
      edje_object_part_geometry_get(entice->edje, "entice.image", NULL, NULL,
                                    &ww, &hh);
      evas_object_resize(entice->current, ww, hh);
      if ((o = evas_object_name_find(ecore_evas_get(entice->ee), "trans")))
      {
         ecore_evas_geometry_get(entice->ee, &ex, &ey, &ew, &eh);
         esmart_trans_x11_freshen(o, (Evas_Coord) ex, (Evas_Coord) ey,
                                  (Evas_Coord) ew, (Evas_Coord) eh);
      }
   }
}

void
entice_preview_thumb(Evas_Object * o)
{
   Evas_Object *swallowed = NULL;
   Evas_Object *newpreview = NULL;

   if (o && entice && entice->edje)
   {
      Evas_Coord x, y, w, h;

      if (esmart_thumb_freshen(o) == EPSILON_OK)
      {
         if (entice->preview)
         {
            swallowed =
               edje_object_part_swallow_get(entice->edje, "entice.preview");
            if (swallowed)
            {
               edje_object_part_unswallow(entice->edje, swallowed);
               evas_object_del(swallowed);
            }
         }
      }
      else
      {
         if (entice->preview)
         {
            swallowed =
               edje_object_part_swallow_get(entice->edje, "entice.preview");
            if (swallowed)
            {
               /* don't repreview the same image if cache is same */
               if (!strcmp
                   (esmart_thumb_file_get(o),
                    esmart_thumb_file_get(swallowed)))
                  return;
               edje_object_part_unswallow(entice->edje, swallowed);
               evas_object_del(swallowed);
            }
         }
      }
      if ((newpreview =
           esmart_thumb_new(evas_object_evas_get(o),
                            esmart_thumb_file_get(o))))
      {

         edje_object_signal_emit(entice->edje, "entice,preview,before", "");
         edje_object_part_geometry_get(entice->edje, "entice.preview", &x, &y,
                                       &w, &h);

         /* 
          * make it almost inivisble before swallowing, without this we get
          * an artifact immediately before a preview request happens 
          */
         evas_object_move(newpreview, -50, -50);
         evas_object_resize(newpreview, (Evas_Coord) 48, (Evas_Coord) 48);
         edje_object_part_swallow(entice->edje, "entice.preview", newpreview);
         evas_object_show(newpreview);

         edje_object_signal_emit(entice->edje, "entice,preview,show", "");
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
         edje_object_signal_emit(entice->edje, "entice,image,modified", "");
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
         edje_object_signal_emit(entice->edje, "entice,image,modified", "");
      edje_thaw();
   }
}
void
entice_image_edit(void)
{
   char buf[PATH_MAX];

   if (entice && entice->current)
   {
      if (entice_config_editor_get()
          && entice_image_file_get(entice->current))
      {
         snprintf(buf, PATH_MAX, "%s %s", entice_config_editor_get(),
                  entice_image_file_get(entice->current));
         ecore_exe_run(buf, NULL);
         edje_object_signal_emit(entice->edje,
                                 "entice,image,current,edit,sent", "");
      }
   }
}
void
entice_flip_vertical(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if (entice_image_flip(entice->current, 1))
         edje_object_signal_emit(entice->edje, "entice,image,modified", "");
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
         edje_object_signal_emit(entice->edje, "entice,image,modified", "");
      edje_thaw();
   }
}
void
entice_save_image(void)
{
   if (entice && entice->current)
   {
      edje_freeze();
      if (entice_image_save(entice->current))
         edje_object_signal_emit(entice->edje, "entice,image,save,ok", "");
      else
         edje_object_signal_emit(entice->edje, "entice,image,save,fail", "");
      edje_thaw();
   }
}
void
entice_thumb_load_ethumb(Evas_Object * o)
{
   if (entice && entice->edje && o)
   {
      _entice_thumb_load(o, NULL, NULL, NULL);
   }
}
void
entice_image_horizontal_align_set(double align)
{
   Evas_Coord w, h;

   if (entice && entice->edje && entice->current)
   {
      evas_object_geometry_get(entice->current, NULL, NULL, &w, &h);
      entice_image_x_align_set(entice->current, align);
      edje_object_part_drag_value_set(entice->edje,
                                      "entice.image.scroll.horizontal.scrollbar",
                                      align, 0.0);
      evas_object_resize(entice->current, w, h);
   }
}
void
entice_image_vertical_align_set(double align)
{
   Evas_Coord w, h;

   if (entice && entice->edje && entice->current)
   {
      evas_object_geometry_get(entice->current, NULL, NULL, &w, &h);
      entice_image_y_align_set(entice->current, align);
      edje_object_part_drag_value_set(entice->edje,
                                      "entice.image.scroll.vertical.scrollbar",
                                      0.0, align);
      evas_object_resize(entice->current, w, h);
   }
}

static char *
filesize_as_string(const char *filename)
{
   char *str = NULL;
   struct stat file;

   if (!stat(filename, &file))
   {
      char buf[PATH_MAX];
      int depth = 0;
      float remainder = 0.0;
      int bytes = (int) file.st_size;

      char *types[] = {
         "Bytes",
         "KB",
         "MB",
         "GB",
         "TB"
      };

      while (bytes > 1024)
      {
         int c;

         c = bytes % 1024;
         remainder += ((float) c / 1024.0);
         bytes = bytes / 1024;
         depth++;
      }
      remainder += (float) bytes;
      snprintf(buf, PATH_MAX, "%0.2f %s", remainder, types[depth]);
      str = strdup(buf);
   }
   else
   {
      fprintf(stderr, "Error stating %s\n", filename);

   }
   return (str);
}
