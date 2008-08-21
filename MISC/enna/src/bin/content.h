#ifndef _ENNA_CONTENT_H
#define _ENNA_CONTENT_H

#include "enna.h"
EAPI Evas_Object   *enna_content_add(Evas *evas);
EAPI int            enna_content_append(const char *name, Evas_Object *content);
EAPI int            enna_content_select(const char *name);
#endif
