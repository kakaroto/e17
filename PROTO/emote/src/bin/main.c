#include "emote.h"

#ifndef ELM_LIB_QUICKLAUNCH

# define EM_MAX_LEVEL 2

/* local function prototypes */
static void _em_main_shutdown_push(int (*func)(void));
static void _em_main_shutdown(int errcode);
static void _em_main_interrupt(int x __UNUSED__, siginfo_t *info __UNUSED__, void *data __UNUSED__);

/* local variables */
static int (*_em_main_shutdown_func[EM_MAX_LEVEL])(void);
static int _em_main_level = 0;

/* public functions */
EAPI int 
elm_main(int argc __UNUSED__, char **argv __UNUSED__) 
{
   struct sigaction action;

# ifdef ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain(PACKAGE);
# endif

   /* trap keyboard interrupt from user (Ctrl + C) */
   action.sa_sigaction = _em_main_interrupt;
   action.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
   sigemptyset(&action.sa_mask);
   sigaction(SIGINT, &action, NULL);

   /* init our config subsystem */
   if (!em_config_init()) _em_main_shutdown(EXIT_FAILURE);
   _em_main_shutdown_push(em_config_shutdown);

   /* init our gui subsystem */
   if (!em_gui_init()) _em_main_shutdown(EXIT_FAILURE);
   _em_main_shutdown_push(em_gui_shutdown);

   /* start main loop */
   elm_run();

   /* shutdown elm */
   elm_shutdown();

   /* shutdown */
   _em_main_shutdown(EXIT_SUCCESS);

   return EXIT_SUCCESS;
}

/* local functions */
static void 
_em_main_shutdown_push(int (*func)(void)) 
{
   _em_main_level++;
   if (_em_main_level > EM_MAX_LEVEL) 
     {
        _em_main_level--;
        return;
     }
   _em_main_shutdown_func[_em_main_level - 1] = func;
}

static void 
_em_main_shutdown(int errcode) 
{
   int i = 0;

   /* loop the shutdown functions and call each one on the stack */
   for (i = (_em_main_level - 1); i >= 0; i--)
     (*_em_main_shutdown_func[i])();

   /* exit if we err'd */
   if (errcode < 0) exit(errcode);
}

static void 
_em_main_interrupt(int x __UNUSED__, siginfo_t *info __UNUSED__, void *data __UNUSED__) 
{
   printf("\nEmote: Caught Interrupt Signal, Exiting\n");

   /* if we are finished with init, then we need to call elm_exit
    * as the app is in a 'running' state else we have not completed our init 
    * function(s) so call our own shutdown */
   if (_em_main_level == EM_MAX_LEVEL)
     elm_exit();
   else 
     {
        _em_main_shutdown(EXIT_SUCCESS);
        exit(EXIT_SUCCESS);
     }
}

#endif
ELM_MAIN();
