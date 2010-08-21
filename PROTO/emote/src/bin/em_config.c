#include "emote.h"

/* local function prototypes */
static void _em_config_cb_free(void);
static void *_em_config_domain_load(const char *domain, Em_Config_DD *edd);
static int _em_config_domain_save(const char *domain, Em_Config_DD *edd, const void *data);

/* local variables */
static Em_Config_DD *_em_config_edd = NULL;

/* global variables */
EM_INTERN Em_Config *em_config = NULL;

EM_INTERN int 
em_config_init(void) 
{
   _em_config_edd = EM_CONFIG_DD_NEW("Em_Config", Em_Config);
   #undef T
   #undef D
   #define T Em_Config
   #define D _em_config_edd
   EM_CONFIG_VAL(D, T, version, EET_T_INT);

   em_config_load();
   em_config_save();
   return 1;
}

EM_INTERN int 
em_config_shutdown(void) 
{
   _em_config_cb_free();
   EM_CONFIG_DD_FREE(_em_config_edd);
   return 1;
}

EM_INTERN Em_Config_DD *
em_config_descriptor_new(const char *name, int size) 
{
   Eet_Data_Descriptor_Class eddc;

   if (!eet_eina_stream_data_descriptor_class_set(&eddc, sizeof(eddc), 
                                                  name, size))
     return NULL;
   return (Em_Config_DD *)eet_data_descriptor_stream_new(&eddc);
}

EM_INTERN void 
em_config_load(void) 
{
   em_config = _em_config_domain_load("emote", _em_config_edd);
   if (em_config) 
     {
        int reload;

        if ((em_config->version >> 16) < EM_CONFIG_FILE_EPOCH) 
          {
             /* config too old */
             reload = 1;
          }
        else if (em_config->version > EM_CONFIG_FILE_VERSION) 
          {
             /* config too new, WTF ? */
             reload = 1;
          }
        if (reload) 
          em_config = _em_config_domain_load("emote", _em_config_edd);
     }
   if (!em_config) em_config = EM_NEW(Em_Config, 1);

   /* define some convenient macros */
#define IFCFG(v) if ((em_config->version & 0xffff) < (v)) {
#define IFCFGELSE } else {
#define IFCFGEND }

   /* setup defaults */
   IFCFG(0x008d);
   IFCFGEND;

   /* limit config values so they are sane */

   em_config->version = EM_CONFIG_FILE_VERSION;
}

EM_INTERN int 
em_config_save(void) 
{
   return _em_config_domain_save("emote", _em_config_edd, em_config);
}

/* local functions */
static void 
_em_config_cb_free(void) 
{
   EM_FREE(em_config);
}

static void *
_em_config_domain_load(const char *domain, Em_Config_DD *edd) 
{
   Eet_File *ef;
   char buff[PATH_MAX];

   if (!domain) return NULL;
   snprintf(buff, sizeof(buff), 
            "%s/config/%s.cfg", em_util_user_dir_get(), domain);
   ef = eet_open(buff, EET_FILE_MODE_READ);
   if (ef) 
     {
        void *data;

        data = eet_data_read(ef, edd, "config");
        eet_close(ef);
        if (data) return data;
     }
   return NULL;
}

static int 
_em_config_domain_save(const char *domain, Em_Config_DD *edd, const void *data) 
{
   Eet_File *ef;
   char buff[PATH_MAX];
   const char *userdir;
   int ret = 0;

   if (!domain) return 0;
   userdir = em_util_user_dir_get();
   snprintf(buff, sizeof(buff), "%s/config/", userdir);
   if (!ecore_file_exists(buff)) ecore_file_mkpath(buff);
   snprintf(buff, sizeof(buff), "%s/config/%s.tmp", userdir, domain);
   ef = eet_open(buff, EET_FILE_MODE_WRITE);
   if (ef) 
     {
        char buff2[PATH_MAX];
        int err;

        snprintf(buff2, sizeof(buff2), "%s/config/%s.cfg", userdir, domain);
        ret = eet_data_write(ef, edd, "config", data, 1);
        err = eet_close(ef);
        ret = ecore_file_mv(buff, buff2);
     }
   return ret;
}
