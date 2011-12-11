#include <e.h>
#include <stdio.h>
#include "e_mod_main.h"

/* local function prototypes */
static int _cb_event_right_button_up(void *data, int type, void *event);

/* local variables */
static E_Action *act = NULL;
Eina_Bool mode=EINA_FALSE;
int device_id=9;

/* public functions */
EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Right Mouse Button Emulator" };

static void
_e_mod_action_cb(E_Object *obj, const char *params)
{
   int id=0;
   char cmd[100];

   if (!strncmp(params, "go_emulate_rmb", 14))
     {
	id = (int)strtol(params+1, NULL, 10);
	if(id!=0) device_id = id;
	mode=!mode;
	if (mode)
          sprintf(cmd, "xinput set-button-map %d 3 2 1 4 5\0", device_id);
        else
	  sprintf(cmd, "xinput set-button-map %d 1 2 3 4 5\0", device_id);

// printf("CMD: %s\n", cmd);

        system(cmd);
     }
}


EAPI void *
e_modapi_init(E_Module *m) 
{
   act = e_action_add("emulate-rmb");
   if (act)
     {
        act->func.go = _e_mod_action_cb;
	act->func.go_key = _e_mod_action_cb;
	act->func.go_mouse = _e_mod_action_cb;
	act->func.go_edge = _e_mod_action_cb;

	e_action_predef_name_set("Right Mouse Button Emulator", "Right Mouse Button Toggle",
	                                 "emulate-rmb", "go_emulate_rmb,9", NULL, 0);
     }
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m) 
{
   return 1;
}
