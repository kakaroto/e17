/*
   Module       : filter.c
   Purpose      : Filter images based on their extension
   More         : see eplay README
   Homepage     : http://mandrake.net
   Policy       : GNU GPL
 */

#include <ctype.h>
#include <string.h>
#include "eplay.h"

int check_image(char *name)
{
	int length = strlen(name);
	char temp[length + 1];
	int i = 0;

	while (i < length) {
		temp[i] = tolower(name[i]);
		++i;
	}
	temp[i] = '\0';
	i = 0;
	while (strcmp(image_extensions[i], "end")) {
		if (strstr(temp, image_extensions[i++]))
			return 1;
	}
	return 0;
}

void filter_images(int *images, char **image_names)
{
	char image_ok;
	int i = 0;
	while (i < *images) {
		image_ok = 0;
		if (check_image(image_names[i]))
			image_ok = 1;
		if (!image_ok) {
			int j = i;
			while (j < *images - 1) {
				image_names[j] = image_names[j + 1];
				++j;
			}
			--(*images);
		} else
			++i;
	}
}
