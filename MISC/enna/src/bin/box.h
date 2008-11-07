#ifndef ENNA_BOX_H
#define ENNA_BOX_H

#include "enna.h"

Evas_Object *enna_box_add               (Evas *evas);
void         enna_box_orientation_set   (Evas_Object *obj, int horizontal);
int          enna_box_orientation_get   (Evas_Object *obj);
void         enna_box_homogenous_set    (Evas_Object *obj, int homogenous);
int          enna_box_pack_start        (Evas_Object *obj, Evas_Object *child);
int          enna_box_pack_end          (Evas_Object *obj, Evas_Object *child);
int          enna_box_pack_before       (Evas_Object *obj, Evas_Object *child, Evas_Object *before);
int          enna_box_pack_after        (Evas_Object *obj, Evas_Object *child, Evas_Object *after);
void         enna_box_unpack            (Evas_Object *obj);

#endif
