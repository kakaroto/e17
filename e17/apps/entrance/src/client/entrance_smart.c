/**
 * entrance_smart.c:
 *
 */
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "entrance_smart.h"

/* smart object handlers */
static Evas_Smart *_entrance_smart_object_smart_get();
static Evas_Object *entrance_smart_object_new(Evas * evas);
void _entrance_smart_object_add(Evas_Object * o);
void _entrance_smart_object_del(Evas_Object * o);
void _entrance_smart_object_layer_set(Evas_Object * o, int l);
void _entrance_smart_object_raise(Evas_Object * o);
void _entrance_smart_object_lower(Evas_Object * o);
void _entrance_smart_object_stack_above(Evas_Object * o, Evas_Object * above);
void _entrance_smart_object_stack_below(Evas_Object * o, Evas_Object * below);
void _entrance_smart_object_move(Evas_Object * o, Evas_Coord x, Evas_Coord y);
void _entrance_smart_object_resize(Evas_Object * o, Evas_Coord w,
                                   Evas_Coord h);
void _entrance_smart_object_show(Evas_Object * o);
void _entrance_smart_object_hide(Evas_Object * o);
void _entrance_smart_object_color_set(Evas_Object * o, int r, int g, int b,
                                      int a);
void _entrance_smart_object_clip_set(Evas_Object * o, Evas_Object * clip);
void _entrance_smart_object_clip_unset(Evas_Object * o);

Evas_Object *
entrance_smart_add(Evas * e)
{
   Evas_Object *result = NULL;
   Entrance_Smart *data = NULL;

   if ((result = entrance_smart_object_new(e)))
   {
      if ((data = evas_object_smart_data_get(result)))
      {
         data->clip = evas_object_rectangle_add(e);
         evas_object_color_set(data->clip, 255, 255, 255, 255);
      }
   }
   return (result);
}

void
entrance_smart_edje_set(Evas_Object * o, Evas_Object * edje)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      if (edje)
      {
         evas_object_clip_set(edje, data->clip);
         evas_object_show(edje);
         data->edje = edje;
      }
   }
}

void
entrance_smart_avatar_set(Evas_Object * o, Evas_Object * avatar)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      if (avatar)
      {
         evas_object_clip_set(avatar, data->clip);
         evas_object_show(avatar);
         data->avatar = avatar;
      }
   }
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
entrance_smart_object_new(Evas * evas)
{
   Evas_Object *entrance_smart_object;

   entrance_smart_object =
      evas_object_smart_add(evas, _entrance_smart_object_smart_get());

   return entrance_smart_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_entrance_smart_object_smart_get()
{
   static Evas_Smart *smart = NULL;

   if (smart)
      return (smart);

   smart =
      evas_smart_new("entrance_smart_object", _entrance_smart_object_add,
                     _entrance_smart_object_del,
                     _entrance_smart_object_layer_set,
                     _entrance_smart_object_raise,
                     _entrance_smart_object_lower,
                     _entrance_smart_object_stack_above,
                     _entrance_smart_object_stack_below,
                     _entrance_smart_object_move,
                     _entrance_smart_object_resize,
                     _entrance_smart_object_show, _entrance_smart_object_hide,
                     _entrance_smart_object_color_set,
                     _entrance_smart_object_clip_set,
                     _entrance_smart_object_clip_unset, NULL);

   return smart;
}

void
_entrance_smart_object_add(Evas_Object * o)
{
   Entrance_Smart *data = NULL;

   data = malloc(sizeof(Entrance_Smart));
   memset(data, 0, sizeof(Entrance_Smart));
   evas_object_smart_data_set(o, data);
}


void
_entrance_smart_object_del(Evas_Object * o)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_del(data->avatar);
      evas_object_del(data->edje);
      evas_object_del(data->clip);
      free(data);
   }
}

void
_entrance_smart_object_layer_set(Evas_Object * o, int l)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_layer_set(data->clip, l);
      evas_object_layer_set(data->edje, l);
      evas_object_layer_set(data->avatar, l);
   }
}

void
_entrance_smart_object_raise(Evas_Object * o)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_raise(data->clip);
   }
}

void
_entrance_smart_object_lower(Evas_Object * o)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_lower(data->clip);
   }
}

void
_entrance_smart_object_stack_above(Evas_Object * o, Evas_Object * above)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_stack_above(data->clip, above);
   }
}

void
_entrance_smart_object_stack_below(Evas_Object * o, Evas_Object * below)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_stack_below(data->clip, below);
   }
}

void
_entrance_smart_object_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_move(data->clip, x, y);
      evas_object_move(data->edje, x, y);
   }
}

void
_entrance_smart_object_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_resize(data->clip, w, h);
      evas_object_resize(data->edje, w, h);
   }
}

void
_entrance_smart_object_show(Evas_Object * o)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_show(data->clip);
   }
}

void
_entrance_smart_object_hide(Evas_Object * o)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_hide(data->clip);
   }
}

void
_entrance_smart_object_color_set(Evas_Object * o, int r, int g, int b, int a)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_color_set(data->clip, r, g, b, a);
   }
}

void
_entrance_smart_object_clip_set(Evas_Object * o, Evas_Object * clip)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_clip_set(data->clip, clip);
   }
}

void
_entrance_smart_object_clip_unset(Evas_Object * o)
{
   Entrance_Smart *data;

   if ((data = evas_object_smart_data_get(o)))
   {
      evas_object_clip_unset(data->clip);
   }
}
