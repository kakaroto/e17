#ifndef __ENNA_SMART_PLAYER_H__
#define __ENNA_SMART_PLAYER_H__

#include "enna.h"

Evas_Object *enna_smart_player_add(Evas * evas);
void enna_smart_player_snapshot_set(Evas_Object *obj,
                                         Enna_Metadata *metadata);
void enna_smart_player_cover_set(Evas_Object *obj,
                                      Enna_Metadata *metadata);
void enna_smart_player_metadata_set(Evas_Object *obj,
        Enna_Metadata *metadata);
void enna_smart_player_show_video(Evas_Object *obj);
void enna_smart_player_hide_video(Evas_Object *obj);
#endif
