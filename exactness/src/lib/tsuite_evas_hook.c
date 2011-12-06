#define _GNU_SOURCE 1
#include <Elementary.h>
#include <Eet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include "tsuite_file_data.h"
#include "tsuite.h"


#define SHOT_KEY_STR "F2"
/* START - EET Handling code */
struct _Eet_Event_Type
{
   unsigned int version;
};
typedef struct _Eet_Event_Type Eet_Event_Type;
/* END   - EET Handling code */

static char *shot_key = SHOT_KEY_STR;
static Lists_st *vr_list = NULL;

void tsuite_evas_hook_init(Lists_st *v)
{  /* Pointer taken from tsuite.c */
   vr_list = v;
   shot_key = getenv("TSUITE_SHOT_KEY");
   if (!shot_key) shot_key = SHOT_KEY_STR;
}

void tsuite_evas_hook_reset(void)
{  /* tsuite.c informs us that vr_list is no longer valid */
   vr_list = NULL;
}

/* Adding variant to list, this list is later written to EET file */
#define ADD_TO_LIST(EVT_TYPE, EVT_STRUCT_NAME, INFO) \
   do { /* This macro will add event to EET data list */ \
        if (vr_list) \
          { \
             Variant_st *v =  malloc(sizeof(Variant_st)); \
             v->data = malloc(sizeof(EVT_STRUCT_NAME)); \
             _variant_type_set(tsuite_event_mapping_type_str_get(EVT_TYPE), \
                   &v->t, EINA_FALSE); \
             memcpy(v->data, &INFO, sizeof(EVT_STRUCT_NAME)); \
             printf("recording <%s> time=<%u>\n", #EVT_STRUCT_NAME, ((EVT_STRUCT_NAME *) v->data)->timestamp); \
             vr_list->variant_list = eina_list_append(vr_list->variant_list, v); \
          } \
   } while (0)

static Tsuite_Event_Type
tsuite_event_type_get(Evas_Callback_Type t)
{
   switch(t)
     {
      case EVAS_CALLBACK_MOUSE_IN:
         return TSUITE_EVENT_MOUSE_IN;
      case EVAS_CALLBACK_MOUSE_OUT:
         return TSUITE_EVENT_MOUSE_OUT;
      case EVAS_CALLBACK_MOUSE_DOWN:
         return TSUITE_EVENT_MOUSE_DOWN;
      case EVAS_CALLBACK_MOUSE_UP:
         return TSUITE_EVENT_MOUSE_UP;
      case EVAS_CALLBACK_MOUSE_MOVE:
         return TSUITE_EVENT_MOUSE_MOVE;
      case EVAS_CALLBACK_MOUSE_WHEEL:
         return TSUITE_EVENT_MOUSE_WHEEL;
      case EVAS_CALLBACK_MULTI_DOWN:
         return TSUITE_EVENT_MULTI_DOWN;
      case EVAS_CALLBACK_MULTI_UP:
         return TSUITE_EVENT_MULTI_UP;
      case EVAS_CALLBACK_MULTI_MOVE:
         return TSUITE_EVENT_MULTI_MOVE;
      case EVAS_CALLBACK_KEY_DOWN:
         return TSUITE_EVENT_KEY_DOWN;
      case EVAS_CALLBACK_KEY_UP:
         return TSUITE_EVENT_KEY_UP;

      default:
         return TSUITE_EVENT_NOT_SUPPORTED;
     }
}

EAPI void
evas_event_feed_mouse_in(Evas *e, unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   mouse_in_mouse_out t = { timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MOUSE_IN);
   ADD_TO_LIST(evt, mouse_in_mouse_out, t);
   void (*orig) (Evas *e, unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   orig(e, timestamp, data);
}

EAPI void
evas_event_feed_mouse_out(Evas *e, unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   mouse_in_mouse_out t = { timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MOUSE_OUT);
   ADD_TO_LIST(evt, mouse_in_mouse_out, t);
   void (*orig) (Evas *e, unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   orig(e, timestamp, data);
}

EAPI void
evas_event_feed_mouse_down(Evas *e, int b, Evas_Button_Flags flags,
      unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   mouse_down_mouse_up t = { b, flags, timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MOUSE_DOWN);
   ADD_TO_LIST(evt, mouse_down_mouse_up, t);
   void (*orig) (Evas *e, int b, Evas_Button_Flags flags,
         unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   orig(e, b, flags, timestamp, data);
}

EAPI void
evas_event_feed_mouse_up(Evas *e, int b, Evas_Button_Flags flags,
      unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   mouse_down_mouse_up t = { b, flags, timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MOUSE_UP);
   ADD_TO_LIST(evt, mouse_down_mouse_up, t);
   void (*orig) (Evas *e, int b, Evas_Button_Flags flags,
         unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   orig(e, b, flags, timestamp, data);
}

EAPI void
evas_event_feed_mouse_move(Evas *e, int x, int y, unsigned int timestamp,
      const void *data)
{
   mouse_move t = { x, y, timestamp };
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MOUSE_MOVE);
   ADD_TO_LIST(evt, mouse_move, t);
   void (*orig) (Evas *e, int x, int y, unsigned int timestamp,
         const void *data) = dlsym(RTLD_NEXT, __func__);
   orig(e, x, y, timestamp, data);
}

EAPI void
evas_event_feed_mouse_wheel(Evas *e, int direction, int z,
      unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   mouse_wheel t = { direction, z, timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MOUSE_WHEEL);
   ADD_TO_LIST(evt, mouse_wheel, t);
   void (*orig) (Evas *e, int direction, int z, unsigned int timestamp,
         const void *data) = dlsym(RTLD_NEXT, __func__);

   orig(e, direction, z, timestamp, data);
}

EAPI void
evas_event_feed_multi_down(Evas *e, int d, int x, int y,
      double rad, double radx, double rady, double pres, double ang,
      double fx, double fy, Evas_Button_Flags flags,
      unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   multi_event t = { d, x, y, rad, radx, rady, pres, ang,
        fx, fy, flags, timestamp };

   int evt = tsuite_event_type_get(EVAS_CALLBACK_MULTI_DOWN);
   ADD_TO_LIST(evt, multi_event, t);
   void (*orig) (Evas *e, int d, int x, int y, double rad,
         double radx, double rady, double pres, double ang,
         double fx, double fy, Evas_Button_Flags flags,
         unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);
   orig(e, d, x, y, rad, radx, rady, pres, ang, fx, fy, flags, timestamp, data);
}

EAPI void
evas_event_feed_multi_up(Evas *e, int d, int x, int y,
      double rad, double radx, double rady, double pres, double ang,
      double fx, double fy, Evas_Button_Flags flags,
      unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   multi_event t = { d, x, y, rad, radx, rady, pres, ang, fx, fy, flags, timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MULTI_UP);
   ADD_TO_LIST(evt, multi_event, t);
   void (*orig) (Evas *e, int d, int x, int y, double rad,
         double radx, double rady, double pres, double ang,
         double fx, double fy, Evas_Button_Flags flags,
         unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   orig(e, d, x, y, rad, radx, rady, pres, ang, fx, fy, flags, timestamp, data);
}

EAPI void
evas_event_feed_multi_move(Evas *e, int d, int x, int y, double rad,
      double radx, double rady, double pres, double ang,
      double fx, double fy, unsigned int timestamp, const void *data)
{
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   multi_move t = { d, x, y, rad, radx, rady, pres, ang, fx, fy, timestamp };
   int evt = tsuite_event_type_get(EVAS_CALLBACK_MULTI_MOVE);
   ADD_TO_LIST(evt, multi_move, t);
   void (*orig) (Evas *e, int d, int x, int y, double rad,
         double radx, double rady, double pres, double ang,
         double fx, double fy, unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   orig(e, d, x, y, rad, radx, rady, pres, ang, fx, fy, timestamp, data);
}

EAPI void
evas_event_feed_key_down(Evas *e, const char *keyname, const char *key,
      const char *string, const char *compose,
      unsigned int timestamp, const void *data)
{
   int evt = tsuite_event_type_get(EVAS_CALLBACK_KEY_DOWN);
   void (*orig) (Evas *e, const char *keyname, const char *key,
         const char *string, const char *compose,
         unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   if (!strcmp(key, shot_key))
     {
#ifdef DEBUG_TSUITE
        printf("Take Screenshot: %s timestamp=<%u>\n", __func__, timestamp);
#endif
        take_screenshot t = { timestamp };
        ADD_TO_LIST(TSUITE_EVENT_TAKE_SHOT, take_screenshot, t);

        orig(e, keyname, key, string, compose, timestamp, data);
        return;
     }

   /* Contruct t duplicate strings, free them when list if freed */
   key_down_key_up t;
   t.timestamp = timestamp;
   t.keyname = eina_stringshare_add(keyname);
   t.key = eina_stringshare_add(key);
   t.string = eina_stringshare_add(string);
   t.compose = eina_stringshare_add(compose);
   ADD_TO_LIST(evt, key_down_key_up, t);

   orig(e, keyname, key, string, compose, timestamp, data);
}

EAPI void
evas_event_feed_key_up(Evas *e, const char *keyname, const char *key,
      const char *string, const char *compose,
      unsigned int timestamp, const void *data)
{
   void (*orig) (Evas *e, const char *keyname, const char *key,
         const char *string, const char *compose,
         unsigned int timestamp, const void *data) =
      dlsym(RTLD_NEXT, __func__);

   if (!strcmp(key, shot_key))
     {
#ifdef DEBUG_TSUITE
        printf("Take Screenshot: %s timestamp=<%u>\n", __func__, timestamp);
#endif
        orig(e, keyname, key, string, compose, timestamp, data);
        return;  /* Take screenshot done on key-down */
     }

   int evt = { tsuite_event_type_get(EVAS_CALLBACK_KEY_UP) };
#ifdef DEBUG_TSUITE
   printf("Calling %s timestamp=<%u>\n", __func__, timestamp);
#endif
   /* Contruct t duplicate strings, free them when list if freed */
   key_down_key_up t;
   t.timestamp = timestamp;
   t.keyname = eina_stringshare_add(keyname);
   t.key = eina_stringshare_add(key);
   t.string = eina_stringshare_add(string);
   t.compose = eina_stringshare_add(compose);
   ADD_TO_LIST(evt, key_down_key_up, t);

   orig(e, keyname, key, string, compose, timestamp, data);
}
