#include <Evas.h>
#include <Ecore_Data.h>
#include <stdio.h>

#include "plugin.h"

char
file_open(Evas *evas, Evas_Object *main_window, Ecore_List *plugins, char *file)
{
   // Check plugins in the list if they can open it, break on success
   Plugin *plugin = NULL;
   Plugin_Open_File_Func open_file = NULL;

   // TEMP
   Plugin_Show_Func show;
   int i;

   for (i = 0;;i++)
   {
      plugin = ecore_list_index_goto(plugins, i);
      if (!plugin)
         break; // No more plugins!

      // Try to open
      open_file = ecore_plugin_symbol_get(plugin->plugin, "open_file");
      if (!open_file)
         break;
      if (!open_file(&(plugin->plugin_data), file, main_window, evas))
         continue;

      // TEMP
      show = ecore_plugin_symbol_get(plugin->plugin, "show");
      show(&(plugin->plugin_data), file, evas);
      return 1;
   }

   if (plugin && !open_file)
      printf("No \'open\' symbol found in plugin %s",
             plugin->name);
   return 0;
}
