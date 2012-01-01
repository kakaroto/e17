#include <Eet.h>
#include <Efreet.h>

#include "cfg.h"
#include "mess_header.h"

Ent_Cfg *_ent_cfg;
static Eet_Data_Descriptor * _ent_cfg_descriptor;
#define _CONFIG_ENTRY "config"

static char *config_file = NULL;

static void
_ent_cfg_descriptor_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Ent_Cfg);
   _ent_cfg_descriptor = eet_data_descriptor_stream_new(&eddc);

#define CFG_ADD_BASIC(member, eet_type)\
   EET_DATA_DESCRIPTOR_ADD_BASIC\
      (_ent_cfg_descriptor, Ent_Cfg, # member, member, eet_type)

   CFG_ADD_BASIC(version, EET_T_UINT);
   CFG_ADD_BASIC(font.name, EET_T_STRING);
   CFG_ADD_BASIC(font.size, EET_T_INT);
   CFG_ADD_BASIC(wrap_type, EET_T_INT);

#undef CFG_ADD_BASIC
}

static void
_ent_cfg_descriptor_shutdown(void)
{
   eet_data_descriptor_free(_ent_cfg_descriptor);
}

void
ecrire_cfg_shutdown(void)
{
   if (config_file)
      free(config_file);

   _ent_cfg_descriptor_shutdown();
}

void
ecrire_cfg_init(const char *file)
{
   const char *ext = ".cfg";
   const char *path = efreet_config_home_get();
   size_t len;

   if (!path || !file)
      return;

   if (config_file)
      free(config_file);

   len = strlen(path) + strlen(file) + strlen(ext) + 1; /* +1 for '/' */

   config_file = malloc(len + 1);
   snprintf(config_file, len + 1, "%s/%s%s", path, file, ext);

   eet_init();

   _ent_cfg_descriptor_init();
}

static Ent_Cfg *
_ecrire_cfg_new(void)
{
   Ent_Cfg *ret;
   ret = calloc(1, sizeof(*ret));

   ret->version = _ENTITLED_CFG_VERSION;

   return ret;
}

/* Return false on error. */
Eina_Bool
ecrire_cfg_load(void)
{
   Eet_File *ef;

   if (!config_file)
      return EINA_FALSE;

   ef = eet_open(config_file, EET_FILE_MODE_READ);
   if (!ef)
     {
        /* FIXME Info message? create new config? */
        goto end;
     }

   _ent_cfg = eet_data_read(ef, _ent_cfg_descriptor, _CONFIG_ENTRY);
   if (!_ent_cfg)
      goto end;

end:
   if (!_ent_cfg)
     {
        _ent_cfg = _ecrire_cfg_new();
     }

   eet_close(ef);
   return EINA_TRUE;
}

/* Return false on error. */
Eina_Bool
ecrire_cfg_save(void)
{
   Eet_File *ef;
   Eina_Bool ret;

   if (!config_file)
      return EINA_FALSE;


   ef = eet_open(config_file, EET_FILE_MODE_WRITE);
   if (!ef)
     {
        EINA_LOG_ERR("could not open '%s' for writing.", config_file);
        return EINA_FALSE;
     }

   ret = eet_data_write
         (ef, _ent_cfg_descriptor, _CONFIG_ENTRY, _ent_cfg, EINA_TRUE);
   eet_close(ef);

   return ret;
}

