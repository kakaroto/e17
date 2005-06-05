#include "exg_callback.h"

void 
_enter_cb(void *data, const char *str)
{
    Ecore_Exe *process;
    process = ecore_exe_run(strdup(str), NULL);
    ecore_main_loop_quit();
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
    char *command;
    
    command = esmart_text_entry_text_get(exg->txt);
    _enter_cb(exg,command);
}
