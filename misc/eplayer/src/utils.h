#ifndef __UTILS_H
#define __UTILS_H

#include <Evas.h>

int is_dir(const char *dir);
char *strstrip(char *str);
Evas_List *dir_get_files(const char *directory);

#endif

