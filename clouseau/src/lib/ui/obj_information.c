#include "obj_information.h"

static Evas_Object *prop_list = NULL;

Evas_Object *
clouseau_obj_information_list_add(Evas_Object *parent)
{
   return prop_list = elm_list_add(parent);
}

void
clouseau_obj_information_list_populate(Tree_Item *treeit)
{
   elm_list_clear(prop_list);

   if (!treeit->parent)
      return;

   Evas_Object *obj = treeit->data.obj;

   /* Populate properties list */
     {
        char buf[1024];
        Eina_Bool visibility;
        Evas_Coord x, y, w, h;
        double dx, dy;

        visibility = evas_object_visible_get(obj);
        snprintf(buf, sizeof(buf), "Visibility: %d", (int) visibility);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        if (evas_object_name_get(obj))
          {
             snprintf(buf, sizeof(buf), "Name: %s",
                   evas_object_name_get(obj));
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }

        snprintf(buf, sizeof(buf), "Layer: %hd",
              evas_object_layer_get(obj));
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_geometry_get(obj, &x, &y, &w, &h);
        snprintf(buf, sizeof(buf), "Position: %d %d", x, y);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
        snprintf(buf, sizeof(buf), "Size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

#if 0
        if (evas_object_clip_get(obj))
          {
             evas_object_geometry_get(
                   evas_object_clip_get(obj), &x, &y, &w, &h);
             snprintf(buf, sizeof(buf), "Clipper position: %d %d", x, y);
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
             snprintf(buf, sizeof(buf), "Clipper size: %d %d", w, h);
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }
#endif

        evas_object_size_hint_min_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Min size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_max_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Max size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_request_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Request size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_align_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Align: %.6lg %.6lg", dx, dy);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_weight_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Weight: %.6lg %.6lg", dx, dy);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        /* Handle color */
          {
             int r, g, b, a;
             evas_object_color_get(obj, &r, &g, &b, &a);
             snprintf(buf, sizeof(buf), "Color: %d %d %d %d", r, g, b, a);
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }

        if (evas_object_clipees_get(obj))
          {
             snprintf(buf, sizeof(buf), "Has clipees");
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }

        elm_list_go(prop_list);
     }
}

void
clouseau_obj_information_list_clear()
{
   elm_list_clear(prop_list);
}
