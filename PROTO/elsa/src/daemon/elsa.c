#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#include <X11/Xlib.h>

#include "elsa.h"

#define ELSA_DISPLAY ":0.0"

static Eina_Bool _testing = EINA_FALSE;

static void _elsa_help ();
static Eina_Bool _open_log();
static Eina_Bool _close_log();
static void _remove_lock();
static void _signal_cb();


static void
_signal_cb(int sig)
{
   fprintf(stderr, PACKAGE": signal %d received\n", sig);
   elsa_session_shutdown();
   elsa_xserver_shutdown();
   exit(1);
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
        if (!fwrite(buf, strlen(buf), 1, f))
          {
             fclose(f);
             fprintf(stderr, PACKAGE": Couldn't write the lockfile\n");
             return EINA_FALSE;
          }
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
   if (!fwrite(buf, strlen(buf), 1, f))
     fprintf(stderr, PACKAGE": Coudn't update lockfile\n");
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
   if (!freopen(elsa_config->logfile, "a", stdout))
     fprintf(stderr, PACKAGE": Error on reopen stdout\n");
   setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
   if (!freopen(elsa_config->logfile, "a", stderr))
     fprintf(stderr, PACKAGE": Error on reopen stderr\n");
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
   fprintf(stderr, PACKAGE": Run client\n");
   ecore_exe_run("elsa_client", NULL);
   return 0;
}


int
main (int argc, char ** argv)
{
   char tmp;
   char *dname = strdup(ELSA_DISPLAY);

   while((tmp = getopt(argc, argv, "thp:n:d?")) != EOF)
     {
        switch (tmp)
          {
           case 'h' : _elsa_help();
                      return (1);
           case 't' : _testing = EINA_TRUE;
           default : continue;
          }
     }

   if(getuid() != 0 && !_testing)
     {
        fprintf(stderr, "Only root can run this program\n");
        return 1;
     }

   elsa_config_init();
   if (!_testing && !_get_lock())
     {
        exit(1);
     }

   if (elsa_config->daemonize)
     {
        if (daemon(0, 1) == -1)
          {
             fprintf(stderr, PACKAGE": Error on daemonize !");
             exit(1);
          }
        _update_lock();
     }

   if (!_open_log())
      exit(1);

   ecore_init();
   /* Initialise event handler */
   signal(SIGQUIT, _signal_cb);
   signal(SIGTERM, _signal_cb);
   signal(SIGKILL, _signal_cb);
   signal(SIGINT, _signal_cb);
   signal(SIGHUP, _signal_cb);
   signal(SIGPIPE, _signal_cb);
   signal(SIGALRM, _signal_cb);

   elsa_pam_init(PACKAGE, dname);
   free(dname);
   elsa_session_init(elsa_config->command.xauth_file);

   if (!_testing)
     elsa_xserver_init(elsa_main, dname);
   if (elsa_config->autologin)
     {
        ecore_main_loop_begin();
        elsa_config->autologin = EINA_FALSE;
        elsa_pam_item_set(ELSA_PAM_ITEM_USER, elsa_config->userlogin);
        elsa_session_login(elsa_config->command.session_login);
     }
   else
     {
        elsa_server_init();
        elsa_server_shutdown();
     }
   elsa_xserver_wait();
   elsa_xserver_shutdown();
   elsa_pam_shutdown();
   _remove_lock();
   elsa_config_shutdown();
   ecore_shutdown();
   fprintf(stderr, PACKAGE": Goodbye until next time :)\n\n\n");
   _close_log();
   return 0;
}

