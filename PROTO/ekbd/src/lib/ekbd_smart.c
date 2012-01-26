#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Edje.h>
#include "Ekbd.h"
#include "ekbd_private.h"
#include "ekbd_layout.h"
#include "ekbd_send.h"

#define E_OBJECT_NAME "ekbd_object"
Evas_Smart *_smart = NULL;

static Ecore_Event_Handler *_handler;

static void _smart_init();
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static Eina_Bool _smart_update(void *data, int type, void *event);

EAPI int
ekbd_init(void)
{
   return 1;
}

EAPI int
ekbd_shutdown(void)
{
   return 0;
}

EAPI Evas_Object *
ekbd_object_add(Evas *e)
{
   _smart_init();
   return evas_object_smart_add(e, _smart);
}

EAPI void
ekbd_object_layout_add(Evas_Object *obj, const char *path)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   ekbd_layout_add(sd, eina_stringshare_add(path));
}

EAPI void
ekbd_object_layout_clear(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   ekbd_layout_free(sd);
   ekbd_layouts_free(sd);
}

EAPI const Eina_List *
ekbd_object_layout_get(const Evas_Object *obj)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;
   return sd->layouts;
}

EAPI void
ekbd_object_layout_select(Evas_Object *obj, Ekbd_Layout *layout)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   if ((!sd->layout.file) || (strcmp(layout->path, sd->layout.file)))
     {
        ekbd_layout_select(sd, layout);
        ekbd_layout_keys_calc(sd);
     }
}

EAPI const char *
ekbd_layout_path_get(const Ekbd_Layout *layout)
{
   if (layout && layout->path)
     return layout->path;
   return NULL;
}

EAPI const char *
ekbd_layout_icon_get(const Ekbd_Layout *layout)
{
   if (layout) return layout->icon;
   return NULL;
}

EAPI Ekbd_Layout_Type
ekbd_layout_type_get(const Ekbd_Layout *layout)
{
   if (layout) return layout->type;
   return EKBD_TYPE_UNKNOWN;
}

static void
_smart_init()
{
   if (!_smart)
     {
        static const Evas_Smart_Class sc =
          {
             E_OBJECT_NAME,
             EVAS_SMART_CLASS_VERSION,
             _smart_add,
             _smart_del,
             _smart_move,
             _smart_resize,
             _smart_show,
             _smart_hide,
             NULL,
             NULL,
             NULL,
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

static void
_smart_add(Evas_Object *obj)
{
   Smart_Data *sd;
   Ekbd_Layout *kil;
   Evas_Object *o;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   evas_object_smart_data_set(obj, sd);
   sd->s_obj = obj;

   /*
   o = ekbd_layout_theme_obj_new(evas_object_evas_get(obj), NULL, "ekbd/base/default");
   evas_object_smart_member_add(obj, sd->base_obj);
   sd->base_obj = o;
   */

   o = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_smart_member_add(o, obj);
   evas_object_move(o, -100001, -100001);
   evas_object_resize(o, 200002, 200002);
   evas_object_color_set(o, 255, 255, 255 ,255);
   evas_object_show(o);
   sd->layout_obj = o;

//   ekbd_layout_update(sd);

   _handler = ecore_event_handler_add(ECORE_X_EVENT_MAPPING_CHANGE,
                                     _smart_update, obj);
}

static void
_smart_del(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   ekbd_layout_free(sd);
   ekbd_layouts_free(sd);
   if (_handler)
     ecore_event_handler_del(_handler);
   evas_object_del(sd->layout_obj);
   free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;
   Ekbd_Int_Key *ky;
   Eina_List *l;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_event_freeze(evas_object_evas_get(sd->event_obj));
   sd->x = x;
   sd->y = y;
   ekbd_layout_keys_calc(sd);
   evas_event_thaw(evas_object_evas_get(sd->event_obj));
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;
   Ekbd_Layout *kil;
   Eina_Bool vertical;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   sd->w = w;
   sd->h = h;
   ekbd_layout_keys_calc(sd);
}

static void
_smart_show(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_show(sd->layout_obj);
}

static void
_smart_hide(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_hide(sd->layout_obj);
}

static Eina_Bool
_smart_update(void *data, int type, void *event)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(data);
   if (!sd) return ECORE_CALLBACK_RENEW;
   ekbd_send_update(sd);
   return ECORE_CALLBACK_RENEW;
}
