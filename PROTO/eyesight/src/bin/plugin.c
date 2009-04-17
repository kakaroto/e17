#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Ecore_Data.h>
#include <Evas.h>

#include "plugin.h"
#include "plugin_private.h"
#include "error.h"

#define PLUGIN_ERROR    0
#define PLUGIN_OK       1

#define PLUGIN_FILE     0
#define PLUGIN_NAME     1
#define PLUGIN_VERSION  2
#define PLUGIN_EMAIL    3

static Eina_Bool
_plugin_id_for_each(const Eina_List *list, void *val, void *_data)
{
   Plist_Data *data = _data;
   Plugin *plugin = malloc(sizeof(Plugin));
   plugin->name = NULL;
   plugin->version = NULL;
   plugin->email = NULL;
   plugin->file = NULL;
   plugin->state = PLUGIN_ERROR;
   Plugin_Identify_Func identify;
   
   plugin->plugin = ecore_plugin_load(data->pg, val, NULL);
   identify = ecore_plugin_symbol_get(plugin->plugin, "identify");
   if (identify)
   {
      identify(&plugin->name, &plugin->version, &plugin->email);
      if (plugin->name && plugin->version && plugin->email)
      {
         plugin->state = PLUGIN_OK;
         plugin->file = val;
         ecore_list_append(data->plugin_list, plugin);
      }            
      else
      {
         append_startup_error(data->startup_errors, ERROR_PLUGIN_NO_ID, val);
         free(val);
         free(plugin);
      }
   }
   else
   {
      append_startup_error2(data->startup_errors, ERROR_PLUGIN_NO_SYMBOL,
                            "identify", val);
      free(val);
      free(plugin);
   }
   return EINA_TRUE;
}

static void
plugin_init_for_each(void *_val, void *_data)
{      
   Plugin *val = _val;
   Plugin_Init_Func init = NULL;
   Plist_Data *data = _data;
   
   init = ecore_plugin_symbol_get(val->plugin, "init");
   if (init)
   {
      switch (init(&(val->plugin_data)))
      {
      case PLUGIN_INIT_API_MISMATCH:
         append_startup_error2(data->startup_errors, ERROR_PLUGIN_INIT_API_MISMATCH,
                               val->name, val->email);
         break;
      case PLUGIN_INIT_FAIL:
         append_startup_error(data->startup_errors, ERROR_PLUGIN_INIT_FAIL,
                              val->name);
         break;
      }
   }
   else
      append_startup_error2(data->startup_errors, ERROR_PLUGIN_NO_SYMBOL,
                            "init", val->name);
}

Plist_Data *
plugin_init(Eina_List **startup_errors)
{
   char *tmp;
   Ecore_Path_Group *pg;
   Eina_List *plugins;
   Eina_Iterator *it;
   Plist_Data *plist_data;
 
   /* Find plugins */  
   pg = ecore_path_group_new();
   
   // TODO: plugin.c: get path group from config file  
   ecore_path_group_add(pg, PACKAGE_PLUGIN_DIR);
   
   // TODO: plugin.c: getenv("HOME"): Windows compatibility?
   tmp = calloc(sizeof(char), strlen(getenv("HOME")) + strlen("/.eyesight/plugins") + 1);
   snprintf(tmp, strlen(getenv("HOME")) + strlen("/.eyesight/plugins") +1, 
            "%s/.eyesight/plugins", getenv("HOME"));
   ecore_path_group_add(pg, tmp);
   
   plugins = ecore_plugin_available_get(pg);
   
   free(tmp);
   
   /* Identify plugins */
   
   plist_data = malloc(sizeof(Plist_Data));
   plist_data->pg = pg;
   plist_data->plugin_list = ecore_list_new();
   plist_data->startup_errors = startup_errors;
      
   it = eina_list_iterator_new(plugins);
   if (it)
     {
	eina_iterator_foreach(it, EINA_EACH(_plugin_id_for_each), plist_data);
	eina_iterator_free(it);
     }
   eina_list_free(plugins);
   
   /* Init plugins */
   
   ecore_list_for_each(plist_data->plugin_list, plugin_init_for_each, NULL);
   
   return plist_data; // for later use
}
