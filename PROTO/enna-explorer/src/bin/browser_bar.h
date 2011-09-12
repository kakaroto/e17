#ifndef BROWSER_BAR_H
#define BROWSER_BAR_H

#include <Elementary.h>

#include "file.h"

Evas_Object *enna_browser_bar_add(Evas_Object *parent, Enna_File *file);
void enna_browser_file_set(Evas_Object *obj, Enna_File *file);

#endif /* BROWSER_BAR_H */
