#ifndef _ENNA_CONTENT_H
#define _ENNA_CONTENT_H

#include "enna.h"
Evas_Object *enna_content_add(Evas *evas);
int enna_content_append(const char *name, Evas_Object *content);
int enna_content_select(const char *name);
void enna_content_show();
void enna_content_hide();
#endif
