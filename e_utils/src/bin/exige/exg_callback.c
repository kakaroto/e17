#include "exg_callback.h"
#include "exg_gui.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

void 
_enter_cb(void *data, const char *str)
{
    Exige *exg =data;
    exg_run(exg, str);
}

void
key_cb(void *data, Evas *e, Evas_Object *o, void *ei)
{
    Exige *exg =data;
    eapp_display(exg);
}

void
_exg_quit(void *data , Evas_Object *obj, const char *emission, 
	  const char *src)
{
    ecore_main_loop_quit();
}

void
_run_cb(void *data , Evas_Object *obj, const char *emission,
	const char *src)
{
    Exige *exg = data;
    const char *command;
    
    command = esmart_text_entry_text_get(exg->txt);
    exg_run(exg, command);
}

void
exg_run(Exige *exg, const char *str)
{
    Ecore_Exe *process;
    char command[PATH_MAX+1];
    const char *check_state;
    
    check_state = edje_object_part_state_get(exg->gui, "check_button", NULL);

    if (!strcmp(check_state, "default"))
    {
	snprintf(command, sizeof(command),"%s",str);
    }
    else if (!strcmp(check_state, "on"))
    {
	snprintf(command, sizeof(command),"%s -e %s",exg_conf_term_get(),str);
    }

    process = ecore_exe_run(strdup(command), NULL);
    ecore_main_loop_quit();   
}
