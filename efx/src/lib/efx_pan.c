#include "efx_private.h"

#define PAN_FUNC_CHECK Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), "efx_pan")))
#define PAN_CB_SETUP Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return

typedef struct Smart_Data
{
   EFX *e;
   Evas_Object *smart_obj;
   Evas_Object *child_obj;
   Evas_Coord   x, y, w, h;
   Evas_Coord   child_w, child_h, px, py, dx, dy;
} Smart_Data;

typedef struct Efx_Pan_Data
{
   EFX *e;
   Evas_Object *pan;
   Ecore_Animator *anim;
   Efx_Effect_Speed speed;
   Evas_Point change;
   Evas_Point current;
   int degrees;
   Efx_End_Cb cb;
   void *data;
} Efx_Pan_Data;

static void _smart_pan_child_set(Evas_Object *obj, Evas_Object *child);

/* local subsystem functions */
static void _smart_child_del_hook(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _smart_child_resize_hook(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _smart_reconfigure(Smart_Data *sd);
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object *obj);
static void _smart_init(void);

/* local subsystem globals */
static Evas_Smart *_smart = NULL;

/* local subsystem functions */
static void
_smart_child_del_hook(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Smart_Data *sd;

   sd = data;
   sd->child_obj = NULL;
}

static void
_smart_child_resize_hook(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Smart_Data *sd;
   Evas_Coord w, h;

   sd = data;
   evas_object_geometry_get(sd->child_obj, NULL, NULL, &w, &h);
   if ((w != sd->child_w) || (h != sd->child_h))
     {
        sd->child_w = w;
        sd->child_h = h;
        _smart_reconfigure(sd);
     }
}

static void
_smart_reconfigure(Smart_Data *sd)
{
   Eina_List *l;
   EFX *e;
   Evas_Coord x, y;

   evas_object_move(sd->child_obj, sd->x - sd->px, sd->y - sd->py);
   efx_maps_apply(sd->e, sd->child_obj, NULL, EFX_MAPS_APPLY_ALL);
   //DBG("DELTA: (%d,%d)", sd->dx, sd->dy);
   EINA_LIST_FOREACH(sd->e->followers, l, e)
     {
        evas_object_geometry_get(e->obj, &x, &y, NULL, NULL);
        evas_object_move(e->obj, x - sd->dx, y - sd->dy);
        efx_maps_apply(e, e->obj, NULL, EFX_MAPS_APPLY_ALL);
//        _size_debug(e->obj);
     }
}

static void
_smart_add(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   sd->smart_obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   evas_object_smart_data_set(obj, sd);
}

static void
_smart_del(Evas_Object *obj)
{
   PAN_CB_SETUP;
   _smart_pan_child_set(obj, NULL);
   free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   PAN_CB_SETUP;
   sd->x = x;
   sd->y = y;
   _smart_reconfigure(sd);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   PAN_CB_SETUP;
   sd->w = w;
   sd->h = h;
   _smart_reconfigure(sd);
}

static void
_smart_show(Evas_Object *obj)
{
   PAN_CB_SETUP;
   if (sd->child_obj)
     evas_object_show(sd->child_obj);
}

static void
_smart_hide(Evas_Object *obj)
{
   PAN_CB_SETUP;
   if (sd->child_obj)
     evas_object_hide(sd->child_obj);
}

static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   PAN_CB_SETUP;
   if (sd->child_obj)
     evas_object_color_set(sd->child_obj, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   PAN_CB_SETUP;
   if (sd->child_obj)
     evas_object_clip_set(sd->child_obj, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
   PAN_CB_SETUP;
   if (sd->child_obj)
     evas_object_clip_unset(sd->child_obj);
}

/* never need to touch this */

static void
_smart_init(void)
{
   if (_smart) return;
   {
      static const Evas_Smart_Class sc =
        {
           "efx_pan",
           EVAS_SMART_CLASS_VERSION,
           _smart_add,
           _smart_del,
           _smart_move,
           _smart_resize,
           _smart_show,
           _smart_hide,
           _smart_color_set,
           _smart_clip_set,
           _smart_clip_unset,
           NULL,
           NULL,
           NULL,
           NULL,
           NULL,
           NULL,
           NULL
        };
      _smart = evas_smart_class_new(&sc);
   }
}


static Evas_Object *
_smart_pan_add(Evas *evas)
{
   _smart_init();
   return evas_object_smart_add(evas, _smart);
}

static void
_smart_pan_child_set(Evas_Object *obj, Evas_Object *child)
{
   PAN_FUNC_CHECK return;
   if (child == sd->child_obj) return;
   if (sd->child_obj)
     {
        evas_object_clip_unset(sd->child_obj);
        evas_object_smart_member_del(sd->child_obj);
        evas_object_event_callback_del_full(sd->child_obj, EVAS_CALLBACK_FREE, _smart_child_del_hook, sd);
        evas_object_event_callback_del_full(sd->child_obj, EVAS_CALLBACK_RESIZE, _smart_child_resize_hook, sd);
        sd->child_obj = NULL;
     }
   if (child)
     {
        Evas_Coord w, h;
        int r, g, b, a;

        sd->child_obj = child;
        evas_object_smart_member_add(sd->child_obj, sd->smart_obj);
        evas_object_geometry_get(sd->child_obj, NULL, NULL, &w, &h);
        sd->child_w = w;
        sd->child_h = h;
        evas_object_event_callback_add(child, EVAS_CALLBACK_FREE, _smart_child_del_hook, sd);
        evas_object_event_callback_add(child, EVAS_CALLBACK_RESIZE, _smart_child_resize_hook, sd);
        evas_object_color_get(sd->smart_obj, &r, &g, &b, &a);
        evas_object_color_set(sd->child_obj, r, g, b, a);
        evas_object_clip_set(sd->child_obj, evas_object_clip_get(sd->smart_obj));
        if (evas_object_visible_get(sd->smart_obj)) evas_object_show(sd->child_obj);
        else evas_object_hide(sd->child_obj);
        _smart_reconfigure(sd);
     }
}

static void
_smart_pan_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   PAN_FUNC_CHECK return;
   if ((x == sd->px) && (y == sd->py)) return;
   sd->dx = x - sd->px;
   sd->dy = y - sd->py;
   sd->px = x;
   sd->py = y;
   _smart_reconfigure(sd);
}

static void
_smart_pan_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y)
{
   PAN_FUNC_CHECK return;
   if (x) *x = sd->px;
   if (y) *y = sd->py;
}

/*
static void
_smart_pan_max_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y)
{
   PAN_FUNC_CHECK return;
   if (x)
     {
        if (sd->w < sd->child_w) *x = sd->child_w - sd->w;
        else *x = 0;
     }
   if (y)
     {
        if (sd->h < sd->child_h) *y = sd->child_h - sd->h;
        else *y = 0;
     }
}
*/

static void
_obj_del(Efx_Pan_Data *epd, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (epd->anim) ecore_animator_del(epd->anim);
   if (epd->pan)
     {
        evas_object_del(epd->pan);
        epd->pan = NULL;
     }
   epd->e->pan_data = NULL;
   if ((!epd->e->owner) && (!epd->e->followers)) efx_free(epd->e);
   free(epd);
}

static Eina_Bool
_pan_cb(Efx_Pan_Data *epd, double pos)
{
   int x, y, px, py;
   double pct;

   pct = ecore_animator_pos_map(pos, epd->speed, 0, 0);
   x = lround(pct * (double)epd->change.x) - epd->current.x;
   y = lround(pct * (double)epd->change.y) - epd->current.y;
   _smart_pan_get(epd->pan, &px, &py);
   //DBG("PAN: (%d,%d) += (%d,%d)", px, py, x, y);
   _smart_pan_set(epd->pan, px + x, py + y);
   epd->e->map_data.pan.x = px + x;
   epd->e->map_data.pan.y = py + y;

   epd->current.x += x;
   epd->current.y += y;
   if (pos != 1.0) return EINA_TRUE;

   epd->anim = NULL;
   EFX_QUEUE_CHECK(epd);
   return EINA_TRUE;
}

static EFX *
_efx_pan_init(Evas_Object *obj)
{
   EFX *e;
   Efx_Pan_Data *epd;
   Smart_Data *sd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   if (!e->pan_data)
     {
        e->pan_data = calloc(1, sizeof(Efx_Pan_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->pan_data, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->pan_data);
        epd = e->pan_data;
        epd->pan = _smart_pan_add(evas_object_evas_get(obj));
        sd = evas_object_smart_data_get(epd->pan);
        sd->e = e;
        _smart_pan_child_set(epd->pan, obj);
        evas_object_show(epd->pan);
     }
   return e;
}

EAPI Eina_Bool
efx_pan_init(Evas_Object *obj)
{
   return !!_efx_pan_init(obj);
}

EAPI Eina_Bool
efx_pan(Evas_Object *obj, Efx_Effect_Speed speed, const Evas_Point *distance, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Pan_Data *epd;
   Evas_Coord x, y;
 
   if (!distance) return EINA_FALSE;
   if (total_time < 0.0) return EINA_FALSE;
   if (speed > EFX_EFFECT_SPEED_SINUSOIDAL) return EINA_FALSE;

   e = _efx_pan_init(obj);
   if (!e) return EINA_FALSE;
   epd = e->pan_data;
   epd->e = e;
   _smart_pan_get(epd->pan, &x, &y);
   INF("pan: %p - (%d,%d) += (%d,%d) over %gs: %s", obj, x, y, distance->x, distance->y, total_time, efx_speed_str[speed]);
   if (!total_time)
     {
        _smart_pan_set(epd->pan, x + distance->x, y + distance->y);
        return EINA_TRUE;
     }

   epd->speed = speed;
   epd->change.x = distance->x;
   epd->change.y = distance->y;
   epd->current.x = epd->current.y = 0;
   epd->cb = cb;
   epd->data = (void*)data;
   if (epd->anim) ecore_animator_del(epd->anim);
   epd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_pan_cb, epd);
   return EINA_TRUE;
}
