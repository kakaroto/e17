#ifndef __ENTROPY_FILE_H_
#define __ENTROPY_FILE_H_

#include "entropy_generic.h"

Ecore_List* entropy_generic_file_list_sort(Ecore_List* file_list);
entropy_generic_file* entropy_generic_file_new();
void entropy_generic_file_destroy(entropy_generic_file* file);
void entropy_thumbnail_destroy(entropy_thumbnail*);
char* entropy_generic_file_display_permissions_parse(entropy_generic_file* file);

#endif
