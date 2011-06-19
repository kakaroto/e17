#include "slideshow_object.h"
#include <locale.h>

typedef struct _Smart_Data Smart_Data;

struct _Slideshow_Item
{
   Evas_Object *obj;
   void *data;
   Slideshow_Item_Class *itc;

   Evas_Object *icon;
};

typedef enum
{
   NONE, RIGHT, LEFT
} Enum_Movement;

struct _Smart_Data
{
   Evas_Object *obj;
   Evas_Object *bt_left;
   Evas_Object *bt_right;

   Enum_Movement movement;

   Eina_List *items; //list of Slideshow_Item*
   Eina_List *current_item;
};

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret)  \
  {                                                  \
     char *_e_smart_str;                             \
                                                     \
     if (!o) return ret;                             \
     smart = evas_object_smart_data_get(o);          \
     if (!smart) return ret;                         \
     _e_smart_str = (char *)evas_object_type_get(o); \
     if (!_e_smart_str) return ret;                  \
     if (strcmp(_e_smart_str, type)) return ret;     \
  }

#define E_SMART_OBJ_GET(smart, o, type)              \
  {                                                  \
     char *_e_smart_str;                             \
                                                     \
     if (!o) return;                                 \
     smart = evas_object_smart_data_get(o);          \
     if (!smart) return;                             \
     _e_smart_str = (char *)evas_object_type_get(o); \
     if (!_e_smart_str) return;                      \
     if (strcmp(_e_smart_str, type)) return;         \
  }

#define E_OBJ_NAME "slideshow_object"
static Evas_Smart *smart = NULL;

static void
_smart_init(void);
static void
_smart_add(Evas_Object *obj);
static void
_smart_del(Evas_Object *obj);
static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void
_smart_show(Evas_Object *obj);
static void
_smart_hide(Evas_Object *obj);
static void
_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void
_smart_clip_unset(Evas_Object *obj);

static void
_bt_left_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_right_cb(void *data, Evas_Object *obj, void *event_info);

static void
_right_done_cb(void *data, Evas_Object *obj, const char *signal,
               const char *source);
static void
_left_done_cb(void *data, Evas_Object *obj, const char *signal,
              const char *source);
static void
_done(Evas_Object *obj);
static void
_init(Evas_Object *obj);

static void
_update(Evas_Object *obj);

Evas_Object *
slideshow_object_add(Evas_Object *obj)
{
   _smart_init();
   return evas_object_smart_add(evas_object_evas_get(obj), smart);
}

void
slideshow_object_file_set(Evas_Object *obj, const char *file, const char *group)
{
   Evas_Object *bt, *icon;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (!sd->obj)
   {
      Evas_Coord x, y, w, h;
      sd->obj = edje_object_add(evas_object_evas_get(obj));
      evas_object_geometry_get(obj, &x, &y, &w, &h);
      evas_object_resize(sd->obj, w, h);
      evas_object_resize(sd->obj, x, y);
      evas_object_smart_member_add(sd->obj, obj);
   }

   edje_object_file_set(sd->obj, file, group);
   edje_object_signal_callback_add(sd->obj, "right,done", "", _right_done_cb,
                                   obj);
   edje_object_signal_callback_add(sd->obj, "left,done", "", _left_done_cb, obj);

   bt = elm_button_add(obj);
   sd->bt_left = bt;
   //elm_object_style_set(bt, "anchor");
   evas_object_size_hint_align_set(bt, -1, -1);
   evas_object_size_hint_weight_set(bt, 1, 1);
   edje_object_part_swallow(sd->obj, "object.swallow.left", bt);
   evas_object_smart_member_add(bt, obj);
   evas_object_smart_callback_add(bt, "clicked", _bt_left_cb, obj);
   evas_object_show(bt);

   icon = elm_icon_add(obj);
   elm_icon_standard_set(icon, "arrow_left");
   elm_button_icon_set(bt, icon);

   bt = elm_button_add(obj);
   sd->bt_right = bt;
   //elm_object_style_set(bt, "anchor");
   evas_object_size_hint_align_set(bt, -1, -1);
   evas_object_size_hint_weight_set(bt, 1, 1);
   edje_object_part_swallow(sd->obj, "object.swallow.right", bt);
   evas_object_smart_member_add(bt, obj);
   evas_object_smart_callback_add(bt, "clicked", _bt_right_cb, obj);
   evas_object_show(bt);

   icon = elm_icon_add(obj);
   elm_icon_standard_set(icon, "arrow_right");
   elm_button_icon_set(bt, icon);

   //manage the size
   Evas_Coord minw, minh, maxw, maxh;
   edje_object_size_min_get(sd->obj, &minw, &minh);
   edje_object_size_max_get(sd->obj, &maxw, &maxh);

   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, maxw, maxh);
   //
}

Slideshow_Item *
slideshow_object_item_append(Evas_Object *obj, Slideshow_Item_Class *itc,
                             void *data)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   Slideshow_Item *item = calloc(1, sizeof(Slideshow_Item));
   item->data = data;
   item->itc = itc;
   item->obj = obj;

   sd->items = eina_list_append(sd->items, item);

   _update(obj);
   return item;
}

void
slideshow_object_item_del(Slideshow_Item *item)
{
   Evas_Object *obj = item->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->items = eina_list_remove(sd->items, item);

   if (item == eina_list_data_get(sd->current_item)) sd->current_item = NULL;

   if (item->icon) evas_object_del(item->icon);
   free(item);

   if (!sd->current_item)
      _done(obj);
   else
      _update(obj);
}

void
slideshow_object_item_select(Evas_Object *obj, Slideshow_Item *item)
{
   Eina_List *l;
   Slideshow_Item *_item;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   EINA_LIST_FOREACH(sd->items, l, _item)
   {
      if(item == _item)
      break;
   }

   sd->current_item = l;
   _update(obj);
}

void *
slideshow_object_item_data_get(Slideshow_Item *item)
{
   return item->data;
}

Eina_List *
slideshow_object_items_get(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   return sd->items;
}

void
slideshow_object_item_update(Slideshow_Item *item)
{
   Evas_Object *obj = item->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   Eina_List *l;
   Slideshow_Item *_item;
   char buf[1024];

   if (item->icon)
   {
      //destroy the icon
      evas_object_del(item->icon);
      item->icon = NULL;
   }

   _update(obj);
}

static void
_update(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   Eina_List *l;
   Slideshow_Item *item;
   int i;
   if (!sd) return;
   char buf[1024];

   Slideshow_Item *items[7] =
      { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

   if (!sd->current_item) sd->current_item = sd->items;

   i = 4;
   EINA_LIST_FOREACH(sd->items, l, item)
   {
      if(!items[3] && l != sd->current_item)
      {
         if(items[0] && items[0]->icon)
         {
            evas_object_del(items[0]->icon);
            items[0]->icon = NULL;
         }
         items[0] = items[1];
         items[1] = items[2];
         items[2] = item;
      }
      else if(l == sd->current_item)
      items[3] = item;
      else if(i < 7)
      {
         items[i] = item;
         i++;
      }
      else if(item->icon)
      {
         //destroy the icon
         evas_object_del(item->icon);
         item->icon = NULL;
      }
   }

   for (i = 0; i < 7; i++)
   {
      if (items[i])
      {
         if (!items[i]->icon)
         {
            items[i]->icon = items[i]->itc->icon_get(items[i]->data, obj);
            evas_object_smart_member_add(items[i]->icon, obj);
            evas_object_stack_below(items[i]->icon, sd->bt_right);
         }

         snprintf(buf, sizeof(buf), "object.swallow.%d", i + 1);
         edje_object_part_unswallow(sd->obj, items[i]->icon);
         edje_object_part_swallow(sd->obj, buf, items[i]->icon);
      }
   }
}

void
_bt_left_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l, *prev = NULL;
   Slideshow_Item *item;
   Smart_Data *sd = evas_object_smart_data_get(data);
   if (!sd) return;

   if (sd->current_item == sd->items) return;

   EINA_LIST_FOREACH(sd->items, l, item)
   {
      if(l != sd->current_item)
      prev = l;
      else
      break;
   }

   if (sd->movement == LEFT && prev == sd->items) return;

   _init(data);

   sd->movement = LEFT;
   edje_object_signal_emit(sd->obj, "left", "");
}

void
_bt_right_cb(void *data, Evas_Object *obj, void *event_info)
{
   Smart_Data *sd = evas_object_smart_data_get(data);
   if (!sd) return;

   if (sd->current_item == eina_list_last(sd->items)) return;

   if (sd->movement == RIGHT && eina_list_next(sd->current_item)
            == eina_list_last(sd->items)) return;

   _init(data);

   sd->movement = RIGHT;
   edje_object_signal_emit(sd->obj, "right", "");
}

static void
_right_done_cb(void *data, Evas_Object *obj, const char *signal,
               const char *source)
{
   Smart_Data *sd = evas_object_smart_data_get(data);
   if (!sd) return;

   _done(data);
}

static void
_left_done_cb(void *data, Evas_Object *obj, const char *signal,
              const char *source)
{
   Smart_Data *sd = evas_object_smart_data_get(data);
   if (!sd) return;

   _done(data);
}

static void
_init(Evas_Object *obj)
{
   Eina_List *l, *prev = NULL;
   Slideshow_Item *item;

   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (sd->movement == NONE) return;

   switch (sd->movement)
   {
   case RIGHT:
      sd->current_item = eina_list_next(sd->current_item);
      break;

   case LEFT:
      EINA_LIST_FOREACH(sd->items, l, item)
      {
         if(l != sd->current_item)
         prev = l;
         else
         break;
      }
      sd->current_item = prev;
      break;

   case NONE:
      ;
   }
   _update(obj);
   edje_object_signal_emit(sd->obj, "init", "");

   sd->movement = NONE;
}

static void
_done(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   _init(obj);

   evas_object_smart_callback_call(obj, "selected",
                                   eina_list_data_get(sd->current_item));
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/

static void
_smart_init(void)
{
   if (smart) return;
   {
      static const Evas_Smart_Class sc =
         { E_OBJ_NAME, EVAS_SMART_CLASS_VERSION, _smart_add, _smart_del,
           _smart_move, _smart_resize, _smart_show, _smart_hide,
           _smart_color_set, _smart_clip_set, _smart_clip_unset, NULL, NULL,
           NULL, NULL };
      smart = evas_smart_class_new(&sc);
   }
}

static void
_smart_add(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   evas_object_smart_data_set(obj, sd);
}

static void
_smart_del(Evas_Object *obj)
{
   Smart_Data *sd;
   Eina_List *l;
   Slideshow_Item *item;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   EINA_LIST_FOREACH(sd->items, l, item)
   {
      if(item->icon)
      evas_object_del(item->icon);
      free(item);
   }

   evas_object_del(sd->bt_left);
   evas_object_del(sd->bt_right);

   if (sd->obj) evas_object_del(sd->obj);

   free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_move(sd->obj, x, y);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_resize(sd->obj, w, h);
}

static void
_smart_show(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_show(sd->obj);
   evas_object_show(sd->bt_left);
   evas_object_show(sd->bt_right);
}

static void
_smart_hide(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_hide(sd->obj);
   evas_object_hide(sd->bt_left);
   evas_object_hide(sd->bt_right);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_set(sd->obj, clip);
   evas_object_clip_set(sd->bt_left, clip);
   evas_object_clip_set(sd->bt_right, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_unset(sd->obj);
   evas_object_clip_unset(sd->bt_left);
   evas_object_clip_unset(sd->bt_right);
}

static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_color_set(sd->obj, r, g, b, a);
   evas_object_color_set(sd->bt_left, r, g, b, a);
   evas_object_color_set(sd->bt_right, r, g, b, a);
}

