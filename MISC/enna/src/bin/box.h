#ifndef ENNA_BOX_H
#define ENNA_BOX_H

#include "enna.h"

Evas_Object *enna_box_add               (Evas *evas);
int          enna_box_freeze            (Evas_Object *obj);
int          enna_box_thaw              (Evas_Object *obj);
void         enna_box_orientation_set   (Evas_Object *obj, int horizontal);
int          enna_box_orientation_get   (Evas_Object *obj);
void         enna_box_homogenous_set    (Evas_Object *obj, int homogenous);
int          enna_box_pack_start        (Evas_Object *obj, Evas_Object *child);
int          enna_box_pack_end          (Evas_Object *obj, Evas_Object *child);
int          enna_box_pack_before       (Evas_Object *obj, Evas_Object *child, Evas_Object *before);
int          enna_box_pack_after        (Evas_Object *obj, Evas_Object *child, Evas_Object *after);
void         enna_box_pack_options_set  (Evas_Object *obj, int fill_w, int fill_h, int expand_w, int expand_h, double align_x, double align_y, Evas_Coord min_w, Evas_Coord min_h, Evas_Coord max_w, Evas_Coord max_h);
void         enna_box_unpack            (Evas_Object *obj);
void         enna_box_min_size_get      (Evas_Object *obj, Evas_Coord *minw, Evas_Coord *minh);
void         enna_box_max_size_get      (Evas_Object *obj, Evas_Coord *maxw, Evas_Coord *maxh);

#endif
