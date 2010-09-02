#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#include <X11/Xlib.h>

#include "elsa.h"

#define ELSA_DISPLAY ":0.0"

static Ecore_Event_Handler *_exit_handler = NULL;

static Eina_Bool _testing = EINA_FALSE;

static void _elsa_help ();
static Eina_Bool _event_exit_cb(void *data, int type, void *event);
static Eina_Bool _open_log();
static Eina_Bool _close_log();
static void _remove_lock();

static Eina_Bool
_event_exit_cb(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   fprintf(stderr, PACKAGE": daemon quit\n");
   elsa_session_shutdown();
   elm_exit();
   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_get_lock()
{
   FILE *f;
   char buf[128];

   f = fopen(elsa_config->lockfile, "r");
   if (!f)
     {
        /* No lockfile, so create one */
        f = fopen(elsa_config->lockfile, "w");
        if (!f)
          {
             fprintf(stderr, PACKAGE": Couldn't create lockfile %s!\n",
                     elsa_config->lockfile);
             return (EINA_FALSE);
          }
        snprintf(buf, sizeof(buf), "%d\n", getpid());
        fwrite(buf, strlen(buf), 1, f);
        fclose(f);
     }
   else
     {
        /* read the lockfile */
        fclose(f);
        fprintf(stderr, "A lock file are present another instance are present ?\n");
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

static void
_update_lock()
{
   FILE *f;
   char buf[128];
   f = fopen(elsa_config->lockfile, "w");
   snprintf(buf, sizeof(buf), "%d\n", getpid());
   fwrite(buf, strlen(buf), 1, f);
   fclose(f);
}

static void
_remove_lock()
{
   remove(elsa_config->lockfile);
}

static Eina_Bool
_open_log()
{
   FILE *elog;
   elog = fopen(elsa_config->logfile, "a");
   if (!elog)
     {
        fprintf(stderr, PACKAGE": could not open logfile %s!!!\n",
                elsa_config->logfile);
        return EINA_FALSE;
     }
   fclose(elog);
   freopen(elsa_config->logfile, "a", stdout);
   setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
   freopen(elsa_config->logfile, "a", stderr);
   setvbuf(stderr, NULL, _IONBF, BUFSIZ);
   return EINA_TRUE;
}

static Eina_Bool
_close_log()
{
   fclose(stderr);
   fclose(stdout);
   return EINA_TRUE;
}

static void
_elsa_help() {
   fprintf(stderr, "Usage: %s [options...]\n", PACKAGE);
   fprintf(stderr, "\th: print this help message\n");
}

int
elsa_main()
{
   fprintf(stderr, PACKAGE": Init\n");
   if (elsa_gui_init()) return 1;
   fprintf(stderr, PACKAGE": Run\n");
   return 0;
}


int
main (int argc, char ** argv)
{
   char tmp;
   char *dname = ELSA_DISPLAY;
   pid_t pid;

   while((tmp = getopt(argc, argv, "vhp:n:d?")) != EOF)
     {
        switch (tmp)
          {
           case 'h' : _elsa_help();
                      return (1);
           default : continue;
          }
     }

   if(getuid() != 0 && !_testing)
     {
        fprintf(stderr, "Only root can run this program\n");
        return 1;
     }

   elsa_config_init();
   if (!_get_lock())
     {
        exit(1);
     }

   if (elsa_config->daemonize)
     {
        if (daemon(0, 1) == -1)
          {
             fprintf(stderr, "Elsa: Error on daemonize !");
             exit(1);
          }
        _update_lock();
     }

   if (!_open_log())
      exit(1);

   ecore_init();
   /* Initialise event handler */
   _exit_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,
                                           _event_exit_cb, NULL);
#ifndef XNEST_DEBUG
   elsa_pam_init(PACKAGE, dname);
   elsa_session_init(elsa_config->command.xauth_file);

#endif
   elsa_xserver_init(elsa_main, dname);
   ecore_main_loop_begin();
   elsa_pam_shutdown();
   _remove_lock();
   _close_log();
   elsa_xserver_shutdown();
   elsa_config_shutdown();
   ecore_shutdown();
   fprintf(stderr, PACKAGE": quit normaly :)\n");
   return 0;
}

