
#include <Elementary.h>
#include <Enlil.h>
#include "../../config.h"
#include "slideshow.h"

static int APP_LOG_DOMAIN;
static Enlil_Root *root;

static const Ecore_Getopt options = {
		"Enki-Slideshow",
		NULL,
		VERSION,
		"(C) 2010 Enki slideshow, see AUTHORS.",
		"LGPL with advertisement, see COPYING",
		"Display a slideshow in full screen !\n\n",
		1,
		{
				ECORE_GETOPT_VERSION('V', "version"),
				ECORE_GETOPT_COPYRIGHT('R', "copyright"),
				ECORE_GETOPT_LICENSE('L', "license"),
				ECORE_GETOPT_STORE_STR('l', "library", "Specify the location of a library"),
				ECORE_GETOPT_HELP('h', "help"),
				ECORE_GETOPT_SENTINEL
		}
};

static void _load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
static void _load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg);
static void _load_album_done_cb(void *data, Enlil_Load *load, Enlil_Root *root, Enlil_Album *album);



static void close_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   elm_exit();
}


int elm_main(int argc, char **argv)
{
	unsigned char exit_option = 0;
	char *root_path = NULL;

	enlil_init();
	APP_LOG_DOMAIN = eina_log_domain_register("Enki-Slideshow", "\033[34;1m");

	//ecore_getopt
	Ecore_Getopt_Value values[] = {
			ECORE_GETOPT_VALUE_BOOL(exit_option),
			ECORE_GETOPT_VALUE_BOOL(exit_option),
			ECORE_GETOPT_VALUE_BOOL(exit_option),
			ECORE_GETOPT_VALUE_STR(root_path),
			ECORE_GETOPT_VALUE_BOOL(exit_option),
	};
	ecore_app_args_set(argc, (const char **) argv);
	int nonargs = ecore_getopt_parse(&options, values, argc, argv);
	if (nonargs < 0)
		return 1;
	else if (nonargs != argc || !root_path)
	{
		fputs("Invalid non-option argument", stderr);
		ecore_getopt_help(stderr, &options);
		return 1;
	}

	if(exit_option)
		return 0;
	//


	elm_finger_size_set(1);

	//
	root = enlil_root_new(NULL, NULL, NULL,
			NULL, NULL, NULL,
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	enlil_root_path_set(root, root_path);

	Enlil_Load *load = enlil_load_new(root,
			_load_album_done_cb,
			_load_done_cb, _load_error_cb, root);

	enlil_load_run(load);
	//

	elm_run();
	enlil_shutdown();

	elm_shutdown();

	return 0;
}


static void _load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos)
{
	slideshow_root_add(root, NULL);
	slideshow_show();
	slideshow_start();
	evas_object_event_callback_add(slideshow_win_get(), EVAS_CALLBACK_HIDE, close_cb, NULL);
}


static void _load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg)
{
    printf("LOAD CB ERROR : %s\n",msg);
}

static void _load_album_done_cb(void *data, Enlil_Load *load,Enlil_Root *root, Enlil_Album *album)
{
	Eina_List *l;
	Enlil_Photo *photo;

	EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
	{
		Enlil_Photo_Data *data = calloc(1, sizeof(Enlil_Photo_Data));
		enlil_photo_user_data_set(photo, data, NULL);
	}
}


ELM_MAIN()

