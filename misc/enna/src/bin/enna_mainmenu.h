/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef ENNA_MAINMENU_H
#define ENNA_MAINMENU_H

#include "enna.h"
#include "enna_box.h"
#include "enna_event.h"

EAPI Evas_Object   *enna_mainmenu_add(Evas * evas);
EAPI unsigned int   enna_mainmenu_has_focus_get(Evas_Object * obj);
EAPI void           enna_mainmenu_has_focus_set(Evas_Object * obj,
						unsigned int focus);
EAPI void           enna_mainmenu_append(Evas_Object * obj, Evas_Object * icon,
					 const char *label, int miniplayer,
					 void (*func) (void *data, void *data2),
					 void (*func_hilight) (void *data,
							       void *data2),
					 void *data, void *data2);
EAPI void           enna_mainmenu_selected_set(Evas_Object * obj, int n);
EAPI void           enna_mainmenu_active_set(Evas_Object * obj, int n);
EAPI int            enna_mainmenu_selected_get(Evas_Object * obj);
EAPI void           enna_mainmenu_process_event(Evas_Object * obj,
						enna_event event);

#endif
