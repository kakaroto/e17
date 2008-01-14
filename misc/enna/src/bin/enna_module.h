#ifndef _ENNA_MODULE_H
#define _ENNA_MODULE_H

#include "enna_event.h"
#include "enna_module_music.h"
#include "enna_module_video.h"
#include "enna_module_photo.h"
#include "enna_module_playlist.h"
#include "enna_mediaplayer.h"

EAPI Evas_Object   *enna_module_add(Evas * evas);
EAPI unsigned int   enna_module_has_focus_get(Evas_Object * obj);
EAPI void           enna_module_has_focus_set(Evas_Object * obj,
					      unsigned int focus);
EAPI void           enna_module_append(Evas_Object * obj,
				       Evas_Object * obj_module, char *name,
				       unsigned char type);
EAPI void           enna_module_prepend(Evas_Object * obj,
					Evas_Object * obj_module, char *name,
					unsigned char type);
EAPI int            enna_module_selected_name_set(Evas_Object * obj,
						  char *name);
EAPI void           enna_module_selected_set(Evas_Object * obj, int n);
EAPI int            enna_module_selected_get(Evas_Object * obj);
EAPI void           enna_module_process_event(Evas_Object * obj,
					      enna_event event);
#endif
