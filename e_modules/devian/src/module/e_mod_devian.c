#include "dEvian.h"

/**
 * Initalise dEvian
 * @param m Pointer to the module
 * @return 1 on success, 0 if fails
 */
int DEVIANF(devian_main_init) (E_Module *m)
{
   DEVIAN_CONF *cfg_devian;
   int i;

   DEVIANM = E_NEW(DEVIAN_MAIN, 1);

   DEVIANM->container = e_container_current_get(e_manager_current_get());
   evas_output_viewport_get(DEVIANM->container->bg_evas, NULL, NULL, &DEVIANM->canvas_w, &DEVIANM->canvas_h);
   DMAIN(("Canvas %d x %d", DEVIANM->canvas_w, DEVIANM->canvas_h));

   DEVIANM->module = m;

   DEVIANM->devians = NULL;
   DEVIANM->container_box_count = 0;
   DEVIANM->container_bar_count = 0;
#ifdef HAVE_PICTURE
   DEVIANM->source_picture_count = 0;
   DEVIANM->picture_list_local = NULL;
   DEVIANM->picture_list_net = NULL;
   DEVIANM->picture_cache = NULL;
#endif
   DEVIANM->dialog_conf = NULL;
   DEVIANM->dialog_conf_theme = NULL;

   /* Libs used */
   DEVIANM->display = DEVIANF(display_init) ();

   /* Config */
   DEVIANM->conf = DEVIANF(config_init) ();
   if (!DEVIANM->conf)
      return 0;

   /* Init devians like it is in the config file */
   DMAIN(("%d devians to create", DEVIANM->conf->nb_devian));
   for (i = 0; i < DEVIANM->conf->nb_devian; i++)
     {
        cfg_devian = evas_list_nth(DEVIANM->conf->devians_conf, i);
        if (!DEVIANF(devian_add) (cfg_devian->source_type, cfg_devian))
          {
             /* Should never happend */
             fprintf(stderr, MODULE_NAME ": Not good, a devian couldnt be created\n");
             evas_list_remove(DEVIANM->conf->devians_conf, cfg_devian);
          }
     }

   DMAIN(("init_devian OK\n"));

   return 1;
}

/**
 * Shutdown dEvian
 */
void DEVIANF(devian_main_shutdown) (void)
{
   DMAIN(("Main shutdown begins"));

   /* dEvians */
   DEVIANF(devian_del_all) ();
   /* Popups */
   DEVIANF(popup_warn_del_all) ();
   /* Dialogs */
   DEVIANF(config_dialog_main_shutdown) ();
   /* Config */
   DEVIANF(config_free) ();
   /* Libs */
   DEVIANF(display_shutdown) ();

   E_FREE(DEVIANM);
}

/**
 * Creates a new dEvian and add it to the devians list
 * @param source_type Type of the devian to create
 * @param cfg_devian Pointer to the config to use for new devian. If its NULL, we create a new config
 * @return: Pointer to the new dEvian or NULL if fails
 */
DEVIANN *DEVIANF(devian_add) (int source_type, DEVIAN_CONF *cfg_devian)
{
   DEVIANN *devian;

   devian = E_NEW(DEVIANN, 1);

   devian->id = NULL;

   devian->source = NULL;
   devian->source_func.timer_change = DEVIANF(source_idle_timer_change);
   devian->source_func.refresh = DEVIANF(source_idle_refresh);
   devian->source_func.set_bg = DEVIANF(source_idle_set_bg);
   devian->source_func.viewer = DEVIANF(source_idle_viewer);
   devian->source_func.gui_update = DEVIANF(source_idle_gui_update);

   devian->size_policy = SIZE_POLICY_DEFAULT;
   devian->popup_warn = NULL;

   devian->container = NULL;
   devian->container_func.resize_auto = DEVIANF(container_idle_resize_auto);
   devian->container_func.update_actions = DEVIANF(container_idle_update_actions);
   devian->container_func.is_in_transition = DEVIANF(container_idle_is_in_transition);
   devian->container_func.alpha_set = DEVIANF(container_idle_alpha_set);

   devian->dying = 0;
   devian->conf = NULL;
   devian->dialog_conf = NULL;
   devian->dialog_conf_rss = NULL;
   devian->menu = NULL;

   /* Append new devian to the list of devians */
   DEVIANM->devians = evas_list_append(DEVIANM->devians, devian);

   /* Id */
   DEVIANF(devian_set_id) (devian, source_type, NULL);

   /* Config */
   if (cfg_devian)
     {
        /* Use existing config */
        devian->conf = cfg_devian;
     }
   else
     {
        /* New config */
        devian->conf = DEVIANF(config_devian_new) (source_type, NULL);
        if (!devian->conf)
          {
             DEVIANF(devian_del) (devian, 1);
             return NULL;
          }
     }

   /* Menu */
   devian->menu = DEVIANF(menu_init) (devian);
   if (!devian->menu)
     {
        DEVIANF(devian_del) (devian, 1);
        return NULL;
     }

   /* Source */
   if (DEVIANF(source_change) (devian, devian->conf->source_type))
     {
        DEVIANF(devian_del) (devian, 1);
        return NULL;
     }

   /* Container */
   if (DEVIANF(container_change) (devian, devian->conf->container_type))
     {
        DEVIANF(devian_del) (devian, 1);
        return NULL;
     }

   DMAIN(("New devian OK"));

   e_config_save_queue();

   return devian;
}

/**
 * Delete a dEvian
 *
 * @param devian The dEvian to delete
 * @param now If 1, delete the dEvian without transition.
 */
void DEVIANF(devian_del) (DEVIANN *devian, int now)
{
   if (!now)
     {
        devian->dying = 1;
        if (DEVIANF(container_devian_dying) (devian))
           return;
     }

   DMAIN(("dEvian: del devian container"));
   DEVIANF(container_change) (devian, CONTAINER_NO);
   DMAIN(("dEvian: del devian source"));
   DEVIANF(source_change) (devian, SOURCE_NO);
   DMAIN(("dEvian: Free devian menu"));
   DEVIANF(menu_shutdown) (devian);
   DMAIN(("dEvian: Free devian config dialog"));
   DEVIANF(config_dialog_devian_shutdown) (devian);
   DMAIN(("dEvian: Free devian config"));
   DEVIANF(config_devian_free) (devian);
   DMAIN(("dEvian: Free devian remove"));
   DEVIANM->devians = evas_list_remove(DEVIANM->devians, devian);
   DMAIN(("Rename dEvians"));
   DEVIANF(devian_set_id) (NULL, 0, NULL);
   DMAIN(("dEvian: Free devian free"));

   if (devian->id)
      E_FREE(devian->id);
   E_FREE(devian);

   e_config_save_queue();

   DMAIN(("dEvian: Free devian OK"));
}

/**
 * Delete all dEvians, with force option (now)
 */
void DEVIANF(devian_del_all) (void)
{
   while (DEVIANM->devians)
     {
        DMAIN(("dEvian: devian_del"));
        DEVIANF(devian_del) (evas_list_data(DEVIANM->devians), 1);
     }
}

/**
 * Set the name of one / all devian(s)
 *
 * @param devian If isnt NULL, set the name only for the pointed devian
 * The name is choosen with -source, and you can add -extra
 * @param source Type of the source of the devian indicated as -devian
 * @param extra You can add an extra to one / all devian(s) name's, or set it to null
*/
void DEVIANF(devian_set_id) (DEVIANN *devian, int source, char *extra)
{
   char *id;
   int n;

   if (!devian)
     {
        Evas_List *l;
        DEVIANN *d;

        n = 1;
        for (l = DEVIANM->devians; l; l = evas_list_next(l))
          {
             id = E_NEW(char, DEVIAN_ID_LEN);

             d = evas_list_data(l);
             if (d->id && d->conf)
               {
                  E_FREE(d->id);
                  switch (d->conf->source_type)
                    {
#ifdef HAVE_PICTURE
                    case SOURCE_PICTURE:
                       snprintf(id, DEVIAN_ID_LEN, "Picture %d", n);
                       break;
#endif
#ifdef HAVE_RSS
                    case SOURCE_RSS:
                       snprintf(id, DEVIAN_ID_LEN, "Rss %d", n);
                       break;
#endif
#ifdef HAVE_FILE
                    case SOURCE_FILE:
                       snprintf(id, DEVIAN_ID_LEN, "Log %d", n);
                       break;
#endif
                    case SOURCE_NO:
                       snprintf(id, DEVIAN_ID_LEN, "No source %d", n);
                       break;
                    }
               }
             else
               {
                  if (d->id)
                     E_FREE(d->id);
                  snprintf(id, DEVIAN_ID_LEN, "Invalid devian %d", n);
               }
             d->id = id;
             DEVIANF(container_update_id_devian) (d);
             n++;
          }
     }
   else
     {
        id = E_NEW(char, DEVIAN_ID_LEN);

        n = evas_list_count(DEVIANM->devians);
        switch (source)
          {
#ifdef HAVE_PICTURE
          case SOURCE_PICTURE:
             if (extra)
                snprintf(id, DEVIAN_ID_LEN, "Picture %d %s", n, extra);
             else
                snprintf(id, DEVIAN_ID_LEN, "Picture %d", n);
             break;
#endif
#ifdef HAVE_RSS
          case SOURCE_RSS:
             if (extra)
                snprintf(id, DEVIAN_ID_LEN, "Rss %d %s", n, extra);
             else
                snprintf(id, DEVIAN_ID_LEN, "Rss %d", n);
             break;
#endif
#ifdef HAVE_FILE
          case SOURCE_FILE:
             if (extra)
                snprintf(id, DEVIAN_ID_LEN, "File %d %s", n, extra);
             else
                snprintf(id, DEVIAN_ID_LEN, "File %d", n);
             break;
#endif
          case SOURCE_NO:
             if (extra)
                snprintf(id, DEVIAN_ID_LEN, "No source %d %s", n, extra);
             else
                snprintf(id, DEVIAN_ID_LEN, "No source %d", n);
             break;
          default:
             if (extra)
                snprintf(id, DEVIAN_ID_LEN, "Invalid dEvian %d %s", n, extra);
             else
                snprintf(id, DEVIAN_ID_LEN, "Invalid dEvian %d", n);
             break;
          }
        if (devian->id)
           E_FREE(devian->id);
        devian->id = id;
        if (devian->container)
           DEVIANF(container_update_id_devian) (devian);
     }

   return;
}

/**
 * Initialise the e_lib
 * 
 * @return Name of the current X display
 */
char *DEVIANF(display_init) (void)
{
   char *display = NULL;
   char *tmp;

   tmp = getenv("DISPLAY");
   if (tmp)
      display = strdup(tmp);

   /* make sure the display var is of the form name:0.0 or :0.0 */
   if (display)
     {
        char *p;
        char buf[1024];

        p = strrchr(display, ':');
        if (!p)
          {
             snprintf(buf, sizeof(buf), "%s:0.0", display);
             free(display);
             display = strdup(buf);
          }
        else
          {
             p = strrchr(p, '.');
             if (!p)
               {
                  snprintf(buf, sizeof(buf), "%s.0", display);
                  free(display);
                  display = strdup(buf);
               }
          }
     }
   else
      display = strdup(":0.0");

   /* Init E Lib */
   if (display)
      e_lib_init(display);
   else
     {
        display = strdup(":0.0");
        e_lib_init(display);
     }

   return display;
}

/**
 * Shutdown e_lib
 */
void DEVIANF(display_shutdown) (void)
{
   free(DEVIANM->display);
   DEVIANM->display = NULL;
   e_lib_shutdown();
}

/**
 * Loads a specific part of the current theme to an edje object
 * 
 * @param edje_obj The edje object
 * @param part Name of the part to load in current theme
 * @param type Type of the theme to use (picture, rss, popup ...)
 * @return The name of the theme used when loading the part, or NULL if fails
 */

const char *DEVIANF(devian_edje_load) (Evas_Object *edje_obj, char *part, int type)
{
   char path[DEVIAN_MAX_PATH];
   const char *theme;

   switch (type)
     {
#ifdef HAVE_PICTURE
     case DEVIAN_THEME_TYPE_PICTURE:
        theme = DEVIANM->conf->theme_picture;
        break;
#endif
#ifdef HAVE_RSS
     case DEVIAN_THEME_TYPE_RSS:
        theme = DEVIANM->conf->theme_rss;
        break;
#endif
#ifdef HAVE_FILE
     case DEVIAN_THEME_TYPE_FILE:
        theme = DEVIANM->conf->theme_file;
        break;
#endif
     case DEVIAN_THEME_TYPE_POPUP:
        theme = DEVIANM->conf->theme_popup;
        break;
     default:
        theme = DEVIANM->conf->theme_popup;
     }

   snprintf(path, DEVIAN_MAX_PATH, "%s/%s/%s%s", e_module_dir_get(DEVIANM->module), "themes", theme, ".edj");
   if (!edje_object_file_set(edje_obj, path, part))
     {
        char buf[4096];

        snprintf(buf, sizeof(buf),
                 _("<hilight>Error !</hilight><br>" "Did not find <hilight>theme</hilight> %s group %s"), path, part);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return NULL;
     }
   DMAIN(("Edje loaded %s group %s\n", path, part));

   return theme;
}

/**
 * Makes a charset convertion to UTF8, and creates an evas_stringshare with the result
 * 
 * @param text The text to use
 * @param tfree Free the text after ?
 * @return The evas_strinshare in utf8 if translation was ok, evas_stringshare of original text otherwise
 */
const char *DEVIANF(ss_utf8_add) (char *text, int tfree)
{
   char *utf8;
   const char *ss;

   utf8 = ecore_txt_convert("iso-8859-1", "UTF-8", text);
   if (!utf8)
     {
        ss = evas_stringshare_add(text);
     }
   else
     {
        ss = evas_stringshare_add(utf8);
        E_FREE(utf8);
     }
   if (tfree)
      E_FREE(text);

   return ss;
}
