#include "ephoto.h"

static void _ephoto_display_usage(void);

/* Global log domain pointer */
int __log_domain = -1;

int 
main(int argc, char **argv)
{
	ethumb_client_init();
	elm_need_efreet();
	elm_init(argc, argv);

        __log_domain = eina_log_domain_register("Ephoto", EINA_COLOR_BLUE);
        if (!__log_domain)
        {
                EINA_LOG_ERR("Could not register log domain: Ephoto");
                elm_shutdown();
                efreet_mime_shutdown();
                ethumb_client_shutdown();

                return 0;
        }


        DBG("Logging initialized");
	if (argc > 2)
	{
		printf("Too Many Arguments!\n");
		_ephoto_display_usage();
		
                eina_log_domain_unregister(__log_domain);
		elm_shutdown();
        	efreet_mime_shutdown();
        	ethumb_client_shutdown();

		return 0;
	}
	else if (argc < 2)
	{
		ephoto_create_main_window(NULL, NULL);
	}
	else if (!strncmp(argv[1], "--help", 6))
	{
		_ephoto_display_usage();

                eina_log_domain_unregister(__log_domain);
		elm_shutdown();
        	efreet_mime_shutdown();
        	ethumb_client_shutdown();

		return 0;
	}
	else if (ecore_file_is_dir(argv[1]))
	{
                char *real;

                real = ecore_file_realpath(argv[1]);
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
		
                eina_log_domain_unregister(__log_domain);
		elm_shutdown();
                efreet_mime_shutdown();
                ethumb_client_shutdown();

                return 0;
	}

	elm_run();

	elm_shutdown();
	efreet_mime_shutdown();
	ethumb_client_shutdown();

	return 0;
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

