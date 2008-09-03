#include "plugin.h"
#include "document.h"
#include "animations.h"
#include <Evas.h>
#include <stdio.h>
#include <Edje.h>

#define ANIM_TIME 0.5

int
show_anim(void *_data)
{
   // TODO: pdf/animations.c: decelerating movement

   Document_Show_Anim_Data *data = _data;
   Evas_Object *object = data->object;
   //Evas *evas = data->evas;
   int start_x = data->start_x;
   int ew = data->ew;
   int x, y, w, delta_x, end_x;

   evas_object_geometry_get(object, &x, &y, &w, NULL);
   end_x = ew / 2 - w / 2;
   delta_x = (start_x - end_x) / (ANIM_TIME / FRAMETIME);

   if (x - delta_x <= end_x)
   {
      evas_object_move(object, end_x, y);
      return 0;
   }

   evas_object_move(object, x - delta_x, y);
   return 1;
}

int
page_next_animator(void *_data)
{
   Document_Nav_Animator_Data *data = _data;
   int x1, y1, w1, h1;
   int x2, y2, w2, h2;
   int ww, wh;
   int dy; // Delta Y

   evas_object_geometry_get(data->controls, NULL, NULL, &ww, &wh);
   evas_object_geometry_get(data->border, &x1, &y1, &w1, &h1);
   evas_object_geometry_get(data->tmp_border, &x2, &y2, &w2, &h2);

   dy = 2 * ++(data->step);

   // Move border1
   if (y1 - dy <= 0 - data->top_margin - h1)
      evas_object_move(data->border, x1, 0 - data->top_margin - h1);
   else
      evas_object_move(data->border, x1, y1 - dy);

   if (y2 - dy <= data->top_margin)
      evas_object_move(data->tmp_border, x2, data->top_margin);
   else
      evas_object_move(data->tmp_border, x2, y2-dy);

   if (y1 == 0 - data->top_margin - h1 && y2 == data->top_margin)
   {
      evas_object_del(edje_object_part_swallow_get(data->border,
                      "eyesight/border_opaque/content"));
      evas_object_del(data->border);
      evas_object_name_set(data->tmp_border, "border");
      evas_object_name_set(edje_object_part_swallow_get(data->tmp_border,
                           "eyesight/border_opaque/content"),
                           "page");
      evas_object_hide(data->border);
      return 0;   // Stop animation
   }
   else
      return 1;
}

int
page_prev_animator(void *_data)
{
   Document_Nav_Animator_Data *data = _data;
   int x1, y1, w1, h1;
   int x2, y2, w2, h2;
   int ww, wh;
   int dy; // Delta Y

   evas_object_geometry_get(data->controls, NULL, NULL, &ww, &wh);
   evas_object_geometry_get(data->border, &x1, &y1, &w1, &h1);
   evas_object_geometry_get(data->tmp_border, &x2, &y2, &w2, &h2);

   dy = 2 * ++(data->step);

   if (y1 + dy >= wh + data->bottom_margin)
      evas_object_move(data->border, x1, wh + data->bottom_margin);
   else
      evas_object_move(data->border, x1, y1 + dy);

   if (y2 + dy >= data->top_margin)
      evas_object_move(data->tmp_border, x2, data->top_margin);
   else
      evas_object_move(data->tmp_border, x2, y2 + dy);

   if (y1 == wh + data->bottom_margin && y2 == data->top_margin)
   {
      evas_object_del(edje_object_part_swallow_get(data->border,
                      "eyesight/border_opaque/content"));
      evas_object_del(data->border);
      evas_object_name_set(data->tmp_border, "border");
      evas_object_name_set(edje_object_part_swallow_get(data->tmp_border,
                           "eyesight/border_opaque/content"),
                           "page");
      return 0;   // Stop animation
   }
   else
      return 1;
}
