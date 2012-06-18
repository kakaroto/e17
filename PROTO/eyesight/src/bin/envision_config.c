#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "envision_config.h"

static Eet_Data_Descriptor *edd_base = NULL;

Env_Config *env_cfg = NULL;

static const char *
_env_homedir(void)
{
#ifdef HAVE_EVIL
  return evil_homedir_get();
#else
   const char *home;

   home = getenv("HOME");
   if (!home) home = getenv("TMP");
   if (!home) home = "/tmp";
   return home;
#endif
}

void
env_config_init(void)
{
  char buf[4096];
  const char *home;
  Eet_Data_Descriptor_Class eddc;
  Eet_File *ef;

  eet_eina_stream_data_descriptor_class_set(&eddc, sizeof(eddc), "Env_Config", sizeof(Env_Config));
  edd_base = eet_data_descriptor_stream_new(&eddc);

  EET_DATA_DESCRIPTOR_ADD_BASIC(edd_base, Env_Config, "theme", theme, EET_T_STRING);

  home = _env_homedir();
  snprintf(buf, sizeof(buf), "%s/.%s/config/standard/base.cfg", home, PACKAGE_NAME);
  ef = eet_open(buf, EET_FILE_MODE_READ);
  if (ef)
    {
      env_cfg = eet_data_read(ef, edd_base, "config");
      eet_close(ef);
    }
  if (!env_cfg)
    {
      env_cfg = calloc(1, sizeof(Env_Config));
      env_cfg->theme = eina_stringshare_add("default.edj");
    }
}

void
env_config_shutdown(void)
{
  if (env_cfg)
    {
      if (env_cfg->theme) eina_stringshare_del(env_cfg->theme);
      free(env_cfg);
      env_cfg = NULL;
    }
  if (edd_base)
    {
      eet_data_descriptor_free(edd_base);
      edd_base = NULL;
    }
}

void
env_config_save(void)
{
  char buf[4096];
  char buf2[4096];
  const char *home;
  Eet_File *ef;
  int ok;

  home = _env_homedir();
  snprintf(buf, sizeof(buf), "%s/.terminology/config/standard", home);
  ecore_file_mkpath(buf);
  snprintf(buf, sizeof(buf), "%s/.terminology/config/standard/base.cfg.tmp", home);
  snprintf(buf2, sizeof(buf2), "%s/.terminology/config/standard/base.cfg", home);
  ef = eet_open(buf, EET_FILE_MODE_WRITE);
  if (ef)
    {
      ok = eet_data_write(ef, edd_base, "config", env_cfg, 1);
      eet_close(ef);
      if (ok) ecore_file_mv(buf, buf2);
    }
}
