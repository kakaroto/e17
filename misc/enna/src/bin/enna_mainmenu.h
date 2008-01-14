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
EAPI void           enna_mainmenu_icon_size_set(Evas_Object * obj, Evas_Coord w,
						Evas_Coord h);
EAPI void           enna_mainmenu_append(Evas_Object * obj, Evas_Object * icon,
					 const char *label, int miniplayer,
					 void (*func) (void *data, void *data2),
					 void (*func_hilight) (void *data,
							       void *data2),
					 void *data, void *data2);
EAPI void           enna_mainmenu_prepend(Evas_Object * obj, Evas_Object * icon,
					  const char *label, int miniplayer,
					  void (*func) (void *data,
							void *data2),
					  void (*func_hilight) (void *data,
								void *data2),
					  void *data, void *data2);
EAPI void           enna_mainmenu_selected_set(Evas_Object * obj, int n);
EAPI void           enna_mainmenu_active_set(Evas_Object * obj, int n);
EAPI int            enna_mainmenu_selected_get(Evas_Object * obj);
EAPI const char    *enna_mainmenu_selected_label_get(Evas_Object * obj);
EAPI void          *enna_mainmenu_selected_data_get(Evas_Object * obj);
EAPI void          *enna_mainmenu_selected_data2_get(Evas_Object * obj);
EAPI void           enna_mainmenu_selected_geometry_get(Evas_Object * obj,
							Evas_Coord * x,
							Evas_Coord * y,
							Evas_Coord * w,
							Evas_Coord * h);
EAPI void           enna_mainmenu_min_size_get(Evas_Object * obj,
					       Evas_Coord * w, Evas_Coord * h);
EAPI void           enna_mainmenu_selector_set(Evas_Object * obj, int selector);
EAPI int            enna_mainmenu_selector_get(Evas_Object * obj);
EAPI void           enna_mainmenu_unselect(Evas_Object * obj);
EAPI void           enna_mainmenu_remove_num(Evas_Object * obj, int n);
EAPI void           enna_mainmenu_remove_label(Evas_Object * obj,
					       const char *label);
EAPI const char    *enna_mainmenu_nth_label_get(Evas_Object * obj, int n);
EAPI void           enna_mainmenu_nth_label_set(Evas_Object * obj, int n,
						const char *label);
EAPI void           enna_mainmenu_nth_icon_set(Evas_Object * obj, int n,
					       Evas_Object * icon);
EAPI Evas_Object   *enna_mainmenu_nth_icon_get(Evas_Object * obj, int n);
EAPI Evas_Object   *enna_mainmenu_selected_icon_get(Evas_Object * obj);
EAPI int            enna_mainmenu_count(Evas_Object * obj);
EAPI void           enna_mainmenu_clear(Evas_Object * obj);
EAPI int            enna_mainmenu_nth_is_header(Evas_Object * obj, int n);
EAPI void           enna_mainmenu_process_event(Evas_Object * obj,
						enna_event event);
EAPI void           enna_mainmenu_center(Evas_Object * obj);
#endif
