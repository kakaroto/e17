#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include "elm_indicator.h"
#include "elm_home.h"
#include "elm_ind_win.h"

#ifndef ELM_LIB_QUICKLAUNCH

/* local function prototypes */

/* local variables */
static Eina_List *ind_wins = NULL;
static Eina_List *handlers = NULL;
static int my = 0;

EAPI int 
elm_main(int argc, char **argv) 
{
   Elm_Ind_Win *iwin;
   Ecore_Event_Handler *handler;
   Ecore_X_Window *roots = NULL;
   int num = 0, i = 0;

   if (!elm_home_init()) return EXIT_FAILURE;

   roots = ecore_x_window_root_list(&num);
   if ((!roots) || (num <= 0)) 
     {
        elm_home_shutdown();
        return EXIT_FAILURE;
     }
   for (i = 0; i < num; i++) 
     {
        Ecore_X_Window *zones;
        int x = 0, count = 0;

        count = 
          ecore_x_window_prop_window_list_get(roots[i], 
                                              ECORE_X_ATOM_E_ILLUME_ZONE_LIST, 
                                              &zones);
        if (!zones) continue;
        for (x = 0; x < count; x++) 
          {
             if (!(iwin = elm_ind_win_new(zones[x]))) continue;
             ind_wins = eina_list_append(ind_wins, iwin);
          }
        free(zones);
     }
   free(roots);

   elm_run();

   elm_home_shutdown();

   EINA_LIST_FREE(ind_wins, iwin) 
     {
        EINA_LIST_FREE(iwin->handlers, handler)
          ecore_event_handler_del(handler);
        free(iwin);
     }

   elm_shutdown();
   return EXIT_SUCCESS;
}

#endif
ELM_MAIN();
