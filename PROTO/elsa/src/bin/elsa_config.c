#include "elsa.h"

#define ELSA_CONFIG_KEY "config"
#define ELSA_SESSION_KEY "session"

static void _defaults_set(Elsa_Config *config);
static void _users_get();
static void _config_free(Elsa_Config *config);
static Elsa_Config *_cache_get(Eet_Data_Descriptor *edd);

static void
_defaults_set(Elsa_Config *config)
{
   config->session_path = strdup("./:/bin:/usr/bin:/usr/local/bin");
   config->command.xinit_path = strdup("/usr/bin/X");
   config->command.xinit_args = strdup("-nolisten tcp -br vt7");
   config->command.xauth_path = strdup("/usr/bin/xauth");
   config->command.xauth_file = strdup("/var/run/elsa.auth");
   config->command.session_start = strdup("/usr/bin/sessreg -a -l :0.0" );
   config->command.session_login = strdup("exec /bin/bash -login /etc/X11/xinit/xinitrc");
   config->command.session_stop = strdup("/usr/bin/sessreg -d -l :0.0");
   config->command.shutdown = strdup("/usr/bin/shutdown -h now");
   config->command.reboot = strdup("/usr/bin/shutdown -r now");
   config->command.suspend = strdup("/usr/sbin/suspend");
   config->daemonize = EINA_TRUE;
   config->numlock = EINA_FALSE;
   config->xsessions = EINA_FALSE;
   config->lockfile = strdup("/var/run/elsa.pid");
   config->logfile = strdup("/var/log/elsa.log");
}


static void
_users_get()
{
   Eet_File *ef;
   FILE *f;
   int textlen;
   char *text;

   if (!ecore_file_is_dir("/var/cache/"PACKAGE))
     ecore_file_mkdir("/var/cache/"PACKAGE);
   ef = eet_open("/var/cache/"PACKAGE"/"ELSA_CONFIG_FILE, EET_FILE_MODE_READ_WRITE);
   if (!ef)
     ef = eet_open("/var/cache/"PACKAGE"/"ELSA_CONFIG_FILE,
                   EET_FILE_MODE_WRITE);
   f = fopen(SYSTEM_CONFIG_DIR"/elsa.conf", "rb");
   if (!f)
     {
        fprintf(stderr, PACKAGE": Could not open "SYSTEM_CONFIG_DIR"/elsa.conf\n");
        return;
     }

   fseek(f, 0, SEEK_END);
   textlen = ftell(f);
   rewind(f);
   text = (char *)malloc(textlen);
   if (!text)
     {
        fclose(f);
        eet_close(ef);
        return;
     }

   if (fread(text, textlen, 1, f) != 1)
     {
        free(text);
        fclose(f);
        eet_close(ef);
        return;
     }

   fclose(f);
   if (eet_data_undump(ef, ELSA_CONFIG_KEY, text, textlen, 1))
     fprintf(stderr, PACKAGE": Updating configuration\n");
   free(text);
   eet_close(ef);
}

static Elsa_Config *
_cache_get(Eet_Data_Descriptor *edd)
{
   Elsa_Config *config = NULL;
   Eet_File *file;

   if (!ecore_file_is_dir("/var/cache/"PACKAGE))
     ecore_file_mkdir("/var/cache/"PACKAGE);
   file = eet_open("/var/cache/"PACKAGE"/"ELSA_CONFIG_FILE,
                   EET_FILE_MODE_READ);

   config = eet_data_read(file, edd, ELSA_CONFIG_KEY);
   if (!config)
     {
        fprintf(stderr, PACKAGE": Warning no configuration found! This must not append, we will go back to default configuration\n" );
        config = (Elsa_Config *) calloc(1, sizeof(Elsa_Config));
        _defaults_set(config);
     }

   config->last_session = eet_read(file, ELSA_SESSION_KEY, NULL);
   eet_close(file);

   return config;
}

static void
_config_free(Elsa_Config *config)
{
   if (config->last_session) free(config->last_session);
   free(config->session_path);
   free(config->command.xinit_path);
   free(config->command.xinit_args);
   free(config->command.xauth_path);
   free(config->command.xauth_file);
   free(config->command.session_start);
   free(config->command.session_login);
   free(config->command.session_stop);
   free(config->command.shutdown);
   free(config->command.reboot);
   free(config->command.suspend);
   free(config->lockfile);
   free(config->logfile);
   free(config);
}

void
elsa_config_init()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;
   struct stat cache;
   struct stat conf;


   eet_init();
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Config);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "session_path", session_path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "xinit_path", command.xinit_path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "xinit_args", command.xinit_args, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "xauth_path", command.xauth_path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "xauth_file", command.xauth_file, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "session_start", command.session_start, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "session_login", command.session_login, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "session_stop", command.session_stop, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "shutdown", command.shutdown, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "reboot", command.shutdown, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "suspend", command.shutdown, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "daemonize", daemonize, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "numlock", numlock, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "xsessions", xsessions, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "lockfile", lockfile, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Config, "logfile", logfile, EET_T_STRING);

   if (stat( "/var/cache/"PACKAGE"/"ELSA_CONFIG_FILE, &cache) == -1)
     {
        _users_get();
     }
   else
     {
        stat(SYSTEM_CONFIG_DIR"/elsa.conf", &conf);
        if (cache.st_mtime < conf.st_mtime)
          {
             _users_get();
          }
     }
   elsa_config = _cache_get(edd);
}

void
elsa_config_last_session_set(const char *session)
{
   Eet_File *ef;
   char buf[1024];

   ef = eet_open("/var/cache/"PACKAGE"/"ELSA_CONFIG_FILE,
                 EET_FILE_MODE_READ_WRITE);
   if (!ef)
     return;

   snprintf(buf, sizeof(buf), "%s", session);
   if (!eet_write(ef, ELSA_SESSION_KEY, buf, strlen(session), 1))
     fprintf(stderr, PACKAGE": Error on updating last session login\n");
   eet_close(ef);
   if (elsa_config->last_session)
     free(elsa_config->last_session);
   elsa_config->last_session = strdup(session);
}

void
elsa_config_shutdown()
{
   eet_shutdown();
   _config_free(elsa_config);
}

