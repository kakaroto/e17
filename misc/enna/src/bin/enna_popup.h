#ifndef _ENNA_POPUP_H
#define _ENNA_POPUP_H

#include "enna_event.h"

EAPI Evas_Object   *enna_popup_add(Evas * evas);
EAPI void           enna_popup_process_event(Evas_Object * obj,
					     enna_event event);
EAPI void           enna_popup_file_set(Evas_Object * obj, char *filename);

#endif
