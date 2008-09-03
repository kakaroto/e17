#ifndef FILE_H_
#define FILE_H_

#include "plugin.h"
#include <Evas.h>
#include <Ecore_Data.h>

char file_open(Evas *evas, Evas_Object *main_window, Ecore_List *plugins, 
               char *file);

#endif /*FILE_H_*/
