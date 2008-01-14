#ifndef _ENNA_MODULE_VIDEO_H
# define _ENNA_MODULE_VIDEO_H

EAPI Evas_Object   *enna_module_video_add(Evas * evas);
EAPI void           enna_module_video_process_event(Evas_Object * obj,
						    enna_event event);
EAPI void           enna_module_video_focus_set(Evas_Object * obj,
						unsigned int focus);
#endif
