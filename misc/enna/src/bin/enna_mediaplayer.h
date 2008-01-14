#ifndef _ENNA_MEDIAPLAYER_H
# define _ENNA_MEDIAPLAYER_H

# include "enna.h"
# include "enna_volume_manager.h"

# define ENNA_MEDIA_TYPE_MUSIC_FILE   0
# define ENNA_MEDIA_TYPE_MUSIC_CDDA   1
# define ENNA_MEDIA_TYPE_VIDEO_FILE   2
# define ENNA_MEDIA_TYPE_PICTURE_FILE 3
# define ENNA_MEDIA_TYPE_VIDEO_DVD 4

EAPI Evas_Object   *enna_mediaplayer_add(Evas * evas);
EAPI Evas_Object   *enna_mediaplayer_menu_get(Evas_Object * obj);
EAPI void           enna_mediaplayer_process_event(Evas_Object * obj,
						   enna_event event);
EAPI void           enna_mediaplayer_focus_set(Evas_Object * obj,
					       unsigned int focus);
EAPI int            enna_mediaplayer_has_focus_get(Evas_Object * obj);
EAPI int            enna_mediaplayer_play(Evas_Object * obj, char *filename,
					  int type, Volume * v);

#endif
