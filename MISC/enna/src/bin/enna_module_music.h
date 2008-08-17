#ifndef _ENNA_MODULE_MUSIC_H
#define _ENNA_MODULE_MUSIC_H

EAPI Evas_Object   *enna_module_music_add(Evas * evas);
EAPI void           enna_module_music_process_event(Evas_Object * obj,
						    enna_event event);
EAPI void           enna_module_music_focus_set(Evas_Object * obj,
						unsigned int focus);
#endif
