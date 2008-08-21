#ifndef ENNA_BOX_H
#define ENNA_BOX_H

#include "enna.h"

EAPI Evas_Object   *enna_box_add(Evas * evas);
EAPI int            enna_box_freeze(Evas_Object * obj);
EAPI int            enna_box_thaw(Evas_Object * obj);
EAPI void           enna_box_orientation_set(Evas_Object * obj, int horizontal);
EAPI int            enna_box_orientation_get(Evas_Object * obj);
EAPI void           enna_box_homogenous_set(Evas_Object * obj, int homogenous);
EAPI int            enna_box_pack_start(Evas_Object * obj, Evas_Object * child);
EAPI int            enna_box_pack_end(Evas_Object * obj, Evas_Object * child);
EAPI int            enna_box_pack_before(Evas_Object * obj, Evas_Object * child,
					 Evas_Object * before);
EAPI int            enna_box_pack_after(Evas_Object * obj, Evas_Object * child,
					Evas_Object * after);
EAPI int            enna_box_pack_count_get(Evas_Object * obj);
EAPI Evas_Object   *enna_box_pack_object_nth(Evas_Object * obj, int n);
EAPI Evas_Object   *enna_box_pack_object_first(Evas_Object * obj);
EAPI Evas_Object   *enna_box_pack_object_last(Evas_Object * obj);
EAPI void           enna_box_pack_options_set(Evas_Object * obj, int fill_w,
					      int fill_h, int expand_w,
					      int expand_h, double align_x,
					      double align_y, Evas_Coord min_w,
					      Evas_Coord min_h,
					      Evas_Coord max_w,
					      Evas_Coord max_h);
EAPI void           enna_box_unpack(Evas_Object * obj);
EAPI void           enna_box_min_size_get(Evas_Object * obj, Evas_Coord * minw,
					  Evas_Coord * minh);
EAPI void           enna_box_max_size_get(Evas_Object * obj, Evas_Coord * maxw,
					  Evas_Coord * maxh);
EAPI void           enna_box_align_get(Evas_Object * obj, double *ax,
				       double *ay);
EAPI void           enna_box_align_set(Evas_Object * obj, double ax, double ay);

#endif
