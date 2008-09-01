#include "main.h"

int
main(int argc, char **argv)
{
	Evas_List *l;
	char *string;
	edje_init();
	
	string = edje_file_data_get(argv[argc - 1], "theme/name");
	if (!string)
		string = "";
	printf("Name: %s\n", string);
	string = edje_file_data_get(argv[argc - 1], "theme/author");
	if (!string)
		string = "";
	printf("Author: %s\n", string);
	string = edje_file_data_get(argv[argc - 1], "theme/license");
	if (!string)
		string = "";
	printf("License: %s\n", string);
	string = edje_file_data_get(argv[argc - 1], "theme/version");
	if (!string)
		string = "";
	printf("Version: %s\n", string);
	for(l = edje_file_collection_list(argv[argc - 1]); l; l = l->next)
	{
		printf("%s\n", l->data);
	}
	edje_shutdown();
	return 0;
}
