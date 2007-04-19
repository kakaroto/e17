#include "ephoto.h"

int main(int argc, char **argv)
{
	char *album, *name, *description, *path, input;
	int i;
	Ecore_List *albums, *images;
	sqlite3 *db;

	/*Check to make sure EWL is accessible*/
        if (!ewl_init(&argc, argv))
        {
                printf("Ewl is not usable, please check your installation!\n");
                return 1;
        }

	for(i = 0; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
		{
			printf("Ephoto Version %s Help Page\n", VERSION);
			printf("Long Commands: \n"
			       " --help			-	This Screen\n"
			       " --add-album %%s %%s	-	Adds Album\n"
			       "			 	arg1 = name of album\n"
			       "				arg2 = description of album\n"
			       " --add-image %%s %%s %%s	-	Adds Image\n"
			       "				arg1 = name of album to add image to\n"
			       "				arg2 = descriptive name for image\n"
			       "				arg3 = path to the image\n"
			       " --add-image-dir %%s %%s	-	Adds All Images From Directory\n"
			       "				arg1 = name of album to add images to\n"
			       "				arg2 = directory to get images from\n"	
			       " --list-albums		-	List Albums\n"
			       " --list-images %%s	-	List Images in Album\n"
			       "				arg1 = name of album to list images from\n"
			       " --remove-album	%%s	-	Removes Album\n"
			       "				arg1 = name of album\n"
			       " --remove-image	%%s %%s	-	Removes Image\n"
			       "				arg1 = name of album to remove image from\n"
			       "				arg2 = path of the image to be removed\n");
			printf("Short Commands: \n"
			       " -h			-	This Screen\n"
			       " -a %%s %%s		-	Adds Album\n"
			       " -i %%s %%s %%s		-	Adds Image\n"
			       " -id %%s %%s		-	Adds all Images From Directory\n"
			       " -la			-	List Albums\n"
			       " -li %%s			-	List Images in Album\n"
			       " -ra %%s			-	Removes Album\n"
			       " -ri %%s %%s		-	Removes Image\n");
			return 0;
		}
		if(!strcmp(argv[i], "--add-album") || !strcmp(argv[i], "-a"))
		{
			i++;
			if(argv[i]) name = argv[i];
			else
			{
				printf("Please specify a name for the album\n");
				return 1;
			}

			i++;
			if(argv[i]) description = argv[i];
			else 
			{
				printf("Please specify a description for the album\n");
				return 1;
			}

			printf("Are you sure you want to create an album with "
			       "the name %s and the description %s? ", 
			       name, description);
			scanf("%c", &input);
			if(input == 'y' || input == 'Y')
			{
				db = ephoto_db_init();
				ephoto_db_add_album(db, name, description);
				ephoto_db_close(db);
				printf("Album was created\n");
			}
			else printf("Album was not created\n");

			return 0;
		}
		if(!strcmp(argv[i], "--add-image") || !strcmp(argv[i], "-i"))
		{
			i++;
			if(argv[i]) album = argv[i];
			else
			{
				printf("Please specify the album you wish to add to\n");
				return 1;
			}

			i++;
			if(argv[i]) name = argv[i];
			else 
			{
				printf("Please specify a descriptive name for the image\n");
				return 1;
			}

			i++;
			if(argv[i] && ecore_file_exists(argv[i])) path = argv[i];
			else 
			{
				printf("Please specify a valid path to the image\n");
				return 1;
			}

			printf("Are you sure you want to add an image "
			       "to album %s with a name %s and path %s? ", 
			       album, name, path);
			scanf("%c", &input);
			if(input == 'y' || input == 'Y')
			{
				db = ephoto_db_init();
				ephoto_db_add_image(db, album, name, path);
				ephoto_db_close(db);
				printf("Image was added\n");
			}
			else printf("Image was not added\n");

			return 0;
		}
		if(!strcmp(argv[i], "--add-image-dir") || !strcmp(argv[i], "-id"))
		{
                        i++;
                        if(argv[i]) album = argv[i];
                        else
                        {
                                printf("Please specify the album you wish to add to\n");
                                return 1;
                        }

                        i++;
                        if(argv[i] && ecore_file_is_dir(argv[i])) path = argv[i];
                        else
                        {
                                printf("Please specify a valid path to the image directory\n");
                                return 1;
                        }

                        printf("Are you sure you want to add images "
                               "from the directory %s to the album %s? ",
                               path, album);
                        scanf("%c", &input);
                        if(input == 'y' || input == 'Y')
                        {
				db = ephoto_db_init();
				images = ecore_list_new();
				images = get_images(path);
				while (!ecore_list_is_empty(images))
				{
					name = ecore_list_remove_first(images);
					ephoto_db_add_image(db, album, basename(name), name);
				}
				ephoto_db_close(db);
				ecore_list_destroy(images);
				printf("Images were added\n");
			}
			else printf("Images were not added\n");
			
			return 0;
		}
		if(!strcmp(argv[i], "--list-albums") || !strcmp(argv[i], "-la"))
		{
			db = ephoto_db_init();
			albums = ecore_list_new();
			albums = ephoto_db_list_albums(db);
			ephoto_db_close(db);
			while(!ecore_list_is_empty(albums))
			{
				album = ecore_list_remove_first(albums);
				printf("%s\n", album);
			}
			ecore_list_destroy(albums);

			return 0;
		}
		if(!strcmp(argv[i], "--list-images") || !strcmp(argv[i], "-li"))
		{
			i++;
			if(argv[i]) album = argv[i];
			else
			{
				printf("Please specify the album to list images from\n");
				return 1;
			}
			
			db = ephoto_db_init();
			images = ephoto_db_list_images(db, album);
			ephoto_db_close(db);
			while(!ecore_list_is_empty(images))
			{
				path = ecore_list_remove_first(images);
				printf("%s\n", path);
			}
			return 0;
		}
		if(!strcmp(argv[i], "--remove-album") || !strcmp(argv[i], "-ra"))
		{
			i++;
			if(argv[i]) name = argv[i];
			else
			{
				printf("Please specify the name of the album\n");
				return 1;
			}

			printf("Are you sure you want to remove the album %s? ", 
			       name);
			scanf("%c", &input);
			if(input == 'y' || input == 'Y')
			{
				db = ephoto_db_init();
				ephoto_db_delete_album(db, name);
				ephoto_db_close(db);
				printf("Album was removed\n");
			}
			else printf("Album was not removed\n");

			return 0;
		}
		if(!strcmp(argv[i], "--remove-image") || !strcmp(argv[i], "-ri"))
		{
			i++;
			if(argv[i]) album = argv[i];
			else
			{
				printf("Please specify the name of the album "
				       "the image belongs to.\n");
				return 1;
			}

			i++;
			if(argv[i]) path = argv[i];
			else 
			{
				printf("Please specify the path of the image\n");
				return 1;
			}

			printf("Are you sure you want to remove the image %s "
			       "from the album %s? ", 
			       path, album);
			scanf("%c", &input);
			if(input == 'y' || input == 'Y')
			{
				db = ephoto_db_init();
				ephoto_db_delete_image(db, album, path);
				ephoto_db_close(db);
				printf("Image was removed\n");
			}
			else printf("Image was not removed\n");

			return 0;
		}
	}

	/* NLS */
#ifdef ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif
	/*Start the GUI*/
	create_main_gui();
	
	/*Star the ewl loop*/
	ewl_main();

	return 0;
}
