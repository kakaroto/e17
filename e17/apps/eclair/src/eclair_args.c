#include "eclair_args.h"
#include <getopt.h>
#include "eclair.h"

static void _eclair_args_print_usage();

//Parse the arguments of the application and stock the filenames to load in the list
Evas_Bool eclair_args_parse(Eclair *eclair, Evas_List **filenames)
{
   int c, i;
   int argc;
   char **argv;

   static struct option long_options[] =
   {
      { "help",         no_argument,            NULL,    'h' },
      { "theme",        required_argument,      NULL,    't' },
      { "gui-engine",   required_argument,      NULL,    'g' },
      { "video-engine", required_argument,      NULL,    'v' },
      { "video-module", required_argument,      NULL,    'm' },
      { NULL,           0,                      NULL,    0 }
   };

   if (!eclair || (argc = *eclair->argc) <= 0 || !(argv = *eclair->argv))
      return 0;

   while ((c = getopt_long(argc, argv, "ht:g:v:m:", long_options, NULL)) != -1)
   {
      switch (c)
      {
         case 0:
            break;
         case 'h':
            _eclair_args_print_usage();
            return 0;
            break;
         case 't':
            free(eclair->gui_theme_file);
            eclair->gui_theme_file = strdup(optarg);
            break;
         case 'g':
            if (strcmp(optarg, "software") == 0) 
               eclair->video_engine = ECLAIR_SOFTWARE;
            else if (strcmp(optarg, "gl") == 0)
               eclair->gui_engine = ECLAIR_GL;
            else
            {
               _eclair_args_print_usage();
               return 0;
            }
            break;
         case 'v':
            if (strcmp(optarg, "software") == 0)
               eclair->video_engine = ECLAIR_SOFTWARE;
            else if (strcmp(optarg, "gl") == 0)
               eclair->video_engine = ECLAIR_GL;
            else
            {
               _eclair_args_print_usage();
               return 0;
            }
            break;
         case 'm':
            if (strcmp(optarg, "xine") == 0)
               eclair->video_module = ECLAIR_VIDEO_XINE;
            else if (strcmp(optarg, "gstreamer") == 0)
               eclair->video_engine = ECLAIR_VIDEO_GSTREAMER;
            else
            {
               _eclair_args_print_usage();
               return 0;
            }
            break;
         default:
            _eclair_args_print_usage();
            return 0;
            break;
      }
   }
   if (optind < argc)
   {
      for (i = optind; i < argc; i++)
         *filenames = evas_list_append(*filenames, argv[i]);
   }
   return 1;
}

static void _eclair_args_print_usage()
{
   fprintf(stdout, "Usage: eclair [arguments] file(s)\n"
      "\n"
      "Available arguments:\n"
      "-h, --help                            Print this message and exit\n"
      "-t, --theme                           Specify an edj theme file for eclair\n"
      "-g, --gui-engine [software|gl]        Specify the gui engine\n"
      "-v, --video-engine [software|gl]      Specify the video engine\n"
      "-m, --video-module [xine|gstreamer]   Specify the video module\n");
}
