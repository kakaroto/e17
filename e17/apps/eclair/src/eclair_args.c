#include "eclair_args.h"

//Parse the arguments of the application and stock the filenames to load in the list
Evas_Bool eclair_args_parse(Eclair *eclair, int argc, char *argv[], Evas_List **filenames)
{
   int i;

   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')
      {
         if (strcmp(argv[i], "--help") == 0)
         {
            //TODO: Display help message
            printf("Help: (TODO)\n--gui-engine [software|gl]\n--video-engine [software|gl]\n");
            return 0;
         }
         else if (strcmp(argv[i], "--gui-engine") == 0)
         {
            if (argc <= i + 1)
            {
               printf("Correct gui engines are: software gl\nDefault is: software\n\n");
               return 0;
            }
            else
            {
               i++;
               if (strcmp(argv[i], "software") == 0)
                  eclair->gui_engine = ECLAIR_SOFTWARE;
               else if (strcmp(argv[i], "gl") == 0)
                  eclair->gui_engine = ECLAIR_GL;
               else
               {
                  printf("Correct gui engines are: software gl\nDefault is: software\n\n");
                  return 0;
               }
            }
         }
         else if (strcmp(argv[i], "--video-engine") == 0)
         {
            if (argc <= i + 1)
            {
               printf("Correct video engines are: software gl\nDefault is: software\n\n");
               return 0;
            }
            else
            {
               i++;
               if (strcmp(argv[i], "software") == 0)
                  eclair->video_engine = ECLAIR_SOFTWARE;
               else if (strcmp(argv[i], "gl") == 0)
                  eclair->video_engine = ECLAIR_GL;
               else
               {
                  printf("Correct video engines are: software gl\nDefault is: software\n\n");
                  return 0;
               }
            }
         }
         else
            printf("Unknown option: %s\nTry `eclair --help' for more information.\n\n", argv[i]);
      }
      else
         *filenames = evas_list_append(*filenames, argv[i]);
   }

   return 1;
}
