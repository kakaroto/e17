#ifndef _ENNA_MINIPLAYER_H
# define _ENNA_MINIPLAYER_H

# include "enna.h"

EAPI Evas_Object   *enna_miniplayer_add(Evas * evas);
EAPI void           enna_miniplayer_infos_set(Evas_Object * obj, char *title,
					      char *album, char *artist,
					      char *cover);
EAPI void           enna_miniplayer_play(Evas_Object * obj);
EAPI void           enna_miniplayer_pause(Evas_Object * obj);
EAPI void           enna_miniplayer_stop(Evas_Object * obj);
EAPI void           enna_miniplayer_select(Evas_Object *obj);
EAPI void           enna_miniplayer_unselect(Evas_Object *obj);

#endif
