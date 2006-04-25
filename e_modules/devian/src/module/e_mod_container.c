#include "dEvian.h"

static void _devian_container_del(DEVIANN *devian);


/* PUBLIC FUNCTIONS */

/*
  Change the current container of a devian
  Return:
    - 0 if ok
    - 1 if fails
*/
int DEVIANF(container_change) (DEVIANN *devian, int container)
{
   int edje_part;

   /* check: if no source, no way */
   if (!devian->source)
      return 0;

   /* check: if already one container, remove it */
   if (devian->container)
     {
        edje_part = DEVIANF(container_edje_part_get) (devian);
        _devian_container_del(devian);
     }
   else
      edje_part = -1;

   /* add the new container to the devian */
   switch (container)
     {
     case CONTAINER_BOX:
        {
           if (DEVIANF(container_box_add) (devian, edje_part))
              return 1;
           break;
        }

     case CONTAINER_BAR:
        {
           return 1;
        }

     case CONTAINER_NO:
        {
           /* i want no container attached, thank you */
           return 0;
        }

     default:
        return 1;
     }

   return 0;
}

int DEVIANF(container_edje_part_get) (DEVIANN *devian)
{
   if (!devian->container)
      return -1;

   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        return DEVIANF(container_box_edje_part_get) (devian->container);
     case CONTAINER_BAR:
        break;
     }

   return -1;
}

int DEVIANF(container_edje_part_change) (DEVIANN *devian)
{
   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        return DEVIANF(container_box_edje_part_change) (devian->container);
     case CONTAINER_BAR:
        break;
     }

   return 0;
}

int DEVIANF(container_infos_text_change) (DEVIANN *devian, char *text)
{
   if (!devian->container)
      return 0;

   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        return DEVIANF(container_box_infos_text_change) (devian->container, text);
     case CONTAINER_BAR:
        break;
     }

   return 0;
}

void DEVIANF(container_infos_text_scroll) (DEVIANN *devian)
{
   if (!devian->container)
      return;

   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        DEVIANF(container_box_infos_text_scroll) (devian->container);
        return;
     case CONTAINER_BAR:
        return;
     }

   return;
}

void DEVIANF(container_infos_text_change_set) (DEVIANN *devian, int action)
{
   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        DEVIANF(container_box_infos_text_change_set) (devian->container, action);
        break;
     case CONTAINER_BAR:
        break;
     }

   return;
}

void DEVIANF(container_loading_state_change) (DEVIANN *devian, int state)
{
   if (!devian->container)
      return;

   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        DEVIANF(container_box_loading_state_change) (devian->container, state);
        return;
     case CONTAINER_BAR:
        return;
     }
}

void DEVIANF(container_warning_indicator_change) (DEVIANN *devian, int state)
{
   if (!devian->container)
      return;

   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        DEVIANF(container_box_warning_indicator_change) (devian->container, state);
        return;
     case CONTAINER_BAR:
        return;
     }
}

int DEVIANF(container_update_id_devian) (DEVIANN *devian)
{
   if (!devian->conf || !devian->container)
      return 0;

   switch (devian->conf->container_type)
     {
     case CONTAINER_BOX:
        return DEVIANF(container_box_update_id_devian) (devian->container);
     case CONTAINER_BAR:
        return 0;
     }

   return 0;
}

int DEVIANF(container_devian_dying) (DEVIANN *devian)
{
   if (devian->conf->container_type == CONTAINER_BOX)
     {
        return DEVIANF(container_box_devian_dying) (devian->container);
     }
   return 0;
}

const char *DEVIANF(container_edje_load) (Evas_Object *edje_obj, char *part, DEVIANN *devian, int container_type, void *container)
{
   int source_w, source_h;
   int border_w, border_h;
   const char *theme;

   if (!(theme = DEVIANF(devian_edje_load) (edje_obj, part, devian->conf->source_type)))
      return NULL;

   /* 'precalculate' (get) borders */
   evas_object_resize(edje_obj, 400, 400);
   edje_object_part_geometry_get(edje_obj, "source0", NULL, NULL, &source_w, &source_h);
   border_w = (400 - source_w);
   border_h = (400 - source_h);
   DCONTAINER(("Theme borders: x:%d y:%d", border_w, border_h));
   evas_object_resize(edje_obj, 0, 0);

   switch (container_type)
     {
        Container_Box *box;

     case CONTAINER_BOX:
        box = container;
        box->theme_border_w = border_w;
        box->theme_border_h = border_h;
        break;
     case CONTAINER_BAR:
        return NULL;
     default:
        return NULL;
     }

   return evas_stringshare_add(theme);
}

/**
 * Change the theme of one / multiple devians
 * 
 * @param devian If not NULL, we are going to change theme for this dEvian only
 * @param source_type If devian is NULL, change theme for all sources of this type
 * @param theme Pointer to the theme currently used for the type of source we want to change. It's only to be able to fall back to default theme if fail
 * @return 0 on fail, 1 on success
 */

int DEVIANF(container_theme_change) (DEVIANN *devian, int source_type, const char **theme)
{
   Evas_List *l;
   int i, max;

   DCONTAINER(("Switching to theme %s", *theme));

   if (devian)
     {
        DCONTAINER(("-- Theme change : devian_id %s, container_type %d, theme %s",
                    devian->id, devian->conf->container_type, *theme));
        DEVIANF(config_devian_save) (devian);
        if (DEVIANF(container_change) (devian, devian->conf->container_type))
          {
             char buf[4096];

             snprintf(buf, sizeof(buf), _("<hilight>Invalid theme !</hilight><br><br>" "Failed to switch to %s theme"), *theme);
             e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
             evas_stringshare_del(*theme);
             *theme = evas_stringshare_add(DEVIAN_THEME_DEFAULT);
             DEVIANF(container_change) (devian, devian->conf->container_type);
             return 0;
          }
     }
   else
     {
        l = DEVIANM->devians;
        max = evas_list_count(l);
        for (i = 0; i < max; i++)
          {
             devian = evas_list_data(l);
             l = evas_list_next(l);
             if (devian->conf->source_type == source_type)
               {
                  DEVIANF(config_devian_save) (devian);
                  if (DEVIANF(container_change) (devian, devian->conf->container_type))
                    {
                       char buf[4096];

                       snprintf(buf, sizeof(buf),
                                _("<hilight>Invalid theme !</hilight><br><br>" "Failed to switch to %s theme"), *theme);
                       e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
                       evas_stringshare_del(*theme);
                       *theme = evas_stringshare_add(DEVIAN_THEME_DEFAULT);
                       DEVIANF(container_change) (devian, devian->conf->container_type);
                       return 0;
                    }
                  devian->source_func.gui_update(devian);
               }
          }
     }

   return 1;
}

void DEVIANF(container_theme_check) (DEVIANN *devian, const char **source_theme)
{
   if (!devian->container)
      return;

   switch (devian->conf->container_type)
     {
        Container_Box *box;

     case CONTAINER_NO:
        break;
     case CONTAINER_BOX:
        box = devian->container;
        if (strcmp(box->theme, *source_theme))
           DEVIANF(container_theme_change) (devian, 0, source_theme);
        break;
     case CONTAINER_BAR:
        break;

     default:
        break;
     }
}

void DEVIANF(container_idle_resize_auto) (DEVIANN *devian)
{
   return;
}
void DEVIANF(container_idle_update_actions) (DEVIANN *devian)
{
   return;
}
int DEVIANF(container_idle_is_in_transition) (DEVIANN *devian)
{
   return 0;
}
void DEVIANF(container_idle_alpha_set) (void *container, int alpha)
{
   return;
}


/* STATIC FUNCTIONS */

static void
_devian_container_del(DEVIANN *devian)
{
   switch (devian->conf->container_type)
     {
     case CONTAINER_NO:
        break;
     case CONTAINER_BOX:
        DEVIANF(container_box_del) (devian->container);
        break;
     case CONTAINER_BAR:
        break;

     default:
        break;
     }

   /* actions */
   devian->container_func.resize_auto = DEVIANF(container_idle_resize_auto);
   devian->container_func.update_actions = DEVIANF(container_idle_update_actions);
   devian->container_func.is_in_transition = DEVIANF(container_idle_is_in_transition);
   devian->container_func.alpha_set = DEVIANF(container_idle_alpha_set);
}
