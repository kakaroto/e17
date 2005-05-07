#include "eclair_config.h"
#include <sys/types.h>
#include <sys/stat.h>
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
   mkdir(config->config_dir_path, 0755);

   sprintf(config->covers_dir_path, "%scovers/", config->config_dir_path);
   mkdir(config->config_dir_path, 0755);

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
