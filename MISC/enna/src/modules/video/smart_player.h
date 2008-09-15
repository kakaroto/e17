#ifndef __ENNA_SMART_PLAYER_H__
#define __ENNA_SMART_PLAYER_H__

#include "enna.h"

EAPI Evas_Object *enna_smart_player_add(Evas * evas);
EAPI void enna_smart_player_metadata_set(Evas_Object *obj,
        Enna_Metadata *metadata);
EAPI void enna_smart_player_show_video(Evas_Object *obj);
EAPI void enna_smart_player_hide_video(Evas_Object *obj);
#endif
