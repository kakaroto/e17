#include "ephoto.h"

#ifndef ELM_LIB_QUICKLAUNCH

static void _ephoto_display_usage(void);
Ephoto *ephoto;

EAPI int
elm_main(int argc, char **argv)
{
   Ethumb_Client *client;
   int r = 0;

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
#endif

   eio_init();
   elm_need_efreet();
   elm_need_ethumb();
   elm_init(argc, argv);

   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

   if (!efreet_mime_init())
     printf("Ephoto could not init efreet_mime!\n");

   client = elm_thumb_ethumb_client_get();
   if (!client)
     {
        printf("Ephoto could not get ethumb_client; Terminating...\n");
        r = 1;
        goto end;
     }
   ethumb_client_crop_align_set(client, 0.5, 0.5);
   ethumb_client_aspect_set(client, ETHUMB_THUMB_CROP);
   ethumb_client_orientation_set(client, ETHUMB_THUMB_ORIENT_ORIGINAL);

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   ephoto = calloc(1, sizeof(Ephoto));
   ephoto->client = client;

   if (argc > 2)
     {
        printf("Too many arguments; Terminating...\n");
        _ephoto_display_usage();
        r = 1;
        goto end;
     }
   if (argc == 2)
     {
        if (!strncmp(argv[1], "--help", 6) || !strncmp(argv[1], "-h", 2))
          {
             _ephoto_display_usage();
             r = 0;
             goto end;
          }
        else if (argc == 2)
          {
             char *real = ecore_file_realpath(argv[1]);
             if (!real)
               {
                  printf("Invalid file or directory: '%s'; Terminating...\n", argv[1]);
                  r = 1;
                  goto end;
               }
             if (ecore_file_is_dir(real))
               ephoto->directory = eina_stringshare_add(real);
             else if (ecore_file_exists(real))
               ephoto->file = eina_stringshare_add(real);
             free(real);
          }
     }

   if (!ephoto_window_add())
     {
        printf("Could not create the main window; Terminating...\n");
        r = 1;
        goto end;
     }

   elm_run();

 end:
   if (ephoto)
     free(ephoto);
   efreet_mime_shutdown();
   elm_shutdown();
   eio_shutdown();
 
   return r;
}

static void
_ephoto_display_usage(void)
{
   printf("Ephoto Usage: \n"
          "ephoto --help | -h	: This page\n"
          "ephoto filename		: Specifies a file to open\n"
          "ephoto directory	: Specifies a directory to open\n");
}

#endif
ELM_MAIN()
