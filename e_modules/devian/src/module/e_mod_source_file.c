#include "dEvian.h"

#ifdef HAVE_FILE

static void _cb_monitor(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path);

/**
 * Add a file source to a dEvian
 * @param devian The dEvian
 * @return 1 on success, 0 if fails
 */
int DEVIANF(source_file_add) (DEVIANN *devian)
{
   Source_File *source;

   /* SOURCE_FILE IS NOT STABLE YET */
   {
      char buf[4096];
      
      snprintf(buf, sizeof(buf),
	       _("<hilight>dEvian's Log feature isn't stable yet !</hilight><br><br>"
		 "It's enabled for debugging purpose only"));
      e_module_dialog_show(_(MODULE_NAME " Module error"), buf);
   }
   /* SOURCE_FILE IS NOT STABLE YET */

   DSOURCE(("Source file creation START"));

   source = E_NEW(Source_File, 1);
   devian->source = source;
   source->devian = devian;
   source->obj = NULL;
   source->blocks = NULL;
   source->fd = 0;
   source->monitor = NULL;
   source->obj_tb = NULL;

   source->obj = e_scrollframe_add(DEVIANM->container->bg_evas);

   if (!DEVIANF(data_file_add) (source))
      return 0;

   /* Set new name for devian */
   DEVIANF(devian_set_id) (devian, SOURCE_FILE, NULL);

   /* Provide declarations */
   devian->source_info.provide_double_buf = 0;
   devian->source_info.provide_previous = 0;
   devian->source_info.provide_set_bg = 0;
   devian->source_info.allow_info_panel = 0;

   /* Actions */
   devian->source_func.refresh = DEVIANF(source_file_change);
   devian->source_func.viewer = DEVIANF(source_file_viewer);
   devian->source_func.timer_change = DEVIANF(source_file_update_change);

   devian->container_func.update_actions(devian);

   /* Hop its active :) */
   devian->conf->source_type = SOURCE_FILE;
   devian->size_policy = SIZE_POLICY_USER;

   /* Activate updates */
   DEVIANF(source_file_update_change) (devian, 1, 0);

   if (source->devian->container)
     {
        if (!DEVIANF(container_edje_part_change) (source->devian))
           return 0;
     }
   devian->container_func.resize_auto(source->devian);

   /* Informations */
   DEVIANF(container_infos_text_change) (source->devian, NULL);

   DSOURCE(("Source file creation OK\n"));

   return 1;
}

void DEVIANF(source_file_del) (Source_File *source)
{
   DEVIANF(source_file_update_change) (source->devian, 0, 0);

   DEVIANF(data_file_del) (source);

   if (source->obj)
      evas_object_del(source->obj);

   E_FREE(source);
}

int DEVIANF(source_file_change) (DEVIANN *devian, int option)
{
   return DEVIANF(data_file_update) (devian->source, option);
}

int DEVIANF(source_file_viewer) (DEVIANN *devian)
{
   return 0;
}

int DEVIANF(source_file_evas_object_get) (DEVIANN *devian, Evas_Object **file0, Evas_Object **file1)
{
   Source_File *source;

   source = devian->source;

   *file0 = source->obj;
   *file1 = source->obj;

   return 1;
}

int DEVIANF(source_file_original_size_get) (Source_File *source, int *w, int *h)
{
   *w = 300;
   *h = DEVIANM->conf->sources_file_nb_lines_ini * 15;

   return 1;
}

void DEVIANF(source_file_dialog_infos) (Source_File *source)
{

}

char *DEVIANF(source_file_name_get) (Source_File *source)
{
   char *name;

   name = strrchr(source->devian->conf->file_path, '/');

   return name + 1;
}

char *DEVIANF(source_file_infos_get) (Source_File *source)
{
   return strdup("No informations about this file");
}

void DEVIANF(source_file_update_change) (DEVIANN *devian, int active, int time)
{
   Source_File *source;

   source = devian->source;

   if (active && !source->monitor)
     {
        source->monitor = ecore_file_monitor_add(source->devian->conf->file_path, _cb_monitor, source);
        devian->source_info.paused = 0;
        devian->container_func.update_actions(devian);
     }
   else
     {
        if (!active && source->monitor)
          {
             ecore_file_monitor_del(source->monitor);
             source->monitor = NULL;
             devian->source_info.paused = 1;
             devian->container_func.update_actions(devian);
          }
     }
}

/* PRIVATE FUNCTIONS */

static void
_cb_monitor(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
   Source_File *source;

   source = data;

   DEVIANF(data_file_update) (source, -1);
}

#endif
