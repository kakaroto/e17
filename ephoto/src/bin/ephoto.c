#include "ephoto.h"

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

