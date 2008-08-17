#ifndef _ENNA_LIST_H
#define _ENNA_LIST_H

#include "enna_event.h"

EAPI Evas_Object   *enna_list_add(Evas * evas);
EAPI void           enna_list_icon_size_set(Evas_Object * obj, Evas_Coord w,
					    Evas_Coord h);

EAPI void           enna_list_append_with_icon_name(Evas_Object * obj,
						    const char *icon_name,
						    const char *label,
						    void (*func) (void *data,
								  void *data2),
						    void (*func_hilight) (void
									  *data,
									  void
									  *data2),
						    void *data, void *data2);

EAPI void           enna_list_append(Evas_Object * obj, Evas_Object * icon,
				     const char *label, int header,
				     void (*func) (void *data, void *data2),
				     void (*func_hilight) (void *data,
							   void *data2),
				     void *data, void *data2);
EAPI void           enna_list_selected_set(Evas_Object * obj, int n);
EAPI int            enna_list_selected_get(Evas_Object * obj);
EAPI void           enna_list_in_playlist_set(Evas_Object * obj,
					      unsigned char in_pl, int n);
EAPI const char    *enna_list_selected_label_get(Evas_Object * obj);
EAPI const char    *enna_list_label_get(Evas_Object * obj, int n);
EAPI void          *enna_list_selected_data_get(Evas_Object * obj);
EAPI void          *enna_list_selected_data2_get(Evas_Object * obj);
EAPI void           enna_list_selected_geometry_get(Evas_Object * obj,
						    Evas_Coord * x,
						    Evas_Coord * y,
						    Evas_Coord * w,
						    Evas_Coord * h);
EAPI void           enna_list_min_size_get(Evas_Object * obj, Evas_Coord * w,
					   Evas_Coord * h);
EAPI void           enna_list_selector_set(Evas_Object * obj, int selector);
EAPI int            enna_list_selector_get(Evas_Object * obj);
EAPI void           enna_list_unselect(Evas_Object * obj);
EAPI void           enna_list_remove_num(Evas_Object * obj, int n);
EAPI void           enna_list_remove_label(Evas_Object * obj,
					   const char *label);
EAPI const char    *enna_list_nth_label_get(Evas_Object * obj, int n);
EAPI void           enna_list_nth_label_set(Evas_Object * obj, int n,
					    const char *label);
EAPI void           enna_list_nth_icon_set(Evas_Object * obj, int n,
					   Evas_Object * icon);
EAPI Evas_Object   *enna_list_nth_icon_get(Evas_Object * obj, int n);
EAPI Evas_Object   *enna_list_selected_icon_get(Evas_Object * obj);
EAPI int            enna_list_count(Evas_Object * obj);
EAPI void           enna_list_clear(Evas_Object * obj);
EAPI int            enna_list_nth_is_header(Evas_Object * obj, int n);
EAPI void           enna_list_process_event(Evas_Object * obj,
					    enna_event event);
EAPI void           enna_list_center(Evas_Object * obj);
#endif
