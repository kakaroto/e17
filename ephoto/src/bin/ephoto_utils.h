#ifndef _EPHOTO_UTILS_H_
#define _EPHOTO_UTILS_H_

#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <Efreet_Mime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Ecore_List *get_directories(const char *directory);
Ecore_List *get_images(const char *directory);
const char *get_file_size(int size);
const char *get_image_pixels(const char *file);

#endif
