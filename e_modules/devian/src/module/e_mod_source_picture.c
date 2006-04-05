#include "dEvian.h"

#ifdef HAVE_PICTURE

static int _cb_timer_picture_change(void *data);

static void _was_set_bg_add(Source_Picture *source, char *name);
static void _was_set_bg_purge(Source_Picture *source, int in_use);

/**
 * Add a source to a devian
 * @param devian The dEvian
 * @return 0 on success, 1 if fails
*/
int DEVIANF(source_picture_add) (DEVIANN *devian)
{
   Source_Picture *source;

   DSOURCE(("Source picture creation START"));

   source = E_NEW(Source_Picture, 1);

   source->devian = devian;
   source->picture0 = NULL;
   source->picture1 = NULL;

   /* Set new name for devian */
   DEVIANF(devian_set_id) (devian, SOURCE_PICTURE, NULL);

   /* Provide declarations */
   devian->source_info.provide_double_buf = 1;
   devian->source_info.provide_previous = 1;
   devian->source_info.provide_set_bg = 1;
   devian->source_info.allow_info_panel = 1;

   /* Actions */
   devian->source_func.timer_change = DEVIANF(source_picture_timer_change);
   devian->source_func.refresh = DEVIANF(source_picture_change);
   devian->source_func.set_bg = DEVIANF(source_picture_set_bg);
   devian->source_func.viewer = DEVIANF(source_picture_viewer);

   devian->container_func.update_actions(devian);

   source->was_set_bg = NULL;
   source->histo = NULL;
   source->histo_pos = 0;
   devian->conf->source_type = SOURCE_PICTURE;
   devian->size_policy = SIZE_POLICY_AUTO;
   devian->source = source;
   DEVIANM->source_picture_count++;

   /* Timer to change picture */
   if (devian->conf->picture_timer_active)
      DEVIANF(source_picture_timer_change) (devian, 1, 0);
   else
      DEVIANF(source_picture_timer_change) (devian, 0, 0);

   /* Attach picture */
   DEVIANF(source_picture_change) (devian, 0);

   DSOURCE(("Source picture creation OK\n"));

   return 0;
}

void DEVIANF(source_picture_del) (Source_Picture *source)
{
   DEVIANN *devian;

   devian = source->devian;

   DEVIANF(data_picture_cache_detach) (source, 0);
   DEVIANF(data_picture_cache_detach) (source, 1);

   if (source->histo)
      DEVIANF(source_picture_histo_clear) (source);

   if (source->timer)
      ecore_timer_del(source->timer);

   if (DEVIANM->conf->sources_picture_set_bg_purge)
      _was_set_bg_purge(source, 0);

   DEVIANM->source_picture_count--;

   E_FREE(source);

   DSOURCE(("Source deleted\n"));
}

/**
 * Change the picture of a picture source
 *
 * @param devian The dEvian
 * @param option The action to perform when changing the picture. 0 means default action, so random picture
 * @return 1 on success, 0 on fail
 */
int DEVIANF(source_picture_change) (DEVIANN *devian, int option)
{
   Source_Picture *source;
   int old_part;

   source = devian->source;
   old_part = DEVIANF(container_edje_part_get) (source->devian);

   /* If no option and we are in historic, show the next picture of historic */
   if (!option && source->histo_pos)
      option = -1;

   if (!DEVIANF(data_picture_cache_attach) (source, !old_part, option))
      return 0;

   if (source->devian->container)
     {
        if (!DEVIANF(container_edje_part_change) (source->devian))
           return 0;
     }
   devian->container_func.resize_auto(source->devian);

   /* Informations */
   DEVIANF(container_infos_text_change) (source->devian, NULL);

   return 1;
}

int DEVIANF(source_picture_set_bg) (DEVIANN *devian)
{
   Source_Picture *source;
   E_Zone *zone;
   char *file = NULL;
   char *name;
   char buf[4096];

   source = devian->source;
   zone = e_zone_current_get(DEVIANM->container);
   if (!zone)
      return 0;

   if (!DEVIANF(container_edje_part_get) (source->devian))
     {
        if (source->picture0)
          {
             file = source->picture0->path;
             name = source->picture0->picture_description->name;
          }
     }
   else
     {
        if (source->picture1)
          {
             file = source->picture1->path;
             name = source->picture1->picture_description->name;
          }
        else
           return 0;
     }
   if (!file)
      return 0;

   if (DEVIANM->conf->sources_picture_set_bg_purge)
      _was_set_bg_purge(source, 1);

   if (!ecore_file_exists(file))
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>File %s doesnt exists.</hilight><br><br>"
                   "This file is in dEvian's picture list, but it seems you removed it from the disk<br>"
                   "It cant be set as background, sorry."), file);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return 0;
     }

   if (!strstr(file, ".edj"))
     {
        if (ecore_file_app_installed("e17setroot"))
          {
             Ecore_Exe *exe;

             snprintf(buf, 4096, "e17setroot -s %s", file);
             DSOURCE(("Set background with %s", buf));
             exe = ecore_exe_run(buf, NULL);
             if (exe)
               {
                  ecore_exe_free(exe);
                  if (DEVIANM->conf->sources_picture_set_bg_purge)
                     _was_set_bg_add(source, name);
               }
          }
        else
          {
             snprintf(buf, sizeof(buf),
                      _("<hilight>e17setroot needed</hilight><br><br>"
                        "%s is not an edje file !<br>"
                        "dEvian need e17setroot util from e_utils package to set you're picture as background"
                        "Please install it and try again"), file);
             e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
             return 0;
          }
     }
   else
     {
        DSOURCE(("Set edje background %s", file));

        if ((zone->container->num == 0) && (zone->num == 0) && (zone->desk_x_current == 0) && (zone->desk_y_current == 0))
          {
             e_lib_background_set(strdup(file));
          }
        else
          {
             e_lib_desktop_background_del(zone->container->num, zone->num, zone->desk_x_current, zone->desk_y_current);
             e_lib_desktop_background_add(zone->container->num, zone->num, zone->desk_x_current, zone->desk_y_current,
                                          strdup(file));
          }
     }

   return 1;
}

int DEVIANF(source_picture_viewer) (DEVIANN *devian)
{
   Source_Picture *source;
   char *file;
   char buf[4096];

   source = devian->source;

   if (!DEVIANF(container_edje_part_get) (source->devian))
     {
        if (source->picture0)
          {
             file = source->picture0->path;
          }
        else
           return 0;
     }
   else
     {
        if (source->picture1)
          {
             file = source->picture1->path;
          }
        else
           return 0;
     }
   if (!file)
      return 0;
   if (!ecore_file_exists(file))
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>File %s doesnt exists !</hilight><br><br>"
                   "This file is in dEvian's picture list, but it seems you removed it from the disk.<br>"
                   "It cant be set opened in a viewer, sorry."), file);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return 0;
     }

   if (ecore_file_app_installed(DEVIANM->conf->viewer_image))
     {
        Ecore_Exe *exe;

        snprintf(buf, 4096, "%s %s", DEVIANM->conf->viewer_image, file);
        DSOURCE(("Viewer: %s", buf));
        exe = ecore_exe_run(buf, NULL);
        if (exe)
           ecore_exe_free(exe);
     }
   else
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>Viewer %s not found !</hilight><br><br>"
                   "You can change the viewer for images in dEvian configuration panel (Advanced view)"),
                 DEVIANM->conf->viewer_image);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return 0;
     }

   return 1;
}

int DEVIANF(source_picture_evas_object_get) (DEVIANN *devian, Evas_Object **picture0, Evas_Object **picture1)
{
   Source_Picture *source;

   source = (Source_Picture *)devian->source;

   if ((!source->picture0) && (!source->picture1))
      return 0;

   if (source->picture0)
      *picture0 = source->picture0->picture;
   else
      *picture0 = NULL;
   if (source->picture1)
      *picture1 = source->picture1->picture;
   else
      *picture1 = NULL;

   return 1;
}

int DEVIANF(source_picture_original_size_get) (Source_Picture *source, int part, int *w, int *h)
{
   if (!source)
      return 0;

   if (part == -1)
     {
        if ((part = DEVIANF(container_edje_part_get) (source->devian)) == -1)
           return 0;
     }

   if (!part && source->picture0)
     {
        *w = source->picture0->original_w;
        *h = source->picture0->original_h;
        return 1;
     }

   if (part && source->picture1)
     {
        *w = source->picture1->original_w;
        *h = source->picture1->original_h;
        return 1;
     }

   return 0;
}

void DEVIANF(source_picture_dialog_infos) (Source_Picture *source)
{
   E_Dialog *dia;
   char buf[4096];
   char *path = NULL;
   int edje_part;

   edje_part = DEVIANF(container_edje_part_get) (source->devian);

   if (!edje_part)
     {
        if (source->picture0)
           path = source->picture0->path;
     }
   else
     {
        if (source->picture1)
           path = source->picture1->path;
     }

   if (!path)
      return;

   dia = e_dialog_new(DEVIANM->container);
   if (!dia)
      return;

   snprintf(buf, 4096,
            "<hilight>Informations on %s</hilight><br><br>"
            "<underline=on underline_color=#000>Picture path :</> %s<br>"
            "<br>"
            "<underline=on underline_color=#000>Total pictures in local list :</> %d",
            DEVIANF(source_picture_name_get) (source, edje_part), path, evas_list_count(DEVIANM->picture_list_local->pictures));
   e_dialog_title_set(dia, MODULE_NAME " Module - Picture Informations");
   e_dialog_icon_set(dia, "enlightenment/e", 64);
   e_dialog_text_set(dia, buf);
   e_dialog_button_add(dia, _("Ok"), NULL, NULL, NULL);
   e_win_centered_set(dia->win, 1);
   e_dialog_show(dia);

}

char *DEVIANF(source_picture_name_get) (Source_Picture *source, int part)
{
   char *name;

   name = NULL;

   if (part == -1)
     {
        part = DEVIANF(container_edje_part_get) (source->devian);
     }

   if (!part && source->picture0)
      name = source->picture0->picture_description->name;

   if (part && source->picture1)
      name = source->picture1->picture_description->name;

   return name;
}

char *DEVIANF(source_picture_infos_get) (Source_Picture *source, int part)
{
   Picture *picture;
   char buf[4096], buf_ext[4096];
   char *path;

   if (part == -1)
      part = DEVIANF(container_edje_part_get) (source->devian);

   if (!part)
      picture = source->picture0;
   else
      picture = source->picture1;

   if (!picture)
      return NULL;

   if (ecore_file_exists(picture->path))
     {
        time_t date;
        char *date_ascii;

        date = ecore_file_mod_time(picture->path);
        date_ascii = (char *)ctime(&date);
        date_ascii[strlen(date_ascii) - 1] = ' ';
        snprintf(buf_ext, sizeof(buf_ext),
                 "<underline=on underline_color=#000>Date :</> %s<br>"
                 "<underline=on underline_color=#000>Size :</> %.2fMo",
                 date_ascii, (float)ecore_file_size(picture->path) / 1000000.0);
     }
   else
     {
        snprintf(buf_ext, sizeof(buf_ext), "This file does not exist anymore on the disk !");
     }

   //path = ecore_txt_convert("iso-8859-1", "UTF-8", picture->path);
   path = picture->path;
   snprintf(buf, sizeof(buf),
            "<underline=on underline_color=#000>%s</><br><br>"
            "<underline=on underline_color=#000>Path :</> %s<br><br>" "%s", picture->picture_description->name, path, buf_ext);
   //E_FREE(path);

   return strdup(buf);
}

void DEVIANF(source_picture_timer_change) (DEVIANN *devian, int active, int time)
{
   Source_Picture *source;

   source = devian->source;

   if (time && (time < SOURCE_PICTURE_TIMER_MIN))
      return;

   devian->conf->picture_timer_active = active;
   if (!time)
      time = devian->conf->picture_timer_s;
   else
      devian->conf->picture_timer_s = time;

   e_config_save_queue();

   if (!active)
     {
        devian->source_info.paused = 1;
        devian->container_func.update_actions(devian);
        if (source->timer)
          {
             ecore_timer_del(source->timer);
             source->timer = NULL;
          }

        return;
     }
   devian->source_info.paused = 0;
   devian->container_func.update_actions(devian);

   if (source->timer)
      ecore_timer_del(source->timer);
   source->timer = ecore_timer_add(time, _cb_timer_picture_change, source);
}

/**                                                                              
 * Attach a picture to a source historic as first element                        
 */
void DEVIANF(source_picture_histo_picture_attach) (Source_Picture *source, Picture *picture)
{
   source->histo = evas_list_prepend(source->histo, picture);
   source->histo_pos = 0;

   if (!evas_list_find(picture->sources_histo, source))
      picture->sources_histo = evas_list_append(picture->sources_histo, source);

   /* Check max size, remove last of the list if needed */
   if (evas_list_count(source->histo) > SOURCE_PICTURE_HISTO_MAX_SIZE)
     {
        Evas_List *l;
        Picture *picture;

        l = evas_list_last(source->histo);
        picture = evas_list_data(l);
        picture->sources_histo = evas_list_remove(picture->sources_histo, source);
        source->histo = evas_list_remove_list(source->histo, l);
     }
}

/**                                                                              
 * Detach a picture from a source historic                                       
 */
void DEVIANF(source_picture_histo_picture_detach) (Source_Picture *source, Picture *picture)
{
   while (evas_list_find(source->histo, picture))
      source->histo = evas_list_remove(source->histo, picture);

   picture->sources_histo = evas_list_remove(picture->sources_histo, source);
}

/**
 * Clear the history of a source
 */
void DEVIANF(source_picture_histo_clear) (Source_Picture *source)
{
   Evas_List *l;
   Picture *picture;

   if (!evas_list_count(source->histo))
      return;

   for (l = source->histo; l; l = evas_list_next(l))
     {
        picture = evas_list_data(l);
        picture->sources_histo = evas_list_remove(picture->sources_histo, source);
     }
}

/* Private functions */

static int
_cb_timer_picture_change(void *data)
{
   Source_Picture *source;

   source = data;

   DEVIANF(source_picture_change) (source->devian, 0);

   return 1;
}

static void
_was_set_bg_add(Source_Picture *source, char *name)
{
   char *home;
   char *file_edj;

   file_edj = E_NEW(char, DEVIAN_MAX_PATH);

   home = e_user_homedir_get();
   snprintf(file_edj, DEVIAN_MAX_PATH, "%s/.e/e/backgrounds/%s.edj", home, name);
   free(home);

   source->was_set_bg = evas_list_append(source->was_set_bg, file_edj);
}

static void
_was_set_bg_purge(Source_Picture *source, int in_use)
{
   char *file;
   int n;

   if (!evas_list_count(source->was_set_bg))
      return;

   while ((n = evas_list_count(source->was_set_bg)))
     {
        file = evas_list_data(source->was_set_bg);
        DSOURCE(("was_set_bg check %d %s", n, file));
        if (!strcmp(e_config->desktop_default_background, file))
          {
             if (in_use)
               {
                  if (n == 1)
                     break;
                  else
                    {
                       source->was_set_bg = evas_list_remove(source->was_set_bg, file);
                       E_FREE(file);
                       continue;
                    }
               }
             else
               {
                  source->was_set_bg = evas_list_remove(source->was_set_bg, file);
                  E_FREE(file);
                  continue;
               }
          }
        DSOURCE(("was_set_bg remove %s", file));
        if (ecore_file_exists(file))
           ecore_file_unlink(file);
        source->was_set_bg = evas_list_remove(source->was_set_bg, file);
        E_FREE(file);
     }

   return;
}
#endif
