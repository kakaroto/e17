#include "ephoto.h"

static void _ephoto_display_usage(void);

/* Global log domain pointer */
int __log_domain = -1;

int
main(int argc, char **argv)
{
   Ethumb_Client *client;
   int r = 0;

   elm_need_efreet();
   elm_need_ethumb();
   elm_init(argc, argv);

   if (!efreet_mime_init())
     fprintf(stderr, "Could not init efreet_mime!\n");

   client = elm_thumb_ethumb_client_get();
   if (!client)
     {
        ERR("could not get ethumb_client");
        r = 1;
        goto end_log_domain;
     }
   ethumb_client_crop_align_set(client, 0.5, 0.5);
   ethumb_client_aspect_set(client, ETHUMB_THUMB_CROP);
   ethumb_client_orientation_set(client, ETHUMB_THUMB_ORIENT_ORIGINAL);
   __log_domain = eina_log_domain_register("ephoto", EINA_COLOR_BLUE);
   if (!__log_domain)
     {
        EINA_LOG_ERR("Could not register log domain: Ephoto");
        r = 1;
        goto end_log_domain;
     }


   DBG("Logging initialized");
   if (argc > 2)
     {
        printf("Too Many Arguments!\n");
        _ephoto_display_usage();
        r = 1;
        goto end;
     }
   else if (argc < 2)
     {
        ephoto_create_main_window(NULL, NULL);
     }
   else if (!strncmp(argv[1], "--help", 6))
     {
        _ephoto_display_usage();
        r = 0;
        goto end;
     }
   else if (ecore_file_is_dir(argv[1]))
     {
        char *real = ecore_file_realpath(argv[1]);
        ephoto_create_main_window(real, NULL);
        free(real);
     }
   else if (ecore_file_exists(argv[1]))
     {
        char *directory, *real;
        const char *image;

        image = eina_stringshare_add(argv[1]);
        directory = ecore_file_dir_get(argv[1]);
        real = ecore_file_realpath(directory);
        ephoto_create_main_window(real, image);
        free(directory);
        free(real);
     }
   else
     {
        printf("Incorrect Argument!\n");
        _ephoto_display_usage();
        r = 1;
        goto end;
     }

   elm_run();

 end:
   eina_log_domain_unregister(__log_domain);
 end_log_domain:
   efreet_mime_shutdown();
   elm_shutdown();

   return r;
}

/*Display useage commands for ephoto*/
static void
_ephoto_display_usage(void)
{
   printf("Ephoto Useage: \n"
          "ephoto --help   : This page\n"
          "ephoto filename : Specifies a file to open\n"
          "ephoto dirname  : Specifies a directory to open\n");
}

