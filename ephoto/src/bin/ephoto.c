#include "ephoto.h"

Ephoto_Main *em;

int 
main(int argc, char **argv)
{
	char *arg;
	int i;

#ifdef ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	for (i=0; i < argc; i++)
	{
		arg = argv[i];	
		if (!strcmp(arg, "-h"))
		{
			printf("Ephoto Help\n"
			       "ephoto -h: This help page.\n"
			       "ephoto -s /path/to/image: Simple image viewer.\n");
				return 0;
		}
		if (!strcmp(arg, "-s") && i <= (argc-2))
		{
			i++;
			arg = argv[i];
			if (ecore_file_exists(arg))
			{
				if (!ecore_init())
				{
					printf("Error - Could not load Ecore.\n");
					return 1;
				}
				show_ephoto_simple(arg);
				ecore_main_loop_begin();
				ecore_shutdown();
				return 0;
			}
		}
	}
	if (!ewl_init(&argc, argv))
        {
                printf("Ewl is not usable, check your installation!\n");
                return 1;
        }
	create_main();
	ewl_main();
	return 0;
}

