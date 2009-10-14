#include "ephoto.h"

Getopts_Data *getopts;

const Ecore_Getopt general_opts =
{
	"ephoto",
	NULL,
	"5.15",
	"(C) 2009 - Stephen okra Houston <UnixTitan@gmail.com>",
	"BSD License",
	"Ephoto - Version 5.15\n"
	" Fancy Image Viewing\n", 
	0,
	{
		ECORE_GETOPT_COUNT
		('\0',"use-software", "Use the Software engine to render this program"),
		ECORE_GETOPT_COUNT
		('\0', "use-xrender", "Use the Xrender engine to render this program"),
		ECORE_GETOPT_COUNT
		('\0', "use-opengl", "Use the OpenGL engine to render this program"),
		ECORE_GETOPT_LICENSE('L', "license"),
		ECORE_GETOPT_COPYRIGHT('C', "copyright"),
		ECORE_GETOPT_VERSION('V', "version"),
		ECORE_GETOPT_HELP('h', "help"),
		ECORE_GETOPT_SENTINEL
	}
};

static Eina_Bool
_ephoto_args_init(int argc, char **argv)
{
	getopts = calloc(1, sizeof(Getopts_Data));
	int arg_index;

	Ecore_Getopt_Value general_values[] = 
	{
		ECORE_GETOPT_VALUE_BOOL(getopts->software),
		ECORE_GETOPT_VALUE_BOOL(getopts->xrender),
		ECORE_GETOPT_VALUE_BOOL(getopts->opengl),
		ECORE_GETOPT_VALUE_BOOL(getopts->quit),
		ECORE_GETOPT_VALUE_BOOL(getopts->quit),
		ECORE_GETOPT_VALUE_BOOL(getopts->quit),
		ECORE_GETOPT_VALUE_BOOL(getopts->quit),
		ECORE_GETOPT_VALUE_NONE
	};
	arg_index = ecore_getopt_parse(&general_opts, general_values, argc, argv);
	
	if (getopts->quit)
	{
		return EINA_FALSE;
	}
	else
		return EINA_TRUE;
}

int main(int argc, char **argv)
{
	if (!eina_init())
        {
                printf("Eina failed. Please check your installation!\n");
                return 1;
        }
	if (!eet_init())
	{
		printf("Eet failed. Please check your installation!\n");
		return 1;
	}
	if (!ecore_init())
        {
                printf("Ecore failed. Please check your installation!\n");
                return 1;
        }
	if (!ecore_file_init())
        {
                printf("Ecore_File failed. Please check your installation!\n");
                return 1;
        }
	if (!evas_init())
	{
		printf("Evas failed. Please check your installation!\n");
		return 1;
	}
	if (!edje_init())
	{
		printf("Edje failed. Please check your installation!\n");
		return 1;
	}
	if (!ecore_evas_init())
	{
		printf("Ecore_Evas failed. Please check your installation!\n");
		return 1;
	}
	if (!efreet_mime_init())
	{
		printf("Efreet failed. Please check your installation!\n");
		return 1;
	}
	if (!ethumb_init())
	{
		printf("Ethumb failed. Please check your installation!\n");
		return 1;
	}
	if (!ethumb_client_init())
	{
		printf("Ethumb Client failed. Please check your installation!\n");
		return 1;
	}

	if (!_ephoto_args_init(argc, argv))
		return 0;

	create_main_window();

	ecore_main_loop_begin();

	ethumb_client_shutdown();
	ethumb_shutdown();
	efreet_mime_shutdown();
	ecore_evas_shutdown();	
	edje_shutdown();
	evas_shutdown();
	ecore_file_shutdown();
	ecore_shutdown();
	eet_shutdown();
	eina_shutdown();

	return 0;
}

