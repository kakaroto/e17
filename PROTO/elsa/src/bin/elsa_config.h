#ifndef ELSA_CONFIG_H_
#define ELSA_CONFIG_H_

#define ELSA_CONFIG_FILE        "elsa.cfg"

typedef struct _Elsa_Config Elsa_Config;

struct _Elsa_Config
{
   char *session_path;
   struct
     {
        char *xinit_path;
        char *xinit_args;
        char *xauth_path;
        char *xauth_file;
        char *session_start;
        char *session_login;
        char *session_stop;
        char *shutdown;
        char *reboot;
        char *suspend;
     } command;
   Eina_Bool daemonize;// :1;
   Eina_Bool numlock;// :1;
   Eina_Bool xsessions;
   char *lockfile;
   char *logfile;
};

Elsa_Config *elsa_config;

void elsa_config_init();
void elsa_config_shutdown();

 /** @} */
#endif /* ELSA_CONFIG_H_ */
