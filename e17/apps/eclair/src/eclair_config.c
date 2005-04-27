#include "eclair_config.h"
#include <stdlib.h>

Evas_Bool eclair_config_init(Eclair_Config *config)
{
   char *home;

   if (!config)
      return 0;

   config->config_dir_path[0] = 0;
   config->covers_dir_path[0] = 0;
   config->config_file_path[0] = 0;
   config->config_file = NULL;
   
   if (!(home = getenv("HOME")))
      return 0;
   strcat(config->config_dir_path, home);
   strcat(config->config_dir_path, "/.eclair/");
   mkdir(config->config_dir_path, 0755);

   strcat(config->covers_dir_path, config->config_dir_path);
   strcat(config->covers_dir_path, "covers/");
   mkdir(config->covers_dir_path, 0755);

   printf("%s\n", config->covers_dir_path);

   strcat(config->config_file_path, config->config_dir_path);
   strcat(config->config_file_path, "eclair.cfg");
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
