#define _GNU_SOURCE 1
#include <Elementary.h>
#include <Eet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include "tsuite_file_data.h"

#define TSUITE_MAX_PATH 1024
#define SHOT_KEY_STR "F2"
#define IMAGE_FILENAME_EXT ".png"

/* START - EET Handling code */
struct _Eet_Event_Type
{
   unsigned int version;
};
typedef struct _Eet_Event_Type Eet_Event_Type;
/* END   - EET Handling code */

struct _evas_hook_setting
{
   char *recording;
   char *base_dir;
   char *dest_dir;
   char *test_name;
   char *file_name;
};
typedef struct _evas_hook_setting evas_hook_setting;

static char *shot_key = SHOT_KEY_STR;
static Lists_st *vr_list = NULL;
static evas_hook_setting *_hook_setting = NULL;
static Tsuite_Data ts;

/**
 * @internal
 *
 * This function initiates Tsuite_Data
 * @param name defines test-name
 * @param Pointer_Event Pointer to PE.
 *
 * @ingroup Tsuite
 */

static unsigned int
evt_time_get(unsigned int tm, Variant_st *v)
{
   switch(tsuite_event_mapping_type_get(v->t.type))
     {
      case TSUITE_EVENT_MOUSE_IN:
           {
              mouse_in_mouse_out *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MOUSE_OUT:
           {
              mouse_in_mouse_out *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MOUSE_DOWN:
           {
              mouse_down_mouse_up *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MOUSE_UP:
           {
              mouse_down_mouse_up *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MOUSE_MOVE:
           {
              mouse_move *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MOUSE_WHEEL:
           {
              mouse_wheel *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MULTI_DOWN:
           {
              multi_event *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MULTI_UP:
           {
              multi_event *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_MULTI_MOVE:
           {
              multi_move *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_KEY_DOWN:
           {
              key_down_key_up *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_KEY_UP:
           {
              key_down_key_up *t = v->data;
              return t->timestamp;
           }
      case TSUITE_EVENT_TAKE_SHOT:
           {
              take_screenshot *t = v->data;
              return t->timestamp;
           }
      default: /* All non-input events are not handeled */
         return tm;
         break;
     }
}

EAPI void
tsuite_evas_hook_init(void)
{  /* Pointer taken from tsuite.c */
   shot_key = getenv("TSUITE_SHOT_KEY");
   if (!shot_key) shot_key = SHOT_KEY_STR;

   if (!vr_list)
     vr_list = calloc(1, sizeof(*vr_list));
}

EAPI void
tsuite_evas_hook_reset(void)
{  /* tsuite.c informs us that vr_list is no longer valid */
   if (vr_list)
     vr_list = free_events(vr_list, _hook_setting->recording);
}

/**
 * @internal
 *
 * This function takes actual shot and saves it in PNG
 * @param data Tsuite_Data pointer initiated by user
 * @param obj  Window pointer
 * @param obj  name file name. Will use name_+serial if NULL
 *
 * @ingroup Tsuite
 */
void
tsuite_shot_do(char *name)
{
   if (!ts.e)
     return;

   Ecore_Evas *ee, *ee_orig;
   Evas_Object *o;
   unsigned int *pixels;
   int w, h,dir_name_len = 0;
   char *filename;
   if (_hook_setting->dest_dir)
     dir_name_len = strlen(_hook_setting->dest_dir) + 1; /* includes space of a '/' */

   if (name)
     {
        filename = malloc(strlen(name) + strlen(IMAGE_FILENAME_EXT) +
              dir_name_len + 4);

        if (_hook_setting->dest_dir)
          sprintf(filename, "%s/", _hook_setting->dest_dir);

        sprintf(filename + dir_name_len, "%s%s", name, IMAGE_FILENAME_EXT);
     }
   else
     {
        filename = malloc(strlen(_hook_setting->test_name) + strlen(IMAGE_FILENAME_EXT) +
              dir_name_len + 8); /* also space for serial */

        ts.serial++;
        if (_hook_setting->dest_dir)
          sprintf(filename, "%s/", _hook_setting->dest_dir);

        sprintf(filename + dir_name_len, "%s_%d%s", _hook_setting->test_name,
              ts.serial, IMAGE_FILENAME_EXT);
     }

   /* A bit hackish, get the ecore_evas from the Evas canvas */
   ee_orig = evas_data_attach_get(ts.e);
   printf("<%s> ts.e=<%p> ee_orig=<%p> file=<%s>\n", __func__, ts.e, ee_orig, filename);

   ecore_evas_manual_render(ee_orig);
   pixels = (void *)ecore_evas_buffer_pixels_get(ee_orig);
   if (!pixels) return;
   ecore_evas_geometry_get(ee_orig, NULL, NULL, &w, &h);
   if ((w < 1) || (h < 1)) return;
   ee = ecore_evas_buffer_new(1, 1);
   o = evas_object_image_add(ecore_evas_get(ee));
   evas_object_image_alpha_set(o, ecore_evas_alpha_get(ee_orig));
   evas_object_image_size_set(o, w, h);
   evas_object_image_data_set(o, pixels);

   if (!evas_object_image_save(o, filename, NULL, NULL))
     {
        printf("Cannot save widget to <%s>\n", filename);
     }
   ecore_evas_free(ee);
   free(filename);
}

EAPI int
ecore_init(void)
{
   int (*_ecore_init)(void) =
      dlsym(RTLD_NEXT, "ecore_init");

   printf("<%s> Calling %s\n", __FILE__, __func__);
   if (!_hook_setting)
     {
        _hook_setting = calloc(1, sizeof(evas_hook_setting));
        _hook_setting->recording = getenv("TSUITE_RECORDING");
        _hook_setting->base_dir = getenv("TSUITE_BASE_DIR");
        _hook_setting->dest_dir = getenv("TSUITE_DEST_DIR");
        _hook_setting->test_name = getenv("TSUITE_TEST_NAME");
        _hook_setting->file_name = getenv("TSUITE_FILE_NAME");

        printf("<%s> test_name=<%s>\n", __func__, _hook_setting->test_name);
        printf("<%s> base_dir=<%s>\n", __func__, _hook_setting->base_dir);
        printf("<%s> dest_dir=<%s>\n", __func__, _hook_setting->dest_dir);
        printf("<%s> recording=<%s>\n", __func__, _hook_setting->recording);
        printf("<%s> rec file is <%s>\n", __func__, _hook_setting->file_name);

        if (_hook_setting->recording)
          tsuite_evas_hook_init();

        printf("<%s> TSUITE_INI! %s\n", __FILE__, __func__);
     }

   return _ecore_init();
}

EAPI int
ecore_shutdown(void)
{
   int (*_ecore_shutdown)(void) =
      dlsym(RTLD_NEXT, "ecore_shutdown");

   if (_hook_setting)
     {
        if (vr_list && _hook_setting->recording)
          write_events(_hook_setting->file_name, vr_list);

        tsuite_evas_hook_reset();

        free(_hook_setting);
        _hook_setting = NULL;
     }

   if (ts.td)
     free(ts.td);

   memset(&ts, 0, sizeof(Tsuite_Data));
   return _ecore_shutdown();
}

EAPI Evas_Object *
elm_win_add(Evas_Object *parent, const char *name, Elm_Win_Type type)
{
   Evas_Object *win;
   Evas_Object * (* _elm_win_add) (Evas_Object *, const char *, Elm_Win_Type) =
      dlsym(RTLD_NEXT, "elm_win_add");

   win = _elm_win_add(parent, name, type);
   ts.e = evas_object_evas_get(win);
   return win;
}

/* We don't use this in the meantime because a new evas
 * is allocated at tsuite_shot_do, this changes ts.e
EAPI Evas *
evas_new(void)
{
   Evas * (*_evas_new)(void) =
      dlsym(RTLD_NEXT, "evas_new");

   ts.e = _evas_new();
   return ts.e;
}
*/

static Eina_Bool
tsuite_feed_event(void *data)
{
   static Ecore_Timer *tmr = NULL;
   Timer_Data *td = data;
   time_t evt_time;
   if (!td)
     return ECORE_CALLBACK_CANCEL;

   Variant_st *v = eina_list_data_get(td->current_event);
   switch(tsuite_event_mapping_type_get(v->t.type))
     {
      case TSUITE_EVENT_MOUSE_IN:
           {
              mouse_in_mouse_out *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_mouse_in timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_mouse_in(td->e, time(NULL), NULL);
              break;
           }
      case TSUITE_EVENT_MOUSE_OUT:
           {
              mouse_in_mouse_out *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_mouse_out timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_mouse_out(td->e, time(NULL), NULL);
              break;
           }
      case TSUITE_EVENT_MOUSE_DOWN:
           {
              mouse_down_mouse_up *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_mouse_down timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_mouse_down(td->e, t->b, t->flags, time(NULL),
                    NULL);

              break;
           }
      case TSUITE_EVENT_MOUSE_UP:
           {
              mouse_down_mouse_up *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_mouse_up timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_mouse_up(td->e, t->b, t->flags, time(NULL),
                    NULL);

              break;
           }
      case TSUITE_EVENT_MOUSE_MOVE:
           {
              mouse_move *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_mouse_move (x,y)=(%d,%d) timestamp=<%u>\n", __func__, t->x, t->y, t->timestamp);
#endif
              evas_event_feed_mouse_move(td->e, t->x, t->y, time(NULL), NULL);
              break;
           }
      case TSUITE_EVENT_MOUSE_WHEEL:
           {
              mouse_wheel *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_mouse_wheel timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_mouse_wheel(td->e, t->direction, t->z,
                    time(NULL), NULL);

              break;
           }
      case TSUITE_EVENT_MULTI_DOWN:
           {
              multi_event *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_multi_down timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_multi_down(td->e, t->d, t->x, t->y, t->rad,
                    t->radx, t->rady, t->pres, t->ang, t->fx, t->fy,
                    t->flags, time(NULL), NULL);

              break;
           }
      case TSUITE_EVENT_MULTI_UP:
           {
              multi_event *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_multi_up timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_multi_up(td->e, t->d, t->x, t->y, t->rad,
                    t->radx, t->rady, t->pres, t->ang, t->fx, t->fy,
                    t->flags, time(NULL), NULL);

              break;
           }
      case TSUITE_EVENT_MULTI_MOVE:
           {
              multi_move *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_multi_move timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_multi_move(td->e, t->d, t->x, t->y, t->rad,
                    t->radx, t->rady, t->pres, t->ang, t->fx, t->fy,
                    time(NULL), NULL);

              break;
           }
      case TSUITE_EVENT_KEY_DOWN:
           {
              key_down_key_up *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_key_down timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_key_down(td->e, t->keyname, t->key, t->string,
                    t->compose, time(NULL), NULL);

              break;
           }
      case TSUITE_EVENT_KEY_UP:
           {
              key_down_key_up *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s evas_event_feed_key_up timestamp=<%u>\n", __func__, t->timestamp);
#endif
              evas_event_feed_key_up(td->e, t->keyname, t->key, t->string,
                    t->compose, time(NULL), NULL);

              break;
           }
      case TSUITE_EVENT_TAKE_SHOT:
           {
              take_screenshot *t = v->data;
              evt_time = t->timestamp;
#ifdef DEBUG_TSUITE
              printf("%s take shot  timestamp=<%u>\n", __func__, t->timestamp);
#endif
              tsuite_shot_do(NULL); /* Serial name based on test-name */
              break;
           }
      default: /* All non-input events are not handeled */
         evt_time = td->recent_event_time;
         break;
     }

   double timer_time;
   td->current_event = eina_list_next(td->current_event);

   if (!td->current_event)
     {  /* Finished reading all events */
        elm_exit();
        return ECORE_CALLBACK_CANCEL;
     }

   td->recent_event_time = evt_time;

   unsigned int current_event_time = evt_time_get(evt_time, eina_list_data_get(td->current_event));

   if (current_event_time < td->recent_event_time) /* Could happen with refeed event */
     current_event_time = td->recent_event_time;

#ifdef DEBUG_TSUITE
   printf("%s td->recent_event_time=<%u> current_event_time=<%u>\n", __func__, td->recent_event_time, current_event_time);
#endif
   timer_time = (current_event_time - td->recent_event_time) / 1000.0;

   if (!td->recent_event_time)
     timer_time = 0.0;

#ifdef DEBUG_TSUITE
   printf("%s timer_time=<%f>\n", __func__, timer_time);
#endif
   tmr = ecore_timer_add(timer_time, tsuite_feed_event, td);

   return ECORE_CALLBACK_CANCEL;
}

void
ecore_main_loop_begin(void)
{
   void (*_ecore_main_loop_begin)(void) =
      dlsym(RTLD_NEXT, "ecore_main_loop_begin");


   if (!_hook_setting->recording && _hook_setting->file_name)
     {
        ts.td = calloc(1, sizeof(Timer_Data));
#ifdef DEBUG_TSUITE
        printf("<%s> rec file is <%s>\n", _hook_setting->file_name);
#endif
        vr_list = read_events(_hook_setting->file_name, ts.e, ts.td);
        if (ts.td->current_event)
          {  /* Got first event in list, run test */
             tsuite_feed_event(ts.td);
          }
     }

   printf("<%s> Calling %s\n", __FILE__, __func__);
   return _ecore_main_loop_begin();
}

/* Adding variant to list, this list is later written to EET file */
#define ADD_TO_LIST(EVT_TYPE, EVT_STRUCT_NAME, INFO) \
   do { /* This macro will add event to EET data list */ \
        if (vr_list && _hook_setting->recording) \
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


   if (vr_list && _hook_setting->recording)
     {  /* Construct duplicate strings, free them when list if freed */
        key_down_key_up t;
        t.timestamp = timestamp;
        t.keyname = eina_stringshare_add(keyname);
        t.key = eina_stringshare_add(key);
        t.string = eina_stringshare_add(string);
        t.compose = eina_stringshare_add(compose);
        ADD_TO_LIST(evt, key_down_key_up, t);
     }

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

   if (vr_list && _hook_setting->recording)
     {  /* Construct duplicate strings, free them when list if freed */
        key_down_key_up t;
        t.timestamp = timestamp;
        t.keyname = eina_stringshare_add(keyname);
        t.key = eina_stringshare_add(key);
        t.string = eina_stringshare_add(string);
        t.compose = eina_stringshare_add(compose);
        ADD_TO_LIST(evt, key_down_key_up, t);
     }

   orig(e, keyname, key, string, compose, timestamp, data);
}
