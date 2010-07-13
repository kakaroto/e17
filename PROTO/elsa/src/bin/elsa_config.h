#ifndef ELSA_CONFIG_H_
#define ELSA_CONFIG_H_
#include <Elementary.h>

#define ELSA_CONFIG_FILE        "elsa.cfg"
#define ELSA_SESSION_PATH       "config/session/path"
#define ELSA_CMD_X_PATH         "/config/command/x/path"
#define ELSA_CMD_X_ARG          "/config/command/x/args"
#define ELSA_CMD_SHUTDOWN       "/config/command/shutdown"
#define ELSA_CMD_REBOOT         "/config/command/reboot"
#define ELSA_CMD_CONSOLE        "/config/command/console"
#define ELSA_CMD_SUSPEND        "/config/command/suspend"
#define ELSA_CMD_XAUTH_PATH     "/config/command/xauth/path"
#define ELSA_CMD_XAUTH_FILE     "/config/command/xauth/file"
#define ELSA_CMD_LOGIN          "/config/command/login"
#define ELSA_CMD_SESSION_START  "/config/command/session/start"
#define ELSA_CMD_SESSION_STOP   "/config/command/session/stop"
#define ELSA_DAEMONIZE          "/config/state/daemonize"
#define ELSA_NUMLOCK            "/config/state/nomlock"
#define ELSA_SESIONS            "/config/state/sessions"
#define ELSA_FILE_LOCK          "/config/file/lock"
#define ELSA_FILE_LOG           "/config/file/log"


typedef struct _Elsa_Config Elsa_Config;
struct _Elsa_Config
{
   char *session_path;
   struct {
      struct {
         char *path;
         char *args;
      } xinit;
      struct {
         char *path;
         char *file;
      } xauth;
      struct {
         char *start;
         char *login;
         char *stop;
      } session;
      char *shutdown;
      char *reboot;
      char *suspend;
   } command;
   Eina_Bool daemonize :1;
   Eina_Bool numlock :1;
   char *sessions;
   char *lockfile;
   char *logfile;
};

Elsa_Config *elsa_config;

void elsa_config_init();
void elsa_config_shutdown();

 /** @} */
#endif /* ELSA_CONFIG_H_ */
