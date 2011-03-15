#include "ephoto.h"

#define CONFIG_VERSION 6

static int _ephoto_config_load(Ephoto *ephoto);
static Eina_Bool _ephoto_on_config_save(void *data);

static Eet_Data_Descriptor *edd = NULL;
static Ecore_Timer *save_timer = NULL;

Eina_Bool
ephoto_config_init(Ephoto *ephoto)
{
   Eet_Data_Descriptor_Class eddc;

   if (!eet_eina_stream_data_descriptor_class_set
        (&eddc, sizeof (eddc), "Ephoto_Config", sizeof(Ephoto_Config)))
     {
        ERR("Unable to create the config data descriptor!");
        return EINA_FALSE;
     }

   if (!edd) edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "config_version", config_version, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "autohide_toolbar", autohide_toolbar, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "best_fit_images", best_fit_images, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "thumb_gen_size", thumb_gen_size, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "thumb_size", thumb_size, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "slideshow_timeout", slideshow_timeout, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "directory", directory, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC
     (edd, Ephoto_Config, "slideshow_transition", slideshow_transition, EET_T_STRING);
   switch (_ephoto_config_load(ephoto))
     {
      case 0:
         /* Start a new config */
         ephoto->config->config_version = CONFIG_VERSION;
         ephoto->config->autohide_toolbar = 0;
         ephoto->config->best_fit_images = 0;
         ephoto->config->thumb_gen_size = 256;
         ephoto->config->thumb_size = 256;
         ephoto->config->thumb_gen_size = 256;
         ephoto->config->slideshow_timeout = 4.0;
         ephoto->config->slideshow_transition = eina_stringshare_add("fade");
         break;

      default:
         return EINA_TRUE;
     }

   ephoto_config_save(ephoto, EINA_FALSE);
   return EINA_TRUE;
}

void
ephoto_config_save(Ephoto *ephoto, Eina_Bool instant)
{
   if (save_timer)
     {
        ecore_timer_del(save_timer);
        save_timer = NULL;
     }

   if (instant)
     _ephoto_on_config_save(ephoto);
   else
     save_timer = ecore_timer_add(5.0, _ephoto_on_config_save, ephoto);
}

void
ephoto_config_free(Ephoto *ephoto)
{
   free(ephoto->config);
   ephoto->config = NULL;
}

static int
_ephoto_config_load(Ephoto *ephoto)
{
   Eet_File *ef;
   char buf[4096], buf2[4096];

   snprintf(buf2, sizeof(buf2), "%s/.config/ephoto", getenv("HOME"));
   ecore_file_mkpath(buf2);
   snprintf(buf, sizeof(buf), "%s/ephoto.cfg", buf2);

   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (!ef)
     {
        ephoto_config_free(ephoto);
        ephoto->config = calloc(1, sizeof(Ephoto_Config));
        return 0;
     }

   ephoto->config = eet_data_read(ef, edd, "config");
   eet_close(ef);

   if (!ephoto->config)
     {
        DBG("Warning! No configuration found! Writing a new"
            "default configuration!\n");
        return 0;
     }
   else if (ephoto->config->config_version > CONFIG_VERSION)
     {
        ephoto_config_free(ephoto);
        ephoto->config = calloc(1, sizeof(Ephoto_Config));
        return 0;
     }

   else if (ephoto->config->config_version < CONFIG_VERSION)
     {
        ephoto_config_free(ephoto);
        ephoto->config = calloc(1, sizeof(Ephoto_Config));
        return 0;
     }
   return 1;
}

static Eina_Bool
_ephoto_on_config_save(void *data)
{
   Ephoto *ephoto = data;
   Eet_File *ef;
   char buf[4096], buf2[4096];

   snprintf(buf, sizeof(buf), "%s/.config/ephoto/ephoto.cfg", getenv("HOME"));
   snprintf(buf2, sizeof(buf2), "%s.tmp", buf);

   ef = eet_open(buf2, EET_FILE_MODE_WRITE);
   if (!ef) goto save_end;

   eet_data_write(ef, edd, "config", ephoto->config, 1);
   if (eet_close(ef)) goto save_end;

   if (!ecore_file_mv(buf2, buf)) goto save_end;

   INF("Config saved");

save_end:
   ecore_file_unlink(buf2);

   if (save_timer)
     {
        ecore_timer_del(save_timer);
        save_timer = NULL;
     }

   return ECORE_CALLBACK_CANCEL;
}
