#include "ephoto.h"

Ephoto_Config *ec;
Ephoto_Main *em;

int 
main(int argc, char **argv)
{
	int i;

        if (!ewl_init(&argc, argv))
        {
                printf("Ewl is not usable, please check your installation!\n");
                return 1;
        }

	em = NULL;
        em = calloc(1, sizeof(Ephoto_Main));

        em->albums = ecore_list_new();
        em->fsystem = ecore_list_new();
        em->images = ecore_dlist_new();

        em->types = ecore_hash_new(ecore_str_hash, ecore_str_compare); 
	ecore_hash_set(em->types, "image/gif", "image");
        ecore_hash_set(em->types, "image/jpeg", "image");
        ecore_hash_set(em->types, "image/png", "image");
        ecore_hash_set(em->types, "image/svg+xml", "image");
        ecore_hash_set(em->types, "image/tiff", "image");

	ec = NULL;
	ec = calloc(1, sizeof(Ephoto_Config));

	for(i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--help"))
		{
			printf( "Help Page\n"
				"ephoto %%f - Opens an image for viewing\n");
			return 0;
		}
		else
		{
			if(strncmp(argv[i], "/", 1))
			{
				char buf[PATH_MAX], file[PATH_MAX];	

				getcwd(buf, PATH_MAX);
        			buf[sizeof(buf)-1] = '\0';
				 
				snprintf(file, PATH_MAX, "%s/%s", buf, argv[i]);
				if (ecore_file_exists(file))
					ec->requested_image = file;
			}
			else
				if (ecore_file_exists(argv[i]))
					ec->requested_image = argv[i];
		}
	}

#ifdef ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	create_main();
	
	ewl_main();

	return 0;
}

