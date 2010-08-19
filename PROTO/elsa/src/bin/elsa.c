#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#include "elsa.h"

#define ELSA_DISPLAY ":0.0"

static Ecore_Event_Handler *_del_handler = NULL;
static Ecore_Event_Handler *_exit_handler = NULL;

static Eina_Bool _testing = EINA_FALSE;

static void _elsa_help ();
static Eina_Bool _event_del_cb(void *data, int type, void *event);
static Eina_Bool _event_exit_cb(void *data, int type, void *event);
static Eina_Bool _open_log();
static Eina_Bool _close_log();
static void _remove_lock();


Ecore_Exe *x_exec;

static Eina_Bool
_event_del_cb(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   fprintf(stderr, PACKAGE": del cb received\n");
   elsa_session_shutdown();
   /* plz check here if X are still running */
   elsa_main();
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_event_exit_cb(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   fprintf(stderr, PACKAGE": daemon quit\n");
   elsa_session_shutdown();
   if (x_exec) ecore_exe_terminate(x_exec);
   elsa_pam_shutdown();
   _remove_lock();
   _close_log();
   ecore_shutdown();
   return ECORE_CALLBACK_DONE;
}


static void
_start_xserver(char *dname)
{
   char buf[4096];
   snprintf(buf, sizeof(buf),
            "%s %s",
            elsa_config->command.xinit_path,
            elsa_config->command.xinit_args);
   x_exec = ecore_exe_run(buf, NULL);
   sleep(1);
   while (!XOpenDisplay(dname))
     {
        fprintf(stderr, PACKAGE": could not open display %s\n", dname);
        sleep(1);
     }

   ecore_exe_hup(x_exec);
   snprintf(buf, sizeof(buf), "DISPLAY=%s", dname);
   putenv(buf);
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
elsa_main() {
   fprintf(stderr, PACKAGE": Init\n");
   if (elsa_gui_init()) return 1;
   fprintf(stderr, PACKAGE": Run\n");
   elm_run();
   fprintf(stderr, PACKAGE": Shutdown\n");
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

   _del_handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                          _event_del_cb, NULL);
   _exit_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,
                                           _event_exit_cb, NULL);
   ecore_init();
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

   /* Initialise event handler */
#ifndef XNEST_DEBUG
   elsa_pam_init(PACKAGE, dname);
   elsa_session_auth(elsa_config->command.xauth_file);

#endif
   _start_xserver(dname);
   elm_init(argc, argv);
   elsa_main();
     {
        int status;
        pid_t wpid = -1;
        pid = elsa_session_pid_get();
        fprintf(stderr, PACKAGE": wait pid %d\n", pid);
        while (wpid != pid)
          {
             pid = wait(&status);
             fprintf(stderr, PACKAGE": pid %d quit\n", wpid);
          }
     }
   elsa_pam_shutdown();
   _remove_lock();
   _close_log();
   elsa_config_shutdown();
   ecore_shutdown();
   return 0;
}

