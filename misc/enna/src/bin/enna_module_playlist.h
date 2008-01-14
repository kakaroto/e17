#ifndef _ENNA_MODULE_PLAYLIST_H
#define _ENNA_MODULE_PLAYLIST_H

EAPI Evas_Object   *enna_module_playlist_add(Evas * evas);
EAPI void           enna_module_playlist_process_event(Evas_Object * obj,
						       enna_event event);
EAPI void           enna_module_playlist_focus_set(Evas_Object * obj,
						   unsigned int focus);
#endif
