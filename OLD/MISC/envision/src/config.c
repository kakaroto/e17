/*
 * $Id$
 * vim:expandtab:ts=3:sts=3:sw=3
 */

#include "config.h"

int
envision_config_get(Config * config, int argc, char *argv[])
{
   /*  Usage  */
   if (argc < 2 || !strncmp(argv[1], "-", 1)) {
      printf_usage(argv);
      return(-1);
   } else {
      if (envision_parse_usage(config, argc, argv) == -1) {
         return(-1);
      }
   }

   /*  Config  */
   if (envision_parse_config(config) == -1)
      return(-1);

   return(0);
}

void
printf_usage(char *argv[])
{
   printf("Envision - Enlightenment Media Player\n");
   printf("=====================================\n");
   printf("Usage: %s [video] options\n\n", argv[0]);
   printf("  OPTIONS:\n\n");
   printf(" POSIX         GNU                 DESCRIPTION\n");
   printf("--------------------------------------------------------\n");
   printf(" -?, -h        --help              Print the Usage\n");
   printf(" -v            --version           Print the Version\n\n");
   printf(" -e <engine>   --engine=<engine>   Specify the Rendering\n");
   printf("                                   engine.\n");
}

int
envision_parse_usage(Config * config, int argc, char *argv[])
{
   int c;
   int option_index = 0;

   while (1) {
      c = getopt_long(argc, argv, OPTSTR, long_options, &option_index);
      if (c == -1)
         break;

      switch (c) {
         case '?':
            printf_usage(argv);
            return(-1);
         case 'v':
            printf("Envision Version: %s\n", VERSION);
            return(-1);
         case 'h':
            printf_usage(argv);
            return(-1);
         case 'e':
            config->engine = strdup (optarg);
            break;
      }
   }

   return (0);
}

int
envision_parse_config(Config * config)
{
   return (0);
}
