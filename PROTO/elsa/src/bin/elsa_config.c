#include "elsa.h"

#define ELSA_CONFIG_UPDATE(conf,update) free(conf); conf = strdup(update)


static void _defaults_set(Elsa_Config *config);
static void _users_get(Elsa_Config *config);
static void _config_save(Elsa_Config *config);

static void
_defaults_set(Elsa_Config *config) {
   config->session_path = strdup("./:/bin:/usr/bin:/usr/local/bin");
   config->command.xinit.path = strdup("/usr/bin/X");
   config->command.xinit.args = strdup("-nolisten tcp -br -deferglyphs 16");
   config->command.xauth.path = strdup("/usr/bin/xauth");
   config->command.xauth.file = strdup("/var/run/elsa.auth");
   config->command.session.start = strdup("/usr/bin/sessreg -a -l :0.0" );
   config->command.session.login = strdup("exec /bin/bash -login /etc/X11/xinit/xinitrc");
   config->command.session.stop = strdup("/usr/bin/sessreg -d -l :0.0");
   config->command.shutdown = strdup("/usr/bin/shutdown -h now");
   config->command.reboot = strdup("/usr/bin/shutdown -r now");
   config->command.suspend = strdup("/usr/sbin/suspend");
   config->daemonize = EINA_TRUE;
   config->numlock = EINA_FALSE;
   config->sessions = strdup("enlightenment,xfce4,icewm,wmaker,blackbox");
   config->lockfile = strdup("/var/run/elsa.run");
   config->logfile = strdup("/var/log/elsa.log");
}


static void
_users_get(Elsa_Config *config) {
   FILE *f;
   char buf[4096];

   f = fopen("/etc/elsa.conf", "r");
   if (!f) return;
   while (fgets (buf, sizeof(buf), f)) {
      if (!*buf || *buf == '#') continue;
      /*
      if (buf == strstr(buf, "default_path")) {
         ELSA_CONFIG_UPDATE(config->session_path, buf);
         continue;
      }
      if (buf == strstr(buf, "default_xserver")) {
         ELSA_CONFIG_UPDATE(config->command.xinit.path, buf);
         continue;
      }
      if (buf == strstr(buf, "xserver_arguments")) {
         ELSA_CONFIG_UPDATE(config->command.xinit.args, buf);
         continue;
      }
      if (buf == strstr(buf, "halt_cmd")) {
         ELSA_CONFIG_UPDATE(config->command.shutdown, buf);
         continue;
      }
      if (buf == strstr(buf, "reboot_cmd")) {
         ELSA_CONFIG_UPDATE(config->command.reboot, buf);
         continue;
      }
      if (buf == strstr(buf, "suspend_cmd")) {
         ELSA_CONFIG_UPDATE(config->command.suspend, buf);
         continue;
      }
      if (buf == strstr(buf, "xauth_path")) {
         ELSA_CONFIG_UPDATE(config->command.xauth.path, buf);
         continue;
      }
      if (buf == strstr(buf, "authfile")) {
         ELSA_CONFIG_UPDATE(config->command.xauth.file, buf);
         continue;
      }
      if (buf == strstr(buf, "sessionstart_cmd")) {
         ELSA_CONFIG_UPDATE(config->command.session.start, buf);
         continue;
      }
      if (buf == strstr(buf, "sessionstop_cmd")) {
         ELSA_CONFIG_UPDATE(config->command.session.stop, buf);
         continue;
      }
      if (buf == strstr(buf, "login_cmd")) {
         ELSA_CONFIG_UPDATE(config->command.session.login, buf);
         continue;
      }
      if (buf == strstr(buf, "sessions")) {
         ELSA_CONFIG_UPDATE(config->sessions, buf);
         continue;
      }
      if (buf == strstr(buf, "lockfile")) {
         ELSA_CONFIG_UPDATE(config->lockfile, buf);
         continue;
      }
      if (buf == strstr(buf, "logfile")) {
         ELSA_CONFIG_UPDATE(config->logfile, buf);
         continue;
      }
      */
   }
   fclose(f);
}

static void
_cache_get(Elsa_Config *config) {
   Eet_File *file;
   char buf[4096];
   char *ret = NULL ;
   int size = 0;

   snprintf(buf, sizeof(buf), "/var/cache/"PACKAGE"/%s" , ELSA_CONFIG_FILE);
   if (!ecore_file_is_dir("/var/cache/"PACKAGE))
     ecore_file_mkdir("/var/cache/"PACKAGE);
   file = eet_open(buf, EET_FILE_MODE_READ);
   if (!file) return;

   ret = eet_read(file, ELSA_SESSION_PATH, &size);
   if (size && ret) {
      free(config->session_path);
      config->session_path = ret;
   }
   eet_close(file);
}

static void
_config_save(Elsa_Config *config) {
   Eet_File *file; 
   char buf[4096];

   snprintf(buf, sizeof(buf), "/var/cache/"PACKAGE"/%s" , ELSA_CONFIG_FILE);
   if (!ecore_file_is_dir("/var/cache/"PACKAGE))
     ecore_file_mkdir("/var/cache/"PACKAGE);

   file = eet_open(buf, EET_FILE_MODE_WRITE);
   if (!file)
      fprintf(stderr, "Error: can't open %s \n", buf);
   eet_write(file, ELSA_SESSION_PATH, config->session_path,
             strlen(config->session_path) + 1, 1);
/*
//   eet_write(file, MPD_PORT_KEY, config->port, sizeof(config->port), 0);
   eet_write(file, MPD_PASSWORD_KEY, config->password,
             config->password?strlen(config->password) + 1:0, 1);
   eet_write(file, MPD_COVERPATH_KEY, config->cover,
             strlen(config->cover) + 1, 1);
*/
   eet_close(file);
}

void
elsa_config_init() {
   struct stat cache;
   struct stat conf;
   char buf[1024];

   elsa_config = (Elsa_Config *) calloc(1, sizeof(Elsa_Config));
   if(!elsa_config) return;
   _defaults_set(elsa_config);
   snprintf(buf, sizeof(buf), "/var/cache/"PACKAGE"/%s" , ELSA_CONFIG_FILE);
   if (stat(buf, &cache) == -1) {
      _users_get(elsa_config);
      _config_save(elsa_config);
      return;
   }
   stat("/etc/elsa.conf", &conf);
   if (cache.st_mtime < conf.st_mtime) {
      _users_get(elsa_config);
      _config_save(elsa_config);
   }
   else
      _cache_get(elsa_config);

   return;
}


void
elsa_config_shutdown() {
   free(elsa_config->session_path);
   free(elsa_config->command.xinit.path);
   free(elsa_config->command.xinit.args);
   free(elsa_config->command.xauth.path);
   free(elsa_config->command.xauth.file);
   free(elsa_config->command.session.start);
   free(elsa_config->command.session.login);
   free(elsa_config->command.session.stop);
   free(elsa_config->command.shutdown);
   free(elsa_config->command.reboot);
   free(elsa_config->command.suspend);
   free(elsa_config->sessions);
   free(elsa_config->lockfile);
   free(elsa_config->logfile);
   free(elsa_config);
}

