#include "eclair_config.h"
#include <Ecore_File.h>
#include <stdio.h>

Evas_Bool eclair_config_init(Eclair_Config *config)
{
   char *home;

   if (!config)
      return 0;

   config->config_file = NULL;
   
   if (!(home = getenv("HOME")))
      return 0;

   sprintf(config->config_dir_path, "%s/.eclair/", home);
   ecore_file_mkdir(config->config_dir_path);

   sprintf(config->covers_dir_path, "%scovers/", config->config_dir_path);
   ecore_file_mkdir(config->covers_dir_path);

   sprintf(config->config_file_path, "%seclair.cfg", config->config_dir_path);
   config->config_file = fopen(config->config_file_path, "a+t");
   if (!config->config_file)
      return 0;

   return 1;
}

void eclair_config_shutdown(Eclair_Config *config)
{
   if (!config)
      return;

   if (config->config_file)
      fclose(config->config_file);
}
