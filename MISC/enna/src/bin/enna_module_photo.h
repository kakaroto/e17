#ifndef _ENNA_MODULE_PHOTO_H
# define _ENNA_MODULE_PHOTO_H

#include "enna.h"
#include "enna_list.h"
#include "enna_fm.h"
#include "enna_config.h"
#include "enna_volume_manager.h"
#include "enna_mediaplayer.h"
#include "enna_image.h"

EAPI Evas_Object   *enna_module_photo_add(Evas * evas);
EAPI void           enna_module_photo_process_event(Evas_Object * obj,
						    enna_event event);
EAPI void           enna_module_photo_focus_set(Evas_Object * obj,
						unsigned int focus);
#endif
